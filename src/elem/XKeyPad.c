// =======================================================================
// GUIslice library extension: XKeyPad control
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
/// \file XKeyPad.c



// GUIslice library
#include "GUIslice.h"
#include "GUIslice_drv.h"

#include "elem/XKeyPad.h"

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

// Reset the content of the keypad
void gslc_ElemXKeyPadReset(gslc_tsXKeyPad* pKeyPad)
{
  // Default to maximum buffer, but may be reduced by association with a smaller target element
  pKeyPad->nBufferMax = XKEYPAD_BUF_MAX;
  memset(pKeyPad->acBuffer, 0, XKEYPAD_BUF_MAX);

  pKeyPad->nBufferLen = 0;
  pKeyPad->nCursorPos = 0;
  pKeyPad->nScrollPos = 0;
  pKeyPad->pTargetRef = NULL;

  pKeyPad->nFocusKeyInd = GSLC_IND_NONE;
  pKeyPad->nGlowKeyInd = GSLC_IND_NONE;

  // Reset any pending redraw state
  gslc_XKeyPadPendRedrawReset(&(pKeyPad->sRedraw));
}

// Default common base config initialization
// - Most of these values will be overwritten by variant-specific
//   initialization or from user configuration.
void gslc_ElemXKeyPadCfgInit(gslc_tsXKeyPadCfg* pConfig)
{
  pConfig->nButtonSzW   = 25;
  pConfig->nButtonSzH   = 25;
  pConfig->nButtonSpaceX = 0;
  pConfig->nButtonSpaceY = 0;
  pConfig->bRoundEn     = false;
  pConfig->nFontId      = GSLC_FONT_NONE;
  pConfig->pLayout      = NULL;
  pConfig->pLayouts     = NULL;
  pConfig->eLayoutSel   = 0;
  pConfig->nMaxCols     = 8;
  pConfig->nMaxRows     = 3;
  pConfig->nFrameMargin = 2;

  // Define callback functions
  pConfig->pfuncReset      = NULL;
  pConfig->pfuncTxtInit    = NULL;
  pConfig->pfuncLabelGet   = NULL;
  pConfig->pfuncStyleGet   = NULL;
  pConfig->pfuncBtnEvt     = NULL;
}

// --------------------------------------------------------------------------
// Create the keypad definition
// --------------------------------------------------------------------------

// Convert between keypad ID and the index into the keypad label array
// - This function assumes that the keypad list has been terminated with KEYPAD_ID__END
int16_t gslc_XKeyPadLookupId(gslc_tsKey* pKeys, uint8_t nKeyId)
{
  if (DEBUG_XKEYPAD) GSLC_DEBUG_PRINT("XKeyPadLookupId KeyId=%d\n",nKeyId);
  int16_t nKeyInd = 0;
  bool bDone = false;
  while (!bDone) {
    uint8_t nId = pKeys[nKeyInd].nId;
    if (nId == KEYPAD_ID__END) {
      bDone = true;
    } else if (nId == nKeyId) {
      return nKeyInd;
    } else {
      nKeyInd++;
    }
  }
  // None found
  return GSLC_ID_NONE;
}

int16_t gslc_XKeyPadLookupSpecialId(gslc_tsLabelSpecial* pLabels, uint8_t nKeyId)
{
  if (DEBUG_XKEYPAD) GSLC_DEBUG_PRINT("XKeyPadLookupSpecialId KeyId=%d\n",nKeyId);
  int16_t nKeyInd = 0;
  bool bDone = false;
  while (!bDone) {
    uint8_t nId = pLabels[nKeyInd].nId;
    if (nId == KEYPAD_ID__END) {
      bDone = true;
    } else if (nId == nKeyId) {
      return nKeyInd;
    } else {
      nKeyInd++;
    }
  }
  // None found
  return GSLC_ID_NONE;
}


// Render the KeyPad layout
void gslc_XKeyPadDrawLayout(gslc_tsGui* pGui, void* pXData)
{
  // - Draw the fields
  gslc_tsXKeyPad* pKeypadData = (gslc_tsXKeyPad*)pXData;
  gslc_tsXKeyPadCfg* pConfig = pKeypadData->pConfig;
  int16_t ePendRedraw = pKeypadData->sRedraw.eRedrawState;
  int16_t nPendRedrawId1 = pKeypadData->sRedraw.nRedrawKeyId1;
  int16_t nPendRedrawId2 = pKeypadData->sRedraw.nRedrawKeyId2;
  int16_t nFocusKeyInd = pKeypadData->nFocusKeyInd;
  int16_t nGlowKeyInd = pKeypadData->nGlowKeyInd;


  // Draw the buttons
  uint8_t       nInd;
  gslc_tsKey    sKey;
  bool          bDone = false;
  bool          bRedraw;
  bool          bGlow;
  bool          bFocus;

  nInd = 0;
  while (!bDone) {
    sKey = pConfig->pLayout[nInd];
    if (sKey.nType == E_XKEYPAD_TYPE_END) {
      bDone = true;
    } else {
      bRedraw = false; // Default to no redraw

      if (sKey.nType == E_XKEYPAD_TYPE_UNUSED) {
        // Don't redraw as Row/Col not defined
      } else if (sKey.nType == E_XKEYPAD_TYPE_BASIC) {
        bRedraw = (ePendRedraw & RBIT_KEYALL);
      } else if (sKey.nType == E_XKEYPAD_TYPE_SPECIAL) {
        bRedraw = (ePendRedraw & RBIT_KEYALL);
      } else if (sKey.nType == E_XKEYPAD_TYPE_TXT) {
        bRedraw = (ePendRedraw & RBIT_TXT);
      }

      // Is this key in focus?
      bFocus = (nFocusKeyInd == nInd);
      bGlow = (nGlowKeyInd == nInd);

      // Check for specific key redraw
      if (ePendRedraw & RBIT_KEYONE) {
        // If this is the specific key, redraw it
        // Note that we support up to 2 pending keys in one pass
        bRedraw = bRedraw || (sKey.nId == nPendRedrawId1) || (sKey.nId == nPendRedrawId2);
      }

      // Now perform the redraw
      if (bRedraw) {
        gslc_XKeyPadDrawKey(pGui, pKeypadData, &sKey, bGlow, bFocus);
      }
      nInd++;
    }
  } // bDone

  // Clear the pending redraw
  gslc_XKeyPadPendRedrawReset(&(pKeypadData->sRedraw));

}

void gslc_XKeyPadDrawKey(gslc_tsGui* pGui, gslc_tsXKeyPad* pXData, gslc_tsKey* pKey, bool bGlow, bool bFocus)
{
  gslc_tsXKeyPadCfg* pConfig;
  char* pKeyStr = NULL;
  char acKeyStr[XKEYPAD_KEY_LEN] = "";

  // Provide default colors in case pfuncStyleGet is not called
  gslc_tsColor colTxt = GSLC_COL_WHITE;
  gslc_tsColor colFrame = GSLC_COL_MAGENTA;
  gslc_tsColor colFill = GSLC_COL_MAGENTA;
  gslc_tsColor colGlow = GSLC_COL_MAGENTA;
  bool bVisible = true;

  pConfig = pXData->pConfig;

  bool bFieldIsTxt = false;
  if (pKey->nType == E_XKEYPAD_TYPE_TXT) {
    bFieldIsTxt = true;
  }

  int16_t nButtonSzW = pConfig->nButtonSzW;
  int16_t nButtonSzH = pConfig->nButtonSzH;
  int16_t nOffsetX = pConfig->nOffsetX;
  int16_t nOffsetY = pConfig->nOffsetY;

  int16_t nButtonW = (pKey->nColSpan * nButtonSzW);
  int16_t nButtonH = (pKey->nRowSpan * nButtonSzH);
  
  // Fetch the keypad key string
  if (bFieldIsTxt) {
    pKeyStr = (char*)pXData->acBuffer;
    if (DEBUG_XKEYPAD) GSLC_DEBUG_PRINT("XKeyPadDrawKey Called LABEL_GET [ID=%d TXT] R=%d C=%d [Str=%s]\n",pKey->nId,pKey->nRow,pKey->nCol,pKeyStr);
  } else {
    GSLC_CB_XKEYPAD_LABEL_GET  pfuncLabelGet = pConfig->pfuncLabelGet;
    if (pfuncLabelGet != NULL) {
      (*pfuncLabelGet)((void*)pXData,pKey->nId,XKEYPAD_KEY_LEN,acKeyStr);
      pKeyStr = acKeyStr;
    }
    if (DEBUG_XKEYPAD) GSLC_DEBUG_PRINT("XKeyPadDrawKey Called LABEL_GET [ID=%d] R=%d C=%d [Str=%s] ptr=%d\n",pKey->nId,pKey->nRow,pKey->nCol,pKeyStr,pKeyStr);
  }

  // Fetch the styling
  GSLC_CB_XKEYPAD_SYTLE_GET  pfuncStyleGet = pConfig->pfuncStyleGet;
  if (pfuncStyleGet != NULL) {
    (*pfuncStyleGet)((void*)pXData,pKey->nId,&bVisible,&colTxt,&colFrame,&colFill,&colGlow);
  }
  if (pKey->nType == E_XKEYPAD_TYPE_UNUSED) {
    bVisible = false;
  }

  // Don't draw any hidden buttons
  if (!bVisible) {
    return;
  }

  gslc_tsRect rElem;
  rElem.x = nOffsetX + (pKey->nCol*nButtonSzW);
  rElem.y = nOffsetY + (pKey->nRow*nButtonSzH);
  rElem.w = nButtonW - 1;
  rElem.h = nButtonH - 1;

  
  if (bFieldIsTxt) {
    // Text field
    gslc_XKeyPadDrawVirtualTxt(pGui,rElem,pXData,colFrame,colFill,colTxt);
  } else {
    // Text button
    // - Support button spacing config
    // TODO: May need to define more glow color attributes
    rElem = gslc_ExpandRect(rElem,-(pConfig->nButtonSpaceX),-(pConfig->nButtonSpaceY));
    gslc_XKeyPadDrawVirtualBtn(pGui,rElem,pKeyStr,sizeof(pKeyStr),pConfig->nFontId,colFrame, 
      colFill,colGlow,colTxt,pConfig->bRoundEn,bGlow,bFocus);
  }

}

