// =======================================================================
// microSDL library
// - Calvin Hass
// - http:/www.impulseadventure.com/elec/microsdl-sdl-gui.html
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


// ------------------------------------------------------------------------

// The page that is currently active
unsigned          microSDL_m_nPageIdCur = MSDL_PAGE_NONE;

// Collection of loaded fonts
microSDL_tsFont   microSDL_m_asFont[FONT_MAX];
unsigned          microSDL_m_nFontCnt = 0;

// Collection of graphic elements (across all pages)
microSDL_tsElem   microSDL_m_asElem[ELEM_MAX];
unsigned          microSDL_m_nElemCnt = 0;

// Current touch-tracking hover status
int               microSDL_m_nTrackElemHover = MSDL_ID_NONE;
bool              microSDL_m_bTrackElemHoverGlow = false;

// Last graphic element clicked
int               microSDL_m_nTrackElemClicked = MSDL_ID_NONE;
int               microSDL_m_nClickLastX = 0;
int               microSDL_m_nClickLastY = 0;
unsigned          microSDL_m_nClickLastPress = 0;

// Touchscreen library interface
#ifdef INC_TS
struct tsdev*     microSDL_m_ts;
#endif

// Primary surface definitions
SDL_Surface*      microSDL_m_surfScreen = NULL;
SDL_Surface*      microSDL_m_surfBkgnd = NULL;


// ========================================================================



// ------------------------------------------------------------------------
// General Functions
// ------------------------------------------------------------------------

// Need to configure a number of SDL and TS environment
// variables. The following demonstrates a way to do this
// programmatically, but it can also be done via export
// commands within the shell (or init script).
// eg. export TSLIB_FBDEVICE=/dev/fb1
void microSDL_InitEnv()
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


bool microSDL_Init()
{
  // Setup SDL
  SDL_Init(SDL_INIT_VIDEO);

  // Set up microSDL_m_surfScreen
  const SDL_VideoInfo*  videoInfo = SDL_GetVideoInfo();
  int                   nSystemX = videoInfo->current_w;
  int                   nSystemY = videoInfo->current_h;
  Uint8                 nBpp = videoInfo->vfmt->BitsPerPixel ;

  // SDL_SWSURFACE is apparently more reliable
  microSDL_m_surfScreen = SDL_SetVideoMode(nSystemX,nSystemY,nBpp,SDL_SWSURFACE);
  if (!microSDL_m_surfScreen) {
    fprintf(stderr,"ERROR: SDL_SetVideoMode() failed: %s\n",SDL_GetError());
    return false;
  }

  // Since the mouse cursor is based on SDL coords which are badly
  // scaled when in touch mode, we will disable the mouse pointer.
  // Note that the SDL coords seem to be OK when in actual mouse mode.
  SDL_ShowCursor(SDL_DISABLE);

  // Initialize fonts
  if (!microSDL_InitFont()) {
    return false;
  }

  return true;
}

void microSDL_Quit()
{
  microSDL_ElemCloseAll();
  SDL_Quit();
}


// ------------------------------------------------------------------------
// Graphics General Functions
// ------------------------------------------------------------------------

SDL_Surface* microSDL_LoadBmp(char* pStrFname)
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


bool microSDL_SetBkgnd(SDL_Surface* pSurf)
{
  if (!pSurf) {
    // TODO: Error handling
    return false;
  }
  if (microSDL_m_surfBkgnd != NULL ) {
    // Dispose of previous background
    SDL_FreeSurface(microSDL_m_surfBkgnd);
    microSDL_m_surfBkgnd = NULL;
  }
  microSDL_m_surfBkgnd = pSurf;
  return true;
}

bool microSDL_SetBkgndImage(char* pStrFname)
{
  if (strlen(pStrFname)==0) {
    // TODO: Error handling
    return false;
  }
  if (microSDL_m_surfBkgnd != NULL ) {
    // Dispose of previous background
    SDL_FreeSurface(microSDL_m_surfBkgnd);
    microSDL_m_surfBkgnd = NULL;
  }
  microSDL_m_surfBkgnd = microSDL_LoadBmp(pStrFname);
  if (microSDL_m_surfBkgnd == NULL) {
    return false;
  }
  return true;
}

