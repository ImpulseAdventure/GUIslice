// =======================================================================
// GUIslice library
// - Calvin Hass
// - https://www.impulseadventure.com/elec/guislice-gui.html
// - https://github.com/ImpulseAdventure/GUIslice
//
// - Version 0.10.4   (2018/10/13)
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


// GUIslice library

// Import configuration ( which will import a sub-config depending on device type)
#include "GUIslice_config.h"

#include "GUIslice.h"
#include "GUIslice_ex.h"
#include "GUIslice_drv.h"

#include <stdio.h>

#ifdef DBG_FRAME_RATE
  #include <time.h> // for FrameRate reporting
#endif

#if (GSLC_USE_FLOAT)
  #include <math.h>
#endif

#if (GSLC_USE_PROGMEM)
  #include <avr/pgmspace.h>   // For memcpy_P()
#endif

#include <stdarg.h>         // For va_*

// Version definition
#define GUISLICE_VER "0.10.2"


// ========================================================================



/// Global debug output function
/// - The user assigns this function via gslc_InitDebug()
GSLC_CB_DEBUG_OUT g_pfDebugOut = NULL;

// Forward declaration for trigonometric lookup table
extern uint16_t  m_nLUTSinF0X16[257];


// ------------------------------------------------------------------------
// Error Strings
// - These strings are generally stored in FLASH memory if possible
// ------------------------------------------------------------------------

const char GSLC_PMEM ERRSTR_NULL[]      = "ERROR: %z() called with NULL ptr\n";
const char GSLC_PMEM ERRSTR_PXD_NULL[]  = "ERROR: %z() pXData NULL\n";


// ------------------------------------------------------------------------
// General Functions
// ------------------------------------------------------------------------

char* gslc_GetVer(gslc_tsGui* pGui)
{
  return (char*)GUISLICE_VER;
}

bool gslc_Init(gslc_tsGui* pGui,void* pvDriver,gslc_tsPage* asPage,uint8_t nMaxPage,gslc_tsFont* asFont,uint8_t nMaxFont)
{
  unsigned  nInd;
  bool      bOk = true;

  // Initialize state
  pGui->nDispW          = 0;
  pGui->nDispH          = 0;
  pGui->nDispDepth      = 0;

  #if defined(DRV_DISP_ADAGFX) || defined(DRV_DISP_ADAGFX_AS) || defined(DRV_DISP_TFT_ESPI) || defined(DRV_DISP_M5STACK)
    pGui->nRotation		= GSLC_ROTATE;
    pGui->nSwapXY		= ADATOUCH_SWAP_XY;
    pGui->nFlipX		= ADATOUCH_FLIP_X;
    pGui->nFlipY		= ADATOUCH_FLIP_Y;
  #endif

  pGui->nPageMax        = nMaxPage;
  pGui->nPageCnt        = 0;
  pGui->asPage          = asPage;

  pGui->pCurPage        = NULL;
  pGui->pCurPageCollect = NULL;

  // Initialize collection of fonts with user-supplied pointer
  pGui->asFont      = asFont;
  pGui->nFontMax    = nMaxFont;
  pGui->nFontCnt    = 0;
  for (nInd=0;nInd<(pGui->nFontMax);nInd++) {
    gslc_ResetFont(&(pGui->asFont[nInd]));
  }

  // Initialize temporary element
#if (GSLC_FEATURE_COMPOUND)
  gslc_ResetElem(&(pGui->sElemTmp));
#endif


  // Last touch event
  pGui->nTouchLastX           = 0;
  pGui->nTouchLastY           = 0;
  pGui->nTouchLastPress       = 0;

  pGui->pfuncXEvent = NULL;

  pGui->sImgRefBkgnd = gslc_ResetImage();

  // Save a link to the driver
  pGui->pvDriver = pvDriver;

  // Default to no support for partial redraw
  // - This may be overridden by the driver-specific init
  pGui->bRedrawPartialEn = false;


  #ifdef DBG_FRAME_RATE
  pGui->nFrameRateCnt = 0;
  pGui->nFrameRateStart = time(NULL);
  #endif

  // Initialize the display and touch drivers
  if (bOk) { bOk &= gslc_DrvInit(pGui); }
  #if !defined(DRV_TOUCH_NONE)
  if (bOk) { bOk &= gslc_InitTouch(pGui,GSLC_DEV_TOUCH); }
  #endif

  if (!bOk) { GSLC_DEBUG_PRINT("ERROR: Init(%s) failed\n",""); }
  return bOk;
}


void gslc_InitDebug(GSLC_CB_DEBUG_OUT pfunc)
{
  g_pfDebugOut = pfunc;
}


// Internal enumerations for printf() parser state machine
typedef enum {
  GSLC_DEBUG_PRINT_NORM,
  GSLC_DEBUG_PRINT_TOKEN,
  GSLC_DEBUG_PRINT_UINT16,
  GSLC_DEBUG_PRINT_STR,
  GSLC_DEBUG_PRINT_STR_P
} gslc_teDebugPrintState;

// A lightweight printf() routine that calls user function for
// character output (enabling redirection to Serial). Only
// supports the following tokens:
// - %u (16-bit unsigned int in RAM) [see NOTE]
// - %d (16-bit signed int in RAM)
// - %s (null-terminated string in RAM)
// - %z (null-terminated string in FLASH)
// Format strings are expected to be in FLASH if GSLC_USE_PROGMEM enabled
// NOTE:
// - Due to the way variadic arguments are passed, we can't pass uint16_t on Arduino
//   as the parameters are promoted to "int" (ie. int16_t). Passing a value over 32767
//   appears to be promoted to int32_t which involves pushing two more bytes onto
//   the stack, causing the remainder of the va_args() to be offset.
// PRE:
// - g_pfDebugOut defined
void gslc_DebugPrintf(const char* pFmt, ...)
{
  if (g_pfDebugOut) {

    char*    pStr=NULL;
    unsigned nMaxDivisor;
    unsigned nNumRemain=0;
    bool     bNumStart=false,bNumNeg=false;
    unsigned nNumDivisor=1;
    uint16_t nFmtInd=0;
    char     cFmt,cOut;

    va_list  vlist;
    va_start(vlist,pFmt);

    gslc_teDebugPrintState  nState = GSLC_DEBUG_PRINT_NORM;

    // Determine maximum number digit size
    #if defined(__AVR__)
      nMaxDivisor = 10000;      // ~2^16
    #else
      nMaxDivisor = 1000000000; // ~2^32
    #endif

    #if (GSLC_USE_PROGMEM)
    cFmt = pgm_read_byte(&pFmt[nFmtInd]);
    #else
    cFmt = pFmt[nFmtInd];
    #endif

    while (cFmt != 0) {

      if (nState == GSLC_DEBUG_PRINT_NORM) {

        if (cFmt == '%') {
          nState = GSLC_DEBUG_PRINT_TOKEN;
        } else {
          // Normal char
          (g_pfDebugOut)(cFmt);
        }
        nFmtInd++; // Advance format index

      } else if (nState == GSLC_DEBUG_PRINT_TOKEN) {

        // Get token
        if (cFmt == 'd') {
          nState = GSLC_DEBUG_PRINT_UINT16;
          // Detect negative value and convert to unsigned value
          // with negation flag. This enables us to reuse the same
          // decoding logic.
          int nNumInt = va_arg(vlist,int);
          if (nNumInt < 0) {
            bNumNeg = true;
            nNumRemain = -nNumInt;
          } else {
            bNumNeg = false;
            nNumRemain = nNumInt;
          }
          bNumStart = false;
          nNumDivisor = nMaxDivisor;

        } else if (cFmt == 'u') {
          nState = GSLC_DEBUG_PRINT_UINT16;
          nNumRemain = va_arg(vlist,unsigned);
          bNumNeg = false;
          bNumStart = false;
          nNumDivisor = nMaxDivisor;

        } else if (cFmt == 's') {
          nState = GSLC_DEBUG_PRINT_STR;
          pStr = va_arg(vlist,char*);

        } else if (cFmt == 'z') {
          nState = GSLC_DEBUG_PRINT_STR_P;
          pStr = va_arg(vlist,char*);
        } else {
          // ERROR
        }
        nFmtInd++; // Advance format index

      } else if (nState == GSLC_DEBUG_PRINT_STR) {
        while (*pStr != 0) {
          cOut = *pStr;
          (g_pfDebugOut)(cOut);
          pStr++;
        }
        nState = GSLC_DEBUG_PRINT_NORM;
        // Don't advance format string index

      } else if (nState == GSLC_DEBUG_PRINT_STR_P) {
        do {
          #if (GSLC_USE_PROGMEM)
          cOut = pgm_read_byte(pStr);
          #else
          cOut = *pStr;
          #endif
          if (cOut != 0) {
            (g_pfDebugOut)(cOut);
            pStr++;
          }
        } while (cOut != 0);
        nState = GSLC_DEBUG_PRINT_NORM;
        // Don't advance format string index

      } else if (nState == GSLC_DEBUG_PRINT_UINT16) {

        // Handle the negation flag if required
        if (bNumNeg) {
          cOut = '-';
          (g_pfDebugOut)(cOut);
          bNumNeg = false;  // Clear the negation flag
        }

        // We remain in this state until we have consumed all of the digits
        // in the original number (starting with the most significant).
        // Each time we process a digit, the parser doesn't advance its input.
        if (nNumRemain < nNumDivisor) {
          if (bNumStart) {
            cOut = '0';
            (g_pfDebugOut)(cOut);
          } else {
            // We haven't started outputting a number yet
            // Check for special case of zero
            if (nNumRemain == 0) {
              cOut = '0';
              (g_pfDebugOut)(cOut);
              // Now fall through to done state
              nNumDivisor = 1;
            }
          }
        } else {
          bNumStart = true;
          unsigned nValDigit = nNumRemain / nNumDivisor;
          cOut = nValDigit+'0';
          nNumRemain -= nNumDivisor*nValDigit;
          (g_pfDebugOut)(cOut);
        }

        // Detect end of digit decode (ie. 1's)
        if (nNumDivisor == 1) {
          // Done
          nState = GSLC_DEBUG_PRINT_NORM;
        } else {
          // Shift the divisor by an order of magnitude
          nNumDivisor /= 10;
        }
        // Don't advance format string index

      }

      // Read the format string (usually the next character)
      #if (GSLC_USE_PROGMEM)
      cFmt = pgm_read_byte(&pFmt[nFmtInd]);
      #else
      cFmt = pFmt[nFmtInd];
      #endif

    }
    va_end(vlist);

  } // g_pfDebugOut


}

// ------------------------------------------------------------------------
// Error strings
// ------------------------------------------------------------------------
extern const char ERRSTR_NULL[];

// ------------------------------------------------------------------------


void gslc_Quit(gslc_tsGui* pGui)
{
  // Close all elements and fonts
  gslc_GuiDestruct(pGui);
}

// Main polling loop for GUIslice
void gslc_Update(gslc_tsGui* pGui)
{
  if (pGui->pCurPage == NULL) {
    return; // No page added yet
  }

  #if !defined(DRV_TOUCH_NONE)

  // ---------------------------------------------
  // Touch handling
  // ---------------------------------------------

  int16_t   nTouchX = 0;
  int16_t   nTouchY = 0;
  uint16_t  nTouchPress = 0;
  bool      bTouchEvent = true;

  // Handle touchscreen presses
  // - We clear the event queue here so that we don't fall behind
  // - In the time it takes to update the display, several mouse /
  //   finger events may have occurred. If we only handle a single
  //   motion event per display update, then we may experience very
  //   lagging responsiveness from the controls.
  // - Instead, we drain the even queue before proceeding on to the
  //   display update, giving rise to a much more responsive GUI.
  //   The maximum number of touch events that can be handled per
  //   main loop is defined by the GSLC_TOUCH_MAX_EVT config param.
  // - Note that SDL2 may synchronize the RenderPresent call to
  //   the VSYNC, which will effectively insert a delay into the
  //   gslc_PageRedrawGo() call below. It might be possible to
  //   adjust this blocking behavior via SDL_RENDERER_PRESENTVSYNC.

  // In case we are flooded with events, limit the maximum number
  // that we handle in one gslc_Update() call.
  bool      bDoneEvts = false;
  uint16_t  nNumEvts  = 0;
  do {
    bTouchEvent = gslc_GetTouch(pGui,&nTouchX,&nTouchY,&nTouchPress);
    if (bTouchEvent) {
      // Track and handle the touch events
      // - Handle the events on the current page
      gslc_TrackTouch(pGui,pGui->pCurPage,nTouchX,nTouchY,nTouchPress);

      #ifdef DBG_TOUCH
      // Highlight current touch for coordinate debug
      gslc_tsRect rMark = gslc_ExpandRect((gslc_tsRect){(int16_t)nTouchX,(int16_t)nTouchY,1,1},1,1);
      gslc_DrawFrameRect(pGui,rMark,GSLC_COL_YELLOW);
      #endif

      nNumEvts++;
    }

    // Should we stop handling events?
    if ((!bTouchEvent) || (nNumEvts >= GSLC_TOUCH_MAX_EVT)) {
      bDoneEvts = true;
    }
  } while (!bDoneEvts);

  #endif // !DRV_TOUCH_NONE

  // ---------------------------------------------

  // Issue a timer tick to all pages
  uint8_t nPageInd;
  gslc_tsPage* pPage = NULL;
  for (nPageInd=0;nPageInd<pGui->nPageCnt;nPageInd++) {
    pPage = &pGui->asPage[nPageInd];
    gslc_tsEvent sEvent = gslc_EventCreate(pGui,GSLC_EVT_TICK,0,(void*)pPage,NULL);
    gslc_PageEvent(pGui,sEvent);
  }

  // Perform any redraw required for current page
  gslc_PageRedrawGo(pGui);

  // Simple "frame" rate reporting
  // - Note that the rate is based on the number of calls to gslc_Update()
  //   per second, which may or may not redraw the frame
  #ifdef DBG_FRAME_RATE
  pGui->nFrameRateCnt++;
  uint32_t  nElapsed = (time(NULL) - pGui->nFrameRateStart);
  if (nElapsed > 0) {
    GSLC_DEBUG_PRINT("Update rate: %6u / sec\n",pGui->nFrameRateCnt);
    pGui->nFrameRateStart = time(NULL);
    pGui->nFrameRateCnt = 0;
  }
  #endif

}

gslc_tsEvent  gslc_EventCreate(gslc_tsGui* pGui,gslc_teEventType eType,uint8_t nSubType,void* pvScope,void* pvData)
{
  gslc_tsEvent      sEvent;
  sEvent.eType      = eType;
  sEvent.nSubType   = nSubType;
  sEvent.pvScope    = pvScope;
  sEvent.pvData     = pvData;
  return sEvent;
}


// ------------------------------------------------------------------------
// Graphics General Functions
// ------------------------------------------------------------------------


bool gslc_IsInRect(int16_t nSelX,int16_t nSelY,gslc_tsRect rRect)
{
  if ( (nSelX >= rRect.x) && (nSelX <= rRect.x+rRect.w) &&
     (nSelY >= rRect.y) && (nSelY <= rRect.y+rRect.h) ) {
    return true;
  } else {
    return false;
  }
}

bool gslc_IsInWH(int16_t nSelX,int16_t nSelY,uint16_t nWidth,uint16_t nHeight)
{
  if ( (nSelX >= 0) && (nSelX <= nWidth-1) &&
     (nSelY >= 0) && (nSelY <= nHeight-1) ) {
    return true;
  } else {
    return false;
  }
}

