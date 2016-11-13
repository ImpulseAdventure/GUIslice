// =======================================================================
// microSDL library
// - Calvin Hass
// - http://www.impulseadventure.com/elec/microsdl-sdl-gui.html
//
// - Version 0.5    (2016/11/12)
// =======================================================================

// MicroSDL library
#include "microsdl.h"
#include "microsdl_ex.h"

#include <stdio.h>

#include "SDL/SDL.h"
#include "SDL/SDL_getenv.h"
#include "SDL/SDL_ttf.h"


// Includes for tslib
#ifdef INC_TS
#include "tslib.h"
#endif

// Optionally enable SDL clean start VT workaround
#ifdef VT_WRK_EN
#include <fcntl.h>      // For O_RDONLY
#include <errno.h>      // For errno
#include <unistd.h>     // For close()
#include <sys/ioctl.h>  // For ioctl()
#include <sys/kd.h>     // for KDSETMODE
#include <sys/vt.h>     // for VT_UNLOCKSWITCH
#define VT_WRK_TTY      "/dev/tty0"
#endif


// Version definition
#define MICROSDL_VER "0.5"

// Debug flags
//#define DBG_LOG     // Enable debugging log output
//#define DBG_TOUCH   // Enable debugging of touch-presses



// ========================================================================

// ------------------------------------------------------------------------
// General Functions
// ------------------------------------------------------------------------

// Need to configure a number of SDL and TS environment
// variables. The following demonstrates a way to do this
// programmatically, but it can also be done via export
// commands within the shell (or init script).
// eg. export TSLIB_FBDEVICE=/dev/fb1
void microSDL_InitEnv(microSDL_tsGui* pGui)
{
  // This line already appears to be set in env
  putenv((char*)"FRAMEBUFFER=/dev/fb1");
  // The following is the only required entra line
  putenv((char*)"SDL_FBDEV=/dev/fb1");

  // The following lines don't appear to be required
  putenv((char*)"SDL_VIDEODRIVER=fbcon");
  putenv((char*)"SDL_FBDEV=/dev/fb1");

  // Disable these lines as it appears to cause
  // conflict in the SDL mouse coordinate reporting
  // (perhaps b/c it is mixing real mouse and touch?)
  //putenv((char*)"SDL_MOUSEDRV=TSLIB");
  //putenv((char*)"SDL_MOUSEDEV=/dev/input/touchscreen");

  #ifdef INC_TS
  putenv((char*)"TSLIB_FBDEVICE=/dev/fb1");
  putenv((char*)"TSLIB_TSDEVICE=/dev/input/touchscreen");

  putenv((char*)"TSLIB_CALIBFILE=/etc/pointercal");
  putenv((char*)"TSLIB_CONFFILE=/etc/ts.conf");
  putenv((char*)"TSLIB_PLUGINDIR=/usr/local/lib/ts");
  #endif

}



bool microSDL_Init(microSDL_tsGui* pGui,microSDL_tsPage* asPage,unsigned nMaxPage,microSDL_tsFont* psFont,unsigned nMaxFont,microSDL_tsView* psView,unsigned nMaxView)
{
  unsigned  nInd;
  
  // Initialize state
  pGui->nPageMax        = nMaxPage;
  pGui->nPageCnt        = 0;
  pGui->asPage          = asPage;

  pGui->pCurPage        = NULL;
  pGui->pCurPageCollect = NULL;

  // Initialize collection of fonts with user-supplied pointer
  pGui->psFont      = psFont;
  pGui->nFontMax    = nMaxFont;
  pGui->nFontCnt    = 0;
  for (nInd=0;nInd<(pGui->nFontMax);nInd++) {
    microSDL_ResetFont(&(pGui->psFont[nInd]));
  }
 
  // Initialize temporary element
  microSDL_ResetElem(&(pGui->sElemTmp));

  // Initialize collection of views with user-supplied pointer
  pGui->psView      = psView;
  pGui->nViewMax    = nMaxView;
  pGui->nViewCnt    = 0;
  pGui->nViewIndCur = MSDL_VIEW_IND_SCREEN;
  for (nInd=0;nInd<(pGui->nViewMax);nInd++) {
    microSDL_ResetView(&(pGui->psView[nInd]));
  }
  
  // Last touch event
  pGui->nTouchLastX           = 0;
  pGui->nTouchLastY           = 0;
  pGui->nTouchLastPress       = 0;

  // Touchscreen library interface
  #ifdef INC_TS
  pGui->ts = NULL;
  #endif
  
  // Primary surface definitions
  pGui->surfScreen = NULL;
  pGui->surfBkgnd = NULL;

  
#ifdef VT_WRK_EN
  // Force a clean start to SDL to workaround any bad state
  // left behind by a previous SDL application's failure to
  // clean up.
  // TODO: Allow compiler option to skip this workaround
  // TODO: Allow determination of the TTY to use
  microSDL_CleanStart(VT_WRK_TTY);
#endif  

  // Setup SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    fprintf(stderr,"ERROR: Unable to init SDL: %s\n",SDL_GetError());
    return false;
  }
  // Now that we have successfully initialized SDL
  // we need to register an exit handler so that SDL_Quit()
  // gets called at program termination. If we don't do this
  // then some types of errors/aborts will result in
  // the SDL environment being left in a bad state that
  // affects the next SDL program execution.
  atexit(SDL_Quit);

  // Set up pGui->surfScreen
  const SDL_VideoInfo*  videoInfo = SDL_GetVideoInfo();
  int                   nSystemX = videoInfo->current_w;
  int                   nSystemY = videoInfo->current_h;
  Uint8                 nBpp = videoInfo->vfmt->BitsPerPixel ;

  // SDL_SWSURFACE is apparently more reliable
  pGui->surfScreen = SDL_SetVideoMode(nSystemX,nSystemY,nBpp,SDL_SWSURFACE);
  if (!pGui->surfScreen) {
    fprintf(stderr,"ERROR: SDL_SetVideoMode() failed: %s\n",SDL_GetError());
    return false;
  }

  // Initialize font engine
  if (TTF_Init() == -1) {
    fprintf(stderr,"ERROR: TTF_Init() failed\n");
    return false;
  }

  // Since the mouse cursor is based on SDL coords which are badly
  // scaled when in touch mode, we will disable the mouse pointer.
  // Note that the SDL coords seem to be OK when in actual mouse mode.
  SDL_ShowCursor(SDL_DISABLE);

  return true;
}


void microSDL_Quit(microSDL_tsGui* pGui)
{
  // Close all elements and fonts
  microSDL_GuiDestruct(pGui);

  // Close down SDL
  SDL_Quit();
}

// Main polling loop for microSDL
void microSDL_Update(microSDL_tsGui* pGui)
{
  int       nTouchX,nTouchY;
  unsigned  nTouchPress;
  bool      bTouchEvent;
  
  // Poll for touchscreen presses
  // - Conditional compiling for tslib or SDL
  #ifdef INC_TS  
  bTouchEvent = microSDL_GetTsTouch(pGui,&nTouchX,&nTouchY,&nTouchPress);
  #else
  bTouchEvent = microSDL_GetSdlTouch(pGui,&nTouchX,&nTouchY,&nTouchPress);
  #endif
  
  if (bTouchEvent) {
    // Track and handle the touch events
    // - Handle the events on the current page
    microSDL_TrackTouch(pGui,pGui->pCurPageCollect,nTouchX,nTouchY,nTouchPress);
  }
  
  // Issue a timer tick to all pages
  int nPage;
  microSDL_tsCollect* pCollect = NULL;
  for (nPage=0;nPage<pGui->nPageCnt;nPage++) {
    pCollect = &pGui->asPage[nPage].sCollect;
    microSDL_CollectTick(pGui,pCollect);
  }
  
  // Perform any redraw required for current page
  microSDL_PageRedrawGo(pGui);
}

// ------------------------------------------------------------------------
// Graphics General Functions
// ------------------------------------------------------------------------

SDL_Surface* microSDL_LoadBmp(microSDL_tsGui* pGui,char* pStrFname)
{
  //The image that's loaded
  SDL_Surface* surfLoaded = NULL;

  //The optimized surface that will be used
  SDL_Surface* surfOptimized = NULL;

  //Load the image
  surfLoaded = SDL_LoadBMP(pStrFname);

  // Confirm that the image loaded correctly
  if (surfLoaded == NULL) {
    fprintf(stderr,"ERROR: SDL_LoadBMP(%s) failed: %s\n",pStrFname,SDL_GetError());
    return NULL;
  } else {
    //Create an optimized surface
    surfOptimized = SDL_DisplayFormat( surfLoaded );

    //Free the old surface
    SDL_FreeSurface( surfLoaded );

    //If the surface was optimized
    if( surfOptimized != NULL ) {

      // Support optional transparency
      if (MSDL_BMP_TRANS_EN) {
        // Color key surface
        // - Use transparency color key defined in BMP_TRANS_RGB
        SDL_SetColorKey( surfOptimized, SDL_SRCCOLORKEY,
          SDL_MapRGB( surfOptimized->format, MSDL_BMP_TRANS_RGB ) );
      } // MSDL_BMP_TRANS_EN
    }
  }

  //Return the optimized surface
  return surfOptimized;
}


bool microSDL_SetBkgnd(microSDL_tsGui* pGui,SDL_Surface* pSurf)
{
  if (!pSurf) {
    // TODO: Error handling
    return false;
  }
  if (pGui->surfBkgnd != NULL ) {
    // Dispose of previous background
    SDL_FreeSurface(pGui->surfBkgnd);
    pGui->surfBkgnd = NULL;
  }
  pGui->surfBkgnd = pSurf;
  microSDL_PageFlipSet(pGui,true);  
  return true;
}

bool microSDL_SetBkgndImage(microSDL_tsGui* pGui,char* pStrFname)
{
  if (strlen(pStrFname)==0) {
    // TODO: Error handling
    return false;
  }
  if (pGui->surfBkgnd != NULL ) {
    // Dispose of previous background
    SDL_FreeSurface(pGui->surfBkgnd);
    pGui->surfBkgnd = NULL;
  }
  pGui->surfBkgnd = microSDL_LoadBmp(pGui,pStrFname);
  if (pGui->surfBkgnd == NULL) {
    return false;
  }
  microSDL_PageFlipSet(pGui,true);
  return true;
}

bool microSDL_SetBkgndColor(microSDL_tsGui* pGui,SDL_Color nCol)
{
  if (pGui->surfBkgnd != NULL ) {
    // Dispose of previous background
    SDL_FreeSurface(pGui->surfBkgnd);
    pGui->surfBkgnd = NULL;
  }
  unsigned nScreenW = pGui->surfScreen->w;
  unsigned nScreenH = pGui->surfScreen->h;
  unsigned nBpp = pGui->surfScreen->format->BytesPerPixel * 8;
  pGui->surfBkgnd = SDL_CreateRGBSurface(SDL_SWSURFACE,
    nScreenW,nScreenH,nBpp,0,0,0,0xFF);
  SDL_FillRect(pGui->surfBkgnd,NULL,
    SDL_MapRGB(pGui->surfBkgnd->format,nCol.r,nCol.g,nCol.b));

  if (pGui->surfBkgnd == NULL) {
    return false;
  }
  microSDL_PageFlipSet(pGui,true);
  return true;
}


