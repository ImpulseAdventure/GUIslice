// =======================================================================
// GUIslice library extension: XKeyPad control (Numeric entry)
// - Paul Conti, Calvin Hass
// - https://www.impulseadventure.com/elec/guislice-gui.html
// - https://github.com/ImpulseAdventure/GUIslice
// =======================================================================
//
// The MIT License
//
// Copyright 2016-2020 Calvin Hass
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
/// \file XKeyPad_Num.c



// GUIslice library
#include "GUIslice.h"
#include "GUIslice_drv.h"

#include "elem/XKeyPad.h"
#include "elem/XKeyPad_Num.h"

#include <stdio.h>

#if (GSLC_USE_PROGMEM)
  #if defined(__AVR__)
    #include <avr/pgmspace.h>
  #else
    #include <pgmspace.h>
  #endif
#endif

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
// - Keypad element with numeric input
// - Optionally supports floating point values
// - Optionally supports negative values
// ============================================================================

// Import the "setup" file for XKeyPad_Num
#include "elem/XKeyPad_Num-setup.h"

// Change the sign of the number string
// - This function also triggers the shifting of the content
void gslc_XKeyPadValSetSign_Num(gslc_tsXKeyPad* pXKeyPad, bool bPositive)
{
  gslc_tsXKeyPadCfg_Num* pConfigNum = (gslc_tsXKeyPadCfg_Num*)pXKeyPad->pConfig; // Retrieve variant config
  bool bValPositive = pConfigNum->bValPositive;

  if (DEBUG_XKEYPAD) GSLC_DEBUG_PRINT("XKeyPadValSetSign_Num pos_cur=%d pos_req=%d\n",bValPositive,bPositive);

  if (bValPositive == bPositive) {
    // No change to sign
    return;
  }

  if ((bValPositive) && (!bPositive)) {
    // Change from positive to negative
    // - Insert negative sign at front of buffer
    if (DEBUG_XKEYPAD) GSLC_DEBUG_PRINT("  SetSign_Num pos to neg\n","");
    gslc_XKeyPadTxtAddStr(pXKeyPad,KEYPAD_LABEL_NEGATIVE,0);
    
  } else if ((!bValPositive) && (bPositive)) {
    // Change from negative to positive
    // - Remove negative sign at front of buffer
    // - ASSERT: ensure negative sign is at front
    if (pXKeyPad->acBuffer[0] != KEYPAD_LABEL_NEGATIVE[0]) {
      if (DEBUG_XKEYPAD) GSLC_DEBUG_PRINT("ASSERT: SetSign_Num flip to positive but negative sign missing\n","");
    }
    // Request delete character from position 1 causes
    // position 0 (the location of the negative sign) to be deleted
    gslc_XKeyPadTxtDelCh(pXKeyPad,1);
  }

  // Update sign state
  pConfigNum->bValPositive = bPositive;
}

// Initialize any internal state upon reset
void gslc_ElemXKeyPadReset_Num(void* pvConfig)
{
  //gslc_tsXKeyPadCfg* pConfig = (gslc_tsXKeyPadCfg*)pvConfig;
  gslc_tsXKeyPadCfg_Num* pConfigNum = (gslc_tsXKeyPadCfg_Num*)pvConfig; // Retrieve the variant config

  // Initialize state
  pConfigNum->bValPositive = true;
  pConfigNum->bValDecimalPt = false;
}


// Initialize any internal state when the text field is first assigned
void gslc_ElemXKeyPadTxtInit_Num(void* pvKeyPad)
{
  gslc_tsXKeyPad* pKeyPad = (gslc_tsXKeyPad*)pvKeyPad;
  gslc_tsXKeyPadCfg* pConfig = pKeyPad->pConfig;
  gslc_tsXKeyPadCfg_Num* pConfigNum = (gslc_tsXKeyPadCfg_Num*)pConfig; // Retrieve the variant config
  char* pBuffer = pKeyPad->acBuffer;

  // Update internal state based on new buffer value
  pConfigNum->bValPositive = true;
  if (pBuffer[0] == KEYPAD_LABEL_NEGATIVE[0]) {
    pConfigNum->bValPositive = false;
  }
  pConfigNum->bValDecimalPt = false;
  if (strchr(pBuffer, KEYPAD_LABEL_DECIMAL_PT[0])) {
    pConfigNum->bValDecimalPt = true;
  }

}