bool microSDL_SetBkgndColor(SDL_Color nCol)
{
  SDL_Surface*  pSurf;
  if (microSDL_m_surfBkgnd != NULL ) {
    // Dispose of previous background
    SDL_FreeSurface(microSDL_m_surfBkgnd);
    microSDL_m_surfBkgnd = NULL;
  }
  unsigned nScreenW = microSDL_m_surfScreen->w;
  unsigned nScreenH = microSDL_m_surfScreen->h;
  unsigned nBpp = microSDL_m_surfScreen->format->BytesPerPixel * 8;
  microSDL_m_surfBkgnd = SDL_CreateRGBSurface(SDL_SWSURFACE,
    nScreenW,nScreenH,nBpp,0,0,0,0xFF);
  SDL_FillRect(microSDL_m_surfBkgnd,NULL,
    SDL_MapRGB(microSDL_m_surfBkgnd->format,nCol.r,nCol.g,nCol.b));

  if (microSDL_m_surfBkgnd == NULL) {
    return false;
  }
  return true;
}


void microSDL_ApplySurface(int x, int y, SDL_Surface* pSrc, SDL_Surface* pDest)
{
  SDL_Rect offset;
  offset.x = x;
  offset.y = y;
  SDL_BlitSurface(pSrc,NULL,pDest,&offset);
}


bool microSDL_IsInRect(unsigned nSelX,unsigned nSelY,SDL_Rect rBtn)
{
  if ( (nSelX >= rBtn.x) && (nSelX <= rBtn.x+rBtn.w) && 
     (nSelY >= rBtn.y) && (nSelY <= rBtn.y+rBtn.h) ) {
    return true;
  } else {
    return false;
  }
}


void microSDL_Flip()
{
  SDL_Flip( microSDL_m_surfScreen );
}



// ------------------------------------------------------------------------
// Graphics Primitive Functions
// ------------------------------------------------------------------------

void microSDL_SetPixel(Sint16 nX,Sint16 nY,SDL_Color nCol)
{
  if (microSDL_Lock()) {
    microSDL_PutPixelRaw(microSDL_m_surfScreen,nX,nY,microSDL_GenPixelColor(nCol));
    microSDL_Unlock();
  }   // microSDL_Lock
}

// Draw an arbitrary line using Bresenham's algorithm
// - Algorithm reference: https://rosettacode.org/wiki/Bitmap/Bresenham's_line_algorithm#C
void microSDL_Line(Sint16 nX0,Sint16 nY0,Sint16 nX1,Sint16 nY1,SDL_Color nCol)
{
  Sint16 nDX = abs(nX1-nX0);
  Sint16 nSX = (nX0 < nX1)? 1 : -1;
  Sint16 nDY = abs(nY1-nY0);
  Sint16 nSY = (nY0 < nY1)? 1 : -1;
  Sint16 nErr = ( (nDX>nDY)? nDX : -nDY )/2;
  Sint16 nE2;
  for (;;) {
    microSDL_SetPixel(nX0,nY0,nCol);
    if ( (nX0 == nX1) && (nY0 == nY1) ) break;
    nE2 = nErr;
    if (nE2 > -nDX) { nErr -= nDY; nX0 += nSX; }
    if (nE2 <  nDY) { nErr += nDX; nY0 += nSY; }
  }
}

void microSDL_LineH(Sint16 nX, Sint16 nY, Uint16 nW,SDL_Color nCol)
{
  Uint16 nOffset;
  Uint32 nPixelCol = microSDL_GenPixelColor(nCol);
  if (microSDL_Lock()) {
    for (nOffset=0;nOffset<nW;nOffset++) {
      microSDL_PutPixelRaw(microSDL_m_surfScreen,nX+nOffset,nY,nPixelCol);    
    }
    microSDL_Unlock();
  }   // microSDL_Lock
}

void microSDL_LineV(Sint16 nX, Sint16 nY, Uint16 nH,SDL_Color nCol)
{
  Uint16 nOffset;
  Uint32 nPixelCol = microSDL_GenPixelColor(nCol);
  if (microSDL_Lock()) {
    for (nOffset=0;nOffset<nH;nOffset++) {
      microSDL_PutPixelRaw(microSDL_m_surfScreen,nX,nY+nOffset,nPixelCol);    
    }
    microSDL_Unlock();
  }   // microSDL_Lock
}



void microSDL_FrameRect(Sint16 nX,Sint16 nY,Uint16 nW,Uint16 nH,SDL_Color nCol)
{
  microSDL_LineH(nX,nY,nW,nCol);                  // Top
  microSDL_LineH(nX,(Sint16)(nY+nH),nW,nCol);     // Bottom
  microSDL_LineV(nX,nY,nH,nCol);                  // Left
  microSDL_LineV((Sint16)(nX+nW),nY,nH,nCol);     // Right
}





