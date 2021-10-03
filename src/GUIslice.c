// =======================================================================
// GUIslice library
// - Calvin Hass
// - https://www.impulseadventure.com/elec/guislice-gui.html
// - https://github.com/ImpulseAdventure/GUIslice
//
// - Version 0.17.0
// =======================================================================
//
// The MIT License
//
// Copyright 2016-2021 Calvin Hass
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
/// \file GUIslice.c


// GUIslice library

// Import configuration ( which will import a sub-config depending on device type)
#include "GUIslice_config.h"

#include "GUIslice.h"
#include "GUIslice_drv.h"

#include <stdio.h>

#if defined(DBG_REDRAW)
  #include <Arduino.h> // For delay()
#endif // DBG_REDRAW

#ifdef DBG_FRAME_RATE
  #include <time.h> // for FrameRate reporting
#endif

#if (GSLC_USE_FLOAT)
  #include <math.h>
#endif

#if (GSLC_USE_PROGMEM)
  #if defined(__AVR__)
    #include <avr/pgmspace.h>
  #else
    #include <pgmspace.h>
  #endif
#endif

#include <stdarg.h>         // For va_*

// Version tracking
#include "GUIslice_version.h"

// ========================================================================

/// Global debug output function
/// - The user assigns this function via gslc_InitDebug()
GSLC_CB_DEBUG_OUT g_pfDebugOut = NULL;

// Forward declaration for trigonometric lookup table
#if defined(LUT_SIN_64)
extern const uint16_t  m_nLUTSinF0X16[65];
#else
extern const uint16_t  m_nLUTSinF0X16[257];
#endif


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
  (void)pGui; // Unused
  return (char*)GUISLICE_VER;
}

const char* gslc_GetNameDisp(gslc_tsGui* pGui)
{
  return gslc_DrvGetNameDisp(pGui);
}

const char* gslc_GetNameTouch(gslc_tsGui* pGui)
{
  return gslc_DrvGetNameTouch(pGui);
}

void* gslc_GetDriverDisp(gslc_tsGui* pGui)
{
  return gslc_DrvGetDriverDisp(pGui);
}

void* gslc_GetDriverTouch(gslc_tsGui* pGui)
{
  return gslc_DrvGetDriverTouch(pGui);
}


bool gslc_Init(gslc_tsGui* pGui,void* pvDriver,gslc_tsPage* asPage,uint8_t nMaxPage,gslc_tsFont* asFont,uint8_t nMaxFont)
{
  unsigned  nInd;
  bool      bOk = true;

  // Provide indication that debug messaging is active
  #if !defined(INIT_MSG_DISABLE)
  GSLC_DEBUG_PRINT("GUIslice version [%s]:\n", gslc_GetVer(pGui));
  #endif

  pGui->eInitStatTouch = GSLC_INITSTAT_UNDEF;

  // Initialize state
  pGui->nDisp0W         = 0;
  pGui->nDisp0H         = 0;
  pGui->nDispW          = 0;
  pGui->nDispH          = 0;
  pGui->nDispDepth      = 0;

  #if defined(DRV_DISP_ADAGFX) || defined(DRV_DISP_ADAGFX_AS) || defined(DRV_DISP_TFT_ESPI) || defined(DRV_DISP_M5STACK)

    // Assign default orientation
    pGui->nRotation     = GSLC_ROTATE;

    #if !defined(DRV_TOUCH_NONE)
      // Assign arbitrary defaults
      // These will be overwritten during DrvInit()
      pGui->nSwapXY = 0;
      pGui->nFlipX = 0;
      pGui->nFlipY = 0;
      pGui->nTouchCalXMin = 100;
      pGui->nTouchCalXMax = 1000;
      pGui->nTouchCalYMin = 100;
      pGui->nTouchCalYMax = 1000;
      pGui->bTouchRemapYX = false;
    #endif // !DRV_TOUCH_NONE

  #endif

  // Default to remapping enabled
  pGui->bTouchRemapEn = true;


  pGui->nPageMax        = nMaxPage;
  pGui->nPageCnt        = 0;
  pGui->asPage          = asPage;

  for (nInd = 0; nInd < GSLC_STACK__MAX; nInd++) {
    pGui->apPageStack[nInd] = NULL;
    pGui->abPageStackActive[nInd] = true;
    pGui->abPageStackDoDraw[nInd] = true;
  }
  pGui->bRedrawNeeded      = false;
  pGui->bScreenNeedRedraw  = true;
  pGui->bScreenNeedFlip    = false;

  gslc_InvalidateRgnReset(pGui);

   // Clear the event-pending struct
  pGui->bEventPending = false;
  pGui->sEventPend.eType = GSLC_EVT_NONE; 

  // Default global element characteristics
  pGui->nRoundRadius = 4;

  // Default image transparency setting
  // - Used when GSLC_BMP_TRANS_EN=1
  // - Defined by config file GSLC_BMP_TRANS_RGB
  // - Can be overridden by user with SetTransparentColor()
  #if defined(GSLC_BMP_TRANS_RGB)
    pGui->sTransCol = (gslc_tsColor){ GSLC_BMP_TRANS_RGB };
  #else
    // Provide a default if not defined in config
    // - The following is equivalent to GSLC_COL_MAGENTA
    pGui->sTransCol = (gslc_tsColor) { 0xFF, 0x00, 0xFF };
  #endif

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

  //pGui->pfuncXEvent           = NULL; // UNUSED
  pGui->pfuncPinPoll          = NULL;

  pGui->asInputMap            = NULL;
  pGui->nInputMapMax          = 0;
  pGui->nInputMapCnt          = 0;
  pGui->nInputMode            = GSLC_INPUTMODE_NAV; // Navigate mode

  #if (GSLC_FEATURE_INPUT)
  pGui->nFocusPageInd         = GSLC_IND_NONE;
  pGui->pFocusPage            = NULL;
  pGui->pFocusElemRef         = NULL;
  pGui->nFocusElemInd         = GSLC_IND_NONE;
  pGui->nFocusElemMax         = 0;
  pGui->nFocusSavedPageInd    = GSLC_IND_NONE;
  pGui->nFocusSavedElemInd    = GSLC_IND_NONE;

  pGui->colFocusNone          = GSLC_COL_BLACK;
  pGui->colFocus              = GSLC_COL_MAGENTA;
  pGui->colFocusEdit          = GSLC_COL_RED;
  #endif // GSLC_FEATURE_INPUT

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
  if (bOk) {
    bOk &= gslc_DrvInit(pGui);
    if (bOk) {
      #if !defined(INIT_MSG_DISABLE)
      GSLC_DEBUG_PRINT("- Init display handler [%s] OK\n", gslc_GetNameDisp(pGui));
      #endif
    } else {
      GSLC_DEBUG_PRINT("- Init display handler [%s] FAIL\n", gslc_GetNameDisp(pGui));
    }
  }
  #if defined(DRV_TOUCH_NONE)
    pGui->eInitStatTouch = GSLC_INITSTAT_INACTIVE;
    GSLC_DEBUG_PRINT("- No touch handler enabled\n", "");
  #else
    if (bOk) {
      // Touch initialization is not made to be a fatal error
      // Instead, a flag is set that can be used to alert the
      // user on their display (in case the debug messaging was
      // not enabled)
      bool bTouchOk = gslc_InitTouch(pGui,GSLC_DEV_TOUCH);
      if (bTouchOk) {
        #if !defined(INIT_MSG_DISABLE)
        GSLC_DEBUG_PRINT("- Init touch handler [%s] OK\n", gslc_GetNameTouch(pGui));
        #endif
        pGui->eInitStatTouch = GSLC_INITSTAT_ACTIVE;
      } else {
        GSLC_DEBUG_PRINT("- Init touch handler [%s] FAIL\n", gslc_GetNameTouch(pGui));
        pGui->eInitStatTouch = GSLC_INITSTAT_FAIL;
      }
    }
  #endif

  // Initialize the entire display as being invalidated
  gslc_InvalidateRgnScreen(pGui);

  // If the display didn't initialize properly, then return
  // false which should mark it as a fatal error. Note that
  // touch handler errors are not included as fatal errors.
  if (!bOk) { GSLC_DEBUG_PRINT("ERROR: Init(%s) failed\n",""); }
  return bOk;
}

void gslc_SetPinPollFunc(gslc_tsGui* pGui,GSLC_CB_PIN_POLL pfunc)
{
  pGui->pfuncPinPoll = pfunc;
}


void gslc_InitInputMap(gslc_tsGui* pGui,gslc_tsInputMap* asInputMap,uint8_t nInputMapMax)
{
#if !(GSLC_FEATURE_INPUT)
  (void)pGui; // Unused
  (void)asInputMap; // Unused
  (void)nInputMapMax; // Unused
  return;
#else
  (void)pGui; // Unused
  pGui->asInputMap    = asInputMap;
  pGui->nInputMapMax  = nInputMapMax;
  pGui->nInputMapCnt  = 0;
#endif
}

void gslc_InputMapAdd(gslc_tsGui* pGui,gslc_teInputRawEvent eInputEvent,int16_t nInputVal,gslc_teAction eAction,int16_t nActionVal)
{
#if !(GSLC_FEATURE_INPUT)
  (void)pGui; // Unused
  (void)eInputEvent; // Unused
  (void)nInputVal; // Unused
  (void)eAction; // Unused
  (void)nActionVal; // Unused
  return;
#else
  if (pGui->nInputMapCnt >= pGui->nInputMapMax) {
    GSLC_DEBUG2_PRINT("ERROR: InputMapAdd() too many mappings. Max=%u\n",pGui->nInputMapMax);
    return;
  }
  gslc_tsInputMap sInputMap;
  sInputMap.eEvent      = eInputEvent;
  sInputMap.nVal        = nInputVal;
  sInputMap.eAction     = eAction;
  sInputMap.nActionVal  = nActionVal;
  pGui->asInputMap[pGui->nInputMapCnt] = sInputMap;
  pGui->nInputMapCnt++;
#endif
}

bool gslc_InputMapLookup(gslc_tsGui* pGui,gslc_teInputRawEvent eInputEvent,int16_t nInputVal,gslc_teAction* peAction,int16_t* pnActionVal)
{
#if !(GSLC_FEATURE_INPUT)
  (void)pGui; // Unused
  (void)eInputEvent; // Unused
  (void)nInputVal; // Unused
  (void)peAction; // Unused
  (void)pnActionVal; // Unused
  return false;
#else
  uint8_t         nInputInd;
  uint8_t         nInputMax = pGui->nInputMapCnt;
  bool            bFound = false;
  gslc_tsInputMap sMapEntry;
  // Assign defaults
  *peAction     = GSLC_ACTION_UNDEF;
  *pnActionVal  = 0;
  // Search
  for (nInputInd=0;((nInputInd<nInputMax)&&(!bFound));nInputInd++) {
    sMapEntry = pGui->asInputMap[nInputInd];
    if ((sMapEntry.eEvent == eInputEvent) && (sMapEntry.nVal == nInputVal)) {
      bFound = true;
      *peAction = pGui->asInputMap[nInputInd].eAction;
      *pnActionVal = sMapEntry.nActionVal;
    }
  }
  return bFound;
#endif
}


void gslc_InitDebug(GSLC_CB_DEBUG_OUT pfunc)
{
  g_pfDebugOut = pfunc;
}


// Internal enumerations for printf() parser state machine
typedef enum {
  GSLC_S_DEBUG_PRINT_NORM,
  GSLC_S_DEBUG_PRINT_TOKEN,
  GSLC_S_DEBUG_PRINT_UINT16,
  GSLC_S_DEBUG_PRINT_CHAR,
  GSLC_S_DEBUG_PRINT_STR,
  GSLC_S_DEBUG_PRINT_STR_P
} gslc_teDebugPrintState;

// A lightweight printf() routine that calls user function for
// character output (enabling redirection to Serial). Only
// supports the following tokens:
// - %u (16-bit unsigned int in RAM) [see NOTE]
// - %d (16-bit signed int in RAM)
// - %c (character)
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
    char     cFmt = 0;
    char     cOut = 0;

    va_list  vlist;
    va_start(vlist,pFmt);

    gslc_teDebugPrintState  nState = GSLC_S_DEBUG_PRINT_NORM;

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

      if (nState == GSLC_S_DEBUG_PRINT_NORM) {

        if (cFmt == '%') {
          nState = GSLC_S_DEBUG_PRINT_TOKEN;
        } else {
          // Normal char
          (g_pfDebugOut)(cFmt);
        }
        nFmtInd++; // Advance format index

      } else if (nState == GSLC_S_DEBUG_PRINT_TOKEN) {

        // Get token
        if (cFmt == 'd') {
          nState = GSLC_S_DEBUG_PRINT_UINT16;
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
          nState = GSLC_S_DEBUG_PRINT_UINT16;
          nNumRemain = va_arg(vlist,unsigned);
          bNumNeg = false;
          bNumStart = false;
          nNumDivisor = nMaxDivisor;

        } else if (cFmt == 'c') {
          nState = GSLC_S_DEBUG_PRINT_CHAR;
          cOut = (char)va_arg(vlist,unsigned);

        } else if (cFmt == 's') {
          nState = GSLC_S_DEBUG_PRINT_STR;
          pStr = va_arg(vlist,char*);

        } else if (cFmt == 'z') {
          nState = GSLC_S_DEBUG_PRINT_STR_P;
          pStr = va_arg(vlist,char*);
        } else {
          // ERROR
        }
        nFmtInd++; // Advance format index

      } else if (nState == GSLC_S_DEBUG_PRINT_STR) {
        while (*pStr != 0) {
          cOut = *pStr;
          (g_pfDebugOut)(cOut);
          pStr++;
        }
        nState = GSLC_S_DEBUG_PRINT_NORM;
        // Don't advance format string index

      } else if (nState == GSLC_S_DEBUG_PRINT_STR_P) {
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
        nState = GSLC_S_DEBUG_PRINT_NORM;
        // Don't advance format string index

      } else if (nState == GSLC_S_DEBUG_PRINT_CHAR) {
        (g_pfDebugOut)(cOut);
        nState = GSLC_S_DEBUG_PRINT_NORM;

      } else if (nState == GSLC_S_DEBUG_PRINT_UINT16) {

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
          nState = GSLC_S_DEBUG_PRINT_NORM;
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
  // The touch handling logic is used by both the touchscreen
  // handler as well as the GPIO/pin/keyboard input controller
  #if !defined(DRV_TOUCH_NONE)

  // Check to see if we had a touch initialization error
  // if so, mark this on the display.
  if (pGui->eInitStatTouch == GSLC_INITSTAT_FAIL) {
    gslc_DrvDrawTxt(pGui,5,5,NULL,(char*)"ERROR: InitTouch",
      GSLC_TXT_DEFAULT, GSLC_COL_RED, GSLC_COL_BLACK);
  }

  // --------------------------------------------------------------
  // Handle any pending events
  // --------------------------------------------------------------
  if (pGui->bEventPending) {
    gslc_ElemEvent((void*)pGui,pGui->sEventPend);
    pGui->bEventPending = false;
  }

  // ---------------------------------------------
  // Touch handling
  // ---------------------------------------------

  // TODO: Move the physical button / GPIO handling (pfuncPinPoll)
  //       outside of !defined(DRV_TOUCH_NONE) check so that we can
  //       enable physical button handling without requiring
  //       all of the other touch driver logic.
  //       Consider creating a define that indicates whether the
  //       touch coordinate handling should be compiled or not
  //       (eg. if !DRV_TOUCH_NONE && !DRV_TOUCH_INPUT)

  int16_t               nTouchX = 0;
  int16_t               nTouchY = 0;
  uint16_t              nTouchPress = 0;
  bool                  bEvent = false;
  gslc_teInputRawEvent  eInputEvent = GSLC_INPUT_NONE;
  int16_t               nInputVal = 0;

  // Handle touchscreen presses
  // - We clear the event queue here so that we don't fall behind
  // - In the time it takes to update the display, several mouse /
  //   finger events may have occurred. If we only handle a single
  //   motion event per display update, then we may experience very
  //   lagging responsiveness from the controls.
  // - Instead, we drain the event queue before proceeding on to the
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
    bEvent = false;

    // --------------------------------------------------------------
    // First check physical pin inputs
    // --------------------------------------------------------------

    #if (GSLC_FEATURE_INPUT)
    int16_t  nPinNum = -1;
    int16_t  nPinState = 0;
    GSLC_CB_PIN_POLL  pfuncPinPoll = pGui->pfuncPinPoll;

    if (pfuncPinPoll != NULL) {
      bEvent = (*pfuncPinPoll)(pGui,&nPinNum,&nPinState);
      if (bEvent) {
        // We only enter here after a pin change/edge event was detected.
        // - PinState=1 means the pin state is now 1 (pressed)
        // - PinState=0 means the pin state is now 0 (released)
        eInputEvent = nPinState? GSLC_INPUT_PIN_ASSERT : GSLC_INPUT_PIN_DEASSERT;
        nInputVal = nPinNum;
      }
    } 
    #endif // GSLC_FEATURE_INPUT

    // --------------------------------------------------------------
    // If no event found yet, check touch / keyboard
    // --------------------------------------------------------------
    if (!bEvent) {
      // Fetch input event, which could include touch / mouse / keyboard / pin
      bEvent = gslc_GetTouch(pGui, &nTouchX, &nTouchY, &nTouchPress, &eInputEvent, &nInputVal);
    }

    // --------------------------------------------------------------
    // If event found, handle it
    // --------------------------------------------------------------
    if (bEvent) {
      // Track and handle the input events
      // - Handle the events on the current page
      switch (eInputEvent) {
        case GSLC_INPUT_KEY_DOWN:
          gslc_TrackInput(pGui,eInputEvent,nInputVal);
          break;
        case GSLC_INPUT_KEY_UP:
          // NOTE: For now, only handling key-down events
          // TODO: gslc_TrackInput(pGui,eInputEvent,nInputVal);
          break;

        case GSLC_INPUT_PIN_ASSERT:
          gslc_TrackInput(pGui,eInputEvent,nInputVal);
          break;
        case GSLC_INPUT_PIN_DEASSERT:
          gslc_TrackInput(pGui,eInputEvent,nInputVal);
          break;

        case GSLC_INPUT_TOUCH:
          // Track and handle the touch events
          // - Handle the events on the current page
          gslc_TrackTouch(pGui,NULL,nTouchX,nTouchY,nTouchPress);

          #ifdef DBG_TOUCH
          // Highlight current touch for coordinate debug
          gslc_tsRect rMark = gslc_ExpandRect((gslc_tsRect){(int16_t)nTouchX,(int16_t)nTouchY,1,1},1,1);
          gslc_DrawFrameRect(pGui,rMark,GSLC_COL_YELLOW);
          #endif
          break;

        case GSLC_INPUT_NONE:
        default:
          break;

      }

      nNumEvts++;
    }

    // Should we stop handling events?
    if ((!bEvent) || (nNumEvts >= GSLC_TOUCH_MAX_EVT)) {
      bDoneEvts = true;
    }
  } while (!bDoneEvts);

  #endif // !DRV_TOUCH_NONE

  // ---------------------------------------------

  // Issue a timer tick to all pages
  // - This is independent of the pages in the stack
  uint8_t nPageInd;
  gslc_tsPage* pPage = NULL;
  for (nPageInd=0;nPageInd<pGui->nPageCnt;nPageInd++) {
    pPage = &pGui->asPage[nPageInd];
    gslc_tsEvent sEvent = gslc_EventCreate(pGui,GSLC_EVT_TICK,0,(void*)pPage,NULL);
    gslc_PageEvent(pGui,sEvent);
  }

  // Perform any redraw required for current page
  // - Only perform the redraw if at least one element
  //   has been marked as requiring redraw
  if (pGui->bRedrawNeeded) {
    gslc_PageRedrawGo(pGui);
  }

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

  // Provide periodic yield
  // - This instruction is important for some devices such as ESP8266
  #if defined(ESP8266)
    yield();
  #endif

}