// Return the display label for a button
// - pStr:    return string
// - nStrMax: return string maximum length (including NULL)
void gslc_ElemXKeyPadLabelGet_Num(void* pvKeyPad,uint8_t nId,uint8_t nStrMax,char* pStr)
{
  uint8_t nOffset;
  gslc_tsXKeyPad* pKeyPad = (gslc_tsXKeyPad*)pvKeyPad;
  gslc_tsKey* pKeys = pKeyPad->pConfig->pLayout; 
  int16_t nInd = gslc_XKeyPadLookupId(pKeys,nId);
  uint8_t nType;
  //gslc_tsXKeyPadCfg_Num* pConfigV = (gslc_tsXKeyPadCfg_Num*)pKeyPad->pConfig; // Retrieve variant config
  int8_t eLayoutSel = pKeyPad->pConfig->eLayoutSel;

  if (DEBUG_XKEYPAD) GSLC_DEBUG_PRINT("XKeyPadLabelGet_Num: pKeyPad=%d nId=%d pKeys=%d nInd=%d\n",
    pKeyPad,nId,pKeys,nInd);

  if (DEBUG_XKEYPAD) GSLC_DEBUG_PRINT("  ID=%d Ind=%d\n",nId,nInd);
  if (nInd == GSLC_IND_NONE) {
    GSLC_DEBUG2_PRINT("ERROR: LabelGet_Num\n","");
    // Should never get here
  } else {
    nType = pKeys[nInd].nType;
    if (nType == E_XKEYPAD_TYPE_BASIC) {
      nOffset = nId - KEYPAD_ID_BASIC_START;
      #if (XKEYPAD_EXTEND_CHAR)
        // KeyPad array composed of strings
        gslc_StrCopy(pStr,KEYPAD_SET_LABEL[eLayoutSel][nOffset],nStrMax);
      #else
        // KeyPad array composed of single characters
        pStr[0] = KEYPAD_SET_LABEL[eLayoutSel][nOffset];
        pStr[1] = '\0';
      #endif
      if (DEBUG_XKEYPAD) GSLC_DEBUG_PRINT("  type=%d sel=%d offset=%d pStr=[%s]\n",
        nType,eLayoutSel,nOffset,pStr);
    } else if (nType == E_XKEYPAD_TYPE_SPECIAL) {
      // Handle any dynamic keys first

      // Static content
      int16_t nIndSpecial = gslc_XKeyPadLookupSpecialId(KEYPAD_SPECIAL_LABEL, nId);
      gslc_StrCopy(pStr,KEYPAD_SPECIAL_LABEL[nIndSpecial].pLabel,nStrMax);
    } else {
      gslc_StrCopy(pStr,"",nStrMax);
    }
  }
}