// -----------------------------------------------------------------------
// Font Functions
// -----------------------------------------------------------------------

bool microSDL_InitFont()
{
  if (TTF_Init() == -1) {
    fprintf(stderr,"ERROR: TTF_Init() failed\n");
    return false;
  }
  return true;
}

  

bool microSDL_FontAdd(unsigned nFontId,const char* acFontName,unsigned nFontSz)
{
  if (microSDL_m_nFontCnt+1 >= FONT_MAX) {
    fprintf(stderr,"ERROR: microSDL_FontAdd() added too many fonts\n");
    return false;
  } else {
    TTF_Font*   pFont;
    pFont = TTF_OpenFont(acFontName,nFontSz);
    if (pFont == NULL) {
      fprintf(stderr,"ERROR: TTF_OpenFont(%s) failed\n",acFontName);
      return false;
    }
    microSDL_m_asFont[microSDL_m_nFontCnt].pFont = pFont;
    microSDL_m_asFont[microSDL_m_nFontCnt].nId = nFontId;
    microSDL_m_nFontCnt++;  
    return true;
  }
}


TTF_Font* microSDL_FontGet(unsigned nFontId)
{
  unsigned nFontInd;
  for (nFontInd=0;nFontInd<microSDL_m_nFontCnt;nFontInd++) {
    if (microSDL_m_asFont[nFontInd].nId == nFontId) {
      return microSDL_m_asFont[nFontInd].pFont;
    }
  }
  return NULL;
}


// TODO: Move  into microSDL_Quit()?
void microSDL_FontCloseAll()
{
  unsigned nFontInd;
  for (nFontInd=0;nFontInd<microSDL_m_nFontCnt;nFontInd++) {
    if (microSDL_m_asFont[nFontInd].pFont != NULL) {
      TTF_CloseFont(microSDL_m_asFont[nFontInd].pFont);
      microSDL_m_asFont[nFontInd].pFont = NULL;
    }
  }
  microSDL_m_nFontCnt = 0;
  TTF_Quit();
}


// ------------------------------------------------------------------------
// Page Functions
// ------------------------------------------------------------------------

unsigned microSDL_GetPageCur()
{
  return microSDL_m_nPageIdCur;
}


void microSDL_SetPageCur(unsigned nPageId)
{
  microSDL_m_nPageIdCur = nPageId;
}


void microSDL_ElemDrawPage(unsigned nPageId)
{
  unsigned nInd;
  
  // Draw background
  // TODO: Consider making background another layer
  microSDL_ApplySurface(0,0,microSDL_m_surfBkgnd,microSDL_m_surfScreen);

  // Draw other elements
  for (nInd=0;nInd<microSDL_m_nElemCnt;nInd++) {
    if ((microSDL_m_asElem[nInd].nPage == nPageId) || (microSDL_m_asElem[nInd].nPage == MSDL_PAGE_ALL)) {
      microSDL_ElemDrawByInd(nInd);
    }
  }
}


void microSDL_ElemDrawPageCur()
{
  microSDL_ElemDrawPage(microSDL_m_nPageIdCur);
}


// ------------------------------------------------------------------------
// Element General Functions
// ------------------------------------------------------------------------

int microSDL_ElemFindIndFromId(unsigned nId)
{
  unsigned nInd;
  int nFound = MSDL_ID_NONE;
  for (nInd=0;nInd<microSDL_m_nElemCnt;nInd++) {
    if (microSDL_m_asElem[nInd].nId == nId) {
      nFound = nInd;
    }
  }
  return nFound;
}