gslc_tsElemRef* gslc_XKeyPadCreateBase(gslc_tsGui* pGui, int16_t nElemId, int16_t nPage,
  gslc_tsXKeyPad* pXData, int16_t nX0, int16_t nY0, int8_t nFontId, gslc_tsXKeyPadCfg* pConfig)
{
  if ((pGui == NULL) || (pXData == NULL)) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXKeyPadCreate";
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL, FUNCSTR);
    return NULL;
  }

  // Fetch config options
  int8_t nButtonSzW = pConfig->nButtonSzW;
  int8_t nButtonSzH = pConfig->nButtonSzH;

  gslc_tsRect rElem;
  rElem.x = nX0;
  rElem.y = nY0;
  rElem.w = (nButtonSzW * pConfig->nMaxCols) + (2 * pConfig->nFrameMargin);
  rElem.h = (nButtonSzH * pConfig->nMaxRows) + (2 * pConfig->nFrameMargin);

  gslc_tsElem sElem;
  gslc_tsElemRef* pElemRef = NULL;

  // Initialize element
  sElem = gslc_ElemCreate(pGui, nElemId, nPage, GSLC_TYPEX_KEYPAD, rElem, NULL, 0, GSLC_FONT_NONE);
  sElem.nFeatures |= GSLC_ELEM_FEA_FRAME_EN;
  sElem.nFeatures |= GSLC_ELEM_FEA_FILL_EN;
  sElem.nFeatures |= GSLC_ELEM_FEA_CLICK_EN;
  sElem.nFeatures &= ~GSLC_ELEM_FEA_GLOW_EN;  // Don't need to glow outer element
  sElem.nFeatures |= GSLC_ELEM_FEA_FOCUS_EN;
  sElem.nFeatures |= GSLC_ELEM_FEA_EDIT_EN;
  sElem.nGroup = GSLC_GROUP_ID_NONE;

  sElem.pXData = (void*)(pXData);
  // Specify the custom drawing callback
  sElem.pfuncXDraw = &gslc_XKeyPadDraw;
  // Specify the custom touch tracking callback
  sElem.pfuncXTouch = &gslc_XKeyPadTouch;
 
  // shouldn't be used
  sElem.colElemFill       = GSLC_COL_BLACK;
  sElem.colElemFillGlow   = GSLC_COL_BLACK;
  sElem.colElemFrame      = GSLC_COL_GRAY_DK2;
  sElem.colElemFrameGlow  = GSLC_COL_WHITE;
  sElem.colElemText       = GSLC_COL_WHITE;

  // Determine offset coordinate of compound element so that we can
  // specify relative positioning during the sub-element Create() operations.
  int16_t nOffsetX = nX0 + pConfig->nFrameMargin;
  int16_t nOffsetY = nY0 + pConfig->nFrameMargin;

  // Reset buffer and associated state
  gslc_ElemXKeyPadReset(pXData);

  pXData->pfuncCb       = NULL;
  pXData->pTargetRef    = NULL;  // Default to no target defined

  // Update config with constructor settings
  // Ensure the Font has been defined
  gslc_tsFont* pFont = gslc_FontGet(pGui,nFontId);
  if (pFont == NULL) {
    GSLC_DEBUG2_PRINT("ERROR: XKeyPadCreateBase(ID=%d): Font(ID=%d) not loaded\n",nElemId,nFontId);
    return NULL;
  }
  pConfig->nFontId = nFontId;
  pConfig->nOffsetX = nOffsetX;
  pConfig->nOffsetY = nOffsetY;

  // Save pointer to config struct
  // - Note that this will likely be a variant config (typecast to appear as base config)
  pXData->pConfig = pConfig;

  // Now proceed to add the element to the page
  if (nPage != GSLC_PAGE_NONE) {
    pElemRef = gslc_ElemAdd(pGui, nPage, &sElem, GSLC_ELEMREF_DEFAULT);
    return pElemRef;
  }
  return NULL;
}

void gslc_XKeyPadAdjustScroll(gslc_tsXKeyPad* pKeyPad)
{
  uint8_t nScrollPos = pKeyPad->nScrollPos;
  uint8_t nCursorPos = pKeyPad->nCursorPos;
  uint8_t nDispMax = pKeyPad->pConfig->nDispMax;

  if (nCursorPos < nScrollPos) {
    if (DEBUG_XKEYPAD) GSLC_DEBUG_PRINT("DBG: AdjustScroll to show left\n","");
    pKeyPad->nScrollPos = nCursorPos;
  } else if (nCursorPos >= (nScrollPos + nDispMax)) {
    if (DEBUG_XKEYPAD) GSLC_DEBUG_PRINT("DBG: AdjustScroll to show right\n","");
    pKeyPad->nScrollPos = nCursorPos - (nDispMax-1);
  }
}

bool gslc_XKeyPadLayoutSet(gslc_tsXKeyPadCfg* pConfig,int8_t eLayoutSel)
{
  int8_t eLayoutCur = pConfig->eLayoutSel;
  gslc_tsKey* pLayoutCur = pConfig->pLayouts[eLayoutCur];
  gslc_tsKey* pLayoutSel = pConfig->pLayouts[eLayoutSel];
  pConfig->eLayoutSel = eLayoutSel;
  pConfig->pLayout = pLayoutSel;

  // Indicate if a full layout change has occurred
  // - This is used by the caller to see if the entire control should be redrawn
  return (pLayoutCur != pLayoutSel);
}

void gslc_ElemXKeyPadValSet(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, const char* pStrBuf)
{
  gslc_tsXKeyPad* pKeyPad = (gslc_tsXKeyPad*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_KEYPAD, __LINE__);
  if (!pKeyPad) return;

  gslc_tsXKeyPadCfg* pConfig = pKeyPad->pConfig;

  // Copy over the new value string
  gslc_StrCopy(pKeyPad->acBuffer,pStrBuf,pKeyPad->nBufferMax);

  // Set the redraw status
  gslc_XKeyPadPendRedrawAddTxt(&(pKeyPad->sRedraw));
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_INC);

  // Handle any state initialization in the KeyPad variants
  // based on the string content
  GSLC_CB_XKEYPAD_TXT_INIT  pfuncTxtInit = pConfig->pfuncTxtInit;
  if (pfuncTxtInit != NULL) {
    (*pfuncTxtInit)((void*)pKeyPad);
  }

  // Update the current buffer position
  pKeyPad->nBufferLen = strlen(pKeyPad->acBuffer);

  // Set the cursor to the end of the buffer
  pKeyPad->nCursorPos = pKeyPad->nBufferLen;
  gslc_XKeyPadAdjustScroll(pKeyPad);

}


void gslc_ElemXKeyPadTargetRefSet(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, gslc_tsElemRef* pTxtRef)
{
  gslc_tsXKeyPad* pKeyPad = (gslc_tsXKeyPad*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_KEYPAD, __LINE__);
  if (!pKeyPad) return;
  pKeyPad->pTargetRef = pTxtRef;
}