// Return the style for a button
void gslc_ElemXKeyPadStyleGet_Num(void* pvKeyPad,uint8_t nId, bool* pbVisible, gslc_tsColor* pcolTxt, gslc_tsColor* pcolFrame, gslc_tsColor* pcolFill, gslc_tsColor* pcolGlow)
{
  gslc_tsXKeyPad* pKeyPad = (gslc_tsXKeyPad*)pvKeyPad;
  gslc_tsXKeyPadCfg_Num* pConfigNum = (gslc_tsXKeyPadCfg_Num*)pKeyPad->pConfig; // Retrieve variant config

  // Provide defaults
  *pcolTxt = XKEYPAD_COL_DEF_TXT;
  *pcolFrame = XKEYPAD_COL_DEF_FRAME;
  *pcolFill = XKEYPAD_COL_DEF_FILL;
  *pcolGlow = XKEYPAD_COL_DEF_GLOW;

  // Provide overrides
  if (nId == KEYPAD_ID_TXT) {
    *pcolTxt = XKEYPAD_COL_TEXT_TXT;
    *pcolFill = XKEYPAD_COL_TEXT_FILL;
    *pcolGlow = XKEYPAD_COL_TEXT_GLOW;
  } else if (nId == KEYPAD_IDV_DECIMAL) {
    // Show the button as dimmed (ie. not available) if
    // a decimal point is already active
    if (pConfigNum->bValDecimalPt) {
      *pcolTxt = XKEYPAD_COL_DISABLE_TXT;
      *pcolFill = XKEYPAD_COL_DISABLE_FILL;
    } else {
      *pcolTxt = XKEYPAD_COL_DEF_TXT;
      *pcolFill = XKEYPAD_COL_DECIMAL_FILL;
    }
    *pcolGlow = XKEYPAD_COL_DECIMAL_GLOW;
  } else if (nId == KEYPAD_IDV_MINUS) {
    *pcolFill = XKEYPAD_COL_MINUS_FILL;
    *pcolGlow = XKEYPAD_COL_MINUS_GLOW;
  } else if (nId == KEYPAD_ID_ESC) {
    *pcolFill = XKEYPAD_COL_ESC_FILL;
    *pcolGlow = XKEYPAD_COL_ESC_GLOW;
  } else if (nId == KEYPAD_ID_ENTER) {
    *pcolFill = XKEYPAD_COL_ENTER_FILL;
    *pcolGlow = XKEYPAD_COL_ENTER_GLOW;
  } else if (nId == KEYPAD_ID_SCROLL_LEFT) {
    *pcolFill = XKEYPAD_COL_SCROLL_L_FILL;
    *pcolGlow = XKEYPAD_COL_SCROLL_L_GLOW;
  } else if (nId == KEYPAD_ID_SCROLL_RIGHT) {
    *pcolFill = XKEYPAD_COL_SCROLL_R_FILL;
    *pcolGlow = XKEYPAD_COL_SCROLL_R_GLOW;
  } else if (nId >= KEYPAD_ID_BASIC_START) {
    *pcolFill = XKEYPAD_COL_BASIC_FILL;
    *pcolGlow = XKEYPAD_COL_BASIC_GLOW;
  }

  // Update any button visibility
  *pbVisible = true;
  if (nId == KEYPAD_IDV_MINUS) {
    *pbVisible = pConfigNum->bSignEn;
  } else if (nId == KEYPAD_IDV_DECIMAL) {
    *pbVisible = pConfigNum->bFloatEn;
  }

}


