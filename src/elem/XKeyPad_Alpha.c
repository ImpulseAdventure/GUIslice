// =======================================================================
// GUIslice library extension: XKeyPad control (Alpha entry)
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
/// \file XKeyPad_Alpha.c



// GUIslice library
#include "GUIslice.h"
#include "GUIslice_drv.h"

#include "elem/XKeyPad.h"
#include "elem/XKeyPad_Alpha.h"

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
// - Keypad element with alpha input
// ============================================================================

// Import the "setup" file for XKeyPad_Alpha
#include "elem/XKeyPad_Alpha-setup.h"

// Initialize any internal state upon reset
void gslc_ElemXKeyPadReset_Alpha(void* pvConfig)
{
  (void)pvConfig; // Unused
  //gslc_tsXKeyPadCfg* pConfig = (gslc_tsXKeyPadCfg*)pvConfig;
  //gslc_tsXKeyPadCfg_Alpha* pConfigAlpha = (gslc_tsXKeyPadCfg_Alpha*)pConfig; // Retrieve the variant config

  // Initialize state
  
}


// Initialize any internal state when the text field is first assigned
void gslc_ElemXKeyPadTxtInit_Alpha(void* pvKeyPad)
{
  (void)pvKeyPad; // Unused
  //gslc_tsXKeyPad* pKeyPad = (gslc_tsXKeyPad*)pvKeyPad;
  //gslc_tsXKeyPadCfg* pConfig = pKeyPad->pConfig;
  //gslc_tsXKeyPadCfg_Alpha* pConfigAlpha = (gslc_tsXKeyPadCfg_Alpha*)pConfig; // Retrieve the variant config
  //char* pBuffer = pKeyPad->acBuffer;

  // No internal state to change

}

// Return the display label for a button
// - pStr:    return string
// - nStrMax: return string maximum length (including NULL)
void gslc_ElemXKeyPadLabelGet_Alpha(void* pvKeyPad,uint8_t nId,uint8_t nStrMax,char* pStr)
{
  uint8_t nOffset;
  gslc_tsXKeyPad* pKeyPad = (gslc_tsXKeyPad*)pvKeyPad;
  gslc_tsKey* pKeys = pKeyPad->pConfig->pLayout; 
  int16_t nInd = gslc_XKeyPadLookupId(pKeys,nId);
  uint8_t nType;
  //gslc_tsXKeyPadCfg_Alpha* pConfigV = (gslc_tsXKeyPadCfg_Alpha*)pKeyPad->pConfig; // Retrieve variant config
  int8_t eLayoutSel = pKeyPad->pConfig->eLayoutSel;

  if (DEBUG_XKEYPAD) GSLC_DEBUG_PRINT("XKeyPadLabelGet_Alpha: ID=%d Ind=%d\n",nId,nInd);
  if (nInd == GSLC_IND_NONE) {
    GSLC_DEBUG2_PRINT("ERROR: LabelGet_Alpha\n","");
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
      if (nId == KEYPAD_ID_SWAP_PAD) {
        // Dynamic content
        // - Show indicator for next keypad in sequence
        gslc_StrCopy(pStr,KEYPAD_SPECIAL_SELECT[(eLayoutSel+1) % E_XKEYPAD_SET__MAX],nStrMax);
      } else {
        // Static content
        int16_t nIndSpecial = gslc_XKeyPadLookupSpecialId(KEYPAD_SPECIAL_LABEL, nId);
        gslc_StrCopy(pStr,KEYPAD_SPECIAL_LABEL[nIndSpecial].pLabel,nStrMax);
      }
    } else {
      gslc_StrCopy(pStr,"",nStrMax);
    }
  }
}

// Return the style for a button
void gslc_ElemXKeyPadStyleGet_Alpha(void* pvKeyPad,uint8_t nId, bool* pbVisible, gslc_tsColor* pcolTxt, gslc_tsColor* pcolFrame, gslc_tsColor* pcolFill, gslc_tsColor* pcolGlow)
{
  (void)pvKeyPad; // Unused
  //gslc_tsXKeyPad* pKeyPad = (gslc_tsXKeyPad*)pvKeyPad;
  //gslc_tsXKeyPadCfg_Alpha* pConfigAlpha = (gslc_tsXKeyPadCfg_Alpha*)pKeyPad->pConfig; // Retrieve variant config

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
  } else if (nId == KEYPAD_ID_SPACE) {
    *pcolFill = XKEYPAD_COL_SPACE_FILL;
    *pcolGlow = XKEYPAD_COL_SPACE_GLOW;
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

}


