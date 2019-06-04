// =======================================================================
// GUIslice library extension: XKeyPad control (Alpha entry)
// - Paul Conti, Calvin Hass
// - https://www.impulseadventure.com/elec/guislice-gui.html
// - https://github.com/ImpulseAdventure/GUIslice
// =======================================================================
//
// The MIT License
//
// Copyright 2016-2019 Calvin Hass
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
/// \file XKeyPad.c



// GUIslice library
#include "GUIslice.h"
#include "GUIslice_drv.h"

#include "elem/XKeyPad.h"
#include "elem/XKeyPad_Alpha.h"

#include <stdio.h>

#if (GSLC_USE_PROGMEM)
    #include <avr/pgmspace.h>
#endif

#if (GSLC_FEATURE_COMPOUND)

// ----------------------------------------------------------------------------
// Error Messages
// ----------------------------------------------------------------------------

extern const char GSLC_PMEM ERRSTR_NULL[];
extern const char GSLC_PMEM ERRSTR_PXD_NULL[];


// ----------------------------------------------------------------------------
// Extended element definitions
// ----------------------------------------------------------------------------
//
// - This file extends the core GUIslice functionality with
//   additional widget types
//
// ----------------------------------------------------------------------------


// ============================================================================
// Extended Element: KeyPad
// - Keypad element with alpha input
// ============================================================================

// Define the button labels
// - TODO: Create more efficient storage for the labels
//   so that it doesn't consume 4 bytes even for labels
//   that can be generated (eg. 0..9, a--z, etc.)
// - TODO: Support use of PROGMEM. Note that these labels
//   are not currently using "const char" since we may
//   want to support user-modification of the labels.

static char* KEYPAD_LABEL_STRINGS[] = {
  // Special buttons
  "<", ".", " ", "ESC", "ENT",
  // Basic buttons
  "A", "B", "C", "D", "E", "F", "G", "H" ,"I", "J",
  "K", "L", "M", "N", "O", "P", "Q", "R" ,"S", "T",
  "U", "V", "W", "X", "Y", "Z",
};

// Define enums for KEYPAD_LABEL_STRINGS
enum {
  // - Special buttons
  KEYPAD_LBL_BACKSPACE,
  KEYPAD_LBL_PERIOD,
  KEYPAD_LBL_SPACE,
  KEYPAD_LBL_ESC,
  KEYPAD_LBL_ENTER,
  // - Basic buttons
  KEYPAD_LBL_BASIC_START
};


// Generate the keypad layout
void XKeyPadCreateKeys_Alpha(gslc_tsGui* pGui, gslc_tsXKeyPad* pXData)
{
  int16_t nKeyInd;
  int16_t nRow, nCol;

  // - Create the "special" buttons
  XKeyPadAddKeyElem(pGui, pXData, KEYPAD_ID_BACKSPACE, false, 1, 6, 1, 2, GSLC_COL_GRAY_LT1, GSLC_COL_GRAY_LT3, true);
  XKeyPadAddKeyElem(pGui, pXData, KEYPAD_ID_PERIOD, false, 3, 7, 1, 1, GSLC_COL_GRAY_LT1, GSLC_COL_GRAY_LT3, true);
  XKeyPadAddKeyElem(pGui, pXData, KEYPAD_ID_SPACE, false, 3, 6, 1, 1, GSLC_COL_GRAY_LT1, GSLC_COL_GRAY_LT3, true);
  XKeyPadAddKeyElem(pGui, pXData, KEYPAD_ID_ESC, false, 2, 6, 1, 2, GSLC_COL_RED, GSLC_COL_RED_LT4, true);
  XKeyPadAddKeyElem(pGui, pXData, KEYPAD_ID_ENTER, false, 0, 6, 1, 2, GSLC_COL_GREEN, GSLC_COL_GREEN_LT4, true);

  // - Create the "simple" buttons
  for (int16_t nKeyId = KEYPAD_ID_BASIC_START; nKeyId < (KEYPAD_ID_BASIC_START + XKEYPADALPHA_BTN_BASIC); nKeyId++) {
    nKeyInd = nKeyId - KEYPAD_ID_BASIC_START;
    nRow = (nKeyInd / 6) + 1;
    nCol = nKeyInd % 6;
    XKeyPadAddKeyElem(pGui, pXData, nKeyId, false, nRow, nCol, 1, 1, GSLC_COL_BLUE_LT1, GSLC_COL_BLUE_LT4, true);
  }

  // - Create the text field
  XKeyPadAddKeyElem(pGui, pXData, KEYPAD_ID_TXT, true, 0, 0, 1, 6, GSLC_COL_BLACK, GSLC_COL_BLACK, true);
}

