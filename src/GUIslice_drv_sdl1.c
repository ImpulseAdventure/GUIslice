// =======================================================================
// GUIslice library (driver layer for SDL1.2)
// - Calvin Hass
// - http://www.impulseadventure.com/elec/microsdl-sdl-gui.html
//
// - Version 0.6.3    (2016/11/20)
// =======================================================================
//
// The MIT License
//
// Copyright 2016 Calvin Hass
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// =======================================================================


// =======================================================================
// Driver Layer for SDL1.2
// =======================================================================

// GUIslice library
#include "GUIslice_drv_sdl1.h"

#include <stdio.h>


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


// =======================================================================
// Public APIs to GUIslice core library
// =======================================================================

// -----------------------------------------------------------------------
// Configuration Functions
// -----------------------------------------------------------------------

bool gslc_DrvInit(gslc_tsGui* pGui)
{
  
  // Primary surface definitions
  pGui->pvSurfScreen = NULL;
  pGui->pvSurfBkgnd = NULL;  
  
#ifdef VT_WRK_EN
  // Force a clean start to SDL to workaround any bad state
  // left behind by a previous SDL application's failure to
  // clean up.
  // TODO: Allow compiler option to skip this workaround
  // TODO: Allow determination of the TTY to use
  gslc_DrvCleanStart(VT_WRK_TTY);
#endif  

  // Setup SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    fprintf(stderr,"ERROR: DrvInit() error in SDL_Init(): %s\n",SDL_GetError());
    return false;
  }
  // Now that we have successfully initialized SDL
  // we need to register an exit handler so that SDL_Quit()
  // gets called at program termination. If we don't do this
  // then some types of errors/aborts will result in
  // the SDL environment being left in a bad state that
  // affects the next SDL program execution.
  atexit(SDL_Quit);

  // Set up pGui->pvSurfScreen
  const SDL_VideoInfo*  videoInfo = SDL_GetVideoInfo();
  int                   nSystemX = videoInfo->current_w;
  int                   nSystemY = videoInfo->current_h;
  uint8_t               nBpp = videoInfo->vfmt->BitsPerPixel ;

  // SDL_SWSURFACE is apparently more reliable
  pGui->pvSurfScreen = (void*)SDL_SetVideoMode(nSystemX,nSystemY,nBpp,SDL_SWSURFACE);
  if (!pGui->pvSurfScreen) {
    fprintf(stderr,"ERROR: DrvInit() error in SDL_SetVideoMode(): %s\n",SDL_GetError());
    return false;
  }

  // Initialize font engine
  if (TTF_Init() == -1) {
    fprintf(stderr,"ERROR: DrvInit() error in TTF_Init()\n");
    return false;
  }

  // Since the mouse cursor is based on SDL coords which are badly
  // scaled when in touch mode, we will disable the mouse pointer.
  // Note that the SDL coords seem to be OK when in actual mouse mode.
  SDL_ShowCursor(SDL_DISABLE);

  return true;
}

#define TMP_INIT

// Need to configure a number of SDL and TS environment
// variables. The following demonstrates a way to do this
// programmatically, but it can also be done via export
// commands within the shell (or init script).
// eg. export TSLIB_FBDEVICE=/dev/fb1
void gslc_DrvInitEnv(char* acDevFb,char* acDevTouch)
{
  
  // This line already appears to be set in env
  setenv((char*)"FRAMEBUFFER",acDevFb,1);
  // The following is the only required entra line
  setenv((char*)"SDL_FBDEV",acDevFb,1);

  // The following lines don't appear to be required
  setenv((char*)"SDL_VIDEODRIVER",(char*)"fbcon",1);
  setenv((char*)"SDL_FBDEV",acDevFb,1);   


  // Disable these lines as it appears to cause
  // conflict in the SDL mouse coordinate reporting
  // (perhaps b/c it is mixing real mouse and touch?)
  //setenv((char*)"SDL_MOUSEDRV",(char*)"TSLIB",1);
  //setenv((char*)"SDL_MOUSEDEV",acDevTouch,1);    
  

  #ifdef INC_TS
  setenv((char*)"TSLIB_FBDEVICE",acDevFb,1);
  setenv((char*)"TSLIB_TSDEVICE",acDevTouch,1); 
  setenv((char*)"TSLIB_CALIBFILE",(char*)"/etc/pointercal",1);
  setenv((char*)"TSLIB_CONFFILE",(char*)"/etc/ts.conf",1);
  setenv((char*)"TSLIB_PLUGINDIR",(char*)"/usr/local/lib/ts",1);
  #endif

}


