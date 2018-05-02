// =======================================================================
// GUIslice library (driver layer for SDL 1.2 & 2.0)
// - Calvin Hass
// - https://www.impulseadventure.com/elec/guislice-gui.html
// - https://github.com/ImpulseAdventure/GUIslice
// =======================================================================
//
// The MIT License
//
// Copyright 2018 Calvin Hass
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

// Compiler guard for requested driver
#include "GUIslice_config.h" // Sets DRV_DISP_*
#if defined(DRV_DISP_SDL1) || defined(DRV_DISP_SDL2)

// =======================================================================
// Driver Layer for SDL
// =======================================================================


// GUIslice library
#include "GUIslice_drv_sdl.h"

#include <stdio.h>


// Optionally enable SDL clean start VT workaround
#if (DRV_SDL_FIX_START)
  #include <fcntl.h>      // For O_RDONLY
  #include <errno.h>      // For errno
  #include <unistd.h>     // For close()
  #include <sys/ioctl.h>  // For ioctl()
  #include <sys/kd.h>     // for KDSETMODE
  #include <sys/vt.h>     // for VT_UNLOCKSWITCH
  #define DRV_SDL_FIX_TTY      "/dev/tty0"
#endif




// =======================================================================
// Public APIs to GUIslice core library
// =======================================================================

// -----------------------------------------------------------------------
// Configuration Functions
// -----------------------------------------------------------------------

bool gslc_DrvInit(gslc_tsGui* pGui)
{
  // Report any debug info (before init) if enabled
  #if defined(DBG_DRIVER)
  gslc_DrvReportInfoPre();
  #endif

  // Primary surface definitions
  pGui->sImgRefBkgnd = gslc_ResetImage();

  // Initialize any SDL version-specific members
  if (pGui->pvDriver) {
    gslc_tsDriver*  pDriver = (gslc_tsDriver*)(pGui->pvDriver);

    #if defined(DRV_DISP_SDL1)
    pDriver->pSurfScreen = NULL;
    pGui->bRedrawPartialEn = true;
    #endif

    #if defined(DRV_DISP_SDL2)
    pDriver->pWind       = NULL;
    pDriver->pRender     = NULL;
    // In SDL2, always need full page redraw since backbuffer
    // is treated as invalidated after every RenderPresent()
    pGui->bRedrawPartialEn = false;
    #endif
  }


#if (DRV_SDL_FIX_START)
  // Force a clean start to SDL to workaround any bad state
  // left behind by a previous SDL application's failure to
  // clean up.
  // TODO: Allow compiler option to skip this workaround
  // TODO: Allow determination of the TTY to use
  gslc_DrvCleanStart(DRV_SDL_FIX_TTY);
#endif

  // Setup SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    GSLC_DEBUG_PRINT("ERROR: DrvInit() error in SDL_Init(): %s\n",SDL_GetError());
    return false;
  }
  // Now that we have successfully initialized SDL
  // we need to register an exit handler so that SDL_Quit()
  // gets called at program termination. If we don't do this
  // then some types of errors/aborts will result in
  // the SDL environment being left in a bad state that
  // affects the next SDL program execution.
  atexit(SDL_Quit);

  // Report any debug info (after init) if enabled
  #if defined(DBG_DRIVER)
  gslc_DrvReportInfoPost();
  #endif


  gslc_tsDriver* pDriver = (gslc_tsDriver*)(pGui->pvDriver);

#if defined(DRV_DISP_SDL1)
  // Set up pGui->pSurfScreen
  const SDL_VideoInfo*  videoInfo = SDL_GetVideoInfo();
  int16_t               nSystemX = videoInfo->current_w;
  int16_t               nSystemY = videoInfo->current_h;
  uint8_t               nBpp = videoInfo->vfmt->BitsPerPixel ;

  // Save a copy of the display dimensions
  pGui->nDispW      = nSystemX;
  pGui->nDispH      = nSystemY;
  pGui->nDispDepth  = nBpp;

  #if defined(DBG_DRIVER)
  printf("DBG: Video mode: %u x %u x %u bit/pixel\n",
          pGui->nDispW,pGui->nDispH,pGui->nDispDepth);
  #endif

  // SDL_SWSURFACE is apparently more reliable
  pDriver->pSurfScreen = SDL_SetVideoMode(nSystemX,nSystemY,nBpp,SDL_SWSURFACE | SDL_FULLSCREEN);
  if (!pDriver->pSurfScreen) {
    GSLC_DEBUG_PRINT("ERROR: DrvInit() error in SDL_SetVideoMode(): %s\n",SDL_GetError());
    return false;
  }
#endif

#if defined(DRV_DISP_SDL2)
  // Default to using OpenGL rendering engine and full-screen
  // When using SDL_WINDOW_FULLSCREEN, the width & height dimensions are ignored
  pDriver->pWind = SDL_CreateWindow("GUIslice",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,
          0,0,SDL_WINDOW_FULLSCREEN | SDL_WINDOW_OPENGL);
  if (!pDriver->pWind) {
    GSLC_DEBUG_PRINT("ERROR: DrvInit() error in SDL_CreateWindow(): %s\n",SDL_GetError());
    return false;
  }

  // Save a copy of the display dimensions from the created fullscreen window
  int nSystemX,nSystemY;
  SDL_GetWindowSize(pDriver->pWind,&nSystemX,&nSystemY);
  pGui->nDispW = nSystemX;
  pGui->nDispH = nSystemY;

  // Fetch the bit depth of the first display
  SDL_DisplayMode sDispMode;
  int nRet = SDL_GetCurrentDisplayMode(0,&sDispMode);
  if (nRet != 0) {
    // TODO: ERROR
    return false;
  }
  pGui->nDispDepth = SDL_BITSPERPIXEL(sDispMode.format);

  #if defined(DBG_DRIVER)
  printf("DBG: Video mode: %u x %u x %u bit/pixel\n",
          pGui->nDispW,pGui->nDispH,pGui->nDispDepth);
  #endif

  // Create renderer
  // TODO: Resolve performance with "accelerated" renderer (SDL_RENDERER_ACCELERATED.
  //       For now, use software renderer (SDL_RENDERER_SOFTWARE) which appears
  //       to be much faster.

  // If we are sure that the OpenGL ES 2.0 driver has been installed, we
  // can request that SDL use this as a renderer. Disable this for now.
  // SDL_SetHint(SDL_HINT_RENDER_DRIVER,"opengles2");

  #if (DRV_SDL_RENDER_ACCEL)
  pDriver->pRender = SDL_CreateRenderer(pDriver->pWind,-1,SDL_RENDERER_ACCELERATED);
  #else
  pDriver->pRender = SDL_CreateRenderer(pDriver->pWind,-1,SDL_RENDERER_SOFTWARE);
  #endif
  if (!pDriver->pRender) {
    GSLC_DEBUG_PRINT("ERROR: DrvInit() error in SDL_CreateRenderer(): %s\n",SDL_GetError());
    return false;
  }

  #if defined(DBG_DRIVER)
  SDL_RendererInfo  sRendInfo;
  SDL_GetRendererInfo(pDriver->pRender,&sRendInfo);
  printf("DBG: Renderer selected: [%s]\n",
          sRendInfo.name);
  #endif

  // If we wanted to support scaling of the renderer, we would call
  // SDL_RenderSetLogicalSize() here. For now, don't scale.