// Update the keypad value string after detecting a button press
// Note that this is only called if the global handler isn't already
// handling this key (eg. Escape, Enter, etc.)
void gslc_ElemXKeyPadBtnEvt_Num(void* pvKeyPad,uint8_t nId,gslc_tsXKeyPadResult* psResult)
{
  gslc_tsXKeyPad* pKeyPad = (gslc_tsXKeyPad*)pvKeyPad;
  gslc_tsXKeyPadCfg* pConfig = pKeyPad->pConfig;
  gslc_tsXKeyPadCfg_Num* pConfigNum = (gslc_tsXKeyPadCfg_Num*)pKeyPad->pConfig; // Retrieve variant config
  gslc_tsKey* pKeys = pConfig->pLayout; 
  int16_t nInd = gslc_XKeyPadLookupId(pKeys,nId);
  bool bRet;
  uint8_t nCursorPos;
  bool* pbFloatEn = &(pConfigNum->bFloatEn);
  bool* pbSignEn = &(pConfigNum->bSignEn);
  bool* pbValPositive = &(pConfigNum->bValPositive);
  bool* pbValDecimalPt = &(pConfigNum->bValDecimalPt);

  if (pKeys[nInd].nType == E_XKEYPAD_TYPE_UNUSED) {
    // Ignore
  } else if (pKeys[nInd].nType == E_XKEYPAD_TYPE_BASIC) {
    // For basic buttons, we append the button key label
    char  acStr[XKEYPAD_LABEL_MAX];
    gslc_ElemXKeyPadLabelGet_Num(pvKeyPad,nId,XKEYPAD_LABEL_MAX,acStr);
    // For numeric input, if the negation is active and the
    // cursor is ahead of the negative sign, then ignore
    // any numeric input.
    if ((*pbValPositive == 0) && (pKeyPad->nCursorPos == 0)) {
      // Ignore the input
    } else {
      bRet = gslc_XKeyPadTxtAddStr(pKeyPad,acStr,pKeyPad->nCursorPos);
      if (bRet) gslc_XKeyPadPendRedrawAddTxt(psResult);
    }
    return;

  } else {
    // Special buttons
    if (nId == KEYPAD_ID_BACKSPACE) {
      // Detect if any special characters were going to be deleted
      // Note that we look at the character to the left of the
      // cursor to determine what will be deleted
      nCursorPos = pKeyPad->nCursorPos;
      if (nCursorPos == 0) {
        // Ignore the request as nothing to delete from here
      } else if (pKeyPad->acBuffer[nCursorPos-1] == KEYPAD_LABEL_DECIMAL_PT[0]) {
        // Clear decimal flag status
        // - Expect only possible if (*pbFloatEn && *pbValDecimalPt)
        if (DEBUG_XKEYPAD) GSLC_DEBUG_PRINT("BtnEvt_Num: back across decimal\n","");
        if ((*pbFloatEn) && (*pbValDecimalPt)) {
          *pbValDecimalPt = false;
        } else {
          GSLC_DEBUG2_PRINT("ASSERT: delete decimal in wrong state\n","");
        }
        // The decimal point key may change state, so request redraw
        gslc_XKeyPadPendRedrawAddKey(psResult,KEYPAD_IDV_DECIMAL);

      } else if (pKeyPad->acBuffer[nCursorPos-1] == KEYPAD_LABEL_NEGATIVE[0]) {
        if (DEBUG_XKEYPAD) GSLC_DEBUG_PRINT("BtnEvt_Num: back across neg\n","");
        // Clear negative flag status
        // - Expect only possible if (*pbSignEn && !*pbValPositive)
        if ((*pbSignEn) && (!*pbValPositive)) {
          if (DEBUG_XKEYPAD) GSLC_DEBUG_PRINT("BtnEvt_Num: change to positive","");
          *pbValPositive = true;
        } else {
          GSLC_DEBUG2_PRINT("ASSERT: delete negative in wrong state\n","");
        }
      }
      
      if (DEBUG_XKEYPAD) GSLC_DEBUG_PRINT("BtnEvt_Num: about to DelCh @ %d\n",nCursorPos);
      bRet = gslc_XKeyPadTxtDelCh(pKeyPad,pKeyPad->nCursorPos);

      if (bRet) gslc_XKeyPadPendRedrawAddTxt(psResult);
      return;

    } else if (nId == KEYPAD_IDV_MINUS) {
      if (*pbSignEn) {
        bool bPositive = *pbValPositive ? false : true; // Toggle sign
        gslc_XKeyPadValSetSign_Num(pKeyPad,bPositive);
        gslc_XKeyPadPendRedrawAddTxt(psResult);
        return;
      }
      return;
    } else if (nId == KEYPAD_IDV_DECIMAL) {
      //GSLC_DEBUG2_PRINT("KeyPad Key=Decimal\n", "");
      if (*pbFloatEn) {
        if (!*pbValDecimalPt) {
          if (DEBUG_XKEYPAD) GSLC_DEBUG_PRINT("BtnEvt_Num: key=dec, not already placed. add\n","");
          // Note that the decimal point will be added at the current
          // cursor position, which could be in the middle of an integer.
          bRet = gslc_XKeyPadTxtAddStr(pKeyPad,KEYPAD_LABEL_DECIMAL_PT,pKeyPad->nCursorPos);
          if (bRet) gslc_XKeyPadPendRedrawAddTxt(psResult);

          *pbValDecimalPt = true;

          // As the decimal point button may be dimmed (to reflect it being disabled),
          // we request a key redraw here.
          gslc_XKeyPadPendRedrawAddKey(psResult,KEYPAD_IDV_DECIMAL);

          return;
        } else {
          if (DEBUG_XKEYPAD) GSLC_DEBUG_PRINT("Bn key=dec, but already placed. ignore\n","");
          // If a decimal point already has been placed, we ignore
          // the button press. If the user wishes to remove the decimal
          // point, they need to backspace across it.
        }
      }
      return;
      
    } else {
      // Other special buttons
      // TODO
    } // nId

  } // eType
  return;

}

