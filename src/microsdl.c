// =======================================================================
// microSDL library
// - Calvin Hass
// - http:/www.impulseadventure.com/elec/microsdl-sdl-pGui->html
//
// - Version 0.2 (2016/09/25)
// =======================================================================


// MicroSDL library
#include "microsdl.h"

#include <stdio.h>
#include <math.h>

#include "SDL/SDL.h"
#include "SDL/SDL_getenv.h"
#include "SDL/SDL_ttf.h"


// Includes for tslib
#ifdef INC_TS
#include "tslib.h"
#endif

// Includes for I2C
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <errno.h>


// Version definition
#define MICROSDL_VER "0.2"

// Debug flags
//#define DBG_LOG
//#define DBG_TOUCH



// ========================================================================

void microSDL_GuiReset(microSDL_tsGui* pGui)
{

  // The page that is currently active
  pGui->nPageIdCur = MSDL_PAGE_NONE;
  
  // Collection of loaded fonts
  //pGui->asFont[FONT_MAX];
  pGui->nFontCnt = 0;

  // Collection of graphic elements (across all pages)
  for (unsigned nInd=0;nInd<ELEM_MAX;nInd++) {
    //TODO pGui->asElem[nInd] Reset?
    pGui->asElem[nInd].bValid = false;
  }
  pGui->nElemCnt = 0;
  pGui->nElemAutoIdNext = 0x8000;

  // Current touch-tracking hover status
  pGui->nTrackElemHover = MSDL_IND_NONE;
  pGui->bTrackElemHoverGlow = false;

  // Last graphic element clicked
  pGui->nTrackElemClicked = MSDL_IND_NONE;
  pGui->nClickLastX = 0;
  pGui->nClickLastY = 0;
  pGui->nClickLastPress = 0;

  // Touchscreen library interface
  #ifdef INC_TS
  pGui->ts = NULL;
  #endif

  // Primary surface definitions
  pGui->surfScreen = NULL;
  pGui->surfBkgnd = NULL;

}


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


bool microSDL_Init(microSDL_tsGui* pGui)
{
  // Setup SDL
  SDL_Init(SDL_INIT_VIDEO);

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

  // Since the mouse cursor is based on SDL coords which are badly
  // scaled when in touch mode, we will disable the mouse pointer.
  // Note that the SDL coords seem to be OK when in actual mouse mode.
  SDL_ShowCursor(SDL_DISABLE);

  // Initialize fonts
  if (!microSDL_InitFont(pGui)) {
    return false;
  }

  return true;
}