int microSDL_ElemFindFromCoord(int nX, int nY)
{
  unsigned    nInd;
  bool        bFound = false;
  int         nFoundInd = MSDL_ID_NONE;

  #ifdef DBG_TOUCH
  // Highlight current touch for coordinate debug
  microSDL_FrameRect(nX-1,nY-1,2,2,m_colYellow);
  printf("    ElemFindFromCoord(%3u,%3u):\n",nX,nY);
  #endif

  for (nInd=0;nInd<microSDL_m_nElemCnt;nInd++) {

    // Ensure this element is visible on this page!
    if ((microSDL_m_asElem[nInd].nPage == microSDL_m_nPageIdCur) ||
      (microSDL_m_asElem[nInd].nPage == MSDL_PAGE_ALL)) {
      // Are we within the rect?
      if (microSDL_IsInRect(nX,nY,microSDL_m_asElem[nInd].rElem)) {
        #ifdef DBG_TOUCH
        printf("      [%3u]:In  ",nInd);
        #endif

        // Only return element index if clickable
        if (microSDL_m_asElem[nInd].bClickEn) {
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

  // Return index or MSDL_ID_NONE if none found
  return nFoundInd;
}


// ------------------------------------------------------------------------
// Element Creation Functions
// ------------------------------------------------------------------------


microSDL_tsElem microSDL_ElemCreateTxt(int nId,unsigned nPage,SDL_Rect rElem,
  const char* pStr,unsigned nFontId)
{
  microSDL_tsElem sElem;
  sElem = microSDL_ElemCreate(nId,nPage,MSDL_TYPE_TXT,rElem,pStr,nFontId);
  sElem.colElemFill = m_colBlack;
  sElem.colElemFrame = m_colBlack;
  sElem.colElemFillSel = m_colBlack;
  sElem.colElemText = m_colYellow;
  sElem.bFillEn = true;
  sElem.eTxtAlignH = MSDL_ALIGN_H_LEFT;
  return sElem;
}

microSDL_tsElem microSDL_ElemCreateBtnTxt(int nId,unsigned nPage,
  SDL_Rect rElem,const char* acStr,unsigned nFontId)
{
  microSDL_tsElem sElem;
  sElem.bValid = false;

  // Ensure the Font is loaded
  TTF_Font* pFont = microSDL_FontGet(nFontId);
  if (pFont == NULL) {
    fprintf(stderr,"ERROR: microSDL_ElemCreateBtnTxt(ID=%d): Font(ID=%u) not loaded\n",nId,nFontId);
    sElem.bValid = false;
    return sElem;
  }

  sElem = microSDL_ElemCreate(nId,nPage,MSDL_TYPE_BTN,rElem,acStr,nFontId);
  sElem.colElemFill = m_colBlueLt;
  sElem.colElemFrame = m_colBlueDk;
  sElem.colElemFillSel = m_colGreenLt;
  sElem.colElemText = m_colBlack;
  sElem.bFrameEn = true;
  sElem.bFillEn = true;
  sElem.bClickEn = true;
  return sElem;
}

microSDL_tsElem microSDL_ElemCreateBtnImg(int nId,unsigned nPage,
  SDL_Rect rElem,const char* acImg,const char* acImgSel)
{
  microSDL_tsElem sElem;
  sElem = microSDL_ElemCreate(nId,nPage,MSDL_TYPE_BTN,rElem,"",MSDL_FONT_NONE);
  sElem.colElemFill = m_colBlueLt;
  sElem.colElemFrame = m_colBlueDk;
  sElem.colElemFillSel = m_colGreenLt;
  sElem.colElemText = m_colBlack;
  sElem.bFrameEn = false;
  sElem.bFillEn = false;
  sElem.bClickEn = true;
  microSDL_ElemSetImage(&sElem,acImg,acImgSel);
  return sElem;
}

microSDL_tsElem microSDL_ElemCreateBox(int nId,unsigned nPage,SDL_Rect rElem)
{
  microSDL_tsElem sElem;
  sElem = microSDL_ElemCreate(nId,nPage,MSDL_TYPE_BOX,rElem,NULL,MSDL_FONT_NONE);
  sElem.colElemFill = m_colBlack;
  sElem.colElemFrame = m_colGray;
  sElem.bFillEn = true;
  sElem.bFrameEn = true;
  return sElem;
}


microSDL_tsElem microSDL_ElemCreateGauge(int nId,unsigned nPage,SDL_Rect rElem,
  int nMin,int nMax,int nVal,SDL_Color colGauge,bool bVert)
{
  microSDL_tsElem sElem;
  sElem = microSDL_ElemCreate(nId,nPage,MSDL_TYPE_GAUGE,rElem,NULL,MSDL_FONT_NONE);
  sElem.bFrameEn = true;
  sElem.bFillEn = true;
  sElem.nGaugeMin = nMin;
  sElem.nGaugeMax = nMax;
  sElem.nGaugeVal = nVal;
  sElem.bGaugeVert = bVert;
  sElem.colGauge = colGauge;
  sElem.colElemFrame = m_colGray;
  sElem.colElemFill = m_colBlack;
  return sElem;
}


bool microSDL_ElemAdd(microSDL_tsElem sElem)
{
  if (microSDL_m_nElemCnt+1 >= ELEM_MAX) {
    fprintf(stderr,"ERROR: microSDL_ElemAdd() too many elements\n");
    return false;
  }
  // In case the element creation failed, trap that here
  if (!sElem.bValid) {
    fprintf(stderr,"ERROR: microSDL_ElemAdd() skipping add of invalid element\n");
    return false;
  }
  // Add the element to the internal array
  microSDL_m_asElem[microSDL_m_nElemCnt] = sElem;
  microSDL_m_nElemCnt++;
  return true;
}


// ------------------------------------------------------------------------
// Element Drawing Functions
// ------------------------------------------------------------------------

void microSDL_ElemDraw(int nElemId)
{
  int nElemInd = microSDL_ElemFindIndFromId(nElemId);
  if (nElemInd == MSDL_ID_NONE) {
    return;
  }
  microSDL_ElemDrawByInd(nElemInd);
}


// ------------------------------------------------------------------------
// Element Update Functions
// ------------------------------------------------------------------------

//TODO: was
// fill,frame,text,fillsell
// now:
// frame,fill,fillsell

void microSDL_ElemSetStyleMain(microSDL_tsElem* sElem,SDL_Color colFrame,
  SDL_Color colFill,SDL_Color colFillSel)
{
  if (sElem == NULL) {
    // ERROR
    return;
  }
  sElem->colElemFrame = colFrame;
  sElem->colElemFill = colFill;
  sElem->colElemFillSel = colFillSel;
}

/*
void microSDL_ElemSetStyle(microSDL_tsElem* sElem,SDL_Color colFill,
  SDL_Color colFrame,SDL_Color colText,SDL_Color colFillSel)
{
  if (sElem == NULL) {
    // ERROR
    return;
  }
  sElem->colElemFill = colFill;
  sElem->colElemFrame = colFrame;
  sElem->colElemText = colText;
  sElem->colElemFillSel = colFillSel;
}
*/


void microSDL_ElemSetImage(microSDL_tsElem* sElem,const char* acImage,
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
    pSurf = microSDL_LoadBmp((char*)acImage);
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
    pSurfSel = microSDL_LoadBmp((char*)acImageSel);
    if (pSurfSel == NULL) {
      fprintf(stderr,"ERROR: microSDL_ElemSetImage(%s) call to microSDL_LoadBmp failed\n",acImageSel);
      return;
    }
    sElem->pSurfSel = pSurfSel;
  }


}

void microSDL_ElemUpdateTxt(unsigned nId,const char* pStr)
{
  int nInd = microSDL_ElemFindIndFromId(nId);
  if (nInd == MSDL_ID_NONE) {
    return;
  }   
  strncpy(microSDL_m_asElem[nInd].acStr,pStr,ELEM_STRLEN_MAX);
}

void microSDL_ElemUpdateTxtCol(unsigned nId,SDL_Color colVal)
{
  int nInd = microSDL_ElemFindIndFromId(nId);
  if (nInd == MSDL_ID_NONE) {
    return;
  }   
  microSDL_m_asElem[nInd].colElemText = colVal;
}
void microSDL_ElemUpdateFont(unsigned nId,unsigned nFont)
{
  int nInd = microSDL_ElemFindIndFromId(nId);
  if (nInd == MSDL_ID_NONE) {
    return;
  }   
  microSDL_m_asElem[nInd].pTxtFont = microSDL_FontGet(nFont);
}


void microSDL_ElemUpdateGauge(unsigned nId,int nVal)
{
  int nInd = microSDL_ElemFindIndFromId(nId);
  if (nInd == MSDL_ID_NONE) {
    return;
  }   
  microSDL_m_asElem[nInd].nGaugeVal = nVal;
}


// ------------------------------------------------------------------------
// Tracking Functions
// ------------------------------------------------------------------------

int microSDL_GetTrackElemClicked()
{
  return microSDL_m_nTrackElemClicked;
}

void microSDL_ClearTrackElemClicked()
{
  microSDL_m_nTrackElemClicked = MSDL_ID_NONE;
}

void microSDL_TrackClick(int nX,int nY,unsigned nPress)
{
  #ifdef DBG_TOUCH
  printf(" TS : (%3d,%3d) Pressure=%3u\n",nX,nY,nPress);
  #endif

  if ((microSDL_m_nClickLastPress == 0) && (nPress > 0)) {
    // TouchDown
    microSDL_TrackTouchDownClick(nX,nY);
  } else if ((microSDL_m_nClickLastPress > 0) && (nPress == 0)) {
    // TouchUp
    microSDL_TrackTouchUpClick(nX,nY);
  } else if ((microSDL_m_nClickLastX != nX) || (microSDL_m_nClickLastY != nY)) {
    // TouchMove
    // Only track movement if touch is down
    if (nPress>0) {
      // TouchDownMove
      microSDL_TrackTouchDownMove(nX,nY);
    }
  }

  // Save sample
  microSDL_m_nClickLastX = nX;
  microSDL_m_nClickLastY = nY;
  microSDL_m_nClickLastPress = nPress;

}

bool microSDL_GetSdlClick(int &nX,int &nY,unsigned &nPress)
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
// - microSDL_m_ts mapped to touchscreen device
bool microSDL_InitTs(const char* acDev)
{
  //CAL!
  // TODO: use env "TSLIB_TSDEVICE" instead
  // Open in non-blocking mode
  microSDL_m_ts = ts_open(acDev,1);
  if (!microSDL_m_ts) {
    fprintf(stderr,"ERROR: microSDL_TsOpen\n");
    return false;
  }

  if (ts_config(microSDL_m_ts)) {
    fprintf(stderr,"ERROR: microSDL_TsConfig\n");
    return false;
  }
  return true;
}

int microSDL_GetTsClick(int &nX,int &nY,unsigned &nPress)
{
  ts_sample   pSamp;
  int nRet = ts_read(microSDL_m_ts,&pSamp,1);
  nX = pSamp.x;
  nY = pSamp.y;
  nPress = pSamp.pressure;
  return nRet;
}

#endif // INC_TS



// ------------------------------------------------------------------------
// Private Functions
// ------------------------------------------------------------------------

Uint32 microSDL_GenPixelColor(SDL_Color nCol)
{
  return SDL_MapRGB(microSDL_m_surfScreen->format,nCol.r,nCol.g,nCol.b);
}


Uint32 microSDL_GetPixelRaw(SDL_Surface *surf, int nX, int nY)
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
void microSDL_PutPixelRaw(SDL_Surface *surf, int nX, int nY, Uint32 nPixelVal)
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

void microSDL_SetPixelRaw(Sint16 nX,Sint16 nY,Uint32 nPixelCol)
{
  microSDL_PutPixelRaw(microSDL_m_surfScreen,nX,nY,nPixelCol);
}


bool microSDL_Lock()
{
  if (SDL_MUSTLOCK(microSDL_m_surfScreen)) {
    if (SDL_LockSurface(microSDL_m_surfScreen) < 0) {
      fprintf(stderr,"ERROR: Can't lock screen: %s\n",SDL_GetError());
      return false;
    }
  }   
  return true;
}


void microSDL_Unlock()
{
  if (SDL_MUSTLOCK(microSDL_m_surfScreen)) {
    SDL_UnlockSurface(microSDL_m_surfScreen);
  }
}

// NOTE: nId is a positive ID specified by the user or
//       MSDL_ID_ANON if there is no need to refer to it again.
microSDL_tsElem microSDL_ElemCreate(int nId,unsigned nPageId,unsigned nType,
  SDL_Rect rElem,const char* pStr,unsigned nFontId)
{
  microSDL_tsElem sElem;
  sElem.bValid = false;

  // Validate the user-supplied ID
  if (nId == MSDL_ID_ANON) {
    // Anonymous ID requested, no further checks required
  } else {
    // Ensure the ID is positive
    if (nId < 0) {
      printf("ERROR: microSDL_ElemCreate() called with negative ID (%d)\n",nId);
      return sElem;
    }
    // Ensure the ID isn't already taken
    if (microSDL_ElemFindIndFromId(nId) != MSDL_ID_NONE) {
      printf("ERROR: microSDL_Create() called with existing ID (%d)\n",nId);
      return sElem;
    }
  }

  sElem.nId = nId;
  sElem.nPage = nPageId;
  sElem.rElem = rElem;
  sElem.nType = nType;
  sElem.pTxtFont = microSDL_FontGet(nFontId);
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
  sElem.eTxtAlignH = MSDL_ALIGN_H_MID;
  sElem.eTxtAlignV = MSDL_ALIGN_V_MID;
  sElem.nTxtMargin = 5;
  sElem.bFillEn = false;
  sElem.bFrameEn = false;
  sElem.bClickEn = false;

  // If the element creation was successful, then set the valid flag
  sElem.bValid = true;

  return sElem;
}


int microSDL_ElemGetId(int nElemInd)
{
  if ((nElemInd >= 0) && (nElemInd < microSDL_m_nElemCnt)) {
    return microSDL_m_asElem[nElemInd].nId;
  } else {
    return MSDL_ID_NONE;  
  }
}


// PRE:
// - Assumes that the element is on the active display
//
// TODO: Handle MSDL_TYPE_BKGND
// TODO: Handle layers
bool microSDL_ElemDrawByInd(int nElemInd)
{
  if ((nElemInd < 0) || (nElemInd >= ELEM_MAX)) {
    return false;
  }

  microSDL_tsElem   sElem;
  bool              bSel;
  unsigned          nElemX,nElemY,nElemW,nElemH;
  SDL_Surface*      surfTxt = NULL;

  sElem = microSDL_m_asElem[nElemInd];
  nElemX = sElem.rElem.x;
  nElemY = sElem.rElem.y;
  nElemW = sElem.rElem.w;
  nElemH = sElem.rElem.h;

  // Determine if this element is currently hovered over
  // TODO: Rename hover for clarity
  bSel = false;
  if (microSDL_m_nTrackElemHover == nElemInd) {
    // This was the hover button
    if (microSDL_m_bTrackElemHoverGlow) {
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
      SDL_FillRect(microSDL_m_surfScreen,&sElem.rElem,
        SDL_MapRGB(microSDL_m_surfScreen->format,
            sElem.colElemFillSel.r,
            sElem.colElemFillSel.g,
            sElem.colElemFillSel.b) );
    } else {
      SDL_FillRect(microSDL_m_surfScreen,&sElem.rElem,
        SDL_MapRGB(microSDL_m_surfScreen->format,
            sElem.colElemFill.r,
            sElem.colElemFill.g,
            sElem.colElemFill.b) );
    }
  } else {
    // TODO: If unfilled, then we might need
    // to redraw the background layer(s)
  }

  // Handle special element types
  // TODO: Add owner-draw type
  if (sElem.nType == MSDL_TYPE_GAUGE) {
    microSDL_ElemDraw_Gauge(sElem);
  }

  // Frame the region
  #ifdef DBG_FRAME
  // For debug purposes, draw a frame around every element
  microSDL_FrameRect(nElemX,nElemY,nElemW,nElemH, m_colGrayDk);
  #else
  if (sElem.bFrameEn) {
    microSDL_FrameRect(nElemX,nElemY,nElemW,nElemH, sElem.colElemFrame);
  }
  #endif

  // Draw any images associated with element
  if (sElem.pSurf != NULL) {
    if ((bSel) && (sElem.pSurfSel != NULL)) {
      microSDL_ApplySurface(nElemX,nElemY,sElem.pSurfSel,microSDL_m_surfScreen);
    } else {
      microSDL_ApplySurface(nElemX,nElemY,sElem.pSurf,microSDL_m_surfScreen);
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
    if      (sElem.eTxtAlignH == MSDL_ALIGN_H_LEFT)  { nTxtX = nElemX+nMargin; }
    else if (sElem.eTxtAlignH == MSDL_ALIGN_H_MID)   { nTxtX = nElemX+(nElemW/2)-(nTxtSzW/2); }
    else if (sElem.eTxtAlignH == MSDL_ALIGN_H_RIGHT) { nTxtX = nElemX+nElemW-nMargin-nTxtSzW; }
    if      (sElem.eTxtAlignV == MSDL_ALIGN_V_TOP)   { nTxtY = nElemY+nMargin; }
    else if (sElem.eTxtAlignV == MSDL_ALIGN_V_MID)   { nTxtY = nElemY+(nElemH/2)-(nTxtSzH/2); }
    else if (sElem.eTxtAlignV == MSDL_ALIGN_V_BOT)   { nTxtY = nElemY+nElemH-nMargin-nTxtSzH; }

    microSDL_ApplySurface(nTxtX,nTxtY,surfTxt,microSDL_m_surfScreen);

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
  microSDL_Flip();

  return true;
}


bool microSDL_ElemDraw_Gauge(microSDL_tsElem sElem)
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

  SDL_FillRect(microSDL_m_surfScreen,&rGauge,
    SDL_MapRGB(microSDL_m_surfScreen->format,
      sElem.colGauge.r,
      sElem.colGauge.g,
      sElem.colGauge.b) );

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
  SDL_FillRect(microSDL_m_surfScreen,&rMidLine,
    SDL_MapRGB(microSDL_m_surfScreen->format,
      sElem.colElemFrame.r,
      sElem.colElemFrame.g,
      sElem.colElemFrame.b) );

  return true;
}


void microSDL_ElemCloseAll()
{
  unsigned nElemInd;
  for (nElemInd=0;nElemInd<microSDL_m_nElemCnt;nElemInd++) {
    if (microSDL_m_asElem[nElemInd].pSurf != NULL) {
      SDL_FreeSurface(microSDL_m_asElem[nElemInd].pSurf);
      microSDL_m_asElem[nElemInd].pSurf = NULL;
    }
    if (microSDL_m_asElem[nElemInd].pSurfSel != NULL) {
      SDL_FreeSurface(microSDL_m_asElem[nElemInd].pSurfSel);
      microSDL_m_asElem[nElemInd].pSurfSel = NULL;
    }
  }

  // TODO: Consider moving into main element array
  if (microSDL_m_surfBkgnd != NULL) {
    SDL_FreeSurface(microSDL_m_surfBkgnd);
    microSDL_m_surfBkgnd = NULL;
  }

}

// Handle MOUSEDOWN
void microSDL_TrackTouchDownClick(int nX,int nY)
{
  // Assume no buttons in hover or clicked

  // Find button to start hover
  // Note that microSDL_ElemFindFromCoord() filters out non-clickable elements
  int nHoverStart = microSDL_ElemFindFromCoord(nX,nY);

  if (nHoverStart != MSDL_ID_NONE) {
    // Touch click down on button
    // Start hover
    microSDL_m_nTrackElemHover = nHoverStart;
    microSDL_m_bTrackElemHoverGlow = true;
    #ifdef DBG_TOUCH
    printf("microSDL_TrackTouchDownClick @ (%3u,%3u): on button [Ind=%u]\n",nX,nY,
      microSDL_m_nTrackElemHover);
    #endif
    // Redraw button
    microSDL_ElemDrawByInd(nHoverStart);
  } else {
    #ifdef DBG_TOUCH
    printf("microSDL_TrackTouchDownClick @ (%3u,%3u): not on button\n",nX,nY);
    #endif
  }
}

// Handle MOUSEUP
//
// POST:
// - Updates microSDL_m_nTrackElemClicked
//
void microSDL_TrackTouchUpClick(int nX,int nY)
{
  // Find button at point of mouse-up
  // NOTE: We could possibly use microSDL_m_nTrackElemHover instead
  int nHoverNew = microSDL_ElemFindFromCoord(nX,nY);

  if (nHoverNew == MSDL_ID_NONE) {
    // Release not over a button
    #ifdef DBG_TOUCH
    printf("microSDL_TrackTouchUpClick: not on button\n");
    #endif
  } else {
    // Released over button
    // Was it released over original hover button?
    if (nHoverNew == microSDL_m_nTrackElemHover) {
      // Yes, proceed to select
      microSDL_m_nTrackElemClicked = microSDL_m_nTrackElemHover;
      #ifdef DBG_TOUCH
      printf("microSDL_TrackTouchUpClick: on hovered button\n");
      printf("Track: Selected button [Ind=%u]\n",microSDL_m_nTrackElemClicked);
      #endif
    } else {
      // No, ignore
      #ifdef DBG_TOUCH
      printf("microSDL_TrackTouchUpClick: on another button\n");
      #endif
    }
  }

  // Clear hover state(s)
  microSDL_m_bTrackElemHoverGlow = false;
  microSDL_ElemDrawByInd(microSDL_m_nTrackElemHover);
  microSDL_ElemDrawByInd(nHoverNew);

  microSDL_m_nTrackElemHover = MSDL_ID_NONE;

}

// Handle MOUSEMOVE while MOUSEDOWN
void microSDL_TrackTouchDownMove(int nX,int nY)
{
  int nHoverOld = microSDL_m_nTrackElemHover;
  int nHoverNew = microSDL_ElemFindFromCoord(nX,nY);

  if (nHoverNew == MSDL_ID_NONE) {
    // Not currently over a button
    // If we were previously hovering, stop hover glow now
    if (microSDL_m_bTrackElemHoverGlow) {
      microSDL_m_bTrackElemHoverGlow = false;
      microSDL_ElemDrawByInd(microSDL_m_nTrackElemHover);
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
    if (!microSDL_m_bTrackElemHoverGlow) {
      microSDL_m_bTrackElemHoverGlow = true;
      microSDL_ElemDrawByInd(microSDL_m_nTrackElemHover);
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