int16_t gslc_ElemXKeyPadDataTargetIdGet(gslc_tsGui* pGui, void* pvData)
{
  if (pvData == NULL) {
    #if defined(DEBUG_LOG)
    GSLC_DEBUG2_PRINT("ERROR: XKeyPadDataTargetIdGet() NULL data\n", "");
    #endif
    return GSLC_ID_NONE;
  }
  gslc_tsXKeyPadData* pData = (gslc_tsXKeyPadData*)pvData;
  gslc_tsElemRef* pElemRef = pData->pTargetRef;
  gslc_tsElem* pElem = gslc_GetElemFromRefD(pGui, pElemRef, __LINE__);
  if (pElem == NULL) {
    return GSLC_ID_NONE;
  }
  return pElem->nId;
}

char* gslc_ElemXKeyPadDataValGet(gslc_tsGui* pGui, void* pvData)
{
  (void)pGui; // Unused
  if (pvData == NULL) {
    #if defined(DEBUG_LOG)
    GSLC_DEBUG2_PRINT("ERROR: XKeyPadDataValGet() NULL data\n", "");
    #endif
    return NULL;
  }
  gslc_tsXKeyPadData* pData = (gslc_tsXKeyPadData*)pvData;
  return pData->pStr;
}


// Fetch the current value of the keypad
// - Note that users will normally fetch the value through the gslc_ElemXKeyPadValGet()
//   as a result of a callback after pressing Enter.
// - gslc_ElemXKeyPadValGet can be useful to fetch the current value at
//   any time during operation.
bool gslc_ElemXKeyPadValGet(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, char* pStrBuf, uint8_t nStrBufLen)
{
  gslc_tsXKeyPad* pKeyPad = (gslc_tsXKeyPad*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_KEYPAD, __LINE__);
  if (!pKeyPad) return false;

  char* pBufPtr = pStrBuf;
  gslc_StrCopy(pBufPtr,pKeyPad->acBuffer,nStrBufLen);

  return true;
}


// Redraw the KeyPad element
// - When drawing a KeyPad we do not clear the background.
bool gslc_XKeyPadDraw(void* pvGui, void* pvElemRef, gslc_teRedrawType eRedraw)
{
  gslc_tsGui*       pGui  = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef*   pElemRef = (gslc_tsElemRef*)(pvElemRef);

  gslc_tsXKeyPad* pKeyPad = (gslc_tsXKeyPad*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_KEYPAD, __LINE__);
  if (!pKeyPad) return false;
  //gslc_tsXKeyPadCfg* pConfig = pKeyPad->pConfig;

  gslc_tsElem*    pElem = gslc_GetElemFromRef(pGui, pElemRef);

  // Determine the regions and colors based on element state
  gslc_tsRectState sState;
  gslc_ElemCalcRectState(pGui,pElemRef,&sState);


  // For full redraw, we draw the background and force
  // the fill to ensure the background doesn't bleed through
  // between the keys.
  if (eRedraw == GSLC_REDRAW_FULL) {
    gslc_DrawFillRect(pGui, sState.rInner, sState.colBack);

    // Also ensure the keypad's pending state is set to full
    pKeyPad->sRedraw.eRedrawState = XKEYPAD_REDRAW_FULL;
  }

  // For now, always redraw frame in order to enable focus/edit
  // transitions to be updated.
  // TODO: Optimize to only redraw frame if the focus/edit has changed.
  if (pElem->nFeatures & GSLC_ELEM_FEA_FRAME_EN) {
    gslc_DrawFrameRect(pGui, sState.rFull, sState.colFrm);
  }

  // Check to see if a key was in focus/glow but the overall keypad
  // is no longer in focus/glow. If so, we need to reset the key focus/glow
  // state. This can happen if we have used external input to focus on
  // a key, and then we use touch outside of the keypad element.
  #if (GSLC_FEATURE_INPUT)
  #if (GSLC_FEATURE_FOCUS_ON_TOUCH)
  if ( (!gslc_ElemGetFocus(pGui,pElemRef)) && (!gslc_ElemGetGlow(pGui,pElemRef)) ) {
    if (pKeyPad->nFocusKeyInd != GSLC_IND_NONE) {
      gslc_XKeyPadTrackSet(pGui,pElemRef,GSLC_IND_NONE,E_XKEYPAD_ATTRIB_FOCUS);
    }
    if (pKeyPad->nGlowKeyInd != GSLC_IND_NONE) {
      gslc_XKeyPadTrackSet(pGui,pElemRef,GSLC_IND_NONE,E_XKEYPAD_ATTRIB_GLOW);
    }
  }
  #endif // GSLC_FEATURE_FOCUS_ON_TOUCH
  #endif // GSLC_FEATURE_INPUT

  // Redraw the keypad
  // - If incremental, may only redraw the text field
  // - Otherwise, redraw everything
  gslc_XKeyPadDrawLayout(pGui, (void*)pKeyPad);

  // Clear the redraw flag
  gslc_ElemSetRedraw(pGui, pElemRef, GSLC_REDRAW_NONE);

  // Mark page as needing flip
  gslc_PageFlipSet(pGui, true);

  return true;
}

void gslc_ElemXKeyPadValSetCb(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, GSLC_CB_INPUT pfuncCb)
{
  gslc_tsXKeyPad* pKeyPad = (gslc_tsXKeyPad*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_KEYPAD, __LINE__);
  if (!pKeyPad) return;

  pKeyPad->pfuncCb = pfuncCb;
}

void gslc_XKeyPadSizeAllGet(gslc_tsKey** pLayouts, uint8_t nNumLayouts, uint8_t* pnRows, uint8_t* pnCols)
{
  uint8_t nMaxRows;
  uint8_t nMaxCols;
  int8_t nIndFirst,nIndLast; // Unused
  *pnRows = 0;
  *pnCols = 0;
  for (uint8_t nLayoutInd=0;nLayoutInd<nNumLayouts;nLayoutInd++) {
    gslc_XKeyPadSizeGet(pLayouts[nLayoutInd],&nMaxRows,&nMaxCols,&nIndFirst,&nIndLast);
    *pnRows = (nMaxRows > *pnRows)? nMaxRows : *pnRows;
    *pnCols = (nMaxCols > *pnCols)? nMaxCols : *pnCols;
  }
}

void gslc_XKeyPadSizeGet(gslc_tsKey* pLayout, uint8_t* pnRows, uint8_t* pnCols,int8_t* pnIndFirst,int8_t* pnIndLast)
{
  uint8_t nRow, nCol, nRowSpan, nColSpan;

  int16_t       nInd;
  bool          bDone;
  uint8_t       nType;
  gslc_tsKey    sKey;

  // Reset maximums
  *pnRows = 0;
  *pnCols = 0;

  // Reset indices
  *pnIndFirst = -1;
  *pnIndLast = -1;

  bDone = false;
  nInd = 0;
  while (!bDone) {
    sKey = pLayout[nInd];
    nType = sKey.nType;
    if (nType == E_XKEYPAD_TYPE_END) {
      bDone = true;
    } else {
      nRow     = sKey.nRow; 
      nCol     = sKey.nCol; 
      nRowSpan = sKey.nRowSpan;
      nColSpan = sKey.nColSpan;

      // Update maximum extents
      *pnRows = (nRow + nRowSpan >= *pnRows)? nRow + nRowSpan : *pnRows;
      *pnCols = (nCol + nColSpan >= *pnCols)? nCol + nColSpan : *pnCols;

      // Only latch valid keys
      if ((nType == E_XKEYPAD_TYPE_BASIC) || (nType == E_XKEYPAD_TYPE_SPECIAL)) {
        // Latch first
        if (*pnIndFirst == -1) {
          *pnIndFirst = nInd;
        }
        // Latch last
        *pnIndLast = nInd;
      }

      // Check next button
      nInd++;

    } // nType

  } // while !bDone
}