// Update the keypad value string after detecting a button press
// Note that this is only called if the global handler isn't already
// handling this key (eg. Escape, Enter, etc.)
void gslc_ElemXKeyPadBtnEvt_Alpha(void* pvKeyPad,uint8_t nId,gslc_tsXKeyPadResult* psResult)
{
  gslc_tsXKeyPad* pKeyPad = (gslc_tsXKeyPad*)pvKeyPad;
  gslc_tsXKeyPadCfg* pConfig = pKeyPad->pConfig;
  //gslc_tsXKeyPadCfg_Alpha* pConfigAlpha = (gslc_tsXKeyPadCfg_Alpha*)pKeyPad->pConfig; // Retrieve variant config
  gslc_tsKey* pKeys = pConfig->pLayout; 

  int16_t nInd = gslc_XKeyPadLookupId(pKeys,nId);
  int8_t eLayoutSel = pConfig->eLayoutSel;
  bool bLayoutChg;
  bool bRet;

  if (pKeys[nInd].nType == E_XKEYPAD_TYPE_UNUSED) {
    // Ignore
  } else if (pKeys[nInd].nType == E_XKEYPAD_TYPE_BASIC) {
    // For basic buttons, we append the button key label
    char  acStr[XKEYPAD_LABEL_MAX];
    gslc_ElemXKeyPadLabelGet_Alpha(pvKeyPad,nId,XKEYPAD_LABEL_MAX,acStr);
    bRet = gslc_XKeyPadTxtAddStr(pKeyPad,acStr,pKeyPad->nCursorPos);
    if (bRet) gslc_XKeyPadPendRedrawAddTxt(psResult);
    return;

  } else {
    // Special buttons
    if (nId == KEYPAD_ID_BACKSPACE) {
      if (DEBUG_XKEYPAD) GSLC_DEBUG_PRINT("BtnEvt_Alpha: Key=BS\n", "");
      bRet = gslc_XKeyPadTxtDelCh(pKeyPad,pKeyPad->nCursorPos);
      if (bRet) gslc_XKeyPadPendRedrawAddTxt(psResult);
      return;

    } else if (nId == KEYPAD_ID_SPACE) {
      if (DEBUG_XKEYPAD) GSLC_DEBUG_PRINT("BtnEvt_Alpha: Key=SPACE\n", "");
      bRet = gslc_XKeyPadTxtAddStr(pKeyPad,XKEYPAD_LABEL_SPACE,pKeyPad->nCursorPos);
      if (bRet) gslc_XKeyPadPendRedrawAddTxt(psResult);
      return;

    } else if (nId == KEYPAD_ID_SWAP_PAD) {
      // Select next keypad in sequence
      eLayoutSel = (eLayoutSel + 1) % E_XKEYPAD_SET__MAX;
      bLayoutChg = gslc_XKeyPadLayoutSet(pConfig,eLayoutSel);
      psResult->eRedrawState = psResult->eRedrawState | ((bLayoutChg)? XKEYPAD_REDRAW_FULL : XKEYPAD_REDRAW_ALL);
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
gslc_tsXKeyPadCfg_Alpha gslc_ElemXKeyPadCfgInit_Alpha()
{
  gslc_tsXKeyPadCfg_Alpha sConfig;

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

  // Define callback functions
  sConfig.sBaseCfg.pfuncReset      = &gslc_ElemXKeyPadReset_Alpha;
  sConfig.sBaseCfg.pfuncTxtInit    = &gslc_ElemXKeyPadTxtInit_Alpha;
  sConfig.sBaseCfg.pfuncLabelGet   = &gslc_ElemXKeyPadLabelGet_Alpha;
  sConfig.sBaseCfg.pfuncStyleGet   = &gslc_ElemXKeyPadStyleGet_Alpha;
  sConfig.sBaseCfg.pfuncBtnEvt     = &gslc_ElemXKeyPadBtnEvt_Alpha;

  return sConfig;
}

// Create the XKeyPad_Alpha element
gslc_tsElemRef* gslc_ElemXKeyPadCreate_Alpha(gslc_tsGui* pGui, int16_t nElemId, int16_t nPage,
  gslc_tsXKeyPad* pXData, int16_t nX0, int16_t nY0, int8_t nFontId, gslc_tsXKeyPadCfg_Alpha* pConfig)
{

  // Provide default config if none supplied
  // FIXME: Take note of variable lifetime here  
  gslc_tsXKeyPadCfg_Alpha sConfigTmp;
  if (pConfig == NULL) {
    sConfigTmp = gslc_ElemXKeyPadCfgInit_Alpha();
    pConfig = &sConfigTmp;
  }

  // Initialize any local state
  gslc_ElemXKeyPadReset_Alpha((void*)(pConfig));
  
  // When passing the variant config to the base code, we typecast to the base type
  return gslc_XKeyPadCreateBase(pGui, nElemId, nPage, (void*)pXData, nX0, nY0, nFontId, (gslc_tsXKeyPadCfg*)pConfig);
}


// ============================================================================