// Reset the XKeyPad config struct
// - This must be called before any XKeyPad config update APIs are called
gslc_tsXKeyPadCfg_Num gslc_ElemXKeyPadCfgInit_Num()
{
  gslc_tsXKeyPadCfg_Num sConfig;

  // Initialize base
  gslc_ElemXKeyPadCfgInit(&sConfig.sBaseCfg);

  // Override common config
  sConfig.sBaseCfg.nDispMax      = XKEYPAD_DISP_MAX;
  sConfig.sBaseCfg.nButtonSzW    = XKEYPAD_KEY_W;
  sConfig.sBaseCfg.nButtonSzH    = XKEYPAD_KEY_H;
  sConfig.sBaseCfg.nButtonSpaceX = XKEYPAD_SPACING_X;
  sConfig.sBaseCfg.nButtonSpaceY = XKEYPAD_SPACING_Y;
  sConfig.sBaseCfg.bRoundEn      = false;
  sConfig.sBaseCfg.nFontId       = GSLC_FONT_NONE; // Will be overwritten
  sConfig.sBaseCfg.pLayouts      = KEYPAD_LAYOUTS;
  sConfig.sBaseCfg.eLayoutDef    = XKEYPAD_LAYOUT_DEFAULT;
  gslc_XKeyPadLayoutSet(&(sConfig.sBaseCfg),XKEYPAD_LAYOUT_DEFAULT);

  // Search layouts for maximum extents
  gslc_XKeyPadSizeAllGet(sConfig.sBaseCfg.pLayouts, E_XKEYPAD_SET__MAX,
    &(sConfig.sBaseCfg.nMaxRows), &(sConfig.sBaseCfg.nMaxCols));

  sConfig.sBaseCfg.nFrameMargin  = 2;

  // Provide variant-specific config
  sConfig.bFloatEn     = true;
  sConfig.bSignEn      = true;

  // Define callback functions
  sConfig.sBaseCfg.pfuncReset      = &gslc_ElemXKeyPadReset_Num;
  sConfig.sBaseCfg.pfuncTxtInit    = &gslc_ElemXKeyPadTxtInit_Num;
  sConfig.sBaseCfg.pfuncLabelGet   = &gslc_ElemXKeyPadLabelGet_Num;
  sConfig.sBaseCfg.pfuncStyleGet   = &gslc_ElemXKeyPadStyleGet_Num;
  sConfig.sBaseCfg.pfuncBtnEvt     = &gslc_ElemXKeyPadBtnEvt_Num;

  return sConfig;
}

// Create the XKeyPad_Num element
gslc_tsElemRef* gslc_ElemXKeyPadCreate_Num(gslc_tsGui* pGui, int16_t nElemId, int16_t nPage,
  gslc_tsXKeyPad* pXData, int16_t nX0, int16_t nY0, int8_t nFontId, gslc_tsXKeyPadCfg_Num* pConfig)
{

  // Provide default config if none supplied
  // FIXME: Take note of variable lifetime here
  gslc_tsXKeyPadCfg_Num sConfigTmp;
  if (pConfig == NULL) {
    sConfigTmp = gslc_ElemXKeyPadCfgInit_Num();
    pConfig = &sConfigTmp;
  }

  // Initialize any local state
  gslc_ElemXKeyPadReset_Num((void*)(pConfig));

  // When passing the variant config to the base code, we typecast to the base type
  return gslc_XKeyPadCreateBase(pGui, nElemId, nPage, (void*)pXData, nX0, nY0, nFontId, (gslc_tsXKeyPadCfg*)pConfig);
}


// ============================================================================

void gslc_ElemXKeyPadCfgSetFloatEn_Num(gslc_tsXKeyPadCfg_Num* pConfig, bool bEn)
{
  pConfig->bFloatEn = bEn;
}

void gslc_ElemXKeyPadCfgSetSignEn_Num(gslc_tsXKeyPadCfg_Num* pConfig, bool bEn)
{
  pConfig->bSignEn = bEn;
}