// Map keypad ID to its bounding rectangle position
int16_t gslc_XKeyPadMapEvent(gslc_tsGui* pGui, void* pXData, int16_t nRelX, int16_t nRelY, int16_t* pnInd)
{
  (void)pGui; // Unused
  gslc_tsXKeyPad* pKeyPad = (gslc_tsXKeyPad*)pXData;
  gslc_tsXKeyPadCfg* pConfig;

  pConfig = pKeyPad->pConfig;

  uint8_t nRow, nCol, nRowSpan, nColSpan;

  int16_t nButtonSzW = pConfig->nButtonSzW;
  int16_t nButtonSzH = pConfig->nButtonSzH;
  int16_t nOffsetX   = pConfig->nOffsetX;
  int16_t nOffsetY   = pConfig->nOffsetY;
  
  if (DEBUG_XKEYPAD) GSLC_DEBUG_PRINT("XKeyPadMapEvent: nRelX=%d nRelY=%d\n",nRelX,nRelY);

  // Scan for button (but skip over text field)
  uint8_t       nId;
  int16_t       nInd;
  bool          bDone;
  gslc_tsRect   selRect;
  uint8_t       nType;
  gslc_tsKey    sKey;

  bDone = false;
  nInd = 0;
  while (!bDone) {
    sKey = pConfig->pLayout[nInd];
    nId = sKey.nId;
    nType = sKey.nType;
    if (nType == E_XKEYPAD_TYPE_END) {
      bDone = true;
      return GSLC_ID_NONE; // Not found
    } else if (nType == E_XKEYPAD_TYPE_TXT) {
      // Skip over text field
      nInd++;
      continue;
    } else {
      // Basic and Special buttons
      nRow     = sKey.nRow; 
      nCol     = sKey.nCol; 
      nRowSpan = sKey.nRowSpan;
      nColSpan = sKey.nColSpan;
  
      int16_t nButtonW = (nColSpan * nButtonSzW);
      int16_t nButtonH = (nRowSpan * nButtonSzH);

      selRect.x = nOffsetX + (nCol*nButtonSzW);
      selRect.y = nOffsetY + (nRow*nButtonSzH);
      selRect.w = nButtonW - 1;
      selRect.h = nButtonH - 1;

      // Adjust for button spacing
      // TODO selRect = gslc_ExpandRect(selRect,-(pConfig->nButtonSpaceX),-(pConfig->nButtonSpaceY));
    
      if (DEBUG_XKEYPAD) GSLC_DEBUG_PRINT("  selRect.x=%d selRect.y=%d selRect.w=%d selRect.h=%d\n",selRect.x,selRect.y,selRect.w,selRect.h);

      if (gslc_IsInRect(nRelX,nRelY,selRect)) {
        if (DEBUG_XKEYPAD) GSLC_DEBUG_PRINT("  match: i=%d ID=%d\n", nInd, nId);
        *pnInd = nInd;
        return (int16_t)nId;
      } else {
        // No match, so check the next button
        nInd++;
      }

    } // nType

  } // while !bDone

  // Should not get here
  *pnInd = GSLC_IND_NONE;
  return GSLC_ID_NONE;
}

void gslc_XKeyPadPendRedrawReset(gslc_tsXKeyPadResult* pResult)
{
  if (!pResult) {
    return;
  }
  pResult->eRedrawState = XKEYPAD_REDRAW_NONE;
  pResult->nRedrawKeyId1 = GSLC_ID_NONE;
  pResult->nRedrawKeyId2 = GSLC_ID_NONE;
}

void gslc_XKeyPadPendRedrawAddTxt(gslc_tsXKeyPadResult* pResult)
{
  if (!pResult) {
    return;
  }
  pResult->eRedrawState = pResult->eRedrawState | XKEYPAD_REDRAW_TXT;
}

void gslc_XKeyPadPendRedrawAddKey(gslc_tsXKeyPadResult* pResult,int16_t nId)
{
  if (!pResult) {
    return;
  }
  pResult->eRedrawState = pResult->eRedrawState | XKEYPAD_REDRAW_KEY;
  // Skip the addition if this key is already pending
  if ((nId == pResult->nRedrawKeyId1) || (nId == pResult->nRedrawKeyId2)) {
    return;
  }
  // Add the key to the list in first available slot
  if (pResult->nRedrawKeyId1 == GSLC_ID_NONE) {
    pResult->nRedrawKeyId1 = nId;
  } else if (pResult->nRedrawKeyId2 == GSLC_ID_NONE) {
    pResult->nRedrawKeyId2 = nId;
  }
}

void gslc_XKeyPadRedrawUpdate(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef)
{
  gslc_tsXKeyPad* pKeyPad = (gslc_tsXKeyPad*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_KEYPAD, __LINE__);
  gslc_tsXKeyPadResult sResult;

  // Look at the pending redraws on the keypad
  sResult = pKeyPad->sRedraw;

  // Assign the GUIslice standard redraw flag
  gslc_teRedrawType eRedraw = GSLC_REDRAW_NONE;
  if (sResult.eRedrawState == XKEYPAD_REDRAW_NONE) {
    eRedraw = GSLC_REDRAW_NONE;
  } else if (sResult.eRedrawState & RBIT_CTRL) {
    eRedraw = GSLC_REDRAW_FULL;
  } else {
    eRedraw = GSLC_REDRAW_INC;
  }

  // Only add redraw requests. We don't want to call ElemSetRedraw(NONE)
  // as there could be a pending redraw (eg. for Edit mode) which
  // would be cleared.
  if (eRedraw != GSLC_REDRAW_NONE) {
    gslc_ElemSetRedraw(pGui,pElemRef,eRedraw);
  }
}

// Update the current key with a new attribute value, and mark both
// the old and new keys with a redraw pending state.
void gslc_XKeyPadTrackSet(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,int16_t nInd,gslc_tsXKeyPadAttrib eAttrib)
{
  gslc_tsXKeyPad* pKeyPad = (gslc_tsXKeyPad*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_KEYPAD, __LINE__);
  gslc_tsXKeyPadCfg* pConfig = pKeyPad->pConfig;

  // Add the old key to the pending redraw (if any)
  int16_t nIndOld = (eAttrib == E_XKEYPAD_ATTRIB_GLOW)? pKeyPad->nGlowKeyInd : pKeyPad->nFocusKeyInd;
  if (nIndOld != GSLC_IND_NONE) {
    // Mark the old key as needing redraw
    int16_t nIdOld = pConfig->pLayout[nIndOld].nId;
    gslc_XKeyPadPendRedrawAddKey(&(pKeyPad->sRedraw),nIdOld);
  }

  // Assign the new key
  if (eAttrib == E_XKEYPAD_ATTRIB_GLOW) {
    pKeyPad->nGlowKeyInd = nInd;
  } else {
    pKeyPad->nFocusKeyInd = nInd;
  }

  // Add the new key to the pending redraw
  if (nInd != GSLC_IND_NONE) {
    // Mark the new key as needing redraw
    int16_t nId = pConfig->pLayout[nInd].nId;
    gslc_XKeyPadPendRedrawAddKey(&(pKeyPad->sRedraw),nId);
  }

  // Now ensure the GUI element has been marked for redraw
  gslc_XKeyPadRedrawUpdate(pGui,pElemRef);

}

// Assign a default key to take focus
void gslc_XKeyPadFocusSetDefault(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef)
{
#if (GSLC_FEATURE_INPUT)
  gslc_tsXKeyPad* pKeyPad = (gslc_tsXKeyPad*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_KEYPAD, __LINE__);
  gslc_tsXKeyPadCfg* pConfig = pKeyPad->pConfig;
  uint8_t nMaxRows,nMaxCols;
  int8_t nIndFirst,nIndLast;

  gslc_XKeyPadSizeGet(pConfig->pLayout,&nMaxRows,&nMaxCols,&nIndFirst,&nIndLast);
  // Start focus at first valid key
  gslc_XKeyPadTrackSet(pGui,pElemRef,nIndFirst,E_XKEYPAD_ATTRIB_FOCUS);
#endif // GSLC_FEATURE_INPUT
}