void microSDL_ApplySurface(microSDL_tsGui* pGui,int x, int y, SDL_Surface* pSrc, SDL_Surface* pDest)
{
  SDL_Rect offset;
  offset.x = x;
  offset.y = y;
  SDL_BlitSurface(pSrc,NULL,pDest,&offset);
  microSDL_PageFlipSet(pGui,true);
}


bool microSDL_IsInRect(int nSelX,int nSelY,SDL_Rect rRect)
{
  if ( (nSelX >= rRect.x) && (nSelX <= rRect.x+rRect.w) && 
     (nSelY >= rRect.y) && (nSelY <= rRect.y+rRect.h) ) {
    return true;
  } else {
    return false;
  }
}

bool microSDL_IsInWH(microSDL_tsGui* pGui,int nSelX,int nSelY,Uint16 nWidth,Uint16 nHeight)
{
  if ( (nSelX >= 0) && (nSelX <= nWidth-1) && 
     (nSelY >= 0) && (nSelY <= nHeight-1) ) {
    return true;
  } else {
    return false;
  }
}



// ------------------------------------------------------------------------
// Graphics Primitive Functions
// ------------------------------------------------------------------------

// bMapEn specifies whether viewport remapping is enabled or not
// - This should be disabled if the caller has already performed the
//   the remap (eg. in Line() function).
void microSDL_SetPixel(microSDL_tsGui* pGui,Sint16 nX,Sint16 nY,SDL_Color nCol,bool bMapEn)
{
  // Support viewport local coordinate remapping
  if (bMapEn) {
    if (pGui->nViewIndCur != MSDL_VIEW_IND_SCREEN) {
      microSDL_ViewRemapPt(pGui,&nX,&nY);
    }
  }

  if (microSDL_Lock(pGui)) {
    microSDL_PutPixelRaw(pGui,pGui->surfScreen,nX,nY,microSDL_GenPixelColor(pGui,nCol));
    microSDL_Unlock(pGui);
  }   // microSDL_Lock
  microSDL_PageFlipSet(pGui,true);
}

// Draw an arbitrary line using Bresenham's algorithm
// - Algorithm reference: https://rosettacode.org/wiki/Bitmap/Bresenham's_line_algorithm#C
void microSDL_Line(microSDL_tsGui* pGui,Sint16 nX0,Sint16 nY0,Sint16 nX1,Sint16 nY1,SDL_Color nCol)
{
  // Support viewport local coordinate remapping
  if (pGui->nViewIndCur != MSDL_VIEW_IND_SCREEN) {
    microSDL_ViewRemapPt(pGui,&nX0,&nY0);
    microSDL_ViewRemapPt(pGui,&nX1,&nY1);
  }

  Sint16 nDX = abs(nX1-nX0);
  Sint16 nDY = abs(nY1-nY0);

  Sint16 nSX = (nX0 < nX1)? 1 : -1;
  Sint16 nSY = (nY0 < nY1)? 1 : -1;
  Sint16 nErr = ( (nDX>nDY)? nDX : -nDY )/2;
  Sint16 nE2;

  // Check for degenerate cases
  // TODO: Need to test these optimizations
/*
  if (nDX == 0) {
    if (nDY == 0) {
      return;
    } else if (nDY >= 0) {
      microSDL_LineV(pGui,nX0,nY0,nDY,nCol);
    } else {
      microSDL_LineV(pGui,nX1,nY1,-nDY,nCol);
    }
  } else if (nDY == 0) {
    if (nDX >= 0) {
      microSDL_LineH(pGui,nX0,nY0,nDX,nCol);
    } else {
      microSDL_LineH(pGui,nX1,nY1,-nDX,nCol);
    }
  }
*/

  for (;;) {
    // Set the pixel but disable viewport remapping since
    // we have already taken care of it here
    microSDL_SetPixel(pGui,nX0,nY0,nCol,false);
    
    // Calculate next coordinates
    if ( (nX0 == nX1) && (nY0 == nY1) ) break;
    nE2 = nErr;
    if (nE2 > -nDX) { nErr -= nDY; nX0 += nSX; }
    if (nE2 <  nDY) { nErr += nDX; nY0 += nSY; }
  }
  microSDL_PageFlipSet(pGui,true);  
}

void microSDL_LineH(microSDL_tsGui* pGui,Sint16 nX, Sint16 nY, Uint16 nW,SDL_Color nCol)
{
  // Support viewport local coordinate remapping
  if (pGui->nViewIndCur != MSDL_VIEW_IND_SCREEN) {
    microSDL_ViewRemapPt(pGui,&nX,&nY);
  }

  Uint16 nOffset;
  Uint32 nPixelCol = microSDL_GenPixelColor(pGui,nCol);
  if (microSDL_Lock(pGui)) {
    for (nOffset=0;nOffset<nW;nOffset++) {
      microSDL_PutPixelRaw(pGui,pGui->surfScreen,nX+nOffset,nY,nPixelCol);    
    }
    microSDL_Unlock(pGui);
  }   // microSDL_Lock
  microSDL_PageFlipSet(pGui,true);  
}

void microSDL_LineV(microSDL_tsGui* pGui,Sint16 nX, Sint16 nY, Uint16 nH,SDL_Color nCol)
{
  // Support viewport local coordinate remapping
  if (pGui->nViewIndCur != MSDL_VIEW_IND_SCREEN) {
    microSDL_ViewRemapPt(pGui,&nX,&nY);
  }
  Uint16 nOffset;
  Uint32 nPixelCol = microSDL_GenPixelColor(pGui,nCol);
  if (microSDL_Lock(pGui)) {
    for (nOffset=0;nOffset<nH;nOffset++) {
      microSDL_PutPixelRaw(pGui,pGui->surfScreen,nX,nY+nOffset,nPixelCol);    
    }
    microSDL_Unlock(pGui);
  }   // microSDL_Lock
  microSDL_PageFlipSet(pGui,true);
}



// Ensure the coordinates are increasing from nX0->nX1 and nY0->nY1
// NOTE: UNUSED
void microSDL_OrderCoord(Sint16* pnX0,Sint16* pnY0,Sint16* pnX1,Sint16* pnY1)
{
  Sint16  nTmp;
  if ((*pnX1) < (*pnX0)) {
    nTmp = (*pnX0);
    (*pnX0) = (*pnX1);
    (*pnX1) = nTmp;
  }
  if ((*pnY1) < (*pnY0)) {
    nTmp = (*pnY0);
    (*pnY0) = (*pnY1);
    (*pnY1) = nTmp;
  }
}


void microSDL_FrameRect(microSDL_tsGui* pGui,SDL_Rect rRect,SDL_Color nCol)
{
  // Ensure dimensions are valid
  if ((rRect.w == 0) || (rRect.h == 0)) {
    return;
  }

  // Support viewport local coordinate remapping
  if (pGui->nViewIndCur != MSDL_VIEW_IND_SCREEN) {
    microSDL_ViewRemapRect(pGui,&rRect);
  }

  Sint16  nX,nY;
  Uint16  nH,nW;
  nX = rRect.x;
  nY = rRect.y;
  nW = rRect.w;
  nH = rRect.h;
  microSDL_LineH(pGui,nX,nY,nW-1,nCol);                 // Top
  microSDL_LineH(pGui,nX,(Sint16)(nY+nH-1),nW-1,nCol);  // Bottom
  microSDL_LineV(pGui,nX,nY,nH-1,nCol);                 // Left
  microSDL_LineV(pGui,(Sint16)(nX+nW-1),nY,nH-1,nCol);  // Right
  microSDL_PageFlipSet(pGui,true);  
}

void microSDL_FillRect(microSDL_tsGui* pGui,SDL_Rect rRect,SDL_Color nCol)
{
  // Ensure dimensions are valid
  if ((rRect.w == 0) || (rRect.h == 0)) {
    return;
  }

  // Support viewport local coordinate remapping
  if (pGui->nViewIndCur != MSDL_VIEW_IND_SCREEN) {
    microSDL_ViewRemapRect(pGui,&rRect);
  }

  SDL_FillRect(pGui->surfScreen,&rRect,
    SDL_MapRGB(pGui->surfScreen->format,nCol.r,nCol.g,nCol.b));
  
  microSDL_PageFlipSet(pGui,true);  
}


// Expand or contract a rectangle in width and/or height (equal
// amounts on both side), based on the centerpoint of the rectangle.
SDL_Rect microSDL_ExpandRect(SDL_Rect rRect,Sint16 nExpandW,Sint16 nExpandH)
{
  SDL_Rect  rNew = {0,0,0,0};

  // Detect error case of contracting region too far
  if (rRect.w + (2*nExpandW) < 0) {
    fprintf(stderr,"ERROR: ExpandRect(%d,%d) contracts too far",nExpandW,nExpandH);
    return rNew;
  }
  if (rRect.w + (2*nExpandW) < 0) {
    fprintf(stderr,"ERROR: ExpandRect(%d,%d) contracts too far",nExpandW,nExpandH);
    return rNew;
  }

  // Adjust the new width/height
  // Note that the overall width/height changes by a factor of
  // two since we are applying the adjustment on both sides (ie.
  // top/bottom or left/right) equally.
  rNew.w = rRect.w + (2*nExpandW);
  rNew.h = rRect.h + (2*nExpandH);

  // Adjust the rectangle coordinate to allow for new dimensions
  // Note that this moves the coordinate in the opposite
  // direction of the expansion/contraction.
  rNew.x = rRect.x - nExpandW;
  rNew.y = rRect.y - nExpandH;

  return rNew;
}


// Draw a circle using midpoint circle algorithm
// - Algorithm reference: https://en.wikipedia.org/wiki/Midpoint_circle_algorithm
void microSDL_FrameCircle(microSDL_tsGui* pGui,Sint16 nMidX,Sint16 nMidY,
  Uint16 nRadius,SDL_Color nCol)
{
  // Support viewport local coordinate remapping
  if (pGui->nViewIndCur != MSDL_VIEW_IND_SCREEN) {
    microSDL_ViewRemapPt(pGui,&nMidX,&nMidY);
  }
  Uint32 nPixelCol = microSDL_GenPixelColor(pGui,nCol);
  
  int nX    = nRadius;
  int nY    = 0;
  int nErr  = 0;
  
  if (microSDL_Lock(pGui)) {
    while (nX >= nY)
    {
      microSDL_PutPixelRaw(pGui,pGui->surfScreen,nMidX + nX, nMidY + nY,nPixelCol);
      microSDL_PutPixelRaw(pGui,pGui->surfScreen,nMidX + nY, nMidY + nX,nPixelCol);
      microSDL_PutPixelRaw(pGui,pGui->surfScreen,nMidX - nY, nMidY + nX,nPixelCol);
      microSDL_PutPixelRaw(pGui,pGui->surfScreen,nMidX - nX, nMidY + nY,nPixelCol);
      microSDL_PutPixelRaw(pGui,pGui->surfScreen,nMidX - nX, nMidY - nY,nPixelCol);
      microSDL_PutPixelRaw(pGui,pGui->surfScreen,nMidX - nY, nMidY - nX,nPixelCol);
      microSDL_PutPixelRaw(pGui,pGui->surfScreen,nMidX + nY, nMidY - nX,nPixelCol);
      microSDL_PutPixelRaw(pGui,pGui->surfScreen,nMidX + nX, nMidY - nY,nPixelCol);

      nY    += 1;
      nErr  += 1 + 2*nY;
      if (2*(nErr-nX) + 1 > 0)
      {
          nX -= 1;
          nErr += 1 - 2*nX;
      }
    } // while

    microSDL_Unlock(pGui);
  }   // microSDL_Lock
  
  microSDL_PageFlipSet(pGui,true);
}