gslc_tsEvent  gslc_EventCreate(gslc_tsGui* pGui,gslc_teEventType eType,uint8_t nSubType,void* pvScope,void* pvData)
{
  (void)pGui; // Unused
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
  if ( (nSelX >= rRect.x) && (nSelX <= rRect.x+(int16_t)rRect.w) &&
     (nSelY >= rRect.y) && (nSelY <= rRect.y+(int16_t)rRect.h) ) {
    return true;
  } else {
    return false;
  }
}

bool gslc_IsInWH(int16_t nSelX,int16_t nSelY,uint16_t nWidth,uint16_t nHeight)
{
  if ( (nSelX >= 0) && (nSelX <= (int16_t)(nWidth)-1) &&
     (nSelY >= 0) && (nSelY <= (int16_t)(nHeight)-1) ) {
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
  (void)pFname; // Unused
  (void)eFmt; // Unused
  // TODO: Change message to also handle non-Arduino output
  GSLC_DEBUG_PRINT("ERROR: GSLC_SD_EN not enabled\n","");
  //GSLC_DEBUG2_PRINT("ERROR: GetImageFromSD(%s) not supported as Config:GSLC_SD_EN=0\n","");
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
#if (GSLC_USE_PROGMEM)
  sImgRef.eImgFlags = GSLC_IMGREF_SRC_PROG | (GSLC_IMGREF_FMT & eFmt);
#else
  // Fallback to RAM if PROGMEM not implemented
  sImgRef.eImgFlags = GSLC_IMGREF_SRC_RAM | (GSLC_IMGREF_FMT & eFmt);
#endif
  sImgRef.pFname    = NULL;
  sImgRef.pImgBuf   = pImgBuf;
  sImgRef.pvImgRaw  = NULL;
  return sImgRef;
}


// Sine function with optional lookup table
// - Note that the n64Ang range is limited by 16-bit integers
//   to an effective degree range of -511 to +511 degrees,
//   defined by the max integer range: 32767/64.
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
  // TODO: Clean up excess integer typecasting

  // Use lookup tables
  bool bNegate = false;

  // Support multiple waveform periods
  if (n64Ang >= 360*64) {
    n64Ang = n64Ang - (360*64);
  } else if (n64Ang <= -360*64) {
    n64Ang = n64Ang + (360*64);
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
#if defined(LUT_SIN_64)
  uint16_t  nLutInd = ((uint32_t)n64Ang * 64)/(90*64);
#else
  uint16_t  nLutInd = ((uint32_t)n64Ang * 256)/(90*64);
#endif
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
// - Note that the n64Ang range is limited by 16-bit integers
//   to an effective degree range of -511 to +511 degrees,
//   defined by the max integer range: 32767/64.
int16_t gslc_cosFX(int16_t n64Ang)
{
#if (GSLC_USE_FLOAT)
int16_t   nRetValS;
  // Use floating-point math library function

  // Calculate angle in radians
  float fAngRad = n64Ang * GSLC_2PI / (360.0*64.0);
  // Perform floating point calc
  float fCos = cos(fAngRad);
  // Return as fixed point result
  nRetValS = fCos * 32767.0;
  return nRetValS;

#else
  // Use lookup tables
  // Cosine function is equivalent to Sine shifted by 90 degrees
  // - To avoid int16_t overflow, we trap range beyond 360 degrees
  //   and shift the angle by one rotation (360 degrees).
  // - This is necessary because otherwise the +90*64 would reduce the
  //   effective range of cosFX() to +421 degrees.
  if (n64Ang >= 360 * 64) {
    return gslc_sinFX((n64Ang - 360*64) + 90*64);
  } else {
    return gslc_sinFX(n64Ang+90*64);
  }

#endif

}

// Convert from polar to cartesian
void gslc_PolarToXY(uint16_t nRad,int16_t n64Ang,int16_t* nDX,int16_t* nDY)
{
  int32_t nTmp;
  // TODO: Clean up excess integer typecasting
  nTmp = (int32_t)nRad *  gslc_sinFX(n64Ang);
  *nDX = nTmp / 32767;
  nTmp = (int32_t)nRad * -gslc_cosFX(n64Ang);
  *nDY = nTmp / 32767;
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
  int32_t   nSubBlendAmt;
  if (nBlendAmt >= nMidAmt) {
    nSubBlendAmt = (int32_t)(nBlendAmt - nMidAmt)*1000/nRngHigh;
    colNew.r = nSubBlendAmt*(colEnd.r - colMid.r)/1000 + colMid.r;
    colNew.g = nSubBlendAmt*(colEnd.g - colMid.g)/1000 + colMid.g;
    colNew.b = nSubBlendAmt*(colEnd.b - colMid.b)/1000 + colMid.b;
  } else {
    nSubBlendAmt = (int32_t)(nBlendAmt - 0)*1000/nRngLow;
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
  GSLC_DEBUG2_PRINT("ERROR: Mandatory DrvDrawPoint() is not defined in driver\n");
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
  int16_t nDxS = (int32_t)nRadStart * gslc_sinFX(n64Ang)/32768;
  int16_t nDyS = (int32_t)nRadStart * gslc_cosFX(n64Ang)/32768;
  int16_t nDxE = (int32_t)nRadEnd   * gslc_sinFX(n64Ang)/32768;
  int16_t nDyE = (int32_t)nRadEnd   * gslc_cosFX(n64Ang)/32768;
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

void gslc_DrawFrameRoundRect(gslc_tsGui* pGui,gslc_tsRect rRect,int16_t nRadius,gslc_tsColor nCol)
{
  // Ensure dimensions are valid
  if ((rRect.w == 0) || (rRect.h == 0)) {
    return;
  }

#if (DRV_HAS_DRAW_RECT_ROUND_FRAME)
  // Call optimized driver implementation
  gslc_DrvDrawFrameRoundRect(pGui,rRect,nRadius,nCol);
#else
  // TODO: Add emulation of rounded rects. For now fallback to square corners
  gslc_DrvDrawFrameRect(pGui,rRect,nCol);
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

void gslc_DrawFillRoundRect(gslc_tsGui* pGui,gslc_tsRect rRect,int16_t nRadius,gslc_tsColor nCol)
{
  // Ensure dimensions are valid
  if ((rRect.w == 0) || (rRect.h == 0)) {
    return;
  }

#if (DRV_HAS_DRAW_RECT_ROUND_FILL)
  // Call optimized driver implementation
  gslc_DrvDrawFillRoundRect(pGui,rRect,nRadius,nCol);
#else
  // TODO: Add emulation of rounded rects. For now fallback to square corners
  gslc_DrvDrawFillRect(pGui,rRect,nCol);
#endif

  gslc_PageFlipSet(pGui,true);
}



// Expand or contract a rectangle in width and/or height (equal
// amounts on both side), based on the centerpoint of the rectangle.
gslc_tsRect gslc_ExpandRect(gslc_tsRect rRect,int16_t nExpandW,int16_t nExpandH)
{
  gslc_tsRect rNew = {0,0,0,0};

  // Detect error case of contracting region too far
  if ( ((int16_t)rRect.w < (-2*nExpandW)) || ((int16_t)rRect.h < (-2*nExpandH)) ) {
    // Return an empty coordinate box (which won't be drawn)
    //GSLC_DEBUG2_PRINT("ERROR: ExpandRect(%d,%d) contracts too far\n",nExpandW,nExpandH);
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


// Expand the current rect (pRect) to enclose the additional rect region (rAddRect)
void gslc_UnionRect(gslc_tsRect* pRect,gslc_tsRect rAddRect)
{
  int16_t nSrcX0, nSrcY0, nSrcX1, nSrcY1;
  int16_t nAddX0, nAddY0, nAddX1, nAddY1;

  // If the source rect has zero dimensions, then treat as empty
  if ((pRect->w == 0) || (pRect->h == 0)) {
    // No source region defined, simply copy add region
    *pRect = rAddRect;
    return;
  }

  // Source region valid, so increase dimensions

  // Calculate the rect boundary coordinates
  nSrcX0 = pRect->x;
  nSrcY0 = pRect->y;
  nSrcX1 = pRect->x + pRect->w - 1;
  nSrcY1 = pRect->y + pRect->h - 1;
  nAddX0 = rAddRect.x;
  nAddY0 = rAddRect.y;
  nAddX1 = rAddRect.x + rAddRect.w - 1;
  nAddY1 = rAddRect.y + rAddRect.h - 1;

  // Find the new maximal dimensions
  nSrcX0 = (nAddX0 < nSrcX0) ? nAddX0 : nSrcX0;
  nSrcY0 = (nAddY0 < nSrcY0) ? nAddY0 : nSrcY0;
  nSrcX1 = (nAddX1 > nSrcX1) ? nAddX1 : nSrcX1;
  nSrcY1 = (nAddY1 > nSrcY1) ? nAddY1 : nSrcY1;

  // Update the original rect region
  pRect->x = nSrcX0;
  pRect->y = nSrcY0;
  pRect->w = nSrcX1 - nSrcX0 + 1;
  pRect->h = nSrcY1 - nSrcY0 + 1;

}

void gslc_InvalidateRgnReset(gslc_tsGui* pGui)
{
#if defined(DBG_REDRAW)
  GSLC_DEBUG_PRINT("DBG: InvRgnReset\n", "");
#endif
  pGui->bInvalidateEn = false;
  pGui->rInvalidateRect = (gslc_tsRect) { 0, 0, 1, 1 };
}

void gslc_InvalidateRgnScreen(gslc_tsGui* pGui)
{
#if defined(DBG_REDRAW)
  GSLC_DEBUG_PRINT("DBG: InvRgnScreen\n", "");
#endif
  pGui->bInvalidateEn = true;
  pGui->rInvalidateRect = (gslc_tsRect) { 0, 0, pGui->nDispW, pGui->nDispH };
}

void gslc_InvalidateRgnPage(gslc_tsGui* pGui, gslc_tsPage* pPage)
{
  if (pPage == NULL) {
    return;
  }
#if defined(DBG_REDRAW)
  GSLC_DEBUG_PRINT("DBG: InvRgnPage: Page=%d (%d,%d)-(%d,%d)\n",
    pPage->nPageId,
    pPage->rBounds.x, pPage->rBounds.y, pPage->rBounds.x + pPage->rBounds.w - 1, pPage->rBounds.y + pPage->rBounds.h - 1); //xxx
#endif // DBG_REDRAW
  gslc_InvalidateRgnAdd(pGui, pPage->rBounds);
  pGui->bInvalidateEn = true;
}


void gslc_InvalidateRgnAdd(gslc_tsGui* pGui, gslc_tsRect rAddRect)
{
  if (pGui->bInvalidateEn) {
    gslc_UnionRect(&(pGui->rInvalidateRect), rAddRect);
  } else {
    pGui->bInvalidateEn = true;
    pGui->rInvalidateRect = rAddRect;
  }
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

void gslc_DrawFillSectorBase(gslc_tsGui* pGui, int16_t nQuality, int16_t nMidX, int16_t nMidY, int16_t nRad1, int16_t nRad2,
  gslc_tsColor cArcStart, gslc_tsColor cArcEnd,bool bGradient, int16_t nAngGradStart, int16_t nAngGradRange,int16_t nAngSecStart,int16_t nAngSecEnd)
{
  gslc_tsPt anPts[4];

  // Calculate degrees per step (based on quality setting)
  int16_t nStepAng = 360 / nQuality;
  int16_t nStep64 = 64 * nStepAng;

  int16_t nAng64;
  int16_t nX, nY;
  int16_t nSegStart, nSegEnd;
  gslc_tsColor colSeg;

  nSegStart = nAngSecStart * (int32_t)nQuality / 360;
  nSegEnd = nAngSecEnd * (int32_t)nQuality / 360;

  int16_t nSegGradStart, nSegGradRange;
  nSegGradStart = nAngGradStart * (int32_t)nQuality / 360;
  nSegGradRange = nAngGradRange * (int32_t)nQuality / 360;
  nSegGradRange = (nSegGradRange == 0) ? 1 : nSegGradRange; // Guard against div/0

  bool bClockwise;
  int16_t nSegInd;
  int16_t nStepCnt;
  if (nSegEnd >= nSegStart) {
    nStepCnt = nSegEnd - nSegStart;
    bClockwise = true;
  } else {
    nStepCnt = nSegStart - nSegEnd;
    bClockwise = false;
  }

  #if defined(DBG_REDRAW)
  //GSLC_DEBUG_PRINT("FillSector: AngSecStart=%d AngSecEnd=%d SegStart=%d SegEnd=%d StepCnt=%d CW=%d\n",
  //  nAngSecStart,nAngSecEnd,nSegStart,nSegEnd,nStepCnt,bClockwise);
  #endif

  for (int16_t nStepInd = 0; nStepInd < nStepCnt; nStepInd++) {
    // Remap from the step to the segment index, depending on direction
    nSegInd = (bClockwise)? (nSegStart + nStepInd) : (nSegStart - nStepInd - 1);

    nAng64 = (int32_t)(nSegInd * nStep64) % (int32_t)(360 * 64);
	
    #if defined(DBG_REDRAW)
    GSLC_DEBUG2_PRINT("FillSector:  StepInd=%d SegInd=%d (%d..%d) Ang64=%d\n", nStepInd, nSegInd, nSegStart, nSegEnd, nAng64);
    #endif

    gslc_PolarToXY(nRad1, nAng64, &nX, &nY);
    anPts[0] = (gslc_tsPt) { nMidX + nX, nMidY + nY };
    gslc_PolarToXY(nRad2, nAng64, &nX, &nY);
    anPts[1] = (gslc_tsPt) { nMidX + nX, nMidY + nY };
    gslc_PolarToXY(nRad2, nAng64 + nStep64, &nX, &nY);
    anPts[2] = (gslc_tsPt) { nMidX + nX, nMidY + nY };
    gslc_PolarToXY(nRad1, nAng64 + nStep64, &nX, &nY);
    anPts[3] = (gslc_tsPt) { nMidX + nX, nMidY + nY };

    if (bGradient) {
      // Gradient coloring
      int16_t nGradPos = 1000 * (int32_t)(nSegInd-nSegGradStart) / nSegGradRange;
      #if defined(DBG_REDRAW)
      GSLC_DEBUG2_PRINT("FillSector:   GradPos=%d\n", nGradPos);
      #endif
      colSeg = gslc_ColorBlend2(cArcStart, cArcEnd, 500, nGradPos);
    } else {
      // Flat coloring
      colSeg = cArcStart;
    }
    
    gslc_DrawFillQuad(pGui, anPts, colSeg);
  }
}

void gslc_DrawFillGradSector(gslc_tsGui* pGui, int16_t nQuality, int16_t nMidX, int16_t nMidY, int16_t nRad1, int16_t nRad2,
  gslc_tsColor cArcStart, gslc_tsColor cArcEnd, int16_t nAngSecStart, int16_t nAngSecEnd, int16_t nAngGradStart, int16_t nAngGradRange)
{
  gslc_DrawFillSectorBase(pGui, nQuality, nMidX, nMidY, nRad1, nRad2, cArcStart, cArcEnd, true,
    nAngGradStart, nAngGradRange, nAngSecStart, nAngSecEnd);
}

void gslc_DrawFillSector(gslc_tsGui* pGui, int16_t nQuality, int16_t nMidX, int16_t nMidY, int16_t nRad1, int16_t nRad2,
  gslc_tsColor cArc, int16_t nAngSecStart, int16_t nAngSecEnd)
{
  gslc_DrawFillSectorBase(pGui, nQuality, nMidX, nMidY, nRad1, nRad2, cArc, cArc, false,
    0, 0, nAngSecStart, nAngSecEnd);
}


// -----------------------------------------------------------------------
// Font Functions
// -----------------------------------------------------------------------

bool gslc_FontSetBase(gslc_tsGui* pGui, uint8_t nFontInd, int16_t nFontId, gslc_teFontRefType eFontRefType,
  const void* pvFontRef, uint16_t nFontSz)
{
  if (nFontInd >= pGui->nFontMax) {
    GSLC_DEBUG2_PRINT("ERROR: FontSetBase() invalid Font index=%d\n",nFontInd);
    return false;
  } else {
    // Fetch a font resource from the driver
    const void* pvFont = gslc_DrvFontAdd(eFontRefType,pvFontRef,nFontSz);
    // FIXME: Resolve a means to detect if LINUX font files failed to load
    //         and then return 'false'. Note that DrvFontAdd() may normally
    //         return NULL in ADAGFX mode for some font types.

    gslc_ResetFont(&(pGui->asFont[nFontInd]));
  
    pGui->asFont[nFontInd].eFontRefType = eFontRefType;
    // TODO: Support specification of mode via FontAdd() API?
    pGui->asFont[nFontInd].eFontRefMode = GSLC_FONTREF_MODE_DEFAULT;
    pGui->asFont[nFontInd].pvFont       = pvFont;
    pGui->asFont[nFontInd].nId          = nFontId;
    pGui->asFont[nFontInd].nSize        = nFontSz;

    return true;
  }
}


// Store font into indexed position in font storage
// - nFontId must be in range 0..nFontMax-1
bool gslc_FontSet(gslc_tsGui* pGui, int16_t nFontId, gslc_teFontRefType eFontRefType,
  const void* pvFontRef, uint16_t nFontSz)
{
  if ((nFontId < 0) || (nFontId >= pGui->nFontMax)) {
    GSLC_DEBUG2_PRINT("ERROR: FontSet() invalid Font ID=%d\n",nFontId);
    return false;
  } else {
    bool bRet = false;

    // The font index is set to the same as the font enum ID
    uint8_t nFontInd = nFontId;
    bRet = gslc_FontSetBase(pGui, nFontInd, nFontId, eFontRefType, pvFontRef, nFontSz);

    // Ensure the total font count is set to max
    pGui->nFontCnt = pGui->nFontMax;

    return bRet;
  }
}

bool gslc_FontAdd(gslc_tsGui* pGui,int16_t nFontId,gslc_teFontRefType eFontRefType,
    const void* pvFontRef,uint16_t nFontSz)
{
  if (pGui->nFontCnt+1 > (pGui->nFontMax)) {
    GSLC_DEBUG2_PRINT("ERROR: FontAdd(%s) added too many fonts\n","");
    return false;
  } else {
    bool bRet = false;

    // Fetch the next unallocated index
    uint8_t nFontInd = pGui->nFontCnt;
    bRet = gslc_FontSetBase(pGui, nFontInd, nFontId, eFontRefType, pvFontRef, nFontSz);

    // Increment the current font index
    pGui->nFontCnt++;

    return bRet;
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

bool gslc_FontSetMode(gslc_tsGui* pGui, int16_t nFontId, gslc_teFontRefMode eFontMode)
{
  gslc_tsFont* pFont = NULL;
  pFont = gslc_FontGet(pGui, nFontId);
  if (!pFont) {
    // TODO: ERROR
    return false;
  }
  pFont->eFontRefMode = eFontMode;
  return true;
}


// ------------------------------------------------------------------------
// Page Functions
// ------------------------------------------------------------------------

// Common event handler
bool gslc_PageEvent(void* pvGui,gslc_tsEvent sEvent)
{
  if (pvGui == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "PageEvent";
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return false;
  }
  //gslc_tsGui*       pGui        = (gslc_tsGui*)(pvGui);
  //void*             pvData      = sEvent.pvData;
  gslc_tsPage*        pPage       = (gslc_tsPage*)(sEvent.pvScope);
  gslc_tsCollect*     pCollect    = NULL;

  if (pPage == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "PageEvent";
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
  }

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
    GSLC_DEBUG2_PRINT("ERROR: PageAdd(%s) added too many pages\n","");
    return;
  }

  gslc_tsPage*  pPage = &pGui->asPage[pGui->nPageCnt];

  //pPage->pfuncXEvent      = NULL; // UNUSED

  // Initialize pPage->sCollect
  gslc_CollectReset(&pPage->sCollect,psElem,nMaxElem,psElemRef,nMaxElemRef);

  // Assign the requested Page ID
  pPage->nPageId = nPageId;

  // Initialize the page elements bounds to empty
  pPage->rBounds = (gslc_tsRect) { 0, 0, 0, 0 };

  // Increment the page count
  pGui->nPageCnt++;

  // Assign the first page added to be the current layer
  // in the page stack. This can be overridden later
  // with SetPageCur()
  if (gslc_GetPageCur(pGui) == GSLC_PAGE_NONE) {
    gslc_SetPageCur(pGui,nPageId);
  }
  // ------------------------------------------------------

  // Force the page to redraw
  // TODO: Should this mark the page or the screen?
  gslc_PageRedrawSet(pGui,true);

}

int gslc_GetPageCur(gslc_tsGui* pGui)
{
  gslc_tsPage* pStackPage = pGui->apPageStack[GSLC_STACK_CUR];
  if (pStackPage == NULL) {
    return GSLC_PAGE_NONE;
  }
  return pStackPage->nPageId;
}

void gslc_SetStackPage(gslc_tsGui* pGui, uint8_t nStackPos, int16_t nPageId)
{
  int16_t nPageSaved = GSLC_PAGE_NONE;
  gslc_tsPage* pPageSaved = pGui->apPageStack[nStackPos];
  if (pPageSaved != NULL) {
    nPageSaved = pPageSaved->nPageId;
  }

  gslc_tsPage* pPage = NULL;
  if (nPageId == GSLC_PAGE_NONE) {
    // Disable the page
    #if defined(DEBUG_LOG)
    GSLC_DEBUG_PRINT("INFO: Disabled PageStack[%u]\n",nStackPos);
    #endif
    pPage = NULL;
  } else {

    // Find the page
    pPage = gslc_PageFindById(pGui, nPageId);
    if (pPage == NULL) {
      GSLC_DEBUG2_PRINT("ERROR: SetStackPage() can't find page (ID=%d)\n", nPageId);
      return;
    }
  }

  // Save a reference to the selected page
  pGui->apPageStack[nStackPos] = pPage;

  #if defined(DEBUG_LOG)
  GSLC_DEBUG_PRINT("INFO: Changed PageStack[%u] to page %u\n",nStackPos,nPageId);
  #endif

  // A change of page should always force a future redraw
  // TODO: Consider that showing a popup could potentially optimize out
  // the forced redraw step.
  if (nPageSaved != nPageId) {
    gslc_PageRedrawSet(pGui,true);
  }

  // Invalidate the old page in the stack
  if (pPageSaved != NULL) {
    gslc_InvalidateRgnPage(pGui, pPageSaved);
  }

  // Invalidate the new page in the stack and any above
  for (uint8_t nStackPage = nStackPos; nStackPage < GSLC_STACK__MAX; nStackPage++) {
    // Select the page collection to process
    pPage = pGui->apPageStack[nStackPage];
    if (pPage != NULL) {
      gslc_InvalidateRgnPage(pGui, pPage);
    }
  }
}


void gslc_SetStackState(gslc_tsGui* pGui, uint8_t nStackPos, bool bActive, bool bDoDraw)
{
  gslc_tsPage* pStackPage = pGui->apPageStack[nStackPos];
  if (pStackPage == NULL) {
    // TODO: Error
    return;
  }
  pGui->abPageStackActive[nStackPos] = bActive;
  pGui->abPageStackDoDraw[nStackPos] = bDoDraw;
}

void gslc_SetPageBase(gslc_tsGui* pGui, int16_t nPageId)
{
  gslc_SetStackPage(pGui, GSLC_STACK_BASE, nPageId);
}

void gslc_SetPageCur(gslc_tsGui* pGui,int16_t nPageId)
{
  gslc_SetStackPage(pGui, GSLC_STACK_CUR, nPageId);

  // If a focus is currently active, change the focus to the new page
  #if (GSLC_FEATURE_INPUT)
  if (pGui->nFocusPageInd != GSLC_IND_NONE) {
    // Update the focus to the new page and find
    // suitable default element to receive focus
    gslc_FocusElemIndSet(pGui,GSLC_STACK_CUR,GSLC_IND_NONE,true);
  }
  #endif
}

void gslc_SetPageOverlay(gslc_tsGui* pGui,int16_t nPageId)
{
  gslc_SetStackPage(pGui, GSLC_STACK_OVERLAY, nPageId);
}

void gslc_PopupShow(gslc_tsGui* pGui, int16_t nPageId, bool bModal)
{
  gslc_SetStackPage(pGui, GSLC_STACK_OVERLAY, nPageId);
  // If modal dialog selected, then deactivate other pages in stack
  // If modeless dialog selected, then don't deactivate other pages in stack
  if (bModal) {
    // Modal: deactive other pages and disable redraw
    gslc_SetStackState(pGui, GSLC_STACK_CUR, false, false);
    gslc_SetStackState(pGui, GSLC_STACK_BASE, false, false);
  }
  else {
    // Modeless: activate other pages and enable background redraw
    // NOTE: If a popup overlaps controls that continue to be updated
    //       in the background, then extra redraws will occur. In that
    //       case, it may be preferrable to set redraw to false.
    gslc_SetStackState(pGui, GSLC_STACK_CUR, true, true);
    gslc_SetStackState(pGui, GSLC_STACK_BASE, true, true);
  }
  // If a focus is currently active, save it and change the
  // focus to the new popup page
  #if (GSLC_FEATURE_INPUT)
  pGui->nFocusSavedPageInd = pGui->nFocusPageInd;
  pGui->nFocusSavedElemInd = pGui->nFocusElemInd;
  if (pGui->nFocusPageInd != GSLC_IND_NONE) {
    // Update the focus to the overlay page and find
    // suitable default element to receive focus
    gslc_FocusElemIndSet(pGui,GSLC_STACK_OVERLAY,GSLC_IND_NONE,true);
  }
  #endif
}

void gslc_PopupHide(gslc_tsGui* pGui)
{
  gslc_SetStackPage(pGui, GSLC_STACK_OVERLAY, GSLC_PAGE_NONE);
  // Ensure other pages in stack are activated
  // - This is done in case they were deactivated due to a modal popup
  gslc_SetStackState(pGui, GSLC_STACK_CUR, true, true);
  gslc_SetStackState(pGui, GSLC_STACK_BASE, true, true);

  #if (GSLC_FEATURE_INPUT)
  // If there was a focus state saved before we opened
  // the popup, restore it now
  if (pGui->nFocusSavedPageInd != GSLC_IND_NONE) {
    gslc_FocusElemIndSet(pGui,pGui->nFocusSavedPageInd,pGui->nFocusSavedElemInd,true);
    // Now clear the saved focus settings
    pGui->nFocusSavedPageInd = GSLC_IND_NONE;
    pGui->nFocusSavedElemInd = GSLC_IND_NONE;
  }
  #endif
}


// Adjust the flag that indicates whether the entire page
// requires a redraw.
void gslc_PageRedrawSet(gslc_tsGui* pGui,bool bRedraw)
{
  if (pGui == NULL) {
    return;
  }

  pGui->bScreenNeedRedraw = bRedraw;

  if (bRedraw) {
    // Set the "need redraw" flag
    pGui->bRedrawNeeded = true;
  }
}

bool gslc_PageRedrawGet(gslc_tsGui* pGui)
{
  if (pGui == NULL) {
    return false;
  }

  return pGui->bScreenNeedRedraw;
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
  uint16_t          nInd;
  int               nStackPage;
  gslc_tsElem*      pElem = NULL;
  gslc_tsElemRef*   pElemRef = NULL;
  gslc_tsCollect*   pCollect = NULL;

  bool  bRedrawFullPage = false;  // Does entire page require redraw?
  gslc_tsPage*  pPage = NULL;

  // Work on each enabled page in the stack
  for (nStackPage=0;nStackPage<GSLC_STACK__MAX;nStackPage++) {
    // Select the page collection to process
    pPage = pGui->apPageStack[nStackPage];
    if (!pPage) {
      // If this stack page is not enabled, skip to next stack page
      continue;
    }
    if (!pGui->abPageStackDoDraw[nStackPage]) {
      // If this stack page has redraw disabled, skip full-page redraw check
      continue;
    }
    pCollect = &pPage->sCollect;

    for (nInd=0;nInd<pCollect->nElemRefCnt;nInd++) {
      pElemRef = &pCollect->asElemRef[nInd];
      gslc_teElemRefFlags eFlags = pElemRef->eElemFlags;
      pElem = gslc_GetElemFromRef(pGui,pElemRef);
      //GSLC_DEBUG2_PRINT("PageRedrawCalc: Ind=%u ID=%u redraw=%u flags_old=%u fea=%u\n",nInd,pElem->nId,
      //        (eFlags & GSLC_ELEMREF_REDRAW_MASK),eFlags,pElem->nFeatures);
      if ((eFlags & GSLC_ELEMREF_REDRAW_MASK) != GSLC_ELEMREF_REDRAW_NONE) {

        // We found at least one item requiring redraw

        // If partial redraw is supported, then we
        // look out for transparent elements which may
        // still warrant full page redraw.
        if (pGui->bRedrawPartialEn) {
          // Is the element transparent?
          // FIXME: Instead of forcing full page redraw, consider
          // using clipping region for partial redraw
          if (!(pElem->nFeatures & GSLC_ELEM_FEA_FILL_EN)) {
            bRedrawFullPage = true;
          }
        } else {
          bRedrawFullPage = true;
        }

        if (bRedrawFullPage) {
          // Determined that full page needs redraw
          // so no need to check any more elements
          break;
        }

      }
    }

  } // nStackPage

  if (bRedrawFullPage) {
    // Mark the entire screen as requiring redraw
    gslc_PageRedrawSet(pGui,true);
  }

}

// Redraw the active page
// - If the page has been marked as needing redraw, then all
//   elements are rendered
// - If the entire page has not been marked as needing redraw then only
//   the elements that have been marked as needing redraw
//   are rendered.
void gslc_PageRedrawGo(gslc_tsGui* pGui)
{
  // Update any page redraw status that may be required
  // - Note that this routine handles cases where an element
  //   marked as requiring update is semi-transparent which can
  //   cause other elements to be redrawn as well.
  gslc_PageRedrawCalc(pGui);

  // Determine final state of full-screen redraw
  bool  bPageRedraw = gslc_PageRedrawGet(pGui);

  // Set the clipping based on the current invalidated region
  if (pGui->bInvalidateEn) {
    #if defined(DBG_REDRAW)
    // Note that this will still outline the invalidation region
    // even if we later discover that the changed element is on
    // a page in the stack that has been disabled through
    // abPageStackDoDraw[] = false.
    GSLC_DEBUG_PRINT("DBG: PageRedrawGo() InvRgn: En=%d (%d,%u)-(%d,%d) PageRedraw=%d\n",
      pGui->bInvalidateEn, pGui->rInvalidateRect.x, pGui->rInvalidateRect.y,
      pGui->rInvalidateRect.x + pGui->rInvalidateRect.w - 1,
      pGui->rInvalidateRect.y + pGui->rInvalidateRect.h - 1, bPageRedraw);

    // Mark the invalidation region
    gslc_DrvDrawFrameRect(pGui, pGui->rInvalidateRect, GSLC_COL_RED);

    // Slow down rendering
    delay(1000);
    #endif // DBG_REDRAW

    gslc_SetClipRect(pGui, &(pGui->rInvalidateRect));
  }
  else {
    // No invalidation region defined, so default the
    // clipping region to the entire display
    gslc_SetClipRect(pGui, NULL);
  }

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
  void*    pvData = NULL;

  // TODO: Consider creating a flag that indicates whether any elements
  // on the page have requested redraw. This would enable us to skip
  // over this exhaustive search every time we call Update()

  // Issue page redraw events to all pages in stack
  // - Start from bottom page in stack first
  for (int nStackPage = 0; nStackPage < GSLC_STACK__MAX; nStackPage++) {
    gslc_tsPage* pStackPage = pGui->apPageStack[nStackPage];
    if (!pStackPage) {
      continue;
    }
    if (!bPageRedraw && !pGui->abPageStackDoDraw[nStackPage]) {
      // When doing a full page redraw, proceed as normal
      // When only doing a parital page redraw, check to see if
      // the page has been marked as redraw-disabled. If so, skip
      // updating the elements on the page.
      //
      // The redraw-disabled mode is useful to prevent "show-through"
      // from dynamically-updating elements in lower layers of the
      // page stack (this may occur with popup dialogs). If the overlay
      // page does not overlap dynamically-updating elements, then
      // DoDraw can be set to true, enabling background updates to occur.
      continue;
    }
    pvData = (void*)(pStackPage);
    gslc_tsEvent sEvent = gslc_EventCreate(pGui,GSLC_EVT_DRAW,nSubType,pvData,NULL);
    gslc_PageEvent(pGui,sEvent);
  }


  // Clear the page redraw flag
  gslc_PageRedrawSet(pGui,false);

  // Reset the invalidated regions
  gslc_InvalidateRgnReset(pGui);
 
  // Restore the clipping region to the entire display
  gslc_SetClipRect(pGui, NULL);

  // Clear the "need redraw" flag
  pGui->bRedrawNeeded = false;

  // Page flip the entire screen
  // - TODO: We could also call Update instead of Flip as that would
  //         limit the region to refresh.
  gslc_PageFlipGo(pGui);

}


void gslc_PageFlipSet(gslc_tsGui* pGui,bool bNeeded)
{
  if (pGui == NULL) {
    return;
  }

  pGui->bScreenNeedFlip = bNeeded;

  // To assist in debug of drawing primitives, support immediate
  // rendering of the current display. Note that this only works
  // for display drivers that don't invalidate the double-buffer (eg. SDL1.2)
#ifdef DBG_DRAW_IMM
  gslc_DrvPageFlipNow(pGui);
#endif
}

bool gslc_PageFlipGet(gslc_tsGui* pGui)
{
  if (pGui == NULL) {
    return false;
  }

  return pGui->bScreenNeedFlip;
}

void gslc_PageFlipGo(gslc_tsGui* pGui)
{
  if (pGui == NULL) {
    return;
  }

  if (pGui->bScreenNeedFlip) {
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
    GSLC_DEBUG2_PRINT("ERROR: PageFindById() can't find page (ID=%d)\n",nPageId);
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
    GSLC_DEBUG2_PRINT("ERROR: PageFindElemById() can't find page (ID=%d)\n",nPageId);
    return NULL;
  }
  // Find the element reference in the page's element collection
  pElemRef = gslc_CollectFindElemById(pGui,&pPage->sCollect,nElemId);
  return pElemRef;
}

/* UNUSED
void gslc_PageSetEventFunc(gslc_tsGui* pGui,gslc_tsPage* pPage,GSLC_CB_EVENT funcCb)
{
  if ((pPage == NULL) || (funcCb == NULL)) {
    static const char GSLC_PMEM FUNCSTR[] = "PageSetEventFunc";
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return;
  }
  pPage->pfuncXEvent       = funcCb;
}
*/


// ------------------------------------------------------------------------
// Element General Functions
// ------------------------------------------------------------------------


int gslc_ElemGetId(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef)
{
  gslc_tsElem* pElem = gslc_GetElemFromRefD(pGui, pElemRef, __LINE__);
  if (!pElem) return GSLC_ID_NONE;
  return pElem->nId;
}


// Get a flag from an element reference
// Note that the flag remains aligned to the position within the flags bitfield
uint8_t gslc_GetElemRefFlag(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,uint8_t nFlagMask)
{
  (void)pGui; // Unused
  if (!pElemRef) {
    static const char GSLC_PMEM FUNCSTR[] = "GetElemRefFlag";
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return 0;
  }
  return pElemRef->eElemFlags & nFlagMask;
}

// Set a flag in an element reference
// Note that the nFlagVal must be aligned to the nFlagMask
void gslc_SetElemRefFlag(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,uint8_t nFlagMask,uint8_t nFlagVal)
{
  (void)pGui; // Unused
  if (!pElemRef) {
    static const char GSLC_PMEM FUNCSTR[] = "SetElemRefFlag";
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
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
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return NULL;
  }
  gslc_teElemRefFlags eFlags  = pElemRef->eElemFlags;
  gslc_tsElem*        pElem   = pElemRef->pElem; // NOTE below

  // NOTE: The pElem assignment above is not applicable for
  //       PROGMEM-based elements. For these elements, we
  //       need to access the structure through the FLASH APIs,
  //       hence the default assignment above will be overwritten
  //       below.

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

// Fetch element from reference, with debug
gslc_tsElem* gslc_GetElemFromRefD(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, int16_t nLineNum)
{
  if (pElemRef == NULL) {
    GSLC_DEBUG2_PRINT("ERROR: GetElemFromRefD(Line %d) pElemRef is NULL\n", nLineNum);
    return NULL;
  }
  gslc_tsElem* pElem = gslc_GetElemFromRef(pGui, pElemRef);
  if (pElem == NULL) {
    GSLC_DEBUG2_PRINT("ERROR: GetElemFromRefD(Line %d) pElem is NULL\n", nLineNum);
    return NULL;
  }
  return pElem;
}

// For extended elements, fetch the extended data structure
void* gslc_GetXDataFromRef(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, int16_t nType, int16_t nLineNum)
{
  if (pElemRef == NULL) {
    GSLC_DEBUG_PRINT("ERROR: GetXDataFromRef(Type %d, Line %d) pElemRef is NULL\n", nType, nLineNum);
    return NULL;
  }
  gslc_tsElem* pElem = gslc_GetElemFromRef(pGui, pElemRef);
  if (pElem == NULL) {
    GSLC_DEBUG_PRINT("ERROR: GetXDataFromRef(Type %d, Line %d) pElem is NULL\n", nType, nLineNum);
    return NULL;
  }
  if (pElem->nType != nType) {
    GSLC_DEBUG_PRINT("ERROR: GetXDataFromRef(Type %d, Line %d) Elem type mismatch\n", nType, nLineNum);
    return NULL;
  }
  void* pXData = pElem->pXData;
  if (pXData == NULL) {
    GSLC_DEBUG_PRINT("ERROR: GetXDataFromRef(Type %d, Line %d) pXData is NULL\n", nType, nLineNum);
    return NULL;
  }
  return pXData;
}

// ------------------------------------------------------------------------
// Element Global Functions
// ------------------------------------------------------------------------


// Set the global rounded radius for rounded rectangles
void gslc_SetRoundRadius(gslc_tsGui* pGui,uint8_t nRadius)
{
  pGui->nRoundRadius = (int16_t)nRadius;
  // Update redraw flag
  gslc_PageRedrawSet(pGui,true);
}

void gslc_SetFocusCol(gslc_tsGui* pGui,gslc_tsColor colFocusNone,gslc_tsColor colFocus,gslc_tsColor colFocusEdit)
{
  #if (GSLC_FEATURE_INPUT)
  pGui->colFocusNone = colFocusNone;
  pGui->colFocus = colFocus;
  pGui->colFocusEdit = colFocusEdit;
  // If we have changed the global settings, invalidate the page for full redraw
  gslc_InvalidateRgnScreen(pGui);
  gslc_PageFlipSet(pGui,true);
  #endif // GSLC_FEATURE_INPUT
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
  sElem.colElemFrame      = GSLC_COL_GRAY;
  sElem.colElemFrameGlow  = GSLC_COL_GRAY;
  sElem.colElemText       = GSLC_COL_YELLOW;
  sElem.colElemTextGlow   = GSLC_COL_YELLOW;
  sElem.nFeatures        |= GSLC_ELEM_FEA_FILL_EN;
  sElem.eTxtAlign         = GSLC_ALIGN_MID_LEFT;
  if (nPage != GSLC_PAGE_NONE) {
    pElemRef = gslc_ElemAdd(pGui,nPage,&sElem,GSLC_ELEMREF_DEFAULT);
    return pElemRef;
#if (GSLC_FEATURE_COMPOUND)
  } else {
    // Save as temporary element
    pGui->sElemTmp = sElem;
    pGui->sElemRefTmp.pElem = &(pGui->sElemTmp);
    pGui->sElemRefTmp.eElemFlags = GSLC_ELEMREF_DEFAULT | GSLC_ELEMREF_REDRAW_FULL;
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
    GSLC_DEBUG2_PRINT("ERROR: ElemCreateBtnTxt(ID=%d): Font(ID=%d) not loaded\n",nElemId,nFontId);
    return NULL;
  }

  sElem = gslc_ElemCreate(pGui,nElemId,nPage,GSLC_TYPE_BTN,rElem,pStrBuf,nStrBufMax,nFontId);
  sElem.colElemFill       = GSLC_COL_BLUE_DK4;
  sElem.colElemFillGlow   = GSLC_COL_BLUE_DK1;
  sElem.colElemFrame      = GSLC_COL_BLUE_DK2;
  sElem.colElemFrameGlow  = GSLC_COL_YELLOW;
  sElem.colElemText       = GSLC_COL_WHITE;
  sElem.colElemTextGlow   = GSLC_COL_WHITE;
  sElem.nFeatures        |= GSLC_ELEM_FEA_FRAME_EN;
  sElem.nFeatures        |= GSLC_ELEM_FEA_FILL_EN;
  sElem.nFeatures        |= GSLC_ELEM_FEA_CLICK_EN;
  sElem.nFeatures        |= GSLC_ELEM_FEA_GLOW_EN;
  sElem.nFeatures        |= GSLC_ELEM_FEA_FOCUS_EN;
  sElem.pfuncXTouch       = cbTouch;
  if (nPage != GSLC_PAGE_NONE) {
    pElemRef = gslc_ElemAdd(pGui,nPage,&sElem,GSLC_ELEMREF_DEFAULT);
    return pElemRef;
#if (GSLC_FEATURE_COMPOUND)
  } else {
    // Save as temporary element
    pGui->sElemTmp = sElem;
    pGui->sElemRefTmp.pElem = &(pGui->sElemTmp);
    pGui->sElemRefTmp.eElemFlags = GSLC_ELEMREF_DEFAULT | GSLC_ELEMREF_REDRAW_FULL;
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
  sElem.colElemFrame      = GSLC_COL_BLACK;
  sElem.colElemFrameGlow  = GSLC_COL_YELLOW;
  sElem.nFeatures        &= ~GSLC_ELEM_FEA_FRAME_EN;
  sElem.nFeatures        |= GSLC_ELEM_FEA_FILL_EN;
  sElem.nFeatures        |= GSLC_ELEM_FEA_CLICK_EN;
  sElem.nFeatures        |= GSLC_ELEM_FEA_GLOW_EN;
  sElem.nFeatures        |= GSLC_ELEM_FEA_FOCUS_EN;
  sElem.nFeatures        |= GSLC_ELEM_FEA_NOSHRINK; // Can't shrink due to image
  sElem.pfuncXTouch       = cbTouch;
  // Update the normal and glowing images
  gslc_DrvSetElemImageNorm(pGui,&sElem,sImgRef);
  gslc_DrvSetElemImageGlow(pGui,&sElem,sImgRefSel);
  if (nPage != GSLC_PAGE_NONE) {
    pElemRef = gslc_ElemAdd(pGui,nPage,&sElem,GSLC_ELEMREF_DEFAULT);
    return pElemRef;
#if (GSLC_FEATURE_COMPOUND)
  } else {
    // Save as temporary element
    pGui->sElemTmp = sElem;
    pGui->sElemRefTmp.pElem = &(pGui->sElemTmp);
    pGui->sElemRefTmp.eElemFlags = GSLC_ELEMREF_DEFAULT | GSLC_ELEMREF_REDRAW_FULL;
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
    pElemRef = gslc_ElemAdd(pGui,nPage,&sElem,GSLC_ELEMREF_DEFAULT);
    return pElemRef;
#if (GSLC_FEATURE_COMPOUND)
  } else {
    // Save as temporary element
    pGui->sElemTmp = sElem;
    pGui->sElemRefTmp.pElem = &(pGui->sElemTmp);
    pGui->sElemRefTmp.eElemFlags = GSLC_ELEMREF_DEFAULT | GSLC_ELEMREF_REDRAW_FULL;
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
    pElemRef = gslc_ElemAdd(pGui,nPage,&sElem,GSLC_ELEMREF_DEFAULT);
    return pElemRef;
#if (GSLC_FEATURE_COMPOUND)
  } else {
    // Save as temporary element
    pGui->sElemTmp = sElem;
    pGui->sElemRefTmp.pElem = &(pGui->sElemTmp);
    pGui->sElemRefTmp.eElemFlags = GSLC_ELEMREF_DEFAULT | GSLC_ELEMREF_REDRAW_FULL;
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
  sElem.nFeatures      |= GSLC_ELEM_FEA_FILL_EN;
  sElem.nFeatures      &= ~GSLC_ELEM_FEA_CLICK_EN;
  sElem.nFeatures      |= GSLC_ELEM_FEA_NOSHRINK; // Can't shrink due to image
  // Update the normal and glowing images
  gslc_DrvSetElemImageNorm(pGui,&sElem,sImgRef);
  gslc_DrvSetElemImageGlow(pGui,&sElem,sImgRef);

  if (nPage != GSLC_PAGE_NONE) {
    pElemRef = gslc_ElemAdd(pGui,nPage,&sElem,GSLC_ELEMREF_DEFAULT);
    return pElemRef;
#if (GSLC_FEATURE_COMPOUND)
  } else {
    // Save as temporary element
    pGui->sElemTmp = sElem;
    pGui->sElemRefTmp.pElem = &(pGui->sElemTmp);
    pGui->sElemRefTmp.eElemFlags = GSLC_ELEMREF_DEFAULT | GSLC_ELEMREF_REDRAW_FULL;
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
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
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
      pElem = gslc_GetElemFromRefD(pGui, pElemRef, __LINE__);

      // Determine if redraw is needed
      gslc_teRedrawType eRedraw = gslc_ElemGetRedraw(pGui,pElemRef);

      if (sEvent.nSubType == GSLC_EVTSUB_DRAW_FORCE) {
        // Despite the current pending redraw state of the element,
        // we will force a full redraw as requested.
        //GSLC_DEBUG_PRINT("DBG: ElemEvent(Draw) nId=%d eRedraw=%d: force to FULL\n",pElem->nId,eRedraw);
        return gslc_ElemDrawByRef(pGui,pElemRef,GSLC_REDRAW_FULL);
      } else if (eRedraw != GSLC_REDRAW_NONE) {
        // There is a pending redraw for the element. It may
        // either be an incremental or full redraw.
        //GSLC_DEBUG_PRINT("DBG: ElemEvent(Draw) nId=%d eRedraw=%d\n",pElem->nId,eRedraw);
        return gslc_ElemDrawByRef(pGui,pElemRef,eRedraw);
      } else {
        // No redraw needed pending
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
      eTouch = pTouchRec->eTouch;
      if ((eTouch & GSLC_TOUCH_TYPE_MASK) == GSLC_TOUCH_DIRECT) {
        // Pass parameters directly (they are not coordinates)
        nRelX = pTouchRec->nX;
        nRelY = pTouchRec->nY;
      } else {
        // Generate relative coordinates
        nRelX = pTouchRec->nX - pElemTracked->rElem.x;
        nRelY = pTouchRec->nY - pElemTracked->rElem.y;
      }

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
// - Tnis routine triggers an immediate redraw of an element, and may be useful
//   for user code in creating custom drawing callbacks
void gslc_ElemDraw(gslc_tsGui* pGui,int16_t nPageId,int16_t nElemId)
{
  gslc_tsElemRef* pElemRef = gslc_PageFindElemById(pGui,nPageId,nElemId);
  gslc_tsEvent sEvent = gslc_EventCreate(pGui,GSLC_EVT_DRAW,GSLC_EVTSUB_DRAW_FORCE,(void*)pElemRef,NULL);
  gslc_ElemEvent(pGui,sEvent);
}

void gslc_DrawTxtBase(gslc_tsGui* pGui, char* pStrBuf,gslc_tsRect rTxt,gslc_tsFont* pTxtFont,gslc_teTxtFlags eTxtFlags,
  int8_t eTxtAlign,gslc_tsColor colTxt,gslc_tsColor colBg,int16_t nMarginW,int16_t nMarginH)
{
  int16_t   nElemX,nElemY;
  uint16_t  nElemW,nElemH;

  nElemX    = rTxt.x;
  nElemY    = rTxt.y;
  nElemW    = rTxt.w;
  nElemH    = rTxt.h;

  // Overlay the text
  bool bRenderTxt = true;
  // Skip text render if buffer pointer not allocated
  if ((bRenderTxt) && (pStrBuf == NULL)) { bRenderTxt = false; }
  // Skip text render if string is not set
  if ((bRenderTxt) && ((eTxtFlags & GSLC_TXT_ALLOC) == GSLC_TXT_ALLOC_NONE)) { bRenderTxt = false; }

  // Do we still want to render?
  if (bRenderTxt) {
#if (DRV_HAS_DRAW_TEXT)

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
    int16_t nX0 = nElemX + nMarginW;
    int16_t nY0 = nElemY + nMarginH;
    int16_t nX1 = nX0 + nElemW - 2*nMarginW;
    int16_t nY1 = nY0 + nElemH - 2*nMarginH;

    gslc_DrvDrawTxtAlign(pGui,nX0,nY0,nX1,nY1,eTxtAlign,pTxtFont,
            pStrBuf,eTxtFlags,colTxt,colBg);

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
    int16_t       nTxtOffsetX=0;
    int16_t       nTxtOffsetY=0;
    uint16_t      nTxtSzW=0;
    uint16_t      nTxtSzH=0;
    gslc_DrvGetTxtSize(pGui,pTxtFont,pStrBuf,eTxtFlags,&nTxtOffsetX,&nTxtOffsetY,&nTxtSzW,&nTxtSzH);

    // Calculate the text alignment
    int16_t       nTxtX,nTxtY;

    // Check for ALIGNH_LEFT & ALIGNH_RIGHT. Default to ALIGNH_MID
    if      (eTxtAlign & GSLC_ALIGNH_LEFT)     { nTxtX = nElemX+nMarginW; }
    else if (eTxtAlign & GSLC_ALIGNH_RIGHT)    { nTxtX = nElemX+nElemW-nMarginW-nTxtSzW; }
    else                                       { nTxtX = nElemX+(nElemW/2)-(nTxtSzW/2); }

    // Check for ALIGNV_TOP & ALIGNV_BOT. Default to ALIGNV_MID
    if      (eTxtAlign & GSLC_ALIGNV_TOP)      { nTxtY = nElemY+nMarginH; }
    else if (eTxtAlign & GSLC_ALIGNV_BOT)      { nTxtY = nElemY+nElemH-nMarginH-nTxtSzH; }
    else                                       { nTxtY = nElemY+(nElemH/2)-(nTxtSzH/2); }

    // Now correct for offset from text bounds
    // - This is used by the driver (such as Adafruit-GFX) to provide an
    //   adjustment for baseline height, etc.
    nTxtX -= nTxtOffsetX;
    nTxtY -= nTxtOffsetY;

    // Call the driver text rendering routine
    gslc_DrvDrawTxt(pGui,nTxtX,nTxtY,pTxtFont,pStrBuf,eTxtFlags,colTxt,colBg);

#endif // DRV_OVERRIDE_TXT_ALIGN

#else // DRV_HAS_DRAW_TEXT
    // No text support in driver, so skip
#endif // DRV_HAS_DRAW_TEXT
  }
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

  gslc_tsElem* pElem = gslc_GetElemFromRefD(pGui, pElemRef, __LINE__);
  if (!pElem) return false;

  bool bOk = true;

  // --------------------------------------------------------------------------
  // Handle visibility
  // --------------------------------------------------------------------------
  bool bVisible = gslc_ElemGetVisible(pGui, pElemRef);
  if (!bVisible) {
    // The element is hidden, so no drawing required.
    // Clear the redraw-pending flag.
    gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_NONE);
    return true;
  }

  // --------------------------------------------------------------------------
  // Init for default drawing
  // --------------------------------------------------------------------------

  int16_t   nElemX,nElemY;
  uint16_t  nElemW,nElemH;

  // --------------------------------------------------------------------------
  // Calculate the style
  // --------------------------------------------------------------------------

  bool bFillEn   = pElem->nFeatures & GSLC_ELEM_FEA_FILL_EN;
  bool bRoundEn  = pElem->nFeatures & GSLC_ELEM_FEA_ROUND_EN;
  bool bFrameEn  = pElem->nFeatures & GSLC_ELEM_FEA_FRAME_EN;
  bool bGlowEn   = pElem->nFeatures & GSLC_ELEM_FEA_GLOW_EN; // Does the element support glow state?
  bool bGlowing  = bGlowEn && gslc_ElemGetGlow(pGui,pElemRef); // Element should be glowing (if enabled)
  bool bFocusEn  = pElem->nFeatures & GSLC_ELEM_FEA_FOCUS_EN; // Does the element support focus state?
  //bool bFocused  = bFocusEn && gslc_ElemGetFocus(pGui,pElemRef); // Element should be focused (if enabled)

  // Determine the regions and colors based on element state
  gslc_tsRectState sState;
  gslc_ElemCalcRectState(pGui,pElemRef,&sState);

  // Extract common fields
  nElemX    = sState.rInner.x;
  nElemY    = sState.rInner.y;
  nElemW    = sState.rInner.w;
  nElemH    = sState.rInner.h;

  // --------------------------------------------------------------------------
  // Focus
  // --------------------------------------------------------------------------

  #if (GSLC_FEATURE_INPUT)

  if (bFocusEn) {
    gslc_DrawFrameRect(pGui, sState.rFocus, sState.colFocus);
  }

  // If we only requested a focus redraw, then we are done
  if (eRedraw == GSLC_REDRAW_FOCUS) {
    // Mark the element as no longer requiring redraw
    gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_NONE);
    return true;
  }

  #endif // GSLC_FEATURE_INPUT

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
  // Background
  // --------------------------------------------------------------------------


  // Fill in the background
  if (bFillEn) {
    if (bRoundEn) {
      gslc_DrawFillRoundRect(pGui, sState.rInner, pGui->nRoundRadius, sState.colInner);
    } else {
      gslc_DrawFillRect(pGui, sState.rInner, sState.colInner);
    }
  }


  // --------------------------------------------------------------------------
  // Handle special element types
  // --------------------------------------------------------------------------
  if (pElem->nType == GSLC_TYPE_LINE) {
    gslc_DrawLine(pGui,nElemX,nElemY,nElemX+nElemW-1,nElemY+nElemH-1,sState.colInner);
  }


  // --------------------------------------------------------------------------
  // Image overlays
  // --------------------------------------------------------------------------

  // Draw any images associated with element
  if (pElem->sImgRefNorm.eImgFlags != GSLC_IMGREF_NONE) {
    if ((bGlowing) && (pElem->sImgRefGlow.eImgFlags != GSLC_IMGREF_NONE)) {
      bOk = gslc_DrvDrawImage(pGui,nElemX,nElemY,pElem->sImgRefGlow);
    } else {
      // Note that when we are focused we are highlighting the frame
      // so we just draw the normal image.
      bOk = gslc_DrvDrawImage(pGui,nElemX,nElemY,pElem->sImgRefNorm);
    }
    if (!bOk) {
      GSLC_DEBUG2_PRINT("ERROR: DrvDrawImage failed\n","");
    }
  }

  // --------------------------------------------------------------------------
  // Frame
  // --------------------------------------------------------------------------

  // Frame the region
  #ifdef DBG_FRAME
  // For debug purposes, draw a frame around every element
  gslc_DrawFrameRect(pGui,sState.rFull,GSLC_COL_GRAY_DK1);
  #else

  if (bFrameEn) {
    if (bRoundEn) {
      gslc_DrawFrameRoundRect(pGui, sState.rFull, pGui->nRoundRadius, sState.colFrm);
    } else {
      gslc_DrawFrameRect(pGui, sState.rFull, sState.colFrm);
    }
  }
  #endif

  // --------------------------------------------------------------------------
  // Text overlays
  // --------------------------------------------------------------------------

  // Draw text string if defined
  if (pElem->pStrBuf) {
    int8_t        nMarginX  = pElem->nTxtMarginX;
    int8_t        nMarginY  = pElem->nTxtMarginY;

    // Note that we use the "inner" region for text placement to
    // avoid overlapping any frame
    gslc_DrawTxtBase(pGui, pElem->pStrBuf, sState.rInner, pElem->pTxtFont, pElem->eTxtFlags,
      pElem->eTxtAlign, sState.colTxtFore, sState.colTxtBack, nMarginX, nMarginY);
  }

  // --------------------------------------------------------------------------

  // Mark the element as no longer requiring redraw
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_NONE);

  return true;
}


// ------------------------------------------------------------------------
// Element Update Functions
// ------------------------------------------------------------------------

void gslc_ElemSetFillEn(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,bool bFillEn)
{
  gslc_tsElem* pElem = gslc_GetElemFromRefD(pGui, pElemRef, __LINE__);
  if (!pElem) return;

  if (bFillEn) {
    pElem->nFeatures |= GSLC_ELEM_FEA_FILL_EN;
  } else {
    pElem->nFeatures &= ~GSLC_ELEM_FEA_FILL_EN;
  }
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_FULL);
}

void gslc_ElemSetFrameEn(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,bool bFrameEn)
{
  gslc_tsElem* pElem = gslc_GetElemFromRefD(pGui, pElemRef, __LINE__);
  if (!pElem) return;

  // Capture current element sizing
  int8_t nResizeOld,nResizeNew;
  nResizeOld = gslc_ElemCalcResizeForFocus(pGui,pElemRef);

  // Update the attribute
  if (bFrameEn) {
    pElem->nFeatures |= GSLC_ELEM_FEA_FRAME_EN;
  } else {
    pElem->nFeatures &= ~GSLC_ELEM_FEA_FRAME_EN;
  }

  // Determine any change in sizing and update the element
  nResizeNew = gslc_ElemCalcResizeForFocus(pGui,pElemRef);
  gslc_ElemGrowRect(pGui,pElemRef,(nResizeNew-nResizeOld));

  // Mark for redraw
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_FULL);

}

void gslc_ElemSetRoundEn(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,bool bRoundEn)
{
  gslc_tsElem* pElem = gslc_GetElemFromRefD(pGui, pElemRef, __LINE__);
  if (!pElem) return;

  if (bRoundEn) {
    pElem->nFeatures |= GSLC_ELEM_FEA_ROUND_EN;
  } else {
    pElem->nFeatures &= ~GSLC_ELEM_FEA_ROUND_EN;
  }
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_FULL);
}


void gslc_ElemSetCol(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,gslc_tsColor colFrame,gslc_tsColor colFill,gslc_tsColor colFillGlow)
{
  gslc_tsElem* pElem = gslc_GetElemFromRefD(pGui, pElemRef, __LINE__);
  if (!pElem) return;

  if (!gslc_ColorEqual(pElem->colElemFrame, colFrame) ||
      !gslc_ColorEqual(pElem->colElemFill, colFill) ||
      !gslc_ColorEqual(pElem->colElemFillGlow, colFillGlow)) {
    pElem->colElemFrame     = colFrame;
    pElem->colElemFill      = colFill;
    pElem->colElemFillGlow  = colFillGlow;
    gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_FULL);
  }
}

void gslc_ElemSetGlowCol(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,gslc_tsColor colFrameGlow,gslc_tsColor colFillGlow,gslc_tsColor colTxtGlow)
{
  gslc_tsElem* pElem = gslc_GetElemFromRefD(pGui, pElemRef, __LINE__);
  if (!pElem) return;

  if (!gslc_ColorEqual(pElem->colElemFrameGlow, colFrameGlow) ||
      !gslc_ColorEqual(pElem->colElemFillGlow, colFillGlow) ||
      !gslc_ColorEqual(pElem->colElemTextGlow, colTxtGlow)) {
    pElem->colElemFrameGlow   = colFrameGlow;
    pElem->colElemFillGlow    = colFillGlow;
    pElem->colElemTextGlow    = colTxtGlow;
    gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_FULL);
  }
}

void gslc_ElemSetGroup(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,int nGroupId)
{
  gslc_tsElem* pElem = gslc_GetElemFromRefD(pGui, pElemRef, __LINE__);
  if (!pElem) return;

  pElem->nGroup           = nGroupId;
}

int gslc_ElemGetGroup(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef)
{
  gslc_tsElem* pElem = gslc_GetElemFromRefD(pGui, pElemRef, __LINE__);
  if (!pElem) return GSLC_GROUP_ID_NONE;

  return pElem->nGroup;
}

void gslc_ElemSetRect(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,gslc_tsRect rElem)
{
  gslc_tsElem* pElem = gslc_GetElemFromRefD(pGui, pElemRef, __LINE__);
  if (!pElem) return;

  // Invalidate region including both rects from before & after
  gslc_InvalidateRgnAdd(pGui, pElem->rElem); // Old region
  gslc_InvalidateRgnAdd(pGui, rElem); // New region
  // Force a page redraw within the scope defined by the invalidation region
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_FULL);
  gslc_PageRedrawSet(pGui,true);

  // Update element
  pElem->rElem           = rElem;

  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_FULL);
}

gslc_tsRect gslc_ElemGetRect(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef)
{
  gslc_tsElem* pElem = gslc_GetElemFromRefD(pGui, pElemRef, __LINE__);
  if (!pElem) return (gslc_tsRect){0,0,0,0};

  return pElem->rElem;
}


void gslc_ElemSetTxtAlign(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,unsigned nAlign)
{
  gslc_tsElem* pElem = gslc_GetElemFromRefD(pGui, pElemRef, __LINE__);
  if (!pElem) return;

  pElem->eTxtAlign        = nAlign;
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_FULL);
}

void gslc_ElemSetTxtMargin(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,unsigned nMargin)
{
  gslc_tsElem* pElem = gslc_GetElemFromRefD(pGui, pElemRef, __LINE__);
  if (!pElem) return;

  pElem->nTxtMarginX       = nMargin;
  pElem->nTxtMarginY       = nMargin;
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_FULL);
}

void gslc_ElemSetTxtMarginXY(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,int8_t nMarginX,int8_t nMarginY)
{
  gslc_tsElem* pElem = gslc_GetElemFromRefD(pGui, pElemRef, __LINE__);
  if (!pElem) return;

  pElem->nTxtMarginX       = nMarginX;
  pElem->nTxtMarginY       = nMarginY;
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_FULL);
}

// Perform a deep copy with termination
void gslc_StrCopy(char* pDstStr,const char* pSrcStr,uint16_t nDstLen)
{
    // Check for read-only status
    if (nDstLen == 0) {
      // Destination has no writeable storage, so abort now
      // This can happen if we only intended the string
      // to be read-only (eg. FLASH / PROGMEM)
      return;
    }

    // Perform deep copy
    strncpy(pDstStr,pSrcStr,nDstLen-1);
    pDstStr[nDstLen-1] = '\0';  // Force termination

}

void gslc_ElemSetTxtStr(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,const char* pStr)
{
  gslc_tsElem* pElem = gslc_GetElemFromRefD(pGui, pElemRef, __LINE__);
  if (!pElem) return;

  // Check for read-only status (in case the string was
  // defined in Flash/PROGMEM)
  if (pElem->nStrBufMax == 0) {
    // String was read-only, so abort now
    return;
  }

  // To avoid unnecessary redraw / flicker, only a change in
  // the text content will drive a redraw

  if (strncmp(pElem->pStrBuf,pStr,pElem->nStrBufMax)) {
    gslc_StrCopy(pElem->pStrBuf,pStr,pElem->nStrBufMax);
    gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_INC);
  }
}

char* gslc_ElemGetTxtStr(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef)
{
  gslc_tsElem* pElem = gslc_GetElemFromRefD(pGui, pElemRef, __LINE__);
  if (!pElem) return NULL;
  return pElem->pStrBuf;
}

void gslc_ElemSetTxtCol(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,gslc_tsColor colVal)
{
  gslc_tsElem* pElem = gslc_GetElemFromRefD(pGui, pElemRef, __LINE__);
  if (!pElem) return;

  if (!gslc_ColorEqual(pElem->colElemText, colVal) ||
      !gslc_ColorEqual(pElem->colElemTextGlow, colVal)) {
    pElem->colElemText      = colVal;
    pElem->colElemTextGlow  = colVal; // Default to same color for glowing state
    // TODO: Might want to change to GSLC_REDRAW_INC
    gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_FULL);
  }
}

