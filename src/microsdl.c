// =======================================================================
// microSDL library
// - Calvin Hass
// - http:/www.impulseadventure.com/elec/microsdl-sdl-gui.html
//
// - Version 0.2.3    (2016/10/29)
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
#define MICROSDL_VER "0.2.3"

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
  pGui->nElemCnt        = 0;
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


  // Current touch-tracking hover status
  pGui->nTrackElemHover     = MSDL_IND_NONE;
  pGui->bTrackElemHoverGlow = false;

  // Last graphic element clicked
  pGui->nTrackElemClicked   = MSDL_IND_NONE;
  pGui->nClickLastX         = 0;
  pGui->nClickLastY         = 0;
  pGui->nClickLastPress     = 0;

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
  return true;
}


void microSDL_ApplySurface(microSDL_tsGui* pGui,int x, int y, SDL_Surface* pSrc, SDL_Surface* pDest)
{
  SDL_Rect offset;
  offset.x = x;
  offset.y = y;
  SDL_BlitSurface(pSrc,NULL,pDest,&offset);
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


void microSDL_Flip(microSDL_tsGui* pGui)
{
  SDL_Flip( pGui->surfScreen );
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
  microSDL_LineH(pGui,nX,nY,nW,nCol);                  // Top
  microSDL_LineH(pGui,nX,(Sint16)(nY+nH),nW,nCol);     // Bottom
  microSDL_LineV(pGui,nX,nY,nH,nCol);                  // Left
  microSDL_LineV(pGui,(Sint16)(nX+nW),nY,nH,nCol);     // Right
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
  pGui->nPageIdCur = nPageId;
}


// Page redraw includes the Flip() to finalize
void microSDL_ElemDrawPage(microSDL_tsGui* pGui,int nPageId)
{
  unsigned  nInd;
  
  // Draw background
  // TODO: Consider making background another layer
  microSDL_ApplySurface(pGui,0,0,pGui->surfBkgnd,pGui->surfScreen);

  // Draw other elements
  for (nInd=0;nInd<pGui->nElemCnt;nInd++) {
    if ( (pGui->psElem[nInd].nPage == nPageId) ||
         (pGui->psElem[nInd].nPage == MSDL_PAGE_ALL) ) {
      microSDL_ElemDrawByInd(pGui,nInd);
    }
  }

  // Update the entire screen
  // - NOTE: We could also call Update instead of Flip as that would
  //         limit the region to refresh.
  microSDL_Flip(pGui);
}


void microSDL_ElemDrawPageCur(microSDL_tsGui* pGui)
{
  microSDL_ElemDrawPage(pGui,pGui->nPageIdCur);
}


// ------------------------------------------------------------------------
// Element General Functions
// ------------------------------------------------------------------------

// Search through the element array for a matching ID
int microSDL_ElemFindIndFromId(microSDL_tsGui* pGui,int nElemId)
{
  unsigned  nInd;
  int       nFound = MSDL_IND_NONE;
  for (nInd=0;nInd<pGui->nElemCnt;nInd++) {
    if (pGui->psElem[nInd].nId == nElemId) {
      nFound = nInd;
    }
  }
  return nFound;
}


int microSDL_ElemFindFromCoord(microSDL_tsGui* pGui,int nX, int nY)
{
  unsigned    nInd;
  bool        bFound = false;
  int         nFoundInd = MSDL_IND_NONE;

  #ifdef DBG_TOUCH
  // Highlight current touch for coordinate debug
  SDL_Rect    rMark = microSDL_ExpandRect((SDL_Rect){(Sint16)nX,(Sint16)nY,1,1},1,1);
  microSDL_FrameRect(pGui,rMark,MSDL_COL_YELLOW);
  printf("    ElemFindFromCoord(%3d,%3d):\n",nX,nY);
  #endif

  for (nInd=0;nInd<pGui->nElemCnt;nInd++) {

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


// ------------------------------------------------------------------------
// Element Creation Functions
// ------------------------------------------------------------------------


int microSDL_ElemCreateTxt(microSDL_tsGui* pGui,int nElemId,int nPage,SDL_Rect rElem,
  const char* pStr,int nFontId)
{
  microSDL_tsElem sElem;
  sElem = microSDL_ElemCreate(pGui,nElemId,nPage,MSDL_TYPE_TXT,rElem,pStr,nFontId);
  sElem.colElemFill     = MSDL_COL_BLACK;
  sElem.colElemFrame    = MSDL_COL_BLACK;
  sElem.colElemFillSel  = MSDL_COL_BLACK;
  sElem.colElemText     = MSDL_COL_YELLOW;
  sElem.bFillEn         = true;
  sElem.eTxtAlign       = MSDL_ALIGN_MID_LEFT;
  bool bOk = microSDL_ElemAdd(pGui,sElem);
  return (bOk)? sElem.nId : MSDL_ID_NONE;
}

int microSDL_ElemCreateBtnTxt(microSDL_tsGui* pGui,int nElemId,int nPage,
  SDL_Rect rElem,const char* acStr,int nFontId)
{
  microSDL_tsElem sElem;
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
  sElem.colElemFillSel  = MSDL_COL_GREEN_LT;
  sElem.colElemText     = MSDL_COL_BLACK;
  sElem.bFrameEn        = true;
  sElem.bFillEn         = true;
  sElem.bClickEn        = true;
  bool bOk = microSDL_ElemAdd(pGui,sElem);
  return (bOk)? sElem.nId : MSDL_ID_NONE;
}

int microSDL_ElemCreateBtnImg(microSDL_tsGui* pGui,int nElemId,int nPage,
  SDL_Rect rElem,const char* acImg,const char* acImgSel)
{
  microSDL_tsElem sElem;
  sElem = microSDL_ElemCreate(pGui,nElemId,nPage,MSDL_TYPE_BTN,rElem,"",MSDL_FONT_NONE);
  sElem.colElemFill     = MSDL_COL_BLUE_LT;
  sElem.colElemFrame    = MSDL_COL_BLUE_DK;
  sElem.colElemFillSel  = MSDL_COL_GREEN_LT;
  sElem.colElemText     = MSDL_COL_BLACK;
  sElem.bFrameEn        = false;
  sElem.bFillEn         = false;
  sElem.bClickEn        = true;
  microSDL_ElemSetImage(pGui,&sElem,acImg,acImgSel);
  bool bOk = microSDL_ElemAdd(pGui,sElem);
  return (bOk)? sElem.nId : MSDL_ID_NONE;
}

int microSDL_ElemCreateBox(microSDL_tsGui* pGui,int nElemId,int nPage,SDL_Rect rElem)
{
  microSDL_tsElem sElem;
  sElem = microSDL_ElemCreate(pGui,nElemId,nPage,MSDL_TYPE_BOX,rElem,NULL,MSDL_FONT_NONE);
  sElem.colElemFill     = MSDL_COL_BLACK;
  sElem.colElemFrame    = MSDL_COL_GRAY;
  sElem.bFillEn         = true;
  sElem.bFrameEn        = true;
  bool bOk = microSDL_ElemAdd(pGui,sElem);
  return (bOk)? sElem.nId : MSDL_ID_NONE;
}


int microSDL_ElemCreateImg(microSDL_tsGui* pGui,int nElemId,int nPage,
  SDL_Rect rElem,const char* acImg)
{
  microSDL_tsElem sElem;
  sElem = microSDL_ElemCreate(pGui,nElemId,nPage,MSDL_TYPE_BOX,rElem,"",MSDL_FONT_NONE);
  sElem.bFrameEn        = false;
  sElem.bFillEn         = false;
  sElem.bClickEn        = false;
  microSDL_ElemSetImage(pGui,&sElem,acImg,acImg);
  bool bOk = microSDL_ElemAdd(pGui,sElem);
  return (bOk)? sElem.nId : MSDL_ID_NONE;
}



// ------------------------------------------------------------------------
// Element Drawing Functions
// ------------------------------------------------------------------------

void microSDL_ElemDraw(microSDL_tsGui* pGui,int nElemId)
{
  int nElemInd = microSDL_ElemFindIndFromId(pGui,nElemId);
  if (!microSDL_ElemIndValid(pGui,nElemInd)) {
    fprintf(stderr,"ERROR: ElemDraw() invalid ID=%d\n",nElemInd);
    return;
  }
  microSDL_ElemDrawByInd(pGui,nElemInd);
}


// ------------------------------------------------------------------------
// Element Update Functions
// ------------------------------------------------------------------------

void microSDL_ElemSetFillEn(microSDL_tsGui* pGui,int nElemId,bool bFillEn)
{
  int nElemInd = microSDL_ElemFindIndFromId(pGui,nElemId);
  if (!microSDL_ElemIndValid(pGui,nElemInd)) {
    fprintf(stderr,"ERROR: ElemSetFillEn() invalid ID=%d\n",nElemInd);
    return;
  }
  microSDL_tsElem* pElem = &pGui->psElem[nElemInd];
  pElem->bFillEn = bFillEn;
}

void microSDL_ElemSetCol(microSDL_tsGui* pGui,int nElemId,SDL_Color colFrame,SDL_Color colFill,SDL_Color colFillSel)
{
  int nElemInd = microSDL_ElemFindIndFromId(pGui,nElemId);
  if (!microSDL_ElemIndValid(pGui,nElemInd)) {
    fprintf(stderr,"ERROR: ElemSetCol() invalid ID=%d\n",nElemInd);
    return;
  }
  microSDL_tsElem* pElem = &pGui->psElem[nElemInd];
  pElem->colElemFrame     = colFrame;
  pElem->colElemFill      = colFill;
  pElem->colElemFillSel   = colFillSel;
}



void microSDL_ElemSetTxtAlign(microSDL_tsGui* pGui,int nElemId,unsigned nAlign)
{
  int nElemInd = microSDL_ElemFindIndFromId(pGui,nElemId);
  if (!microSDL_ElemIndValid(pGui,nElemInd)) {
    fprintf(stderr,"ERROR: ElemSetTxtAlign() invalid ID=%d\n",nElemInd);
    return;
  }
  microSDL_tsElem* pElem = &pGui->psElem[nElemInd];
  pElem->eTxtAlign = nAlign;
}


void microSDL_ElemSetTxtStr(microSDL_tsGui* pGui,int nElemId,const char* pStr)
{
  int nElemInd = microSDL_ElemFindIndFromId(pGui,nElemId);
  if (!microSDL_ElemIndValid(pGui,nElemInd)) {
    fprintf(stderr,"ERROR: ElemSetTxtStr() invalid ID=%d\n",nElemInd);
    return;
  }
  microSDL_tsElem* pElem = &pGui->psElem[nElemInd];
  strncpy(pElem->acStr,pStr,MSDL_ELEM_STRLEN_MAX);
}

void microSDL_ElemSetTxtCol(microSDL_tsGui* pGui,int nElemId,SDL_Color colVal)
{
  int nElemInd = microSDL_ElemFindIndFromId(pGui,nElemId);
  if (!microSDL_ElemIndValid(pGui,nElemInd)) {
    fprintf(stderr,"ERROR: ElemSetTxtCol() invalid ID=%d\n",nElemInd);
    return;
  }
  microSDL_tsElem* pElem = &pGui->psElem[nElemInd];
  pElem->colElemText = colVal;
}

void microSDL_ElemUpdateFont(microSDL_tsGui* pGui,int nElemId,int nFontId)
{
  int nElemInd = microSDL_ElemFindIndFromId(pGui,nElemId);
  if (!microSDL_ElemIndValid(pGui,nElemInd)) {
    fprintf(stderr,"ERROR: ElemUpdateFont() invalid ID=%d\n",nElemInd);
    return;
  }
  microSDL_tsElem* pElem = &pGui->psElem[nElemInd];
  pElem->pTxtFont = microSDL_FontGet(pGui,nFontId);
}



// ------------------------------------------------------------------------
// Viewport Functions
// ------------------------------------------------------------------------

int microSDL_ViewCreate(microSDL_tsGui* pGui,int nViewId,SDL_Rect rView,unsigned nOriginX,unsigned nOriginY)
{
  if (pGui->nViewCnt+1 >= (pGui->nViewMax)) {
    fprintf(stderr,"ERROR: microSDL_ViewCreate() too many views\n");
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
  int   nElemInd = pGui->nTrackElemClicked;
  if (nElemInd == MSDL_IND_NONE) {
    return MSDL_ID_NONE;
  } else {
    return microSDL_ElemGetIdFromInd(pGui,nElemInd);
  }
}


void microSDL_ClearTrackElemClicked(microSDL_tsGui* pGui)
{
  pGui->nTrackElemClicked = MSDL_IND_NONE;
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
  unsigned nType,SDL_Rect rElem,const char* pStr,int nFontId)
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
    if (microSDL_ElemFindIndFromId(pGui,nElemId) != MSDL_IND_NONE) {
      printf("ERROR: microSDL_Create() called with existing ID (%d)\n",nElemId);
      return sElem;
    }
  }

  // Assign defaults to the element record
  sElem.nId             = nElemId;
  sElem.nPage           = nPageId;
  sElem.rElem           = rElem;
  sElem.nType           = nType;
  sElem.pTxtFont        = microSDL_FontGet(pGui,nFontId);
  sElem.pSurf           = NULL;
  sElem.pSurfSel        = NULL; 
  sElem.colElemFill     = MSDL_COL_BLACK;
  sElem.colElemFrame    = MSDL_COL_BLUE_DK;
  sElem.colElemFillSel  = MSDL_COL_BLACK;
  sElem.colElemText     = MSDL_COL_YELLOW;
  if (pStr != NULL) {
    strncpy(sElem.acStr,pStr,MSDL_ELEM_STRLEN_MAX);
  } else {
    sElem.acStr[0] = '\0';
  }

  // Assign defaults
  sElem.eTxtAlign = MSDL_ALIGN_MID_MID;
  sElem.nTxtMargin = 5;
  sElem.bFillEn = false;
  sElem.bFrameEn = false;
  sElem.bClickEn = false;

  sElem.pXData = NULL;
  sElem.pfuncXDraw = NULL;
  sElem.pfuncXTouch = NULL;
  
  // If the element creation was successful, then set the valid flag
  sElem.bValid = true;

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

void microSDL_ElemSetImage(microSDL_tsGui* pGui,microSDL_tsElem* pElem,const char* acImage,
  const char* acImageSel)
{
  SDL_Surface*    pSurf;
  SDL_Surface*    pSurfSel;
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
    if (pElem->pSurfSel != NULL) {
      fprintf(stderr,"ERROR: microSDL_ElemSetImage(%s) with pSurfSel already set\n",acImageSel);
      return;
    }
    pSurfSel = microSDL_LoadBmp(pGui,(char*)acImageSel);
    if (pSurfSel == NULL) {
      fprintf(stderr,"ERROR: microSDL_ElemSetImage(%s) call to microSDL_LoadBmp failed\n",acImageSel);
      return;
    }
    pElem->pSurfSel = pSurfSel;
  }

}


// PRE:
// - Assumes that the element is on the active display
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
  if (nElemInd == MSDL_IND_NONE) {
    return true;
  } else if (!microSDL_ElemIndValid(pGui,nElemInd)) {
    printf("ERROR: ElemDrawByInd(%d) invalid index\n",nElemInd);
    return false;
  }

  // TODO: Mark bNeedUpdate=false

  microSDL_tsElem   sElem;
  bool              bSel;
  int               nElemX,nElemY;
  unsigned          nElemW,nElemH;
  SDL_Surface*      surfTxt = NULL;

  sElem = pGui->psElem[nElemInd];
  nElemX = sElem.rElem.x;
  nElemY = sElem.rElem.y;
  nElemW = sElem.rElem.w;
  nElemH = sElem.rElem.h;

  // Determine if this element is currently hovered over
  // TODO: Rename hover for clarity
  bSel = false;
  if (pGui->nTrackElemHover == nElemInd) {
    // This was the hover button
    if (pGui->bTrackElemHoverGlow) {
      #ifdef DBG_LOG
      printf("microSDL_ElemDrawByInd(%u) on hover btn (glow)\n",nElemInd);
      #endif
      bSel = true;
    } else {
      #ifdef DBG_LOG
      printf("microSDL_ElemDrawByInd(%u) on hover btn (no glow)\n",nElemInd);
      #endif
    }
  } else {
    #ifdef DBG_LOG
    printf("microSDL_ElemDrawByInd(%u) on non-hover btn\n",nElemInd);
    #endif
  }

  // Fill in the background
  // - This also changes the fill color if it is a button
  //   being selected
  if (sElem.bFillEn) {
    if ((sElem.nType == MSDL_TYPE_BTN) && (bSel)) {
      microSDL_FillRect(pGui,sElem.rElem,sElem.colElemFillSel);
    } else {
      microSDL_FillRect(pGui,sElem.rElem,sElem.colElemFill);
    }
  } else {
    // TODO: If unfilled, then we might need
    // to redraw the background layer(s)
  }

  
  // Handle any extended element types
  switch(sElem.nType) {
    case MSDL_TYPEX_GAUGE:
      if (sElem.pfuncXDraw == NULL) {
        return false;
      }
      (*sElem.pfuncXDraw)((void*)(pGui),(void*)(&sElem));
      //xxx microSDL_ElemXGaugeDraw((void*)(pGui),(void*)(&sElem));
      break;
      
    default:
      // No extra handling required
      break;
  }


  // Frame the region
  #ifdef DBG_FRAME
  // For debug purposes, draw a frame around every element
  microSDL_FrameRect(pGui,sElem.rElem,MSDL_COL_GRAY_DK);
  #else
  if (sElem.bFrameEn) {
    microSDL_FrameRect(pGui,sElem.rElem,sElem.colElemFrame);
  }
  #endif

  // Draw any images associated with element
  if (sElem.pSurf != NULL) {
    if ((bSel) && (sElem.pSurfSel != NULL)) {
      microSDL_ApplySurface(pGui,nElemX,nElemY,sElem.pSurfSel,pGui->surfScreen);
    } else {
      microSDL_ApplySurface(pGui,nElemX,nElemY,sElem.pSurf,pGui->surfScreen);
    }
  }


 
  // Overlay the text
  bool    bRenderTxt = true;
  bRenderTxt &= (sElem.acStr[0] != '\0');
  bRenderTxt &= (sElem.pTxtFont != NULL);

  if (bRenderTxt) {

    // Define margined element bounds
    int         nTxtX = nElemX;
    int         nTxtY = nElemY;
    unsigned    nMargin = sElem.nTxtMargin;
    TTF_Font*   pFont = sElem.pTxtFont;

    // Fetch the size of the text to allow for justification
    int nTxtSzW,nTxtSzH;
    TTF_SizeText(pFont,sElem.acStr,&nTxtSzW,&nTxtSzH);

    surfTxt = TTF_RenderText_Solid(pFont,sElem.acStr,sElem.colElemText);
    if (surfTxt == NULL) {
      fprintf(stderr,"ERROR: TTF_RenderText_Solid failed (%s)\n",sElem.acStr);
      return false;
    }

    // Handle text alignments

    // Check for ALIGNH_LEFT & ALIGNH_RIGHT. Default to ALIGNH_MID
    if      (sElem.eTxtAlign & MSDL_ALIGNH_LEFT)      { nTxtX = nElemX+nMargin; }
    else if (sElem.eTxtAlign & MSDL_ALIGNH_RIGHT)     { nTxtX = nElemX+nElemW-nMargin-nTxtSzW; }
    else                                              { nTxtX = nElemX+(nElemW/2)-(nTxtSzW/2); }

    // Check for ALIGNV_TOP & ALIGNV_BOT. Default to ALIGNV_MID
    if      (sElem.eTxtAlign & MSDL_ALIGNV_TOP)       { nTxtY = nElemY+nMargin; }
    else if (sElem.eTxtAlign & MSDL_ALIGNV_BOT)       { nTxtY = nElemY+nElemH-nMargin-nTxtSzH; }
    else                                              { nTxtY = nElemY+(nElemH/2)-(nTxtSzH/2); }


    microSDL_ApplySurface(pGui,nTxtX,nTxtY,surfTxt,pGui->surfScreen);

    if (surfTxt != NULL) {
      SDL_FreeSurface(surfTxt);
      surfTxt = NULL;
    }
  }

  return true;
}



void microSDL_ElemCloseAll(microSDL_tsGui* pGui)
{
  unsigned  nElemInd;
  for (nElemInd=0;nElemInd<pGui->nElemCnt;nElemInd++) {
    if (pGui->psElem[nElemInd].pSurf != NULL) {
      SDL_FreeSurface(pGui->psElem[nElemInd].pSurf);
      pGui->psElem[nElemInd].pSurf = NULL;
    }
    if (pGui->psElem[nElemInd].pSurfSel != NULL) {
      SDL_FreeSurface(pGui->psElem[nElemInd].pSurfSel);
      pGui->psElem[nElemInd].pSurfSel = NULL;
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
  int       nFound = MSDL_IND_NONE;
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
  // TODO: Might need to correct by -1
  Sint16 nX0,nY0,nX1,nY1;
  nX0 = prRect->x;
  nY0 = prRect->y;
  nX1 = prRect->x + prRect->w;
  nY1 = prRect->y + prRect->h;
  microSDL_ViewRemapPt(pGui,&nX0,&nY0);
  microSDL_ViewRemapPt(pGui,&nX1,&nY1);
  prRect->x = nX0;
  prRect->y = nY0;
  prRect->w = nX1-nX0;
  prRect->h = nY1-nY0;
}


int microSDL_GetTrackElemIndClicked(microSDL_tsGui* pGui)
{
  return pGui->nTrackElemClicked;
}

// Handle MOUSEDOWN
void microSDL_TrackTouchDownClick(microSDL_tsGui* pGui,int nX,int nY)
{
  // Assume no buttons in hover or clicked

  // Find button to start hover
  // Note that microSDL_ElemFindFromCoord() filters out non-clickable elements
  int nHoverStart = microSDL_ElemFindFromCoord(pGui,nX,nY);

  if (nHoverStart != MSDL_IND_NONE) {
    // Touch click down on button
    // Start hover
    pGui->nTrackElemHover = nHoverStart;
    pGui->bTrackElemHoverGlow = true;
    #ifdef DBG_TOUCH
    printf("microSDL_TrackTouchDownClick @ (%3u,%3u): on button [Ind=%u]\n",nX,nY,
      pGui->nTrackElemHover);
    #endif

    // Notify element for optional custom handling
    // - We do this after we have determined which element should
    //   receive the touch tracking
    microSDL_NotifyElemTouch(pGui,MSDL_TOUCH_DOWN,nX,nY);
    
    // Redraw button
    microSDL_ElemDrawByInd(pGui,nHoverStart);
    // Redraw
    microSDL_Flip(pGui);
  } else {
    #ifdef DBG_TOUCH
    printf("microSDL_TrackTouchDownClick @ (%3u,%3u): not on button\n",nX,nY);
    #endif
  }
}

// Handle MOUSEUP
//
// POST:
// - Updates pGui->nTrackElemClicked
//
void microSDL_TrackTouchUpClick(microSDL_tsGui* pGui,int nX,int nY)
{
  
  // Notify original hover element for optional custom handling
  // - We do this before any changes are made to nTrackElemHover
  microSDL_NotifyElemTouch(pGui,MSDL_TOUCH_UP,nX,nY);
  
  // Find button at point of mouse-up
  // NOTE: We could possibly use pGui->nTrackElemHover instead
  int nHoverNew = microSDL_ElemFindFromCoord(pGui,nX,nY);

  if (nHoverNew == MSDL_IND_NONE) {
    // Release not over a button
    #ifdef DBG_TOUCH
    printf("microSDL_TrackTouchUpClick: not on button\n");
    #endif
  } else {
    // Released over button
    // Was it released over original hover button?
    if (nHoverNew == pGui->nTrackElemHover) {
      // Yes, proceed to select
      pGui->nTrackElemClicked = pGui->nTrackElemHover;
      #ifdef DBG_TOUCH
      printf("microSDL_TrackTouchUpClick: on hovered button\n");
      printf("Track: Selected button [Ind=%u]\n",pGui->nTrackElemClicked);
      #endif
    } else {
      // No, ignore
      #ifdef DBG_TOUCH
      printf("microSDL_TrackTouchUpClick: on another button\n");
      #endif
    }
  }

  // Clear hover state(s)
  pGui->bTrackElemHoverGlow = false;
  
  
  
  // Redraw elements
  microSDL_ElemDrawByInd(pGui,pGui->nTrackElemHover);
  microSDL_ElemDrawByInd(pGui,nHoverNew);

  // Redraw window
  microSDL_Flip(pGui);

  pGui->nTrackElemHover = MSDL_IND_NONE;

}

// Handle MOUSEMOVE while MOUSEDOWN
void microSDL_TrackTouchDownMove(microSDL_tsGui* pGui,int nX,int nY)
{
  // Notify original hover element for optional custom handling
  // - We do this before any changes are made to nTrackElemHover  
  microSDL_NotifyElemTouch(pGui,MSDL_TOUCH_MOVE,nX,nY);
  
  
  int nHoverOld = pGui->nTrackElemHover;
  int nHoverNew = microSDL_ElemFindFromCoord(pGui,nX,nY);

  if (nHoverNew == MSDL_IND_NONE) {
    // Not currently over a button
    // If we were previously hovering, stop hover glow now
    if (pGui->bTrackElemHoverGlow) {
      pGui->bTrackElemHoverGlow = false;
      microSDL_ElemDrawByInd(pGui,pGui->nTrackElemHover);
      #ifdef DBG_TOUCH
      printf("microSDL_TrackTouchDownMove: not over hover button, stop glow\n");
      #endif
    } else {
      #ifdef DBG_TOUCH
      printf("microSDL_TrackTouchDownMove: not over hover button, wasn't glowing\n");
      #endif
    }
  } else if (nHoverOld == nHoverNew) {
    // Still over same button
    // If not already in hover glow, do it now
    if (!pGui->bTrackElemHoverGlow) {
      pGui->bTrackElemHoverGlow = true;
      microSDL_ElemDrawByInd(pGui,pGui->nTrackElemHover);
      #ifdef DBG_TOUCH
      printf("microSDL_TrackTouchDownMove: over hover button, start glow\n");
      #endif
    } else {
      // Already glowing and same button
      // so nothing to do
      #ifdef DBG_TOUCH
      printf("microSDL_TrackTouchDownMove: over hover button, already glow\n");
      #endif
    }
  }
  
}

// Callback to Extended Element to notify of touch event
// - Note that this call depends on the current "hover" element
//   (ie. the element being tracked)
// - eTouch is the enumeration: MSDL_TOUCH_*
bool microSDL_NotifyElemTouch(microSDL_tsGui* pGui,int eTouch,int nX,int nY)
{
  // Function pointer
  bool  (*pfuncXTouch)(void* pGui,int eTouch,int nX,int nY);
  
  // Determine the element currently in hover mode
  int nElemInd = pGui->nTrackElemHover;
  
  // Find element
  if (nElemInd == MSDL_IND_NONE) {
    return true;
  } else if (!microSDL_ElemIndValid(pGui,nElemInd)) {
    printf("ERROR: microSDL_NotifyElemTouch(%d) invalid index\n",nElemInd);
    return false;
  }
  
  // Fetch the extended element callback (if enabled)
  pfuncXTouch = pGui->psElem[nElemInd].pfuncXTouch;
  if (pfuncXTouch != NULL) {
    (*pfuncXTouch)((void*)(pGui),eTouch,nX,nY);
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
  pElem->nType            = MSDL_TYPE_BOX; //
  pElem->rElem            = (SDL_Rect){0,0,0,0};
  pElem->pSurf            = NULL;
  pElem->pSurfSel         = NULL;
  pElem->bClickEn         = false;
  pElem->bFrameEn         = false;
  pElem->bFillEn          = false;
  pElem->colElemFrame     = MSDL_COL_WHITE;
  pElem->colElemFill      = MSDL_COL_WHITE;
  pElem->colElemFillSel   = MSDL_COL_WHITE;

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