// -----------------------------------------------------------------------
// Font Functions
// -----------------------------------------------------------------------

bool microSDL_FontAdd(microSDL_tsGui* pGui,int nFontId,const char* acFontName,unsigned nFontSz)
{
  if (pGui->nFontCnt+1 >= (pGui->nFontMax)) {
    fprintf(stderr,"ERROR: FontAdd() added too many fonts\n");
    return false;
  } else {
    TTF_Font*   pFont;
    pFont = TTF_OpenFont(acFontName,nFontSz);
    if (pFont == NULL) {
      fprintf(stderr,"ERROR: TTF_OpenFont(%s) failed\n",acFontName);
      return false;
    }
    pGui->psFont[pGui->nFontCnt].pFont = pFont;
    pGui->psFont[pGui->nFontCnt].nId = nFontId;
    pGui->nFontCnt++;  
    return true;
  }
}


TTF_Font* microSDL_FontGet(microSDL_tsGui* pGui,int nFontId)
{
  unsigned  nFontInd;
  for (nFontInd=0;nFontInd<pGui->nFontCnt;nFontInd++) {
    if (pGui->psFont[nFontInd].nId == nFontId) {
      return pGui->psFont[nFontInd].pFont;
    }
  }
  return NULL;
}




// ------------------------------------------------------------------------
// Page Functions
// ------------------------------------------------------------------------


void microSDL_PageAdd(microSDL_tsGui* pGui,int nPageId,microSDL_tsElem* psElem,unsigned nMaxElem)
{
  microSDL_tsPage*  pPage = &pGui->asPage[pGui->nPageCnt];

  microSDL_CollectReset(&pPage->sCollect,psElem,nMaxElem);
  
  // Assign the requested Page ID
  pPage->nPageId = nPageId;
  
  // Increment the page count
  pGui->nPageCnt++;
  
  // Default the page pointer to the first page we create
  if (microSDL_GetPageCur(pGui) == MSDL_PAGE_NONE) {
    microSDL_SetPageCur(pGui,nPageId);
  }
  
  // Force the page to redraw
  microSDL_PageRedrawSet(pGui,true);

}

int microSDL_GetPageCur(microSDL_tsGui* pGui)
{
  if (pGui->pCurPage == NULL) {
    return MSDL_PAGE_NONE;
  }
  return pGui->pCurPage->nPageId;
}


void microSDL_SetPageCur(microSDL_tsGui* pGui,int nPageId)
{
  int nPageSaved = MSDL_PAGE_NONE;
  if (pGui->pCurPage != NULL) {
    nPageSaved = pGui->pCurPage->nPageId;
  }
  
  // Find the page
  microSDL_tsPage* pPage = microSDL_PageFindById(pGui,nPageId);
  if (pPage == NULL) {
    fprintf(stderr,"ERROR: SetPageCur() can't find page (ID=%d)\n",nPageId);
    exit(1);
  }
  
  // Save a reference to the selected page
  pGui->pCurPage = pPage;
  
  // Save a reference to the selected page's element collection
  pGui->pCurPageCollect = &pPage->sCollect;
  
  // A change of page should always force a future redraw
  if (nPageSaved != nPageId) {
    microSDL_PageRedrawSet(pGui,true);
  }
}


// Adjust the flag that indicates whether the entire page
// requires a redraw.
void microSDL_PageRedrawSet(microSDL_tsGui* pGui,bool bRedraw)
{
  pGui->pCurPage->bPageNeedRedraw = bRedraw;
}

bool microSDL_PageRedrawGet(microSDL_tsGui* pGui)
{
  return pGui->pCurPage->bPageNeedRedraw;
}

// Check the redraw flag on all elements on the current page and update
// the redraw status if additional redraws are required (or the
// entire page should be marked as requiring redraw).
// - The typical case for this being required is when an element
//   requires redraw but it is marked as being transparent. Therefore,
//   the lower level elements should be redrawn.
// - For now, just mark the entire page as requiring redraw.
// TODO: Determine which elements underneath should be redrawn based
//       on the region exposed by the transparent element.
void microSDL_PageRedrawCalc(microSDL_tsGui* pGui)
{
  int               nInd;
  microSDL_tsElem*  pElem = NULL;
  for (nInd=MSDL_IND_FIRST;nInd<pGui->pCurPageCollect->nElemCnt;nInd++) {
    pElem = &(pGui->pCurPageCollect->asElem[nInd]);
    if (pElem->bNeedRedraw) {
      // Is the element transparent?
      if (!pElem->bFillEn) {
        // For now, mark the entire page as requiring redraw
        microSDL_PageRedrawSet(pGui,true);
        // No need to check any more elements
        break;
      }
    }
  }
}

// Redraw the active page
// - If the page has been marked as needing redraw, then all
//   elements are rendered
// - If the page has not been marked as needing redraw then only
//   the elements that have been marked as needing redraw
//   are rendered.
void microSDL_PageRedrawGo(microSDL_tsGui* pGui)
{ 
  // Update any page redraw status that may be required
  // - Note that this routine handles cases where an element
  //   marked as requiring update is semi-transparent which can
  //   cause other elements to be redrawn as well.
  microSDL_PageRedrawCalc(pGui);

  // Determine final state of full-page redraw
  bool  bPageRedraw = microSDL_PageRedrawGet(pGui);

  // If a full page redraw is required, then start by
  // redrawing the background.
  // NOTE:
  // - It would be cleaner if we could treat the background
  //   layer like any other element (and hence check for its
  //   need-redraw status).
  // - For now, assume background doesn't need update except
  //   if the entire page is to be redrawn
  //   TODO: Fix this assumption (either add specific flag
  //         for bBkgndNeedRedraw or make the background just
  //         another element).
  if (bPageRedraw) {
    microSDL_ApplySurface(pGui,0,0,pGui->surfBkgnd,pGui->surfScreen);
  }
    
  // Draw other elements (as needed)
  microSDL_CollectRedraw(pGui,pGui->pCurPageCollect,bPageRedraw);

  // Clear the page redraw flag
  microSDL_PageRedrawSet(pGui,false);
  
  // Page flip the entire screen
  // - TODO: We could also call Update instead of Flip as that would
  //         limit the region to refresh.
  microSDL_PageFlipGo(pGui);
  
}


void microSDL_PageFlipSet(microSDL_tsGui* pGui,bool bNeeded)
{
  pGui->pCurPage->bPageNeedFlip = bNeeded;
}

bool microSDL_PageFlipGet(microSDL_tsGui* pGui)
{
  return pGui->pCurPage->bPageNeedFlip;
}

void microSDL_PageFlipGo(microSDL_tsGui* pGui)
{
  if (pGui->pCurPage->bPageNeedFlip) {
    SDL_Flip(pGui->surfScreen);
    
    // Indicate that page flip is no longer required
    microSDL_PageFlipSet(pGui,false);
  }
}


microSDL_tsPage* microSDL_PageFindById(microSDL_tsGui* pGui,int nPageId)
{
  int nInd;
  
  // Loop through list of pages
  // Return pointer to page
  microSDL_tsPage*  pFoundPage = NULL;
  for (nInd=0;nInd<pGui->nPageMax;nInd++) {
    if (pGui->asPage[nInd].nPageId == nPageId) {
      pFoundPage = &pGui->asPage[nInd];
      break;
    }
  }
  
  // Error handling: if not found, make this a fatal error
  // as it shows a serious config error and continued operation
  // is not viable.
  if (pFoundPage == NULL) {
    fprintf(stderr,"ERROR: PageGet() could not find page (ID=%d)",nPageId);
    exit(1);
  }
  
  return pFoundPage;
}

microSDL_tsElem* microSDL_PageFindElemById(microSDL_tsGui* pGui,int nPageId,int nElemId)
{
  microSDL_tsPage*  pPage = NULL;
  microSDL_tsElem*  pElem = NULL;

  // Get the page
  pPage = microSDL_PageFindById(pGui,nPageId);
  if (pPage == NULL) {
    fprintf(stderr,"ERROR: PageFindElemById() can't find page (ID=%d)\n",nPageId);
    exit(1);
  }
  // Find the element in the page's element collection
  pElem = microSDL_CollectFindElemById(&pPage->sCollect,nElemId);
  return pElem;
}


// ------------------------------------------------------------------------
// Element General Functions
// ------------------------------------------------------------------------


int microSDL_ElemGetIdFromElem(microSDL_tsGui* pGui,microSDL_tsElem* pElem)
{
  return pElem->nId;
}


// ------------------------------------------------------------------------
// Element Creation Functions
// ------------------------------------------------------------------------


microSDL_tsElem* microSDL_ElemCreateTxt(microSDL_tsGui* pGui,int nElemId,int nPage,SDL_Rect rElem,
  const char* pStr,int nFontId)
{
  microSDL_tsElem   sElem;
  microSDL_tsElem*  pElem = NULL;
  sElem = microSDL_ElemCreate(pGui,nElemId,nPage,MSDL_TYPE_TXT,rElem,pStr,nFontId);
  sElem.colElemFill     = MSDL_COL_BLACK;
  sElem.colElemFrame    = MSDL_COL_BLACK;
  sElem.colElemGlow     = MSDL_COL_BLACK;
  sElem.colElemText     = MSDL_COL_YELLOW;
  sElem.bFillEn         = true;
  sElem.eTxtAlign       = MSDL_ALIGN_MID_LEFT;
  if (nPage != MSDL_PAGE_NONE) {
    pElem = microSDL_ElemAdd(pGui,nPage,&sElem);
    return pElem;
  } else {
    // Save as temporary element
    pGui->sElemTmp = sElem;
    return &(pGui->sElemTmp);      
  }
}


microSDL_tsElem* microSDL_ElemCreateBtnTxt(microSDL_tsGui* pGui,int nElemId,int nPage,
  SDL_Rect rElem,const char* acStr,int nFontId,MSDL_CB_TOUCH cbTouch)
{
  microSDL_tsElem   sElem;
  microSDL_tsElem*  pElem = NULL;
  
  // Ensure the Font is loaded
  TTF_Font* pFont = microSDL_FontGet(pGui,nFontId);
  if (pFont == NULL) {
    fprintf(stderr,"ERROR: ElemCreateBtnTxt(ID=%d): Font(ID=%d) not loaded\n",nElemId,nFontId);
    return NULL;
  }

  sElem = microSDL_ElemCreate(pGui,nElemId,nPage,MSDL_TYPE_BTN,rElem,acStr,nFontId);
  sElem.colElemFill     = MSDL_COL_BLUE_LT;
  sElem.colElemFrame    = MSDL_COL_BLUE_DK;
  sElem.colElemGlow     = MSDL_COL_GREEN_LT;
  sElem.colElemText     = MSDL_COL_BLACK;
  sElem.bFrameEn        = true;
  sElem.bFillEn         = true;
  sElem.bClickEn        = true;
  sElem.pfuncXTouch     = cbTouch;
  if (nPage != MSDL_PAGE_NONE) {
    pElem = microSDL_ElemAdd(pGui,nPage,&sElem);
    return pElem;
  } else {
    // Save as temporary element
    pGui->sElemTmp = sElem;
    return &(pGui->sElemTmp);       
  }
}