void gslc_ElemSetTxtMem(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,gslc_teTxtFlags eFlags)
{
  gslc_tsElem* pElem = gslc_GetElemFromRefD(pGui, pElemRef, __LINE__);
  if (!pElem) return;

  if (GSLC_LOCAL_STR) {
    if ((eFlags & GSLC_TXT_MEM) == GSLC_TXT_MEM_PROG) {
      // ERROR: Unsupported mode
      // - We don't support internal buffer mode with initialization
      //   from flash (PROGMEM)
      GSLC_DEBUG2_PRINT("ERROR: ElemSetTxtMem(%s) GSLC_LOCAL_STR can't be used with GSLC_TXT_MEM_PROG\n","");
      return;
    }
  }
  gslc_teTxtFlags eFlagsCur = pElem->eTxtFlags;
  pElem->eTxtFlags = (eFlagsCur & ~GSLC_TXT_MEM) | (eFlags & GSLC_TXT_MEM);
}

void gslc_ElemSetTxtEnc(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,gslc_teTxtFlags eFlags)
{
  gslc_tsElem* pElem = gslc_GetElemFromRefD(pGui, pElemRef, __LINE__);
  if (!pElem) return;

  gslc_teTxtFlags eFlagsCur = pElem->eTxtFlags;
  pElem->eTxtFlags = (eFlagsCur & ~GSLC_TXT_ENC) | (eFlags & GSLC_TXT_ENC);
}