void microSDL_Quit(microSDL_tsGui* pGui)
{
  microSDL_ElemCloseAll(pGui);
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
      if (BMP_TRANS_EN) {

        // Color key surface
        // - Use pink as the transparency color key
        //SDL_SetColorKey( surfOptimized, SDL_SRCCOLORKEY, SDL_MapRGB( surfOptimized->format, 0xFF, 0x00, 0xFF ) );
        SDL_SetColorKey( surfOptimized, SDL_SRCCOLORKEY,
          SDL_MapRGB( surfOptimized->format, BMP_TRANS_RGB ) );
      } // BMP_TRANS_EN
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
  SDL_Surface*  pSurf;
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


bool microSDL_IsInRect(microSDL_tsGui* pGui,unsigned nSelX,unsigned nSelY,SDL_Rect rBtn)
{
  if ( (nSelX >= rBtn.x) && (nSelX <= rBtn.x+rBtn.w) && 
     (nSelY >= rBtn.y) && (nSelY <= rBtn.y+rBtn.h) ) {
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

void microSDL_SetPixel(microSDL_tsGui* pGui,Sint16 nX,Sint16 nY,SDL_Color nCol)
{
  if (microSDL_Lock(pGui)) {
    microSDL_PutPixelRaw(pGui,pGui->surfScreen,nX,nY,microSDL_GenPixelColor(pGui,nCol));
    microSDL_Unlock(pGui);
  }   // microSDL_Lock
}

// Draw an arbitrary line using Bresenham's algorithm
// - Algorithm reference: https://rosettacode.org/wiki/Bitmap/Bresenham's_line_algorithm#C
void microSDL_Line(microSDL_tsGui* pGui,Sint16 nX0,Sint16 nY0,Sint16 nX1,Sint16 nY1,SDL_Color nCol)
{
  Sint16 nDX = abs(nX1-nX0);
  Sint16 nSX = (nX0 < nX1)? 1 : -1;
  Sint16 nDY = abs(nY1-nY0);
  Sint16 nSY = (nY0 < nY1)? 1 : -1;
  Sint16 nErr = ( (nDX>nDY)? nDX : -nDY )/2;
  Sint16 nE2;
  for (;;) {
    microSDL_SetPixel(pGui,nX0,nY0,nCol);
    if ( (nX0 == nX1) && (nY0 == nY1) ) break;
    nE2 = nErr;
    if (nE2 > -nDX) { nErr -= nDY; nX0 += nSX; }
    if (nE2 <  nDY) { nErr += nDX; nY0 += nSY; }
  }
}

void microSDL_LineH(microSDL_tsGui* pGui,Sint16 nX, Sint16 nY, Uint16 nW,SDL_Color nCol)
{
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
  Uint16 nOffset;
  Uint32 nPixelCol = microSDL_GenPixelColor(pGui,nCol);
  if (microSDL_Lock(pGui)) {
    for (nOffset=0;nOffset<nH;nOffset++) {
      microSDL_PutPixelRaw(pGui,pGui->surfScreen,nX,nY+nOffset,nPixelCol);    
    }
    microSDL_Unlock(pGui);
  }   // microSDL_Lock
}



void microSDL_FrameRect(microSDL_tsGui* pGui,Sint16 nX,Sint16 nY,Uint16 nW,Uint16 nH,SDL_Color nCol)
{
  microSDL_LineH(pGui,nX,nY,nW,nCol);                  // Top
  microSDL_LineH(pGui,nX,(Sint16)(nY+nH),nW,nCol);     // Bottom
  microSDL_LineV(pGui,nX,nY,nH,nCol);                  // Left
  microSDL_LineV(pGui,(Sint16)(nX+nW),nY,nH,nCol);     // Right
}

void microSDL_FillRect(microSDL_tsGui* pGui,SDL_Rect rRect,SDL_Color nCol)
{
  SDL_FillRect(pGui->surfScreen,&rRect,
    SDL_MapRGB(pGui->surfScreen->format,nCol.r,nCol.g,nCol.b));
}





// -----------------------------------------------------------------------
// Font Functions
// -----------------------------------------------------------------------

bool microSDL_InitFont(microSDL_tsGui* pGui)
{
  if (TTF_Init() == -1) {
    fprintf(stderr,"ERROR: TTF_Init() failed\n");
    return false;
  }
  return true;
}

  

bool microSDL_FontAdd(microSDL_tsGui* pGui,unsigned nFontId,const char* acFontName,unsigned nFontSz)
{
  if (pGui->nFontCnt+1 >= FONT_MAX) {
    fprintf(stderr,"ERROR: microSDL_FontAdd() added too many fonts\n");
    return false;
  } else {
    TTF_Font*   pFont;
    pFont = TTF_OpenFont(acFontName,nFontSz);
    if (pFont == NULL) {
      fprintf(stderr,"ERROR: TTF_OpenFont(%s) failed\n",acFontName);
      return false;
    }
    pGui->asFont[pGui->nFontCnt].pFont = pFont;
    pGui->asFont[pGui->nFontCnt].nId = nFontId;
    pGui->nFontCnt++;  
    return true;
  }
}


TTF_Font* microSDL_FontGet(microSDL_tsGui* pGui,unsigned nFontId)
{
  unsigned nFontInd;
  for (nFontInd=0;nFontInd<pGui->nFontCnt;nFontInd++) {
    if (pGui->asFont[nFontInd].nId == nFontId) {
      return pGui->asFont[nFontInd].pFont;
    }
  }
  return NULL;
}


// TODO: Move  into microSDL_Quit()?
void microSDL_FontCloseAll(microSDL_tsGui* pGui)
{
  unsigned nFontInd;
  for (nFontInd=0;nFontInd<pGui->nFontCnt;nFontInd++) {
    if (pGui->asFont[nFontInd].pFont != NULL) {
      TTF_CloseFont(pGui->asFont[nFontInd].pFont);
      pGui->asFont[nFontInd].pFont = NULL;
    }
  }
  pGui->nFontCnt = 0;
  TTF_Quit();
}


// ------------------------------------------------------------------------
// Page Functions
// ------------------------------------------------------------------------

unsigned microSDL_GetPageCur(microSDL_tsGui* pGui)
{
  return pGui->nPageIdCur;
}


void microSDL_SetPageCur(microSDL_tsGui* pGui,unsigned nPageId)
{
  pGui->nPageIdCur = nPageId;
}


void microSDL_ElemDrawPage(microSDL_tsGui* pGui,unsigned nPageId)
{
  unsigned nInd;
  
  // Draw background
  // TODO: Consider making background another layer
  microSDL_ApplySurface(pGui,0,0,pGui->surfBkgnd,pGui->surfScreen);

  // Draw other elements
  for (nInd=0;nInd<pGui->nElemCnt;nInd++) {
    if ( (pGui->asElem[nInd].nPage == nPageId) ||
         (pGui->asElem[nInd].nPage == MSDL_PAGE_ALL) ) {
      microSDL_ElemDrawByInd(pGui,nInd);
    }
  }
}


void microSDL_ElemDrawPageCur(microSDL_tsGui* pGui)
{
  microSDL_ElemDrawPage(pGui,pGui->nPageIdCur);
}


// ------------------------------------------------------------------------
// Element General Functions
// ------------------------------------------------------------------------

int microSDL_ElemFindIndFromId(microSDL_tsGui* pGui,int nElemId)
{
  int nInd;
  int nFound = MSDL_IND_NONE;
  for (nInd=0;nInd<pGui->nElemCnt;nInd++) {
    if (pGui->asElem[nInd].nId == nElemId) {
      nFound = nInd;
    }
  }
  return nFound;
}


int microSDL_ElemFindFromCoord(microSDL_tsGui* pGui,int nX, int nY)
{
  int         nInd;
  bool        bFound = false;
  int         nFoundInd = MSDL_IND_NONE;

  #ifdef DBG_TOUCH
  // Highlight current touch for coordinate debug
  microSDL_FrameRect(pGui,nX-1,nY-1,2,2,m_colYellow);
  printf("    ElemFindFromCoord(%3u,%3u):\n",nX,nY);
  #endif

  for (nInd=0;nInd<pGui->nElemCnt;nInd++) {

    // Ensure this element is visible on this page!
    if ((pGui->asElem[nInd].nPage == pGui->nPageIdCur) ||
      (pGui->asElem[nInd].nPage == MSDL_PAGE_ALL)) {
      // Are we within the rect?
      if (microSDL_IsInRect(pGui,nX,nY,pGui->asElem[nInd].rElem)) {
        #ifdef DBG_TOUCH
        printf("      [%3u]:In  ",nInd);
        #endif

        // Only return element index if clickable
        if (pGui->asElem[nInd].bClickEn) {
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


microSDL_tsElem microSDL_ElemCreateTxt(microSDL_tsGui* pGui,int nId,unsigned nPage,SDL_Rect rElem,
  const char* pStr,unsigned nFontId)
{
  microSDL_tsElem sElem;
  sElem = microSDL_ElemCreate(pGui,nId,nPage,MSDL_TYPE_TXT,rElem,pStr,nFontId);
  sElem.colElemFill = m_colBlack;
  sElem.colElemFrame = m_colBlack;
  sElem.colElemFillSel = m_colBlack;
  sElem.colElemText = m_colYellow;
  sElem.bFillEn = true;
  sElem.eTxtAlign = MSDL_ALIGN_MID_LEFT;
  bool bOk = microSDL_ElemAdd(pGui,sElem);  //guiy
  return sElem;
}

microSDL_tsElem microSDL_ElemCreateBtnTxt(microSDL_tsGui* pGui,int nId,unsigned nPage,
  SDL_Rect rElem,const char* acStr,unsigned nFontId)
{
  microSDL_tsElem sElem;
  sElem.bValid = false;

  // Ensure the Font is loaded
  TTF_Font* pFont = microSDL_FontGet(pGui,nFontId);
  if (pFont == NULL) {
    fprintf(stderr,"ERROR: microSDL_ElemCreateBtnTxt(ID=%d): Font(ID=%u) not loaded\n",nId,nFontId);
    sElem.bValid = false;
    return sElem;
  }

  sElem = microSDL_ElemCreate(pGui,nId,nPage,MSDL_TYPE_BTN,rElem,acStr,nFontId);
  sElem.colElemFill = m_colBlueLt;
  sElem.colElemFrame = m_colBlueDk;
  sElem.colElemFillSel = m_colGreenLt;
  sElem.colElemText = m_colBlack;
  sElem.bFrameEn = true;
  sElem.bFillEn = true;
  sElem.bClickEn = true;
  bool bOk = microSDL_ElemAdd(pGui,sElem);  //guiy
  return sElem;
}

microSDL_tsElem microSDL_ElemCreateBtnImg(microSDL_tsGui* pGui,int nElemId,unsigned nPage,
  SDL_Rect rElem,const char* acImg,const char* acImgSel)
{
  microSDL_tsElem sElem;
  sElem = microSDL_ElemCreate(pGui,nElemId,nPage,MSDL_TYPE_BTN,rElem,"",MSDL_FONT_NONE);
  sElem.colElemFill = m_colBlueLt;
  sElem.colElemFrame = m_colBlueDk;
  sElem.colElemFillSel = m_colGreenLt;
  sElem.colElemText = m_colBlack;
  sElem.bFrameEn = false;
  sElem.bFillEn = false;
  sElem.bClickEn = true;
  microSDL_ElemSetImage(pGui,&sElem,acImg,acImgSel);
  bool bOk = microSDL_ElemAdd(pGui,sElem);  //guiy
  return sElem;
}

microSDL_tsElem microSDL_ElemCreateBox(microSDL_tsGui* pGui,int nElemId,unsigned nPage,SDL_Rect rElem)
{
  microSDL_tsElem sElem;
  sElem = microSDL_ElemCreate(pGui,nElemId,nPage,MSDL_TYPE_BOX,rElem,NULL,MSDL_FONT_NONE);
  sElem.colElemFill = m_colBlack;
  sElem.colElemFrame = m_colGray;
  sElem.bFillEn = true;
  sElem.bFrameEn = true;
  bool bOk = microSDL_ElemAdd(pGui,sElem);  //guiy
  return sElem;
}


microSDL_tsElem microSDL_ElemCreateImg(microSDL_tsGui* pGui,int nElemId,unsigned nPage,
  SDL_Rect rElem,const char* acImg)
{
  microSDL_tsElem sElem;
  sElem = microSDL_ElemCreate(pGui,nElemId,nPage,MSDL_TYPE_BOX,rElem,"",MSDL_FONT_NONE);
  sElem.bFrameEn = false;
  sElem.bFillEn = false;
  sElem.bClickEn = false;
  microSDL_ElemSetImage(pGui,&sElem,acImg,acImg);
  bool bOk = microSDL_ElemAdd(pGui,sElem);  //guiy
  return sElem;
}


microSDL_tsElem microSDL_ElemCreateGauge(microSDL_tsGui* pGui,int nElemId,unsigned nPage,SDL_Rect rElem,
  int nMin,int nMax,int nVal,SDL_Color colGauge,bool bVert)
{
  microSDL_tsElem sElem;
  sElem = microSDL_ElemCreate(pGui,nElemId,nPage,MSDL_TYPE_GAUGE,rElem,NULL,MSDL_FONT_NONE);
  sElem.bFrameEn = true;
  sElem.bFillEn = true;
  sElem.nGaugeMin = nMin;
  sElem.nGaugeMax = nMax;
  sElem.nGaugeVal = nVal;
  sElem.bGaugeVert = bVert;
  sElem.colGauge = colGauge;
  sElem.colElemFrame = m_colGray;
  sElem.colElemFill = m_colBlack;
  bool bOk = microSDL_ElemAdd(pGui,sElem);  //guiy
  return sElem;
}


// TODO: Move to private section
bool microSDL_ElemAdd(microSDL_tsGui* pGui,microSDL_tsElem sElem)
{
  if (pGui->nElemCnt+1 >= ELEM_MAX) {
    fprintf(stderr,"ERROR: microSDL_ElemAdd() too many elements\n");
    return false;
  }
  // In case the element creation failed, trap that here
  if (!sElem.bValid) {
    fprintf(stderr,"ERROR: microSDL_ElemAdd() skipping add of invalid element\n");
    return false;
  }
  // Add the element to the internal array
  pGui->asElem[pGui->nElemCnt] = sElem;
  pGui->nElemCnt++;
  return true;
}


// ------------------------------------------------------------------------
// Element Drawing Functions
// ------------------------------------------------------------------------

void microSDL_ElemDraw(microSDL_tsGui* pGui,int nElemId)
{
  int nElemInd = microSDL_ElemFindIndFromId(pGui,nElemId);
  if (!microSDL_ElemIndValid(pGui,nElemInd)) { return; }
  microSDL_ElemDrawByInd(pGui,nElemInd);
}


// ------------------------------------------------------------------------
// Element Update Functions
// ------------------------------------------------------------------------

void microSDL_ElemSetFillEn(microSDL_tsGui* pGui,int nElemId,bool bFillEn)
{
  int nElemInd = microSDL_ElemFindIndFromId(pGui,nElemId);
  if (!microSDL_ElemIndValid(pGui,nElemInd)) { return; }
  microSDL_tsElem* pElem = &pGui->asElem[nElemInd];
  pElem->bFillEn = bFillEn;
}

void microSDL_ElemSetCol(microSDL_tsGui* pGui,int nElemId,SDL_Color colFrame,SDL_Color colFill,SDL_Color colFillSel)
{
  int nElemInd = microSDL_ElemFindIndFromId(pGui,nElemId);
  if (!microSDL_ElemIndValid(pGui,nElemInd)) { return; }
  microSDL_tsElem* pElem = &pGui->asElem[nElemInd];
  pElem->colElemFrame     = colFrame;
  pElem->colElemFill      = colFill;
  pElem->colElemFillSel   = colFillSel;
}



void microSDL_ElemSetTxtAlign(microSDL_tsGui* pGui,int nElemId,unsigned nAlign)
{
  int nElemInd = microSDL_ElemFindIndFromId(pGui,nElemId);
  if (!microSDL_ElemIndValid(pGui,nElemInd)) { return; }
  microSDL_tsElem* pElem = &pGui->asElem[nElemInd];
  pElem->eTxtAlign = nAlign;
}


void microSDL_ElemSetTxtStr(microSDL_tsGui* pGui,int nElemId,const char* pStr)
{
  int nElemInd = microSDL_ElemFindIndFromId(pGui,nElemId);
  if (!microSDL_ElemIndValid(pGui,nElemInd)) { return; }
  microSDL_tsElem* pElem = &pGui->asElem[nElemInd];
  strncpy(pElem->acStr,pStr,ELEM_STRLEN_MAX);
}

void microSDL_ElemSetTxtCol(microSDL_tsGui* pGui,int nElemId,SDL_Color colVal)
{
  int nElemInd = microSDL_ElemFindIndFromId(pGui,nElemId);
  if (!microSDL_ElemIndValid(pGui,nElemInd)) { return; }
  microSDL_tsElem* pElem = &pGui->asElem[nElemInd];
  pElem->colElemText = colVal;
}

void microSDL_ElemUpdateFont(microSDL_tsGui* pGui,int nElemId,unsigned nFont)
{
  int nElemInd = microSDL_ElemFindIndFromId(pGui,nElemId);
  if (!microSDL_ElemIndValid(pGui,nElemInd)) { return; }
  microSDL_tsElem* pElem = &pGui->asElem[nElemInd];
  pElem->pTxtFont = microSDL_FontGet(pGui,nFont);
}


void microSDL_ElemUpdateGauge(microSDL_tsGui* pGui,int nElemId,int nVal)
{
  int nElemInd = microSDL_ElemFindIndFromId(pGui,nElemId);
  if (!microSDL_ElemIndValid(pGui,nElemInd)) { return; }
  microSDL_tsElem* pElem = &pGui->asElem[nElemInd];
  pElem->nGaugeVal = nVal;
}


// ------------------------------------------------------------------------
// Tracking Functions
// ------------------------------------------------------------------------

int microSDL_GetTrackElemClicked(microSDL_tsGui* pGui)
{
  return pGui->nTrackElemClicked;
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

bool microSDL_GetSdlClick(microSDL_tsGui* pGui,int &nX,int &nY,unsigned &nPress)
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
      }
    } else if (sEvent.type == SDL_KEYUP) {

    } else if (sEvent.type == SDL_MOUSEMOTION) {
    } else if (sEvent.type == SDL_MOUSEBUTTONDOWN) {
      SDL_GetMouseState(&nX,&nY);
      nPress = 1;
      bRet = true;
    } else if (sEvent.type == SDL_MOUSEBUTTONUP) {
      SDL_GetMouseState(&nX,&nY);
      nPress = 0;
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
  //CAL!
  // TODO: use env "TSLIB_TSDEVICE" instead
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

int microSDL_GetTsClick(microSDL_tsGui* pGui,int &nX,int &nY,unsigned &nPress)
{
  ts_sample   pSamp;
  int nRet = ts_read(pGui->ts,&pSamp,1);
  nX = pSamp.x;
  nY = pSamp.y;
  nPress = pSamp.pressure;
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
void microSDL_PutPixelRaw(microSDL_tsGui* pGui,SDL_Surface *surf, int nX, int nY, Uint32 nPixelVal)
{
  int nBpp = surf->format->BytesPerPixel;
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
//       MSDL_ID_ANON if the user wants an auto-generated ID
microSDL_tsElem microSDL_ElemCreate(microSDL_tsGui* pGui,int nId,unsigned nPageId,unsigned nType,
  SDL_Rect rElem,const char* pStr,unsigned nFontId)
{
  microSDL_tsElem sElem;
  sElem.bValid = false;

  // Validate the user-supplied ID
  if (nId == MSDL_ID_ANON) {
    // Anonymous ID requested
    // Assign an automatic ID for this element
    nId = pGui->nElemAutoIdNext;
    pGui->nElemAutoIdNext++;
  } else {
    // Ensure the ID is positive
    if (nId < 0) {
      printf("ERROR: microSDL_ElemCreate() called with negative ID (%d)\n",nId);
      return sElem;
    }
    // Ensure the ID isn't already taken
    if (microSDL_ElemFindIndFromId(pGui,nId) != MSDL_IND_NONE) {
      printf("ERROR: microSDL_Create() called with existing ID (%d)\n",nId);
      return sElem;
    }
  }

  sElem.nId = nId;
  sElem.nPage = nPageId;
  sElem.rElem = rElem;
  sElem.nType = nType;
  sElem.pTxtFont = microSDL_FontGet(pGui,nFontId);
  sElem.pSurf = NULL;
  sElem.pSurfSel = NULL;
  sElem.nGaugeMin = 0;
  sElem.nGaugeMax = 0;
  sElem.nGaugeVal = 0;
  sElem.bGaugeVert = false;
  sElem.colGauge = m_colBlack;
  sElem.colElemFill = m_colBlack;
  sElem.colElemFrame = m_colBlueDk;
  sElem.colElemFillSel = m_colBlack;
  sElem.colElemText = m_colYellow;
  if (pStr != NULL) {
    strncpy(sElem.acStr,pStr,ELEM_STRLEN_MAX);
  } else {
    sElem.acStr[0] = '\0';
  }

  // Assign defaults
  sElem.eTxtAlign = MSDL_ALIGN_MID_MID;
  sElem.nTxtMargin = 5;
  sElem.bFillEn = false;
  sElem.bFrameEn = false;
  sElem.bClickEn = false;

  // If the element creation was successful, then set the valid flag
  sElem.bValid = true;

  return sElem;
}

bool microSDL_ElemIndValid(microSDL_tsGui* pGui,int nElemInd)
{
  if ((nElemInd >= 0) && (nElemInd < pGui->nElemCnt)) {
    return true;
  } else {
    return false;
  }
  
}

int microSDL_ElemGetIdFromInd(microSDL_tsGui* pGui,int nElemInd)
{
  if (microSDL_ElemIndValid(pGui,nElemInd)) {
  //xxx if ((nElemInd >= 0) && (nElemInd < pGui->nElemCnt)) {
    return pGui->asElem[nElemInd].nId;
  } else {
    return MSDL_ID_NONE;
  }
}

void microSDL_ElemSetImage(microSDL_tsGui* pGui,microSDL_tsElem* sElem,const char* acImage,
  const char* acImageSel)
{
  SDL_Surface*    pSurf;
  SDL_Surface*    pSurfSel;
  if (sElem == NULL) {
    // ERROR
    return;
  }

  if (strlen(acImage) > 0) {
    if (sElem->pSurf != NULL) {
      fprintf(stderr,"ERROR: microSDL_ElemSetImage(%s) with pSurf already set\n",acImage);
      return;
    }
    pSurf = microSDL_LoadBmp(pGui,(char*)acImage);
    if (pSurf == NULL) {
      fprintf(stderr,"ERROR: microSDL_ElemSetImage(%s) call to microSDL_LoadBmp failed\n",acImage);
      return;
    }
    sElem->pSurf = pSurf;
  }

  if (strlen(acImageSel) > 0) {
    if (sElem->pSurfSel != NULL) {
      fprintf(stderr,"ERROR: microSDL_ElemSetImage(%s) with pSurfSel already set\n",acImageSel);
      return;
    }
    pSurfSel = microSDL_LoadBmp(pGui,(char*)acImageSel);
    if (pSurfSel == NULL) {
      fprintf(stderr,"ERROR: microSDL_ElemSetImage(%s) call to microSDL_LoadBmp failed\n",acImageSel);
      return;
    }
    sElem->pSurfSel = pSurfSel;
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
  unsigned          nElemX,nElemY,nElemW,nElemH;
  SDL_Surface*      surfTxt = NULL;

  sElem = pGui->asElem[nElemInd];
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

  // Handle special element types
  // TODO: Add owner-draw type
  if (sElem.nType == MSDL_TYPE_GAUGE) {
    microSDL_ElemDraw_Gauge(pGui,sElem);
  }

  // Frame the region
  #ifdef DBG_FRAME
  // For debug purposes, draw a frame around every element
  microSDL_FrameRect(pGui,nElemX,nElemY,nElemW,nElemH, m_colGrayDk);
  #else
  if (sElem.bFrameEn) {
    microSDL_FrameRect(pGui,nElemX,nElemY,nElemW,nElemH, sElem.colElemFrame);
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
    unsigned    nTxtX = nElemX;
    unsigned    nTxtY = nElemY;
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


  // Update the active display
  //
  // - NOTE: Currently, we are calling the display update on every
  //         element draw. We could probably defer this until an explicit
  //         call after all page elements are done. However, doing it
  //         here makes it simpler to call minor element updates.
  // - NOTE: We could also call Update instead of Flip as that would
  //         limit the region refresh.
  microSDL_Flip(pGui);

  return true;
}


bool microSDL_ElemDraw_Gauge(microSDL_tsGui* pGui,microSDL_tsElem sElem)
{
  SDL_Rect    rGauge;
  unsigned    nElemX,nElemY,nElemW,nElemH;

  nElemX = sElem.rElem.x;
  nElemY = sElem.rElem.y;
  nElemW = sElem.rElem.w;
  nElemH = sElem.rElem.h;

  rGauge.x = nElemX;
  rGauge.y = nElemY;

  bool  bVert = sElem.bGaugeVert;
  int   nMax = sElem.nGaugeMax;
  int   nMin = sElem.nGaugeMin;
  int   nVal = sElem.nGaugeVal;
  int   nRng = sElem.nGaugeMax-sElem.nGaugeMin;

  if (nRng == 0) {
    fprintf(stderr,"ERROR: Zero gauge range [%d,%d]\n",nMin,nMax);
    return false;
  }
  float   fScl;
  if (bVert) {
    fScl = (float)nElemH/(float)nRng;
  } else {
    fScl = (float)nElemW/(float)nRng;
  }

  int nGaugeMid;
  if ((nMin == 0) && (nMax > 0)) {
    nGaugeMid = 0;
  } else if ((nMin < 0) && (nMax > 0)) {
    nGaugeMid = -nMin*fScl;
  } else if ((nMin < 0) && (nMax == 0)) {
    nGaugeMid = -nMin*fScl;
  } else {
    fprintf(stderr,"ERROR: Unsupported gauge range [%d,%d]\n",nMin,nMax);
    return false;
  }

  // Calculate the length of the bar
  int nLen = sElem.nGaugeVal * fScl;

  // Handle negative ranges
  if (nLen >= 0) {
    if (bVert) {
      rGauge.h = nLen;
      rGauge.y = nElemY + nGaugeMid;
    } else {
      rGauge.w = nLen;
      rGauge.x = nElemX + nGaugeMid;
    }
  } else {
    if (bVert) {
      rGauge.y = nElemY + nGaugeMid+nLen;
      rGauge.h = -nLen;
    } else {
      rGauge.x = nElemX + nGaugeMid+nLen;
      rGauge.w = -nLen;
    }
  }

  if (bVert) {
    rGauge.w = nElemW;
    rGauge.x = nElemX;
  } else {
    rGauge.h = nElemH;
    rGauge.y = nElemY;
  }

  // Now clip the region
  int nRectClipX1 = rGauge.x;
  int nRectClipX2 = rGauge.x+rGauge.w;
  int nRectClipY1 = rGauge.y;
  int nRectClipY2 = rGauge.y+rGauge.h;
  if (nRectClipX1 < nElemX)         { nRectClipX1 = nElemX;         }
  if (nRectClipX2 > nElemX+nElemW)  { nRectClipX2 = nElemX+nElemW;  }
  if (nRectClipY1 < nElemY)         { nRectClipY1 = nElemY;         }
  if (nRectClipY2 > nElemY+nElemH)  { nRectClipY2 = nElemY+nElemH;  }
  rGauge.x = nRectClipX1;
  rGauge.y = nRectClipY1;
  rGauge.w = nRectClipX2-nRectClipX1;
  rGauge.h = nRectClipY2-nRectClipY1;

  #ifdef DBG_LOG
  printf("Gauge: nMin=%4d nMax=%4d nRng=%d nVal=%4d fScl=%6.3f nGaugeMid=%4d RectX=%4d RectW=%4d\n",
    nMin,nMax,nRng,sElem.nGaugeVal,fScl,nGaugeMid,rGauge.x,rGauge.w);
  #endif

  microSDL_FillRect(pGui,rGauge,sElem.colGauge);

  // Now draw the midpoint line
  SDL_Rect    rMidLine;
  if (bVert) {
    rMidLine.x = nElemX;
    rMidLine.y = nElemY+nGaugeMid;
    rMidLine.w = nElemW;
    rMidLine.h = 1;
  } else {
    rMidLine.x = nElemX+nGaugeMid;
    rMidLine.y = nElemY;
    rMidLine.w = 1;
    rMidLine.h = nElemH;
  }
  microSDL_FillRect(pGui,rMidLine,sElem.colElemFrame);

  return true;
}


void microSDL_ElemCloseAll(microSDL_tsGui* pGui)
{
  unsigned nElemInd;
  for (nElemInd=0;nElemInd<pGui->nElemCnt;nElemInd++) {
    if (pGui->asElem[nElemInd].pSurf != NULL) {
      SDL_FreeSurface(pGui->asElem[nElemInd].pSurf);
      pGui->asElem[nElemInd].pSurf = NULL;
    }
    if (pGui->asElem[nElemInd].pSurfSel != NULL) {
      SDL_FreeSurface(pGui->asElem[nElemInd].pSurfSel);
      pGui->asElem[nElemInd].pSurfSel = NULL;
    }
  }

  // TODO: Consider moving into main element array
  if (pGui->surfBkgnd != NULL) {
    SDL_FreeSurface(pGui->surfBkgnd);
    pGui->surfBkgnd = NULL;
  }

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
    // Redraw button
    microSDL_ElemDrawByInd(pGui,nHoverStart);
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
  microSDL_ElemDrawByInd(pGui,pGui->nTrackElemHover);
  microSDL_ElemDrawByInd(pGui,nHoverNew);

  pGui->nTrackElemHover = MSDL_IND_NONE;

}

// Handle MOUSEMOVE while MOUSEDOWN
void microSDL_TrackTouchDownMove(microSDL_tsGui* pGui,int nX,int nY)
{
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