#endif

  // Initialize font engine
  if (TTF_Init() == -1) {
    GSLC_DEBUG_PRINT("ERROR: DrvInit(%s) error in TTF_Init()\n","");
    return false;
  }

  // Since the mouse cursor is based on SDL coords which are badly
  // scaled when in touch mode, we will disable the mouse pointer.
  // Note that the SDL coords seem to be OK when in actual mouse mode.
  #if (!DRV_SDL_MOUSE_SHOW)
  SDL_ShowCursor(SDL_DISABLE);
  #endif

  return true;
}


void gslc_DrvDestruct(gslc_tsGui* pGui)
{
#if defined(DRV_DISP_SDL2)
  gslc_tsDriver* pDriver = (gslc_tsDriver*)(pGui->pvDriver);
  if (pDriver->pRender) {
    SDL_DestroyRenderer(pDriver->pRender);
    pDriver->pRender = NULL;
  }
  if (pDriver->pWind) {
    SDL_DestroyWindow(pDriver->pWind);
    pDriver->pWind = NULL;
  }
#endif

  // Close down SDL
  SDL_Quit();
}

// -----------------------------------------------------------------------
// Image/surface handling Functions
// -----------------------------------------------------------------------

void* gslc_DrvLoadImage(gslc_tsGui* pGui,gslc_tsImgRef sImgRef)
{

  if (sImgRef.eImgFlags == GSLC_IMGREF_NONE) {
    return NULL;
  } else if ((sImgRef.eImgFlags & GSLC_IMGREF_SRC) == GSLC_IMGREF_SRC_SD) {
    // Load image from SD card
    // TODO: Not yet supported
    return NULL;
  } else if ((sImgRef.eImgFlags & GSLC_IMGREF_SRC) == GSLC_IMGREF_SRC_RAM) {
    // Load image from RAM
    // TODO: Not yet supported
    return NULL;
  } else if ((sImgRef.eImgFlags & GSLC_IMGREF_SRC) == GSLC_IMGREF_SRC_PROG) {
    // Load image from FLASH
    // TODO: Not yet supported
    return NULL;
  } else if ((sImgRef.eImgFlags & GSLC_IMGREF_SRC) == GSLC_IMGREF_SRC_FILE) {
    // Load image from file system
    const char* pStrFname = sImgRef.pFname;

    // Pointer to the surface image that was loaded
    SDL_Surface* pSurfLoaded = NULL;

    // Load the image
    // - The SDL_LoadBMP() routine should be able to handle a multitude of
    //   BMP format types.
    // - TODO: Check (eImgFlags & GSLC_IMGREF_FMT) to ensure type is supported
    pSurfLoaded = SDL_LoadBMP(pStrFname);

    // Confirm that the image loaded correctly
    if (pSurfLoaded == NULL) {
      GSLC_DEBUG_PRINT("ERROR: DrvLoadBmpFile(%s) failed: %s\n",pStrFname,SDL_GetError());
      return NULL;
    }

    #if defined(DRV_DISP_SDL1)

    //Create an optimized surface

    //The optimized surface that will be used
    SDL_Surface* pSurfOptimized = SDL_DisplayFormat( pSurfLoaded );

    //Free the old surface
    SDL_FreeSurface( pSurfLoaded );

    //If the surface was optimized
    if( pSurfOptimized != NULL ) {

      // Support optional transparency
      if (GSLC_BMP_TRANS_EN) {
        // Color key surface
        // - Use transparency color key defined in BMP_TRANS_RGB
        SDL_SetColorKey( pSurfOptimized, SDL_SRCCOLORKEY,
          SDL_MapRGB( pSurfOptimized->format, GSLC_BMP_TRANS_RGB ) );
      } // GSLC_BMP_TRANS_EN
    }

    //Return the optimized surface
    return (void*)(pSurfOptimized);

    #endif

    #if defined(DRV_DISP_SDL2)
    gslc_tsDriver* pDriver = (gslc_tsDriver*)(pGui->pvDriver);
    SDL_Texture*  pTex = NULL;

    if( pSurfLoaded != NULL ) {

      // Support optional transparency
      if (GSLC_BMP_TRANS_EN) {
        // Color key surface
        // - Use transparency color key defined in BMP_TRANS_RGB
        // - SDL2 passes SDL_TRUE instead of SDL_SRCCOLORKEY
        SDL_SetColorKey( pSurfLoaded, SDL_TRUE,
          SDL_MapRGB( pSurfLoaded->format, GSLC_BMP_TRANS_RGB ) );
      } // GSLC_BMP_TRANS_EN
    }

    pTex = (void*)SDL_CreateTextureFromSurface(pDriver->pRender,pSurfLoaded);
    if (pTex == NULL) {
      GSLC_DEBUG_PRINT("ERROR: DrvLoadBmp(%s) SDL_CreateTextureFromSurface() failed: %s\n",pStrFname,SDL_GetError());
      return NULL;
    }

    // Dispose of surface
    SDL_FreeSurface(pSurfLoaded);
    pSurfLoaded = NULL;

    //Return the texture
    return (void*)pTex;

    #endif

  } // eImgFlags

  // If reached here, it is an error
  return NULL;
}