void gslc_ElemUpdateFont(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,int nFontId)
{
  gslc_tsElem* pElem = gslc_GetElemFromRefD(pGui, pElemRef, __LINE__);
  if (!pElem) return;

  pElem->pTxtFont = gslc_FontGet(pGui,nFontId);
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_FULL);
}


void gslc_ElemSetRedraw(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,gslc_teRedrawType eRedraw)
{
  if (pElemRef == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemSetRedraw";
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return;
  }

  // Update the redraw flag
  gslc_teElemRefFlags eFlags = pElemRef->eElemFlags;
  gslc_tsElem* pElem = gslc_GetElemFromRefD(pGui, pElemRef, __LINE__);

  // Current redraw state in element flags
  gslc_teElemRefFlags eFlagsRedrawOld = (eFlags & GSLC_ELEMREF_REDRAW_MASK);

  // Current element flags with redraw state cleared
  gslc_teElemRefFlags eFlagsWoRedraw = (eFlags & ~GSLC_ELEMREF_REDRAW_MASK);

  if (eRedraw == GSLC_REDRAW_NONE) {
    // If a GSLC_REDRAW_NONE is assigned, override any existing
    // redraw state. This is often used to clear a redraw state
    // once a redraw has been completed.
    eFlags = eFlagsWoRedraw | GSLC_ELEMREF_REDRAW_NONE;
  } else {

    // Indicate that at least one element requires redraw
    pGui->bRedrawNeeded = true;

    // For all cases where a new redraw has been requested,
    // only update the redraw state if we are increasing
    // the level of redraw.
    switch (eRedraw) {
      case GSLC_REDRAW_FULL:
        eFlags = eFlagsWoRedraw | GSLC_ELEMREF_REDRAW_FULL;
        break;
      case GSLC_REDRAW_INC:
        if (eFlagsRedrawOld == GSLC_ELEMREF_REDRAW_FULL) {
          // Skip the update as a larger redraw was already requested
        } else {
          eFlags = eFlagsWoRedraw | GSLC_ELEMREF_REDRAW_INC;
        }
        break;
      case GSLC_REDRAW_FOCUS:
        if ((eFlagsRedrawOld == GSLC_ELEMREF_REDRAW_FULL) ||
            (eFlagsRedrawOld == GSLC_ELEMREF_REDRAW_INC)) {
          // Skip the update as a larger redraw was already requested
        } else {
          eFlags = eFlagsWoRedraw | GSLC_ELEMREF_REDRAW_FOCUS;
        }
        break;
      case GSLC_REDRAW_NONE:
        // Will not enter this case as it is trapped earlier
        break;
    }
    // Mark the region as invalidated
    // - Only invalidate if the element is visible on the screen
    if (gslc_ElemGetOnScreen(pGui,pElemRef)) {
      gslc_InvalidateRgnAdd(pGui, pElem->rElem);
    }
  }

  // Update the redraw state
  pElemRef->eElemFlags = eFlags;

#if (GSLC_FEATURE_COMPOUND)
  // In the case of compound elements, optionally propagate the
  // redraw status up the hierarchy (ie. to the parent element).
  // - This may be useful in scenarios wherein a sub-element has
  //   been marked for redraw, but because it has transparency,
  //   the parent element may need to be redrawn first.
  // - For now, assume no need to trigger a parent redraw.
  // - TODO: Consider detecting scenarios in which we should
  //   propagate the redraw to the parent.
  if (pElem->pElemRefParent != NULL) {
    gslc_ElemSetRedraw(pGui,pElem->pElemRefParent,eRedraw);
  }
#endif
}