microSDL_tsElem* microSDL_ElemCreateBtnImg(microSDL_tsGui* pGui,int nElemId,int nPage,
  SDL_Rect rElem,const char* acImg,const char* acImgSel,MSDL_CB_TOUCH cbTouch)
{
  microSDL_tsElem   sElem;
  microSDL_tsElem*  pElem = NULL;
  sElem = microSDL_ElemCreate(pGui,nElemId,nPage,MSDL_TYPE_BTN,rElem,"",MSDL_FONT_NONE);
  sElem.colElemFill     = MSDL_COL_BLUE_LT;
  sElem.colElemFrame    = MSDL_COL_BLUE_DK;
  sElem.colElemGlow     = MSDL_COL_GREEN_LT;
  sElem.colElemText     = MSDL_COL_BLACK;
  sElem.bFrameEn        = false;
  sElem.bFillEn         = false;
  sElem.bClickEn        = true;
  sElem.pfuncXTouch     = cbTouch;  
  microSDL_ElemSetImage(pGui,&sElem,acImg,acImgSel);
  if (nPage != MSDL_PAGE_NONE) {
    pElem = microSDL_ElemAdd(pGui,nPage,&sElem);
    return pElem;
  } else {
    // Save as temporary element
    pGui->sElemTmp = sElem;
    return &(pGui->sElemTmp);     
  }
}


microSDL_tsElem* microSDL_ElemCreateBox(microSDL_tsGui* pGui,int nElemId,int nPage,SDL_Rect rElem)
{
  microSDL_tsElem   sElem;
  microSDL_tsElem*  pElem = NULL;
  sElem = microSDL_ElemCreate(pGui,nElemId,nPage,MSDL_TYPE_BOX,rElem,NULL,MSDL_FONT_NONE);
  sElem.colElemFill     = MSDL_COL_BLACK;
  sElem.colElemFrame    = MSDL_COL_GRAY;
  sElem.bFillEn         = true;
  sElem.bFrameEn        = true;
  if (nPage != MSDL_PAGE_NONE) {
    pElem = microSDL_ElemAdd(pGui,nPage,&sElem);  
    return pElem;
  } else {
    // Save as temporary element
    pGui->sElemTmp = sElem;
    return &(pGui->sElemTmp);     
  }
}


microSDL_tsElem* microSDL_ElemCreateImg(microSDL_tsGui* pGui,int nElemId,int nPage,
  SDL_Rect rElem,const char* acImg)
{
  microSDL_tsElem   sElem;
  microSDL_tsElem*  pElem = NULL;
  sElem = microSDL_ElemCreate(pGui,nElemId,nPage,MSDL_TYPE_BOX,rElem,"",MSDL_FONT_NONE);
  sElem.bFrameEn        = false;
  sElem.bFillEn         = false;
  sElem.bClickEn        = false;
  microSDL_ElemSetImage(pGui,&sElem,acImg,acImg);
  if (nPage != MSDL_PAGE_NONE) {
    pElem = microSDL_ElemAdd(pGui,nPage,&sElem);
    return pElem;
  } else {
    // Save as temporary element
    pGui->sElemTmp = sElem;
    return &(pGui->sElemTmp);     
  }
}



// ------------------------------------------------------------------------
// Element Drawing Functions
// ------------------------------------------------------------------------


// Draw an element to the active display
// - Element is referenced by page ID and element ID
// - This routine is typically called by user code for custom
//   drawing callbacks
void microSDL_ElemDraw(microSDL_tsGui* pGui,int nPageId,int nElemId)
{
  microSDL_tsElem* pElem = microSDL_PageFindElemById(pGui,nPageId,nElemId);
  microSDL_ElemDrawByRef(pGui,pElem);
}

// Draw an element to the active display
// - Element is referenced by an element pointer
// - TODO: Handle MSDL_TYPE_BKGND
bool microSDL_ElemDrawByRef(microSDL_tsGui* pGui,microSDL_tsElem* pElem)
{
  if ((pGui == NULL) || (pElem == NULL)) {
    fprintf(stderr,"ERROR: ElemDrawByRef() called with NULL ptr\n");
    return false;
  }    
  
  // --------------------------------------------------------------------------
  // Custom drawing
  // --------------------------------------------------------------------------
  
  // Handle any extended element types
  // - If the pfuncXDraw callback is defined, then let the callback
  //   function supersede all default handling here
  // - Note that the end of the callback function is expected
  //   to clear the redraw flag
  if (pElem->pfuncXDraw != NULL) {
    (*pElem->pfuncXDraw)((void*)(pGui),(void*)(pElem));
    return true;
  }  
  
  // --------------------------------------------------------------------------
  // Init for default drawing
  // --------------------------------------------------------------------------
  
  bool              bGlowing;
  int               nElemX,nElemY;
  unsigned          nElemW,nElemH;
  SDL_Surface*      surfTxt = NULL;
  
  nElemX    = pElem->rElem.x;
  nElemY    = pElem->rElem.y;
  nElemW    = pElem->rElem.w;
  nElemH    = pElem->rElem.h;
  bGlowing  = pElem->bGlowing; // Is the element currently glowing?
  
  // --------------------------------------------------------------------------
  // Background
  // --------------------------------------------------------------------------
  
  // Fill in the background
  // - This also changes the fill color if it is a button
  //   being selected
  if (pElem->bFillEn) {
    // TODO: Remove nType check during glowing. Instead use
    // an enable?
    if ((pElem->nType == MSDL_TYPE_BTN) && (bGlowing)) {
      microSDL_FillRect(pGui,pElem->rElem,pElem->colElemGlow);
    } else {
      microSDL_FillRect(pGui,pElem->rElem,pElem->colElemFill);
    }
  } else {
    // TODO: If unfilled, then we might need
    // to redraw the background layer(s)
  }

  // --------------------------------------------------------------------------
  // Frame
  // --------------------------------------------------------------------------

  // Frame the region
  #ifdef DBG_FRAME
  // For debug purposes, draw a frame around every element
  microSDL_FrameRect(pGui,sElem.rElem,MSDL_COL_GRAY_DK);
  #else
  if (pElem->bFrameEn) {
    microSDL_FrameRect(pGui,pElem->rElem,pElem->colElemFrame);
  }
  #endif

  
  // --------------------------------------------------------------------------
  // Image overlays
  // --------------------------------------------------------------------------
  
  // Draw any images associated with element
  if (pElem->pSurf != NULL) {
    if ((bGlowing) && (pElem->pSurfGlow != NULL)) {
      microSDL_ApplySurface(pGui,nElemX,nElemY,pElem->pSurfGlow,pGui->surfScreen);
    } else {
      microSDL_ApplySurface(pGui,nElemX,nElemY,pElem->pSurf,pGui->surfScreen);
    }
  }

  // --------------------------------------------------------------------------
  // Text overlays
  // --------------------------------------------------------------------------
 
  // Overlay the text
  bool    bRenderTxt = true;
  bRenderTxt &= (pElem->acStr[0] != '\0');
  bRenderTxt &= (pElem->pTxtFont != NULL);

  if (bRenderTxt) {

    // Define margined element bounds
    int         nTxtX = nElemX;
    int         nTxtY = nElemY;
    unsigned    nMargin = pElem->nTxtMargin;
    TTF_Font*   pFont = pElem->pTxtFont;

    // Fetch the size of the text to allow for justification
    int nTxtSzW,nTxtSzH;
    TTF_SizeText(pFont,pElem->acStr,&nTxtSzW,&nTxtSzH);

    surfTxt = TTF_RenderText_Solid(pFont,pElem->acStr,pElem->colElemText);
    if (surfTxt == NULL) {
      fprintf(stderr,"ERROR: TTF_RenderText_Solid failed (%s)\n",pElem->acStr);
      return false;
    }

    // Handle text alignments

    // Check for ALIGNH_LEFT & ALIGNH_RIGHT. Default to ALIGNH_MID
    if      (pElem->eTxtAlign & MSDL_ALIGNH_LEFT)     { nTxtX = nElemX+nMargin; }
    else if (pElem->eTxtAlign & MSDL_ALIGNH_RIGHT)    { nTxtX = nElemX+nElemW-nMargin-nTxtSzW; }
    else                                              { nTxtX = nElemX+(nElemW/2)-(nTxtSzW/2); }

    // Check for ALIGNV_TOP & ALIGNV_BOT. Default to ALIGNV_MID
    if      (pElem->eTxtAlign & MSDL_ALIGNV_TOP)      { nTxtY = nElemY+nMargin; }
    else if (pElem->eTxtAlign & MSDL_ALIGNV_BOT)      { nTxtY = nElemY+nElemH-nMargin-nTxtSzH; }
    else                                              { nTxtY = nElemY+(nElemH/2)-(nTxtSzH/2); }


    microSDL_ApplySurface(pGui,nTxtX,nTxtY,surfTxt,pGui->surfScreen);

    if (surfTxt != NULL) {
      SDL_FreeSurface(surfTxt);
      surfTxt = NULL;
    }
  }

  // Mark the element as no longer requiring redraw
  microSDL_ElemSetRedraw(pElem,false);
  
  return true;
}



// ------------------------------------------------------------------------
// Element Update Functions
// ------------------------------------------------------------------------

void microSDL_ElemSetFillEn(microSDL_tsElem* pElem,bool bFillEn)
{
  if (pElem == NULL) {
    fprintf(stderr,"ERROR: ElemSetFillEn() called with NULL ptr\n");
    return;
  }
  pElem->bFillEn          = bFillEn;  
  microSDL_ElemSetRedraw(pElem,true); 
}

void microSDL_ElemSetCol(microSDL_tsElem* pElem,SDL_Color colFrame,SDL_Color colFill,SDL_Color colGlow)
{
  if (pElem == NULL) {
    fprintf(stderr,"ERROR: ElemSetCol() called with NULL ptr\n");
    return;
  }  
  pElem->colElemFrame     = colFrame;
  pElem->colElemFill      = colFill;
  pElem->colElemGlow      = colGlow;
  microSDL_ElemSetRedraw(pElem,true); 
}

void microSDL_ElemSetGroup(microSDL_tsElem* pElem,int nGroupId)
{
  if (pElem == NULL) {
    fprintf(stderr,"ERROR: ElemSetGroup() called with NULL ptr\n");
    return;
  }    
  pElem->nGroup           = nGroupId;
}

int microSDL_ElemGetGroup(microSDL_tsElem* pElem)
{
  if (pElem == NULL) {
    fprintf(stderr,"ERROR: ElemGetGroup() called with NULL ptr\n");
    return MSDL_GROUP_ID_NONE;
  }    
  return pElem->nGroup;  
}


void microSDL_ElemSetTxtAlign(microSDL_tsElem* pElem,unsigned nAlign)
{
  if (pElem == NULL) {
    fprintf(stderr,"ERROR: ElemSetTxtAlign() called with NULL ptr\n");
    return;
  }    
  pElem->eTxtAlign        = nAlign;
  microSDL_ElemSetRedraw(pElem,true);  
}