// Ensure the coordinates are increasing from nX0->nX1 and nY0->nY1
// NOTE: UNUSED
void gslc_OrderCoord(int16_t* pnX0,int16_t* pnY0,int16_t* pnX1,int16_t* pnY1)
{
  int16_t  nTmp;
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


bool gslc_ClipPt(gslc_tsRect* pClipRect,int16_t nX,int16_t nY)
{
  int16_t nCX0 = pClipRect->x;
  int16_t nCY0 = pClipRect->y;
  int16_t nCX1 = pClipRect->x + pClipRect->w - 1;
  int16_t nCY1 = pClipRect->y + pClipRect->h - 1;
  if ( (nX < nCX0) || (nX > nCX1) ) { return false; }
  if ( (nY < nCY0) || (nY > nCY1) ) { return false; }
  return true;
}


// This routine implements a basic Cohen-Sutherland line-clipping algorithm
// TODO: Optimize the code further
bool gslc_ClipLine(gslc_tsRect* pClipRect,int16_t* pnX0,int16_t* pnY0,int16_t* pnX1,int16_t* pnY1)
{
  int16_t nTmpX,nTmpY;
  int16_t nCXMin,nCXMax,nCYMin,nCYMax;
  uint8_t nRegion0,nRegion1,nRegionSel;
  int16_t nLX0,nLY0,nLX1,nLY1;

  int16_t nCX0 = pClipRect->x;
  int16_t nCY0 = pClipRect->y;
  int16_t nCX1 = pClipRect->x + pClipRect->w - 1;
  int16_t nCY1 = pClipRect->y + pClipRect->h - 1;

  if (nCX0 > nCX1) {
    nCXMin = nCX1;
    nCXMax = nCX0;
  } else {
    nCXMin = nCX0;
    nCXMax = nCX1;
  }
  if (nCY0 > nCY1) {
    nCYMin = nCY1;
    nCYMax = nCY0;
  } else {
    nCYMin = nCY0;
    nCYMax = nCY1;
  }

  nTmpX = 0;
  nTmpY = 0;
  while (1) {
    nLX0 = *pnX0;
    nLY0 = *pnY0;
    nLX1 = *pnX1;
    nLY1 = *pnY1;

    // Step 1: Assign a region code to each endpoint
    nRegion0 = 0;
    nRegion1 = 0;
    if      (nLX0 < nCX0) { nRegion0 |= 1; }
    else if (nLX0 > nCX1) { nRegion0 |= 2; }
    if      (nLY0 < nCY0) { nRegion0 |= 4; }
    else if (nLY0 > nCY1) { nRegion0 |= 8; }
    if      (nLX1 < nCX0) { nRegion1 |= 1; }
    else if (nLX1 > nCX1) { nRegion1 |= 2; }
    if      (nLY1 < nCY0) { nRegion1 |= 4; }
    else if (nLY1 > nCY1) { nRegion1 |= 8; }

    // Step 2: Check for complete inclusion
    if ((nRegion0 == 0) && (nRegion1 == 0)) {
      return true;
    }

    // Step 3: Check for complete exclusion
    if ((nRegion0 & nRegion1) != 0) {
      return false;
    }

    // Step 4: Clipping
    nRegionSel = nRegion0 ? nRegion0 : nRegion1;
    if (nRegionSel & 8) {
      nTmpX = nLX0 + (nLX1 - nLX0) * (nCYMax - nLY0) / (nLY1 - nLY0);
      nTmpY = nCYMax;
    } else if (nRegionSel & 4) {
      nTmpX = nLX0 + (nLX1 - nLX0) * (nCYMin - nLY0) / (nLY1 - nLY0);
      nTmpY = nCYMin;
    } else if (nRegionSel & 2) {
      nTmpY = nLY0 + (nLY1 - nLY0) * (nCXMax - nLX0) / (nLX1 - nLX0);
      nTmpX = nCXMax;
    } else if (nRegionSel & 1) {
      nTmpY = nLY0 + (nLY1 - nLY0) * (nCXMin - nLX0) / (nLX1 - nLX0);
      nTmpX = nCXMin;
    }

    // Update endpoint
    if (nRegionSel == nRegion0) {
      *pnX0 = nTmpX;
      *pnY0 = nTmpY;
    } else {
      *pnX1 = nTmpX;
      *pnY1 = nTmpY;
    }
  } // while(1)

  return true;
}


bool gslc_ClipRect(gslc_tsRect* pClipRect,gslc_tsRect* pRect)
{
  int16_t nCX0 = pClipRect->x;
  int16_t nCY0 = pClipRect->y;
  int16_t nCX1 = pClipRect->x + pClipRect->w - 1;
  int16_t nCY1 = pClipRect->y + pClipRect->h - 1;
  int16_t nRX0 = pRect->x;
  int16_t nRY0 = pRect->y;
  int16_t nRX1 = pRect->x + pRect->w - 1;
  int16_t nRY1 = pRect->y + pRect->h - 1;
  // Check for completely out of clip view
  if ( (nRX1 < nCX0) || (nRX0 > nCX1) ) { return false; }
  if ( (nRY1 < nCY0) || (nRY0 > nCY1) ) { return false; }
  // Reduce rect as required to fit view
  nRX0 = (nRX0<nCX0)? nCX0 : nRX0;
  nRY0 = (nRY0<nCY0)? nCY0 : nRY0;
  nRX1 = (nRX1>nCX1)? nCX1 : nRX1;
  nRY1 = (nRY1>nCY1)? nCY1 : nRY1;
  pRect->x = nRX0;
  pRect->y = nRY0;
  pRect->w = nRX1-nRX0+1;
  pRect->h = nRY1-nRY0+1;
  return true;
}



gslc_tsImgRef gslc_ResetImage()
{
  gslc_tsImgRef sImgRef;
  sImgRef.eImgFlags = GSLC_IMGREF_NONE;
  sImgRef.pFname    = NULL;
  sImgRef.pImgBuf   = NULL;
  sImgRef.pvImgRaw  = NULL;
  return sImgRef;
}

gslc_tsImgRef gslc_GetImageFromFile(const char* pFname,gslc_teImgRefFlags eFmt)
{
  gslc_tsImgRef sImgRef;
  sImgRef.eImgFlags = GSLC_IMGREF_SRC_FILE | (GSLC_IMGREF_FMT & eFmt);
  sImgRef.pFname    = pFname;
  sImgRef.pImgBuf   = NULL;
  sImgRef.pvImgRaw  = NULL;
  return sImgRef;
}

gslc_tsImgRef gslc_GetImageFromSD(const char* pFname,gslc_teImgRefFlags eFmt)
{
  gslc_tsImgRef sImgRef;
#if (GSLC_SD_EN)
  sImgRef.eImgFlags = GSLC_IMGREF_SRC_SD | (GSLC_IMGREF_FMT & eFmt);
  sImgRef.pFname    = pFname;
  sImgRef.pImgBuf   = NULL;
  sImgRef.pvImgRaw  = NULL;
#else
  // TODO: Change message to also handle non-Arduino output
  GSLC_DEBUG_PRINT("ERROR: GetImageFromSD(%s) not supported as Config:GSLC_SD_EN=0\n","");
  sImgRef.eImgFlags = GSLC_IMGREF_NONE;
#endif
  return sImgRef;
}

gslc_tsImgRef gslc_GetImageFromRam(unsigned char* pImgBuf,gslc_teImgRefFlags eFmt)
{
  gslc_tsImgRef sImgRef;
  sImgRef.eImgFlags = GSLC_IMGREF_SRC_RAM | (GSLC_IMGREF_FMT & eFmt);
  sImgRef.pFname    = NULL;
  sImgRef.pImgBuf   = pImgBuf;
  sImgRef.pvImgRaw  = NULL;
  return sImgRef;
}


gslc_tsImgRef gslc_GetImageFromProg(const unsigned char* pImgBuf,gslc_teImgRefFlags eFmt)
{
  gslc_tsImgRef sImgRef;
  sImgRef.eImgFlags = GSLC_IMGREF_SRC_PROG | (GSLC_IMGREF_FMT & eFmt);
  sImgRef.pFname    = NULL;
  sImgRef.pImgBuf   = pImgBuf;
  sImgRef.pvImgRaw  = NULL;
  return sImgRef;
}


// Sine function with optional lookup table
int16_t gslc_sinFX(int16_t n64Ang)
{
  int16_t   nRetValS;

#if (GSLC_USE_FLOAT)
  // Use floating-point math library function

  // Calculate angle in radians
  float fAngRad = n64Ang*GSLC_2PI/(360.0*64.0);
  // Perform floating point calc
  float fSin = sin(fAngRad);
  // Return as fixed point result
  nRetValS = fSin * 32767.0;
  return nRetValS;

#else
  // Use lookup tables
  bool bNegate = false;

  // Support multiple waveform periods
  if (n64Ang >= 360*64) {
    // For some reason this modulus is broken!
    n64Ang = n64Ang % (int16_t)(360*64);
    //n64Ang = n64Ang - (360*64);
  } else if (n64Ang <= -360*64) {
    n64Ang = -(-n64Ang % 360*64);
  }
  // Handle negative range
  if (n64Ang < 0) {
    n64Ang = -n64Ang;
    bNegate = !bNegate;
  }
  // Handle 3rd and 4th phase
  if (n64Ang >= 180*64) {
    n64Ang -= 180*64;
    bNegate = !bNegate;
  }
  // Handle 2nd phase
  if (n64Ang >= 90*64) {
    n64Ang = 180*64 - n64Ang;
  }

  // n64Ang is quarter-phase range [0 .. 90*64]
  // suitable for lookup table indexing
  uint16_t  nLutInd = (n64Ang * 256)/(90*64);
  uint16_t  nLutVal = m_nLUTSinF0X16[nLutInd];

  // Leave MSB for the signed bit
  nLutVal /= 2;
  if (bNegate) {
    nRetValS = -nLutVal;
  } else {
    nRetValS = nLutVal;
  }
  return nRetValS;

#endif

}

// Cosine function with optional lookup table
int16_t gslc_cosFX(int16_t n64Ang)
{
#if (GSLC_USE_FLOAT)
  int16_t   nRetValS;
  // Use floating-point math library function

  // Calculate angle in radians
  float fAngRad = n64Ang*GSLC_2PI/(360.0*64.0);
  // Perform floating point calc
  float fCos = cos(fAngRad);
  // Return as fixed point result
  nRetValS = fCos * 32767.0;
  return nRetValS;

#else
  // Use lookup tables
  // Cosine function is equivalent to Sine shifted by 90 degrees
  return gslc_sinFX(n64Ang+90*64);

#endif

}

// Convert from polar to cartesian
void gslc_PolarToXY(uint16_t nRad,int16_t n64Ang,int16_t* nDX,int16_t* nDY)
{
  *nDX = (int16_t)nRad *  gslc_sinFX(n64Ang)/(int16_t)32767;
  *nDY = (int16_t)nRad * -gslc_cosFX(n64Ang)/(int16_t)32767;
}

// Call with nMidAmt=500 to create simple linear blend between two colors
gslc_tsColor gslc_ColorBlend2(gslc_tsColor colStart,gslc_tsColor colEnd,uint16_t nMidAmt,uint16_t nBlendAmt)
{
  gslc_tsColor  colMid;
  colMid.r = (colEnd.r+colStart.r)/2;
  colMid.g = (colEnd.g+colStart.g)/2;
  colMid.b = (colEnd.b+colStart.b)/2;
  return gslc_ColorBlend3(colStart,colMid,colEnd,nMidAmt,nBlendAmt);
}

gslc_tsColor gslc_ColorBlend3(gslc_tsColor colStart,gslc_tsColor colMid,gslc_tsColor colEnd,uint16_t nMidAmt,uint16_t nBlendAmt)
{
  gslc_tsColor  colNew;
  nMidAmt   = (nMidAmt  >1000)?1000:nMidAmt;
  nBlendAmt = (nBlendAmt>1000)?1000:nBlendAmt;

  uint16_t  nRngLow   = nMidAmt;
  uint16_t  nRngHigh  = 1000-nMidAmt;
  uint16_t  nSubBlendAmt;
  if (nBlendAmt >= nMidAmt) {
    nSubBlendAmt = (nBlendAmt - nMidAmt)*1000/nRngHigh;
    colNew.r = nSubBlendAmt*(colEnd.r - colMid.r)/1000 + colMid.r;
    colNew.g = nSubBlendAmt*(colEnd.g - colMid.g)/1000 + colMid.g;
    colNew.b = nSubBlendAmt*(colEnd.b - colMid.b)/1000 + colMid.b;
  } else {
    nSubBlendAmt = (nBlendAmt - 0)*1000/nRngLow;
    colNew.r = nSubBlendAmt*(colMid.r - colStart.r)/1000 + colStart.r;
    colNew.g = nSubBlendAmt*(colMid.g - colStart.g)/1000 + colStart.g;
    colNew.b = nSubBlendAmt*(colMid.b - colStart.b)/1000 + colStart.b;
  }
  return colNew;
}

bool gslc_ColorEqual(gslc_tsColor a,gslc_tsColor b)
{
  return a.r == b.r && a.g == b.g && a.b == b.b;
}

// ------------------------------------------------------------------------
// Graphics Primitive Functions
// ------------------------------------------------------------------------

void gslc_DrawSetPixel(gslc_tsGui* pGui,int16_t nX,int16_t nY,gslc_tsColor nCol)
{

#if (DRV_HAS_DRAW_POINT)
  // Call optimized driver point drawing
  gslc_DrvDrawPoint(pGui,nX,nY,nCol);
#else
  GSLC_DEBUG_PRINT("ERROR: Mandatory DrvDrawPoint() is not defined in driver\n");
#endif

  gslc_PageFlipSet(pGui,true);
}

// Draw an arbitrary line using Bresenham's algorithm
// - Algorithm reference: https://rosettacode.org/wiki/Bitmap/Bresenham's_line_algorithm#C
void gslc_DrawLine(gslc_tsGui* pGui,int16_t nX0,int16_t nY0,int16_t nX1,int16_t nY1,gslc_tsColor nCol)
{

#if (DRV_HAS_DRAW_LINE)
  // Call optimized driver line drawing
  gslc_DrvDrawLine(pGui,nX0,nY0,nX1,nY1,nCol);

#else
  // Perform Bresenham's line algorithm
  int16_t nDX = abs(nX1-nX0);
  int16_t nDY = abs(nY1-nY0);

  int16_t nSX = (nX0 < nX1)? 1 : -1;
  int16_t nSY = (nY0 < nY1)? 1 : -1;
  int16_t nErr = ( (nDX>nDY)? nDX : -nDY )/2;
  int16_t nE2;

  // Check for degenerate cases
  // TODO: Need to test these optimizations
  bool bDone = false;
  if (nDX == 0) {
    if (nDY == 0) {
      return;
    } else if (nY1-nY0 >= 0) {
      gslc_DrawLineV(pGui,nX0,nY0,nDY+1,nCol);
      bDone = true;
    } else {
      gslc_DrawLineV(pGui,nX1,nY1,nDY+1,nCol);
      bDone = true;
    }
  } else if (nDY == 0) {
    if (nX1-nX0 >= 0) {
      gslc_DrawLineH(pGui,nX0,nY0,nDX+1,nCol);
      bDone = true;
    } else {
      gslc_DrawLineH(pGui,nX1,nY1,nDX+1,nCol);
      bDone = true;
    }
  }

  if (!bDone) {
    for (;;) {
      // Set the pixel
      gslc_DrvDrawPoint(pGui,nX0,nY0,nCol);

      // Calculate next coordinates
      if ( (nX0 == nX1) && (nY0 == nY1) ) break;
      nE2 = nErr;
      if (nE2 > -nDX) { nErr -= nDY; nX0 += nSX; }
      if (nE2 <  nDY) { nErr += nDX; nY0 += nSY; }
    }
  }
  gslc_PageFlipSet(pGui,true);
#endif

}


void gslc_DrawLineH(gslc_tsGui* pGui,int16_t nX, int16_t nY, uint16_t nW,gslc_tsColor nCol)
{
  uint16_t nOffset;
  for (nOffset=0;nOffset<nW;nOffset++) {
    gslc_DrvDrawPoint(pGui,nX+nOffset,nY,nCol);
  }

  gslc_PageFlipSet(pGui,true);
}

void gslc_DrawLineV(gslc_tsGui* pGui,int16_t nX, int16_t nY, uint16_t nH,gslc_tsColor nCol)
{
  uint16_t nOffset;
  for (nOffset=0;nOffset<nH;nOffset++) {
    gslc_DrvDrawPoint(pGui,nX,nY+nOffset,nCol);
  }

  gslc_PageFlipSet(pGui,true);
}


// Note that angle is in degrees * 64
void gslc_DrawLinePolar(gslc_tsGui* pGui,int16_t nX,int16_t nY,uint16_t nRadStart,uint16_t nRadEnd,int16_t n64Ang,gslc_tsColor nCol)
{
  // Draw the ray representing the current value
  int16_t nDxS = nRadStart * gslc_sinFX(n64Ang)/32768;
  int16_t nDyS = nRadStart * gslc_cosFX(n64Ang)/32768;
  int16_t nDxE = nRadEnd   * gslc_sinFX(n64Ang)/32768;
  int16_t nDyE = nRadEnd   * gslc_cosFX(n64Ang)/32768;
  gslc_DrawLine(pGui,nX+nDxS,nY-nDyS,nX+nDxE,nY-nDyE,nCol);
}


void gslc_DrawFrameRect(gslc_tsGui* pGui,gslc_tsRect rRect,gslc_tsColor nCol)
{
  // Ensure dimensions are valid
  if ((rRect.w == 0) || (rRect.h == 0)) {
    return;
  }

#if (DRV_HAS_DRAW_RECT_FRAME)
  // Call optimized driver implementation
  gslc_DrvDrawFrameRect(pGui,rRect,nCol);
#else
  // Emulate rect frame with four lines
  int16_t   nX,nY;
  uint16_t  nH,nW;
  nX = rRect.x;
  nY = rRect.y;
  nW = rRect.w;
  nH = rRect.h;
  gslc_DrawLineH(pGui,nX,nY,nW-1,nCol);                 // Top
  gslc_DrawLineH(pGui,nX,(int16_t)(nY+nH-1),nW-1,nCol); // Bottom
  gslc_DrawLineV(pGui,nX,nY,nH-1,nCol);                 // Left
  gslc_DrawLineV(pGui,(int16_t)(nX+nW-1),nY,nH-1,nCol); // Right
#endif

  gslc_PageFlipSet(pGui,true);
}

void gslc_DrawFillRect(gslc_tsGui* pGui,gslc_tsRect rRect,gslc_tsColor nCol)
{
  // Ensure dimensions are valid
  if ((rRect.w == 0) || (rRect.h == 0)) {
    return;
  }

#if (DRV_HAS_DRAW_RECT_FILL)
  // Call optimized driver implementation
  gslc_DrvDrawFillRect(pGui,rRect,nCol);
#else
  // Emulate it with individual line draws
  // TODO: This should be avoided as it will generally be very inefficient
  int nRow;
  for (nRow=0;nRow<rRect.h;nRow++) {
    gslc_DrawLineH(pGui, rRect.x, rRect.y+nRow, rRect.w, nCol);
  }

#endif

  gslc_PageFlipSet(pGui,true);
}


// Expand or contract a rectangle in width and/or height (equal
// amounts on both side), based on the centerpoint of the rectangle.
gslc_tsRect gslc_ExpandRect(gslc_tsRect rRect,int16_t nExpandW,int16_t nExpandH)
{
  gslc_tsRect rNew = {0,0,0,0};

  // Detect error case of contracting region too far
  if ( (rRect.w < (-2*nExpandW)) || (rRect.h < (-2*nExpandH)) ) {
    // Return an empty coordinate box (which won't be drawn)
    //GSLC_DEBUG_PRINT("ERROR: ExpandRect(%d,%d) contracts too far\n",nExpandW,nExpandH);
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
void gslc_DrawFrameCircle(gslc_tsGui* pGui,int16_t nMidX,int16_t nMidY,
  uint16_t nRadius,gslc_tsColor nCol)
{

  #if (DRV_HAS_DRAW_CIRCLE_FRAME)
    // Call optimized driver implementation
    gslc_DrvDrawFrameCircle(pGui,nMidX,nMidY,nRadius,nCol);
  #else
    // Emulate circle with point drawing

    int16_t nX    = nRadius;
    int16_t nY    = 0;
    int16_t nErr  = 0;

    #if (DRV_HAS_DRAW_POINTS)
      gslc_tsPt asPt[8];
      while (nX >= nY)
      {
        asPt[0] = (gslc_tsPt){nMidX + nX, nMidY + nY};
        asPt[1] = (gslc_tsPt){nMidX + nY, nMidY + nX};
        asPt[2] = (gslc_tsPt){nMidX - nY, nMidY + nX};
        asPt[3] = (gslc_tsPt){nMidX - nX, nMidY + nY};
        asPt[4] = (gslc_tsPt){nMidX - nX, nMidY - nY};
        asPt[5] = (gslc_tsPt){nMidX - nY, nMidY - nX};
        asPt[6] = (gslc_tsPt){nMidX + nY, nMidY - nX};
        asPt[7] = (gslc_tsPt){nMidX + nX, nMidY - nY};
        gslc_DrvDrawPoints(pGui,asPt,8,nCol);

        nY    += 1;
        nErr  += 1 + 2*nY;
        if (2*(nErr-nX) + 1 > 0)
        {
            nX -= 1;
            nErr += 1 - 2*nX;
        }
      } // while

    #elif (DRV_HAS_DRAW_POINT)
      while (nX >= nY)
      {
        gslc_DrvDrawPoint(pGui,nMidX + nX, nMidY + nY,nCol);
        gslc_DrvDrawPoint(pGui,nMidX + nY, nMidY + nX,nCol);
        gslc_DrvDrawPoint(pGui,nMidX - nY, nMidY + nX,nCol);
        gslc_DrvDrawPoint(pGui,nMidX - nX, nMidY + nY,nCol);
        gslc_DrvDrawPoint(pGui,nMidX - nX, nMidY - nY,nCol);
        gslc_DrvDrawPoint(pGui,nMidX - nY, nMidY - nX,nCol);
        gslc_DrvDrawPoint(pGui,nMidX + nY, nMidY - nX,nCol);
        gslc_DrvDrawPoint(pGui,nMidX + nX, nMidY - nY,nCol);

        nY    += 1;
        nErr  += 1 + 2*nY;
        if (2*(nErr-nX) + 1 > 0)
        {
            nX -= 1;
            nErr += 1 - 2*nX;
        }
      } // while

    #else
      // ERROR
    #endif

  #endif

  gslc_PageFlipSet(pGui,true);
}

// Draw a filled circle using midpoint circle algorithm
// - Algorithm reference: https://en.wikipedia.org/wiki/Midpoint_circle_algorithm
// - Adapted for fill by connecting points by lines
// - No separate frame color is performed
void gslc_DrawFillCircle(gslc_tsGui* pGui,int16_t nMidX,int16_t nMidY,
  uint16_t nRadius,gslc_tsColor nCol)
{

  #if (DRV_HAS_DRAW_CIRCLE_FILL)
    // Call optimized driver implementation
    gslc_DrvDrawFillCircle(pGui,nMidX,nMidY,nRadius,nCol);
  #else
    // Emulate circle with line drawing

    int16_t nX    = nRadius;  // a
    int16_t nY    = 0;        // b
    int16_t nErr  = 0;

    while (nX >= nY)
    {

      // Connect pairs of the reflected points around the circumference

      //gslc_DrvDrawPoint(pGui,nMidX - nY, nMidY + nX,nCol);  // (-b,+a)
      //gslc_DrvDrawPoint(pGui,nMidX + nY, nMidY + nX,nCol);  // (+b,+a)
      gslc_DrawLine(pGui,nMidX-nY,nMidY+nX,nMidX+nY,nMidY+nX,nCol);

      //gslc_DrvDrawPoint(pGui,nMidX - nX, nMidY + nY,nCol);  // (-a,+b)
      //gslc_DrvDrawPoint(pGui,nMidX + nX, nMidY + nY,nCol);  // (+a,+b)
      gslc_DrawLine(pGui,nMidX-nX,nMidY+nY,nMidX+nX,nMidY+nY,nCol);

      //gslc_DrvDrawPoint(pGui,nMidX - nX, nMidY - nY,nCol);  // (-a,-b)
      //gslc_DrvDrawPoint(pGui,nMidX + nX, nMidY - nY,nCol);  // (+a,-b)
      gslc_DrawLine(pGui,nMidX-nX,nMidY-nY,nMidX+nX,nMidY-nY,nCol);

      //gslc_DrvDrawPoint(pGui,nMidX - nY, nMidY - nX,nCol);  // (-b,-a)
      //gslc_DrvDrawPoint(pGui,nMidX + nY, nMidY - nX,nCol);  // (+b,-a)
      gslc_DrawLine(pGui,nMidX-nY,nMidY-nX,nMidX+nY,nMidY-nX,nCol);


      nY    += 1;
      nErr  += 1 + 2*nY;
      if (2*(nErr-nX) + 1 > 0)
      {
          nX -= 1;
          nErr += 1 - 2*nX;
      }
    } // while


  #endif

  gslc_PageFlipSet(pGui,true);
}


// Draw a triangle
void gslc_DrawFrameTriangle(gslc_tsGui* pGui,int16_t nX0,int16_t nY0,
    int16_t nX1,int16_t nY1,int16_t nX2,int16_t nY2,gslc_tsColor nCol)
{

  #if (DRV_HAS_DRAW_TRI_FRAME)
    // Call optimized driver implementation
    gslc_DrvDrawFrameTriangle(pGui,nX0,nY0,nX1,nY1,nX2,nY2,nCol);
  #else
    // Draw triangle with three lines
    gslc_DrawLine(pGui,nX0,nY0,nX1,nY1,nCol);
    gslc_DrawLine(pGui,nX1,nY1,nX2,nY2,nCol);
    gslc_DrawLine(pGui,nX2,nY2,nX0,nY0,nCol);

  #endif

  gslc_PageFlipSet(pGui,true);
}

void gslc_SwapCoords(int16_t* pnXa,int16_t* pnYa,int16_t* pnXb,int16_t* pnYb)
{
  int16_t nSwapX,nSwapY;
  nSwapX = *pnXa;
  nSwapY = *pnYa;
  *pnXa = *pnXb;
  *pnYa = *pnYb;
  *pnXb = nSwapX;
  *pnYb = nSwapY;
}


// Draw a filled triangle
void gslc_DrawFillTriangle(gslc_tsGui* pGui,int16_t nX0,int16_t nY0,
    int16_t nX1,int16_t nY1,int16_t nX2,int16_t nY2,gslc_tsColor nCol)
{

  #if (DRV_HAS_DRAW_TRI_FILL)
    // Call optimized driver implementation
    gslc_DrvDrawFillTriangle(pGui,nX0,nY0,nX1,nY1,nX2,nY2,nCol);

  #else
    // Emulate triangle fill

    // Algorithm:
    // - An arbitrary triangle is cut into two portions:
    //   1) a flat bottom triangle
    //   2) a flat top triangle
    // - Sort the vertices in descending vertical position.
    //   This serves two purposes:
    //   1) ensures that the division between the flat bottom
    //      and flat top triangles occurs at Y=Y1
    //   2) ensure that we avoid division-by-zero in the for loops
    // - Walk each scan line and determine the intersection
    //   between triangle side A & B (flat bottom triangle)
    //   and then C and B (flat top triangle) using line slopes.

    // Sort vertices
    // - Want nY0 >= nY1 >= nY2
    if (nY2>nY1) { gslc_SwapCoords(&nX2,&nY2,&nX1,&nY1); }
    if (nY1>nY0) { gslc_SwapCoords(&nX0,&nY0,&nX1,&nY1); }
    if (nY2>nY1) { gslc_SwapCoords(&nX2,&nY2,&nX1,&nY1); }

    // TODO: It is more efficient to calculate row endpoints
    // using incremental additions instead of multiplies/divides

    int16_t nXa,nXb,nXc,nYos;
    int16_t nX01,nX20,nY01,nY20,nX21,nY21;
    nX01 = nX0-nX1; nY01 = nY0-nY1;
    nX20 = nX2-nX0; nY20 = nY2-nY0;
    nX21 = nX2-nX1; nY21 = nY2-nY1;

    // Flat bottom scenario
    // NOTE: Due to vertex sorting and loop range, it shouldn't
    // be possible to enter loop when nY0 == nY1 or nY2
    for (nYos=0;nYos<nY01;nYos++) {
      // Determine row endpoints (no rounding)
      //nXa = (nYos          )*(nX0-nX1)/(nY0-nY1);
      //nXb = (nYos-(nY0-nY1))*(nX2-nX0)/(nY2-nY0);

      // Determine row endpoints (using rounding)
      nXa  = 2*(nYos)*nX01;
      nXa += (nXa>=0)?abs(nY01):-abs(nY01);
      nXa /= 2*nY01;

      nXb = 2*(nYos-nY01)*nX20;
      nXb += (nXb>=0)?abs(nY20):-abs(nY20);
      nXb /= 2*nY20;

      // Draw horizontal line between endpoints
      gslc_DrawLine(pGui,nX1+nXa,nY1+nYos,nX0+nXb,nY1+nYos,nCol);
    }

    // Flat top scenario
    // NOTE: Due to vertex sorting and loop range, it shouldn't
    // be possible to enter loop when nY2 == nY0 or nY1
    for (nYos=nY21;nYos<0;nYos++) {

      // Determine row endpoints (no rounding)
      //nXc = (nYos          )*(nX2-nX1)/(nY2-nY1);
      //nXb = (nYos-(nY0-nY1))*(nX2-nX0)/(nY2-nY0);

      // Determine row endpoints (using rounding)
      nXc  = 2*(nYos)*nX21;
      nXc += (nXc>=0)?abs(nY21):-abs(nY21);
      nXc /= 2*nY21;

      nXb = 2*(nYos-nY01)*nX20;
      nXb += (nXb>=0)?abs(nY20):-abs(nY20);
      nXb /= 2*nY20;

      // Draw horizontal line between endpoints
      gslc_DrawLine(pGui,nX1+nXc,nY1+nYos,nX0+nXb,nY1+nYos,nCol);
    }

  #endif  // DRV_HAS_DRAW_TRI_FILL

  gslc_PageFlipSet(pGui,true);
}

void gslc_DrawFrameQuad(gslc_tsGui* pGui,gslc_tsPt* psPt,gslc_tsColor nCol)
{
  int16_t nX0,nY0,nX1,nY1;

  nX0 = psPt[0].x; nY0 = psPt[0].y; nX1 = psPt[1].x; nY1 = psPt[1].y;
  gslc_DrawLine(pGui,nX0,nY0,nX1,nY1,nCol);

  nX0 = psPt[1].x; nY0 = psPt[1].y; nX1 = psPt[2].x; nY1 = psPt[2].y;
  gslc_DrawLine(pGui,nX0,nY0,nX1,nY1,nCol);

  nX0 = psPt[2].x; nY0 = psPt[2].y; nX1 = psPt[3].x; nY1 = psPt[3].y;
  gslc_DrawLine(pGui,nX0,nY0,nX1,nY1,nCol);

  nX0 = psPt[3].x; nY0 = psPt[3].y; nX1 = psPt[0].x; nY1 = psPt[0].y;
  gslc_DrawLine(pGui,nX0,nY0,nX1,nY1,nCol);

}

// Filling a quadrilateral is done by breaking it down into
// two filled triangles sharing one side. We have to be careful
// about the triangle fill routine (ie. using rounding) so that
// we can avoid leaving a thin seam between the two triangles.
void gslc_DrawFillQuad(gslc_tsGui* pGui,gslc_tsPt* psPt,gslc_tsColor nCol)
{
  int16_t nX0,nY0,nX1,nY1,nX2,nY2;

  // Break down quadrilateral into two triangles
  nX0 = psPt[0].x; nY0 = psPt[0].y;
  nX1 = psPt[1].x; nY1 = psPt[1].y;
  nX2 = psPt[2].x; nY2 = psPt[2].y;
  gslc_DrawFillTriangle(pGui,nX0,nY0,nX1,nY1,nX2,nY2,nCol);

  nX0 = psPt[2].x; nY0 = psPt[2].y;
  nX1 = psPt[0].x; nY1 = psPt[0].y;
  nX2 = psPt[3].x; nY2 = psPt[3].y;
  gslc_DrawFillTriangle(pGui,nX0,nY0,nX1,nY1,nX2,nY2,nCol);

}


// -----------------------------------------------------------------------
// Font Functions
// -----------------------------------------------------------------------

bool gslc_FontAdd(gslc_tsGui* pGui,int16_t nFontId,gslc_teFontRefType eFontRefType,
    const void* pvFontRef,uint16_t nFontSz)
{
  if (pGui->nFontCnt+1 > (pGui->nFontMax)) {
    GSLC_DEBUG_PRINT("ERROR: FontAdd(%s) added too many fonts\n","");
    return false;
  } else {
    // Fetch a font resource from the driver
    const void* pvFont = gslc_DrvFontAdd(eFontRefType,pvFontRef,nFontSz);

    pGui->asFont[pGui->nFontCnt].eFontRefType = eFontRefType;
    pGui->asFont[pGui->nFontCnt].pvFont       = pvFont;
    pGui->asFont[pGui->nFontCnt].nId          = nFontId;
    pGui->asFont[pGui->nFontCnt].nSize        = nFontSz;
    pGui->nFontCnt++;
    return true;
  }
}


gslc_tsFont* gslc_FontGet(gslc_tsGui* pGui,int16_t nFontId)
{
  uint8_t  nFontInd;
  for (nFontInd=0;nFontInd<pGui->nFontCnt;nFontInd++) {
    if (pGui->asFont[nFontInd].nId == nFontId) {
      return &(pGui->asFont[nFontInd]);
    }
  }
  return NULL;
}




// ------------------------------------------------------------------------
// Page Functions
// ------------------------------------------------------------------------

// Common event handler
bool gslc_PageEvent(void* pvGui,gslc_tsEvent sEvent)
{
  if (pvGui == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "PageEvent";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return false;
  }
  //gslc_tsGui*       pGui        = (gslc_tsGui*)(pvGui);
  //void*             pvData      = sEvent.pvData;
  gslc_tsPage*        pPage       = (gslc_tsPage*)(sEvent.pvScope);
  gslc_tsCollect*     pCollect    = NULL;

  // Handle any page-level events first
  // ...

  // A Page only contains one Element Collection, so propagate

  // Handle the event types
  switch(sEvent.eType) {
    case GSLC_EVT_DRAW:
    case GSLC_EVT_TICK:
    case GSLC_EVT_TOUCH:
      pCollect  = &pPage->sCollect;
      // Update scope reference & propagate
      sEvent.pvScope = (void*)(pCollect);
      gslc_CollectEvent(pvGui,sEvent);
      break;

    default:
      break;
  } // sEvent.eType

  return true;
}


void gslc_PageAdd(gslc_tsGui* pGui,int16_t nPageId,gslc_tsElem* psElem,uint16_t nMaxElem,
        gslc_tsElemRef* psElemRef,uint16_t nMaxElemRef)
{
  if (pGui->nPageCnt+1 > (pGui->nPageMax)) {
    GSLC_DEBUG_PRINT("ERROR: PageAdd(%s) added too many pages\n","");
    return;
  }

  gslc_tsPage*  pPage = &pGui->asPage[pGui->nPageCnt];

  // TODO: Create proper PageReset()
  pPage->bPageNeedRedraw  = true;
  pPage->bPageNeedFlip    = false;
  pPage->pfuncXEvent      = NULL;

  // Initialize pPage->sCollect
  gslc_CollectReset(&pPage->sCollect,psElem,nMaxElem,psElemRef,nMaxElemRef);

  // Assign the requested Page ID
  pPage->nPageId = nPageId;

  // Increment the page count
  pGui->nPageCnt++;

  // Default the page pointer to the first page we create
  if (gslc_GetPageCur(pGui) == GSLC_PAGE_NONE) {
    gslc_SetPageCur(pGui,nPageId);
  }

  // Force the page to redraw
  gslc_PageRedrawSet(pGui,true);

}

int gslc_GetPageCur(gslc_tsGui* pGui)
{
  if (pGui->pCurPage == NULL) {
    return GSLC_PAGE_NONE;
  }
  return pGui->pCurPage->nPageId;
}


void gslc_SetPageCur(gslc_tsGui* pGui,int16_t nPageId)
{
  int16_t nPageSaved = GSLC_PAGE_NONE;
  if (pGui->pCurPage != NULL) {
    nPageSaved = pGui->pCurPage->nPageId;
  }

  // Find the page
  gslc_tsPage* pPage = gslc_PageFindById(pGui,nPageId);
  if (pPage == NULL) {
    GSLC_DEBUG_PRINT("ERROR: SetPageCur() can't find page (ID=%d)\n",nPageId);
    return;
  }

  // Save a reference to the selected page
  pGui->pCurPage = pPage;

  // Save a reference to the selected page's element collection
  pGui->pCurPageCollect = &pPage->sCollect;

  // A change of page should always force a future redraw
  if (nPageSaved != nPageId) {
    gslc_PageRedrawSet(pGui,true);
  }
}


// Adjust the flag that indicates whether the entire page
// requires a redraw.
void gslc_PageRedrawSet(gslc_tsGui* pGui,bool bRedraw)
{
  if (pGui->pCurPage == NULL) {
    return; // No page added yet
  }

  pGui->pCurPage->bPageNeedRedraw = bRedraw;
}

bool gslc_PageRedrawGet(gslc_tsGui* pGui)
{
  if (pGui->pCurPage == NULL) {
    return false; // No page added yet
  }

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
void gslc_PageRedrawCalc(gslc_tsGui* pGui)
{
  int               nInd;
  gslc_tsElem*      pElem = NULL;
  gslc_tsElemRef*   pElemRef = NULL;
  gslc_tsCollect*   pCollect = NULL;

  if (pGui->pCurPage == NULL) {
    return; // No page added yet
  }

  // Only work on current page
  pCollect = pGui->pCurPageCollect;

  for (nInd=0;nInd<pCollect->nElemRefCnt;nInd++) {
    pElemRef = &pCollect->asElemRef[nInd];
    gslc_teElemRefFlags eFlags = pElemRef->eElemFlags;
    pElem = gslc_GetElemFromRef(pGui,pElemRef);
    //GSLC_DEBUG_PRINT("PageRedrawCalc: Ind=%u ID=%u redraw=%u flags_old=%u fea=%u\n",nInd,pElem->nId,
    //        (eFlags & GSLC_ELEMREF_REDRAW_MASK),eFlags,pElem->nFeatures);
    if ((eFlags & GSLC_ELEMREF_REDRAW_MASK) != GSLC_ELEMREF_REDRAW_NONE) {

      // Determine if entire page requires redraw
      bool  bRedrawFullPage = false;

      // If partial redraw is supported, then we
      // look out for transparent elements which may
      // still warrant full page redraw.
      if (pGui->bRedrawPartialEn) {
        // Is the element transparent?
        if (!(pElem->nFeatures & GSLC_ELEM_FEA_FILL_EN)) {
          bRedrawFullPage = true;
        }
      } else {
        bRedrawFullPage = true;
      }

      if (bRedrawFullPage) {
        // Mark the entire page as requiring redraw
        gslc_PageRedrawSet(pGui,true);
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
void gslc_PageRedrawGo(gslc_tsGui* pGui)
{
  if (pGui->pCurPage == NULL) {
    return; // No page added yet
  }

  // Update any page redraw status that may be required
  // - Note that this routine handles cases where an element
  //   marked as requiring update is semi-transparent which can
  //   cause other elements to be redrawn as well.
  gslc_PageRedrawCalc(pGui);

  // Determine final state of full-page redraw
  bool  bPageRedraw = gslc_PageRedrawGet(pGui);

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
    gslc_DrvDrawBkgnd(pGui);
    gslc_PageFlipSet(pGui,true);
  }

  // Draw other elements (as needed, unless forced page redraw)
  // TODO: Handle GSLC_EVTSUB_DRAW_NEEDED
  uint32_t nSubType = (bPageRedraw)?GSLC_EVTSUB_DRAW_FORCE:GSLC_EVTSUB_DRAW_NEEDED;
  void* pvData = (void*)(pGui->pCurPage);
  gslc_tsEvent sEvent = gslc_EventCreate(pGui,GSLC_EVT_DRAW,nSubType,pvData,NULL);
  gslc_PageEvent(pGui,sEvent);


  // Clear the page redraw flag
  gslc_PageRedrawSet(pGui,false);

  // Page flip the entire screen
  // - TODO: We could also call Update instead of Flip as that would
  //         limit the region to refresh.
  gslc_PageFlipGo(pGui);

}


void gslc_PageFlipSet(gslc_tsGui* pGui,bool bNeeded)
{
  if (pGui->pCurPage == NULL) {
    return; // No page added yet
  }

  pGui->pCurPage->bPageNeedFlip = bNeeded;

  // To assist in debug of drawing primitives, support immediate
  // rendering of the current display. Note that this only works
  // for display drivers that don't invalidate the double-buffer (eg. SDL1.2)
#ifdef DBG_DRAW_IMM
  gslc_DrvPageFlipNow(pGui);
#endif
}

bool gslc_PageFlipGet(gslc_tsGui* pGui)
{
  if (pGui->pCurPage == NULL) {
    return false; // No page added yet
  }

  return pGui->pCurPage->bPageNeedFlip;
}

void gslc_PageFlipGo(gslc_tsGui* pGui)
{
  if (pGui->pCurPage == NULL) {
    return; // No page added yet
  }

  if (pGui->pCurPage->bPageNeedFlip) {
    gslc_DrvPageFlipNow(pGui);

    // Indicate that page flip is no longer required
    gslc_PageFlipSet(pGui,false);
  }
}


gslc_tsPage* gslc_PageFindById(gslc_tsGui* pGui,int16_t nPageId)
{
  int8_t nInd;

  // Loop through list of pages
  // Return pointer to page
  gslc_tsPage*  pFoundPage = NULL;
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
    GSLC_DEBUG_PRINT("ERROR: PageGet() can't find page (ID=%d)\n",nPageId);
    return NULL;
  }

  return pFoundPage;
}

gslc_tsElemRef* gslc_PageFindElemById(gslc_tsGui* pGui,int16_t nPageId,int16_t nElemId)
{
  gslc_tsPage*    pPage = NULL;
  gslc_tsElemRef* pElemRef = NULL;

  // Get the page
  pPage = gslc_PageFindById(pGui,nPageId);
  if (pPage == NULL) {
    GSLC_DEBUG_PRINT("ERROR: PageFindElemById() can't find page (ID=%d)\n",nPageId);
    return NULL;
  }
  // Find the element reference in the page's element collection
  pElemRef = gslc_CollectFindElemById(pGui,&pPage->sCollect,nElemId);
  return pElemRef;
}

void gslc_PageSetEventFunc(gslc_tsGui* pGui,gslc_tsPage* pPage,GSLC_CB_EVENT funcCb)
{
  if ((pPage == NULL) || (funcCb == NULL)) {
    static const char GSLC_PMEM FUNCSTR[] = "PageSetEventFunc";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return;
  }
  pPage->pfuncXEvent       = funcCb;
}

// ------------------------------------------------------------------------
// Element General Functions
// ------------------------------------------------------------------------


int gslc_ElemGetId(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef)
{
  gslc_tsElem*  pElem = gslc_GetElemFromRef(pGui,pElemRef);
  return pElem->nId;
}


// Get a flag from an element reference
// Note that the flag remains aligned to the position within the flags bitfield
uint8_t gslc_GetElemRefFlag(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,uint8_t nFlagMask)
{
  if (!pElemRef) {
    static const char GSLC_PMEM FUNCSTR[] = "GetElemRefFlag";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return 0;
  }
  return pElemRef->eElemFlags & nFlagMask;
}

// Set a flag in an element reference
// Note that the nFlagVal must be aligned to the nFlagMask
void gslc_SetElemRefFlag(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,uint8_t nFlagMask,uint8_t nFlagVal)
{
  if (!pElemRef) {
    static const char GSLC_PMEM FUNCSTR[] = "SetElemRefFlag";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return;
  }
  uint8_t nCurFlags = pElemRef->eElemFlags;
  uint8_t nNewFlags = (nCurFlags & ~nFlagMask) | nFlagVal;
  pElemRef->eElemFlags = nNewFlags;
}

// Returns a pointer to an element from an element reference
// - Handle caching from FLASH if element is accessed via PROGMEM
gslc_tsElem* gslc_GetElemFromRef(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef)
{
  if (!pElemRef) {
    static const char GSLC_PMEM FUNCSTR[] = "GetElemFromRef";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return NULL;
  }
  gslc_teElemRefFlags eFlags  = pElemRef->eElemFlags;
  gslc_tsElem*        pElem   = pElemRef->pElem;

  // If the element is in FLASH and requires PROGMEM to access
  // then cache it locally and return a pointer to the global
  // temporary element instead so that further accesses can
  // be direct.
  if ((eFlags & GSLC_ELEMREF_SRC) == GSLC_ELEMREF_SRC_PROG) {
    // TODO: Optimize by checking to see if we already cached this element
    // - Can probably do this by comparing the bValid and nId
    #if (GSLC_USE_PROGMEM)
    memcpy_P(&pGui->sElemTmpProg,pElem,sizeof(gslc_tsElem));
    pElem = &pGui->sElemTmpProg;
    #endif
  } else if ((eFlags & GSLC_ELEMREF_SRC) == GSLC_ELEMREF_SRC_CONST) {
    // We are running on device that may support FLASH storage
    // simply by using a const declaration. No special
    // PROGMEM access mechanism is required, therefore it is
    // not necessary to copy to the temporary RAM element.
    // An example device using this mode is Cortex-M0.
    // No explicit copy from FLASH is required.
  } else {
    // SRC_ELEMREF_SRC_RAM
    // No copy required
  }
  return pElem;
}


// ------------------------------------------------------------------------
// Element Creation Functions
// ------------------------------------------------------------------------

gslc_tsElemRef* gslc_ElemCreateTxt(gslc_tsGui* pGui,int16_t nElemId,int16_t nPage,gslc_tsRect rElem,
  char* pStrBuf,uint8_t nStrBufMax,int16_t nFontId)
{
  gslc_tsElem       sElem;
  gslc_tsElemRef*   pElemRef = NULL;
  sElem = gslc_ElemCreate(pGui,nElemId,nPage,GSLC_TYPE_TXT,rElem,pStrBuf,nStrBufMax,nFontId);
  sElem.colElemFill       = GSLC_COL_BLACK;
  sElem.colElemFillGlow   = GSLC_COL_BLACK;
  sElem.colElemFrame      = GSLC_COL_BLACK;
  sElem.colElemFrameGlow  = GSLC_COL_BLACK;
  sElem.colElemText       = GSLC_COL_YELLOW;
  sElem.colElemTextGlow   = GSLC_COL_YELLOW;
  sElem.nFeatures        |= GSLC_ELEM_FEA_FILL_EN;
  sElem.eTxtAlign         = GSLC_ALIGN_MID_LEFT;
  if (nPage != GSLC_PAGE_NONE) {
    pElemRef = gslc_ElemAdd(pGui,nPage,&sElem,GSLC_ELEMREF_SRC_RAM);
    return pElemRef;
#if (GSLC_FEATURE_COMPOUND)
  } else {
    // Save as temporary element
    pGui->sElemTmp = sElem;
    pGui->sElemRefTmp.pElem = &(pGui->sElemTmp);
    pGui->sElemRefTmp.eElemFlags = GSLC_ELEMREF_SRC_RAM | GSLC_ELEMREF_REDRAW_FULL;
    return &(pGui->sElemRefTmp);
#endif
  }
  return NULL;
}

gslc_tsElemRef* gslc_ElemCreateBtnTxt(gslc_tsGui* pGui,int16_t nElemId,int16_t nPage,
  gslc_tsRect rElem,char* pStrBuf,uint8_t nStrBufMax,int16_t nFontId,GSLC_CB_TOUCH cbTouch)
{
  gslc_tsElem     sElem;
  gslc_tsElemRef* pElemRef = NULL;

  // Ensure the Font has been defined
  if (gslc_FontGet(pGui,nFontId) == NULL) {
    GSLC_DEBUG_PRINT("ERROR: ElemCreateBtnTxt(ID=%d): Font(ID=%d) not loaded\n",nElemId,nFontId);
    return NULL;
  }

  sElem = gslc_ElemCreate(pGui,nElemId,nPage,GSLC_TYPE_BTN,rElem,pStrBuf,nStrBufMax,nFontId);
  sElem.colElemFill       = GSLC_COL_BLUE_DK4;
  sElem.colElemFillGlow   = GSLC_COL_BLUE_DK1;
  sElem.colElemFrame      = GSLC_COL_BLUE_DK2;
  sElem.colElemFrameGlow  = GSLC_COL_BLUE_DK2;
  sElem.colElemText       = GSLC_COL_WHITE;
  sElem.colElemTextGlow   = GSLC_COL_WHITE;
  sElem.nFeatures        |= GSLC_ELEM_FEA_FRAME_EN;
  sElem.nFeatures        |= GSLC_ELEM_FEA_FILL_EN;
  sElem.nFeatures        |= GSLC_ELEM_FEA_CLICK_EN;
  sElem.nFeatures        |= GSLC_ELEM_FEA_GLOW_EN;
  sElem.pfuncXTouch       = cbTouch;
  if (nPage != GSLC_PAGE_NONE) {
    pElemRef = gslc_ElemAdd(pGui,nPage,&sElem,GSLC_ELEMREF_SRC_RAM);
    return pElemRef;
#if (GSLC_FEATURE_COMPOUND)
  } else {
    // Save as temporary element
    pGui->sElemTmp = sElem;
    pGui->sElemRefTmp.pElem = &(pGui->sElemTmp);
    pGui->sElemRefTmp.eElemFlags = GSLC_ELEMREF_SRC_RAM | GSLC_ELEMREF_REDRAW_FULL;
    return &(pGui->sElemRefTmp);
#endif
  }
  return NULL;
}

gslc_tsElemRef* gslc_ElemCreateBtnImg(gslc_tsGui* pGui,int16_t nElemId,int16_t nPage,
  gslc_tsRect rElem,gslc_tsImgRef sImgRef,gslc_tsImgRef sImgRefSel,GSLC_CB_TOUCH cbTouch)
{
  gslc_tsElem     sElem;
  gslc_tsElemRef* pElemRef = NULL;
  sElem = gslc_ElemCreate(pGui,nElemId,nPage,GSLC_TYPE_BTN,rElem,NULL,0,GSLC_FONT_NONE);
  sElem.colElemFill       = GSLC_COL_BLACK;
  sElem.colElemFillGlow   = GSLC_COL_BLACK;
  sElem.colElemFrame      = GSLC_COL_BLUE_DK2;
  sElem.colElemFrameGlow  = GSLC_COL_BLUE_DK2;
  sElem.nFeatures        &= ~GSLC_ELEM_FEA_FRAME_EN;
  sElem.nFeatures        &= ~GSLC_ELEM_FEA_FILL_EN;
  sElem.nFeatures        |= GSLC_ELEM_FEA_CLICK_EN;
  sElem.nFeatures        |= GSLC_ELEM_FEA_GLOW_EN;
  sElem.pfuncXTouch       = cbTouch;
  // Update the normal and glowing images
  gslc_DrvSetElemImageNorm(pGui,&sElem,sImgRef);
  gslc_DrvSetElemImageGlow(pGui,&sElem,sImgRefSel);
  if (nPage != GSLC_PAGE_NONE) {
    pElemRef = gslc_ElemAdd(pGui,nPage,&sElem,GSLC_ELEMREF_SRC_RAM);
    return pElemRef;
#if (GSLC_FEATURE_COMPOUND)
  } else {
    // Save as temporary element
    pGui->sElemTmp = sElem;
    pGui->sElemRefTmp.pElem = &(pGui->sElemTmp);
    pGui->sElemRefTmp.eElemFlags = GSLC_ELEMREF_SRC_RAM | GSLC_ELEMREF_REDRAW_FULL;
    return &(pGui->sElemRefTmp);
#endif
  }
  return NULL;
}


gslc_tsElemRef* gslc_ElemCreateBox(gslc_tsGui* pGui,int16_t nElemId,int16_t nPage,gslc_tsRect rElem)
{
  gslc_tsElem     sElem;
  gslc_tsElemRef* pElemRef = NULL;
  sElem = gslc_ElemCreate(pGui,nElemId,nPage,GSLC_TYPE_BOX,rElem,NULL,0,GSLC_FONT_NONE);
  sElem.colElemFill       = GSLC_COL_BLACK;
  sElem.colElemFillGlow   = GSLC_COL_BLACK;
  sElem.colElemFrame      = GSLC_COL_GRAY;
  sElem.colElemFrameGlow  = GSLC_COL_GRAY;
  sElem.nFeatures        |= GSLC_ELEM_FEA_FILL_EN;
  sElem.nFeatures        |= GSLC_ELEM_FEA_FRAME_EN;
  if (nPage != GSLC_PAGE_NONE) {
    pElemRef = gslc_ElemAdd(pGui,nPage,&sElem,GSLC_ELEMREF_SRC_RAM);
    return pElemRef;
#if (GSLC_FEATURE_COMPOUND)
  } else {
    // Save as temporary element
    pGui->sElemTmp = sElem;
    pGui->sElemRefTmp.pElem = &(pGui->sElemTmp);
    pGui->sElemRefTmp.eElemFlags = GSLC_ELEMREF_SRC_RAM | GSLC_ELEMREF_REDRAW_FULL;
    return &(pGui->sElemRefTmp);
#endif
  }
  return NULL;
}

gslc_tsElemRef* gslc_ElemCreateLine(gslc_tsGui* pGui,int16_t nElemId,int16_t nPage,int16_t nX0,int16_t nY0,int16_t nX1,int16_t nY1)
{
  gslc_tsElem     sElem;
  gslc_tsElemRef* pElemRef = NULL;
  gslc_tsRect     rRect;
  rRect.x = nX0;
  rRect.y = nY0;
  rRect.w = nX1 - nX0 + 1;
  rRect.h = nY1 - nY0 + 1;
  sElem = gslc_ElemCreate(pGui,nElemId,nPage,GSLC_TYPE_LINE,rRect,NULL,0,GSLC_FONT_NONE);
  // For line elements, we will draw it with the "fill" color
  sElem.colElemFill       = GSLC_COL_GRAY;
  sElem.colElemFillGlow   = GSLC_COL_GRAY;
  sElem.nFeatures        &= ~GSLC_ELEM_FEA_FILL_EN;   // Disable boundary box fill
  sElem.nFeatures        &= ~GSLC_ELEM_FEA_FRAME_EN;  // Disable boundary box frame
  if (nPage != GSLC_PAGE_NONE) {
    pElemRef = gslc_ElemAdd(pGui,nPage,&sElem,GSLC_ELEMREF_SRC_RAM);
    return pElemRef;
#if (GSLC_FEATURE_COMPOUND)
  } else {
    // Save as temporary element
    pGui->sElemTmp = sElem;
    pGui->sElemRefTmp.pElem = &(pGui->sElemTmp);
    pGui->sElemRefTmp.eElemFlags = GSLC_ELEMREF_SRC_RAM | GSLC_ELEMREF_REDRAW_FULL;
    return &(pGui->sElemRefTmp);
#endif
  }
  return NULL;
}

gslc_tsElemRef* gslc_ElemCreateImg(gslc_tsGui* pGui,int16_t nElemId,int16_t nPage,
  gslc_tsRect rElem,gslc_tsImgRef sImgRef)
{
  gslc_tsElem     sElem;
  gslc_tsElemRef* pElemRef = NULL;
  sElem = gslc_ElemCreate(pGui,nElemId,nPage,GSLC_TYPE_BOX,rElem,NULL,0,GSLC_FONT_NONE);
  sElem.nFeatures      &= ~GSLC_ELEM_FEA_FRAME_EN;
  sElem.nFeatures      &= ~GSLC_ELEM_FEA_FILL_EN;
  sElem.nFeatures      &= ~GSLC_ELEM_FEA_CLICK_EN;
  // Update the normal and glowing images
  gslc_DrvSetElemImageNorm(pGui,&sElem,sImgRef);
  gslc_DrvSetElemImageGlow(pGui,&sElem,sImgRef);

  if (nPage != GSLC_PAGE_NONE) {
    pElemRef = gslc_ElemAdd(pGui,nPage,&sElem,GSLC_ELEMREF_SRC_RAM);
    return pElemRef;
#if (GSLC_FEATURE_COMPOUND)
  } else {
    // Save as temporary element
    pGui->sElemTmp = sElem;
    pGui->sElemRefTmp.pElem = &(pGui->sElemTmp);
    pGui->sElemRefTmp.eElemFlags = GSLC_ELEMREF_SRC_RAM | GSLC_ELEMREF_REDRAW_FULL;
    return &(pGui->sElemRefTmp);
#endif
  }
  return NULL;
}


// ------------------------------------------------------------------------
// Element Event Handlers
// ------------------------------------------------------------------------

// Common event handler
bool gslc_ElemEvent(void* pvGui,gslc_tsEvent sEvent)
{
  if (pvGui == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemEvent";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return false;
  }
  gslc_tsGui*         pGui              = (gslc_tsGui*)(pvGui);
  void*               pvData            = sEvent.pvData;
  void*               pvScope           = sEvent.pvScope;
  gslc_tsElemRef*     pElemRef          = NULL;
  gslc_tsElemRef*     pElemRefTracked   = NULL;
  gslc_tsElem*        pElem             = NULL;
  gslc_tsElem*        pElemTracked      = NULL;
  gslc_tsEventTouch*  pTouchRec         = NULL;
  int                 nRelX,nRelY;
  gslc_teTouch        eTouch;
  GSLC_CB_TOUCH       pfuncXTouch       = NULL;
  GSLC_CB_TICK        pfuncXTick        = NULL;

  switch(sEvent.eType) {
    case GSLC_EVT_DRAW:
      // Fetch the parameters
      pElemRef = (gslc_tsElemRef*)(pvScope);

      // Determine if redraw is needed
      gslc_teRedrawType eRedraw = gslc_ElemGetRedraw(pGui,pElemRef);

      if (sEvent.nSubType == GSLC_EVTSUB_DRAW_FORCE) {
        return gslc_ElemDrawByRef(pGui,pElemRef,GSLC_REDRAW_FULL);
      } else if (eRedraw != GSLC_REDRAW_NONE) {
        return gslc_ElemDrawByRef(pGui,pElemRef,eRedraw);
      } else {
        // No redraw needed
        return true;
      }
      break;

    case GSLC_EVT_TOUCH:
      #if defined(DRV_TOUCH_NONE)
      // Hide warnings about unused variables when TOUCH is not enabled
      (void)pvData;
      (void)pElemRefTracked;
      (void)pElemTracked;
      (void)pTouchRec;
      (void)nRelX;
      (void)nRelY;
      (void)eTouch;
      (void)pfuncXTouch;
      #else
      // Fetch the parameters
      pElemRef = (gslc_tsElemRef*)(pvScope);
      pTouchRec = (gslc_tsEventTouch*)(pvData);
      pElemRefTracked = pElemRef;
      pElemTracked = gslc_GetElemFromRef(pGui,pElemRefTracked);
      nRelX = pTouchRec->nX - pElemTracked->rElem.x;
      nRelY = pTouchRec->nY - pElemTracked->rElem.y;
      eTouch = pTouchRec->eTouch;

      // Since we are going to use the callback within the element
      // we need to ensure it is cached in RAM first
      pElemTracked = gslc_GetElemFromRef(pGui,pElemRefTracked);
      pfuncXTouch = pElemTracked->pfuncXTouch;

      // Invoke the callback function
      if (pfuncXTouch != NULL) {
        // Pass in the relative position from corner of element region
        (*pfuncXTouch)(pvGui,(void*)(pElemRefTracked),eTouch,nRelX,nRelY);
      }
      #endif // DRV_TOUCH_NONE
      break;

    case GSLC_EVT_TICK:
      // Fetch the parameters
      pElemRef = (gslc_tsElemRef*)(pvScope);

      // Since we are going to use the callback within the element
      // we need to ensure it is cached in RAM first
      pElem = gslc_GetElemFromRef(pGui,pElemRef);
      pfuncXTick = pElem->pfuncXTick;

      // Invoke the callback function
      if (pfuncXTick != NULL) {
        // TODO: Confirm that tick functions want pvScope
        (*pfuncXTick)(pvGui,(void*)(pElemRef));
        return true;
      }
      break;

    default:
      break;
  }
  return true;
}

// ------------------------------------------------------------------------
// Element Drawing Functions
// ------------------------------------------------------------------------


// Draw an element to the active display
// - Element is referenced by page ID and element ID
// - This routine is typically called by user code for custom
//   drawing callbacks
void gslc_ElemDraw(gslc_tsGui* pGui,int16_t nPageId,int16_t nElemId)
{
  gslc_tsElemRef* pElemRef = gslc_PageFindElemById(pGui,nPageId,nElemId);
  gslc_tsEvent sEvent = gslc_EventCreate(pGui,GSLC_EVT_DRAW,GSLC_EVTSUB_DRAW_FORCE,(void*)pElemRef,NULL);
  gslc_ElemEvent(pGui,sEvent);
}

// Draw an element to the active display
// - Element is referenced by an element pointer
// - TODO: Handle GSLC_TYPE_BKGND
bool gslc_ElemDrawByRef(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,gslc_teRedrawType eRedraw)
{
  if (eRedraw == GSLC_REDRAW_NONE) {
    // No redraw to do
    return true;
  }

  if ((pGui == NULL) || (pElemRef == NULL)) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemDrawByRef";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return false;
  }

  gslc_tsElem*  pElem = gslc_GetElemFromRef(pGui,pElemRef);

  // --------------------------------------------------------------------------
  // Custom drawing
  // --------------------------------------------------------------------------

  // Handle any extended element types
  // - If the pfuncXDraw callback is defined, then let the callback
  //   function supersede all default handling here
  // - Note that the end of the callback function is expected
  //   to clear the redraw flag
  if (pElem->pfuncXDraw != NULL) {
    (*pElem->pfuncXDraw)((void*)(pGui),(void*)(pElemRef),eRedraw);
    return true;
  }

  // --------------------------------------------------------------------------
  // Init for default drawing
  // --------------------------------------------------------------------------

  bool      bGlowEn,bGlowing,bGlowNow;
  int16_t   nElemX,nElemY;
  uint16_t  nElemW,nElemH;

  nElemX    = pElem->rElem.x;
  nElemY    = pElem->rElem.y;
  nElemW    = pElem->rElem.w;
  nElemH    = pElem->rElem.h;
  bGlowEn   = pElem->nFeatures & GSLC_ELEM_FEA_GLOW_EN; // Does the element support glow state?
  bGlowing  = gslc_ElemGetGlow(pGui,pElemRef); // Element should be glowing (if enabled)
  bGlowNow  = bGlowEn & bGlowing; // Element is currently glowing


  // --------------------------------------------------------------------------
  // Background
  // --------------------------------------------------------------------------

  // Fill in the background
  gslc_tsRect rElemInner = pElem->rElem;
  // - If both fill and frame are enabled then contract
  //   the fill region slightly so that we don't overdraw
  //   the frame (prevent unnecessary flicker).
  if ((pElem->nFeatures & GSLC_ELEM_FEA_FILL_EN) && (pElem->nFeatures & GSLC_ELEM_FEA_FRAME_EN)) {
    // NOTE: If the region is already too small to shrink (eg. w=1 or h=1)
    // then a zero dimension box will be returned by ExpandRect() and not drawn
    rElemInner = gslc_ExpandRect(rElemInner,-1,-1);
  }
  // - This also changes the fill color if selected and glow state is enabled
  if (pElem->nFeatures & GSLC_ELEM_FEA_FILL_EN) {
    if (bGlowEn && bGlowing) {
      gslc_DrawFillRect(pGui,rElemInner,pElem->colElemFillGlow);
    } else {
      gslc_DrawFillRect(pGui,rElemInner,pElem->colElemFill);
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
  gslc_DrawFrameRect(pGui,pElem->rElem,GSLC_COL_GRAY_DK1);
  #else
  if (pElem->nFeatures & GSLC_ELEM_FEA_FRAME_EN) {
    gslc_DrawFrameRect(pGui,pElem->rElem,pElem->colElemFrame);
  }
  #endif


  // --------------------------------------------------------------------------
  // Handle special element types
  // --------------------------------------------------------------------------
  if (pElem->nType == GSLC_TYPE_LINE) {
    gslc_DrawLine(pGui,nElemX,nElemY,nElemX+nElemW-1,nElemY+nElemH-1,pElem->colElemFill);
  }


  // --------------------------------------------------------------------------
  // Image overlays
  // --------------------------------------------------------------------------

  // Draw any images associated with element
  if (pElem->sImgRefNorm.eImgFlags != GSLC_IMGREF_NONE) {
    if ((bGlowEn && bGlowing) && (pElem->sImgRefGlow.eImgFlags != GSLC_IMGREF_NONE)) {
      gslc_DrvDrawImage(pGui,nElemX,nElemY,pElem->sImgRefGlow);
    } else {
      gslc_DrvDrawImage(pGui,nElemX,nElemY,pElem->sImgRefNorm);
    }
  }

  // --------------------------------------------------------------------------
  // Text overlays
  // --------------------------------------------------------------------------

  // Overlay the text
  bool bRenderTxt = true;
  // Skip text render if buffer pointer not allocated
  if ((bRenderTxt) && (pElem->pStrBuf == NULL)) { bRenderTxt = false; }
  // Skip text render if string is not set
  if ((bRenderTxt) && ((pElem->eTxtFlags & GSLC_TXT_ALLOC) == GSLC_TXT_ALLOC_NONE)) { bRenderTxt = false; }

  // Do we still want to render?
  if (bRenderTxt) {
#if (DRV_HAS_DRAW_TEXT)
    int16_t       nMargin   = pElem->nTxtMargin;

    // Determine the text color
    gslc_tsColor  colTxt    = (bGlowNow)? pElem->colElemTextGlow : pElem->colElemText;


    // Determine if GUIslice or driver should perform text alignment
    // - Generally, GUIslice provides the text alignment functionality
    //   by querying the display driver for the dimensions of the text
    //   to be rendered.
    // - Some drivers (such as TFT_eSPI) perform more complex logic
    //   associated with the text alignment and hence GUIslice will
    //   defer to the driver to handle the positioning. In that case
    //   the bounding box and alignment mode is provided to the driver.
#if (DRV_OVERRIDE_TXT_ALIGN)

    // GUIslice will allow the driver to perform the text alignment
    // calculations.

    // Provide bounding box and alignment flag to driver to calculate
    int16_t nX0 = nElemX + nMargin;
    int16_t nY0 = nElemY + nMargin;
    int16_t nX1 = nX0 + nElemW - 2*nMargin;
    int16_t nY1 = nY0 + nElemH - 2*nMargin;

    gslc_DrvDrawTxtAlign(pGui,nX0,nY0,nX1,nY1,pElem->eTxtAlign,pElem->pTxtFont,
            pElem->pStrBuf,pElem->eTxtFlags,colTxt);

#else // DRV_OVERRIDE_TXT_ALIGN

    // GUIslice will ask the driver for the text dimensions and calculate
    // the appropriate positioning to support the requested text
    // alignment mode.

    // Fetch the size of the text to allow for justification
    // NOTE: For multi-line text strings, the following call will
    //       return the maximum dimensions of the entire block of
    //       text, thus alignment will be based on the outer dimensions
    //       not individual rows of text. As a result, the overall
    //       text block will be rendered with the requested alignment
    //       but individual rows will render like GSLC_ALIGNH_LEFT
    //       within the aligned text block. In order to support per-line
    //       horizontal justification, a pre-scan and alignment calculation
    //       for each text row would need to be performed.
    int16_t       nTxtOffsetX,nTxtOffsetY;
    uint16_t      nTxtSzW,nTxtSzH;
    gslc_DrvGetTxtSize(pGui,pElem->pTxtFont,pElem->pStrBuf,pElem->eTxtFlags,&nTxtOffsetX,&nTxtOffsetY,&nTxtSzW,&nTxtSzH);

    // Calculate the text alignment
    int16_t       nTxtX,nTxtY;

    // Check for ALIGNH_LEFT & ALIGNH_RIGHT. Default to ALIGNH_MID
    if      (pElem->eTxtAlign & GSLC_ALIGNH_LEFT)     { nTxtX = nElemX+nMargin; }
    else if (pElem->eTxtAlign & GSLC_ALIGNH_RIGHT)    { nTxtX = nElemX+nElemW-nMargin-nTxtSzW; }
    else                                              { nTxtX = nElemX+(nElemW/2)-(nTxtSzW/2); }

    // Check for ALIGNV_TOP & ALIGNV_BOT. Default to ALIGNV_MID
    if      (pElem->eTxtAlign & GSLC_ALIGNV_TOP)      { nTxtY = nElemY+nMargin; }
    else if (pElem->eTxtAlign & GSLC_ALIGNV_BOT)      { nTxtY = nElemY+nElemH-nMargin-nTxtSzH; }
    else                                              { nTxtY = nElemY+(nElemH/2)-(nTxtSzH/2); }

    // Now correct for offset from text bounds
    // - This is used by the driver (such as Adafruit-GFX) to provide an
    //   adjustment for baseline height, etc.
    nTxtX += nTxtOffsetX;
    nTxtY -= nTxtOffsetY;

    // Call the driver text rendering routine
    gslc_DrvDrawTxt(pGui,nTxtX,nTxtY,pElem->pTxtFont,pElem->pStrBuf,pElem->eTxtFlags,colTxt);

#endif // DRV_OVERRIDE_TXT_ALIGN

#else // DRV_HAS_DRAW_TEXT
    // No text support in driver, so skip
#endif // DRV_HAS_DRAW_TEXT
  }

  // Mark the element as no longer requiring redraw
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_NONE);

  return true;
}


// ------------------------------------------------------------------------
// Element Update Functions
// ------------------------------------------------------------------------

void gslc_ElemSetFillEn(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,bool bFillEn)
{
  if (pElemRef == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemSetFillEn";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return;
  }
  gslc_tsElem*  pElem = gslc_GetElemFromRef(pGui,pElemRef);
  if (bFillEn) {
    pElem->nFeatures |= GSLC_ELEM_FEA_FILL_EN;
  } else {
    pElem->nFeatures &= ~GSLC_ELEM_FEA_FILL_EN;
  }
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_FULL);
}


void gslc_ElemSetFrameEn(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,bool bFrameEn)
{
  if (pElemRef == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemSetFrameEn";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return;
  }
  gslc_tsElem*  pElem = gslc_GetElemFromRef(pGui,pElemRef);
  if (bFrameEn) {
    pElem->nFeatures |= GSLC_ELEM_FEA_FRAME_EN;
  } else {
    pElem->nFeatures &= ~GSLC_ELEM_FEA_FRAME_EN;
  }
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_FULL);
}


void gslc_ElemSetCol(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,gslc_tsColor colFrame,gslc_tsColor colFill,gslc_tsColor colFillGlow)
{
  if (pElemRef == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemSetCol";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return;
  }
  gslc_tsElem*  pElem = gslc_GetElemFromRef(pGui,pElemRef);
  pElem->colElemFrame     = colFrame;
  pElem->colElemFill      = colFill;
  pElem->colElemFillGlow  = colFillGlow;
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_FULL);
}

void gslc_ElemSetGlowCol(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,gslc_tsColor colFrameGlow,gslc_tsColor colFillGlow,gslc_tsColor colTxtGlow)
{
  if (pElemRef == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemSetGlowCol";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return;
  }
  gslc_tsElem*  pElem = gslc_GetElemFromRef(pGui,pElemRef);
  pElem->colElemFrameGlow   = colFrameGlow;
  pElem->colElemFillGlow    = colFillGlow;
  pElem->colElemTextGlow    = colTxtGlow;
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_FULL);
}

void gslc_ElemSetGroup(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,int nGroupId)
{
  if (pElemRef == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemSetGroup";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return;
  }
  gslc_tsElem*  pElem = gslc_GetElemFromRef(pGui,pElemRef);
  pElem->nGroup           = nGroupId;
}

int gslc_ElemGetGroup(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef)
{
  if (pElemRef == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemGetGroup";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return GSLC_GROUP_ID_NONE;
  }
  gslc_tsElem*  pElem = gslc_GetElemFromRef(pGui,pElemRef);
  return pElem->nGroup;
}


void gslc_ElemSetTxtAlign(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,unsigned nAlign)
{
  if (pElemRef == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemSetTxtAlign";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return;
  }
  gslc_tsElem*  pElem = gslc_GetElemFromRef(pGui,pElemRef);
  pElem->eTxtAlign        = nAlign;
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_FULL);
}

void gslc_ElemSetTxtMargin(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,unsigned nMargin)
{
  if (pElemRef == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemSetTxtMargin";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return;
  }
  gslc_tsElem*  pElem = gslc_GetElemFromRef(pGui,pElemRef);
  pElem->nTxtMargin        = nMargin;
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_FULL);
}

