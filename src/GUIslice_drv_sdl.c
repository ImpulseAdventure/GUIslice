// =======================================================================
// GUIslice library (driver layer for SDL 1.2 & 2.0)
// - Calvin Hass
// - http://www.impulseadventure.com/elec/microsdl-sdl-gui.html
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
// Driver Layer for SDL
// =======================================================================


// GUIslice library
#include "GUIslice_drv_sdl.h"

#include <stdio.h>


// Optionally enable SDL clean start VT workaround
#ifdef DRV_SDL_FIX_START
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
  
  // Primary surface definitions
  pGui->pvImgBkgnd = NULL;
  
  // Initialize any SDL version-specific members
  if (pGui->pvDriver) {
    gslc_tsDriver*  pDriver = (gslc_tsDriver*)(pGui->pvDriver);
    
    #ifdef DRV_TYPE_SDL1
    pDriver->pSurfScreen = NULL;
    pGui->bRedrawPartialEn = true;
    #endif

    #ifdef DRV_TYPE_SDL2
    pDriver->pWind       = NULL;
    pDriver->pRender     = NULL;
    // In SDL2, always need full page redraw since backbuffer
    // is treated as invalidated after every RenderPresent()
    pGui->bRedrawPartialEn = false;
    #endif
  }


#ifdef DRV_SDL_FIX_START
  // Force a clean start to SDL to workaround any bad state
  // left behind by a previous SDL application's failure to
  // clean up.
  // TODO: Allow compiler option to skip this workaround
  // TODO: Allow determination of the TTY to use
  gslc_DrvCleanStart(DRV_SDL_FIX_TTY);
#endif  

  // Setup SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    debug_print("ERROR: DrvInit() error in SDL_Init(): %s\n",SDL_GetError());
    return false;
  }
  // Now that we have successfully initialized SDL
  // we need to register an exit handler so that SDL_Quit()
  // gets called at program termination. If we don't do this
  // then some types of errors/aborts will result in
  // the SDL environment being left in a bad state that
  // affects the next SDL program execution.
  atexit(SDL_Quit);

  gslc_tsDriver* pDriver = (gslc_tsDriver*)(pGui->pvDriver);
  
#ifdef DRV_TYPE_SDL1  
  // Set up pGui->pSurfScreen
  const SDL_VideoInfo*  videoInfo = SDL_GetVideoInfo();
  int                   nSystemX = videoInfo->current_w;
  int                   nSystemY = videoInfo->current_h;
  uint8_t               nBpp = videoInfo->vfmt->BitsPerPixel ;

  // Save a copy of the display dimensions
  pGui->nDispW      = nSystemX;
  pGui->nDispH      = nSystemY;  
  pGui->nDispDepth  = nBpp;
  
  // SDL_SWSURFACE is apparently more reliable
  pDriver->pSurfScreen = SDL_SetVideoMode(nSystemX,nSystemY,nBpp,SDL_SWSURFACE);
  if (!pDriver->pSurfScreen) {
    debug_print("ERROR: DrvInit() error in SDL_SetVideoMode(): %s\n",SDL_GetError());
    return false;
  }
#endif  
#ifdef DRV_TYPE_SDL2
  // Default to using OpenGL rendering engine and full-screen
  // When using SDL_WINDOW_FULLSCREEN, the width & height dimensions are ignored
  pDriver->pWind = SDL_CreateWindow("GUIslice",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,
          0,0,SDL_WINDOW_FULLSCREEN | SDL_WINDOW_OPENGL);
  if (!pDriver->pWind) {
    debug_print("ERROR: DrvInit() error in SDL_CreateWindow(): %s\n",SDL_GetError());
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
  
  // Create renderer
  pDriver->pRender = SDL_CreateRenderer(pDriver->pWind,-1,SDL_RENDERER_ACCELERATED);
  if (!pDriver->pRender) {
    debug_print("ERROR: DrvInit() error in SDL_CreateRenderer(): %s\n",SDL_GetError());
    return false;
  }

  // If we wanted to support scaling of the renderer, we would call
  // SDL_RenderSetLogicalSize() here. For now, don't scale.
  
#endif

  // Initialize font engine
  if (TTF_Init() == -1) {
    debug_print("ERROR: DrvInit(%s) error in TTF_Init()\n","");
    return false;
  }

  // Since the mouse cursor is based on SDL coords which are badly
  // scaled when in touch mode, we will disable the mouse pointer.
  // Note that the SDL coords seem to be OK when in actual mouse mode.
  SDL_ShowCursor(SDL_DISABLE);

  return true;
}