void microSDL_ElemSetTxtStr(microSDL_tsElem* pElem,const char* pStr)
{
  if (pElem == NULL) {
    fprintf(stderr,"ERROR: ElemSetTxtStr() called with NULL ptr\n");
    return;
  }    
  strncpy(pElem->acStr,pStr,MSDL_ELEM_STRLEN_MAX-1);
  pElem->acStr[MSDL_ELEM_STRLEN_MAX-1] = '\0';  // Force termination
  microSDL_ElemSetRedraw(pElem,true);
}

void microSDL_ElemSetTxtCol(microSDL_tsElem* pElem,SDL_Color colVal)
{
  if (pElem == NULL) {
    fprintf(stderr,"ERROR: ElemSetTxtCol() called with NULL ptr\n");
    return;
  }    
  pElem->colElemText      = colVal;
  microSDL_ElemSetRedraw(pElem,true); 
}

void microSDL_ElemUpdateFont(microSDL_tsGui* pGui,microSDL_tsElem* pElem,int nFontId)
{
  if ((pGui == NULL) || (pElem == NULL)) {
    fprintf(stderr,"ERROR: ElemUpdateFont() called with NULL ptr\n");
    return;
  }    
  pElem->pTxtFont         = microSDL_FontGet(pGui,nFontId);
  microSDL_ElemSetRedraw(pElem,true);
}

void microSDL_ElemSetRedraw(microSDL_tsElem* pElem,bool bRedraw)
{
  if (pElem == NULL) {
    fprintf(stderr,"ERROR: ElemSetRedraw() called with NULL ptr\n");
    return;
  }    
  pElem->bNeedRedraw      = bRedraw;
  
  // Now propagate up the element hierarchy
  // (eg. in case of compound elements)
  if (pElem->pElemParent != NULL) {
    microSDL_ElemSetRedraw(pElem->pElemParent,bRedraw);
  }
}

void microSDL_ElemSetGlow(microSDL_tsElem* pElem,bool bGlowing)
{
  if (pElem == NULL) {
    fprintf(stderr,"ERROR: ElemSetGlow() called with NULL ptr\n");
    return;
  }    
  pElem->bGlowing         = bGlowing;
  microSDL_ElemSetRedraw(pElem,true);
}

bool microSDL_ElemGetGlow(microSDL_tsElem* pElem)
{
  if (pElem == NULL) {
    fprintf(stderr,"ERROR: ElemGetGlow() called with NULL ptr\n");
    return false;
  }    
  return pElem->bGlowing;
}

void microSDL_ElemSetDrawFunc(microSDL_tsElem* pElem,MSDL_CB_DRAW funcCb)
{
  if ((pElem == NULL) || (funcCb == NULL)) {
    fprintf(stderr,"ERROR: ElemSetDrawFunc() called with NULL ptr\n");
    return;
  }    
  pElem->pfuncXDraw       = funcCb;
  microSDL_ElemSetRedraw(pElem,true);   
}

void microSDL_ElemSetTickFunc(microSDL_tsElem* pElem,MSDL_CB_TICK funcCb)
{
  if ((pElem == NULL) || (funcCb == NULL)) {
    fprintf(stderr,"ERROR: ElemSetTickFunc() called with NULL ptr\n");
    return;
  }    
  pElem->pfuncXTick       = funcCb; 
}


// ------------------------------------------------------------------------
// Viewport Functions
// ------------------------------------------------------------------------

// TODO: Consider changing return value to tsView*?
int microSDL_ViewCreate(microSDL_tsGui* pGui,int nViewId,SDL_Rect rView,unsigned nOriginX,unsigned nOriginY)
{
  if (pGui == NULL) {
    fprintf(stderr,"ERROR: ViewCreate() called with NULL ptr\n");
    return MSDL_VIEW_ID_NONE;
  }      
  if (pGui->nViewCnt+1 >= (pGui->nViewMax)) {
    fprintf(stderr,"ERROR: ViewCreate() too many views\n");
    return MSDL_VIEW_ID_NONE;
  }
  // Add the element to the internal array
  microSDL_tsView   sView;
  sView.nId         = nViewId;
  sView.rView       = rView;
  sView.nOriginX    = nOriginX;
  sView.nOriginY    = nOriginY;
  pGui->psView[pGui->nViewCnt] = sView;
  pGui->nViewCnt++;

  return nViewId;
}

bool microSDL_ViewSetOrigin(microSDL_tsGui* pGui,int nViewId,unsigned nOriginX,unsigned nOriginY)
{
  if (pGui == NULL) {
    fprintf(stderr,"ERROR: ViewSetOrigin() called with NULL ptr\n");
    return false;
  }      
  // Find View
  int nViewInd = microSDL_ViewFindIndFromId(pGui,nViewId);
  // If MSDL_VIEW_ID_SCREEN is passed, then it will be ignored here
  if (nViewInd == MSDL_VIEW_IND_NONE) {
    return false;
  }
  pGui->psView[nViewInd].nOriginX = nOriginX;
  pGui->psView[nViewInd].nOriginY = nOriginY;
  return true;
}

void microSDL_ViewSet(microSDL_tsGui* pGui,int nViewId)
{
  if (pGui == NULL) {
    fprintf(stderr,"ERROR: ViewSet() called with NULL ptr\n");
    return;
  }      
  // Ensure that the view ID is valid
  int nViewInd = MSDL_VIEW_IND_NONE;
  if (nViewId == MSDL_VIEW_ID_SCREEN) {
    nViewInd = MSDL_VIEW_IND_SCREEN;
  } else {
    nViewInd = microSDL_ViewFindIndFromId(pGui,nViewId); 
  }

  if (nViewInd == MSDL_VIEW_IND_NONE) {
    fprintf(stderr,"ERROR: ViewSet() invalid ID=%d\n",nViewId);
  }

  // Update the clipping rect temporarily
  if (nViewInd == MSDL_VIEW_IND_SCREEN) {
    // Set to full size of screen
    SDL_SetClipRect(pGui->surfScreen,NULL);
  } else if (microSDL_ViewIndValid(pGui,nViewInd)) {
    // Set to user-specified region
    SDL_SetClipRect(pGui->surfScreen,&(pGui->psView[nViewInd]).rView);
  } else {
    // INVALID
  }
  // Assign the view
  pGui->nViewIndCur = nViewInd;
}


// ------------------------------------------------------------------------
// Tracking Functions
// ------------------------------------------------------------------------

void microSDL_CollectTouch(microSDL_tsGui* pGui,microSDL_tsCollect* pCollect,bool bTouchDown,bool bTouchUp,bool bTouchMove,int nX,int nY)
{
  microSDL_tsElem*  pTrackedOld = NULL;
  microSDL_tsElem*  pTrackedNew = NULL;
  
  // Fetch the item currently being tracked (if any)
  pTrackedOld = microSDL_CollectGetElemTracked(pCollect);
  
  // Reset the in-tracked flag
  bool  bInTracked = false;
  
  if (bTouchDown) {
    // ---------------------------------
    // Touch Down Event
    // ---------------------------------
    
    // End glow on previously tracked element (if any)
    // - We shouldn't really enter a "Touch Down" event
    //   with an element still marked as being tracked
    if (pTrackedOld != NULL) {
      microSDL_ElemSetGlow(pTrackedOld,false);
    }
    
    // Determine the new element to start tracking
    pTrackedNew = microSDL_CollectFindElemFromCoord(pCollect,nX,nY);
    
    if (pTrackedNew == NULL) {
      // Didn't find an element, so clear the tracking reference   
      microSDL_CollectSetElemTracked(pCollect,NULL);
    } else {
      // Found an element, so mark it as being the tracked element

      // Set the new tracked element reference
      microSDL_CollectSetElemTracked(pCollect,pTrackedNew);

      // Start glow on new element
      microSDL_ElemSetGlow(pTrackedNew,true);
      
      // Notify element for optional custom handling
      // - We do this after we have determined which element should
      //   receive the touch tracking
      microSDL_NotifyElemTouch(pGui,pTrackedNew,MSDL_TOUCH_DOWN_IN,nX,nY);
    }
   
  } else if (bTouchUp) {
    // ---------------------------------
    // Touch Up Event
    // ---------------------------------

    if (pTrackedOld != NULL) {
      // Are we still over tracked element?
      bInTracked = microSDL_IsInRect(nX,nY,pTrackedOld->rElem);
  
      if (!bInTracked) {
        // Released not over tracked element
        microSDL_NotifyElemTouch(pGui,pTrackedOld,MSDL_TOUCH_UP_OUT,nX,nY);
      } else {
        // Notify original tracked element for optional custom handling
        microSDL_NotifyElemTouch(pGui,pTrackedOld,MSDL_TOUCH_UP_IN,nX,nY);

        // Clear glow state
        microSDL_ElemSetGlow(pTrackedOld,false);
      }

    }

    // Clear the element tracking state
    microSDL_CollectSetElemTracked(pCollect,NULL);
    
  } else if (bTouchMove) {
    // ---------------------------------
    // Touch Move Event
    // ---------------------------------
    
    // Determine if we are still over tracked element
    if (pTrackedOld != NULL) {
      bInTracked = microSDL_IsInRect(nX,nY,pTrackedOld->rElem);

      if (!bInTracked) {

        // Not currently over tracked element
        // - Notify tracked element that we moved out of it
        microSDL_NotifyElemTouch(pGui,pTrackedOld,MSDL_TOUCH_MOVE_OUT,nX,nY);

        // Ensure the tracked element is no longer glowing
        if (pTrackedOld) {
          microSDL_ElemSetGlow(pTrackedOld,false);
        }
      } else {
        // We are still over tracked element
        // - Notify tracked element
        microSDL_NotifyElemTouch(pGui,pTrackedOld,MSDL_TOUCH_MOVE_IN,nX,nY);

        // Ensure it is glowing
        microSDL_ElemSetGlow(pTrackedOld,true);
      }      
      
    }   

  }  
}

// This routine is responsible for the GUI-level touch event state machine
// and dispatching to the touch even handler for the collection
void microSDL_TrackTouch(microSDL_tsGui* pGui,microSDL_tsCollect* pCollect,int nX,int nY,unsigned nPress)
{
  if ((pGui == NULL) || (pCollect == NULL)) {
    fprintf(stderr,"ERROR: TrackTouch() called with NULL ptr\n");
    return;
  }    
  #ifdef DBG_TOUCH
  printf(" TS : (%3d,%3d) Pressure=%3u\n",nX,nY,nPress);
  #endif

  bool  bTouchDown  = false;
  bool  bTouchUp    = false;
  bool  bTouchMove  = false;
  if ((pGui->nTouchLastPress == 0) && (nPress > 0)) {
    bTouchDown = true;
  } else if ((pGui->nTouchLastPress > 0) && (nPress == 0)) {
    bTouchUp = true;
  } else if ((pGui->nTouchLastX != nX) || (pGui->nTouchLastY != nY)) {
    // We only track movement if touch is "down"
    if (nPress > 0) {
      bTouchMove = true;
    }
  }
  
  microSDL_CollectTouch(pGui,pCollect,bTouchDown,bTouchUp,bTouchMove,nX,nY);

  // Save raw touch status so that we can detect transitions
  pGui->nTouchLastX      = nX;
  pGui->nTouchLastY      = nY;
  pGui->nTouchLastPress  = nPress;
}