// -----------------------------------------------------------------------
// Image/surface handling Functions
// -----------------------------------------------------------------------


bool gslc_DrvScreenLock(gslc_tsGui* pGui)
{
  if (pGui == NULL) {
    fprintf(stderr,"ERROR: DrvScreenLock() called with NULL ptr\n");
    return false;
  }       
  // Typecast
  SDL_Surface*  pScreen   = (SDL_Surface*)(pGui->pvSurfScreen);  
  
  if (SDL_MUSTLOCK(pScreen)) {
    if (SDL_LockSurface(pScreen) < 0) {
      fprintf(stderr,"ERROR: DrvScreenLock() can't lock screen: %s\n",SDL_GetError());
      return false;
    }
  }   
  return true;
}


void gslc_DrvScreenUnlock(gslc_tsGui* pGui)
{
  if (pGui == NULL) {
    fprintf(stderr,"ERROR: DrvScreenUnlock() called with NULL ptr\n");
    return;
  }      
  // Typecast
  SDL_Surface*  pScreen   = (SDL_Surface*)(pGui->pvSurfScreen);  
  
  if (SDL_MUSTLOCK(pScreen)) {
    SDL_UnlockSurface(pScreen);
  }
}

SDL_Surface* gslc_DrvLoadBmp(gslc_tsGui* pGui,char* pStrFname)
{
  //The image that's loaded
  SDL_Surface* surfLoaded = NULL;

  //The optimized surface that will be used
  SDL_Surface* surfOptimized = NULL;

  //Load the image
  surfLoaded = SDL_LoadBMP(pStrFname);

  // Confirm that the image loaded correctly
  if (surfLoaded == NULL) {
    fprintf(stderr,"ERROR: DrvLoadBmp(%s) failed: %s\n",pStrFname,SDL_GetError());
    return NULL;
  } else {
    //Create an optimized surface
    surfOptimized = SDL_DisplayFormat( surfLoaded );

    //Free the old surface
    SDL_FreeSurface( surfLoaded );

    //If the surface was optimized
    if( surfOptimized != NULL ) {

      // Support optional transparency
      if (GSLC_BMP_TRANS_EN) {
        // Color key surface
        // - Use transparency color key defined in BMP_TRANS_RGB
        SDL_SetColorKey( surfOptimized, SDL_SRCCOLORKEY,
          SDL_MapRGB( surfOptimized->format, GSLC_BMP_TRANS_RGB ) );
      } // GSLC_BMP_TRANS_EN
    }
  }

  //Return the optimized surface
  return surfOptimized;
}


bool gslc_DrvSetBkgndImage(gslc_tsGui* pGui,char* pStrFname)
{
  if (strlen(pStrFname)==0) {
    // TODO: Error handling
    return false;
  }
  if (pGui->pvSurfBkgnd != NULL ) {
    // Dispose of previous background
    SDL_FreeSurface((SDL_Surface*)(pGui->pvSurfBkgnd));
    pGui->pvSurfBkgnd = NULL;
  }
  pGui->pvSurfBkgnd = gslc_DrvLoadBmp(pGui,pStrFname);
  if (pGui->pvSurfBkgnd == NULL) {
    return false;
  }
  
  return true;
}

bool gslc_DrvSetBkgndColor(gslc_tsGui* pGui,gslc_Color nCol)
{
  if (pGui->pvSurfBkgnd != NULL ) {
    // Dispose of previous background
    gslc_DrvSurfaceDestruct(pGui->pvSurfBkgnd);
    pGui->pvSurfBkgnd = NULL;
  }
  SDL_Surface*  pSurfScreen = (SDL_Surface*)(pGui->pvSurfScreen);
  SDL_Surface*  pSurfBkgnd = NULL;
  
  unsigned nScreenW = pSurfScreen->w;
  unsigned nScreenH = pSurfScreen->h;
  unsigned nBpp = pSurfScreen->format->BytesPerPixel * 8;
  pSurfBkgnd = SDL_CreateRGBSurface(SDL_SWSURFACE,
    nScreenW,nScreenH,nBpp,0,0,0,0xFF);
  
  SDL_FillRect(pSurfBkgnd,NULL,
    SDL_MapRGB(pSurfBkgnd->format,nCol.r,nCol.g,nCol.b));

  pGui->pvSurfBkgnd = (void*)(pSurfBkgnd);

  if (pGui->pvSurfBkgnd == NULL) {
    return false;
  }
  
  return true;
}