gslc_teRedrawType gslc_ElemGetRedraw(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef)
{
  (void)pGui; // Unused
  if (pElemRef == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemGetRedraw";
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
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
    case GSLC_ELEMREF_REDRAW_FOCUS:
      return GSLC_REDRAW_FOCUS;
  }
  // Should not reach here
  return GSLC_REDRAW_NONE;
}

void gslc_ElemSetGlow(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,bool bGlowing)
{
  if (pElemRef == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemSetGlow";
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return;
  }
  // Only change glow state if enabled
  if (gslc_ElemGetGlowEn(pGui, pElemRef)) {
    bool bGlowingOld = gslc_ElemGetGlow(pGui, pElemRef);
    gslc_SetElemRefFlag(pGui, pElemRef, GSLC_ELEMREF_GLOWING, (bGlowing) ? GSLC_ELEMREF_GLOWING : 0);

    if (bGlowing != bGlowingOld) {
      gslc_ElemSetRedraw(pGui, pElemRef, GSLC_REDRAW_INC);
    }
  }
}

bool gslc_ElemGetGlow(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef)
{
  if (pElemRef == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemGetGlow";
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return false;
  }
  return gslc_GetElemRefFlag(pGui,pElemRef,GSLC_ELEMREF_GLOWING);
}

void gslc_ElemSetFocus(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,bool bFocused)
{
#if !(GSLC_FEATURE_INPUT)
  (void)pGui; // Unused
  (void)pElemRef; // Unused
  (void)bFocused; // Unused
  return;
#else
  if (pElemRef == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemSetFocus";
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return;
  }
  // Only change focus state if enabled
  if (gslc_ElemGetFocusEn(pGui, pElemRef)) {
    bool bFocusedOld = gslc_ElemGetFocus(pGui, pElemRef);
    gslc_SetElemRefFlag(pGui, pElemRef, GSLC_ELEMREF_FOCUSED, (bFocused) ? GSLC_ELEMREF_FOCUSED : 0);

    if (bFocused != bFocusedOld) {
      gslc_ElemSetRedraw(pGui, pElemRef, GSLC_REDRAW_FOCUS);
    }
  }
#endif // GSLC_FEATURE_INPUT
}

bool gslc_ElemGetFocus(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef)
{
#if !(GSLC_FEATURE_INPUT)
  (void)pGui; // Unused
  (void)pElemRef; // Unused
  return false;
#else
  if (pElemRef == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemGetFocus";
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return false;
  }
  return gslc_GetElemRefFlag(pGui,pElemRef,GSLC_ELEMREF_FOCUSED);
#endif // GSLC_FEATURE_INPUT
}

/// \todo Doc
bool gslc_ElemGetEditEn(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef)
{
#if !(GSLC_FEATURE_INPUT)
  (void)pGui; // Unused
  (void)pElemRef; // Unused
  return false;
#else
  gslc_tsElem* pElem = gslc_GetElemFromRefD(pGui, pElemRef, __LINE__);
  if (!pElem) return false;

  return pElem->nFeatures & GSLC_ELEM_FEA_EDIT_EN;
#endif // GSLC_FEATURE_INPUT
}


void gslc_ElemSetEdit(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,bool bEditing)
{
#if !(GSLC_FEATURE_INPUT)
  (void)pGui; // Unused
  (void)pElemRef; // Unused
  (void)bEditing; // Unused
  return;
#else
  if (pElemRef == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemSetEdit";
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return;
  }
  // Only change edit state if enabled
  gslc_tsElem* pElem = gslc_GetElemFromRef(pGui,pElemRef);
  if (pElem->nFeatures & GSLC_ELEM_FEA_EDIT_EN) {
    gslc_SetElemRefFlag(pGui, pElemRef, GSLC_ELEMREF_EDITING, (bEditing) ? GSLC_ELEMREF_EDITING : 0);
    gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_INC);
  }

  // Update the system edit mode flag
  if (bEditing) {
    if (pGui->nInputMode == GSLC_INPUTMODE_EDIT) {
      GSLC_DEBUG_PRINT("ERROR: ElemSetEdit(edit=1) when GUI already in edit mode\n","");
    } else {
      pGui->nInputMode = GSLC_INPUTMODE_EDIT;
    }
  } else {
    pGui->nInputMode = GSLC_INPUTMODE_NAV;
  }
#endif // GSLC_FEATURE_INPUT
}

bool gslc_ElemGetEdit(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef)
{
#if !(GSLC_FEATURE_INPUT)
  (void)pGui; // Unused
  (void)pElemRef; // Unused
  return false;
#else
  if (pElemRef == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemGetEdit";
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return false;
  }
  return gslc_GetElemRefFlag(pGui,pElemRef,GSLC_ELEMREF_EDITING);
#endif // GSLC_FEATURE_INPUT
}


void gslc_ElemSetVisible(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,bool bVisible)
{
  gslc_tsElem* pElem = gslc_GetElemFromRefD(pGui, pElemRef, __LINE__);
  if (!pElem) return;

  bool bVisibleOld = gslc_ElemGetVisible(pGui,pElemRef);
  gslc_SetElemRefFlag(pGui,pElemRef,GSLC_ELEMREF_VISIBLE,(bVisible)?GSLC_ELEMREF_VISIBLE:0);

  // Mark the element as needing redraw if its visibility status changed
  if (bVisible != bVisibleOld) {
    gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_FULL);
    if (bVisible == false) {
      // Since we are hiding an element, we need to invalidate
      // the region underneath the element, so that it can
      // be redrawn.
      gslc_InvalidateRgnAdd(pGui, pElem->rElem);
      // Mark the page as having a redraw pending
      gslc_PageRedrawSet(pGui,true);
     }
  }

}

bool gslc_ElemGetVisible(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef)
{
  if (pElemRef == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemGetVisible";
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return false;
  }
  return gslc_GetElemRefFlag(pGui,pElemRef,GSLC_ELEMREF_VISIBLE);
}

bool gslc_ElemGetOnScreen(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef)
{
  int16_t nElemId = gslc_ElemGetId(pGui, pElemRef);
  for (int8_t nStackPage = 0; nStackPage < GSLC_STACK__MAX; nStackPage++) {
    gslc_tsPage* pStackPage = pGui->apPageStack[nStackPage];
    if (!pStackPage) {
      continue;
    }
    gslc_tsCollect* pCollect = &pStackPage->sCollect;
    pElemRef = gslc_CollectFindElemById(pGui, pCollect, nElemId);
    if (pElemRef) {
      return gslc_ElemGetVisible(pGui, pElemRef);
    }
  }
  return false;
}

void gslc_ElemSetGlowEn(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,bool bGlowEn)
{
  gslc_tsElem* pElem = gslc_GetElemFromRefD(pGui, pElemRef, __LINE__);
  if (!pElem) return;

  if (bGlowEn) {
    pElem->nFeatures |= GSLC_ELEM_FEA_GLOW_EN;
  } else {
    pElem->nFeatures &= ~GSLC_ELEM_FEA_GLOW_EN;
  }
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_INC);
}

bool gslc_ElemGetGlowEn(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef)
{
  gslc_tsElem* pElem = gslc_GetElemFromRefD(pGui, pElemRef, __LINE__);
  if (!pElem) return false;

  return pElem->nFeatures & GSLC_ELEM_FEA_GLOW_EN;
}

/// \todo Doc
bool gslc_ElemGetFocusEn(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef)
{
#if !(GSLC_FEATURE_INPUT)
  (void)pGui; // Unused
  (void)pElemRef; // Unused
  return false;
#else
  gslc_tsElem* pElem = gslc_GetElemFromRefD(pGui, pElemRef, __LINE__);
  if (!pElem) return false;

  return pElem->nFeatures & GSLC_ELEM_FEA_FOCUS_EN;
#endif // GSLC_FEATURE_INPUT
}

/// \todo Doc
void gslc_ElemSetFocusEn(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,bool bFocusEn)
{
#if !(GSLC_FEATURE_INPUT)
  (void)pGui; // Unused
  (void)pElemRef; // Unused
  (void)bFocusEn; // Unused
#else
  gslc_tsElem* pElem = gslc_GetElemFromRefD(pGui, pElemRef, __LINE__);
  if (!pElem) return;

  // Capture current element sizing
  int8_t nResizeOld,nResizeNew;
  nResizeOld = gslc_ElemCalcResizeForFocus(pGui,pElemRef);

  // Update the attribute
  if (bFocusEn) {
    pElem->nFeatures |= GSLC_ELEM_FEA_FOCUS_EN;
  } else {
    pElem->nFeatures &= ~GSLC_ELEM_FEA_FOCUS_EN;
  }

  // Determine any change in sizing and update the element
  nResizeNew = gslc_ElemCalcResizeForFocus(pGui,pElemRef);
  gslc_ElemGrowRect(pGui,pElemRef,(nResizeNew-nResizeOld));

  // Mark for redraw
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_FULL);

#endif // GSLC_FEATURE_INPUT
}



void gslc_ElemSetClickEn(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,bool bClickEn)
{
  gslc_tsElem* pElem = gslc_GetElemFromRefD(pGui, pElemRef, __LINE__);
  if (!pElem) return;

  if (bClickEn) {
    pElem->nFeatures |= GSLC_ELEM_FEA_CLICK_EN;
  } else {
    pElem->nFeatures &= ~GSLC_ELEM_FEA_CLICK_EN;
  }

  #if (GSLC_FEATURE_INPUT)
  // For now, associate accepting click events as also accepting focus
  // We call this API since it also takes care of resizing 
  // element if necessary.
  gslc_ElemSetFocusEn(pGui,pElemRef,bClickEn);
  #endif // GSLC_FEATURE_INPUT

  // No need to call ElemSetRedraw() as we aren't changing a visual characteristic
}