// Need to configure a number of SDL and TS environment
// variables. The following demonstrates a way to do this
// programmatically, but it can also be done via export
// commands within the shell (or init script).
// eg. export TSLIB_FBDEVICE=/dev/fb1
void gslc_DrvInitEnv(const char* acDevFb,const char* acDevTouch)
{
#ifdef DRV_TYPE_SDL1
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
#endif  

  #ifdef DRV_INC_TS
  setenv((char*)"TSLIB_FBDEVICE",acDevFb,1);
  setenv((char*)"TSLIB_TSDEVICE",acDevTouch,1); 
  setenv((char*)"TSLIB_CALIBFILE",(char*)"/etc/pointercal",1);
  setenv((char*)"TSLIB_CONFFILE",(char*)"/etc/ts.conf",1);
  setenv((char*)"TSLIB_PLUGINDIR",(char*)"/usr/local/lib/ts",1);
  #endif

}

void gslc_DrvDestruct(gslc_tsGui* pGui)
{
#ifdef DRV_TYPE_SDL2
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

void* gslc_DrvLoadBmp(gslc_tsGui* pGui,const char* pStrFname)
{

  // Pointer to the surface image that was loaded
  SDL_Surface* pSurfLoaded = NULL;

  // Load the image
  pSurfLoaded = SDL_LoadBMP(pStrFname);

  // Confirm that the image loaded correctly
  if (pSurfLoaded == NULL) {
    debug_print("ERROR: DrvLoadBmp(%s) failed: %s\n",pStrFname,SDL_GetError());
    return NULL;
  }
  
  #ifdef DRV_TYPE_SDL1

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

  #ifdef DRV_TYPE_SDL2
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
    debug_print("ERROR: DrvLoadBmp(%s) SDL_CreateTextureFromSurface() failed: %s\n",pStrFname,SDL_GetError());
    return NULL;    
  }  

  // Dispose of surface
  SDL_FreeSurface(pSurfLoaded);
  pSurfLoaded = NULL;  

  //Return the texture
  return (void*)pTex;
  
  #endif      
    
}


bool gslc_DrvSetBkgndImage(gslc_tsGui* pGui,char* pStrFname)
{
  if (strlen(pStrFname)==0) {
    // TODO: Error handling
    return false;
  }

  // Dispose of previous background
  if (pGui->pvImgBkgnd != NULL ) {
    gslc_DrvImageDestruct(pGui->pvImgBkgnd);
    pGui->pvImgBkgnd = NULL;
  }

  pGui->pvImgBkgnd = gslc_DrvLoadBmp(pGui,pStrFname);
  if (pGui->pvImgBkgnd == NULL) {
    debug_print("ERROR: DrvSetBkgndImage(%s) failed\n","");
    return false;
  }    
  
  return true;
}

bool gslc_DrvSetBkgndColor(gslc_tsGui* pGui,gslc_Color nCol)
{
  
  // Dispose of any previous background  
  if (pGui->pvImgBkgnd != NULL ) {
    gslc_DrvImageDestruct(pGui->pvImgBkgnd);
    pGui->pvImgBkgnd = NULL;
  } 
  
  SDL_Surface* pSurfBkgnd = NULL;
  
  unsigned nScreenW = pGui->nDispW;
  unsigned nScreenH = pGui->nDispH;
  unsigned nBpp     = pGui->nDispDepth;  
  
  // Create surface that we can draw into
  // - For the masks, we can pass 0 to get defaults  
#ifdef DRV_TYPE_SDL1
  pSurfBkgnd = SDL_CreateRGBSurface(SDL_SWSURFACE,nScreenW,nScreenH,nBpp,0,0,0,0);
#endif
#ifdef DRV_TYPE_SDL2
  gslc_tsDriver* pDriver = (gslc_tsDriver*)(pGui->pvDriver);  
  // - In SDL2, the flags field is ignored, so set to 0
  pSurfBkgnd = SDL_CreateRGBSurface(0,nScreenW,nScreenH,nBpp,0,0,0,0);  
#endif
  
  if (pSurfBkgnd == NULL) {
    debug_print("ERROR: DrvSetBkgndColor() SDL_CreateRGBSurface failed: %s\n",SDL_GetError());
    return false;
  }
  
  // Fill with requested color
  SDL_FillRect(pSurfBkgnd,NULL,
    SDL_MapRGB(pSurfBkgnd->format,nCol.r,nCol.g,nCol.b));

#ifdef DRV_TYPE_SDL1
  // Save surface into GUI struct
  pGui->pvImgBkgnd = (void*)(pSurfBkgnd);
#endif
#ifdef DRV_TYPE_SDL2 
  // Convert to texture and save into GUI struct
  pGui->pvImgBkgnd = (void*)SDL_CreateTextureFromSurface(pDriver->pRender,pSurfBkgnd);
  if (pGui->pvImgBkgnd == NULL) {
    debug_print("ERROR: DrvSetBkgndColor() SDL_CreateTextureFromSurface failed: %s\n",SDL_GetError());
    return false;
  }  
  // Dispose of temporary surface
  SDL_FreeSurface(pSurfBkgnd);
  pSurfBkgnd = NULL;  
#endif
  
  return true;
}