void gslc_DrvSetElemImageNorm(gslc_tsGui* pGui,gslc_tsElem* pElem,const char* acImage)
{
  SDL_Surface*    pSurf;
  if (pElem->pvSurfNorm != NULL) {
    fprintf(stderr,"ERROR: DrvSetElemImageNorm(%s) with pvSurfNorm already set\n",acImage);
    return;
  }
  pSurf = gslc_DrvLoadBmp(pGui,(char*)acImage);
  if (pSurf == NULL) {
    fprintf(stderr,"ERROR: DrvSetElemImageNorm(%s) call to gslc_DrvLoadBmp failed\n",acImage);
    return;
  }
  pElem->pvSurfNorm = pSurf;
}


void gslc_DrvSetElemImageGlow(gslc_tsGui* pGui,gslc_tsElem* pElem,const char* acImage)
{
  SDL_Surface*    pSurf;
  if (pElem->pvSurfGlow != NULL) {
    fprintf(stderr,"ERROR: DrvSetElemImageGlow(%s) with pvSurfGlow already set\n",acImage);
    return;
  }
  pSurf = gslc_DrvLoadBmp(pGui,(char*)acImage);
  if (pSurf == NULL) {
    fprintf(stderr,"ERROR: DrvSetElemImageGlow(%s) call to gslc_DrvLoadBmp failed\n",acImage);
    return;
  }
  pElem->pvSurfGlow = pSurf;  
}

void gslc_DrvPasteSurface(gslc_tsGui* pGui,int nX, int nY, void* pvSrc, void* pvDest)
{
  if ((pGui == NULL) || (pvSrc == NULL) || (pvDest == NULL)) {
    fprintf(stderr,"ERROR: DrvPasteSurface() called with NULL ptr\n");
    return;
  }
  SDL_Surface*  pSrc  = (SDL_Surface*)(pvSrc);
  SDL_Surface*  pDest = (SDL_Surface*)(pvDest);
  SDL_Rect offset;
  offset.x = nX;
  offset.y = nY;
  SDL_BlitSurface(pSrc,NULL,pDest,&offset);
  gslc_PageFlipSet(pGui,true);
}

void gslc_DrvSurfaceDestruct(void* pvSurf)
{
  SDL_Surface*  pSurf = (SDL_Surface*)(pvSurf);
  if (pSurf == NULL) {
    return;
  }
  SDL_FreeSurface(pSurf);
}

void gslc_DrvSetClipRect(gslc_tsGui* pGui,gslc_Rect* pRect)
{
  SDL_Surface*  pScreen = (SDL_Surface*)(pGui->pvSurfScreen);
  if (pRect == NULL) {
    SDL_SetClipRect(pScreen,NULL);
  } else {
    SDL_Rect  rSRect = gslc_DrvAdaptRect(*pRect);
    SDL_SetClipRect(pScreen,&rSRect);
  }
}



// -----------------------------------------------------------------------
// Font handling Functions
// -----------------------------------------------------------------------

void gslc_DrvFontsDestruct(gslc_tsGui* pGui)
{
  unsigned  nFontInd;
  TTF_Font* pFont = NULL;
  for (nFontInd=0;nFontInd<pGui->nFontCnt;nFontInd++) {
    if (pGui->asFont[nFontInd].pvFont != NULL) {
      pFont = (TTF_Font*)(pGui->asFont[nFontInd].pvFont);
      TTF_CloseFont(pFont);
      pGui->asFont[nFontInd].pvFont = NULL;
    }
  }
  pGui->nFontCnt = 0;
  TTF_Quit();  
}