bool gslc_DrvSetBkgndImage(gslc_tsGui* pGui,gslc_tsImgRef sImgRef)
{
  // Dispose of previous background
  if (pGui->sImgRefBkgnd.eImgFlags != GSLC_IMGREF_NONE) {
    gslc_DrvImageDestruct(pGui->sImgRefBkgnd.pvImgRaw);
    pGui->sImgRefBkgnd = gslc_ResetImage();
  }

  pGui->sImgRefBkgnd = sImgRef;
  pGui->sImgRefBkgnd.pvImgRaw = gslc_DrvLoadImage(pGui,sImgRef);
  if (pGui->sImgRefBkgnd.pvImgRaw == NULL) {
    GSLC_DEBUG_PRINT("ERROR: DrvSetBkgndImage(%s) failed\n","");
    return false;
  }

  return true;
}

bool gslc_DrvSetBkgndColor(gslc_tsGui* pGui,gslc_tsColor nCol)
{

  // Dispose of previous background
  if (pGui->sImgRefBkgnd.eImgFlags != GSLC_IMGREF_NONE) {
    gslc_DrvImageDestruct(pGui->sImgRefBkgnd.pvImgRaw);
    pGui->sImgRefBkgnd = gslc_ResetImage();
  }

  SDL_Surface* pSurfBkgnd = NULL;

  uint16_t nScreenW = pGui->nDispW;
  uint16_t nScreenH = pGui->nDispH;
  uint8_t  nBpp     = pGui->nDispDepth;

  // Create surface that we can draw into
  // - For the masks, we can pass 0 to get defaults
#if defined(DRV_DISP_SDL1)
  pSurfBkgnd = SDL_CreateRGBSurface(SDL_SWSURFACE,nScreenW,nScreenH,nBpp,0,0,0,0);
#endif
#if defined(DRV_DISP_SDL2)
  gslc_tsDriver* pDriver = (gslc_tsDriver*)(pGui->pvDriver);
  // - In SDL2, the flags field is ignored, so set to 0
  pSurfBkgnd = SDL_CreateRGBSurface(0,nScreenW,nScreenH,nBpp,0,0,0,0);
#endif

  if (pSurfBkgnd == NULL) {
    GSLC_DEBUG_PRINT("ERROR: DrvSetBkgndColor() SDL_CreateRGBSurface failed: %s\n",SDL_GetError());
    return false;
  }

  // Fill with requested color
  SDL_FillRect(pSurfBkgnd,NULL,
    SDL_MapRGB(pSurfBkgnd->format,nCol.r,nCol.g,nCol.b));

#if defined(DRV_DISP_SDL1)
  // Save surface into GUI struct
  pGui->sImgRefBkgnd.pvImgRaw = (void*)(pSurfBkgnd);
#endif
#if defined(DRV_DISP_SDL2)
  // Convert to texture and save into GUI struct
  pGui->sImgRefBkgnd.pvImgRaw = (void*)SDL_CreateTextureFromSurface(pDriver->pRender,pSurfBkgnd);
  if (pGui->sImgRefBkgnd.pvImgRaw == NULL) {
    GSLC_DEBUG_PRINT("ERROR: DrvSetBkgndColor() SDL_CreateTextureFromSurface failed: %s\n",SDL_GetError());
    return false;
  }
  // Dispose of temporary surface
  SDL_FreeSurface(pSurfBkgnd);
  pSurfBkgnd = NULL;
#endif

  return true;
}


bool gslc_DrvSetElemImageNorm(gslc_tsGui* pGui,gslc_tsElem* pElem,gslc_tsImgRef sImgRef)
{
  if (pElem->sImgRefNorm.pvImgRaw != NULL) {
    GSLC_DEBUG_PRINT("ERROR: DrvSetElemImageNorm(%s) with pvImgRaw already set\n","");
    return false;
  }

  pElem->sImgRefNorm = sImgRef;
  pElem->sImgRefNorm.pvImgRaw = gslc_DrvLoadImage(pGui,sImgRef);
  if (pElem->sImgRefNorm.pvImgRaw == NULL) {
    GSLC_DEBUG_PRINT("ERROR: DrvSetElemImageNorm(%s) failed\n","");
    return false;
  }
  return true;
}


bool gslc_DrvSetElemImageGlow(gslc_tsGui* pGui,gslc_tsElem* pElem,gslc_tsImgRef sImgRef)
{
  if (pElem->sImgRefGlow.pvImgRaw != NULL) {
    GSLC_DEBUG_PRINT("ERROR: DrvSetElemImageGlow(%s) with pvImgRaw already set\n","");
    return false;
  }

  pElem->sImgRefGlow = sImgRef;
  pElem->sImgRefGlow.pvImgRaw = gslc_DrvLoadImage(pGui,sImgRef);
  if (pElem->sImgRefGlow.pvImgRaw == NULL) {
    GSLC_DEBUG_PRINT("ERROR: DrvSetElemImageGlow(%s) failed\n","");
    return false;
  }
  return true;
}


void gslc_DrvImageDestruct(void* pvImg)
{
  if (pvImg == NULL) {
    return;
  }
  #if defined(DRV_DISP_SDL1)
  SDL_FreeSurface((SDL_Surface*)pvImg);
  #endif
  #if defined(DRV_DISP_SDL2)
  SDL_DestroyTexture((SDL_Texture*)pvImg);
  #endif
}

bool gslc_DrvSetClipRect(gslc_tsGui* pGui,gslc_tsRect* pRect)
{
  gslc_tsDriver* pDriver = (gslc_tsDriver*)(pGui->pvDriver);
#if defined(DRV_DISP_SDL1)
  SDL_Surface*  pScreen = pDriver->pSurfScreen;
  if (pRect == NULL) {
    SDL_SetClipRect(pScreen,NULL);
  } else {
    SDL_Rect  rSRect = gslc_DrvAdaptRect(*pRect);
    SDL_SetClipRect(pScreen,&rSRect);
  }
  return true;
#endif

#if defined(DRV_DISP_SDL2)
  SDL_Renderer*  pRender = pDriver->pRender;
  if (pRect == NULL) {
    SDL_RenderSetClipRect(pRender,NULL);
  } else {
    SDL_Rect  rSRect = gslc_DrvAdaptRect(*pRect);
    SDL_RenderSetClipRect(pRender,&rSRect);
  }
  return true;
#endif

}



// -----------------------------------------------------------------------
// Font handling Functions
// -----------------------------------------------------------------------