void gslc_ElemSetTxtStr(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,const char* pStr)
{
  if (pElemRef == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemSetTxtStr";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return;
  }
  gslc_tsElem*  pElem = gslc_GetElemFromRef(pGui,pElemRef);

  // Check for read-only status (in case the string was
  // defined in Flash/PROGMEM)
  if (pElem->nStrBufMax == 0) {
    // String was read-only, so abort now
    return;
  }

  // To avoid unnecessary redraw / flicker, only a change in
  // the text content will drive a redraw

  if (strncmp(pElem->pStrBuf,pStr,pElem->nStrBufMax-1)) {
    strncpy(pElem->pStrBuf,pStr,pElem->nStrBufMax-1);
    pElem->pStrBuf[pElem->nStrBufMax-1] = '\0';  // Force termination
    gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_FULL);
  }
}

void gslc_ElemSetTxtCol(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,gslc_tsColor colVal)
{
  if (pElemRef == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemSetTxtCol";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return;
  }
  gslc_tsElem*  pElem = gslc_GetElemFromRef(pGui,pElemRef);
  if (!gslc_ColorEqual(pElem->colElemText, colVal) ||
      !gslc_ColorEqual(pElem->colElemTextGlow, colVal)) {
    pElem->colElemText      = colVal;
    pElem->colElemTextGlow  = colVal; // Default to same color for glowing state
    gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_FULL);
  }
}