bool gslc_DrvSetElemImageNorm(gslc_tsGui* pGui,gslc_tsElem* pElem,const char* acImage)
{
  if (pElem->pvImgNorm != NULL) {
    debug_print("ERROR: DrvSetElemImageNorm(%s) with pvImgNorm already set\n",acImage);
    return false;
  }
  pElem->pvImgNorm = gslc_DrvLoadBmp(pGui,acImage);
  if (pElem->pvImgNorm == NULL) {
    debug_print("ERROR: DrvSetElemImageNorm(%s) failed\n",acImage);
    return false;
  }    
  return true;  
}


bool gslc_DrvSetElemImageGlow(gslc_tsGui* pGui,gslc_tsElem* pElem,const char* acImage)
{
  if (pElem->pvImgGlow != NULL) {
    debug_print("ERROR: DrvSetElemImageGlow(%s) with pvImgGlow already set\n",acImage);
    return false;
  }
  pElem->pvImgGlow = gslc_DrvLoadBmp(pGui,acImage);
  if (pElem->pvImgGlow == NULL) {
    debug_print("ERROR: DrvSetElemImageGlow(%s) failed\n",acImage);
    return false;
  }    
  return true;  
}


void gslc_DrvImageDestruct(void* pvImg)
{
  if (pvImg == NULL) {
    return;
  }  
  #ifdef DRV_TYPE_SDL1  
  SDL_FreeSurface((SDL_Surface*)pvImg);
  #endif
  #ifdef DRV_TYPE_SDL2
  SDL_DestroyTexture((SDL_Texture*)pvImg);
  #endif  
}

bool gslc_DrvSetClipRect(gslc_tsGui* pGui,gslc_Rect* pRect)
{
  gslc_tsDriver* pDriver = (gslc_tsDriver*)(pGui->pvDriver);
#ifdef DRV_TYPE_SDL1
  SDL_Surface*  pScreen = pDriver->pSurfScreen;
  if (pRect == NULL) {
    SDL_SetClipRect(pScreen,NULL);
  } else {
    SDL_Rect  rSRect = gslc_DrvAdaptRect(*pRect);
    SDL_SetClipRect(pScreen,&rSRect);
  }
  return true;
#endif
  
#ifdef DRV_TYPE_SDL2
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

void* gslc_DrvFontAdd(const char* acFontName,unsigned nFontSz)
{
  TTF_Font*   pFont;
  pFont = TTF_OpenFont(acFontName,nFontSz);
  if (pFont == NULL) {
    debug_print("ERROR: DrvFontAdd(%s) failed in TTF_OpenFont failed\n",acFontName);
    return NULL;
  } 
  return (void*)pFont;
}

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
    debug_print("ERROR: DrvDrawTxt(%s) with NULL ptr\n",""); 
    return false;
  }
  TTF_Font*   pFont = (TTF_Font*)(pElem->pvTxtFont);  
  if (pFont == NULL) {
    debug_print("ERROR: DrvDrawTxt(%s) with NULL font\n",""); 
    return false;
  }

  gslc_tsDriver* pDriver = (gslc_tsDriver*)(pGui->pvDriver);
  
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
    debug_print("ERROR: DrvDrawTxt() failed in TTF_RenderText_Solid() (%s)\n",pElem->acStr);
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

#ifdef DRV_TYPE_SDL1
  gslc_DrvPasteSurface(pGui,nTxtX,nTxtY,surfTxt,pDriver->pSurfScreen);