bool gslc_DrvDrawTxt(gslc_tsGui* pGui,gslc_tsElem* pElem)
{
  if ((pGui == NULL) || (pElem == NULL)) {
    fprintf(stderr,"ERROR: DrvDrawTxt() with NULL ptr\n"); 
    return false;
  }
  TTF_Font*   pFont = (TTF_Font*)(pElem->pvTxtFont);  
  if (pFont == NULL) {
    fprintf(stderr,"ERROR: DrvDrawTxt() with NULL font\n"); 
    return false;
  }

  SDL_Surface*      surfTxt = NULL;
  int               nElemX,nElemY;
  unsigned          nElemW,nElemH;
  bool              bGlow;
  
  nElemX    = pElem->rElem.x;
  nElemY    = pElem->rElem.y;
  nElemW    = pElem->rElem.w;
  nElemH    = pElem->rElem.h;
  bGlow     = (pElem->bGlowEn && pElem->bGlowing);
  
  // Define margined element bounds
  int         nTxtX = nElemX;
  int         nTxtY = nElemY;
  unsigned    nMargin = pElem->nTxtMargin;


  // Fetch the size of the text to allow for justification
  int nTxtSzW,nTxtSzH;
  TTF_SizeText(pFont,pElem->acStr,&nTxtSzW,&nTxtSzH);

  surfTxt = TTF_RenderText_Solid(pFont,pElem->acStr,
          gslc_DrvAdaptColor((bGlow)?pElem->colElemTextGlow:pElem->colElemText));
  if (surfTxt == NULL) {
    fprintf(stderr,"ERROR: DrvDrawTxt() failed in TTF_RenderText_Solid() (%s)\n",pElem->acStr);
    return false;
  }

  // Handle text alignments

  // Check for ALIGNH_LEFT & ALIGNH_RIGHT. Default to ALIGNH_MID
  if      (pElem->eTxtAlign & GSLC_ALIGNH_LEFT)     { nTxtX = nElemX+nMargin; }
  else if (pElem->eTxtAlign & GSLC_ALIGNH_RIGHT)    { nTxtX = nElemX+nElemW-nMargin-nTxtSzW; }
  else                                              { nTxtX = nElemX+(nElemW/2)-(nTxtSzW/2); }

  // Check for ALIGNV_TOP & ALIGNV_BOT. Default to ALIGNV_MID
  if      (pElem->eTxtAlign & GSLC_ALIGNV_TOP)      { nTxtY = nElemY+nMargin; }
  else if (pElem->eTxtAlign & GSLC_ALIGNV_BOT)      { nTxtY = nElemY+nElemH-nMargin-nTxtSzH; }
  else                                              { nTxtY = nElemY+(nElemH/2)-(nTxtSzH/2); }


  gslc_DrvPasteSurface(pGui,nTxtX,nTxtY,surfTxt,pGui->pvSurfScreen);

  if (surfTxt != NULL) {
    SDL_FreeSurface(surfTxt);
    surfTxt = NULL;
  }  
  
  return true;
}


// -----------------------------------------------------------------------
// Screen Management Functions
// -----------------------------------------------------------------------

void gslc_DrvPageFlipNow(gslc_tsGui* pGui)
{
  SDL_Surface*  pScreen = (SDL_Surface*)(pGui->pvSurfScreen);  
  SDL_Flip(pScreen);
}


// -----------------------------------------------------------------------
// Graphics Primitives Functions
// -----------------------------------------------------------------------  

uint32_t gslc_DrvAdaptColorRaw(gslc_tsGui* pGui,gslc_Color nCol)
{
  if (pGui == NULL) {
    fprintf(stderr,"ERROR: DrvAdaptColorRaw() called with NULL ptr\n");
    return 0;
  }      
  SDL_Surface*  pScreen = (SDL_Surface*)(pGui->pvSurfScreen);
  return SDL_MapRGB(pScreen->format,nCol.r,nCol.g,nCol.b);
}

// - Based on code from:
// -   https://www.libsdl.org/release/SDL-1.2.15/docs/html/guidevideo.html
uint32_t gslc_DrvDrawGetPixelRaw(gslc_tsGui* pGui, int nX, int nY)
{
  if (pGui == NULL) {
    fprintf(stderr,"ERROR: DrvDrawGetPixelRaw() called with NULL ptr\n");
    return 0;
  }
  SDL_Surface*  pScreen = (SDL_Surface*)(pGui->pvSurfScreen);
  if (pScreen == NULL) {
    fprintf(stderr,"ERROR: DrvDrawGetPixelRaw() screen surface NULL\n");
    return 0;
  }      
  int nBpp = pScreen->format->BytesPerPixel;

  // Handle any range violations for entire surface
  if ( (nX < 0) || (nX >= pScreen->w) ||
       (nY < 0) || (nY >= pScreen->h) ) {
    // ERROR
    fprintf(stderr,"ERROR: DrvDrawGetPixelRaw() out of range (%i,%i)\n",nX,nY);
    return 0;
  }

  /* Here pPixel is the address to the pixel we want to get */
  uint8_t *pPixel = (uint8_t *)pScreen->pixels + nY * pScreen->pitch + nX * nBpp;

  switch(nBpp) {
    case 1:
      return *pPixel;
  
    case 2:
      return *(uint16_t *)pPixel;
  
    case 3:
      if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
        return pPixel[0] << 16 | pPixel[1] << 8 | pPixel[2];
      else
        return pPixel[0] | pPixel[1] << 8 | pPixel[2] << 16;
  
    case 4:
      return *(uint32_t *)pPixel;
  
    default:
      return 0;       /* shouldn't happen, but avoids warnings */
  }

}