void gslc_ElemSetTxtMem(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,gslc_teTxtFlags eFlags)
{
  if (pElemRef == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemSetTxtMem";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return;
  }
  gslc_tsElem*  pElem = gslc_GetElemFromRef(pGui,pElemRef);
  if (GSLC_LOCAL_STR) {
    if ((eFlags & GSLC_TXT_MEM) == GSLC_TXT_MEM_PROG) {
      // ERROR: Unsupported mode
      // - We don't support internal buffer mode with initialization
      //   from flash (PROGMEM)
      GSLC_DEBUG_PRINT("ERROR: ElemSetTxtMem(%s) GSLC_LOCAL_STR can't be used with GSLC_TXT_MEM_PROG\n","");
      return;
    }
  }
  gslc_teTxtFlags eFlagsCur = pElem->eTxtFlags;
  pElem->eTxtFlags = (eFlagsCur & ~GSLC_TXT_MEM) | (eFlags & GSLC_TXT_MEM);
}

void gslc_ElemSetTxtEnc(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,gslc_teTxtFlags eFlags)
{
  if (pElemRef == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemSetTxtEnc";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return;
  }
  gslc_tsElem*  pElem = gslc_GetElemFromRef(pGui,pElemRef);

  gslc_teTxtFlags eFlagsCur = pElem->eTxtFlags;
  pElem->eTxtFlags = (eFlagsCur & ~GSLC_TXT_ENC) | (eFlags & GSLC_TXT_ENC);
}