// - This routine is called by gslc_ElemEvent() to handle
//   any click events that resulted from the touch tracking process.
// FIXME: Add back in support for glowing state?
bool gslc_XKeyPadTouch(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY)
{
  #if defined(DRV_TOUCH_NONE)
  return false;
  #else

  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem* pElem = gslc_GetElemFromRefD(pGui, pElemRef, __LINE__);
  gslc_tsRect rElem = pElem->rElem;
  
  int16_t nRelX = nX + rElem.x;
  int16_t nRelY = nY + rElem.y;

  gslc_tsXKeyPad* pKeyPad = (gslc_tsXKeyPad*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_KEYPAD, __LINE__);
  if (pKeyPad == NULL) return false;

  gslc_tsXKeyPadCfg* pConfig = pKeyPad->pConfig;

  uint8_t nMaxRows,nMaxCols;
  int8_t nIndFirst,nIndLast;
  int16_t nInd = GSLC_IND_NONE;
  int16_t nId;
  
  // Get a list of pending redraws
  gslc_tsXKeyPadResult* pResult = &(pKeyPad->sRedraw);

  // Intercept a focus-select and turn it into a key press event
  // If we are not in edit mode yet, then enter edit mode
  // If we are already in edit mode, then turn into key press event
  int16_t nForceId = GSLC_ID_NONE;
  #if (GSLC_FEATURE_INPUT)
  if (eTouch == GSLC_TOUCH_FOCUS_SELECT) {
    if (gslc_ElemGetEdit(pGui,pElemRef)) {
      // Fetch the key ID from the focused keypad index
      nForceId = pConfig->pLayout[pKeyPad->nFocusKeyInd].nId;
      // Turn off any glow
      gslc_XKeyPadTrackSet(pGui,pElemRef,GSLC_IND_NONE,E_XKEYPAD_ATTRIB_GLOW);

      // Continue on with TOUCH_UP_IN handler
      eTouch = GSLC_TOUCH_UP_IN;
    } else {
      // We are not in edit mode, so enter edit mode
      // but don't force any key presses yet
      gslc_XKeyPadFocusSetDefault(pGui,pElemRef);

      gslc_ElemSetEdit(pGui,pElemRef,true);
    }

  } else if (eTouch == GSLC_TOUCH_FOCUS_PRESELECT) {
    if (gslc_ElemGetEdit(pGui,pElemRef)) {
      // Show glow state in the currently-focused key
      gslc_XKeyPadTrackSet(pGui,pElemRef,pKeyPad->nFocusKeyInd,E_XKEYPAD_ATTRIB_GLOW);
      // Fetch the key ID from the focused keypad index
      nForceId = pConfig->pLayout[pKeyPad->nFocusKeyInd].nId;

      // Continue on with TOUCH_DOWN_IN handler
      eTouch = GSLC_TOUCH_DOWN_IN;
    }

  }
  #endif // GSLC_FEATURE_INPUT
  
  // Handle the various button presses
  // TODO: Also handle GSLC_TOUCH_DOWN_IN (for glow state)
  // TODO: Also handle GSLC_TOUCH_MOVE?
  if (eTouch == GSLC_TOUCH_SET_REL) {
    #if (GSLC_FEATURE_INPUT)
    int16_t nIndOld = pKeyPad->nFocusKeyInd;
    nInd = nIndOld;
    gslc_XKeyPadSizeGet(pConfig->pLayout,&nMaxRows,&nMaxCols,&nIndFirst,&nIndLast);
    if (nInd == GSLC_IND_NONE) {
      // No existing focus key has been assigned yet, so do it now
      nInd = nIndFirst;
    } else {
      if (nY > 0) {
        nInd = (nInd+1 > nIndLast)? nIndFirst : nInd+1;
      } else {
        nInd = (nInd-1 < nIndFirst)? nIndLast : nInd-1;
      }
    }

    gslc_XKeyPadTrackSet(pGui,pElemRef,nInd,E_XKEYPAD_ATTRIB_FOCUS);

    #endif // GSLC_FEATURE_INPUT

  } else if (eTouch == GSLC_TOUCH_DOWN_IN) {
    // This case is entered by both:
    // - GSLC_TOUCH_DOWN_IN and
    // - GSLC_TOUCH_FOCUS_PRESELECT

    if (nForceId == GSLC_ID_NONE) {
      // Key comes from a touch event
      // - Fetch the key index (nInd)
      nId = gslc_XKeyPadMapEvent(pGui, pKeyPad, nRelX, nRelY, &nInd);

      // We should also ensure that the keypad itself is in glow state
      gslc_ElemSetGlow(pGui,pElemRef,true);

      // Start the tracking (glow) on this key
      gslc_XKeyPadTrackSet(pGui,pElemRef,nInd,E_XKEYPAD_ATTRIB_GLOW);

    }

  } else if (eTouch == GSLC_TOUCH_MOVE) {
    // TODO: Later, could consider changing the tracked key

  } else if (eTouch == GSLC_TOUCH_UP_OUT) {

    // Release the glow state
    gslc_XKeyPadTrackSet(pGui,pElemRef,GSLC_IND_NONE,E_XKEYPAD_ATTRIB_GLOW);

    #if (GSLC_FEATURE_INPUT)
    // Clear any tracking state
    gslc_XKeyPadTrackSet(pGui,pElemRef,GSLC_IND_NONE,E_XKEYPAD_ATTRIB_GLOW);
    gslc_XKeyPadTrackSet(pGui,pElemRef,GSLC_IND_NONE,E_XKEYPAD_ATTRIB_FOCUS);
    #endif // GSLC_FEATURE_INPUT

    if (DEBUG_XKEYPAD) GSLC_DEBUG_PRINT("XKeyPadTouch: UP_OUT\n", "");

  } else if (eTouch == GSLC_TOUCH_UP_IN) {
    // This case is entered by both:
    // - GSLC_TOUCH_UP_IN
    // - GSLC_TOUCH_FOCUS_SELECT

    GSLC_CB_INPUT pfuncXInput = pKeyPad->pfuncCb;
    gslc_tsXKeyPadData sKeyPadData;

    // Determine if touch matches any fields on the keypad
    // - If nForceId is set, it means that a direct key ID was provided
    //   by external input (eg. pin input) rather than touch coordinate.
    if (nForceId == GSLC_ID_NONE) {
      // Key comes from a touch event

      // Release the glow state
      gslc_XKeyPadTrackSet(pGui,pElemRef,GSLC_IND_NONE,E_XKEYPAD_ATTRIB_GLOW);

      #if (GSLC_FEATURE_INPUT)
      // If we received a real touch release event, then we should
      // clear any focus states so that we don't get
      // interference between the two tracking mechanisms.
      gslc_XKeyPadTrackSet(pGui,pElemRef,GSLC_IND_NONE,E_XKEYPAD_ATTRIB_FOCUS);
      #endif // GSLC_FEATURE_INPUT

      // An external input forced a key ID
      nId = gslc_XKeyPadMapEvent(pGui, pKeyPad, nRelX, nRelY, &nInd);
    } else {
      // Key comes from external input
      nId = nForceId;
    }

    // Prepare the return data
    sKeyPadData.pStr = pKeyPad->acBuffer;
    sKeyPadData.pTargetRef = pKeyPad->pTargetRef;


    if (DEBUG_XKEYPAD) GSLC_DEBUG_PRINT("XKeyPadTouch: ID=%d\n", nId);

    GSLC_CB_XKEYPAD_BTN_EVT  pfuncBtnEvt = pConfig->pfuncBtnEvt;

    
    switch (nId) {

    case GSLC_ID_NONE:
      // No key
      break;

    case KEYPAD_ID_ENTER:
      if (DEBUG_XKEYPAD) GSLC_DEBUG_PRINT("  Key=ENT Done Str=[%s]\n", pKeyPad->acBuffer);

      #if (GSLC_FEATURE_INPUT)
      // Leave edit mode
      gslc_ElemSetEdit(pGui,pElemRef,false);
      #endif // GSLC_FEATURE_INPUT
   
      // Issue callback with Done status
      if (pfuncXInput != NULL) {
        (*pfuncXInput)(pvGui, (void*)(pElemRef), XKEYPAD_CB_STATE_DONE, (void*)(&sKeyPadData));
      }

      // Reset buffer and associated state
      gslc_ElemXKeyPadReset(pKeyPad);
      break;

    case KEYPAD_ID_SCROLL_RIGHT:
      // Ensure we don't step past the end of the buffer
      // - Note that the cursor ranges from 0..len(acBuffer)
      if (pKeyPad->nCursorPos < pKeyPad->nBufferLen) {
        pKeyPad->nCursorPos++;
      }
      // In case we have changed the cursor, adjust the scroll
      gslc_XKeyPadAdjustScroll(pKeyPad);
      if (DEBUG_XKEYPAD) GSLC_DEBUG_PRINT("  Key=ScrollR Src=[%s]/%d/%d CursorPos=%d ScrollPos=%d\n",
        pKeyPad->acBuffer,strlen(pKeyPad->acBuffer),pKeyPad->nBufferLen,pKeyPad->nCursorPos,pKeyPad->nScrollPos);
      gslc_XKeyPadPendRedrawAddTxt(pResult);
      break;

    case KEYPAD_ID_SCROLL_LEFT:
      // Ensure we don't step past the front of the buffer
      if (pKeyPad->nCursorPos > 0) {
        pKeyPad->nCursorPos--;
      }
      // In case we have changed the cursor, adjust the scroll
      gslc_XKeyPadAdjustScroll(pKeyPad);
      if (DEBUG_XKEYPAD) GSLC_DEBUG_PRINT("  Key=ScrollL Src=[%s]/%d/%d CurPos=%d ScrPos=%d\n",
        pKeyPad->acBuffer,strlen(pKeyPad->acBuffer),pKeyPad->nBufferLen,pKeyPad->nCursorPos,pKeyPad->nScrollPos);
      gslc_XKeyPadPendRedrawAddTxt(pResult);
      break;

    case KEYPAD_ID_ESC:
      if (DEBUG_XKEYPAD) GSLC_DEBUG_PRINT("  Key=ESC\n", "");

      #if (GSLC_FEATURE_INPUT)
      // Leave edit mode
      gslc_ElemSetEdit(pGui,pElemRef,false);
      #endif // GSLC_FEATURE_EDIT

      // Reset buffer and associated state
      gslc_ElemXKeyPadReset(pKeyPad);
      gslc_XKeyPadPendRedrawAddTxt(pResult);
      if (DEBUG_XKEYPAD) GSLC_DEBUG_PRINT("  Done Str=[%s]\n", pKeyPad->acBuffer);
      // Issue callback with Cancel status
      if (pfuncXInput != NULL) {
        (*pfuncXInput)(pvGui, (void*)(pElemRef), XKEYPAD_CB_STATE_CANCEL, (void*)(&sKeyPadData));
      }
      break;

    // Defer handling of all other buttons to variant
    default:
      // Normal character

      // For basic buttons, we need to fetch the keypad string index

      if (DEBUG_XKEYPAD) GSLC_DEBUG_PRINT("  Key=Other ID=%d Str act pos=%d before=[%s]\n",
        nId,pKeyPad->nBufferLen,pKeyPad->acBuffer);

      // Call the variant button handler
      if (pfuncBtnEvt != NULL) {
        (*pfuncBtnEvt)((void*)pKeyPad,nId,pResult);
      }

      // Note regarding backspace
      // - We let the variant handler (BtnEvt) take care of the main edit
      //   functions, which may in turn call DelCh(), updating the
      //   buffer length and cursor position.
      if (nId == KEYPAD_ID_BACKSPACE) {
        if (DEBUG_XKEYPAD) GSLC_DEBUG_PRINT("  Key=BACK\n","");
      }
      // In case we have changed the buffer or cursor, adjust the scroll
      gslc_XKeyPadAdjustScroll(pKeyPad);
      if (DEBUG_XKEYPAD) GSLC_DEBUG_PRINT("  Key Src=[%s]/%d/%d CursorPos=%d ScrollPos=%d\n",
        pKeyPad->acBuffer,strlen(pKeyPad->acBuffer),pKeyPad->nBufferLen,pKeyPad->nCursorPos,pKeyPad->nScrollPos);
      break;

    } // switch

  } // eTouch

  // Finalize the pending redraws
  gslc_XKeyPadRedrawUpdate(pGui,pElemRef);

  return true;
  #endif // !DRV_TOUCH_NONE
}