// - Based on code from:
// -   https://www.libsdl.org/release/SDL-1.2.15/docs/html/guidevideo.html
// - Added range checks from surface clipping rect
void gslc_DrvDrawSetPixelRaw(gslc_tsGui* pGui,int nX, int nY, uint32_t nPixelVal)
{
  if (pGui == NULL) {
    fprintf(stderr,"ERROR: DrvDrawSetPixelRaw() called with NULL ptr\n");
    return;
  }
  SDL_Surface*  pScreen = (SDL_Surface*)(pGui->pvSurfScreen);
  if (pScreen == NULL) {
    fprintf(stderr,"ERROR: DrvDrawSetPixelRaw() screen surface NULL\n");
    return;
  }        
  int nBpp = pScreen->format->BytesPerPixel;

  // Handle any clipping
  if ( (nX < pScreen->clip_rect.x) || (nX >= pScreen->clip_rect.x+pScreen->clip_rect.w) ||
       (nY < pScreen->clip_rect.y) || (nY >= pScreen->clip_rect.y+pScreen->clip_rect.h) ) {
    return;
  }

  /* Here pPixel is the address to the pixel we want to set */
  uint8_t *pPixel = (uint8_t *)pScreen->pixels + nY * pScreen->pitch + nX * nBpp;

  switch(nBpp) {
    case 1:
      *pPixel = nPixelVal;
      break;

    case 2:
      *(uint16_t *)pPixel = nPixelVal;
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
      *(uint32_t *)pPixel = nPixelVal;
      break;
  }
}

void gslc_DrvDrawFillRect(gslc_tsGui* pGui,gslc_Rect rRect,gslc_Color nCol)
{
  // Typecast
  SDL_Rect      rSRect  = gslc_DrvAdaptRect(rRect);
  SDL_Surface*  pScreen   = (SDL_Surface*)(pGui->pvSurfScreen);
  
  // Call SDL optimized routine
  SDL_FillRect(pScreen,&rSRect,
    SDL_MapRGB(pScreen->format,nCol.r,nCol.g,nCol.b)); 
}


// -----------------------------------------------------------------------
// Touch Functions
// -----------------------------------------------------------------------  

bool gslc_DrvGetTouch(gslc_tsGui* pGui,int* pnX,int* pnY,unsigned* pnPress)
{
  if (pGui == NULL) {
    fprintf(stderr,"ERROR: DrvGetTouch() called with NULL ptr\n");
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


// =======================================================================
// Private Functions
// =======================================================================

// -----------------------------------------------------------------------
// Configuration Functions
// -----------------------------------------------------------------------

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
bool gslc_DrvCleanStart(const char* sTTY)
{
#ifdef VT_WRK_EN    
    int     nFD = -1;
    int     nRet = false;
    
    nFD = open(sTTY, O_RDONLY, 0);
    if (nFD < 0) {
        fprintf(stderr, "ERROR: DrvCleanStart() failed to open console (%s): %s\n",
            sTTY,strerror(errno));
        return false;
    }
    nRet = ioctl(nFD, VT_UNLOCKSWITCH, 1);
    if (nRet != false) {
         fprintf(stderr, "ERROR: DrvCleanStart() failed to unlock console (%s): %s\n",
            sTTY,strerror(errno));
        return false;
    }
    nRet = ioctl(nFD, KDSETMODE, KD_TEXT);
    if (nRet != 0) {
         fprintf(stderr, "ERROR: DrvCleanStart() failed to set text mode (%s): %s\n",
            sTTY,strerror(errno));
        return false;
    }
    close(nFD);
#endif    
    return true;
}


// -----------------------------------------------------------------------
// Conversion Functions
// -----------------------------------------------------------------------

// Simple typecasting for abstraction layer
SDL_Rect  gslc_DrvAdaptRect(gslc_Rect rRect)
{
  SDL_Rect  rSRect;
  rSRect.x = rRect.x;
  rSRect.y = rRect.y;
  rSRect.h = rRect.h;
  rSRect.w = rRect.w;
  return rSRect;
}

// Simple typecasting for abstraction layer
SDL_Color  gslc_DrvAdaptColor(gslc_Color sCol)
{
  SDL_Color  sSCol;
  sSCol.r = sCol.r;
  sSCol.g = sCol.g;
  sSCol.b = sCol.b;
  sSCol.unused = 0;
  return sSCol;
}