void gslc_ElemUpdateFont(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,int nFontId)
{
  if ((pGui == NULL) || (pElemRef == NULL)) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemUpdateFont";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return;
  }
  gslc_tsElem*  pElem = gslc_GetElemFromRef(pGui,pElemRef);
  pElem->pTxtFont = gslc_FontGet(pGui,nFontId);
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_FULL);
}


void gslc_ElemSetRedraw(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,gslc_teRedrawType eRedraw)
{
  if (pElemRef == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemSetRedraw";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return;
  }

  // Update the redraw flag
  gslc_teElemRefFlags eFlags = pElemRef->eElemFlags;
  switch (eRedraw) {
    case GSLC_REDRAW_NONE:
      eFlags = (eFlags & ~GSLC_ELEMREF_REDRAW_MASK) | GSLC_ELEMREF_REDRAW_NONE;
      break;
    case GSLC_REDRAW_FULL:
      eFlags = (eFlags & ~GSLC_ELEMREF_REDRAW_MASK) | GSLC_ELEMREF_REDRAW_FULL;
      break;
    case GSLC_REDRAW_INC:
      eFlags = (eFlags & ~GSLC_ELEMREF_REDRAW_MASK) | GSLC_ELEMREF_REDRAW_INC;
      break;
  }

  // Handle request for update of redraw state
  // - We permit the new state to be assigned except in the
  //   case wherein we are currently pending a full redraw
  //   but an incremental redraw has been requested. In that
  //   case we leave the full redraw request pending.
  if (eRedraw == GSLC_REDRAW_INC) {
    if ((eFlags & GSLC_ELEMREF_REDRAW_MASK) == GSLC_ELEMREF_REDRAW_FULL) {
      // Don't update redraw state; leave it as full redraw pending
    } else {
      pElemRef->eElemFlags = eFlags;
    }
  } else {
    pElemRef->eElemFlags = eFlags;
  }

#if (GSLC_FEATURE_COMPOUND)
  // Now propagate up the element hierarchy
  // (eg. in case of compound elements)

  // TODO: Perhaps we need to qualify this with bRedraw=true?
  // - ie. we only want to invalidate the parent element
  //   containers, but we don't want to reset their status

  // - This functionality is only used in the case of compound
  //   elements (eg. XSelNum).
  gslc_tsElem*  pElem = gslc_GetElemFromRef(pGui,pElemRef);
  if (pElem->pElemRefParent != NULL) {
    gslc_ElemSetRedraw(pGui,pElem->pElemRefParent,eRedraw);
  }
#endif
}