// Remove a character from the KeyPad text field at the selected position
// - May change the buffer & length
// - May change the cursor position
bool gslc_XKeyPadTxtDelCh(gslc_tsXKeyPad* pKeyPad,uint8_t nPos)
{
  uint8_t nCursorPos = pKeyPad->nCursorPos;
  uint8_t nSrcLen = pKeyPad->nBufferLen;
  bool bRedraw = false;

  if ((nPos == 0) || (nSrcLen == 0)) {
    // Nothing to do
  } else {
    // Shift source buffer at selected position left by one
    uint8_t nMoveLen = nSrcLen - nPos;
    if (DEBUG_XKEYPAD) GSLC_DEBUG_PRINT("XKeyPadDelCh @ %d: SrcLen=%d CursorPos=%d MoveLen=%d Before=[%s]\n",
      nPos,nSrcLen,nCursorPos,nMoveLen,pKeyPad->acBuffer);
    memmove(pKeyPad->acBuffer+nPos-1,pKeyPad->acBuffer+nPos,nMoveLen);

    // Update the buffer length
    pKeyPad->nBufferLen--;
    pKeyPad->acBuffer[pKeyPad->nBufferLen] = '\0'; // NULL terminate

    // Retreat the cursor
    // - Only do this if the deleted position was at or before the cursor
    if (nPos <= pKeyPad->nCursorPos) {
      pKeyPad->nCursorPos--;
    }

    if (DEBUG_XKEYPAD) GSLC_DEBUG_PRINT("XKeyPadDelCh: SrcLen=%d After=[%s]\n",
      pKeyPad->nBufferLen,pKeyPad->acBuffer);

    bRedraw = true;
  }
  return bRedraw;
}

// Add a character to the KeyPad text field at the selected position
// - May change the buffer & length
// - May change the cursor position
bool gslc_XKeyPadTxtAddCh(gslc_tsXKeyPad* pKeyPad,char ch,uint8_t nPos)
{
  uint8_t nCursorPos = pKeyPad->nCursorPos;
  uint8_t nBufLen = pKeyPad->nBufferLen;
  bool bRedraw = false;

  // Is there space for this character at the specified
  // position within the buffer?
  if (nPos+1 >= pKeyPad->nBufferMax) {
    // No space, so terminate
    if (DEBUG_XKEYPAD) GSLC_DEBUG_PRINT("XKeyPadTxtAddCh @ %d: no space for char at pos\n",nPos);
    return false;
  }

  // - Buffer is sized as: acBuffer[nBufferMax]
  // - Need to leave room for extra character as well as the NULL

  // Increment the total buffer length, but truncate at max (with NULL)
  uint8_t nBufLenNew = (nBufLen+1 >= pKeyPad->nBufferMax)? pKeyPad->nBufferMax-1 : nBufLen+1;

  // First, shift source buffer after selected position right by one
  int8_t nMoveLen = (nBufLenNew - nPos) - 1;
  if (DEBUG_XKEYPAD) GSLC_DEBUG_PRINT("XKeyPadTxtAddCh @ %d: SrcLen=%d SrcLenNew=%d CursorPos=%d MoveLen=%d Before=[%s]\n",
    nPos,nBufLen,nBufLenNew,nCursorPos,nMoveLen,pKeyPad->acBuffer);
  if (nMoveLen > 0) {
    if (DEBUG_XKEYPAD) GSLC_DEBUG_PRINT("  AddCh: move(d=%d,s=%d,len=%d)\n",
      nPos+1,nPos,nMoveLen);
    memmove(pKeyPad->acBuffer+nPos+1,pKeyPad->acBuffer+nPos,nMoveLen);

  } else {
    if (DEBUG_XKEYPAD) GSLC_DEBUG_PRINT("  AddCh: skip move\n","");
  }

  if (DEBUG_XKEYPAD) GSLC_DEBUG_PRINT("  AddCh: set buf[%d]\n",nPos);

  // Now insert the character
  pKeyPad->acBuffer[nPos] = ch;


  // Adjust the length
  pKeyPad->nBufferLen = nBufLenNew;
  pKeyPad->acBuffer[nBufLenNew] = '\0'; // NULL terminate

  // Advance the cursor
  // - Only do this if the inserted position was at or before the cursor
  if (DEBUG_XKEYPAD) GSLC_DEBUG_PRINT("  AddCh: cursor check: pos=%d CursorPos=%d\n",nPos,pKeyPad->nCursorPos);
  if (nPos <= pKeyPad->nCursorPos) {
    if (DEBUG_XKEYPAD) GSLC_DEBUG_PRINT("  AddCh:   cursor advance\n","");
    pKeyPad->nCursorPos++;
  } else {
    if (DEBUG_XKEYPAD) GSLC_DEBUG_PRINT("  AddCh:   skip cursor advance (no shift)\n","");
  }

  if (DEBUG_XKEYPAD) GSLC_DEBUG_PRINT("  AddCh: SrcLen=%d CursorPos=%d After=[%s]\n",
    pKeyPad->nBufferLen,pKeyPad->nCursorPos,pKeyPad->acBuffer);

  bRedraw = true;

  return bRedraw;
}

// Add a string (or UTF-8 character) to the current KeyPad txt field
bool gslc_XKeyPadTxtAddStr(gslc_tsXKeyPad* pKeyPad,const char* pStr,uint8_t nPos)
{
  uint8_t nInsertPos = nPos;
  uint8_t nStrLen = strlen(pStr);
  uint8_t nStrPos;
  bool bRedraw = false;
  for (nStrPos=0;nStrPos<nStrLen;nStrPos++) {
    bRedraw = bRedraw | gslc_XKeyPadTxtAddCh(pKeyPad,pStr[nStrPos],nInsertPos++);
  }
  return bRedraw;
}


void gslc_ElemXKeyPadCfgSetButtonSz(gslc_tsXKeyPadCfg* pConfig, int8_t nButtonSzW, int8_t nButtonSzH)
{
  pConfig->nButtonSzW = nButtonSzW;
  pConfig->nButtonSzH = nButtonSzH;
}