bool microSDL_GetSdlTouch(microSDL_tsGui* pGui,int* pnX,int* pnY,unsigned* pnPress)
{
  if (pGui == NULL) {
    fprintf(stderr,"ERROR: GetSdlTouch() called with NULL ptr\n");
    return false;
  }    
  bool        bRet = false;
  SDL_Event   sEvent;
  if (SDL_PollEvent(&sEvent)) {
    // Placeholder content for keypress handler
    // TODO: Move into separate routine
    if (sEvent.type == SDL_KEYDOWN) {
      switch(sEvent.key.keysym.sym) {
        case SDLK_UP: break;
        case SDLK_DOWN: break;
        case SDLK_RIGHT: break;
        default: break;
      }
    } else if (sEvent.type == SDL_KEYUP) {

    } else if (sEvent.type == SDL_MOUSEMOTION) {
    } else if (sEvent.type == SDL_MOUSEBUTTONDOWN) {
      SDL_GetMouseState(pnX,pnY);
      (*pnPress) = 1;
      bRet = true;
    } else if (sEvent.type == SDL_MOUSEBUTTONUP) {
      SDL_GetMouseState(pnX,pnY);
      (*pnPress) = 0;
      bRet = true;
    }
  } // SDL_PollEvent()

  return bRet;
}


// ------------------------------------------------------------------------
// Touchscreen Functions
// ------------------------------------------------------------------------

#ifdef INC_TS

// POST:
// - pGui->ts mapped to touchscreen device
bool microSDL_InitTs(microSDL_tsGui* pGui,const char* acDev)
{
  if (pGui == NULL) {
    fprintf(stderr,"ERROR: InitTs() called with NULL ptr\n");
    return false;
  }    
  // TODO: Consider using env "TSLIB_TSDEVICE" instead
  //char* pDevName = NULL;
  //pDevName = getenv("TSLIB_TSDEVICE");
  //pGui->ts = ts_open(pDevName,1);
  
  // Open in non-blocking mode
  pGui->ts = ts_open(acDev,1);
  if (!pGui->ts) {
    fprintf(stderr,"ERROR: TsOpen\n");
    return false;
  }

  if (ts_config(pGui->ts)) {
    fprintf(stderr,"ERROR: TsConfig\n");
    // Clear the tslib pointer so we don't try to call it again
    pGui->ts = NULL;
    return false;
  }
  return true;
}

int microSDL_GetTsTouch(microSDL_tsGui* pGui,int* pnX,int* pnY,unsigned* pnPress)
{
  if (pGui == NULL) {
    fprintf(stderr,"ERROR: GetTsTouch() called with NULL ptr\n");
    return 0;
  }    
  // In case tslib was not loaded, exit now
  if (pGui->ts == NULL) {
    return 0;
  }
  struct ts_sample   pSamp;
  int nRet    = ts_read(pGui->ts,&pSamp,1);
  (*pnX)      = pSamp.x;
  (*pnY)      = pSamp.y;
  (*pnPress)  = pSamp.pressure;
  return nRet;
}

#endif // INC_TS



// ------------------------------------------------------------------------
// Private Functions
// ------------------------------------------------------------------------

Uint32 microSDL_GenPixelColor(microSDL_tsGui* pGui,SDL_Color nCol)
{
  if (pGui == NULL) {
    fprintf(stderr,"ERROR: GenPixelColor() called with NULL ptr\n");
    return 0;
  }      
  return SDL_MapRGB(pGui->surfScreen->format,nCol.r,nCol.g,nCol.b);
}


Uint32 microSDL_GetPixelRaw(microSDL_tsGui* pGui,SDL_Surface *surf, int nX, int nY)
{
  if ((pGui == NULL) || (surf == NULL)) {
    fprintf(stderr,"ERROR: GetPixelRaw() called with NULL ptr\n");
    return 0;
  }       
  int nBpp = surf->format->BytesPerPixel;

  // Handle any range violations for entire surface
  if ( (nX < 0) || (nX >= surf->w) ||
       (nY < 0) || (nY >= surf->h) ) {
    // ERROR
    fprintf(stderr,"ERROR: GetPixelRaw() out of range (%i,%i)\n",nX,nY);
    return 0;
  }

  /* Here pPixel is the address to the pixel we want to get */
  Uint8 *pPixel = (Uint8 *)surf->pixels + nY * surf->pitch + nX * nBpp;

  switch(nBpp) {
    case 1:
      return *pPixel;
  
    case 2:
      return *(Uint16 *)pPixel;
  
    case 3:
      if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
        return pPixel[0] << 16 | pPixel[1] << 8 | pPixel[2];
      else
        return pPixel[0] | pPixel[1] << 8 | pPixel[2] << 16;
  
    case 4:
      return *(Uint32 *)pPixel;
  
    default:
      return 0;       /* shouldn't happen, but avoids warnings */
  }

}


// - Based on code from:
// -   https://www.libsdl.org/release/SDL-1.2.15/docs/html/guidevideo.html
// - Added range checks from surface clipping rect
// - TODO: Remove pGui
void microSDL_PutPixelRaw(microSDL_tsGui* pGui,SDL_Surface *surf, int nX, int nY, Uint32 nPixelVal)
{
  if (surf == NULL) {
    fprintf(stderr,"ERROR: PutPixelRaw() called with NULL ptr\n");
    return;
  }        
  int nBpp = surf->format->BytesPerPixel;

  // Handle any clipping
  if ( (nX < surf->clip_rect.x) || (nX >= surf->clip_rect.x+surf->clip_rect.w) ||
       (nY < surf->clip_rect.y) || (nY >= surf->clip_rect.y+surf->clip_rect.h) ) {
    return;
  }

  /* Here pPixel is the address to the pixel we want to set */
  Uint8 *pPixel = (Uint8 *)surf->pixels + nY * surf->pitch + nX * nBpp;

  switch(nBpp) {
    case 1:
      *pPixel = nPixelVal;
      break;

    case 2:
      *(Uint16 *)pPixel = nPixelVal;
      break;

    case 3:
      if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
        pPixel[0] = (nPixelVal >> 16) & 0xff;
        pPixel[1] = (nPixelVal >> 8) & 0xff;
        pPixel[2] = nPixelVal & 0xff;
      } else {
        pPixel[0] = nPixelVal & 0xff;
        pPixel[1] = (nPixelVal >> 8) & 0xff;
        pPixel[2] = (nPixelVal >> 16) & 0xff;
      }
      break;

    case 4:
      *(Uint32 *)pPixel = nPixelVal;
      break;
  }
}

void microSDL_SetPixelRaw(microSDL_tsGui* pGui,Sint16 nX,Sint16 nY,Uint32 nPixelCol)
{
  if (pGui == NULL) {
    fprintf(stderr,"ERROR: SetPixelRaw() called with NULL ptr\n");
    return;
  }     
  microSDL_PutPixelRaw(pGui,pGui->surfScreen,nX,nY,nPixelCol);
}


bool microSDL_Lock(microSDL_tsGui* pGui)
{
  if (pGui == NULL) {
    fprintf(stderr,"ERROR: Lock() called with NULL ptr\n");
    return false;
  }       
  if (SDL_MUSTLOCK(pGui->surfScreen)) {
    if (SDL_LockSurface(pGui->surfScreen) < 0) {
      fprintf(stderr,"ERROR: Can't lock screen: %s\n",SDL_GetError());
      return false;
    }
  }   
  return true;
}


void microSDL_Unlock(microSDL_tsGui* pGui)
{
  if (pGui == NULL) {
    fprintf(stderr,"ERROR: Unlock() called with NULL ptr\n");
    return;
  }      
  if (SDL_MUSTLOCK(pGui->surfScreen)) {
    SDL_UnlockSurface(pGui->surfScreen);
  }
}

// NOTE: nId is a positive ID specified by the user or
//       MSDL_ID_AUTO if the user wants an auto-generated ID
//       (which will be assigned in Element nId)
// NOTE: When we are creating sub-elements within a compound element,
//       we usually pass nPageId=MSDL_PAGE_NONE. In this mode we
//       won't add the element to any page, but just create the
//       element struct. However, in this mode we can't support
//       auto-generated IDs since we don't know which IDs will
//       be taken when we finally create the compound element.
microSDL_tsElem microSDL_ElemCreate(microSDL_tsGui* pGui,int nElemId,int nPageId,
  microSDL_teType nType,SDL_Rect rElem,const char* pStr,int nFontId)
{
  microSDL_tsElem sElem;
  // Assign defaults to the element record
  microSDL_ResetElem(&sElem);
  
  if (pGui == NULL) {
    fprintf(stderr,"ERROR: ElemCreate() called with NULL ptr\n");
    return sElem;
  }  

  microSDL_tsPage*    pPage = NULL;
  microSDL_tsCollect* pCollect = NULL;
  
  // If we are going to be adding the element to a page then we
  // perform some additional checks
  if (nPageId == MSDL_PAGE_NONE) {
    // This is a temporary element, so we skip the ID collision checks.
    // In this mode we don't support auto ID assignment
    if (nElemId == MSDL_ID_AUTO) {
      fprintf(stderr,"ERROR: ElemCreate() doesn't support temp elements with auto ID\n");
      return sElem;
    }
  } else {
    // Look up the targeted page to ensure that we check its
    // collection for collision with other IDs (or assign the
    // next available if auto-incremented)
    pPage     = microSDL_PageFindById(pGui,nPageId);
    if (pPage == NULL) {
      fprintf(stderr,"ERROR: ElemCreate() can't find page (ID=%d)\n",nPageId);
      return sElem;
    }     
    pCollect  = &pPage->sCollect;
  
    // Validate the user-supplied ID
    if (nElemId == MSDL_ID_AUTO) {
      // Get next auto-generated ID
      nElemId = microSDL_CollectGetNextId(pCollect);
    } else {
      // Ensure the ID is positive
      if (nElemId < 0) {
        fprintf(stderr,"ERROR: ElemCreate() called with negative ID (%d)\n",nElemId);
        return sElem;
      }
      // Ensure the ID isn't already taken
      if (microSDL_CollectFindElemById(pCollect,nElemId) != NULL) {
        fprintf(stderr,"ERROR: ElemCreate() called with existing ID (%d)\n",nElemId);
        return sElem;
      }
    }
  }


  // Override defaults with parameterization
  sElem.nId             = nElemId;
  sElem.rElem           = rElem;
  sElem.nType           = nType;
  sElem.pTxtFont        = microSDL_FontGet(pGui,nFontId);  
  if (pStr != NULL) {
    strncpy(sElem.acStr,pStr,MSDL_ELEM_STRLEN_MAX-1);
    sElem.acStr[MSDL_ELEM_STRLEN_MAX-1] = '\0';  // Force termination    
  }  
  
  // If the element creation was successful, then set the valid flag
  sElem.bValid          = true;

  return sElem;
}


// Helper function for microSDL_ElemAdd()
microSDL_tsElem* microSDL_CollectElemAdd(microSDL_tsCollect* pCollect,microSDL_tsElem* pElem)
{
  if ((pCollect == NULL) || (pElem == NULL)) {
    fprintf(stderr,"ERROR: CollectElemAdd() called with NULL ptr\n");
    return NULL;
  }    
  
  if (pCollect->nElemCnt+1 >= (pCollect->nElemMax)) {
    fprintf(stderr,"ERROR: CollectElemAdd() too many elements\n");
    return NULL;
  }
  // In case the element creation failed, trap that here
  if (!pElem->bValid) {
    fprintf(stderr,"ERROR: CollectElemAdd() skipping add of invalid element\n");
    return NULL;
  }
  // Add the element to the internal array
  // - This performs a copy so that we can discard the element
  //   pointer after the call is complete
  int nElemInd = pCollect->nElemCnt;  
  microSDL_CollectAdd(pCollect,pElem);

  return &(pCollect->asElem[nElemInd]);    
}