gslc_teRedrawType gslc_ElemGetRedraw(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef)
{
  if (pElemRef == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemGetRedraw";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return GSLC_REDRAW_NONE;
  }
  gslc_teElemRefFlags eFlags = pElemRef->eElemFlags;
  switch (eFlags & GSLC_ELEMREF_REDRAW_MASK) {
    case GSLC_ELEMREF_REDRAW_NONE:
      return GSLC_REDRAW_NONE;
    case GSLC_ELEMREF_REDRAW_FULL:
      return GSLC_REDRAW_FULL;
    case GSLC_ELEMREF_REDRAW_INC:
      return GSLC_REDRAW_INC;
  }
  // Should not reach here
  return GSLC_REDRAW_NONE;
}


void gslc_ElemSetGlow(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,bool bGlowing)
{
  if (pElemRef == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemSetGlow";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return;
  }
  // FIXME: Should also check for change in bGlowEn
  bool bGlowingOld = gslc_ElemGetGlow(pGui,pElemRef);
  gslc_SetElemRefFlag(pGui,pElemRef,GSLC_ELEMREF_GLOWING,(bGlowing)?GSLC_ELEMREF_GLOWING:0);

  if (bGlowing != bGlowingOld) {
    gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_FULL);
  }
}

bool gslc_ElemGetGlow(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef)
{
  if (pElemRef == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemGetGlow";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return false;
  }
  return gslc_GetElemRefFlag(pGui,pElemRef,GSLC_ELEMREF_GLOWING);
}


void gslc_ElemSetGlowEn(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,bool bGlowEn)
{
  if (pElemRef == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemSetGlowEn";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return;
  }
  gslc_tsElem*  pElem = gslc_GetElemFromRef(pGui,pElemRef);
  if (bGlowEn) {
    pElem->nFeatures |= GSLC_ELEM_FEA_GLOW_EN;
  } else {
    pElem->nFeatures &= ~GSLC_ELEM_FEA_GLOW_EN;
  }
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_FULL);
}

bool gslc_ElemGetGlowEn(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef)
{
  if (pElemRef == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemGetGlowEn";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return false;
  }
  gslc_tsElem*  pElem = gslc_GetElemFromRef(pGui,pElemRef);
  return pElem->nFeatures & GSLC_ELEM_FEA_GLOW_EN;
}

void gslc_ElemSetStyleFrom(gslc_tsGui* pGui,gslc_tsElemRef* pElemRefSrc,gslc_tsElemRef* pElemRefDest)
{
  if ((pElemRefSrc == NULL) || (pElemRefDest == NULL)) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemSetStyleFrom";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return;
  }
  gslc_tsElem*  pElemSrc  = gslc_GetElemFromRef(pGui,pElemRefSrc);
  gslc_tsElem*  pElemDest = gslc_GetElemFromRef(pGui,pElemRefDest);

  // TODO: Check ElemRef SRC type for compatibility

  // nId
  // nType
  // rElem
  pElemDest->nGroup           = pElemSrc->nGroup;
  pElemDest->nFeatures        = pElemSrc->nFeatures;
  pElemDest->sImgRefNorm      = pElemSrc->sImgRefNorm;
  pElemDest->sImgRefGlow      = pElemSrc->sImgRefGlow;

  pElemDest->colElemFill      = pElemSrc->colElemFill;
  pElemDest->colElemFillGlow  = pElemSrc->colElemFillGlow;
  pElemDest->colElemFrame     = pElemSrc->colElemFrame;
  pElemDest->colElemFrameGlow = pElemSrc->colElemFrameGlow;

  // eRedraw
#ifdef GLSC_COMPOUND
  pElemDest->pElemRefParent   = pElemSrc->pElemRefParent;
#endif

  // Don't copy over the text strings
  //  pStrBuf[GSLC_LOCAL_STR_LEN]
  //  pStr
  //  nStrMax
  //  eTxtFlags

  pElemDest->colElemText      = pElemSrc->colElemText;
  pElemDest->colElemTextGlow  = pElemSrc->colElemTextGlow;
  pElemDest->eTxtAlign        = pElemSrc->eTxtAlign;
  pElemDest->nTxtMargin       = pElemSrc->nTxtMargin;
  pElemDest->pTxtFont         = pElemSrc->pTxtFont;

  // pXData

  pElemDest->pfuncXEvent      = pElemSrc->pfuncXEvent;
  pElemDest->pfuncXDraw       = pElemSrc->pfuncXDraw;
  pElemDest->pfuncXTouch      = pElemSrc->pfuncXTouch;
  pElemDest->pfuncXTick       = pElemSrc->pfuncXTick;

  gslc_ElemSetRedraw(pGui,pElemRefDest,GSLC_REDRAW_FULL);
}

void gslc_ElemSetEventFunc(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,GSLC_CB_EVENT funcCb)
{
  if ((pElemRef == NULL) || (funcCb == NULL)) {;
    static const char GSLC_PMEM FUNCSTR[] = "ElemSetEventFunc";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return;
  }
  gslc_tsElem*  pElem = gslc_GetElemFromRef(pGui,pElemRef);
  pElem->pfuncXEvent       = funcCb;
}


void gslc_ElemSetDrawFunc(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,GSLC_CB_DRAW funcCb)
{
  if ((pElemRef == NULL) || (funcCb == NULL)) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemSetDrawFunc";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return;
  }
  gslc_tsElem* pElem    = gslc_GetElemFromRef(pGui,pElemRef);
  pElem->pfuncXDraw     = funcCb;
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_FULL);
}

void gslc_ElemSetTickFunc(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,GSLC_CB_TICK funcCb)
{
  if ((pElemRef == NULL) || (funcCb == NULL)) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemSetTickFunc";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return;
  }
  gslc_tsElem* pElem      = gslc_GetElemFromRef(pGui,pElemRef);
  pElem->pfuncXTick       = funcCb;
}

bool gslc_ElemOwnsCoord(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,int16_t nX,int16_t nY,bool bOnlyClickEn)
{
  if (pElemRef == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemOwnsCoord";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return false;
  }
  gslc_tsElem*  pElem = gslc_GetElemFromRef(pGui,pElemRef);
  if (bOnlyClickEn && !(pElem->nFeatures & GSLC_ELEM_FEA_CLICK_EN) ) {
    return false;
  }
  return gslc_IsInRect(nX,nY,pElem->rElem);
}

#if !defined(DRV_TOUCH_NONE)

// ------------------------------------------------------------------------
// Tracking Functions
// ------------------------------------------------------------------------

void gslc_CollectTouch(gslc_tsGui* pGui,gslc_tsCollect* pCollect,gslc_tsEventTouch* pEventTouch)
{
  // Fetch the data members of the touch event
  int16_t       nX      = pEventTouch->nX;
  int16_t       nY      = pEventTouch->nY;
  gslc_teTouch  eTouch  = pEventTouch->eTouch;

  gslc_tsElemRef*   pTrackedRefOld = NULL;
  gslc_tsElemRef*   pTrackedRefNew = NULL;

  // Fetch the item currently being tracked (if any)
  pTrackedRefOld = gslc_CollectGetElemRefTracked(pGui,pCollect);

  // Reset the in-tracked flag
  bool  bInTracked = false;

  if (eTouch == GSLC_TOUCH_DOWN) {
    // ---------------------------------
    // Touch Down Event
    // ---------------------------------

    // End glow on previously tracked element (if any)
    // - We shouldn't really enter a "Touch Down" event
    //   with an element still marked as being tracked
    if (pTrackedRefOld != NULL) {
      gslc_ElemSetGlow(pGui,pTrackedRefOld,false);
    }

    // Determine the new element to start tracking
    pTrackedRefNew = gslc_CollectFindElemFromCoord(pGui,pCollect,nX,nY);

    if (pTrackedRefNew == NULL) {
      // Didn't find an element, so clear the tracking reference
      gslc_CollectSetElemTracked(pGui,pCollect,NULL);
    } else {
      // Found an element, so mark it as being the tracked element

      // Set the new tracked element reference
      gslc_CollectSetElemTracked(pGui,pCollect,pTrackedRefNew);

      // Start glow on new element
      gslc_ElemSetGlow(pGui,pTrackedRefNew,true);

      // Notify element for optional custom handling
      // - We do this after we have determined which element should
      //   receive the touch tracking
      eTouch = GSLC_TOUCH_DOWN_IN;
      gslc_ElemSendEventTouch(pGui,pTrackedRefNew,eTouch,nX,nY);

    }

  } else if (eTouch == GSLC_TOUCH_UP) {
    // ---------------------------------
    // Touch Up Event
    // ---------------------------------

    if (pTrackedRefOld != NULL) {
      // Are we still over tracked element?
      bInTracked = gslc_ElemOwnsCoord(pGui,pTrackedRefOld,nX,nY,true);

      if (!bInTracked) {
        // Released not over tracked element
        eTouch = GSLC_TOUCH_UP_OUT;
        gslc_ElemSendEventTouch(pGui,pTrackedRefOld,eTouch,nX,nY);
      } else {
        // Notify original tracked element for optional custom handling
        eTouch = GSLC_TOUCH_UP_IN;
        gslc_ElemSendEventTouch(pGui,pTrackedRefOld,eTouch,nX,nY);
      }

      // Clear glow state
      gslc_ElemSetGlow(pGui,pTrackedRefOld,false);

    }

    // Clear the element tracking state
    gslc_CollectSetElemTracked(pGui,pCollect,NULL);

  } else if (eTouch == GSLC_TOUCH_MOVE) {
    // ---------------------------------
    // Touch Move Event
    // ---------------------------------

    // Determine if we are still over tracked element
    if (pTrackedRefOld != NULL) {
      bInTracked = gslc_ElemOwnsCoord(pGui,pTrackedRefOld,nX,nY,true);

      if (!bInTracked) {

        // Not currently over tracked element
        // - Notify tracked element that we moved out of it
        eTouch = GSLC_TOUCH_MOVE_OUT;
        gslc_ElemSendEventTouch(pGui,pTrackedRefOld,eTouch,nX,nY);

        // Ensure the tracked element is no longer glowing
        if (pTrackedRefOld) {
          gslc_ElemSetGlow(pGui,pTrackedRefOld,false);
        }
      } else {
        // We are still over tracked element
        // - Notify tracked element
        eTouch = GSLC_TOUCH_MOVE_IN;
        gslc_ElemSendEventTouch(pGui,pTrackedRefOld,eTouch,nX,nY);

        // Ensure it is glowing
        gslc_ElemSetGlow(pGui,pTrackedRefOld,true);
      }

    }

  }

}