#endif
#ifdef DRV_TYPE_SDL2
  SDL_Rect rRect = (SDL_Rect){nTxtX,nTxtY,nTxtSzW,nTxtSzH};
  SDL_Renderer* pRender = pDriver->pRender;
  SDL_Texture* pTex = SDL_CreateTextureFromSurface(pRender,surfTxt);
  if (pTex == NULL) {
    debug_print("ERROR: DrvDrawTxt() error in SDL_CreateTextureFromSurface(): %s\n",SDL_GetError());
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
  gslc_tsDriver* pDriver = (gslc_tsDriver*)(pGui->pvDriver);
#ifdef DRV_TYPE_SDL1  
  SDL_Surface*   pScreen = pDriver->pSurfScreen;  
  SDL_Flip(pScreen);
#endif
#ifdef DRV_TYPE_SDL2
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


bool gslc_DrvDrawPoint(gslc_tsGui* pGui,int nX,int nY,gslc_Color nCol)
{
#ifdef DRV_TYPE_SDL1
  if (gslc_DrvScreenLock(pGui)) {
    uint32_t nColRaw = gslc_DrvAdaptColorRaw(pGui,nCol);    
    gslc_DrvDrawSetPixelRaw(pGui,nX,nY,nColRaw);
    gslc_DrvScreenUnlock(pGui);
  }
#endif
#ifdef DRV_TYPE_SDL2
  gslc_tsDriver* pDriver = (gslc_tsDriver*)(pGui->pvDriver);
  SDL_Renderer*  pRender = pDriver->pRender;    
  SDL_SetRenderDrawColor(pRender,nCol.r,nCol.g,nCol.b,255);
  
  // Call SDL optimized routine
  SDL_RenderDrawPoint(pRender,nX,nY);  
#endif
  return true;
}


bool gslc_DrvDrawPoints(gslc_tsGui* pGui,gslc_Pt* asPt,unsigned nNumPt,gslc_Color nCol)
{
#ifdef DRV_TYPE_SDL1
  unsigned nIndPt;
  if (gslc_DrvScreenLock(pGui)) {
    uint32_t nColRaw = gslc_DrvAdaptColorRaw(pGui,nCol);
    for (nIndPt=0;nIndPt<nNumPt;nIndPt++) {
      gslc_DrvDrawSetPixelRaw(pGui,asPt[nIndPt].x,asPt[nIndPt].y,nColRaw);
    }
    gslc_DrvScreenUnlock(pGui);
  }
#endif
#ifdef DRV_TYPE_SDL2
  gslc_tsDriver* pDriver = (gslc_tsDriver*)(pGui->pvDriver);
  SDL_Renderer* pRender = pDriver->pRender;     
  // NOTE: gslc_pt is defined to have the same layout as SDL_Point
  //       so we simply typecast it here. This saves us from having
  //       to perform any malloc() and type conversion.
  SDL_SetRenderDrawColor(pRender,nCol.r,nCol.g,nCol.b,255);
  
  // Call SDL optimized routine
  SDL_RenderDrawPoints(pRender,(SDL_Point*)asPt,(int)nNumPt);  
#endif  
  return true;
}

bool gslc_DrvDrawFillRect(gslc_tsGui* pGui,gslc_Rect rRect,gslc_Color nCol)
{
  gslc_tsDriver* pDriver = (gslc_tsDriver*)(pGui->pvDriver);
#ifdef DRV_TYPE_SDL1
  // Typecast
  SDL_Rect      rSRect  = gslc_DrvAdaptRect(rRect);  
  SDL_Surface*  pScreen = pDriver->pSurfScreen;
  
  // Call SDL optimized routine
  SDL_FillRect(pScreen,&rSRect,
    SDL_MapRGB(pScreen->format,nCol.r,nCol.g,nCol.b)); 
#endif
#ifdef DRV_TYPE_SDL2
  SDL_Renderer* pRender = pDriver->pRender;    
  SDL_SetRenderDrawColor(pRender,nCol.r,nCol.g,nCol.b,255);
  
  // Call SDL optimized routine
  SDL_Rect  rRectSdl;
  rRectSdl = gslc_DrvAdaptRect(rRect);  
  SDL_RenderFillRect(pRender,&rRectSdl);  
#endif  
  return true;
}

bool gslc_DrvDrawFrameRect(gslc_tsGui* pGui,gslc_Rect rRect,gslc_Color nCol)
{
#ifdef DRV_TYPE_SDL1
  return false;
#endif
#ifdef DRV_TYPE_SDL2
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


bool gslc_DrvDrawLine(gslc_tsGui* pGui,int16_t nX0,int16_t nY0,int16_t nX1,int16_t nY1,gslc_Color nCol)
{
#ifdef DRV_TYPE_SDL1
  // ERROR
  return false;
#endif
#ifdef DRV_TYPE_SDL2
  gslc_tsDriver* pDriver = (gslc_tsDriver*)(pGui->pvDriver);
  SDL_Renderer* pRender  = pDriver->pRender;
  SDL_SetRenderDrawColor(pRender,nCol.r,nCol.g,nCol.b,255);
  
  // Call SDL optimized routine
  SDL_RenderDrawLine(pRender,nX0,nY0,nX1,nY1);
  return true;
#endif  
}

bool gslc_DrvDrawImage(gslc_tsGui* pGui,int nDstX,int nDstY,void* pImage)
{
  if ((pGui == NULL) || (pImage == NULL)) {
    debug_print("ERROR: DrvDrawImage(%s) with NULL ptr\n","");
    return false;
  }
  gslc_tsDriver* pDriver = (gslc_tsDriver*)(pGui->pvDriver);
  
#ifdef DRV_TYPE_SDL1
  gslc_DrvPasteSurface(pGui,nDstX,nDstY,pImage,pDriver->pSurfScreen);
#endif

#ifdef DRV_TYPE_SDL2  
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


/// NOTE: Background image is stored in pGui->pvImgBkgnd
void gslc_DrvDrawBkgnd(gslc_tsGui* pGui)
{
  if (pGui->pvImgBkgnd == NULL) {
    return;
  }

  if ((pGui == NULL) || (pGui->pvImgBkgnd == NULL)) {
    debug_print("ERROR: DrvDrawBkgnd(%s) with NULL ptr\n","");
    return;
  }
  gslc_tsDriver* pDriver = (gslc_tsDriver*)(pGui->pvDriver);
  
#ifdef DRV_TYPE_SDL1
  gslc_DrvPasteSurface(pGui,0,0,pGui->pvImgBkgnd,pDriver->pSurfScreen);
#endif

#ifdef DRV_TYPE_SDL2  
  SDL_Renderer* pRender = pDriver->pRender;
  SDL_Texture*  pTex    = (SDL_Texture*)(pGui->pvImgBkgnd);
  
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


// -----------------------------------------------------------------------
// Touch Functions
// -----------------------------------------------------------------------  

bool gslc_DrvGetTouch(gslc_tsGui* pGui,int* pnX,int* pnY,unsigned* pnPress)
{
  if (pGui == NULL) {
    debug_print("ERROR: DrvGetTouch(%s) called with NULL ptr\n","");
    return false;
  }    
  
  // Support use of tslib instead of native SDL for touch handling
  #ifdef DRV_INC_TS  
  return gslc_DrvGetTsTouch(pGui,pnX,pnY,pnPress);
  #endif
  
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
// Touch Functions (via tslib)
// ------------------------------------------------------------------------


// POST:
// - pDriver->pTsDev mapped to touchscreen device
bool gslc_DrvInitTs(gslc_tsGui* pGui,const char* acDev)
{
  if (pGui == NULL) {
    debug_print("ERROR: DrvInitTs(%s) called with NULL ptr\n","");
    return false;
  }    
  
  // Perform any driver-specific touchscreen init here
  
#ifdef DRV_INC_TS

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
    debug_print("ERROR: TsOpen(%s)\n","");
    return false;
  }

  if (ts_config(pDriver->pTsDev)) {
    debug_print("ERROR: TsConfig(%s)\n","");
    // Clear the tslib pointer so we don't try to call it again
    pDriver->pTsDev = NULL;
    return false;
  }
#endif
  
  return true;
}

#ifdef DRV_INC_TS
int gslc_DrvGetTsTouch(gslc_tsGui* pGui,int* pnX,int* pnY,unsigned* pnPress)
{
  if (pGui == NULL) {
    debug_print("ERROR: DrvGetTsTouch(%s) called with NULL ptr\n","");
    return 0;
  }    
  gslc_tsDriver* pDriver = (gslc_tsDriver*)(pGui->pvDriver);
  // In case tslib was not loaded, exit now
  if (pDriver->pTsDev == NULL) {
    return 0;
  }
  struct ts_sample   pSamp;
  int nRet    = ts_read(pDriver->pTsDev,&pSamp,1);
  (*pnX)      = pSamp.x;
  (*pnY)      = pSamp.y;
  (*pnPress)  = pSamp.pressure;
  return nRet;
}
#endif // DRV_INC_TS



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
#ifdef DRV_SDL_FIX_START    
    int     nFD = -1;
    int     nRet = false;
    
    nFD = open(sTTY, O_RDONLY, 0);
    if (nFD < 0) {
        debug_print( "ERROR: DrvCleanStart() failed to open console (%s): %s\n",
            sTTY,strerror(errno));
        return false;
    }
    nRet = ioctl(nFD, VT_UNLOCKSWITCH, 1);
    if (nRet != false) {
         debug_print( "ERROR: DrvCleanStart() failed to unlock console (%s): %s\n",
            sTTY,strerror(errno));
        return false;
    }
    nRet = ioctl(nFD, KDSETMODE, KD_TEXT);
    if (nRet != 0) {
         debug_print( "ERROR: DrvCleanStart() failed to set text mode (%s): %s\n",
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
#ifdef DRV_TYPE_SDL1
  sSCol.unused = 0;
#endif
#ifdef DRV_TYPE_SDL2
  //???
#endif
  return sSCol;
}


// -----------------------------------------------------------------------
// Private Drawing Functions
// -----------------------------------------------------------------------


#ifdef DRV_TYPE_SDL1
uint32_t gslc_DrvAdaptColorRaw(gslc_tsGui* pGui,gslc_Color nCol)
{
  if (pGui == NULL) {
    debug_print("ERROR: DrvAdaptColorRaw(%s) called with NULL ptr\n","");
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
    debug_print("ERROR: DrvScreenLock(%s) called with NULL ptr\n","");
    return false;
  }       
  gslc_tsDriver* pDriver = (gslc_tsDriver*)(pGui->pvDriver);
  // Typecast
  SDL_Surface*   pScreen = pDriver->pSurfScreen;  
  
  if (SDL_MUSTLOCK(pScreen)) {
    if (SDL_LockSurface(pScreen) < 0) {
      debug_print("ERROR: DrvScreenLock() can't lock screen: %s\n",SDL_GetError());
      return false;
    }
  }   

  return true;
}

void gslc_DrvScreenUnlock(gslc_tsGui* pGui)
{
  if (pGui == NULL) {
    debug_print("ERROR: DrvScreenUnlock(%s) called with NULL ptr\n","");
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
uint32_t gslc_DrvDrawGetPixelRaw(gslc_tsGui* pGui, int nX, int nY)
{
  if (pGui == NULL) {
    debug_print("ERROR: DrvDrawGetPixelRaw(%s) called with NULL ptr\n","");
    return 0;
  }
  gslc_tsDriver* pDriver = (gslc_tsDriver*)(pGui->pvDriver);
  SDL_Surface*   pScreen = pDriver->pSurfScreen;
  if (pScreen == NULL) {
    debug_print("ERROR: DrvDrawGetPixelRaw(%s) screen surface NULL\n","");
    return 0;
  }      
  int nBpp = pScreen->format->BytesPerPixel;

  // Handle any range violations for entire surface
  if ( (nX < 0) || (nX >= pScreen->w) ||
       (nY < 0) || (nY >= pScreen->h) ) {
    // ERROR
    debug_print("ERROR: DrvDrawGetPixelRaw() out of range (%i,%i)\n",nX,nY);
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
void gslc_DrvDrawSetPixelRaw(gslc_tsGui* pGui,int nX, int nY, uint32_t nPixelVal)
{
  if (pGui == NULL) {
    debug_print("ERROR: DrvDrawSetPixelRaw(%s) called with NULL ptr\n","");
    return;
  }
  gslc_tsDriver* pDriver = (gslc_tsDriver*)(pGui->pvDriver);
  SDL_Surface*   pScreen = pDriver->pSurfScreen;
  if (pScreen == NULL) {
    debug_print("ERROR: DrvDrawSetPixelRaw(%s) screen surface NULL\n","");
    return;
  }        
  int nBpp = pScreen->format->BytesPerPixel;

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

void gslc_DrvPasteSurface(gslc_tsGui* pGui,int nX, int nY, void* pvSrc, void* pvDest)
{
  if ((pGui == NULL) || (pvSrc == NULL) || (pvDest == NULL)) {
    debug_print("ERROR: DrvPasteSurface(%s) called with NULL ptr\n","");
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