void gslc_ElemXKeyPadCfgSetButtonSpace(gslc_tsXKeyPadCfg* pConfig, int8_t nSpaceX, int8_t nSpaceY)
{
  pConfig->nButtonSpaceX = nSpaceX;
  pConfig->nButtonSpaceY = nSpaceY;
}

void gslc_ElemXKeyPadCfgSetRoundEn(gslc_tsXKeyPadCfg* pConfig, bool bEn)
{
  pConfig->bRoundEn = bEn;
}


void gslc_XKeyPadDrawVirtualTxt(gslc_tsGui* pGui,gslc_tsRect rElem,gslc_tsXKeyPad* pKeyPad,
  gslc_tsColor cColFrame, gslc_tsColor cColFill, gslc_tsColor cColTxt)
{
  gslc_tsElem  sElem;
  gslc_tsElem* pVirtualElem = &sElem;
  gslc_tsXKeyPadCfg* pConfig = pKeyPad->pConfig;

  // Create the display string from the main buffer
  uint8_t nSrcPos;
  char* pValStr = pKeyPad->acBuffer;
  uint8_t nDispPos;
  uint8_t nDispMax = pKeyPad->pConfig->nDispMax;
  char acDispStr[nDispMax+1]; // Dynamic allocation, include NULL
  uint8_t nCursorPos = pKeyPad->nCursorPos;
  uint8_t nScrollPos = pKeyPad->nScrollPos;
  uint8_t nSrcLen = pKeyPad->nBufferLen;
  #if (XKEYPAD_CURSOR_ENHANCED)
    #if (GSLC_LOCAL_STR==0)
      char acTempStr[nDispMax+1]; // Dynamic allocation, include NULL
    #endif
  #endif

  // ---
  // The generation of the display string is done in 3 parts
  // - Note that this could have been done in a single loop
  //   or with memmove calls, but that makes the range checking
  //   more involved.

  // Reset the display string
  nDispPos = 0;
  acDispStr[nDispPos] = '\0';
  int8_t nDispPosCursor = -1; // Default to no cursor visible

  // - Step1: Source buffer prior to cursor
  for (nSrcPos=nScrollPos;nSrcPos<nCursorPos;nSrcPos++) {
    if ((nSrcPos < nSrcLen) && (nDispPos < nDispMax)) {
      acDispStr[nDispPos++] = pValStr[nSrcPos];
    }
  }
  // - Step2: Insert the cursor
  if (nDispPos < nDispMax) {
    nDispPosCursor = nDispPos; // Save the cursor position within display
    acDispStr[nDispPos++] = XKEYPAD_CURSOR_CH;
  }
  // - Step3: Source buffer after cursor
  for (nSrcPos=nCursorPos;nSrcPos<nSrcLen;nSrcPos++) {
    if ((nSrcPos < nSrcLen) && (nDispPos < nDispMax)) {
      acDispStr[nDispPos++] = pValStr[nSrcPos];
    }
  }
  // ---

  // NULL terminate
  // - Note that we have left room for the NULL as acDispStr[]
  //   is defined to have size (nDispMax+1)
  acDispStr[nDispPos] = '\0';

  gslc_ResetElem(pVirtualElem);
  pVirtualElem->colElemFill       = cColFill;
  pVirtualElem->colElemFillGlow   = cColFill;
  pVirtualElem->colElemFrame      = cColFrame;
  pVirtualElem->colElemFrameGlow  = cColFrame;
  pVirtualElem->colElemText       = cColTxt;
  pVirtualElem->colElemTextGlow   = cColTxt;

  pVirtualElem->nFeatures         = GSLC_ELEM_FEA_NONE;
  pVirtualElem->nFeatures        |= GSLC_ELEM_FEA_FRAME_EN;
  pVirtualElem->nFeatures        |= GSLC_ELEM_FEA_FILL_EN;
  pVirtualElem->nFeatures        |= GSLC_ELEM_FEA_VALID;

  pVirtualElem->rElem             = rElem;
  pVirtualElem->pTxtFont          = gslc_FontGet(pGui,pConfig->nFontId);

  pVirtualElem->eTxtFlags         = GSLC_TXT_DEFAULT;
  pVirtualElem->eTxtFlags         = (pVirtualElem->eTxtFlags & ~GSLC_TXT_ALLOC) | GSLC_TXT_ALLOC_EXT;
  pVirtualElem->eTxtAlign         = GSLC_ALIGN_MID_LEFT;
  pVirtualElem->nTxtMarginX       = 2;
  pVirtualElem->nTxtMarginY       = 2;
  
  // Render the virtual element
  gslc_tsElemRef sElemRef;
  sElemRef.pElem = pVirtualElem;
  sElemRef.eElemFlags = GSLC_ELEMREF_SRC_RAM | GSLC_ELEMREF_VISIBLE;

  // Two different keypad cursor modes are available
  // - XKEYPAD_CURSOR_ENHANCED=0: basic embedded cursor (same color as text)
  // - XKEYPAD_CURSOR_ENHANCED=1: multi-color embedded cursor

  #if (XKEYPAD_CURSOR_ENHANCED == 0)
   // In the basic cursor mode, the element's text buffer
   // is simply updated to the full display string.

   // In this mode, the cursor is the same color
   // as the text. Proportional fonts should be supported.

   // In GSLC_LOCAL_STR mode, pStrBuf is a local character
   // array, so we need to perform a deep string copy here.
   // When not in GSLC_LOCAL_STR mode, pStrBuf is just a
   // pointer, so we can simply update the pointer.
   #if (GSLC_LOCAL_STR)
     // Deep copy
     gslc_StrCopy(pVirtualElem->pStrBuf,acDispStr,GSLC_LOCAL_STR_LEN);
   #else
     // Shallow copy 
     pVirtualElem->nStrBufMax = 0; // Read-only string buffer
     pVirtualElem->pStrBuf = acDispStr;
   #endif
   gslc_ElemDrawByRef(pGui,&sElemRef,GSLC_REDRAW_FULL);

   nDispPosCursor; // Avoid unused warning

  #else
    // In the enhanced cursor mode, the element's text buffer
    // is updated three times during rendering.
  
    // In this mode, we support a differently-colored
    // cursor versus the text. But we also have to make assumptions
    // about each character width, meaning that proportional fonts
    // won't be supported.
    //
    // In order to do this, we render the text string in 3 parts:
    // - The text prior to the cursor
    // - The cursor itself (potentially in a different color)
    // - Any text after the cursor
    // To leverage the existing code, we are rendering the
    // virtual element three times, each with different parameters
  
    int8_t nTxtLen;
    uint16_t nTxtSzW = 0;
    int16_t nTxtPosX = 0;
  
    #if (GSLC_LOCAL_STR)
      // Use the buffer internal to the element
      char* pStrBuf = pVirtualElem->pStrBuf;
      // Note that in GSLC_LOCAL_STR mode, pVirtualElem->pStrBuf
      // is actually an internal character array, so we don't
      // need to update it here.
      //int8_t nStrBufMax = GSLC_LOCAL_STR_LEN;
  
      // IMPORTANT NOTE:
      // - It is assumed that GSLC_LOCAL_STR_LEN will be at least
      //   as large as the maximum display length (nDispMax).
      // - To limit the extent of code dedicated to error checking,
      //   the StrCopy() calls are not currently checking for
      //   MIN(nStrBufMax,nTxtLen), but this could be added.
    #else
      // Use the dynamically allocated buffer
      char* pStrBuf = acTempStr;
      // Update the virtual element to use this buffer
      pVirtualElem->pStrBuf = pStrBuf;
      //int8_t nStrBufMax = nDispMax+1;
    #endif
  
    // Clear out content first with empty framed baackground
    strcpy(pStrBuf,"");
  
    gslc_ElemDrawByRef(pGui,&sElemRef,GSLC_REDRAW_FULL);
  
    // For the interior context, we no longer want any frames
    // - Also we shift the nTxtX by +1, nTxtY by +1 and
    //   nTxtH by -2 to inset the text within the above frame
    pVirtualElem->nFeatures &= ~GSLC_ELEM_FEA_FRAME_EN;
  
    // Since we are splitting up the text field render into
    // three parts, we need to disable the default "marginX"
    // calculation in ElemDrawByRef, otherwise the margin
    // will be accounted for multiple times. Instead, we
    // clear the margin value but manually offset each render.
    int16_t nMarginX = pVirtualElem->nTxtMarginX;
    pVirtualElem->nTxtMarginX = 0;
  
    // TODO: Determine if/how we should handle:
    // - TxtOffsetX/Y
    // Note that ElemDrawByRef() already calls DrvGetTxtSize()
    // which utilizes the nTxtOffsetX/Y
  
    int16_t       nTxtOffsetX=0;
    int16_t       nTxtOffsetY=0;
    uint16_t      nTxtSzH=0;
  
    // Part 1: text before the cursor
    // - The width of the first section also includes the
    //   margin on the left side
    nTxtLen = nDispPosCursor;
    gslc_StrCopy(pStrBuf,acDispStr,nTxtLen+1);
  
    gslc_DrvGetTxtSize(pGui,pVirtualElem->pTxtFont,pStrBuf,pVirtualElem->eTxtFlags,&nTxtOffsetX,&nTxtOffsetY,&nTxtSzW,&nTxtSzH);
    nTxtPosX += nMarginX; // Account for margin once at start
  
    pVirtualElem->rElem = (gslc_tsRect){rElem.x+1+nTxtPosX,rElem.y+1,nTxtSzW,rElem.h-2};
    pVirtualElem->colElemText = cColTxt;
    gslc_ElemDrawByRef(pGui,&sElemRef,GSLC_REDRAW_FULL);
    nTxtPosX += nTxtSzW;
  
    // Part 2: the cursor
    nTxtLen = 1;
    gslc_StrCopy(pStrBuf,acDispStr+nDispPosCursor,nTxtLen+1);
  
    gslc_DrvGetTxtSize(pGui,pVirtualElem->pTxtFont,pStrBuf,pVirtualElem->eTxtFlags,&nTxtOffsetX,&nTxtOffsetY,&nTxtSzW,&nTxtSzH);
  
    pVirtualElem->rElem = (gslc_tsRect){rElem.x+1+nTxtPosX,rElem.y+1,nTxtSzW,rElem.h-2};
    pVirtualElem->colElemText = cColFrame;
    gslc_ElemDrawByRef(pGui,&sElemRef,GSLC_REDRAW_FULL);
    nTxtPosX += nTxtSzW;
  
    // Part 3: text after cursor
    nTxtLen = strlen(acDispStr)-nDispPosCursor;
    gslc_StrCopy(pStrBuf,acDispStr+nDispPosCursor+1,nTxtLen+1);
  
    gslc_DrvGetTxtSize(pGui,pVirtualElem->pTxtFont,pStrBuf,pVirtualElem->eTxtFlags,&nTxtOffsetX,&nTxtOffsetY,&nTxtSzW,&nTxtSzH);
  
    pVirtualElem->rElem = (gslc_tsRect){rElem.x+1+nTxtPosX,rElem.y+1,nTxtSzW,rElem.h-2};
    pVirtualElem->colElemText = cColTxt;
    gslc_ElemDrawByRef(pGui,&sElemRef,GSLC_REDRAW_FULL);
    nTxtPosX += nTxtSzW;
  
    
  #endif // XKEYPAD_CURSOR_ENHANCED

}