void gslc_ElemSetTouchFunc(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, GSLC_CB_TOUCH funcCb)
{
  gslc_tsElem* pElem = gslc_GetElemFromRefD(pGui, pElemRef, __LINE__);
  if (!pElem) return;

  pElem->pfuncXTouch = funcCb;
}


void gslc_ElemSetStyleFrom(gslc_tsGui* pGui,gslc_tsElemRef* pElemRefSrc,gslc_tsElemRef* pElemRefDest)
{
  gslc_tsElem* pElemSrc = gslc_GetElemFromRefD(pGui, pElemRefSrc, __LINE__);
  gslc_tsElem* pElemDest = gslc_GetElemFromRefD(pGui, pElemRefDest, __LINE__);
  if ((!pElemSrc) || (!pElemDest)) return;

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
#if (GSLC_FEATURE_COMPOUND)
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
  pElemDest->nTxtMarginX      = pElemSrc->nTxtMarginX;
  pElemDest->nTxtMarginY      = pElemSrc->nTxtMarginY;
  pElemDest->pTxtFont         = pElemSrc->pTxtFont;

  // pXData

  //pElemDest->pfuncXEvent      = pElemSrc->pfuncXEvent; // UNUSED
  pElemDest->pfuncXDraw       = pElemSrc->pfuncXDraw;
  pElemDest->pfuncXTouch      = pElemSrc->pfuncXTouch;
  pElemDest->pfuncXTick       = pElemSrc->pfuncXTick;

  gslc_ElemSetRedraw(pGui,pElemRefDest,GSLC_REDRAW_FULL);
}

// Calculate the expansion required for the element
// rect to allow for focus and/or frame modes.
//
// In some cases, it may be necessary to expand the
// size of an element to accommodate additional levels
// of framing:
// - A Focus rect. If external inputs are enabled, a
//   focus rect needs to be drawn around the element,
//   meaning that expansion will be required.
// - A Frame rect. If a frame is requested and the
//   element has been marked as NoShrink (eg. image button),
//   then additional expansion is required.
int8_t gslc_ElemCalcResizeForFocus(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef)
{
  int8_t nDelta = 0;

  gslc_tsElem* pElem = gslc_GetElemFromRefD(pGui, pElemRef, __LINE__);
  uint16_t nFeatures = pElem->nFeatures;

  bool bFrameEn = nFeatures & GSLC_ELEM_FEA_FRAME_EN; // Does the element require a frame?
  bool bFocusEn = nFeatures & GSLC_ELEM_FEA_FOCUS_EN; // Does the element support focus state?
  bool bNoShrink = nFeatures & GSLC_ELEM_FEA_NOSHRINK; // Does the element disable shrinking?
  if (bFrameEn) {
    if (bNoShrink) {
      // If a frame is requested but the element doesn't support shrinking
      // of its contents (eg. because the entire element rect is occupied by
      // an image), then we must increase the element size to allow for a
      // frame on the outside.
      nDelta += 1;
    } else {
      // If a frame is requested, but the element can support shrinking,
      // then we use the original element size for the frame and then
      // perform all remaining drawing with a shrunk region inside the frame.
    }
  }
  if (bFocusEn) {
    // If a focus indicator is requested, we always increase the element
    // size to allow for a focus indicator on the outside. This may be
    // in addition to a frame.
    nDelta += 1;
  }

  return nDelta;
}

// Adjust the size of an element's rect by an incremental amount
// - This is generally used to adjust element dimensions when
//   enabling focus or frame modes on an element.
void gslc_ElemGrowRect(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, int8_t nDelta)
{
  // If we need to resize the element region, update it now
  if (nDelta != 0) {
    gslc_tsRect rElem = gslc_ElemGetRect(pGui,pElemRef);
    rElem = gslc_ExpandRect(rElem,nDelta,nDelta);
    gslc_ElemSetRect(pGui, pElemRef, rElem);
  }
}

void gslc_ResetRectState(gslc_tsRectState *pState)
{
  if (!pState) {
    return;
  }
  pState->rFocus = (gslc_tsRect){0,0,0,0};
  pState->rFull = (gslc_tsRect){0,0,0,0};
  pState->rInner = (gslc_tsRect){0,0,0,0};
  pState->colFocus = GSLC_COL_BLACK;
  pState->colFrm = GSLC_COL_BLACK;
  pState->colInner = GSLC_COL_BLACK;
  pState->colBack = GSLC_COL_BLACK;
  pState->colTxtFore = GSLC_COL_WHITE;
  pState->colTxtBack = GSLC_COL_BLACK;
};

void gslc_ElemCalcRectState(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, gslc_tsRectState* pState)
{
  gslc_tsElem* pElem = gslc_GetElemFromRefD(pGui, pElemRef, __LINE__);
  uint16_t nFeatures = pElem->nFeatures;
  gslc_tsRect rElem = pElem->rElem;

  bool bFillEn   = nFeatures & GSLC_ELEM_FEA_FILL_EN;
  bool bFrameEn  = nFeatures & GSLC_ELEM_FEA_FRAME_EN;
  bool bGlowEn   = nFeatures & GSLC_ELEM_FEA_GLOW_EN; // Does the element support glow state?
  bool bFocusEn  = nFeatures & GSLC_ELEM_FEA_FOCUS_EN; // Does the element support focus state?
  bool bGlowing  = bGlowEn && gslc_ElemGetGlow(pGui,pElemRef); // Element should be glowing (if enabled)
  bool bFocused  = bFocusEn && gslc_ElemGetFocus(pGui,pElemRef); // Element should be focused (if enabled)
  bool bEditing  = gslc_ElemGetEdit(pGui,pElemRef);

  // Initialize the return struct
  gslc_ResetRectState(pState);

  #if (GSLC_FEATURE_INPUT)
  // Calculate the regions
  if (bFocusEn) {
    // Focus enabled
    // - If the focus was enabled, then the element's rect already
    //   accounts for the focus frame (this expansion was done
    //   in gslc_ElemResizeForFocus)
    pState->rFocus = rElem;
    // Now adjust for all following rect calcs
    rElem = gslc_ExpandRect(rElem,-1,-1);
  }
  #endif // GSLC_FEATURE_INPUT

  if (bFrameEn) {
    // If a frame is enabled, shrink the inner region
    pState->rFull = rElem;
    pState->rInner = gslc_ExpandRect(rElem,-1,-1);
  } else {
    pState->rFull = rElem; // TODO: Should this be different?
    pState->rInner = rElem;
  }

  // Calculate the focus rect color
  #if (GSLC_FEATURE_INPUT)
  if (bEditing) {
    pState->colFocus = pGui->colFocusEdit;
  } else if (bFocused) {
    pState->colFocus = pGui->colFocus;
  } else {
    pState->colFocus = pGui->colFocusNone;
  }
  #endif // GSLC_FEATURE_INPUT

  // Calculate the element colors
  pState->colBack = pElem->colElemFill;
  if (bGlowing) {
    pState->colFrm = pElem->colElemFrameGlow;
    pState->colInner = pElem->colElemFillGlow;
    pState->colTxtFore = pElem->colElemTextGlow;
  } else {
    pState->colFrm = pElem->colElemFrame;
    pState->colInner = pElem->colElemFill;
    pState->colTxtFore = pElem->colElemText;
  }

  // Calculate text background color
  if (bFillEn) {
    pState->colTxtBack = pState->colInner;
  } else {
    // To indicate transparent mode, we set text and background to same color
    pState->colTxtBack = pState->colTxtFore;
  }

}



/* UNUSED
void gslc_ElemSetEventFunc(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,GSLC_CB_EVENT funcCb)
{
  gslc_tsElem* pElem = gslc_GetElemFromRefD(pGui, pElemRef, __LINE__);
  if (!pElem) return;

  pElem->pfuncXEvent       = funcCb;
}
*/


void gslc_ElemSetDrawFunc(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,GSLC_CB_DRAW funcCb)
{
  gslc_tsElem* pElem = gslc_GetElemFromRefD(pGui, pElemRef, __LINE__);
  if (!pElem) return;

  pElem->pfuncXDraw     = funcCb;
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_FULL);
}

void gslc_ElemSetTickFunc(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,GSLC_CB_TICK funcCb)
{
  gslc_tsElem* pElem = gslc_GetElemFromRefD(pGui, pElemRef, __LINE__);
  if (!pElem) return;

  pElem->pfuncXTick       = funcCb;
}

bool gslc_ElemOwnsCoord(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,int16_t nX,int16_t nY,bool bOnlyClickEn)
{
  // If no element was provided, return gracefully
  if (!pElemRef) return false;

  gslc_tsElem* pElem = gslc_GetElemFromRefD(pGui, pElemRef, __LINE__);
  if (!pElem) return false;

  if (bOnlyClickEn && !(pElem->nFeatures & GSLC_ELEM_FEA_CLICK_EN) ) {
    return false;
  }
  return gslc_IsInRect(nX,nY,pElem->rElem);
}

#if !defined(DRV_TOUCH_NONE)

// ------------------------------------------------------------------------
// Tracking Functions
// ------------------------------------------------------------------------


void gslc_CollectInput(gslc_tsGui* pGui,gslc_tsCollect* pCollect,gslc_tsEventTouch* pEventTouch)
{
  (void)pGui; // Unused
#if !(GSLC_FEATURE_INPUT)
  (void)pCollect; // Unused
  (void)pEventTouch; // Unused
  return;
#else
  if (pCollect == NULL) {
    return;
  }
  if (pEventTouch == NULL) {
    return;
  }
  // Fetch the data members of the touch event
  gslc_teTouch      eTouch    = pEventTouch->eTouch;
  int16_t           nInputInd = pEventTouch->nX;      // nX overloaded as element index
  int16_t           nInputVal = pEventTouch->nY;      // nY overloaded as action value
  gslc_tsElemRef*   pElemIndRef;

  if ((nInputInd >= 0) && (nInputInd < pCollect->nElemRefMax)) {
    pElemIndRef = &(pCollect->asElemRef[nInputInd]);
  } else {
    pElemIndRef = NULL;
  }

  gslc_tsElemRef*   pTrackedRefOld = NULL;
  gslc_tsElemRef*   pTrackedRefNew = NULL;

  // Pass zero to callbacks since no actual position
  int16_t           nX = 0;
  int16_t           nY = 0;

  // Fetch the item currently being tracked (if any)
  pTrackedRefOld = pGui->pFocusElemRef;

  // For direct input events, eTouch will already be fully defined with DOWN/UP + IN/OUT
  if (eTouch == GSLC_TOUCH_FOCUS_ON) {
    // ---------------------------------
    // Touch Down Event
    // ---------------------------------

    // End glow on previously tracked element (if any)
    // - We shouldn't really enter a "Touch Down" event
    //   with an element still marked as being tracked
    if (pTrackedRefOld != NULL) {
      gslc_ElemSetFocus(pGui,pTrackedRefOld,false);
    }

    // Determine the new element to start tracking
    pTrackedRefNew = pElemIndRef;

    if (pTrackedRefNew == NULL) {
      // Didn't find an element, so clear the tracking reference
      pGui->pFocusElemRef = NULL;
    } else {
      // Found an element, so mark it as being the tracked element

      // Set the new tracked element reference
      pGui->pFocusElemRef = pTrackedRefNew;
      if (pTrackedRefNew->pElem == NULL) {
        return;
      }

      // Start focus on new element
      gslc_ElemSetFocus(pGui,pTrackedRefNew,true);
    }

  } else if (eTouch == GSLC_TOUCH_FOCUS_OFF) {

    // ---------------------------------
    // Touch Up Event
    // ---------------------------------

    if (pTrackedRefOld != NULL) {

      // Clear focus state
      if (pTrackedRefOld->pElem == NULL) {
        return;
      }
      gslc_ElemSetFocus(pGui,pTrackedRefOld,false);
    }

    // Clear the element tracking state
    pGui->pFocusElemRef = NULL;

  } else if (eTouch == GSLC_TOUCH_FOCUS_PRESELECT) {
    // ---------------------------------
    // PRESELECT -> Glow before Select
    // ---------------------------------
    //xxx GSLC_DEBUG_PRINT("CollectInput: TOUCH_FOCUS_PRESELECT\n",""); //xxx

    if (pTrackedRefOld != NULL) {

      // Notify original tracked element for optional custom handling
      // If the element is not editable, then mimic a touch event,
      // otherwise, pass on the focus select event
      if (!gslc_ElemGetEditEn(pGui,pTrackedRefOld)) {
        // Start the glow state
        gslc_ElemSetGlow(pGui,pTrackedRefOld,true);
        eTouch = GSLC_TOUCH_DOWN_IN;
        // TODO: Do we really want to change the eTouch type here?
        //       - Perhaps this is the best way to reuse the existing touch handler in the element
        // Here we are mimicking a touch event within the element
        nX = 0; // Arbitrary
        nY = 0; // Arbitrary
      }
      gslc_ElemSendEventTouch(pGui,pTrackedRefOld,eTouch,nX,nY);

    }

  } else if (eTouch == GSLC_TOUCH_FOCUS_SELECT) {
    // ---------------------------------
    // SELECT -> Select or Touch Up Event
    // ---------------------------------

    if (pTrackedRefOld != NULL) {

      // Notify original tracked element for optional custom handling
      // If the element is not editable, then mimic a touch event,
      // otherwise, pass on the focus select event
      if (!gslc_ElemGetEditEn(pGui,pTrackedRefOld)) {
        eTouch = GSLC_TOUCH_UP_IN;
        // TODO: Do we really want to change the eTouch type here?
        //       - Perhaps this is the best way to reuse the existing touch handler in the element
        // Here we are mimicking a touch event within the element
        nX = 0; // Arbitrary
        nY = 0; // Arbitrary
      }
      gslc_ElemSendEventTouch(pGui,pTrackedRefOld,eTouch,nX,nY);

      // End the glow state
      gslc_ElemSetGlow(pGui,pTrackedRefOld,false);

    }

  } else if ((eTouch == GSLC_TOUCH_SET_REL) || (eTouch == GSLC_TOUCH_SET_ABS)) {
    // ---------------------------------
    // Element Value Adjust Event
    // ---------------------------------
    nX = 0; // Unused
    nY = nInputVal; // Relative or Absolute value
    if (pTrackedRefOld == NULL) {
      return; //xxx Ignore the SET_REL if nothing in focus!
    }
    gslc_ElemSendEventTouch(pGui,pTrackedRefOld,eTouch,nX,nY);
  }
#endif // GSLC_FEATURE_INPUT
}


void gslc_CollectTouch(gslc_tsGui* pGui,gslc_tsCollect* pCollect,gslc_tsEventTouch* pEventTouch)
{
  // Fetch the data members of the touch event
  int16_t       nX        = pEventTouch->nX;
  int16_t       nY        = pEventTouch->nY;
  gslc_teTouch  eTouch    = pEventTouch->eTouch;

  gslc_tsElemRef*   pTrackedRefOld = NULL;
  gslc_tsElemRef*   pTrackedRefNew = NULL;
  int16_t nTrackedIndNew = GSLC_IND_NONE;

  // Fetch the item currently being tracked (if any)
  pTrackedRefOld = gslc_CollectGetElemRefTracked(pGui,pCollect);



  // Reset the in-tracked flag
  bool  bInTracked = false;

  if (eTouch == GSLC_TOUCH_DOWN) {
    // ---------------------------------
    // Touch Down Event
    // ---------------------------------

    // Determine the new element to start tracking
    // - Also get the index of the element. This is saved into Collect->nElemIndTracked
    //   so that we can later support auto-focus upon TOUCH_UP_IN events.
    pTrackedRefNew = gslc_CollectFindElemFromCoord(pGui,pCollect,nX,nY,&nTrackedIndNew);

    // End glow on previously tracked element (if any)
    // - We shouldn't really enter a "Touch Down" event
    //   with an element still marked as being tracked
    if (pTrackedRefOld != NULL) {
      gslc_ElemSetGlow(pGui,pTrackedRefOld,false);
    }

    #if (GSLC_FEATURE_INPUT)
    // Handle any existing focus state
    if (pGui->pFocusElemRef != NULL) {
      // Upon a physical touch event:
      // - If we happen to be in a focus "edit" mode, then leave the
      //   edit mode and return to nav mode.
      gslc_ElemSetEdit(pGui,pGui->pFocusElemRef,false);

      bool bDoFocusOff = true;
      #if (GSLC_FEATURE_FOCUS_ON_TOUCH)
        // Since we will be focusing on an element as a result of a TOUCH_UP_IN
        // event, we don't want to de-focus and reapply focus if they are the
        // same element.
        // NOTE: If the TOUCH_DOWN_IN event turns into a TOUCH_UP_OUT case
        //       then it is OK since we will just retain the existing focus.
        if (pGui->pFocusElemRef == pTrackedRefNew) {
          bDoFocusOff = false;
        }
      #else
        // If we are not supporting FOCUS_ON_TOUCH, then we always
        // remove focus on TOUCH_DOWN events.
      #endif // FOCUS_ON_TOUCH

      if (bDoFocusOff) {
        void* pvFocusPage = (void*)(pGui->pFocusPage);

        gslc_tsEventTouch sEventTouchTmp;
        void*             pvData = (void*)(&sEventTouchTmp);
        gslc_tsEvent      sEventTmp;
        sEventTouchTmp.eTouch = GSLC_TOUCH_FOCUS_OFF;
        sEventTouchTmp.nX = GSLC_IND_NONE;
        sEventTouchTmp.nY = 0; // Unused

        sEventTmp = gslc_EventCreate(pGui,GSLC_EVT_TOUCH,0,pvFocusPage,pvData);
        gslc_PageEvent(pGui,sEventTmp);
      }
    }
    #endif // GSLC_FEATURE_INPUT

    if (pTrackedRefNew == NULL) {
      // Didn't find an element, so clear the tracking reference
      gslc_CollectSetElemTracked(pGui,pCollect,NULL,GSLC_IND_NONE);
    } else {
      // Found an element, so mark it as being the tracked element

      // Set the new tracked element reference
      gslc_CollectSetElemTracked(pGui,pCollect,pTrackedRefNew,nTrackedIndNew);

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

        #if (GSLC_FEATURE_INPUT)
        #if (GSLC_FEATURE_FOCUS_ON_TOUCH)
        // Now if we support INPUT mode, then we want to change the focus to be on this element
        // - This would facilitate the combination of touch and input events
        if (gslc_ElemGetFocusEn(pGui,pTrackedRefOld)) {

          gslc_FocusSetToTrackedElem(pGui,pCollect);

        }
        #endif // FOCUS_ON_TOUCH
        #endif // GSLC_FEATURE_INPUT

        // Notify original tracked element for optional custom handling
        eTouch = GSLC_TOUCH_UP_IN;
        gslc_ElemSendEventTouch(pGui,pTrackedRefOld,eTouch,nX,nY);
      } // bInTracked

      // Clear glow state
      gslc_ElemSetGlow(pGui,pTrackedRefOld,false);

    }

    gslc_CollectSetElemTracked(pGui,pCollect,NULL,GSLC_IND_NONE);


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
      } // bInTracked

    }

  }

}

#if (GSLC_FEATURE_COMPOUND)