const void* gslc_DrvFontAdd(gslc_teFontRefType eFontRefType,const void* pvFontRef,uint16_t nFontSz)
{
  // UNIX/SDL mode currently only supports font definitions from files
  if (eFontRefType != GSLC_FONTREF_FNAME) {
    GSLC_DEBUG_PRINT("ERROR: DrvFontAdd(%s) failed - SDL mode only supports file-based fonts\n","");
    return NULL;
  }
  TTF_Font*   pFont;
  // In the case of SDL, the pvFontRef is a string that defines the
  // file path to the font file
  pFont = TTF_OpenFont((const char*)pvFontRef,nFontSz);
  if (pFont == NULL) {
    GSLC_DEBUG_PRINT("ERROR: DrvFontAdd(%s) failed in TTF_OpenFont failed\n",pvFontRef);
    return NULL;
  }
  return (const void*)pFont;
}

void gslc_DrvFontsDestruct(gslc_tsGui* pGui)
{
  uint16_t  nFontInd;
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

bool gslc_DrvGetTxtSize(gslc_tsGui* pGui,gslc_tsFont* pFont,const char* pStr,gslc_teTxtFlags eTxtFlags,
        int16_t* pnTxtX,int16_t* pnTxtY,uint16_t* pnTxtSzW,uint16_t* pnTxtSzH)
{
  // NOTE: Shouldn't need to process eTxtFlags
  int32_t nTxtSzW,nTxtSzH;
  TTF_Font* pDrvFont = (TTF_Font*)(pFont->pvFont);
  if ((eTxtFlags & GSLC_TXT_ENC) == GSLC_TXT_ENC_UTF8) {
    TTF_SizeUTF8(pDrvFont,pStr,&nTxtSzW,&nTxtSzH);
  } else {
    TTF_SizeText(pDrvFont,pStr,&nTxtSzW,&nTxtSzH);
  }
  *pnTxtSzW = (uint16_t)nTxtSzW;
  *pnTxtSzH = (uint16_t)nTxtSzH;
  // No offset coordinates used
  *pnTxtX = 0;
  *pnTxtY = 0;
  return true;
}


bool gslc_DrvDrawTxt(gslc_tsGui* pGui,int16_t nTxtX,int16_t nTxtY,gslc_tsFont* pFont,const char* pStr,gslc_teTxtFlags eTxtFlags,gslc_tsColor colTxt)
{
  if ((pGui == NULL) || (pFont == NULL)) {
    GSLC_DEBUG_PRINT("ERROR: DrvDrawTxt(%s) with NULL ptr\n","");
    return false;
  }
  if ((pStr == NULL) || (pStr[0] == '\0')) {
    return true;
  }

  gslc_tsDriver*  pDriver   = (gslc_tsDriver*)(pGui->pvDriver);
  SDL_Surface*    pSurfTxt  = NULL;
  TTF_Font*       pDrvFont  = (TTF_Font*)(pFont->pvFont);
  if ((eTxtFlags & GSLC_TXT_ENC) == GSLC_TXT_ENC_UTF8) {
    pSurfTxt = TTF_RenderUTF8_Blended(pDrvFont,pStr,gslc_DrvAdaptColor(colTxt));
  } else {
    pSurfTxt = TTF_RenderText_Blended(pDrvFont,pStr,gslc_DrvAdaptColor(colTxt));
  }
  if (pSurfTxt == NULL) {
    GSLC_DEBUG_PRINT("ERROR: DrvDrawTxt() failed in TTF_RenderText_Solid() (%s)\n",pStr);
    return false;
  }


#if defined(DRV_DISP_SDL1)
  gslc_DrvPasteSurface(pGui,nTxtX,nTxtY,pSurfTxt,pDriver->pSurfScreen);
#endif
#if defined(DRV_DISP_SDL2)
  SDL_Rect rRect = (SDL_Rect){nTxtX,nTxtY,pSurfTxt->w,pSurfTxt->h};
  SDL_Renderer* pRender = pDriver->pRender;
  SDL_Texture* pTex = SDL_CreateTextureFromSurface(pRender,pSurfTxt);
  if (pTex == NULL) {
    GSLC_DEBUG_PRINT("ERROR: DrvDrawTxt() error in SDL_CreateTextureFromSurface(): %s\n",SDL_GetError());
    return false;
  }
  SDL_RenderCopy(pRender,pTex,NULL,&rRect);

  // Destroy texture
  // TODO: Consider if worth optimizing by retaining texture in case
  //       we need to redraw it without changing content
  SDL_DestroyTexture(pTex);
  pTex = NULL;
#endif

  // Dispose of temporary surface
  if (pSurfTxt != NULL) {
    SDL_FreeSurface(pSurfTxt);
    pSurfTxt = NULL;
  }

  return true;
}


// -----------------------------------------------------------------------
// Screen Management Functions
// -----------------------------------------------------------------------

void gslc_DrvPageFlipNow(gslc_tsGui* pGui)
{
  gslc_tsDriver* pDriver = (gslc_tsDriver*)(pGui->pvDriver);
#if defined(DRV_DISP_SDL1)
  SDL_Surface*   pScreen = pDriver->pSurfScreen;
  SDL_Flip(pScreen);
#endif
#if defined(DRV_DISP_SDL2)
  SDL_Renderer* pRender = pDriver->pRender;
  if (pRender) {
    // Flip the offscreen buffer so we can display our drawing output
    SDL_RenderPresent(pRender);
    // Clear the drawing before any new drawing occurs
    SDL_SetRenderDrawColor(pRender,0x00,0x00,0x00,0xFF);
    SDL_RenderClear(pRender);
  }
#endif
}


// -----------------------------------------------------------------------
// Graphics Primitives Functions
// -----------------------------------------------------------------------


bool gslc_DrvDrawPoint(gslc_tsGui* pGui,int16_t nX,int16_t nY,gslc_tsColor nCol)
{
#if defined(DRV_DISP_SDL1)
  if (gslc_DrvScreenLock(pGui)) {
    uint32_t nColRaw = gslc_DrvAdaptColorRaw(pGui,nCol);
    gslc_DrvDrawSetPixelRaw(pGui,nX,nY,nColRaw);
    gslc_DrvScreenUnlock(pGui);
  }
#endif
#if defined(DRV_DISP_SDL2)
  gslc_tsDriver* pDriver = (gslc_tsDriver*)(pGui->pvDriver);
  SDL_Renderer*  pRender = pDriver->pRender;
  SDL_SetRenderDrawColor(pRender,nCol.r,nCol.g,nCol.b,255);

  // Call SDL optimized routine
  SDL_RenderDrawPoint(pRender,nX,nY);
#endif
  return true;
}


bool gslc_DrvDrawPoints(gslc_tsGui* pGui,gslc_tsPt* asPt,uint16_t nNumPt,gslc_tsColor nCol)
{
#if defined(DRV_DISP_SDL1)
  uint16_t nIndPt;
  if (gslc_DrvScreenLock(pGui)) {
    uint32_t nColRaw = gslc_DrvAdaptColorRaw(pGui,nCol);
    for (nIndPt=0;nIndPt<nNumPt;nIndPt++) {
      gslc_DrvDrawSetPixelRaw(pGui,asPt[nIndPt].x,asPt[nIndPt].y,nColRaw);
    }
    gslc_DrvScreenUnlock(pGui);
  }
#endif
#if defined(DRV_DISP_SDL2)
  gslc_tsDriver* pDriver = (gslc_tsDriver*)(pGui->pvDriver);
  SDL_Renderer* pRender = pDriver->pRender;
  // NOTE: gslc_tsPt is defined to have the same layout as SDL_Point
  //       so we simply typecast it here. This saves us from having
  //       to perform any malloc() and type conversion.
  SDL_SetRenderDrawColor(pRender,nCol.r,nCol.g,nCol.b,255);

  // Call SDL optimized routine
  SDL_RenderDrawPoints(pRender,(SDL_Point*)asPt,(int)nNumPt);
#endif
  return true;
}

bool gslc_DrvDrawFillRect(gslc_tsGui* pGui,gslc_tsRect rRect,gslc_tsColor nCol)
{
  gslc_tsDriver* pDriver = (gslc_tsDriver*)(pGui->pvDriver);
#if defined(DRV_DISP_SDL1)
  // Typecast
  SDL_Rect      rSRect  = gslc_DrvAdaptRect(rRect);
  SDL_Surface*  pScreen = pDriver->pSurfScreen;

  // Call SDL optimized routine
  SDL_FillRect(pScreen,&rSRect,
    SDL_MapRGB(pScreen->format,nCol.r,nCol.g,nCol.b));
#endif
#if defined(DRV_DISP_SDL2)
  SDL_Renderer* pRender = pDriver->pRender;
  SDL_SetRenderDrawColor(pRender,nCol.r,nCol.g,nCol.b,255);

  // Call SDL optimized routine
  SDL_Rect  rRectSdl;
  rRectSdl = gslc_DrvAdaptRect(rRect);
  SDL_RenderFillRect(pRender,&rRectSdl);
#endif
  return true;
}

bool gslc_DrvDrawFrameRect(gslc_tsGui* pGui,gslc_tsRect rRect,gslc_tsColor nCol)
{
#if defined(DRV_DISP_SDL1)
  return false;
#endif
#if defined(DRV_DISP_SDL2)
  gslc_tsDriver* pDriver = (gslc_tsDriver*)(pGui->pvDriver);
  SDL_Renderer* pRender  = pDriver->pRender;
  SDL_SetRenderDrawColor(pRender,nCol.r,nCol.g,nCol.b,255);

  // Call SDL optimized routine
  SDL_Rect  rRectSdl;
  rRectSdl = gslc_DrvAdaptRect(rRect);
  SDL_RenderDrawRect(pRender,&rRectSdl);
  return true;
#endif
}


bool gslc_DrvDrawLine(gslc_tsGui* pGui,int16_t nX0,int16_t nY0,int16_t nX1,int16_t nY1,gslc_tsColor nCol)
{
#if defined(DRV_DISP_SDL1)
  // ERROR
  return false;
#endif
#if defined(DRV_DISP_SDL2)
  gslc_tsDriver* pDriver = (gslc_tsDriver*)(pGui->pvDriver);
  SDL_Renderer* pRender  = pDriver->pRender;
  SDL_SetRenderDrawColor(pRender,nCol.r,nCol.g,nCol.b,255);

  // Call SDL optimized routine
  SDL_RenderDrawLine(pRender,nX0,nY0,nX1,nY1);
  return true;
#endif
}

bool gslc_DrvDrawImage(gslc_tsGui* pGui,int16_t nDstX,int16_t nDstY,gslc_tsImgRef sImgRef)
{
  if (pGui == NULL) {
    GSLC_DEBUG_PRINT("ERROR: DrvDrawImage(%s) with NULL ptr\n","");
    return false;
  }
  // GUIslice adapter library for SDL always pre-loads
  // surfaces / textures before calling DrvDrawImage(), so
  // we just need to confirm that the raw image data is defined.
  void* pImage = sImgRef.pvImgRaw;
  if (pImage == NULL) {
    GSLC_DEBUG_PRINT("ERROR: DrvDrawImage(%s) with NULL pvImgRaw\n","");
    return false;
  }

  gslc_tsDriver* pDriver = (gslc_tsDriver*)(pGui->pvDriver);

#if defined(DRV_DISP_SDL1)
  gslc_DrvPasteSurface(pGui,nDstX,nDstY,pImage,pDriver->pSurfScreen);
#endif

#if defined(DRV_DISP_SDL2)
  SDL_Renderer* pRender = pDriver->pRender;
  SDL_Texture*  pTex    = (SDL_Texture*)pImage;

  // Determine dest rect based on source texture dimensions and parameterized offset
  SDL_Rect  rDest;
  rDest.x = nDstX;
  rDest.y = nDstY;
  SDL_QueryTexture(pTex,NULL,NULL,&rDest.w,&rDest.h);

  // Default to copying all of source texture rect by specifying NULL
  SDL_RenderCopy(pRender,pTex,NULL,&rDest);
#endif

  return true;
}


/// NOTE: Background image is stored in pGui->sImgRefBkgnd
void gslc_DrvDrawBkgnd(gslc_tsGui* pGui)
{
  if (pGui == NULL) {
    GSLC_DEBUG_PRINT("ERROR: DrvDrawBkgnd(%s) with NULL ptr\n","");
    return;
  }
  // GUIslice adapter library for SDL always pre-loads
  // surfaces / textures before calling DrvDrawBkgnd(), so
  // we just need to confirm that the raw image data is defined.
  void* pImage = pGui->sImgRefBkgnd.pvImgRaw;
  if (pImage == NULL) {
    GSLC_DEBUG_PRINT("ERROR: DrvDrawBkgnd(%s) with NULL pvImgRaw\n","");

    // Since the image load failed, resort to black background
    gslc_DrvSetBkgndColor(pGui,GSLC_COL_BLACK);
    return;
  }

  gslc_tsDriver* pDriver = (gslc_tsDriver*)(pGui->pvDriver);

#if defined(DRV_DISP_SDL1)
  gslc_DrvPasteSurface(pGui,0,0,pGui->sImgRefBkgnd.pvImgRaw,pDriver->pSurfScreen);
#endif

#if defined(DRV_DISP_SDL2)
  SDL_Renderer* pRender = pDriver->pRender;
  SDL_Texture*  pTex    = (SDL_Texture*)(pGui->sImgRefBkgnd.pvImgRaw);

  // Determine destination rect

  // TODO: Support other background modes such as:
  // - Single Corner Unscaled
  // - Single Centered Unscaled
  // - Single Centered Scaled
  // - Tiled Corner Unscaled

  // For "single centered scaled", we fetch entire viewport
  SDL_Rect  rDest;
  SDL_RenderGetViewport(pDriver->pRender,&rDest);
  rDest.x = 0;
  rDest.y = 0;

  // Default to copying all of source texture rect by specifying NULL
  SDL_RenderCopy(pRender,pTex,NULL,&rDest);
#endif

}



// ------------------------------------------------------------------------
// Touch Functions (via SDL)
// ------------------------------------------------------------------------

// POST:
// - pDriver->pTsDev mapped to touchscreen device
bool gslc_DrvInitTouch(gslc_tsGui* pGui,const char* acDev)
{
  if (pGui == NULL) {
    GSLC_DEBUG_PRINT("ERROR: DrvInitTouch(%s) called with NULL ptr\n","");
    return false;
  }

  // Perform any driver-specific touchscreen init here

  #if defined(DRV_TOUCH_TSLIB)
    // Assign default
    gslc_tsDriver* pDriver = (gslc_tsDriver*)(pGui->pvDriver);
    pDriver->pTsDev = NULL;
  #endif

  // Nothing further to do with SDL driver
  return true;
}



bool gslc_DrvGetTouch(gslc_tsGui* pGui,int16_t* pnX,int16_t* pnY,uint16_t* pnPress)
{
  if (pGui == NULL) {
    GSLC_DEBUG_PRINT("ERROR: DrvGetTouch(%s) called with NULL ptr\n","");
    return false;
  }

  // Use SDL for touch events
  bool        bRet = false;
  SDL_Event   sEvent;
  int32_t     nX,nY;
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
      #if defined(DRV_DISP_SDL1)
      SDL_GetMouseState(&nX,&nY);
      #elif defined(DRV_DISP_SDL2)
      nX = sEvent.button.x;
      nY = sEvent.button.y;
      #endif
      *pnX = (int16_t)nX;
      *pnY = (int16_t)nY;
      // For MOUSEMOTION, we want to return the previous state of
      // the touch pressure in pnPress as MOUSEMOTION is called during
      // both mouse up and mouse down states.
      // Note that we can't simply leave pnPress as-is since it
      // doesn't retain its state in the caller.
      *pnPress = pGui->nTouchLastPress;
      bRet = true;
    } else if (sEvent.type == SDL_MOUSEBUTTONDOWN) {
      #if defined(DRV_DISP_SDL1)
      SDL_GetMouseState(&nX,&nY);
      #elif defined(DRV_DISP_SDL2)
      nX = sEvent.button.x;
      nY = sEvent.button.y;
      #endif
      *pnX = (int16_t)nX;
      *pnY = (int16_t)nY;
      (*pnPress) = 1;
      bRet = true;
    } else if (sEvent.type == SDL_MOUSEBUTTONUP) {
      #if defined(DRV_DISP_SDL1)
      SDL_GetMouseState(&nX,&nY);
      #elif defined(DRV_DISP_SDL2)
      nX = sEvent.motion.x;
      nY = sEvent.motion.y;
      #endif
      *pnX = (int16_t)nX;
      *pnY = (int16_t)nY;
      (*pnPress) = 0;
      bRet = true;


    // SDL2 defines touch events instead of reusing mouse events
    #if defined(DRV_DISP_SDL2)

    } else if (sEvent.type == SDL_FINGERMOTION) {
      *pnX = (int16_t)(sEvent.tfinger.x);
      *pnY = (int16_t)(sEvent.tfinger.y);
      // For FINGERMOTION, we want to return the previous state of
      // the touch pressure in pnPress as FINGERMOTION is called during
      // both up and down states.
      // Note that we can't simply leave pnPress as-is since it
      // doesn't retain its state in the caller.
      *pnPress = pGui->nTouchLastPress;
      bRet = true;
    } else if (sEvent.type == SDL_FINGERDOWN) {
      *pnX = (int16_t)(sEvent.tfinger.x);
      *pnY = (int16_t)(sEvent.tfinger.y);
      (*pnPress) = 1;
      bRet = true;
    } else if (sEvent.type == SDL_FINGERUP) {
      *pnX = (int16_t)(sEvent.tfinger.x);
      *pnY = (int16_t)(sEvent.tfinger.y);
      (*pnPress) = 0;
      bRet = true;
    #endif  // DRV_DISP_SDL2

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
#if (DRV_SDL_FIX_START)
    int     nFD = -1;
    int     nRet = false;

    nFD = open(sTTY, O_RDONLY, 0);
    if (nFD < 0) {
        GSLC_DEBUG_PRINT( "ERROR: DrvCleanStart() failed to open console (%s): %s\n",
            sTTY,strerror(errno));
        return false;
    }
    nRet = ioctl(nFD, VT_UNLOCKSWITCH, 1);
    if (nRet != false) {
         GSLC_DEBUG_PRINT( "ERROR: DrvCleanStart() failed to unlock console (%s): %s\n",
            sTTY,strerror(errno));
        return false;
    }
    nRet = ioctl(nFD, KDSETMODE, KD_TEXT);
    if (nRet != 0) {
         GSLC_DEBUG_PRINT( "ERROR: DrvCleanStart() failed to set text mode (%s): %s\n",
            sTTY,strerror(errno));
        return false;
    }
    close(nFD);
#endif
    return true;
}

// Report some SDL debug info
void gslc_DrvReportInfoPre()
{


#if defined(DRV_DISP_SDL1)

#elif defined(DRV_DISP_SDL2)
  int16_t           nDriverInd = 0;
  int16_t           nNumDrivers = 0;

  // Video driver info
  const char*   pDriverName = NULL;
  bool          bDriverOk = false;
  nNumDrivers = SDL_GetNumVideoDrivers();
  for (nDriverInd=0;nDriverInd<nNumDrivers;nDriverInd++) {
    pDriverName = SDL_GetVideoDriver(nDriverInd);
    printf("DBG: Video driver #%2d: [%s]\n",nDriverInd,pDriverName);

    bDriverOk = false;
    #if defined(DRV_DISP_SDL1)
    if (SDL_VideoInit(pDriverName,0) == 0) {
      SDL_VideoQuit();
      bDriverOk = true;
    }
    #elif defined(DRV_DISP_SDL2)
    if (SDL_VideoInit(pDriverName) == 0) {
      SDL_VideoQuit();
      bDriverOk = true;
    }
    #endif
    if (bDriverOk) {
      printf("DBG: - OK\n");
    } else {
      printf("DBG: - Failed [%s]\n",SDL_GetError());
    }

  }
  #endif



  #if defined(DRV_DISP_SDL2)
  // Renderer info
  SDL_RendererInfo  sRendInfo;
  nNumDrivers = SDL_GetNumRenderDrivers();
  for (nDriverInd=0;nDriverInd<nNumDrivers;nDriverInd++) {
    SDL_GetRenderDriverInfo(nDriverInd,&sRendInfo);
    printf("DBG: Render driver #%2d: [%s]\n",nDriverInd,sRendInfo.name);
    printf("DBG: - Flags         = 0x%08X\n",sRendInfo.flags);
    printf("DBG: -  Software     = %u\n",(sRendInfo.flags & SDL_RENDERER_SOFTWARE)?1:0);
    printf("DBG: -  Accelerated  = %u\n",(sRendInfo.flags & SDL_RENDERER_ACCELERATED)?1:0);
    printf("DBG: -  PresentVSync = %u\n",(sRendInfo.flags & SDL_RENDERER_PRESENTVSYNC)?1:0);
  }

  #endif
}

// Report some SDL debug info
void gslc_DrvReportInfoPost()
{
  #if defined(DRV_DISP_SDL1)
  // Video driver info
  char acDriverName[40];
  SDL_VideoDriverName(acDriverName,40);
  printf("DBG: Video Driver [%s]\n",acDriverName);

  #elif defined(DRV_DISP_SDL2)
  // Display Modes
  int16_t nDispCnt,nDispInd;
  int16_t nModeInd = 0;
  bool    bModeOk;
  SDL_DisplayMode sMode = { SDL_PIXELFORMAT_UNKNOWN, 0, 0, 0, 0 };

  nDispCnt = SDL_GetNumVideoDisplays();
  printf("DBG: Display count = %u\n",nDispCnt);
  for (nDispInd=0;nDispInd<nDispCnt;nDispInd++) {
    const char* pDispName = NULL;
    bModeOk = (SDL_GetDisplayMode(nDispInd, nModeInd, &sMode) == 0)? true : false;
    pDispName = SDL_GetDisplayName(nDispInd);
    printf("DBG: Display #%2d: [%s]\n",nDispInd,pDispName);
    printf("DBG: - OK = %u\n",bModeOk?1:0);
    if (bModeOk) {
      printf("DBG: - %d x %d, %u bpp\n",sMode.w,sMode.h,SDL_BITSPERPIXEL(sMode.format));
    }
  }
  #endif
}

// -----------------------------------------------------------------------
// Conversion Functions
// -----------------------------------------------------------------------

// Simple typecasting for abstraction layer
SDL_Rect  gslc_DrvAdaptRect(gslc_tsRect rRect)
{
  SDL_Rect  rSRect;
  rSRect.x = rRect.x;
  rSRect.y = rRect.y;
  rSRect.h = rRect.h;
  rSRect.w = rRect.w;
  return rSRect;
}

// Simple typecasting for abstraction layer
SDL_Color  gslc_DrvAdaptColor(gslc_tsColor sCol)
{
  SDL_Color  sSCol;
  sSCol.r = sCol.r;
  sSCol.g = sCol.g;
  sSCol.b = sCol.b;
#if defined(DRV_DISP_SDL1)
  sSCol.unused = 0;
#endif
#if defined(DRV_DISP_SDL2)
  //???
#endif
  return sSCol;
}


// -----------------------------------------------------------------------
// Private Drawing Functions
// -----------------------------------------------------------------------


#if defined(DRV_DISP_SDL1)
uint32_t gslc_DrvAdaptColorRaw(gslc_tsGui* pGui,gslc_tsColor nCol)
{
  if (pGui == NULL) {
    GSLC_DEBUG_PRINT("ERROR: DrvAdaptColorRaw(%s) called with NULL ptr\n","");
    return 0;
  }
  gslc_tsDriver* pDriver = (gslc_tsDriver*)(pGui->pvDriver);
  SDL_Surface*   pScreen = pDriver->pSurfScreen;
  return SDL_MapRGB(pScreen->format,nCol.r,nCol.g,nCol.b);
}


// SDL1 requires direct pixel access as there is no "draw point / pixel"
// function. SDL2 has native access for point/pixel drawing so there is
// no need to access the pixel map directly.

bool gslc_DrvScreenLock(gslc_tsGui* pGui)
{

  if (pGui == NULL) {
    GSLC_DEBUG_PRINT("ERROR: DrvScreenLock(%s) called with NULL ptr\n","");
    return false;
  }
  gslc_tsDriver* pDriver = (gslc_tsDriver*)(pGui->pvDriver);
  // Typecast
  SDL_Surface*   pScreen = pDriver->pSurfScreen;

  if (SDL_MUSTLOCK(pScreen)) {
    if (SDL_LockSurface(pScreen) < 0) {
      GSLC_DEBUG_PRINT("ERROR: DrvScreenLock() can't lock screen: %s\n",SDL_GetError());
      return false;
    }
  }

  return true;
}

void gslc_DrvScreenUnlock(gslc_tsGui* pGui)
{
  if (pGui == NULL) {
    GSLC_DEBUG_PRINT("ERROR: DrvScreenUnlock(%s) called with NULL ptr\n","");
    return;
  }
  gslc_tsDriver* pDriver = (gslc_tsDriver*)(pGui->pvDriver);
  // Typecast
  SDL_Surface*   pScreen = pDriver->pSurfScreen;

  if (SDL_MUSTLOCK(pScreen)) {
    SDL_UnlockSurface(pScreen);
  }
}


// - Based on code from:
// -   https://www.libsdl.org/release/SDL-1.2.15/docs/html/guidevideo.html
uint32_t gslc_DrvDrawGetPixelRaw(gslc_tsGui* pGui, int16_t nX, int16_t nY)
{
  if (pGui == NULL) {
    GSLC_DEBUG_PRINT("ERROR: DrvDrawGetPixelRaw(%s) called with NULL ptr\n","");
    return 0;
  }
  gslc_tsDriver* pDriver = (gslc_tsDriver*)(pGui->pvDriver);
  SDL_Surface*   pScreen = pDriver->pSurfScreen;
  if (pScreen == NULL) {
    GSLC_DEBUG_PRINT("ERROR: DrvDrawGetPixelRaw(%s) screen surface NULL\n","");
    return 0;
  }
  int nBpp = pScreen->format->BytesPerPixel;

  // Handle any range violations for entire surface
  if ( (nX < 0) || (nX >= pScreen->w) ||
       (nY < 0) || (nY >= pScreen->h) ) {
    // ERROR
    GSLC_DEBUG_PRINT("ERROR: DrvDrawGetPixelRaw() out of range (%i,%i)\n",nX,nY);
    return 0;
  }

  // Here pPixel is the address to the pixel we want to get
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
      return 0;       // shouldn't happen, but avoids warnings
  }

}