// Add an element to the collection associated with the page
//
// NOTE: The content (not address) of pElem is copied so the pointer
//       can be released after the call.
microSDL_tsElem* microSDL_ElemAdd(microSDL_tsGui* pGui,int nPageId,microSDL_tsElem* pElem)
{
  if ((pGui == NULL) || (pElem == NULL)) {
    fprintf(stderr,"ERROR: ElemAdd() called with NULL ptr\n");
    return NULL;
  }    

  // Fetch the page containing the item
  microSDL_tsPage*      pPage     = microSDL_PageFindById(pGui,nPageId);
  if (pPage == NULL) {
    fprintf(stderr,"ERROR: ElemAdd() page (ID=%d) was not found\n",nPageId);
    return NULL;
  }   
  
  microSDL_tsCollect*   pCollect  = &pPage->sCollect;
  
  return microSDL_CollectElemAdd(pCollect,pElem);
}


void microSDL_ElemSetImage(microSDL_tsGui* pGui,microSDL_tsElem* pElem,const char* acImage,
  const char* acImageSel)
{
  if ((pGui == NULL) || (pElem == NULL)) {
    fprintf(stderr,"ERROR: ElemSetImage() called with NULL ptr\n");
    return;
  }    
  SDL_Surface*    pSurf;
  SDL_Surface*    pSurfGlow;

  if (strlen(acImage) > 0) {
    if (pElem->pSurf != NULL) {
      fprintf(stderr,"ERROR: ElemSetImage(%s) with pSurf already set\n",acImage);
      return;
    }
    pSurf = microSDL_LoadBmp(pGui,(char*)acImage);
    if (pSurf == NULL) {
      fprintf(stderr,"ERROR: ElemSetImage(%s) call to microSDL_LoadBmp failed\n",acImage);
      return;
    }
    pElem->pSurf = pSurf;
  }

  if (strlen(acImageSel) > 0) {
    if (pElem->pSurfGlow != NULL) {
      fprintf(stderr,"ERROR: ElemSetImage(%s) with pSurfGlow already set\n",acImageSel);
      return;
    }
    pSurfGlow = microSDL_LoadBmp(pGui,(char*)acImageSel);
    if (pSurfGlow == NULL) {
      fprintf(stderr,"ERROR: ElemSetImage(%s) call to microSDL_LoadBmp failed\n",acImageSel);
      return;
    }
    pElem->pSurfGlow = pSurfGlow;
  }

}




bool microSDL_ViewIndValid(microSDL_tsGui* pGui,int nViewInd)
{
  if (pGui == NULL) {
    fprintf(stderr,"ERROR: ViewIndValid() called with NULL ptr\n");
    return false;
  }    
  if ((nViewInd >= 0) && (nViewInd < (int)(pGui->nViewCnt))) {
    return true;
  } else {
    return false;
  }
}

// Search through the viewport array for a matching ID
int microSDL_ViewFindIndFromId(microSDL_tsGui* pGui,int nViewId)
{
  if (pGui == NULL) {
    fprintf(stderr,"ERROR: ViewFindIndFromId() called with NULL ptr\n");
    return MSDL_IND_NONE;
  }    
  unsigned  nInd;
  int nFound = MSDL_IND_NONE;
  for (nInd=0;nInd<pGui->nViewCnt;nInd++) {
    if (pGui->psView[nInd].nId == nViewId) {
      nFound = nInd;
    }
  }
  return nFound;
}

// Translate a coordinate from local to global according
// to the viewport region and origin.
void microSDL_ViewRemapPt(microSDL_tsGui* pGui,Sint16* pnX,Sint16* pnY)
{
  if (pGui == NULL) {
    fprintf(stderr,"ERROR: ViewRemapPt() called with NULL ptr\n");
    return;
  }    
  int nViewInd = pGui->nViewIndCur;
  if (nViewInd == MSDL_VIEW_IND_SCREEN) {
    return;
  }
  if (!microSDL_ViewIndValid(pGui,nViewInd)) {
    return;
  }
  int   nFinalX,nFinalY;
  int   nFrameX,nFrameY;
  int   nOriginX,nOriginY;
  nFrameX   = pGui->psView[nViewInd].rView.x;
  nFrameY   = pGui->psView[nViewInd].rView.y;
  nOriginX  = pGui->psView[nViewInd].nOriginX;
  nOriginY  = pGui->psView[nViewInd].nOriginY;

  nFinalX   = nFrameX + nOriginX + (*pnX);
  nFinalY   = nFrameY + nOriginY + (*pnY);

  *pnX = nFinalX;
  *pnY = nFinalY;
}

// Translate a rectangle's coordinates from local to global according
// to the viewport region and origin.
void microSDL_ViewRemapRect(microSDL_tsGui* pGui,SDL_Rect* prRect)
{
  if (pGui == NULL) {
    fprintf(stderr,"ERROR: ViewRemapRect() called with NULL ptr\n");
    return;
  }    
  // TODO: Check corrections +/- 1
  Sint16 nX0,nY0,nX1,nY1;
  nX0 = prRect->x;
  nY0 = prRect->y;
  nX1 = prRect->x + prRect->w - 1;
  nY1 = prRect->y + prRect->h - 1;
  microSDL_ViewRemapPt(pGui,&nX0,&nY0);
  microSDL_ViewRemapPt(pGui,&nX1,&nY1);
  prRect->x = nX0;
  prRect->y = nY0;
  prRect->w = nX1-nX0+1;
  prRect->h = nY1-nY0+1;
}


// Notifies an element of a touch event by invoking its touch callback
// function (if enabled) and passing in relative coordinates.
bool microSDL_NotifyElemTouch(microSDL_tsGui* pGui,microSDL_tsElem* pElem,
        microSDL_teTouch eTouch,int nX,int nY)
{
  if ((pGui == NULL) || (pElem == NULL)) {
    fprintf(stderr,"ERROR: NotifyElemTouch() called with NULL ptr\n");
    return false;
  }    
  
  // Fetch the extended element callback (if enabled)
  MSDL_CB_TOUCH pfuncXTouch = pElem->pfuncXTouch;
  if (pfuncXTouch != NULL) {
    // Pass in the relative position from corner of element region
    int nElemPosX = pElem->rElem.x;
    int nElemPosY = pElem->rElem.y;
    (*pfuncXTouch)((void*)(pGui),(void*)(pElem),eTouch,nX-nElemPosX,nY-nElemPosY);
  }
  
  return true;
}


// Initialize the element struct to all zeros
void microSDL_ResetElem(microSDL_tsElem* pElem)
{
  if (pElem == NULL) {
    fprintf(stderr,"ERROR: ResetElem() called with NULL ptr\n");
    return;
  }  
  pElem->bValid           = false;
  pElem->nId              = MSDL_ID_NONE;
  pElem->nType            = MSDL_TYPE_BOX;
  pElem->nGroup           = MSDL_GROUP_ID_NONE;
  pElem->rElem            = (SDL_Rect){0,0,0,0};
  pElem->bGlowing         = false;
  pElem->pSurf            = NULL;
  pElem->pSurfGlow        = NULL;
  pElem->bClickEn         = false;
  pElem->bFrameEn         = false;
  pElem->bFillEn          = false;
  pElem->bNeedRedraw      = true;
  pElem->colElemFrame     = MSDL_COL_WHITE;
  pElem->colElemFill      = MSDL_COL_WHITE;
  pElem->colElemGlow      = MSDL_COL_WHITE;

  pElem->acStr[0]         = '\0';
  pElem->colElemText      = MSDL_COL_WHITE;
  pElem->eTxtAlign        = MSDL_ALIGN_MID_MID;
  pElem->nTxtMargin       = 0;
  pElem->pTxtFont         = NULL;
  
  pElem->pXData           = NULL;
  pElem->pfuncXDraw       = NULL;
  pElem->pfuncXTouch      = NULL;
  pElem->pfuncXTick       = NULL;
  
  pElem->pElemParent      = NULL;

}

// Initialize the font struct to all zeros
void microSDL_ResetFont(microSDL_tsFont* pFont)
{
  if (pFont == NULL) {
    fprintf(stderr,"ERROR: ResetFont() called with NULL ptr\n");
    return;
  }    
  pFont->nId = MSDL_FONT_NONE;
  pFont->pFont = NULL;
}

// Initialize the view struct to all zeros
void microSDL_ResetView(microSDL_tsView* pView)
{
  if (pView == NULL) {
    fprintf(stderr,"ERROR: ResetView() called with NULL ptr\n");
    return;
  }    
  pView->nId = MSDL_VIEW_ID_NONE;
  pView->rView = (SDL_Rect){0,0,0,0};
  pView->nOriginX = 0;
  pView->nOriginY = 0;
}


// Close down an element
void microSDL_ElemDestruct(microSDL_tsElem* pElem)
{
  if (pElem == NULL) {
    fprintf(stderr,"ERROR: ElemDestruct() called with NULL ptr\n");
    return;
  }    
  if (pElem->pSurf != NULL) {
    SDL_FreeSurface(pElem->pSurf);
    pElem->pSurf = NULL;
  }
  if (pElem->pSurfGlow != NULL) {
    SDL_FreeSurface(pElem->pSurfGlow);
    pElem->pSurfGlow = NULL;
  }
  
  // TODO: Add callback function so that
  // we can support additional closure actions
  // (eg. closing sub-elements of compound element).
  
}


// Close down a collection
void microSDL_CollectDestruct(microSDL_tsCollect* pCollect)
{
  if (pCollect == NULL) {
    fprintf(stderr,"ERROR: CollectDestruct() called with NULL ptr\n");
    return;
  }
  unsigned          nElemInd;
  microSDL_tsElem*  pElem = NULL;

  for (nElemInd=0;nElemInd<pCollect->nElemCnt;nElemInd++) {
    pElem = &pCollect->asElem[nElemInd];
    microSDL_ElemDestruct(pElem);
  }
    
}

// Close down all in page
void microSDL_PageDestruct(microSDL_tsPage* pPage)
{
  if (pPage == NULL) {
    fprintf(stderr,"ERROR: PageDestruct() called with NULL ptr\n");
    return;
  }      
  microSDL_tsCollect* pCollect = &pPage->sCollect;
  microSDL_CollectDestruct(pCollect);
}

// Close down all GUI members, including pages and fonts
void microSDL_GuiDestruct(microSDL_tsGui* pGui)
{
  if (pGui == NULL) {
    fprintf(stderr,"ERROR: GuiDestruct() called with NULL ptr\n");
    return;
  }    
  // Loop through all pages in GUI
  int nPageInd;
  microSDL_tsPage*  pPage = NULL;
  for (nPageInd=0;nPageInd<pGui->nPageCnt;nPageInd++) {
    pPage = &pGui->asPage[nPageInd];
    microSDL_PageDestruct(pPage);
  }
  
  // TODO: Consider moving into main element array
  if (pGui->surfBkgnd != NULL) {
    SDL_FreeSurface(pGui->surfBkgnd);
    pGui->surfBkgnd = NULL;
  }
  
  // Close all fonts
  unsigned  nFontInd;
  for (nFontInd=0;nFontInd<pGui->nFontCnt;nFontInd++) {
    if (pGui->psFont[nFontInd].pFont != NULL) {
      TTF_CloseFont(pGui->psFont[nFontInd].pFont);
      pGui->psFont[nFontInd].pFont = NULL;
    }
  }
  pGui->nFontCnt = 0;
  TTF_Quit();
  
}