// Helper function for dispatching touch events to sub-components
bool gslc_CollectTouchCompound(void* pvGui, void* pvElemRef, gslc_teTouch eTouch, int16_t nRelX, int16_t nRelY, gslc_tsCollect* pCollect)
{
  #if defined(DRV_TOUCH_NONE)
  return false;
  #else

  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem* pElem = gslc_GetElemFromRefD(pGui, pElemRef, __LINE__);
  if (!pElem) return false;

  // Reset the in/out status of the touch event since
  // we are now looking for coordinates of a sub-element
  // rather than the compound element. gslc_CollectTouch()
  // is responsible for determining in/out status at the
  // next level down in the element hierarchy.
  switch (eTouch) {
  case GSLC_TOUCH_DOWN_IN:
  case GSLC_TOUCH_DOWN_OUT:
    eTouch &= ~GSLC_TOUCH_SUBTYPE_MASK;
    eTouch |= GSLC_TOUCH_DOWN;
    break;
  case GSLC_TOUCH_UP_IN:
  case GSLC_TOUCH_UP_OUT:
    eTouch &= ~GSLC_TOUCH_SUBTYPE_MASK;
    eTouch |= GSLC_TOUCH_UP;
    break;
  case GSLC_TOUCH_MOVE_IN:
  case GSLC_TOUCH_MOVE_OUT:
    eTouch &= ~GSLC_TOUCH_SUBTYPE_MASK;
    eTouch |= GSLC_TOUCH_MOVE;
    break;
  default:
    break;
  }

  // Cascade the touch event to the sub-element collection
  // - Note that we convert to absolute coordinates
  gslc_tsEventTouch sEventTouch;
  sEventTouch.nX = pElem->rElem.x + nRelX;
  sEventTouch.nY = pElem->rElem.y + nRelY;
  sEventTouch.eTouch = eTouch;
  gslc_CollectTouch(pGui, pCollect, &sEventTouch);

  return true;
  #endif // !DRV_TOUCH_NONE
}

#endif // GSLC_FEATURE_COMPOUND


bool gslc_ElemCanFocus(gslc_tsGui* pGui,gslc_tsCollect* pCollect,int16_t nElemInd)
{
#if !(GSLC_FEATURE_INPUT)
  (void)pGui; // Unused
  (void)pCollect; // Unused
  (void)nElemInd; // Unused
  return false;
#else
  // Get focus capability attribute
  bool bCanFocus = false;
  gslc_tsElemRef* pElemRef = &(pCollect->asElemRef[nElemInd]);
  gslc_tsElem* pElem = gslc_GetElemFromRefD(pGui, pElemRef, __LINE__);
  if (pElemRef->eElemFlags != GSLC_ELEMREF_NONE) {
    if (pElem == NULL) {
      GSLC_DEBUG2_PRINT("ERROR: eElemFlags not none, but pElem is NULL%s\n","");
      exit(1); // FATAL FIXME
    } else {
      // Check the "accept focus" flag
      // TODO: Consider whether we should also qualify with the
      //       FRAME_EN flag. If the frame is not enabled, then
      //       a focus might not be shown.
      if (pElem->nFeatures & GSLC_ELEM_FEA_FOCUS_EN) {
        bCanFocus = true;
      }
    }
  }
  return bCanFocus;
#endif // GSLC_FEATURE_INPUT
}

// Return element reference of currently-focused element or NULL for none
// PRE:
// - pGui->pFocusPage
// - pGui->nFocusElemInd
gslc_tsElemRef* gslc_FocusElemGet(gslc_tsGui* pGui)
{
#if !(GSLC_FEATURE_INPUT)
  (void)pGui; // Unused
  return NULL;
#else
  gslc_tsPage* pFocusPage = pGui->pFocusPage;
  if (!pFocusPage) {
    return NULL;
  }
  // TODO: Error checking
  gslc_tsCollect* pCollect = &(pFocusPage->sCollect);
  gslc_tsElemRef* pElemRef = &(pCollect->asElemRef[pGui->nFocusElemInd]);
  return pElemRef;
#endif // GSLC_FEATURE_INPUT
}

// Update the focus to another element
// - Clears focus on existing element
// - Identifies default element if new page (nElemInd=GSLC_IND_NONE)
// - Sets focus on new element (if bFocus=true)
// - Updates tracking element reference
void gslc_FocusElemIndSet(gslc_tsGui* pGui,int16_t nPageInd,int16_t nElemInd,bool bFocus)
{
#if !(GSLC_FEATURE_INPUT)
  (void)pGui; // Unused
  (void)nPageInd; // Unused
  (void)nElemInd; // Unused
  (void)bFocus; // Unused
  return;
#else
  gslc_tsElemRef* pElemRef = NULL;

  // Unfocus existing element (if one is enabled)
  pElemRef = gslc_FocusElemGet(pGui);

  if (pElemRef) {
    gslc_ElemSetFocus(pGui,pElemRef,false);
    // Reset the tracking
    pGui->pFocusElemRef = NULL; //zzz
  }

  // Focus new element (if enabled) and set attributes
  if (bFocus) {
    pGui->nFocusPageInd = nPageInd;
    pGui->pFocusPage = pGui->apPageStack[nPageInd];
    if (pGui->pFocusPage == NULL) {
      // TODO: ERROR handling
      return;
    }
    pGui->nFocusElemInd = nElemInd;
    pGui->nFocusElemMax = pGui->pFocusPage->sCollect.nElemCnt;
    if (nElemInd == GSLC_IND_NONE) {
      // No element was specified, so assign default
      gslc_FocusElemStep(pGui,true);
    }
    // Fetch the focused element reference
    pElemRef = gslc_FocusElemGet(pGui);
    if (pElemRef) {
      // Mark it as focused
      gslc_ElemSetFocus(pGui,pElemRef,true);
      // Update the tracking reference
      pGui->pFocusElemRef = pElemRef; //zzz
    }
  } else {
    // Reset the focus state
    pGui->nFocusPageInd = GSLC_IND_NONE;
    pGui->pFocusPage = NULL;
    pGui->nFocusElemInd = GSLC_IND_NONE;
    pGui->nFocusElemMax = 0;
    pGui->pFocusElemRef = NULL;
  }
#endif // GSLC_FEATURE_INPUT
}

// Set the focus to the element indicated
// - We need to determine nFocusPageInd as the collection
//   does not currently include the page index
// - Note that it may be possible that the requested element
//   is no longer visible on an active page in the stack. In that
//   case, no focus will be updated.
void gslc_FocusSetToTrackedElem(gslc_tsGui* pGui,gslc_tsCollect* pCollect)
{
#if !(GSLC_FEATURE_INPUT)
  return;
#else
  int16_t nInd = GSLC_IND_NONE;
  int16_t nElemIndMax = 0;
  gslc_tsPage* pTmpPage = NULL;
  int16_t nFoundFocusPageInd = GSLC_IND_NONE;
  gslc_tsPage* pFoundFocusPage = NULL;
  bool bFound;

  nElemIndMax = pCollect->nElemCnt;

  // Find the page that matches the collection
  bFound = false;
  for (nInd=0;((!bFound)&&(nInd<GSLC_STACK__MAX));nInd++) {
    pTmpPage = pGui->apPageStack[nInd];
    if (&(pTmpPage->sCollect) == pCollect) {
      nFoundFocusPageInd = nInd;
      pFoundFocusPage = pTmpPage;
      bFound = true;
    }
  }
  if (!bFound) {
    // We could not find an active page associated with the
    // collection, so we skip the remainder of the updates. This may happen
    // if the original element is no longer visible on a page.

    // Don't proceed to look for the element
    return;
  }

  // Set the focus to the tracked element
  pGui->nFocusPageInd = nFoundFocusPageInd;
  pGui->pFocusPage = pFoundFocusPage;
  pGui->nFocusElemInd = pCollect->nElemIndTracked;
  pGui->nFocusElemMax = nElemIndMax;
  pGui->pFocusElemRef = pCollect->pElemRefTracked;

  gslc_ElemSetFocus(pGui,pGui->pFocusElemRef,true);
#endif // GSLC_FEATURE_INPUT
}

// Advance to the next page in the stack that can accept a focus event
// POST:
// - nFocusPageInd
// - pFocusPage
// - nFocusElemInd
// - nFocusElemMax
void gslc_FocusPageStep(gslc_tsGui* pGui,bool bNext)
{
#if !(GSLC_FEATURE_INPUT)
  (void)pGui; // Unused
  (void)bNext; // Unused
  return;
#else
  bool bFound = false;
  bool bDone = false;
  uint8_t nInd = 0;
  gslc_tsPage* pPage = NULL;
  int16_t nFocusPageInd = pGui->nFocusPageInd;

  while (!bDone) {

    // Keep track of iterations to detect search exhaustion
    nInd++;

    if (nFocusPageInd == GSLC_IND_NONE) {
      // Provide default
      nFocusPageInd = (bNext)? 0 : GSLC_STACK__MAX-1;
    } else {
      if (bNext) {
        nFocusPageInd = (nFocusPageInd >= GSLC_STACK__MAX-1)? 0 : nFocusPageInd+1;
      } else {
        nFocusPageInd = (nFocusPageInd <= 0)? GSLC_STACK__MAX-1 : nFocusPageInd-1;
      }
    }
    // Is this stack layer valid?
    pPage = pGui->apPageStack[nFocusPageInd];
    if (pPage) {
      // Ensure the page layer is active (receiving touch events)
      if (pGui->abPageStackActive[nFocusPageInd]) {
        bFound = true;
        bDone = true;
      }
    }
    // Did we exhaust our search?
    if (nInd >= GSLC_STACK__MAX) {
      bDone = true;
    }
  }

  if (bFound) {
    pGui->pFocusPage = pPage;
    pGui->nFocusPageInd = nFocusPageInd;
    pGui->nFocusElemMax = pPage->sCollect.nElemCnt;
    pGui->nFocusElemInd = (bNext)? 0 : pGui->nFocusElemMax-1;
  } else {
    pGui->pFocusPage = NULL;
    pGui->nFocusPageInd = GSLC_IND_NONE;
    pGui->nFocusElemMax = 0;
    pGui->nFocusElemInd = GSLC_IND_NONE;
  }
#endif // GSLC_FEATURE_INPUT
}


// Advance the focus to the next/previous element
// Reaching the end/start of the elements on the current page
// will cause another page to be selected for focus
// If nFocusElemInd is GSLC_IND_NONE, then start search at default element on page
// If no element is found, pFocusElem
//
// TODO: Cache local vars to avoid excess pGui indirection
//
// POST:
// - nFocusPageInd  (via FocusPageStep)
// - pFocusPage     (via FocusPageStep)
// - nFocusElemInd
// - nFocusElemMax  (via FocusPageStep)
int16_t gslc_FocusElemStep(gslc_tsGui* pGui,bool bNext)
{
#if !(GSLC_FEATURE_INPUT)
  (void)pGui; // Unused
  (void)bNext; // Unused
  return GSLC_IND_NONE;
#else
  // Ensure we initialize the focused page
  if (pGui->pFocusPage == NULL) {
    gslc_FocusPageStep(pGui,bNext);
  }

  // If no element has been focused yet, provide a default
  if (pGui->nFocusElemInd == GSLC_IND_NONE) {
    pGui->nFocusElemInd = (bNext)? 0 : pGui->nFocusElemMax-1;
  }

  // Save starting element to detect search exhaustion
  gslc_tsPage* pStartPage = pGui->pFocusPage;
  int16_t nStartElemInd = pGui->nFocusElemInd;

  // Start search
  bool bFound = false;
  bool bDone = false;
  while (!bDone) {
    if (bNext) {
      if (pGui->nFocusElemInd >= pGui->nFocusElemMax-1) {
        // At end of element list, so increment page (also reset element index)
        gslc_FocusPageStep(pGui,bNext);
      } else {
        // Increment element position
        pGui->nFocusElemInd++;
      }
    } else {
      if (pGui->nFocusElemInd == 0) {
        // At start of element list, so decrement page (also reset element index)
        gslc_FocusPageStep(pGui,bNext);
      } else {
        // Decrement element position
        pGui->nFocusElemInd--;
      }
    } // bNext

    // Check if element can be focused
    // TODO: simplify the params
    if (gslc_ElemCanFocus(pGui,&(pGui->pFocusPage->sCollect),pGui->nFocusElemInd)) {
      bDone = true;
      bFound = true;
    }

    // Check if exhausted search across all pages in stack
    // - If we have performed a search and still didn't locate a suitable
    //   next element by the time we loop back to the starting element
    //   then terminate the search.
    // - If the starting element "can focus" then we return that,
    //   otherwise we indicate no match (GSLC_IND_NONE)
    if ((pGui->pFocusPage == pStartPage) && (pGui->nFocusElemInd == nStartElemInd)) {
      bDone = true;
      if (!bFound) {
        // Even after wrapping, we haven't found a match, so indicate "no match"
        pGui->nFocusElemInd = GSLC_IND_NONE;
      }
    }

  } // bDone

  // FIXME: What should we return and what params should be updated?
  return pGui->nFocusElemInd;
#endif // GSLC_FEATURE_INPUT
}

void gslc_TrackInput(gslc_tsGui* pGui,gslc_teInputRawEvent eInputEvent,int16_t nInputVal)
{
#if !(GSLC_FEATURE_INPUT)
  (void)pGui; // Unused
  (void)eInputEvent; // Unused
  (void)nInputVal; // Unused
  GSLC_DEBUG_PRINT("WARNING: GSLC_FEATURE_INPUT not enabled in GUIslice config%s\n", "");
  return;
#else
  if (pGui == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "TrackInput";
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return;
  }

  gslc_tsEventTouch sEventTouch;
  void*             pvData = (void*)(&sEventTouch);
  gslc_tsEvent      sEvent;

  gslc_teAction     eAction = GSLC_ACTION_NONE;
  int16_t           nActionVal;


  if (pGui->nFocusPageInd == GSLC_IND_NONE) {
    // If we haven't initialized our starting page, do it now
    // This assigns pFocusPage
    gslc_FocusPageStep(pGui,true);
  }

  void* pvFocusPage = (void*)(pGui->pFocusPage);
  if (!pvFocusPage) {
    // No pages enabled in the stack, so exit
    return;
  }

  gslc_InputMapLookup(pGui,eInputEvent,nInputVal,&eAction,&nActionVal);

  uint8_t nInputMode = pGui->nInputMode;

  switch(eAction) {
    case GSLC_ACTION_FOCUS_PREV:
    case GSLC_ACTION_FOCUS_NEXT:

      if (nInputMode == GSLC_INPUTMODE_EDIT) {
        // Edit mode
        // - NOTE: It is assumed that in edit mode an element is already in focus
        sEventTouch.eTouch = GSLC_TOUCH_SET_REL;
        sEventTouch.nX = 0; // Unused
        sEventTouch.nY = (eAction == GSLC_ACTION_FOCUS_PREV)? -1 : 1;
        sEvent = gslc_EventCreate(pGui,GSLC_EVT_TOUCH,0,pvFocusPage,pvData);
        gslc_PageEvent(pGui,sEvent);
      } else {
        // Navigate mode

        // Unfocus old element
        sEventTouch.eTouch = GSLC_TOUCH_FOCUS_OFF;
        sEventTouch.nX = GSLC_IND_NONE;
        sEventTouch.nY = 0; // Unused

        sEvent = gslc_EventCreate(pGui,GSLC_EVT_TOUCH,0,pvFocusPage,pvData);
        gslc_PageEvent(pGui,sEvent);

        // Focus on new element
        bool bStepNext = (eAction == GSLC_ACTION_FOCUS_NEXT);

        gslc_FocusElemStep(pGui,bStepNext);
        pvFocusPage = (void*)(pGui->pFocusPage); // Focus page may have changed

        if (pGui->nFocusElemInd == GSLC_IND_NONE) {
          break;
        }
        sEventTouch.eTouch = GSLC_TOUCH_FOCUS_ON;
        sEventTouch.nX = pGui->nFocusElemInd;
        sEventTouch.nY = 0; // Unused
        sEvent = gslc_EventCreate(pGui,GSLC_EVT_TOUCH,0,pvFocusPage,pvData);
        gslc_PageEvent(pGui,sEvent);

      } // nInputMode

      break;

    case GSLC_ACTION_PRESELECT:
      // TODO: Merge with GSLC_ACTION_SELECT

      // Ensure an element is in focus!
      if (pGui->nFocusElemInd == GSLC_IND_NONE) {
        break;
      } else {

        // Select currently focused element
        sEventTouch.eTouch = GSLC_TOUCH_FOCUS_PRESELECT;
        sEventTouch.nX = pGui->nFocusElemInd;
        sEventTouch.nY = 0; // Unused
        sEvent = gslc_EventCreate(pGui,GSLC_EVT_TOUCH,0,pvFocusPage,pvData);
        gslc_PageEvent(pGui,sEvent);

      }

      break;

    case GSLC_ACTION_SELECT:

      // Ensure an element is in focus!
      if (pGui->nFocusElemInd == GSLC_IND_NONE) {
        break;
      } else {
        // Select currently focused element
        sEventTouch.eTouch = GSLC_TOUCH_FOCUS_SELECT;
        sEventTouch.nX = pGui->nFocusElemInd;
        sEventTouch.nY = 0; // Unused
        sEvent = gslc_EventCreate(pGui,GSLC_EVT_TOUCH,0,pvFocusPage,pvData);
        gslc_PageEvent(pGui,sEvent);
      }

      break;

    case GSLC_ACTION_UNDEF:
      // ERROR: unknown action
      break;

    default:
      // No action mapped
      break;
  }
#endif // GSLC_FEATURE_INPUT
}

// This routine is responsible for the GUI-level touch event state machine
// and dispatching to the touch event handler for the page
// FIXME: pPage UNUSED with new PageStack implementation
void gslc_TrackTouch(gslc_tsGui* pGui,gslc_tsPage* pPage,int16_t nX,int16_t nY,uint16_t nPress)
{
  (void)pPage; // Unused
  if (pGui == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "TrackTouch";
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
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
    GSLC_DEBUG_PRINT("Trk: (%3d,%3d) P=%3u : TouchDown\n\n",nX,nY,nPress);
    #endif
  } else if ((pGui->nTouchLastPress > 0) && (nPress == 0)) {
    eTouch = GSLC_TOUCH_UP;
    #ifdef DBG_TOUCH
    GSLC_DEBUG_PRINT("Trk: (%3d,%3d) P=%3u : TouchUp\n\n",nX,nY,nPress);
    #endif

  } else if ((pGui->nTouchLastX != nX) || (pGui->nTouchLastY != nY)) {
    // We only track movement if touch is "down"
    if (nPress > 0) {
      eTouch = GSLC_TOUCH_MOVE;
      #ifdef DBG_TOUCH
      GSLC_DEBUG_PRINT("Trk: (%3d,%3d) P=%3u : TouchMove\n\n",nX,nY,nPress);
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
  gslc_tsEvent sEvent;

  // Generate touch page event for any enabled pages in the stack
  for (unsigned nStack = 0; nStack < GSLC_STACK__MAX; nStack++) {
    gslc_tsPage* pStackPage = pGui->apPageStack[nStack];
    if (pStackPage) {
      // Ensure the page layer is active (receiving touch events)
      if (pGui->abPageStackActive[nStack]) {
        sEvent = gslc_EventCreate(pGui, GSLC_EVT_TOUCH, 0, (void*)pStackPage, pvData);
        gslc_PageEvent(pGui, sEvent);
        // FIXME: Should we be aborting this loop upon finding the
        // first page / element that accepts the hit?
      }
    }
  }


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
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
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
    GSLC_DEBUG2_PRINT("ERROR: InitTouch() failed in touch driver init\n",0);
  }
  return bOk;
}


bool gslc_GetTouch(gslc_tsGui* pGui,int16_t* pnX,int16_t* pnY,uint16_t* pnPress,gslc_teInputRawEvent* peInputEvent,int16_t* pnInputVal)
{
  if (pGui == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "GetTouch";
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return false;
  }

  *peInputEvent = GSLC_INPUT_NONE;
  *pnInputVal   = 0;

#if defined(DRV_TOUCH_NONE)
  // Touch handling disabled
  return false;
#elif defined(DRV_TOUCH_IN_DISP)
  // Use display driver for touch events
  return gslc_DrvGetTouch(pGui,pnX,pnY,pnPress,peInputEvent,pnInputVal);
#else
  // Use external touch driver for touch events
  return gslc_TDrvGetTouch(pGui,pnX,pnY,pnPress,peInputEvent,pnInputVal);
#endif

  return false;
}


void gslc_SetTouchRemapEn(gslc_tsGui* pGui, bool bEn)
{
  if (pGui == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "SetTouchRemapEn";
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL, FUNCSTR);
    return;
  }
  pGui->bTouchRemapEn = bEn;
}