// - Based on code from:
// -   https://www.libsdl.org/release/SDL-1.2.15/docs/html/guidevideo.html
// - Added range checks from surface clipping rect
void gslc_DrvDrawSetPixelRaw(gslc_tsGui* pGui,int16_t nX, int16_t nY, uint32_t nPixelVal)
{
  if (pGui == NULL) {
    GSLC_DEBUG_PRINT("ERROR: DrvDrawSetPixelRaw(%s) called with NULL ptr\n","");
    return;
  }
  gslc_tsDriver* pDriver = (gslc_tsDriver*)(pGui->pvDriver);
  SDL_Surface*   pScreen = pDriver->pSurfScreen;
  if (pScreen == NULL) {
    GSLC_DEBUG_PRINT("ERROR: DrvDrawSetPixelRaw(%s) screen surface NULL\n","");
    return;
  }
  uint8_t nBpp = pScreen->format->BytesPerPixel;

  // Handle any clipping
  if ( (nX < pScreen->clip_rect.x) || (nX >= pScreen->clip_rect.x+pScreen->clip_rect.w) ||
       (nY < pScreen->clip_rect.y) || (nY >= pScreen->clip_rect.y+pScreen->clip_rect.h) ) {
    return;
  }

  // Here pPixel is the address to the pixel we want to set
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

void gslc_DrvPasteSurface(gslc_tsGui* pGui,int16_t nX, int16_t nY, void* pvSrc, void* pvDest)
{
  if ((pGui == NULL) || (pvSrc == NULL) || (pvDest == NULL)) {
    GSLC_DEBUG_PRINT("ERROR: DrvPasteSurface(%s) called with NULL ptr\n","");
    return;
  }
  SDL_Surface*  pSrc  = (SDL_Surface*)(pvSrc);
  SDL_Surface*  pDest = (SDL_Surface*)(pvDest);
  SDL_Rect offset;
  offset.x = nX;
  offset.y = nY;
  SDL_BlitSurface(pSrc,NULL,pDest,&offset);
}

#endif



// ------------------------------------------------------------------------
// Touch Functions (via external tslib)
// ------------------------------------------------------------------------

#if defined(DRV_TOUCH_TSLIB)

// POST:
// - pDriver->pTsDev mapped to touchscreen device
bool gslc_TDrvInitTouch(gslc_tsGui* pGui,const char* acDev)
{
  if (pGui == NULL) {
    GSLC_DEBUG_PRINT("ERROR: TDrvInitTouch(%s) called with NULL ptr\n","");
    return false;
  }

  // Perform any driver-specific touchscreen init here

  // Assign default
  gslc_tsDriver* pDriver = (gslc_tsDriver*)(pGui->pvDriver);
  pDriver->pTsDev = NULL;

  // TODO: Consider using env "TSLIB_TSDEVICE" instead
  //char* pDevName = NULL;
  //pDevName = getenv("TSLIB_TSDEVICE");
  //pDriver->pTsDev = ts_open(pDevName,1);

  // Open in non-blocking mode
  pDriver->pTsDev = ts_open(acDev,1);
  if (!pDriver->pTsDev) {
    GSLC_DEBUG_PRINT("ERROR: TsOpen(%s) failed\n","");
    return false;
  }

  if (ts_config(pDriver->pTsDev)) {
    GSLC_DEBUG_PRINT("ERROR: ts_config(%s) failed\n","");
    // Clear the tslib pointer so we don't try to call it again
    pDriver->pTsDev = NULL;
    return false;
  }

  return true;
}


int gslc_TDrvGetTouch(gslc_tsGui* pGui,int16_t* pnX,int16_t* pnY,uint16_t* pnPress)
{
  if (pGui == NULL) {
    GSLC_DEBUG_PRINT("ERROR: TDrvGetTouch(%s) called with NULL ptr\n","");
    return 0;
  }
  gslc_tsDriver* pDriver = (gslc_tsDriver*)(pGui->pvDriver);
  // In case tslib was not loaded, exit now
  if (pDriver->pTsDev == NULL) {
    return 0;
  }
  struct ts_sample   pSamp;
  int32_t nRet = ts_read(pDriver->pTsDev,&pSamp,1);
  (*pnX)      = pSamp.x;
  (*pnY)      = pSamp.y;
  (*pnPress)  = pSamp.pressure;
  return nRet;
}

#endif // DRV_TOUCH_TSLIB

#endif // Compiler guard for requested driver