// Convert between keypad ID and the index into the keypad label array
int16_t XKeyPadLookup_Alpha(gslc_tsGui* pGui, int16_t nKeyId)
{
  int16_t nKeyInd;

  // Basic button
  if (nKeyId >= KEYPAD_ID_BASIC_START) {
    nKeyInd = (nKeyId - KEYPAD_ID_BASIC_START) + KEYPAD_LBL_BASIC_START;
  } else {
    // Special button
    switch (nKeyId) {
    case KEYPAD_ID_PERIOD:
      nKeyInd = KEYPAD_LBL_PERIOD;
      break;
    case KEYPAD_ID_SPACE:
      nKeyInd = KEYPAD_LBL_SPACE;
      break;
    case KEYPAD_ID_BACKSPACE:
      nKeyInd = KEYPAD_LBL_BACKSPACE;
      break;
    case KEYPAD_ID_ESC:
      nKeyInd = KEYPAD_LBL_ESC;
      break;
    case KEYPAD_ID_ENTER:
      nKeyInd = KEYPAD_LBL_ENTER;
      break;
    default:
      // FIXME: ERROR
      nKeyInd = -1; // Not expected
      break;
    }
  }
  return nKeyInd;
}

// Create the XKeyPad_Alpha compound element
// - Note that this also revises some of the members of the base XKeyPad struct
gslc_tsElemRef* gslc_ElemXKeyPadCreate_Alpha(gslc_tsGui* pGui, int16_t nElemId, int16_t nPage,
  gslc_tsXKeyPad_Alpha* pXData, int16_t nX0, int16_t nY0, int8_t nFontId, gslc_tsXKeyPadCfg* pConfig)
{
  gslc_tsXKeyPad* pXDataBase = (gslc_tsXKeyPad*)pXData;
  pXDataBase->nSubElemMax = XKEYPADALPHA_ELEM_MAX;
  pXDataBase->psElemRef = pXData->asElemRef;
  pXDataBase->psElem = pXData->asElem;

  // Provide default config if none supplied
  gslc_tsXKeyPadCfg sConfigTmp;
  if (pConfig == NULL) {
    sConfigTmp = gslc_ElemXKeyPadCfgInit_Alpha();
    pConfig = &sConfigTmp;
  }

  return gslc_ElemXKeyPadCreateBase(pGui, nElemId, nPage, pXDataBase, nX0, nY0, nFontId, pConfig,
    &XKeyPadCreateKeys_Alpha,&XKeyPadLookup_Alpha);
}

// Reset the XKeyPad config struct
// - This must be called before any XKeyPad config update APIs are called
gslc_tsXKeyPadCfg gslc_ElemXKeyPadCfgInit_Alpha()
{
  gslc_tsXKeyPadCfg sConfig;
  sConfig.nButtonSzW = 25;
  sConfig.nButtonSzH = 25;
  sConfig.bFloatEn = false; // Unused
  sConfig.bSignEn = false; // Unused
  sConfig.bRoundEn = false;
  sConfig.nFontId = GSLC_FONT_NONE; // Will be overwritten
  sConfig.pacKeys = KEYPAD_LABEL_STRINGS;
  sConfig.nMaxCols = 8;
  sConfig.nMaxRows = 6;
  sConfig.nFrameMargin = 2;
  return sConfig;
}

#endif // GSLC_FEATURE_COMPOUND

// ============================================================================
