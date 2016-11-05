// =======================================================================
// microSDL library
// - Calvin Hass
// - http:/www.impulseadventure.com/elec/microsdl-sdl-gui.html
//
// - Version 0.3.3    (2016/11/04)
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
#define MICROSDL_VER "0.3.2"

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

bool microSDL_Init(microSDL_tsGui* pGui,microSDL_tsElem* psElem,unsigned nMaxElem,microSDL_tsFont* psFont,unsigned nMaxFont,microSDL_tsView* psView,unsigned nMaxView)
{
  unsigned  nInd;
  
  // Initialize state

  // The page that is currently active
  pGui->nPageIdCur  = MSDL_PAGE_NONE;
  
  // Initialize collection of fonts with user-supplied pointer
  pGui->psFont      = psFont;
  pGui->nFontMax    = nMaxFont;
  pGui->nFontCnt    = 0;
  for (nInd=0;nInd<(pGui->nFontMax);nInd++) {
    microSDL_ResetFont(&(pGui->psFont[nInd]));
  }

  // Initialize collection of elements with user-supplied pointer
  pGui->psElem          = psElem;
  pGui->nElemMax        = nMaxElem;
  pGui->nElemCnt        = MSDL_IND_FIRST;
  pGui->nElemAutoIdNext = MSDL_ID_AUTO_BASE;
  for (nInd=0;nInd<(pGui->nElemMax);nInd++) {
    microSDL_ResetElem(&(pGui->psElem[nInd]));
  }

  // Initialize collection of views with user-supplied pointer
  pGui->psView      = psView;
  pGui->nViewMax    = nMaxView;
  pGui->nViewCnt    = 0;
  pGui->nViewIndCur = MSDL_VIEW_IND_SCREEN;
  for (nInd=0;nInd<(pGui->nViewMax);nInd++) {
    microSDL_ResetView(&(pGui->psView[nInd]));
  }

  // Element currently started tracking
  pGui->nTrackElemIdStart     = MSDL_ID_NONE;
  
  // Last element clicked
  pGui->nTrackElemIdClicked   = MSDL_ID_NONE;
  pGui->nClickLastX           = 0;
  pGui->nClickLastY           = 0;
  pGui->nClickLastPress       = 0;

  // Touchscreen library interface
  #ifdef INC_TS
  pGui->ts = NULL;
  #endif

  // Redraw
  pGui->bPageNeedRedraw = true;
  pGui->bPageNeedFlip = true;
  
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
  // Close all elements
  microSDL_ElemCloseAll(pGui);

  // Close all fonts
  microSDL_FontCloseAll(pGui);

  // Close down SDL
  SDL_Quit();
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


bool microSDL_IsInRect(microSDL_tsGui* pGui,int nSelX,int nSelY,SDL_Rect rRect)
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


void microSDL_Flip(microSDL_tsGui* pGui)
{
  SDL_Flip( pGui->surfScreen );
  
  // Page flip no longer required
  microSDL_PageFlipSet(pGui,false);
}

void microSDL_PageFlipSet(microSDL_tsGui* pGui,bool bNeeded)
{
  pGui->bPageNeedFlip = bNeeded;
}

bool microSDL_PageFlipGet(microSDL_tsGui* pGui)
{
  return pGui->bPageNeedFlip;
}

void microSDL_PageFlipGo(microSDL_tsGui* pGui)
{
  if (pGui->bPageNeedFlip) {
    SDL_Flip(pGui->surfScreen);
    
    // Indicate that page flip is no longer required
    microSDL_PageFlipSet(pGui,false);
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


// -----------------------------------------------------------------------
// Font Functions
// -----------------------------------------------------------------------

bool microSDL_FontAdd(microSDL_tsGui* pGui,int nFontId,const char* acFontName,unsigned nFontSz)
{
  if (pGui->nFontCnt+1 >= (pGui->nFontMax)) {
    fprintf(stderr,"ERROR: microSDL_FontAdd() added too many fonts\n");
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

int microSDL_GetPageCur(microSDL_tsGui* pGui)
{
  return pGui->nPageIdCur;
}


void microSDL_SetPageCur(microSDL_tsGui* pGui,int nPageId)
{
  int nPageSaved = pGui->nPageIdCur;
  pGui->nPageIdCur = nPageId;
  
  // A change of page should always force a future redraw
  if (nPageSaved != nPageId) {
    microSDL_PageRedrawSet(pGui,true);
  }
}


// Adjust the flag that indicates whether the entire page
// requires a redraw.
void microSDL_PageRedrawSet(microSDL_tsGui* pGui,bool bRedraw)
{
  pGui->bPageNeedRedraw = bRedraw;
}

bool microSDL_PageRedrawGet(microSDL_tsGui* pGui)
{
  return pGui->bPageNeedRedraw;
}

// Redraw the active page
// - If the page has been marked as needing redraw, then all
//   elements are rendered
// - If the page has not been marked as needing redraw then only
//   the elements that have been marked as needing redraw
//   are rendered.
//   - A further check is done to see if any elements needing
//     redraw are semi-transparent. If so, a complete redraw is
//     performed. Later optimization could support a more advanced
//     search of lower-level overlapping elements that may be
//     visible and hence should be rendered.
void microSDL_PageRedrawGo(microSDL_tsGui* pGui)
{
  unsigned  nInd;
  
  // Determine what page we are on as we only want to render
  // elements that are on the current page (or marked as visible
  // on all pages via MSDL_PAGE_ALL)
  int       nPageId = pGui->nPageIdCur;
  
  // Start by determining if there are any elements needing redraw
  // that are transparent (ie. bFillEn=false). If so, force an entire
  // page redraw.
  // - TODO: Optimize to figure out which other elements below need redraw.

  // If bPageNeedRedraw is already set, then we can skip PASS 1
  // by setting the flag here which will skip the PASS 1 search
  
  // PASS 1: Search for transparency
  //         Find any transparent elements that should cause
  //         a page redraw
  microSDL_tsElem*  pElem;
  bool  bPageRedraw = pGui->bPageNeedRedraw;
  for (nInd=MSDL_IND_FIRST;(!bPageRedraw)&&(nInd<pGui->nElemCnt);nInd++) {
    pElem = &(pGui->psElem[nInd]);
    if ( (pElem->nPage == nPageId) ||
         (pElem->nPage == MSDL_PAGE_ALL) ) {
      if (pElem->bNeedRedraw) {
        if (!pElem->bFillEn) {
          bPageRedraw = true;
        }
      }
    }
  }
   
  // PASS 2: Perform redraw
  
  // Draw background (only if full-page redraw)
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
  for (nInd=MSDL_IND_FIRST;nInd<pGui->nElemCnt;nInd++) {
    pElem = &(pGui->psElem[nInd]);    
    if ( (pElem->nPage == nPageId) ||
         (pElem->nPage == MSDL_PAGE_ALL) ) {
      if ((pElem->bNeedRedraw) || (bPageRedraw)) {      
        // Draw the element
        // - Note that this also clears the redraw flag
        microSDL_ElemDrawByInd(pGui,nInd);
      }
    }
  }

  // Clear the page redraw flag
  microSDL_PageRedrawSet(pGui,false);
  
  // Page flip the entire screen
  // - TODO: We could also call Update instead of Flip as that would
  //         limit the region to refresh.
  microSDL_PageFlipGo(pGui);
  
}


// ------------------------------------------------------------------------
// Element General Functions
// ------------------------------------------------------------------------

// Search through the element array for a matching ID
// - Special handling of search for MSDL_ID_TEMP which
//   always returns MSDL_IND_TEMP (equal to index 0).
//   This is done because the temporary element will
//   have its own ID assigned in nId and not MSDL_ID_TEMP.
int microSDL_ElemFindIndFromId(microSDL_tsElem* asElem,unsigned nNumElem,int nElemId)
{
  unsigned  nInd;
  int       nFound = MSDL_IND_NONE;
  if (nElemId == MSDL_ID_TEMP) {
    return MSDL_IND_TEMP;
  }
  for (nInd=MSDL_IND_FIRST;nInd<nNumElem;nInd++) {
    if (asElem[nInd].nId == nElemId) {
      nFound = nInd;
    }
  }
  return nFound;
}

int microSDL_ElemFindIndFromCoord(microSDL_tsGui* pGui,int nX, int nY)
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

  for (nInd=MSDL_IND_FIRST;nInd<pGui->nElemCnt;nInd++) {

    // Ensure this element is visible on this page!
    if ((pGui->psElem[nInd].nPage == pGui->nPageIdCur) ||
      (pGui->psElem[nInd].nPage == MSDL_PAGE_ALL)) {
      // Are we within the rect?
      if (microSDL_IsInRect(pGui,nX,nY,pGui->psElem[nInd].rElem)) {
        #ifdef DBG_TOUCH
        printf("      [%3u]:In  ",nInd);
        #endif

        // Only return element index if clickable
        if (pGui->psElem[nInd].bClickEn) {
            if (bFound) {
                fprintf(stderr,"WARNING: Overlapping clickable regions detected\n");
            }
            bFound = true;  
            nFoundInd = nInd;
            #ifdef DBG_TOUCH
            printf("En \n");
            #endif
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
    } // nPage
  }

  // Return index or MSDL_IND_NONE if none found
  return nFoundInd;
}


int microSDL_ElemGetIdFromElem(microSDL_tsGui* pGui,microSDL_tsElem* pElem)
{
  return pElem->nId;
}

// ------------------------------------------------------------------------
// Element Creation Functions
// ------------------------------------------------------------------------


int microSDL_ElemCreateTxt(microSDL_tsGui* pGui,int nElemId,int nPage,SDL_Rect rElem,
  const char* pStr,int nFontId)
{
  microSDL_tsElem sElem;
  bool            bOk = true;
  sElem = microSDL_ElemCreate(pGui,nElemId,nPage,MSDL_TYPE_TXT,rElem,pStr,nFontId);
  sElem.colElemFill     = MSDL_COL_BLACK;
  sElem.colElemFrame    = MSDL_COL_BLACK;
  sElem.colElemGlow     = MSDL_COL_BLACK;
  sElem.colElemText     = MSDL_COL_YELLOW;
  sElem.bFillEn         = true;
  sElem.eTxtAlign       = MSDL_ALIGN_MID_LEFT;
  if (nPage != MSDL_PAGE_NONE) {
    bOk = microSDL_ElemAdd(pGui,sElem);
  } else {
    // Save as temporary element
    pGui->psElem[MSDL_IND_TEMP] = sElem;
  }
  return (bOk)? sElem.nId : MSDL_ID_NONE;
}

int microSDL_ElemCreateBtnTxt(microSDL_tsGui* pGui,int nElemId,int nPage,
  SDL_Rect rElem,const char* acStr,int nFontId)
{
  microSDL_tsElem sElem;
  bool            bOk = true;
  sElem.bValid = false;

  // Ensure the Font is loaded
  TTF_Font* pFont = microSDL_FontGet(pGui,nFontId);
  if (pFont == NULL) {
    fprintf(stderr,"ERROR: microSDL_ElemCreateBtnTxt(ID=%d): Font(ID=%d) not loaded\n",nElemId,nFontId);
    sElem.bValid = false;
    return MSDL_ID_NONE;
  }

  sElem = microSDL_ElemCreate(pGui,nElemId,nPage,MSDL_TYPE_BTN,rElem,acStr,nFontId);
  sElem.colElemFill     = MSDL_COL_BLUE_LT;
  sElem.colElemFrame    = MSDL_COL_BLUE_DK;
  sElem.colElemGlow     = MSDL_COL_GREEN_LT;
  sElem.colElemText     = MSDL_COL_BLACK;
  sElem.bFrameEn        = true;
  sElem.bFillEn         = true;
  sElem.bClickEn        = true;
  if (nPage != MSDL_PAGE_NONE) {  
    bOk = microSDL_ElemAdd(pGui,sElem);
  } else {
    // Save as temporary element
    pGui->psElem[MSDL_IND_TEMP] = sElem;
  }
  return (bOk)? sElem.nId : MSDL_ID_NONE;
}

int microSDL_ElemCreateBtnImg(microSDL_tsGui* pGui,int nElemId,int nPage,
  SDL_Rect rElem,const char* acImg,const char* acImgSel)
{
  microSDL_tsElem sElem;
  bool            bOk = true;
  sElem = microSDL_ElemCreate(pGui,nElemId,nPage,MSDL_TYPE_BTN,rElem,"",MSDL_FONT_NONE);
  sElem.colElemFill     = MSDL_COL_BLUE_LT;
  sElem.colElemFrame    = MSDL_COL_BLUE_DK;
  sElem.colElemGlow     = MSDL_COL_GREEN_LT;
  sElem.colElemText     = MSDL_COL_BLACK;
  sElem.bFrameEn        = false;
  sElem.bFillEn         = false;
  sElem.bClickEn        = true;
  microSDL_ElemSetImage(pGui,&sElem,acImg,acImgSel);
  if (nPage != MSDL_PAGE_NONE) {  
    bOk = microSDL_ElemAdd(pGui,sElem);
  } else {
    // Save as temporary element
    pGui->psElem[MSDL_IND_TEMP] = sElem;
  }
  return (bOk)? sElem.nId : MSDL_ID_NONE;
}

int microSDL_ElemCreateBox(microSDL_tsGui* pGui,int nElemId,int nPage,SDL_Rect rElem)
{
  microSDL_tsElem sElem;
  bool            bOk = true;
  sElem = microSDL_ElemCreate(pGui,nElemId,nPage,MSDL_TYPE_BOX,rElem,NULL,MSDL_FONT_NONE);
  sElem.colElemFill     = MSDL_COL_BLACK;
  sElem.colElemFrame    = MSDL_COL_GRAY;
  sElem.bFillEn         = true;
  sElem.bFrameEn        = true;
  if (nPage != MSDL_PAGE_NONE) {  
    bOk = microSDL_ElemAdd(pGui,sElem);
  } else {
    // Save as temporary element
    pGui->psElem[MSDL_IND_TEMP] = sElem;
  }
  return (bOk)? sElem.nId : MSDL_ID_NONE;
}


int microSDL_ElemCreateImg(microSDL_tsGui* pGui,int nElemId,int nPage,
  SDL_Rect rElem,const char* acImg)
{
  microSDL_tsElem sElem;
  bool            bOk = true;
  sElem = microSDL_ElemCreate(pGui,nElemId,nPage,MSDL_TYPE_BOX,rElem,"",MSDL_FONT_NONE);
  sElem.bFrameEn        = false;
  sElem.bFillEn         = false;
  sElem.bClickEn        = false;
  microSDL_ElemSetImage(pGui,&sElem,acImg,acImg);
  if (nPage != MSDL_PAGE_NONE) {
    bOk = microSDL_ElemAdd(pGui,sElem);
  } else {
    // Save as temporary element
    pGui->psElem[MSDL_IND_TEMP] = sElem;
  }
  return (bOk)? sElem.nId : MSDL_ID_NONE;
}



// ------------------------------------------------------------------------
// Element Drawing Functions
// ------------------------------------------------------------------------

void microSDL_ElemDraw(microSDL_tsGui* pGui,int nElemId)
{
  int nElemInd = microSDL_ElemFindIndFromId(pGui->psElem,pGui->nElemCnt,nElemId);
  if (!microSDL_ElemIndValid(pGui,nElemInd)) {
    fprintf(stderr,"ERROR: ElemDraw() invalid ID=%d\n",nElemId);
    return;
  }
  microSDL_ElemDrawByInd(pGui,nElemInd);
}


// ------------------------------------------------------------------------
// Element Update Functions
// ------------------------------------------------------------------------

void microSDL_ElemSetFillEn(microSDL_tsGui* pGui,int nElemId,bool bFillEn)
{
  int nElemInd = microSDL_ElemFindIndFromId(pGui->psElem,pGui->nElemCnt,nElemId);  
  if (!microSDL_ElemIndValid(pGui,nElemInd)) {
    fprintf(stderr,"ERROR: ElemSetFillEn() invalid ID=%d\n",nElemId);
    return;
  }
  microSDL_tsElem* pElem  = &pGui->psElem[nElemInd];
  pElem->bFillEn          = bFillEn;  
  pElem->bNeedRedraw      = true;  
}

void microSDL_ElemSetCol(microSDL_tsGui* pGui,int nElemId,SDL_Color colFrame,SDL_Color colFill,SDL_Color colGlow)
{
  int nElemInd = microSDL_ElemFindIndFromId(pGui->psElem,pGui->nElemCnt,nElemId);  
  if (!microSDL_ElemIndValid(pGui,nElemInd)) {
    fprintf(stderr,"ERROR: ElemSetCol() invalid ID=%d\n",nElemId);
    return;
  }
  microSDL_tsElem* pElem  = &pGui->psElem[nElemInd];
  pElem->colElemFrame     = colFrame;
  pElem->colElemFill      = colFill;
  pElem->colElemGlow      = colGlow;
  pElem->bNeedRedraw      = true;  
}



void microSDL_ElemSetTxtAlign(microSDL_tsGui* pGui,int nElemId,unsigned nAlign)
{
  int nElemInd = microSDL_ElemFindIndFromId(pGui->psElem,pGui->nElemCnt,nElemId);  
  if (!microSDL_ElemIndValid(pGui,nElemInd)) {
    fprintf(stderr,"ERROR: ElemSetTxtAlign() invalid ID=%d\n",nElemId);
    return;
  }
  microSDL_tsElem* pElem  = &pGui->psElem[nElemInd];
  pElem->eTxtAlign        = nAlign;
  pElem->bNeedRedraw      = true;  
}


void microSDL_ElemSetTxtStr(microSDL_tsGui* pGui,int nElemId,const char* pStr)
{
  int nElemInd = microSDL_ElemFindIndFromId(pGui->psElem,pGui->nElemCnt,nElemId);  
  if (!microSDL_ElemIndValid(pGui,nElemInd)) {
    fprintf(stderr,"ERROR: ElemSetTxtStr() invalid ID=%d\n",nElemId);
    return;
  }
  microSDL_tsElem* pElem  = &pGui->psElem[nElemInd];
  strncpy(pElem->acStr,pStr,MSDL_ELEM_STRLEN_MAX);
  pElem->bNeedRedraw      = true;
}

void microSDL_ElemSetTxtCol(microSDL_tsGui* pGui,int nElemId,SDL_Color colVal)
{
  int nElemInd = microSDL_ElemFindIndFromId(pGui->psElem,pGui->nElemCnt,nElemId);  
  if (!microSDL_ElemIndValid(pGui,nElemInd)) {
    fprintf(stderr,"ERROR: ElemSetTxtCol() invalid ID=%d\n",nElemId);
    return;
  }
  microSDL_tsElem* pElem  = &pGui->psElem[nElemInd];
  pElem->colElemText      = colVal;
  pElem->bNeedRedraw      = true;  
}

void microSDL_ElemUpdateFont(microSDL_tsGui* pGui,int nElemId,int nFontId)
{
  int nElemInd = microSDL_ElemFindIndFromId(pGui->psElem,pGui->nElemCnt,nElemId);  
  if (!microSDL_ElemIndValid(pGui,nElemInd)) {
    fprintf(stderr,"ERROR: ElemUpdateFont() invalid ID=%d\n",nElemId);
    return;
  }
  microSDL_tsElem* pElem  = &pGui->psElem[nElemInd];
  pElem->pTxtFont         = microSDL_FontGet(pGui,nFontId);
  pElem->bNeedRedraw      = true;  
}

void microSDL_ElemSetRedraw(microSDL_tsGui* pGui,int nElemId,bool bRedraw)
{
  int nElemInd = microSDL_ElemFindIndFromId(pGui->psElem,pGui->nElemCnt,nElemId);  
  if (!microSDL_ElemIndValid(pGui,nElemInd)) {
    fprintf(stderr,"ERROR: ElemSetRedraw() invalid ID=%d\n",nElemId);
    return;
  }
  microSDL_tsElem* pElem  = &pGui->psElem[nElemInd];
  pElem->bNeedRedraw      = bRedraw;
}

void microSDL_ElemSetGlow(microSDL_tsGui* pGui,int nElemId,bool bGlowing)
{
  int nElemInd = microSDL_ElemFindIndFromId(pGui->psElem,pGui->nElemCnt,nElemId);  
  if (!microSDL_ElemIndValid(pGui,nElemInd)) {
    fprintf(stderr,"ERROR: microSDL_ElemSetGlow() invalid ID=%d\n",nElemId);
    return;
  }
  microSDL_tsElem* pElem  = &pGui->psElem[nElemInd];
  pElem->bGlowing         = bGlowing;
  pElem->bNeedRedraw      = true;
}

bool microSDL_ElemGetGlow(microSDL_tsGui* pGui,int nElemId)
{
  int nElemInd = microSDL_ElemFindIndFromId(pGui->psElem,pGui->nElemCnt,nElemId);  
  if (!microSDL_ElemIndValid(pGui,nElemInd)) {
    fprintf(stderr,"ERROR: microSDL_ElemGetGlow() invalid ID=%d\n",nElemId);
    return false;
  }
  microSDL_tsElem* pElem  = &pGui->psElem[nElemInd];
  return pElem->bGlowing;
}

void microSDL_ElemSetDrawFunc(microSDL_tsGui* pGui,int nElemId,MSDL_CB_DRAW funcCb)
{
  int nElemInd = microSDL_ElemFindIndFromId(pGui->psElem,pGui->nElemCnt,nElemId);  
  if (!microSDL_ElemIndValid(pGui,nElemInd)) {
    fprintf(stderr,"ERROR: ElemSetDrawFunc() invalid ID=%d\n",nElemId);
    return;
  }
  microSDL_tsElem* pElem  = &pGui->psElem[nElemInd];
  pElem->pfuncXDraw       = funcCb;
  pElem->bNeedRedraw      = true;    
}


// ------------------------------------------------------------------------
// Viewport Functions
// ------------------------------------------------------------------------

int microSDL_ViewCreate(microSDL_tsGui* pGui,int nViewId,SDL_Rect rView,unsigned nOriginX,unsigned nOriginY)
{
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

int microSDL_GetTrackElemClicked(microSDL_tsGui* pGui)
{
  return pGui->nTrackElemIdClicked;
}


void microSDL_ClearTrackElemClicked(microSDL_tsGui* pGui)
{
  pGui->nTrackElemIdClicked = MSDL_ID_NONE;  
}

void microSDL_TrackClick(microSDL_tsGui* pGui,int nX,int nY,unsigned nPress)
{
  #ifdef DBG_TOUCH
  printf(" TS : (%3d,%3d) Pressure=%3u\n",nX,nY,nPress);
  #endif

  if ((pGui->nClickLastPress == 0) && (nPress > 0)) {
    // TouchDown
    microSDL_TrackTouchDownClick(pGui,nX,nY);
  } else if ((pGui->nClickLastPress > 0) && (nPress == 0)) {
    // TouchUp
    microSDL_TrackTouchUpClick(pGui,nX,nY);
  } else if ((pGui->nClickLastX != nX) || (pGui->nClickLastY != nY)) {
    // TouchMove
    // Only track movement if touch is down
    if (nPress>0) {
      // TouchDownMove
      microSDL_TrackTouchDownMove(pGui,nX,nY);
    }
  }

  // Save sample
  pGui->nClickLastX      = nX;
  pGui->nClickLastY      = nY;
  pGui->nClickLastPress  = nPress;

}

bool microSDL_GetSdlClick(microSDL_tsGui* pGui,int* pnX,int* pnY,unsigned* pnPress)
{
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
  // TODO: Consider using env "TSLIB_TSDEVICE" instead
  //char* pDevName = NULL;
  //pDevName = getenv("TSLIB_TSDEVICE");
  //pGui->ts = ts_open(pDevName,1);
  
  // Open in non-blocking mode
  pGui->ts = ts_open(acDev,1);
  if (!pGui->ts) {
    fprintf(stderr,"ERROR: microSDL_TsOpen\n");
    return false;
  }

  if (ts_config(pGui->ts)) {
    fprintf(stderr,"ERROR: microSDL_TsConfig\n");
    return false;
  }
  return true;
}

int microSDL_GetTsClick(microSDL_tsGui* pGui,int* pnX,int* pnY,unsigned* pnPress)
{
  struct ts_sample   pSamp;
  int nRet = ts_read(pGui->ts,&pSamp,1);
  (*pnX) = pSamp.x;
  (*pnY) = pSamp.y;
  (*pnPress) = pSamp.pressure;
  return nRet;
}

#endif // INC_TS



// ------------------------------------------------------------------------
// Private Functions
// ------------------------------------------------------------------------

Uint32 microSDL_GenPixelColor(microSDL_tsGui* pGui,SDL_Color nCol)
{
  return SDL_MapRGB(pGui->surfScreen->format,nCol.r,nCol.g,nCol.b);
}


Uint32 microSDL_GetPixelRaw(microSDL_tsGui* pGui,SDL_Surface *surf, int nX, int nY)
{
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
void microSDL_PutPixelRaw(microSDL_tsGui* pGui,SDL_Surface *surf, int nX, int nY, Uint32 nPixelVal)
{
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
  microSDL_PutPixelRaw(pGui,pGui->surfScreen,nX,nY,nPixelCol);
}


bool microSDL_Lock(microSDL_tsGui* pGui)
{
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
  if (SDL_MUSTLOCK(pGui->surfScreen)) {
    SDL_UnlockSurface(pGui->surfScreen);
  }
}

// NOTE: nId is a positive ID specified by the user or
//       MSDL_ID_AUTO if the user wants an auto-generated ID
//       (which will be assigned in Element nId)
microSDL_tsElem microSDL_ElemCreate(microSDL_tsGui* pGui,int nElemId,int nPageId,
  microSDL_teType nType,SDL_Rect rElem,const char* pStr,int nFontId)
{
  microSDL_tsElem sElem;
  sElem.bValid = false;

  // Validate the user-supplied ID
  if (nElemId == MSDL_ID_AUTO) {
    // Autogenerated ID requested
    nElemId = pGui->nElemAutoIdNext;
    pGui->nElemAutoIdNext++;
  } else {
    // Ensure the ID is positive
    if (nElemId < 0) {
      printf("ERROR: microSDL_ElemCreate() called with negative ID (%d)\n",nElemId);
      return sElem;
    }
    // Ensure the ID isn't already taken
    if (microSDL_ElemFindIndFromId(pGui->psElem,pGui->nElemCnt,nElemId) != MSDL_IND_NONE) {
      printf("ERROR: microSDL_Create() called with existing ID (%d)\n",nElemId);
      return sElem;
    }
  }

  // Assign defaults to the element record
  // TODO: Use ResetElem()?
  sElem.nId             = nElemId;
  sElem.nPage           = nPageId;
  sElem.rElem           = rElem;
  sElem.nType           = nType;
  sElem.bGlowing        = false;
  sElem.pTxtFont        = microSDL_FontGet(pGui,nFontId);
  sElem.pSurf           = NULL;
  sElem.pSurfGlow       = NULL; 
  sElem.colElemFill     = MSDL_COL_BLACK;
  sElem.colElemFrame    = MSDL_COL_BLUE_DK;
  sElem.colElemGlow     = MSDL_COL_BLACK;
  sElem.colElemText     = MSDL_COL_YELLOW;
  if (pStr != NULL) {
    strncpy(sElem.acStr,pStr,MSDL_ELEM_STRLEN_MAX);
  } else {
    sElem.acStr[0] = '\0';
  }

  // Assign defaults
  sElem.eTxtAlign       = MSDL_ALIGN_MID_MID;
  sElem.nTxtMargin      = 5;
  sElem.bFillEn         = false;
  sElem.bFrameEn        = false;
  sElem.bClickEn        = false;

  sElem.pXData          = NULL;
  sElem.pfuncXDraw      = NULL;
  sElem.pfuncXTouch     = NULL;
  
  sElem.bNeedRedraw     = true; // Start with redraw
  
  // If the element creation was successful, then set the valid flag
  sElem.bValid          = true;

  return sElem;
}

bool microSDL_ElemAdd(microSDL_tsGui* pGui,microSDL_tsElem sElem)
{
  if (pGui->nElemCnt+1 >= (pGui->nElemMax)) {
    fprintf(stderr,"ERROR: microSDL_ElemAdd() too many elements\n");
    return false;
  }
  // In case the element creation failed, trap that here
  if (!sElem.bValid) {
    fprintf(stderr,"ERROR: microSDL_ElemAdd() skipping add of invalid element\n");
    return false;
  }
  // Add the element to the internal array
  pGui->psElem[pGui->nElemCnt] = sElem;
  pGui->nElemCnt++;

  return true;
}


bool microSDL_ElemIndValid(microSDL_tsGui* pGui,int nElemInd)
{
  if ((nElemInd >= 0) && (nElemInd < (int)(pGui->nElemCnt))) {
    return true;
  } else {
    return false;
  }
}

int microSDL_ElemGetIdFromInd(microSDL_tsGui* pGui,int nElemInd)
{
  if (microSDL_ElemIndValid(pGui,nElemInd)) {
    return pGui->psElem[nElemInd].nId;
  } else {
    return MSDL_ID_NONE;
  }
}

microSDL_tsElem microSDL_ElemGetTemp(microSDL_tsGui* pGui)
{
  return pGui->psElem[MSDL_IND_TEMP];
}

void microSDL_ElemSetImage(microSDL_tsGui* pGui,microSDL_tsElem* pElem,const char* acImage,
  const char* acImageSel)
{
  SDL_Surface*    pSurf;
  SDL_Surface*    pSurfGlow;
  if (pElem == NULL) {
    // ERROR
    return;
  }

  if (strlen(acImage) > 0) {
    if (pElem->pSurf != NULL) {
      fprintf(stderr,"ERROR: microSDL_ElemSetImage(%s) with pSurf already set\n",acImage);
      return;
    }
    pSurf = microSDL_LoadBmp(pGui,(char*)acImage);
    if (pSurf == NULL) {
      fprintf(stderr,"ERROR: microSDL_ElemSetImage(%s) call to microSDL_LoadBmp failed\n",acImage);
      return;
    }
    pElem->pSurf = pSurf;
  }

  if (strlen(acImageSel) > 0) {
    if (pElem->pSurfGlow != NULL) {
      fprintf(stderr,"ERROR: microSDL_ElemSetImage(%s) with pSurfGlow already set\n",acImageSel);
      return;
    }
    pSurfGlow = microSDL_LoadBmp(pGui,(char*)acImageSel);
    if (pSurfGlow == NULL) {
      fprintf(stderr,"ERROR: microSDL_ElemSetImage(%s) call to microSDL_LoadBmp failed\n",acImageSel);
      return;
    }
    pElem->pSurfGlow = pSurfGlow;
  }

}


// TODO: FIXME Doc
// PRE:
// - Assumes that the element is on the active display page
//
// NOTE:
// - The nElemInd may be negative under normal operations
//   to indicate that no draw is required (MSDL_IND_NONE).
//   So we must exit. All other values should be positive.
//
// TODO: Handle MSDL_TYPE_BKGND
// TODO: Handle layers
bool microSDL_ElemDrawByRef(microSDL_tsGui* pGui,microSDL_tsElem* pElem)
{
  if (pElem == NULL) {
    printf("ERROR: microSDL_ElemDrawByRef() NULL element\n");
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

  
  return true;
}


// PRE:
// - Assumes that the element is on the active display page
//
// NOTE:
// - The nElemInd may be negative under normal operations
//   to indicate that no draw is required (MSDL_IND_NONE).
//   So we must exit. All other values should be positive.
//
// TODO: Handle MSDL_TYPE_BKGND
// TODO: Handle layers
bool microSDL_ElemDrawByInd(microSDL_tsGui* pGui,int nElemInd)
{
  bool  bOk = true;
  if (nElemInd == MSDL_IND_NONE) {
    return true;
  } else if (!microSDL_ElemIndValid(pGui,nElemInd)) {
    printf("ERROR: ElemDrawByInd(%d) invalid index\n",nElemInd);
    return false;
  }

  // Draw the element
  bOk = microSDL_ElemDrawByRef(pGui,&(pGui->psElem[nElemInd]));
  
  // Indicate that the item no longer needs a redraw
  pGui->psElem[nElemInd].bNeedRedraw = false;
  
  return bOk;
 
}


void microSDL_ElemCloseAll(microSDL_tsGui* pGui)
{
  unsigned  nElemInd;
  for (nElemInd=0;nElemInd<pGui->nElemCnt;nElemInd++) {
    if (pGui->psElem[nElemInd].pSurf != NULL) {
      SDL_FreeSurface(pGui->psElem[nElemInd].pSurf);
      pGui->psElem[nElemInd].pSurf = NULL;
    }
    if (pGui->psElem[nElemInd].pSurfGlow != NULL) {
      SDL_FreeSurface(pGui->psElem[nElemInd].pSurfGlow);
      pGui->psElem[nElemInd].pSurfGlow = NULL;
    }
  }

  // TODO: Consider moving into main element array
  if (pGui->surfBkgnd != NULL) {
    SDL_FreeSurface(pGui->surfBkgnd);
    pGui->surfBkgnd = NULL;
  }

}

bool microSDL_ViewIndValid(microSDL_tsGui* pGui,int nViewInd)
{
  if ((nViewInd >= 0) && (nViewInd < (int)(pGui->nViewCnt))) {
    return true;
  } else {
    return false;
  }
}

// Search through the viewport array for a matching ID
int microSDL_ViewFindIndFromId(microSDL_tsGui* pGui,int nViewId)
{
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


// Handle MOUSEDOWN
void microSDL_TrackTouchDownClick(microSDL_tsGui* pGui,int nX,int nY)
{
  // Assume no buttons already started tracking, but check
  // just in case
  int nTrackIdOld = pGui->nTrackElemIdStart;

  
  // Find button to start glowing
  // Note that microSDL_ElemFindIndFromCoord() filters out non-clickable elements
  int nTrackIndNew = microSDL_ElemFindIndFromCoord(pGui,nX,nY);
  int nTrackIdNew = microSDL_ElemGetIdFromInd(pGui,nTrackIndNew);
  
  if (nTrackIdNew != MSDL_ID_NONE) {
    // Touch click down on button
    
    // End glow on last element (if any)
    if (nTrackIdOld != MSDL_ID_NONE) {
      microSDL_ElemSetGlow(pGui,nTrackIdOld,false);
    }
    
    // Start glow on new element
    pGui->nTrackElemIdStart = nTrackIdNew;
    microSDL_ElemSetGlow(pGui,nTrackIdNew,true);

    if (nTrackIdNew != MSDL_ID_NONE) {
      microSDL_ElemSetRedraw(pGui,nTrackIdNew,true);
    }
    
    
    // Notify element for optional custom handling
    // - We do this after we have determined which element should
    //   receive the touch tracking
    microSDL_NotifyElemTouch(pGui,MSDL_TOUCH_DOWN,nX,nY);
    
  }
}

// Handle MOUSEUP
//
// POST:
// - Updates pGui->nTrackElemIdClicked
//
void microSDL_TrackTouchUpClick(microSDL_tsGui* pGui,int nX,int nY)
{
  
  // Notify original tracked element for optional custom handling
  // - We do this before any changes are made to nTrackElemIdStart
  microSDL_NotifyElemTouch(pGui,MSDL_TOUCH_UP,nX,nY);
  
  int nTrackIdOld = pGui->nTrackElemIdStart;
  
  // Find button at point of mouse-up
  // - We use this to determine if we had up-event
  //   over the original tracked button
  // NOTE: We could possibly use pGui->nTrackElemIdStart instead
  int nTrackIndNew = microSDL_ElemFindIndFromCoord(pGui,nX,nY);
  int nTrackIdNew = microSDL_ElemGetIdFromInd(pGui,nTrackIndNew);


  if (nTrackIdNew == MSDL_ID_NONE) {
    // Release not over a button
  } else {
    // Released over button
    // Was it released over original tracked element?
    if (nTrackIdNew == pGui->nTrackElemIdStart) {
      // Yes, proceed to select
      pGui->nTrackElemIdClicked = pGui->nTrackElemIdStart;
      // TODO: Create function to set Clicked status at GUI level
    } else {
      // No, ignore
    }
  }

  // Clear glow state
  if (nTrackIdOld != MSDL_ID_NONE) {
    microSDL_ElemSetGlow(pGui,nTrackIdOld,false);
  }  
  
  // Mark this element as needing redraw
  // - Redraw the tracked item to reflect that it is no longer glowing
  if (pGui->nTrackElemIdStart != MSDL_ID_NONE) {
    microSDL_ElemSetRedraw(pGui,pGui->nTrackElemIdStart,true);
  }

  pGui->nTrackElemIdStart = MSDL_ID_NONE;

}

// Handle MOUSEMOVE while MOUSEDOWN
void microSDL_TrackTouchDownMove(microSDL_tsGui* pGui,int nX,int nY)
{
  // Notify original tracked element for optional custom handling
  // - We do this before any changes are made to nTrackElemIdStart  
  microSDL_NotifyElemTouch(pGui,MSDL_TOUCH_MOVE,nX,nY);
  
  // Fetch the item currently being tracked
  int nTrackIdOld = pGui->nTrackElemIdStart;
  
  // Determine the element we are currently over
  int nTrackIndNew = microSDL_ElemFindIndFromCoord(pGui,nX,nY);
  int nTrackIdNew = microSDL_ElemGetIdFromInd(pGui,nTrackIndNew);

  if (nTrackIdNew == MSDL_ID_NONE) {
    // Not currently over a button
    // If we were previously glowing, stop it now
    if (nTrackIdOld != MSDL_ID_NONE) {
      if (microSDL_ElemGetGlow(pGui,nTrackIdOld)) {
        microSDL_ElemSetGlow(pGui,nTrackIdOld,false);
      }
    }
  } else if (nTrackIdOld == nTrackIdNew) {
    // Still over same button
    // If not already glowing, do it now
    microSDL_ElemSetGlow(pGui,nTrackIdOld,true);
  }
  
}

// Callback to Extended Element for notification of touch event
// - Note that this call depends on the current tracked element
//   (nTrackElemIdStart)
// - If the touch callback function is not set, then the default
//   handler can be used instead. The default handler does touch
//   tracking and sets the glowing status.
bool microSDL_NotifyElemTouch(microSDL_tsGui* pGui,microSDL_teTouch eTouch,int nX,int nY)
{
  // Callback function pointer
  MSDL_CB_TOUCH   pfuncXTouch;
  
  // For initial touch-down event, microSDL_TrackTouchDownClick() will
  // find the element owning the coordinate and then set the ID
  // into nTrackElemIdStart before calling microSDL_NotifyElemTouch().
  
  // For initial click on element, nTrackElemIdStart is set to the index
  // of the element and we enter NotifyElemTouch with eTouch=MSDL_TOUCH_DOWN.
  //
  // For continued move / dragging, NotifyElemTouch is called with
  // nTrackElemIdStart still set to the initial element that accepted the
  // touch-down event. NotifyElemTouch is called with eTouch=MSDL_TOUCH_MOVE.
  //
  // For click release, nTrackElemIdStart is still set to the ID of the
  // element that initially accepted the touch-down event, and
  // NotifyElemTouch is called with eTouch=MSDL_TOUCH_DOWN.
  
  // Determine the element currently being tracked
  int nElemId = pGui->nTrackElemIdStart;
  
  // Find element
  if (nElemId == MSDL_ID_NONE) {
    return true;
  }
  int nElemInd = microSDL_ElemFindIndFromId(pGui->psElem,pGui->nElemCnt,nElemId);  
  if (nElemInd == MSDL_IND_NONE) {
    return true;
  }
  
  // Fetch the extended element callback (if enabled)
  pfuncXTouch = pGui->psElem[nElemInd].pfuncXTouch;
  if (pfuncXTouch != NULL) {
    // Pass in the relative position from corner of element region
    int nElemPosX = pGui->psElem[nElemInd].rElem.x;
    int nElemPosY = pGui->psElem[nElemInd].rElem.y;
    (*pfuncXTouch)((void*)(pGui),eTouch,nX-nElemPosX,nY-nElemPosY);
  }
  
  return true;
}


void microSDL_FontCloseAll(microSDL_tsGui* pGui)
{
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

// Initialize the element struct to all zeros
void microSDL_ResetElem(microSDL_tsElem* pElem)
{
  if (pElem == NULL) {
    return;
  }
  pElem->bValid           = false;
  pElem->nId              = MSDL_ID_NONE;
  pElem->nPage            = MSDL_PAGE_NONE;
  pElem->nType            = MSDL_TYPE_BOX;
  pElem->nGroup           = MSDL_GROUP_ID_NONE;
  pElem->rElem            = (SDL_Rect){0,0,0,0};
  pElem->bGlowing         = false;
  pElem->pSurf            = NULL;
  pElem->pSurfGlow        = NULL;
  pElem->bClickEn         = false;
  pElem->bFrameEn         = false;
  pElem->bFillEn          = false;
  pElem->bNeedRedraw      = false;
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

}

// Initialize the font struct to all zeros
void microSDL_ResetFont(microSDL_tsFont* pFont)
{
  if (pFont == NULL) {
    return;
  }
  pFont->nId = MSDL_FONT_NONE;
  pFont->pFont = NULL;
}

// Initialize the view struct to all zeros
void microSDL_ResetView(microSDL_tsView* pView)
{
  if (pView == NULL) {
    return;
  }
  pView->nId = MSDL_VIEW_ID_NONE;
  pView->rView = (SDL_Rect){0,0,0,0};
  pView->nOriginX = 0;
  pView->nOriginY = 0;
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