// This routine is responsible for the GUI-level touch event state machine
// and dispatching to the touch event handler for the page
void gslc_TrackTouch(gslc_tsGui* pGui,gslc_tsPage* pPage,int16_t nX,int16_t nY,uint16_t nPress)
{
  if ((pGui == NULL) || (pPage == NULL)) {
    static const char GSLC_PMEM FUNCSTR[] = "TrackTouch";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return;
  }

  // Determine the transitions in the touch events based
  // on the previous touch pressure state
  // TODO: Provide hysteresis thresholds for the touch
  //       pressure levels in case a display outputs a
  //       variable range (eg. 15..200) that doesn't
  //       go to zero when touch is removed.
  gslc_teTouch  eTouch = GSLC_TOUCH_NONE;
  if ((pGui->nTouchLastPress == 0) && (nPress > 0)) {
    eTouch = GSLC_TOUCH_DOWN;
    #ifdef DBG_TOUCH
    GSLC_DEBUG_PRINT(" TS : (%3d,%3d) Pressure=%3u : TouchDown\n",nX,nY,nPress);
    #endif
  } else if ((pGui->nTouchLastPress > 0) && (nPress == 0)) {
    eTouch = GSLC_TOUCH_UP;
    #ifdef DBG_TOUCH
    GSLC_DEBUG_PRINT(" TS : (%3d,%3d) Pressure=%3u : TouchUp\n",nX,nY,nPress);
    #endif

  } else if ((pGui->nTouchLastX != nX) || (pGui->nTouchLastY != nY)) {
    // We only track movement if touch is "down"
    if (nPress > 0) {
      eTouch = GSLC_TOUCH_MOVE;
      #ifdef DBG_TOUCH
      GSLC_DEBUG_PRINT(" TS : (%3d,%3d) Pressure=%3u : TouchMove\n",nX,nY,nPress);
      #endif
    }
  }

  gslc_tsEventTouch sEventTouch;
  sEventTouch.eTouch        = eTouch;

  // Save the coordinates from the touch driver
  // NOTE: Many display touch drivers return valid coordinates upon a
  //       TOUCH_UP event, whereas some return zero position coordinates in
  //       this event (eg. TFT_eSPI with ILI9486). Thus, to ensure that we
  //       have consistent detection of position when the touch is released,
  //       we will pass the previous good position in this event (transition
  //       from TOUCH_DOWN to TOUCH_UP).
  //
  //       The position during the TOUCH_UP event is used to determine if a
  //       touch was released within an element (causing a button selection)
  //       or outside of it (generally leading to a non-selection).
  //
  if (eTouch == GSLC_TOUCH_UP) {
    // Use previous (good) coordinates from touch driver
    sEventTouch.nX          = pGui->nTouchLastX;
    sEventTouch.nY          = pGui->nTouchLastY;
  } else {
    // Use most recent coordinate from touch driver
    sEventTouch.nX          = nX;
    sEventTouch.nY          = nY;
  }

  void* pvData = (void*)(&sEventTouch);
  gslc_tsEvent sEvent = gslc_EventCreate(pGui,GSLC_EVT_TOUCH,0,(void*)pPage,pvData);
  gslc_PageEvent(pGui,sEvent);


  // Save raw touch status so that we can detect transitions
  pGui->nTouchLastX      = nX;
  pGui->nTouchLastY      = nY;
  pGui->nTouchLastPress  = nPress;
}



// ------------------------------------------------------------------------
// Touchscreen Functions
// ------------------------------------------------------------------------

bool gslc_InitTouch(gslc_tsGui* pGui,const char* acDev)
{
  bool bOk;
  if (pGui == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "InitTouch";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return false;
  }

  // Call driver-specific touchscreen init
  //
  // Determine if touch events are provided by the display driver
  // or an external touch driver.
#if defined(DRV_TOUCH_NONE)
  // Touch handling disabled
  bOk = true;
#elif defined(DRV_TOUCH_IN_DISP)
  // Touch handling by display driver
  bOk = gslc_DrvInitTouch(pGui,acDev);
#else
  // Touch handling by external touch driver
  bOk = gslc_TDrvInitTouch(pGui,acDev);
#endif
  if (!bOk) {
    GSLC_DEBUG_PRINT("ERROR: InitTouch() failed in touch driver init\n",0);
  }
  return bOk;
}


bool gslc_GetTouch(gslc_tsGui* pGui,int16_t* pnX,int16_t* pnY,uint16_t* pnPress)
{
  if (pGui == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "GetTouch";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return false;
  }

#if defined(DRV_TOUCH_NONE)
  // Touch handling disabled
  return false;
#elif defined(DRV_TOUCH_IN_DISP)
  // Use display driver for touch events
  return gslc_DrvGetTouch(pGui,pnX,pnY,pnPress);
#else
  // Use external touch driver for touch events
  return gslc_TDrvGetTouch(pGui,pnX,pnY,pnPress);
#endif

  return false;
}

#endif // !DRV_TOUCH_NONE

// ------------------------------------------------------------------------
// Private Functions
// ------------------------------------------------------------------------


// NOTE: nId is a positive ID specified by the user or
//       GSLC_ID_AUTO if the user wants an auto-generated ID
//       (which will be assigned in Element nId)
#if (GSLC_FEATURE_COMPOUND)
// NOTE: When we are creating sub-elements within a compound element,
//       we usually pass nPageId=GSLC_PAGE_NONE. In this mode we
//       won't add the element to any page, but just create the
//       element struct. However, in this mode we can't support
//       auto-generated IDs since we don't know which IDs will
//       be taken when we finally create the compound element.
#endif
gslc_tsElem gslc_ElemCreate(gslc_tsGui* pGui,int16_t nElemId,int16_t nPageId,
  int16_t nType,gslc_tsRect rElem,char* pStrBuf,uint8_t nStrBufMax,int16_t nFontId)
{
  gslc_tsElem sElem;
  // Assign defaults to the element record
  gslc_ResetElem(&sElem);

  if (pGui == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemCreate";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return sElem;
  }

  gslc_tsPage*    pPage = NULL;
  gslc_tsCollect* pCollect = NULL;

  // If we are going to be adding the element to a page then we
  // perform some additional checks
  if (nPageId == GSLC_PAGE_NONE) {
#if (GSLC_FEATURE_COMPOUND)
    // This is a temporary element, so we skip the ID collision checks.
    // In this mode we don't support auto ID assignment
    if (nElemId == GSLC_ID_AUTO) {
      GSLC_DEBUG_PRINT("ERROR: ElemCreate(%s) doesn't support temp elements with auto ID\n","");
      return sElem;
    }
#endif
  } else {
    // Look up the targeted page to ensure that we check its
    // collection for collision with other IDs (or assign the
    // next available if auto-incremented)
    pPage = gslc_PageFindById(pGui,nPageId);
    if (pPage == NULL) {
      GSLC_DEBUG_PRINT("ERROR: ElemCreate() can't find page (ID=%d)\n",nPageId);
      return sElem;
    }
    pCollect  = &pPage->sCollect;

    // Validate the user-supplied ID
    if (nElemId == GSLC_ID_AUTO) {
      // Get next auto-generated ID
      nElemId = gslc_CollectGetNextId(pGui,pCollect);
    } else {
      // Ensure the ID is positive
      if (nElemId < 0) {
        GSLC_DEBUG_PRINT("ERROR: ElemCreate() called with negative ID (%d)\n",nElemId);
        return sElem;
      }
      // Ensure the ID isn't already taken
      if (gslc_CollectFindElemById(pGui,pCollect,nElemId) != NULL) {
        GSLC_DEBUG_PRINT("ERROR: ElemCreate() called with existing ID (%d)\n",nElemId);
        return sElem;
      }
    }
  }


  // Override defaults with parameterization
  sElem.nId             = nElemId;
  sElem.rElem           = rElem;
  sElem.nType           = nType;
  sElem.pTxtFont        = gslc_FontGet(pGui,nFontId);

  // Initialize the local string buffer (if enabled via GSLC_LOCAL_STR)
  // otherwise just save a copy of the external string buffer pointer
  // and maximum buffer length from the parameters.
  //
  // NOTE: GSLC_LOCAL_STR=1 mode does not accept string pointers to
  //       PROGMEM (Flash). This is because at time of ElemCreate()
  //       the string location (SRAM vs PROGMEM) won't be known (as
  //       ElemSetTxtMem() has not been called yet). Therefore, we
  //       wouldn't know to perform a copy from PROGMEM. This should not
  //       be a problem since it would be unlikely that a user would
  //       want to use internal buffers but initialized by content in
  //       flash.
  if (pStrBuf == NULL) {
    // No string enabled, so set the flag accordingly
    sElem.nStrBufMax = 0;
    sElem.eTxtFlags  = (sElem.eTxtFlags & ~GSLC_TXT_ALLOC) | GSLC_TXT_ALLOC_NONE;
  } else {
    #if (GSLC_LOCAL_STR)
      // NOTE: Assume the string buffer pointer is located in RAM and not PROGMEM
      strncpy(sElem.pStrBuf,pStrBuf,GSLC_LOCAL_STR_LEN-1);
      sElem.pStrBuf[GSLC_LOCAL_STR_LEN-1] = '\0';  // Force termination
      sElem.nStrBufMax = GSLC_LOCAL_STR_LEN;
      sElem.eTxtFlags  = (sElem.eTxtFlags & ~GSLC_TXT_ALLOC) | GSLC_TXT_ALLOC_INT;
    #else
      // No need to copy locally; instead, we are going to retain
      // the external string pointer (must be static)
      sElem.pStrBuf    = pStrBuf;
      sElem.nStrBufMax = nStrBufMax;
      sElem.eTxtFlags  = (sElem.eTxtFlags & ~GSLC_TXT_ALLOC) | GSLC_TXT_ALLOC_EXT;
    #endif
  }

  // If the element creation was successful, then set the valid flag
  sElem.nFeatures      |= GSLC_ELEM_FEA_VALID;

  return sElem;
}

// ------------------------------------------------------------------------
// Collect Event Handlers
// ------------------------------------------------------------------------


// Common event handler
bool gslc_CollectEvent(void* pvGui,gslc_tsEvent sEvent)
{
  if (pvGui == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "CollectEvent";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return false;
  }
  void*           pvScope   = sEvent.pvScope;
  gslc_tsCollect* pCollect  = (gslc_tsCollect*)(pvScope);

  unsigned        nInd;
  gslc_tsElemRef* pElemRef = NULL;

  // Handle any collection-based events first
  // ...
  if (sEvent.eType == GSLC_EVT_TOUCH) {
    #if defined(DRV_TOUCH_NONE)
    return false;
    #else
    gslc_tsGui*     pGui      = (gslc_tsGui*)(pvGui);
    void*           pvData    = sEvent.pvData;
    // TOUCH is passed to CollectTouch which determines the element
    // in the collection that should receive the event
    gslc_tsEventTouch* pEventTouch = (gslc_tsEventTouch*)(pvData);
    gslc_CollectTouch(pGui,pCollect,pEventTouch);
    return true;
    #endif  // !DRV_TOUCH_NONE

  } else if ( (sEvent.eType == GSLC_EVT_DRAW) || (sEvent.eType == GSLC_EVT_TICK) ) {
    // DRAW and TICK are propagated down to all elements in collection

    for (nInd=0;nInd<pCollect->nElemRefCnt;nInd++) {
      pElemRef = &(pCollect->asElemRef[nInd]);

      // Copy event so we can modify it in the loop
      gslc_tsEvent sEventNew = sEvent;
      sEventNew.pvScope = (void*)(pElemRef);

      // Propagate the event to the element
      gslc_ElemEvent(pvGui,sEventNew);

    } // nInd

  } // eType

  return true;
}


// NOTE:
// - When eFlags=GSLC_ELEMREF_SRC_RAM, the contents of pElem are copied into
//   the internal element array so that the pointer (pElem) can be released
//   after this call (ie. it can be an automatic variable).
// - When eFlags=GSLC_ELEMREF_SRC_PROG, the pointer value pElem is saved
//   into the element reference array so that the element can be fetched
//   directly from PROGMEM (Flash) at a later time. In this mode, pElem
//   must be a static variable.
// - When eFlags=GSLC_ELEMREF_SRC_CONST, the same is done as for
//   GSLC_ELEMREF_SRC_PROG except that PROGMEM is not required to access.
gslc_tsElemRef* gslc_CollectElemAdd(gslc_tsGui* pGui,gslc_tsCollect* pCollect,const gslc_tsElem* pElem,gslc_teElemRefFlags eFlags)
{
  if ((pCollect == NULL) || (pElem == NULL)) {
    static const char GSLC_PMEM FUNCSTR[] = "CollectElemAdd";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return NULL;
  }

  if (pCollect->nElemRefCnt+1 > (pCollect->nElemRefMax)) {
    GSLC_DEBUG_PRINT("ERROR: CollectElemAdd() too many element references (max=%u)\n",pCollect->nElemRefMax);
    return NULL;
  }


  // If the element is stored in RAM:
  // - Copy the element into the internal RAM element array (asElem)
  // - Create an entry in the element reference array (asElemRef) that points
  //   to the internal RAM element array.
  // - Since a copy has been made of the input element, the caller can
  //   discard/reuse the pointer (pElem) after the call.
  //
  // If the element is stored in FLASH:
  // - Save the element pointer (pElem) into the element reference
  //   array (asElemRef).
  // - Since the input pointer (pElem) has been recorded, the caller must
  //   not discard/reuse the pointer. It should be defined as a static
  //   variable.

  uint16_t nElemInd;
  uint16_t nElemRefInd;
  if ((eFlags & GSLC_ELEMREF_SRC) == GSLC_ELEMREF_SRC_RAM) {

    // Ensure we have enough space in internal element array
    if (pCollect->nElemCnt+1 > (pCollect->nElemMax)) {
      GSLC_DEBUG_PRINT("ERROR: CollectElemAdd() too many RAM elements (max=%u)\n",pCollect->nElemMax);
      return NULL;
    }

    // Copy the element to the internal array
    // - This performs a copy so that we can discard the element
    //   pointer after the call is complete
    nElemInd = pCollect->nElemCnt;
    pCollect->asElem[nElemInd] = *pElem;
    pCollect->nElemCnt++;

    // Add a reference
    // - Pointer (pElem) links to an item of internal element array
    nElemRefInd = pCollect->nElemRefCnt;
    pCollect->asElemRef[nElemRefInd].eElemFlags = eFlags;
    pCollect->asElemRef[nElemRefInd].pElem = &(pCollect->asElem[nElemInd]);
    pCollect->nElemRefCnt++;

  } else {
    // External reference
    // - Pointer (pElem) links to an element stored in FLASH (must be declared statically)

    // Add a reference
    nElemRefInd = pCollect->nElemRefCnt;
    pCollect->asElemRef[nElemRefInd].eElemFlags = eFlags;
    pCollect->asElemRef[nElemRefInd].pElem = (gslc_tsElem*)pElem;  // Typecast to drop const modifier
    pCollect->nElemRefCnt++;
  }

  // Return the new element reference
  return &(pCollect->asElemRef[nElemRefInd]);
}

bool gslc_CollectGetRedraw(gslc_tsGui* pGui,gslc_tsCollect* pCollect)
{
  if (pCollect == NULL) {
    // ERROR
    return false;
  }
  // Determine if any sub-element in collection needs redraw
  // - This is generally used when deciding whether to redraw
  //   a compound element
  uint16_t        nInd;
  gslc_tsElemRef* pSubElemRef;
  bool            bCollectRedraw = false;

  for (nInd=0;nInd<pCollect->nElemRefCnt;nInd++) {
    // Fetch the element pointer from the reference array
    pSubElemRef = &(pCollect->asElemRef[nInd]);
    if (gslc_ElemGetRedraw(pGui,pSubElemRef) != GSLC_REDRAW_NONE) {
      bCollectRedraw = true;
      break;
    }
  }

  return bCollectRedraw;
}

// Add an element to the collection associated with the page
//
// - Depending on the GSLC_ELEMREF_SRC_* setting, CollectElemAdd()
//   may either copy the contents of the element into the internal
//   RAM array or else just save a copy of the pointer (which
//   would mean that the pointer must not be invalidated).
gslc_tsElemRef* gslc_ElemAdd(gslc_tsGui* pGui,int16_t nPageId,gslc_tsElem* pElem,gslc_teElemRefFlags eFlags)
{
  if ((pGui == NULL) || (pElem == NULL)) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemAdd";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return NULL;
  }

  if (nPageId == GSLC_PAGE_NONE) {
    // No page ID was provided, so skip add
    GSLC_DEBUG_PRINT("ERROR: ElemAdd(%s) with PAGE_NONE\n","");
    return NULL;
  }

  // Fetch the page containing the item
  gslc_tsPage* pPage = gslc_PageFindById(pGui,nPageId);
  if (pPage == NULL) {
    GSLC_DEBUG_PRINT("ERROR: ElemAdd() page (ID=%d) was not found\n",nPageId);
    return NULL;
  }

  gslc_tsCollect* pCollect = &pPage->sCollect;
  gslc_tsElemRef* pElemRefAdd = gslc_CollectElemAdd(pGui,pCollect,pElem,eFlags);
  return pElemRefAdd;
}