void gslc_SetTouchRemapCal(gslc_tsGui* pGui,uint16_t nXMin, uint16_t nXMax, uint16_t nYMin, uint16_t nYMax)
{
  if (pGui == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "SetTouchRemapCal";
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL, FUNCSTR);
    return;
  }
  pGui->nTouchCalXMin = nXMin;
  pGui->nTouchCalXMax = nXMax;
  pGui->nTouchCalYMin = nYMin;
  pGui->nTouchCalYMax = nYMax;
}

void gslc_SetTouchPressCal(gslc_tsGui* pGui,uint16_t nPressMin, uint16_t nPressMax)
{
  if (pGui == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "SetTouchPressCal";
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL, FUNCSTR);
    return;
  }
  pGui->nTouchCalPressMin = nPressMin;
  pGui->nTouchCalPressMax = nPressMax;
}

void gslc_SetTouchRemapYX(gslc_tsGui* pGui, bool bSwap)
{
  if (pGui == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "SetTouchRemapYX";
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL, FUNCSTR);
    return;
  }
  pGui->bTouchRemapYX = bSwap;
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
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
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
      GSLC_DEBUG2_PRINT("ERROR: ElemCreate(%s) doesn't support temp elements with auto ID\n","");
      return sElem;
    }
#endif
  } else {
    // Look up the targeted page to ensure that we check its
    // collection for collision with other IDs (or assign the
    // next available if auto-incremented)
    pPage = gslc_PageFindById(pGui,nPageId);
    if (pPage == NULL) {
      GSLC_DEBUG2_PRINT("ERROR: ElemCreate() can't find page (ID=%d)\n",nPageId);
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
        GSLC_DEBUG2_PRINT("ERROR: ElemCreate() called with negative ID (%d)\n",nElemId);
        return sElem;
      }
      // Ensure the ID isn't already taken
      if (gslc_CollectFindElemById(pGui,pCollect,nElemId) != NULL) {
        GSLC_DEBUG2_PRINT("ERROR: ElemCreate() called with existing ID (%d)\n",nElemId);
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

      // Limit string length to minimum of either the internal buffer (GSLC_LOCAL_STR_LEN)
      // or the max length provided by the user (nStrBufMax)
      int8_t nBufMax;
      if (nStrBufMax == 0) {
        nBufMax = GSLC_LOCAL_STR_LEN;
      } else {
        nBufMax = GSLC_MIN(GSLC_LOCAL_STR_LEN,nStrBufMax);
      }
      gslc_StrCopy(sElem.pStrBuf,pStrBuf,nBufMax);
      sElem.nStrBufMax = nBufMax;
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
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return false;
  }
  void*           pvScope   = sEvent.pvScope;
  gslc_tsCollect* pCollect  = (gslc_tsCollect*)(pvScope);

  unsigned        nInd;
  gslc_tsElemRef* pElemRef = NULL;

  if (pCollect == NULL) {
    return false; //xxx
  }

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
    gslc_tsEventTouch*  pEventTouch = (gslc_tsEventTouch*)(pvData);
    gslc_teTouch        eTouch      = pEventTouch->eTouch;

    if (pvData == NULL) {
      return false;
    }

    if ((eTouch & GSLC_TOUCH_TYPE_MASK) == GSLC_TOUCH_COORD) {
      gslc_CollectTouch(pGui,pCollect,pEventTouch);
    } else if ((eTouch & GSLC_TOUCH_TYPE_MASK) == GSLC_TOUCH_DIRECT) {
      gslc_CollectInput(pGui,pCollect,pEventTouch);
    } else {
      // FAIL
    }
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
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return NULL;
  }

  if (pCollect->nElemRefCnt+1 > (pCollect->nElemRefMax)) {
    GSLC_DEBUG2_PRINT("ERROR: CollectElemAdd() too many element references (%d/%d)\n",
            pCollect->nElemRefCnt+1,pCollect->nElemRefMax);
    return NULL;
  }

  // TODO: Determine way of reporting compound elements in debug messages

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

    #if defined(DBG_LOG)
    GSLC_DEBUG_PRINT("INFO:   Add elem to collection: ElemRef=%d/%d, ElemRAM=%d/%d, ElemId=%u (RAM)\n",
            pCollect->nElemRefCnt+1,pCollect->nElemRefMax,pCollect->nElemCnt+1,pCollect->nElemMax,pElem->nId);
    #endif

    // Ensure we have enough space in internal element array
    if (pCollect->nElemCnt+1 > (pCollect->nElemMax)) {
      GSLC_DEBUG2_PRINT("ERROR: CollectElemAdd() too many RAM elements (%d/%d), ElemID=%d\n",
              pCollect->nElemCnt+1,pCollect->nElemMax,pElem->nId);
      // TODO: Implement a function that returns the current page's ID so that
      //       users can more easily identify the problematic page.
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

    // Fetch a RAM copy of the FLASH element
    #if (GSLC_USE_PROGMEM)
    memcpy_P(&pGui->sElemTmpProg,pElem,sizeof(gslc_tsElem));
    #endif

    #if defined(DBG_LOG)
      const gslc_tsElem* pElemRam = pElem; // Local element in RAM
      #if (GSLC_USE_PROGMEM)
        pElemRam = &pGui->sElemTmpProg;
      #endif
    GSLC_DEBUG_PRINT("INFO:   Add elem to collection: ElemRef=%d/%d, ElemId=%u (FLASH)\n",
            pCollect->nElemRefCnt+1,pCollect->nElemRefMax,pElemRam->nId);
    #endif

    // Add a reference
    nElemRefInd = pCollect->nElemRefCnt;
    pCollect->asElemRef[nElemRefInd].eElemFlags = eFlags;
    pCollect->asElemRef[nElemRefInd].pElem = (gslc_tsElem*)pElem;  // Typecast to drop const modifier
    pCollect->nElemRefCnt++;
  }

  // Fetch a pointer to the element reference array entry
  gslc_tsElemRef* pElemRef = &(pCollect->asElemRef[nElemRefInd]);

  // Mark any newly added element as requiring redraw
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_FULL);

  // Return the new element reference
  return pElemRef;
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
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return NULL;
  }

  if (nPageId == GSLC_PAGE_NONE) {
    // No page ID was provided, so skip add
    GSLC_DEBUG2_PRINT("ERROR: ElemAdd(%s) with PAGE_NONE\n","");
    return NULL;
  }

  // Fetch the page containing the item
  gslc_tsPage* pPage = gslc_PageFindById(pGui,nPageId);
  if (pPage == NULL) {
    GSLC_DEBUG2_PRINT("ERROR: ElemAdd() page (ID=%d) was not found\n",nPageId);
    return NULL;
  }

  #if defined(DBG_LOG)
  // TODO: Determine way of reporting compound elements in debug messages
  GSLC_DEBUG_PRINT("INFO: Add elem to PageID=%d: ElemID=%d Rect=(X=%d,Y=%d,W=%d,H=%d)\n",
    nPageId,pElem->nId,pElem->rElem.x,pElem->rElem.y,pElem->rElem.w,pElem->rElem.h);
  #endif

  gslc_tsCollect* pCollect = &pPage->sCollect;
  gslc_tsElemRef* pElemRefAdd = gslc_CollectElemAdd(pGui,pCollect,pElem,eFlags);

  // Determine any change in sizing and update the element
  int8_t nResizeNew = gslc_ElemCalcResizeForFocus(pGui,pElemRefAdd);
  gslc_ElemGrowRect(pGui,pElemRefAdd,(nResizeNew-0));

  // Fetch access to the element from the reference
  // - This also handles the case with elements in FLASH
  gslc_tsElem* pElemLocal = NULL;
  pElemLocal = gslc_GetElemFromRef(pGui,pElemRefAdd);

  // Update the page's bounding rect
  gslc_UnionRect(&(pPage->rBounds), pElemLocal->rElem);

  return pElemRefAdd;
}

gslc_tsRect gslc_GetClipRect(gslc_tsGui* pGui)
{
  gslc_tsDriver* pDriver = (gslc_tsDriver*)(pGui->pvDriver);
  return pDriver->rClipRect;
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

// Determine if an image has changed
// - This could be useful to detect whether an image button
//   needs to be redrawn.
bool gslc_ImgRefEqual(gslc_tsImgRef* pImgRef1,gslc_tsImgRef* pImgRef2)
{
  bool bSame = true;
  if (pImgRef1->pImgBuf != pImgRef2->pImgBuf) { bSame = false; }
  if (strcmp(pImgRef1->pFname,pImgRef2->pFname)!=0) { bSame = false; }
  if (pImgRef1->eImgFlags != pImgRef2->eImgFlags) { bSame = false; }
  if (pImgRef1->pvImgRaw != pImgRef2->pvImgRaw) { bSame = false; }
  return bSame;
}

void gslc_ElemSetImage(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,gslc_tsImgRef sImgRef,
  gslc_tsImgRef sImgRefSel)
{
  gslc_tsElem* pElem = gslc_GetElemFromRefD(pGui, pElemRef, __LINE__);
  if (!pElem) return;

  // Determine if a redraw will be needed
  #if (1) // Disable check for now
    // Always perform a redraw
    bool bImagesEqual = false;
  #else
    // Detect to see if a redraw is needed
    bool bImagesEqual = true;
    bImagesEqual &= gslc_ImgRefEqual(&(pElem->sImgRefNorm),&sImgRef);
    bImagesEqual &= gslc_ImgRefEqual(&(pElem->sImgRefGlow),&sImgRefSel);
  #endif
  
  // Update the normal and glowing images
  gslc_DrvSetElemImageNorm(pGui,pElem,sImgRef);
  gslc_DrvSetElemImageGlow(pGui,pElem,sImgRefSel);

  // Mark as needing redraw only if the image has changed
  if (!bImagesEqual) {
    gslc_ElemSetRedraw(pGui, pElemRef, GSLC_REDRAW_FULL);
  }
}

bool gslc_SetBkgndImage(gslc_tsGui* pGui,gslc_tsImgRef sImgRef)
{
  if (!gslc_DrvSetBkgndImage(pGui,sImgRef)) {
    return false;
  }
  gslc_InvalidateRgnScreen(pGui);
  gslc_PageFlipSet(pGui,true);
  return true;
}

bool gslc_SetBkgndColor(gslc_tsGui* pGui,gslc_tsColor nCol)
{
  if (!gslc_DrvSetBkgndColor(pGui,nCol)) {
    return false;
  }
  gslc_InvalidateRgnScreen(pGui);
  gslc_PageFlipSet(pGui,true);
  return true;
}

bool gslc_SetTransparentColor(gslc_tsGui* pGui, gslc_tsColor nCol)
{
  pGui->sTransCol = nCol;
  return true;
}

bool gslc_GuiRotate(gslc_tsGui* pGui, uint8_t nRotation)
{
  // Simple wrapper for driver-specific rotation
  bool bOk = gslc_DrvRotate(pGui,nRotation);

  // Invalidate the new screen dimensions
  gslc_InvalidateRgnScreen(pGui);

  return bOk;
}

// Trigger a touch event on an element
// - These events are put into the deferred state to be caught on the next update loop
// - If there is already a deferred event waiting, then it is issued immediately.
bool gslc_ElemSendEventTouch(gslc_tsGui* pGui,gslc_tsElemRef* pElemRefTracked,
        gslc_teTouch eTouch,int16_t nX,int16_t nY)
{
#if defined(DRV_TOUCH_NONE)
  return false;
#else

  if (pElemRefTracked == NULL) {
    return false; //FIXME: Need to add this check!
  }

  // Check to see if the deferred event queue is available
  if (pGui->bEventPending) {
    // If the pending slot already has an event, then
    // service it now and then save the new event into
    // the pending slot.
    gslc_ElemEvent((void*)pGui,pGui->sEventPend);
  }
  // Now we can save the new event into the pending slot
  pGui->sEventTouchPend.eTouch = eTouch;
  pGui->sEventTouchPend.nX = nX;
  pGui->sEventTouchPend.nY = nY;
  pGui->sEventPend = gslc_EventCreate(pGui,GSLC_EVT_TOUCH,0,(void*)pElemRefTracked,&(pGui->sEventTouchPend));
  pGui->bEventPending = true;

  return true;
#endif // !DRV_TOUCH_NONE
}

// Initialize the element struct to all zeros
void gslc_ResetElem(gslc_tsElem* pElem)
{
  if (pElem == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "ResetElem";
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
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
    pElem->nStrBufMax       = GSLC_LOCAL_STR_LEN;
  #else
    pElem->pStrBuf          = NULL;
    pElem->nStrBufMax       = 0;
  #endif
  pElem->colElemText      = GSLC_COL_WHITE;
  pElem->colElemTextGlow  = GSLC_COL_WHITE;
  pElem->eTxtAlign        = GSLC_ALIGN_MID_MID;
  pElem->nTxtMarginX      = 0;
  pElem->nTxtMarginY      = 0;
  pElem->pTxtFont         = NULL;

  pElem->pXData           = NULL;
  pElem->pfuncXEvent      = NULL; // UNUSED
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
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return;
  }
  pFont->nId            = GSLC_FONT_NONE;
  pFont->eFontRefType   = GSLC_FONTREF_FNAME;
  pFont->eFontRefMode   = GSLC_FONTREF_MODE_DEFAULT;
  pFont->pvFont         = NULL;
  pFont->nSize          = 0;
}


// Close down an element
void gslc_ElemDestruct(gslc_tsElem* pElem)
{
  if (pElem == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemDestruct";
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
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
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
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
  pCollect->nElemRefCnt = 0;
  pCollect->nElemCnt = 0;

}

// Close down all in page
void gslc_PageDestruct(gslc_tsGui* pGui,gslc_tsPage* pPage)
{
  if (pPage == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "PageDestruct";
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
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
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return;
  }
  // Loop through all pages in GUI
  uint8_t nPageInd;
  gslc_tsPage*  pPage = NULL;
  for (nPageInd=0;nPageInd<pGui->nPageCnt;nPageInd++) {
    pPage = &pGui->asPage[nPageInd];
    gslc_PageDestruct(pGui,pPage);
  }
  pGui->nPageCnt = 0;

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
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return;
  }

  pCollect->nElemMax          = nElemMax;
  pCollect->nElemCnt          = 0;

  pCollect->nElemAutoIdNext   = GSLC_ID_AUTO_BASE;

  //pCollect->pfuncXEvent       = NULL; // UNUSED

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

  // Reset touch / input tracking
  pCollect->pElemRefTracked = NULL;
  pCollect->nElemIndTracked = GSLC_IND_NONE;
}


// Search internal element array for one with a particular ID
gslc_tsElemRef* gslc_CollectFindElemById(gslc_tsGui* pGui,gslc_tsCollect* pCollect,int16_t nElemId)
{
  if (pCollect == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "CollectFindElemById";
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return NULL;
  }
  gslc_tsElem*      pElem = NULL;
  gslc_tsElemRef*   pElemRef = NULL;
  gslc_tsElemRef*   pFoundElemRef = NULL;
  uint16_t          nInd;

  if (nElemId == GSLC_ID_TEMP) {
    // ERROR: Don't expect to do this
    GSLC_DEBUG2_PRINT("ERROR: CollectFindElemById(%s) searching for temp ID\n","");
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
  (void)pGui; // Unused
  int16_t nElemId = pCollect->nElemAutoIdNext;
  pCollect->nElemAutoIdNext++;
  return nElemId;
}

gslc_tsElemRef* gslc_CollectGetElemRefTracked(gslc_tsGui* pGui,gslc_tsCollect* pCollect)
{
  (void)pGui; // Unused
  return pCollect->pElemRefTracked;
}

void gslc_CollectSetElemTracked(gslc_tsGui* pGui,gslc_tsCollect* pCollect,gslc_tsElemRef* pElemRef,int16_t nElemInd)
{
  (void)pGui; // Unused
  pCollect->pElemRefTracked = pElemRef;
  pCollect->nElemIndTracked = nElemInd;
}

// Find an element index in a collection from a coordinate
// - Note that the search is in decreasing Z-order (ie. front to back)
//   so that we effectively find the top-most element that should
//   receive an event.
// - Also returns the element index in the collection
gslc_tsElemRef* gslc_CollectFindElemFromCoord(gslc_tsGui* pGui,gslc_tsCollect* pCollect,int16_t nX, int16_t nY, int16_t* pnElemInd)
{
  int16_t               nInd;
  bool                  bFound = false;
  gslc_tsElemRef*       pElemRef = NULL;
  gslc_tsElemRef*       pFoundElemRef = NULL;

  // Reset element index
  *pnElemInd = GSLC_IND_NONE;

  if (pCollect->nElemRefCnt == 0) { return NULL; }
  for (nInd=pCollect->nElemRefCnt-1;nInd>=0;nInd--) {
    pElemRef  = &(pCollect->asElemRef[nInd]);

    if (!gslc_ElemGetVisible(pGui, pElemRef)) {
      // Element is marked as hidden, so don't accept touch
      continue;
    }

    bFound = gslc_ElemOwnsCoord(pGui,pElemRef,nX,nY,true);
    if (bFound) {
      pFoundElemRef = pElemRef;
      *pnElemInd = nInd;
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

/* UNUSED
void gslc_CollectSetEventFunc(gslc_tsGui* pGui,gslc_tsCollect* pCollect,GSLC_CB_EVENT funcCb)
{
  if ((pCollect == NULL) || (funcCb == NULL)) {
    static const char GSLC_PMEM FUNCSTR[] = "CollectSetEventFunc";
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return;
  }
  pCollect->pfuncXEvent       = funcCb;
}
*/

// ============================================================================

// Trigonometric lookup table for sin(x)
// - The lookup table saves us from having to import the sin/cos
//   functions and the associated floating point libraries. GUIslice
//   has provided a relatively lightweight fixed-point representation
//   using the following 16-bit LUT (8-bit index).
// - At this point in time, the LUT is only used by GUIslice for sin/cos
//   in supporting the XRadial controls
// - The LUT consumes approx 514 bytes of FLASH memory. FIXME: Ensure in FLASH!
#if defined(LUT_SIN_64)
// Reduced-precision (64-entry LUT) for reduced memory footprint (130 Bytes)
const uint16_t  m_nLUTSinF0X16[65] = {
  0x0000,0x0648,0x0C8F,0x12D5,
  0x1917,0x1F56,0x258F,0x2BC3,
  0x31F1,0x3816,0x3E33,0x4447,
  0x4A4F,0x504D,0x563E,0x5C21,
  0x61F7,0x67BD,0x6D73,0x7319,
  0x78AC,0x7E2E,0x839B,0x88F5,
  0x8E39,0x9367,0x987F,0x9D7F,
  0xA266,0xA735,0xABEA,0xB085,
  0xB504,0xB967,0xBDAE,0xC1D7,
  0xC5E3,0xC9D0,0xCD9E,0xD14C,
  0xD4DA,0xD847,0xDB93,0xDEBD,
  0xE1C4,0xE4A9,0xE76A,0xEA08,
  0xEC82,0xEED7,0xF108,0xF313,
  0xF4F9,0xF6B9,0xF853,0xF9C6,
  0xFB13,0xFC3A,0xFD39,0xFE12,
  0xFEC3,0xFF4D,0xFFB0,0xFFEB,
  0xFFFF,
};
#else
const uint16_t  m_nLUTSinF0X16[257] = {
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
#endif // LUT_SIN_*