// Unfortunately, SDL has an issue wherein if a previous
// SDL program execution aborts before cleaning up properly (eg.
// with SDL_Quit) then the next time SDL_SetVideoMode
// is called, it may hang.
//
// The following code attempts to work around this sensitivity
// in SDL, making the SDL_SetVideoMode call more robust.
//
// DETAILS:
// - If an SDL program exits without first calling the cleanup
//   routines (ie. in SDL_Quit() ), the terminal may be left in
//   KD_GRAPHICS mode.
// - When another SDL program is started and attempts to call
//   SDL_SetVideoMode(), program execution may hang in
//   FB_EnterGraphicsMode() during the wait on switch to
//   KD_GRAPHICS mode. Since we are already in KD_GRAPHICS
//   this event never occurs.
// - This workaround unlocks the switch (which was set at the
//   end of a previous call to FB_EnterGraphicsMode) and then
//   forces the terminal to KD_TEXT mode.
// - By starting in text mode, we should then observe a VT
//   switch event as we attempt to transition to graphics mode
//   in the call to SDL_SetVideoMode().
bool microSDL_CleanStart(const char* sTTY)
{
#ifdef VT_WRK_EN    
    int     nFD = -1;
    int     nRet = false;
    
    nFD = open(sTTY, O_RDONLY, 0);
    if (nFD < 0) {
        fprintf(stderr, "ERROR: CleanStart() failed to open console (%s): %s\n",
            sTTY,strerror(errno));
        return false;
    }
    nRet = ioctl(nFD, VT_UNLOCKSWITCH, 1);
    if (nRet != false) {
         fprintf(stderr, "ERROR: CleanStart() failed to unlock console (%s): %s\n",
            sTTY,strerror(errno));
        return false;
    }
    nRet = ioctl(nFD, KDSETMODE, KD_TEXT);
    if (nRet != 0) {
         fprintf(stderr, "ERROR: CleanStart() failed to set text mode (%s): %s\n",
            sTTY,strerror(errno));
        return false;
    }
    close(nFD);
#endif    
    return true;
}


// ================================
// Private: Element Collection

void microSDL_CollectReset(microSDL_tsCollect* pCollect,microSDL_tsElem* asElem,unsigned nElemMax)
{
  if (pCollect == NULL) {
    fprintf(stderr,"ERROR: CollectReset() called with NULL ptr\n");
    return;
  }  
  pCollect->pElemParent       = NULL;
  
  pCollect->nElemMax          = nElemMax;
  pCollect->nElemCnt          = MSDL_IND_FIRST;
  
  pCollect->nElemAutoIdNext   = MSDL_ID_AUTO_BASE;
  
  pCollect->pElemTracked      = NULL;
  
  // Save the pointer to the element array
  pCollect->asElem = asElem;
  
  int nInd;
  for (nInd=0;nInd<nElemMax;nInd++) {
    microSDL_ResetElem(&(pCollect->asElem[nInd]));
  }
}

// Note that this copies the content of pElem to collection element array
// so the pointer can disappear after the call
microSDL_tsElem* microSDL_CollectAdd(microSDL_tsCollect* pCollect,microSDL_tsElem* pElem)
{
  if ((pCollect == NULL) || (pElem == NULL)) {
    fprintf(stderr,"ERROR: CollectAdd() called with NULL ptr\n");
    return NULL;
  }
  if (pCollect->nElemCnt+1 >= (pCollect->nElemMax)) {
    fprintf(stderr,"ERROR: CollectAdd() too many elements\n");
    return NULL;
  }
  // In case the element creation failed, trap that here
  if (!pElem->bValid) {
    fprintf(stderr,"ERROR: CollectAdd() skipping add of invalid element\n");
    return NULL;
  }
  // Copy the element to the internal array
  int nElemInd = pCollect->nElemCnt;
  pCollect->asElem[nElemInd] = *pElem;
  pCollect->nElemCnt++;

  return &(pCollect->asElem[nElemInd]);  
}

microSDL_tsElem* microSDL_CollectFindElemById(microSDL_tsCollect* pCollect,int nElemId)
{
  if (pCollect == NULL) {
    fprintf(stderr,"ERROR: CollectFindElemById() called with NULL ptr\n");
    return NULL;
  }  
  microSDL_tsElem*  pFoundElem = NULL;
  unsigned          nInd;
  if (nElemId == MSDL_ID_TEMP) {
    // ERROR: Don't expect to do this
    fprintf(stderr,"ERROR: CollectFindElemById() searching for temp ID\n");    
    return NULL;
  }
  for (nInd=MSDL_IND_FIRST;nInd<pCollect->nElemCnt;nInd++) {
    if (pCollect->asElem[nInd].nId == nElemId) {
      pFoundElem = &(pCollect->asElem[nInd]);
      break;
    }
  }
  return pFoundElem;
}

int microSDL_CollectGetNextId(microSDL_tsCollect* pCollect)
{
  int nElemId = pCollect->nElemAutoIdNext;
  pCollect->nElemAutoIdNext++;
  return nElemId;
}

microSDL_tsElem* microSDL_CollectGetElemTracked(microSDL_tsCollect* pCollect)
{
  return pCollect->pElemTracked;
}

void microSDL_CollectSetElemTracked(microSDL_tsCollect* pCollect,microSDL_tsElem* pElem)
{
  pCollect->pElemTracked = pElem;
}


// Redraw the element collection
// - Only the elements that have been marked as needing redraw
//   are rendered.

// NOTE: The following code was based on PageRedrawGo()
// - However, all page logic was removed
// - Transparency detect was removed
// TODO: How to handle background or transparency?
void microSDL_CollectRedraw(microSDL_tsGui* pGui,microSDL_tsCollect* pCollect,bool bRedrawAll)
{
  unsigned          nInd;
  microSDL_tsElem*  pElem = NULL;
  // Draw elements (as needed)
  for (nInd=MSDL_IND_FIRST;nInd<pCollect->nElemCnt;nInd++) {
    pElem = &(pCollect->asElem[nInd]);       
    if ((pElem->bNeedRedraw) || (bRedrawAll)) {      
      // Draw the element
      // - Note that this also clears the redraw flag
      microSDL_ElemDrawByRef(pGui,pElem);        
    }
  }
}

void microSDL_CollectTick(microSDL_tsGui* pGui,microSDL_tsCollect* pCollect)
{
  unsigned          nInd;
  microSDL_tsElem*  pElem = NULL;
  // Loop through elements in collection and issue tick callback
  for (nInd=MSDL_IND_FIRST;nInd<pCollect->nElemCnt;nInd++) {
    pElem = &(pCollect->asElem[nInd]);
    if (pElem->pfuncXTick != NULL) {
      (*pElem->pfuncXTick)((void*)(pGui),(void*)(pElem));
    }      
  }  
}

// Find an element index in a collection from a coordinate
int microSDL_CollectFindIndFromCoord(microSDL_tsCollect* pCollect,int nX, int nY)
{
  unsigned  nInd;
  bool      bFound = false;
  int       nFoundInd = MSDL_IND_NONE;

  #ifdef DBG_TOUCH
  // Highlight current touch for coordinate debug
  SDL_Rect    rMark = microSDL_ExpandRect((SDL_Rect){(Sint16)nX,(Sint16)nY,1,1},1,1);
  microSDL_FrameRect(pGui,rMark,MSDL_COL_YELLOW);
  printf("    ElemFindFromCoord(%3d,%3d):\n",nX,nY);
  #endif

  for (nInd=MSDL_IND_FIRST;nInd<pCollect->nElemCnt;nInd++) {

    // Are we within the rect?
    if (microSDL_IsInRect(nX,nY,pCollect->asElem[nInd].rElem)) {
      #ifdef DBG_TOUCH
      printf("      [%3u]:In  ",nInd);
      #endif

      // Only return element index if clickable
      if (pCollect->asElem[nInd].bClickEn) {
          if (bFound) {
              fprintf(stderr,"WARNING: Overlapping clickable regions detected\n");
          }
          bFound = true;  
          nFoundInd = nInd;
          #ifdef DBG_TOUCH
          printf("En \n");
          #endif
          // No more search to do
          break;
      } else {
          #ifdef DBG_TOUCH
          printf("Dis\n");
          #endif
      }

    } // microSDL_IsInRect()
    else {
      #ifdef DBG_TOUCH
      printf("      [%3u]:Out\n",nInd);
      #endif
    }
  }

  // Return index or MSDL_IND_NONE if none found
  return nFoundInd;
}

// Find an element index in a collection from a coordinate
microSDL_tsElem* microSDL_CollectFindElemFromCoord(microSDL_tsCollect* pCollect,int nX, int nY)
{
  unsigned  nInd;
  bool      bFound = false;
  microSDL_tsElem*  pElem = NULL;
  microSDL_tsElem*  pFoundElem = NULL;

  #ifdef DBG_TOUCH
  // Highlight current touch for coordinate debug
  SDL_Rect    rMark = microSDL_ExpandRect((SDL_Rect){(Sint16)nX,(Sint16)nY,1,1},1,1);
  microSDL_FrameRect(pGui,rMark,MSDL_COL_YELLOW);
  printf("    CollectFindElemFromCoord(%3d,%3d):\n",nX,nY);
  #endif

  for (nInd=MSDL_IND_FIRST;nInd<pCollect->nElemCnt;nInd++) {

    pElem = &pCollect->asElem[nInd];
    
    // Are we within the rect?
    if (microSDL_IsInRect(nX,nY,pElem->rElem)) {
      #ifdef DBG_TOUCH
      printf("      [%3u]:In  ",nInd);
      #endif

      // Only return element index if clickable
      if (pElem->bClickEn) {
          if (bFound) {
              fprintf(stderr,"WARNING: Overlapping clickable regions detected\n");
          }
          bFound = true;
          // NOTE: If multiple hits were found, the last one in the
          // collection will be returned
          pFoundElem = pElem;
          #ifdef DBG_TOUCH
          printf("En \n");
          #endif
          // No more search to do
          break;
      } else {
          #ifdef DBG_TOUCH
          printf("Dis\n");
          #endif
      }

    } // microSDL_IsInRect()
    else {
      #ifdef DBG_TOUCH
      printf("      [%3u]:Out\n",nInd);
      #endif
    }
  }

  // Return pointer or NULL if none found
  return pFoundElem;
}

// Go through all elements in a collection and set the parent
// element pointer.
void microSDL_CollectSetParent(microSDL_tsCollect* pCollect,microSDL_tsElem* pElemParent)
{
  microSDL_tsElem*  pElem = NULL;
  int               nInd;
  for (nInd=MSDL_IND_FIRST;nInd<pCollect->nElemCnt;nInd++) {
    pElem = &pCollect->asElem[nInd];
    pElem->pElemParent = pElemParent;
  }
}