bool gslc_SetClipRect(gslc_tsGui* pGui,gslc_tsRect* pRect)
{
  // Update the drawing clip rectangle
  if (pRect == NULL) {
    // Set to full size of screen
    return gslc_DrvSetClipRect(pGui,NULL);
  } else {
    // Set to user-specified region
    return gslc_DrvSetClipRect(pGui,pRect);
  }
}


void gslc_ElemSetImage(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,gslc_tsImgRef sImgRef,
  gslc_tsImgRef sImgRefSel)
{
  if ((pGui == NULL) || (pElemRef == NULL)) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemSetImage";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return;
  }

  gslc_tsElem*  pElem = gslc_GetElemFromRef(pGui,pElemRef);

  // Update the normal and glowing images
  gslc_DrvSetElemImageNorm(pGui,pElem,sImgRef);
  gslc_DrvSetElemImageGlow(pGui,pElem,sImgRefSel);
}

bool gslc_SetBkgndImage(gslc_tsGui* pGui,gslc_tsImgRef sImgRef)
{
  if (!gslc_DrvSetBkgndImage(pGui,sImgRef)) {
    return false;
  }
  gslc_PageFlipSet(pGui,true);
  return true;
}

bool gslc_SetBkgndColor(gslc_tsGui* pGui,gslc_tsColor nCol)
{
  if (!gslc_DrvSetBkgndColor(pGui,nCol)) {
    return false;
  }
  gslc_PageFlipSet(pGui,true);
  return true;
}

// Trigger a touch event on an element
bool gslc_ElemSendEventTouch(gslc_tsGui* pGui,gslc_tsElemRef* pElemRefTracked,
        gslc_teTouch eTouch,int16_t nX,int16_t nY)
{
#if defined(DRV_TOUCH_NONE)
  return false;
#else

  gslc_tsEventTouch sEventTouch;
  sEventTouch.eTouch        = eTouch;
  sEventTouch.nX            = nX;
  sEventTouch.nY            = nY;
  gslc_tsEvent sEvent = gslc_EventCreate(pGui,GSLC_EVT_TOUCH,0,(void*)pElemRefTracked,&sEventTouch);
  gslc_ElemEvent((void*)pGui,sEvent);
  return true;
#endif // !DRV_TOUCH_NONE
}

// Initialize the element struct to all zeros
void gslc_ResetElem(gslc_tsElem* pElem)
{
  if (pElem == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "ResetElem";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return;
  }
  pElem->nFeatures        = GSLC_ELEM_FEA_NONE;
  pElem->nFeatures       &= ~GSLC_ELEM_FEA_VALID; // Init with Valid=false
  pElem->nFeatures       &= ~GSLC_ELEM_FEA_CLICK_EN;
  pElem->nFeatures       &= ~GSLC_ELEM_FEA_GLOW_EN;
  pElem->nFeatures       &= ~GSLC_ELEM_FEA_FRAME_EN;
  pElem->nFeatures       &= ~GSLC_ELEM_FEA_FILL_EN;
  pElem->nId              = GSLC_ID_NONE;
  pElem->nType            = GSLC_TYPE_BOX;
  pElem->nGroup           = GSLC_GROUP_ID_NONE;
  pElem->rElem            = (gslc_tsRect){0,0,0,0};
  pElem->sImgRefNorm      = gslc_ResetImage();
  pElem->sImgRefGlow      = gslc_ResetImage();
  pElem->colElemFrame     = GSLC_COL_WHITE;
  pElem->colElemFill      = GSLC_COL_WHITE;
  pElem->colElemFrameGlow = GSLC_COL_WHITE;
  pElem->colElemFillGlow  = GSLC_COL_WHITE;
  pElem->eTxtFlags        = GSLC_TXT_DEFAULT;
  #if (GSLC_LOCAL_STR)
    pElem->pStrBuf[0]       = '\0';
    pElem->nStrBufMax       = 0;
  #else
    pElem->pStrBuf          = NULL;
    pElem->nStrBufMax       = 0;
  #endif
  pElem->colElemText      = GSLC_COL_WHITE;
  pElem->colElemTextGlow  = GSLC_COL_WHITE;
  pElem->eTxtAlign        = GSLC_ALIGN_MID_MID;
  pElem->nTxtMargin       = 0;
  pElem->pTxtFont         = NULL;

  pElem->pXData           = NULL;
  pElem->pfuncXEvent      = NULL;
  pElem->pfuncXDraw       = NULL;
  pElem->pfuncXTouch      = NULL;
  pElem->pfuncXTick       = NULL;
#if (GSLC_FEATURE_COMPOUND)
  pElem->pElemRefParent   = NULL;
#endif

}

// Initialize the font struct to all zeros
void gslc_ResetFont(gslc_tsFont* pFont)
{
  if (pFont == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "ResetFont";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return;
  }
  pFont->nId            = GSLC_FONT_NONE;
  pFont->eFontRefType   = GSLC_FONTREF_FNAME;
  pFont->pvFont         = NULL;
  pFont->nSize          = 0;
}


// Close down an element
void gslc_ElemDestruct(gslc_tsElem* pElem)
{
  if (pElem == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemDestruct";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return;
  }
  if (pElem->sImgRefNorm.pvImgRaw != NULL) {
    gslc_DrvImageDestruct(pElem->sImgRefNorm.pvImgRaw);
    pElem->sImgRefNorm = gslc_ResetImage();
  }
  if (pElem->sImgRefGlow.pvImgRaw != NULL) {
    gslc_DrvImageDestruct(pElem->sImgRefGlow.pvImgRaw);
    pElem->sImgRefGlow = gslc_ResetImage();
  }

  // TODO: Mark Element valid as false?

  // TODO: Add callback function so that
  // we can support additional closure actions
  // (eg. closing sub-elements of compound element).

}


// Close down a collection
void gslc_CollectDestruct(gslc_tsGui* pGui,gslc_tsCollect* pCollect)
{
  if (pCollect == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "CollectDestruct";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return;
  }
  uint16_t      nInd;
  gslc_tsElem*  pElem = NULL;

  for (nInd=0;nInd<pCollect->nElemRefCnt;nInd++) {
    gslc_tsElemRef*     pElemRef = &pCollect->asElemRef[nInd];
    gslc_teElemRefFlags eFlags = pElemRef->eElemFlags;
    // Only elements in RAM need destruction
    if ((eFlags & GSLC_ELEMREF_SRC) != GSLC_ELEMREF_SRC_RAM) {
      continue;
    }
    // Fetch the element pointer from the reference array
    pElem = gslc_GetElemFromRef(pGui,pElemRef);
    gslc_ElemDestruct(pElem);
  }

}

// Close down all in page
void gslc_PageDestruct(gslc_tsGui* pGui,gslc_tsPage* pPage)
{
  if (pPage == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "PageDestruct";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return;
  }
  gslc_tsCollect* pCollect = &pPage->sCollect;
  gslc_CollectDestruct(pGui,pCollect);
}

// Close down all GUI members, including pages and fonts
void gslc_GuiDestruct(gslc_tsGui* pGui)
{
  if (pGui == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "GuiDestruct";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return;
  }
  // Loop through all pages in GUI
  uint8_t nPageInd;
  gslc_tsPage*  pPage = NULL;
  for (nPageInd=0;nPageInd<pGui->nPageCnt;nPageInd++) {
    pPage = &pGui->asPage[nPageInd];
    gslc_PageDestruct(pGui,pPage);
  }

  // TODO: Consider moving into main element array
  if (pGui->sImgRefBkgnd.eImgFlags != GSLC_IMGREF_NONE) {
    gslc_DrvImageDestruct(pGui->sImgRefBkgnd.pvImgRaw);
    pGui->sImgRefBkgnd = gslc_ResetImage();
  }

  // Close all fonts
  gslc_DrvFontsDestruct(pGui);

  // Close any driver-specific data
  gslc_DrvDestruct(pGui);

}


// ================================
// Private: Element Collection

void gslc_CollectReset(gslc_tsCollect* pCollect,gslc_tsElem* asElem,uint16_t nElemMax,
        gslc_tsElemRef* asElemRef,uint16_t nElemRefMax)
{
  if (pCollect == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "CollectReset";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return;
  }

  pCollect->nElemMax          = nElemMax;
  pCollect->nElemCnt          = 0;

  pCollect->nElemAutoIdNext   = GSLC_ID_AUTO_BASE;

  pCollect->pfuncXEvent       = NULL;

  // Save the pointer to the element array
  pCollect->asElem = asElem;

  uint16_t nInd;
  for (nInd=0;nInd<nElemMax;nInd++) {
    gslc_ResetElem(&(pCollect->asElem[nInd]));
  }

  // Initialize element references
  pCollect->nElemRefMax = nElemRefMax;
  pCollect->nElemRefCnt = 0;
  pCollect->asElemRef   = asElemRef;
  for (nInd=0;nInd<nElemMax;nInd++) {
    (pCollect->asElemRef[nInd]).pElem = NULL;
  }
}

// Search internal element array for one with a particular ID
gslc_tsElemRef* gslc_CollectFindElemById(gslc_tsGui* pGui,gslc_tsCollect* pCollect,int16_t nElemId)
{
  if (pCollect == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "CollectFindElemById";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return NULL;
  }
  gslc_tsElem*      pElem = NULL;
  gslc_tsElemRef*   pElemRef = NULL;
  gslc_tsElemRef*   pFoundElemRef = NULL;
  uint16_t          nInd;

  if (nElemId == GSLC_ID_TEMP) {
    // ERROR: Don't expect to do this
    GSLC_DEBUG_PRINT("ERROR: CollectFindElemById(%s) searching for temp ID\n","");
    return NULL;
  }

  for (nInd=0;nInd<pCollect->nElemRefCnt;nInd++) {
    // Fetch the element pointer from the reference array
    pElemRef = &(pCollect->asElemRef[nInd]);
    pElem = gslc_GetElemFromRef(pGui,pElemRef);

    if (pElem->nId == nElemId) {
      pFoundElemRef = pElemRef;
      break;
    }

  }

  return pFoundElemRef;
}

int gslc_CollectGetNextId(gslc_tsGui* pGui,gslc_tsCollect* pCollect)
{
  int16_t nElemId = pCollect->nElemAutoIdNext;
  pCollect->nElemAutoIdNext++;
  return nElemId;
}

gslc_tsElemRef* gslc_CollectGetElemRefTracked(gslc_tsGui* pGui,gslc_tsCollect* pCollect)
{
  return pCollect->pElemRefTracked;
}

void gslc_CollectSetElemTracked(gslc_tsGui* pGui,gslc_tsCollect* pCollect,gslc_tsElemRef* pElemRef)
{
  pCollect->pElemRefTracked = pElemRef;
}

// Find an element index in a collection from a coordinate
gslc_tsElemRef* gslc_CollectFindElemFromCoord(gslc_tsGui* pGui,gslc_tsCollect* pCollect,int16_t nX, int16_t nY)
{
  uint16_t              nInd;
  bool                  bFound = false;
  gslc_tsElemRef*       pElemRef = NULL;
  gslc_tsElemRef*       pFoundElemRef = NULL;

  for (nInd=0;nInd<pCollect->nElemRefCnt;nInd++) {
    pElemRef  = &(pCollect->asElemRef[nInd]);

    bFound = gslc_ElemOwnsCoord(pGui,pElemRef,nX,nY,true);
    if (bFound) {
      pFoundElemRef = pElemRef;
      // Stop searching
      break;
    }
  }
   // Return pointer or NULL if none found
  return pFoundElemRef;
}


#if (GSLC_FEATURE_COMPOUND)
// Go through all elements in a collection and set the parent
// element pointer.
void gslc_CollectSetParent(gslc_tsGui* pGui,gslc_tsCollect* pCollect,gslc_tsElemRef* pElemRefParent)
{
  gslc_tsElemRef* pElemRef = NULL;
  gslc_tsElem*    pElem = NULL;
  uint16_t        nInd;
  for (nInd=0;nInd<pCollect->nElemRefCnt;nInd++) {
    gslc_teElemRefFlags eFlags = pCollect->asElemRef[nInd].eElemFlags;
    // NOTE: Only elements in RAM are updated
    // TODO: Error handling if we attempt to modify FLASH-based element
    if ((eFlags & GSLC_ELEMREF_SRC) != GSLC_ELEMREF_SRC_RAM) {
      continue;
    }
    pElemRef = &pCollect->asElemRef[nInd];
    pElem = gslc_GetElemFromRef(pGui,pElemRef);
    pElem->pElemRefParent = pElemRefParent;
  }
}
#endif

void gslc_CollectSetEventFunc(gslc_tsGui* pGui,gslc_tsCollect* pCollect,GSLC_CB_EVENT funcCb)
{
  if ((pCollect == NULL) || (funcCb == NULL)) {
    static const char GSLC_PMEM FUNCSTR[] = "CollectSetEventFunc";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return;
  }
  pCollect->pfuncXEvent       = funcCb;
}

// ============================================================================

// Trigonometric lookup table for sin(x)
// - The lookup table saves us from having to import the sin/cos
//   functions and the associated floating point libraries. GUIslice
//   has provided a relatively lightweight fixed-point representation
//   using the following 16-bit LUT (8-bit index).
// - At this point in time, the LUT is only used by GUIslice for sin/cos
//   in supporting the XGauge Radial controls (enabled by GSLC_FEATURE_XGAUGE_RADIAL)
// - The LUT consumes approx 514 bytes of FLASH memory
uint16_t  m_nLUTSinF0X16[257] = {
  0x0000,0x0192,0x0324,0x04B6,0x0648,0x07DA,0x096C,0x0AFD,0x0C8F,0x0E21,0x0FB2,0x1143,0x12D5,0x1465,0x15F6,0x1787,
  0x1917,0x1AA7,0x1C37,0x1DC6,0x1F56,0x20E5,0x2273,0x2402,0x258F,0x271D,0x28AA,0x2A37,0x2BC3,0x2D4F,0x2EDB,0x3066,
  0x31F1,0x337B,0x3505,0x368E,0x3816,0x399E,0x3B26,0x3CAD,0x3E33,0x3FB9,0x413E,0x42C3,0x4447,0x45CA,0x474C,0x48CE,
  0x4A4F,0x4BD0,0x4D4F,0x4ECE,0x504D,0x51CA,0x5347,0x54C3,0x563E,0x57B8,0x5931,0x5AAA,0x5C21,0x5D98,0x5F0E,0x6083,
  0x61F7,0x636A,0x64DC,0x664D,0x67BD,0x692C,0x6A9A,0x6C07,0x6D73,0x6EDE,0x7048,0x71B1,0x7319,0x747F,0x75E5,0x7749,
  0x78AC,0x7A0F,0x7B6F,0x7CCF,0x7E2E,0x7F8B,0x80E7,0x8242,0x839B,0x84F3,0x864A,0x87A0,0x88F5,0x8A48,0x8B99,0x8CEA,
  0x8E39,0x8F86,0x90D3,0x921E,0x9367,0x94AF,0x95F6,0x973B,0x987F,0x99C1,0x9B02,0x9C41,0x9D7F,0x9EBB,0x9FF6,0xA12F,
  0xA266,0xA39D,0xA4D1,0xA604,0xA735,0xA865,0xA993,0xAABF,0xABEA,0xAD13,0xAE3B,0xAF60,0xB085,0xB1A7,0xB2C8,0xB3E7,
  0xB504,0xB61F,0xB739,0xB851,0xB967,0xBA7B,0xBB8E,0xBC9F,0xBDAE,0xBEBB,0xBFC6,0xC0D0,0xC1D7,0xC2DD,0xC3E1,0xC4E3,
  0xC5E3,0xC6E1,0xC7DD,0xC8D7,0xC9D0,0xCAC6,0xCBBB,0xCCAD,0xCD9E,0xCE8C,0xCF79,0xD063,0xD14C,0xD232,0xD317,0xD3F9,
  0xD4DA,0xD5B8,0xD695,0xD76F,0xD847,0xD91D,0xD9F1,0xDAC3,0xDB93,0xDC60,0xDD2C,0xDDF5,0xDEBD,0xDF82,0xE045,0xE106,
  0xE1C4,0xE281,0xE33B,0xE3F3,0xE4A9,0xE55D,0xE60E,0xE6BD,0xE76A,0xE815,0xE8BE,0xE964,0xEA08,0xEAAA,0xEB4A,0xEBE7,
  0xEC82,0xED1B,0xEDB1,0xEE45,0xEED7,0xEF67,0xEFF4,0xF07F,0xF108,0xF18E,0xF212,0xF294,0xF313,0xF390,0xF40A,0xF483,
  0xF4F9,0xF56C,0xF5DD,0xF64C,0xF6B9,0xF723,0xF78A,0xF7F0,0xF853,0xF8B3,0xF911,0xF96D,0xF9C6,0xFA1D,0xFA72,0xFAC4,
  0xFB13,0xFB61,0xFBAB,0xFBF4,0xFC3A,0xFC7D,0xFCBE,0xFCFD,0xFD39,0xFD73,0xFDAA,0xFDDF,0xFE12,0xFE42,0xFE6F,0xFE9A,
  0xFEC3,0xFEE9,0xFF0D,0xFF2E,0xFF4D,0xFF69,0xFF83,0xFF9B,0xFFB0,0xFFC2,0xFFD2,0xFFE0,0xFFEB,0xFFF3,0xFFFA,0xFFFD,
  0xFFFF,
};