void gslc_XKeyPadDrawVirtualBtn(gslc_tsGui* pGui, gslc_tsRect rElem,
  char* pStrBuf,uint8_t nStrBufMax,int16_t nFontId, gslc_tsColor cColFrame,
  gslc_tsColor cColFill, gslc_tsColor cColFillGlow, gslc_tsColor cColText,
  bool bRoundedEn, bool bGlow, bool bFocus)
{
  gslc_tsElem  sElem;
  gslc_tsElem* pVirtualElem = &sElem;

  gslc_ResetElem(pVirtualElem);
  pVirtualElem->colElemFill       = cColFill;
  pVirtualElem->colElemFillGlow   = cColFillGlow;
  pVirtualElem->colElemFrame      = cColFrame;
  pVirtualElem->colElemFrameGlow  = cColFrame;
  pVirtualElem->colElemText       = cColText;
  pVirtualElem->colElemTextGlow   = cColText;
  pVirtualElem->nFeatures         = GSLC_ELEM_FEA_NONE;
  pVirtualElem->nFeatures        |= GSLC_ELEM_FEA_FRAME_EN;
  pVirtualElem->nFeatures        |= GSLC_ELEM_FEA_FILL_EN;
  pVirtualElem->nFeatures        |= GSLC_ELEM_FEA_VALID;
  pVirtualElem->nFeatures        |= GSLC_ELEM_FEA_GLOW_EN;
  pVirtualElem->nFeatures        |= GSLC_ELEM_FEA_FOCUS_EN;

  pVirtualElem->nType             = GSLC_TYPE_BTN;

  pVirtualElem->rElem             = rElem;
  pVirtualElem->pTxtFont          = gslc_FontGet(pGui,nFontId);

  pVirtualElem->nStrBufMax        = nStrBufMax;
  #if (GSLC_LOCAL_STR)
    // Deep copy
    gslc_StrCopy(pVirtualElem->pStrBuf,pStrBuf,GSLC_LOCAL_STR_LEN);
  #else
    // Shallow copy 
    pVirtualElem->pStrBuf         = pStrBuf;
  #endif

  pVirtualElem->eTxtFlags         = GSLC_TXT_DEFAULT;
  pVirtualElem->eTxtFlags         = (pVirtualElem->eTxtFlags & ~GSLC_TXT_ALLOC) | GSLC_TXT_ALLOC_EXT;
  pVirtualElem->eTxtAlign         = GSLC_ALIGN_MID_MID;
  pVirtualElem->nTxtMarginX       = 0;
  pVirtualElem->nTxtMarginY       = 0;

  // For the text buttons, optionally use rounded profile if enabled
  if (bRoundedEn) {
    pVirtualElem->nFeatures |= GSLC_ELEM_FEA_ROUND_EN;
  } else {
    pVirtualElem->nFeatures &= ~GSLC_ELEM_FEA_ROUND_EN;
  }

  // Render the virtual element
  gslc_tsElemRef sElemRef;
  sElemRef.pElem = pVirtualElem;
  sElemRef.eElemFlags = GSLC_ELEMREF_SRC_RAM | GSLC_ELEMREF_VISIBLE;

  // Update any other element reference states
  gslc_ElemSetGlow(pGui,&sElemRef,bGlow);
  gslc_ElemSetFocus(pGui,&sElemRef,bFocus);

  gslc_ElemDrawByRef(pGui,&sElemRef,GSLC_REDRAW_FULL);

}

void gslc_ElemXKeyPadInputAsk(gslc_tsGui* pGui, gslc_tsElemRef* pKeyPadRef, int16_t nPgPopup, gslc_tsElemRef* pTxtRef)
{
  // Associate the keypad with the text field
  gslc_ElemXKeyPadTargetRefSet(pGui, pKeyPadRef, pTxtRef);

  // Set the maximum buffer length based on the target text field size
  // - The upper limit is defined by XKEYPAD_BUF_MAX
  gslc_tsElem* pKeyPad = gslc_GetElemFromRef(pGui,pKeyPadRef);
  gslc_tsXKeyPad* pKeyPadData = (gslc_tsXKeyPad*)(pKeyPad->pXData);
  gslc_tsElem* pTxtElem = gslc_GetElemFromRef(pGui,pTxtRef);
  pKeyPadData->nBufferMax = (pTxtElem->nStrBufMax >= XKEYPAD_BUF_MAX) ? XKEYPAD_BUF_MAX : pTxtElem->nStrBufMax;

  // Initialize KeyPad state to defaults
  GSLC_CB_XKEYPAD_RESET  pfuncReset = pKeyPadData->pConfig->pfuncReset;
  if (pfuncReset != NULL) {
    (*pfuncReset)((void*)(pKeyPadData->pConfig));
  }

  // Start with the default layout
  gslc_XKeyPadLayoutSet(pKeyPadData->pConfig,pKeyPadData->pConfig->eLayoutDef);

  // Show a popup box for the keypad
  gslc_PopupShow(pGui, nPgPopup, true);

  // Preload text field with current value
  gslc_ElemXKeyPadValSet(pGui, pKeyPadRef, gslc_ElemGetTxtStr(pGui, pTxtRef));
}

char* gslc_ElemXKeyPadInputGet(gslc_tsGui* pGui, gslc_tsElemRef* pTxtRef, void* pvCbData)
{
  char* pStr = NULL;

  // Fetch the current value of the keypad popup
  pStr = gslc_ElemXKeyPadDataValGet(pGui, pvCbData);

  // Update the linked text item
  gslc_ElemSetTxtStr(pGui, pTxtRef, pStr);

  // Hide the popup
  gslc_PopupHide(pGui);

  // Return the text string in case the user wants it
  return pStr;
}


// ============================================================================
