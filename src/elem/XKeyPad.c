// =======================================================================
// GUIslice library extension: XKeyPad control
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
// - Keypad element with numeric input
// - Optionally supports floating point values
// - Optionally supports negative values
// ============================================================================

// Define the button labels
// - TODO: Create more efficient storage for the labels
//   so that it doesn't consume 4 bytes even for labels
//   that can be generated (eg. 0..9, a--z, etc.)
// - NOTE: Not using "const char" in order to support
//   modification of labels by user.


const char KEYPAD_DISP_NEGATIVE = '-';
const char KEYPAD_DISP_DECIMAL_PT = '.';



// --------------------------------------------------------------------------
// Create the keypad definition
// --------------------------------------------------------------------------


void XKeyPadAddKeyElem(gslc_tsGui* pGui, gslc_tsXKeyPad* pXData, int16_t nKeyId, bool bTxtField, int16_t nRow, int16_t nCol, int8_t nRowSpan, int8_t nColSpan,
  gslc_tsColor cColFill, gslc_tsColor cColGlow, bool bVisible)
{
  gslc_tsXKeyPadCfg* pConfig;
  char* pKeyStr = NULL;

  pConfig = &(pXData->sConfig);

  gslc_tsElemRef* pElemRefTmp = NULL;
  gslc_tsElem*    pElemTmp = NULL;

  int16_t nButtonSzW = pConfig->nButtonSzW;
  int16_t nButtonSzH = pConfig->nButtonSzH;
  int16_t nOffsetX = pConfig->nOffsetX;
  int16_t nOffsetY = pConfig->nOffsetY;
  int8_t nFontId = pConfig->nFontId;

  int16_t nButtonW = (nColSpan * nButtonSzW);
  int16_t nButtonH = (nRowSpan * nButtonSzH);

  // Fetch the keypad key string
  if (bTxtField) {
    pKeyStr = (char*)pXData->acValStr;
  } else {
    XKEYPAD_LOOKUP pfuncLookup = pXData->pfuncLookup;
    int16_t nKeyInd = (*pfuncLookup)(pGui, nKeyId);
    pKeyStr = pXData->pacKeys[nKeyInd];
  }

  //GSLC_DEBUG2_PRINT("DBG: KeyId=%d R=%d C=%d str=[%s] SubElemMax=%d\n", nKeyId, nRow,nCol,pKeyStr,pXData->nSubElemMax);

  if (bTxtField) {
    // Text field
    pElemRefTmp = gslc_ElemCreateTxt(pGui, nKeyId, GSLC_PAGE_NONE,
      (gslc_tsRect) { nOffsetX + (nCol*nButtonSzW), nOffsetY + (nRow*nButtonSzH), nButtonW-1, nButtonH - 1 },
      pKeyStr, XKEYPAD_VAL_LEN, nFontId);
    gslc_ElemSetFrameEn(pGui, pElemRefTmp, true);
    gslc_ElemSetTxtMargin(pGui, pElemRefTmp, 5); // Apply default margin
  } else {
    // Text button
    //GSLC_DEBUG2_PRINT("DrawBtn: %d [%s] @ r=%d,c=%d,width=%d\n", nKeyId, pXData->pacKeys[nKeyId], nRow, nCol, nButtonW);
    pElemRefTmp = gslc_ElemCreateBtnTxt(pGui, nKeyId, GSLC_PAGE_NONE,
      (gslc_tsRect) { nOffsetX + (nCol*nButtonSzW), nOffsetY + (nRow*nButtonSzH), nButtonW - 1, nButtonH - 1 },
      pKeyStr, sizeof(pKeyStr), nFontId, &gslc_ElemXKeyPadClick);
    // For the text buttons, optionally use rounded profile if enabled
    gslc_ElemSetRoundEn(pGui, pElemRefTmp, pConfig->bRoundEn);
  }

  // Set color
  gslc_ElemSetTxtCol(pGui, pElemRefTmp, GSLC_COL_WHITE);
  gslc_ElemSetCol(pGui, pElemRefTmp, GSLC_COL_WHITE, cColFill, cColGlow);

  // Set the visibility status
  // FIXME: Need to fix dynamic visibility change
  // gslc_ElemSetVisible(pGui, pElemRefTmp, bVisible);
  if (!bVisible) {
    // For now, skip addition of invisible buttons
	  return;
  }

  // Add element to compound element collection
  pElemTmp = gslc_GetElemFromRef(pGui, pElemRefTmp);
  pElemRefTmp = gslc_CollectElemAdd(pGui, &pXData->sCollect, pElemTmp, GSLC_ELEMREF_DEFAULT);
}

gslc_tsElemRef* gslc_ElemXKeyPadCreateBase(gslc_tsGui* pGui, int16_t nElemId, int16_t nPage,
  gslc_tsXKeyPad* pXData, int16_t nX0, int16_t nY0, int8_t nFontId, gslc_tsXKeyPadCfg* pConfig,
  XKEYPAD_CREATE pfuncCreate, XKEYPAD_LOOKUP pfuncLookup)
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

  // Initialize composite element
  sElem = gslc_ElemCreate(pGui, nElemId, nPage, GSLC_TYPEX_KEYPAD, rElem, NULL, 0, GSLC_FONT_NONE);
  sElem.nFeatures |= GSLC_ELEM_FEA_FRAME_EN;
  sElem.nFeatures |= GSLC_ELEM_FEA_FILL_EN;
  sElem.nFeatures |= GSLC_ELEM_FEA_CLICK_EN;
  sElem.nFeatures &= ~GSLC_ELEM_FEA_GLOW_EN;  // Don't need to glow outer element
  sElem.nGroup = GSLC_GROUP_ID_NONE;

  gslc_CollectReset(&pXData->sCollect, pXData->psElem, pXData->nSubElemMax, pXData->psElemRef, pXData->nSubElemMax);

  sElem.pXData = (void*)(pXData);
  // Specify the custom drawing callback
  sElem.pfuncXDraw = &gslc_ElemXKeyPadDraw;
  // Specify the custom touch tracking callback
  sElem.pfuncXTouch = &gslc_ElemXKeyPadTouch;

  // shouldn't be used
  sElem.colElemFill = GSLC_COL_BLACK;
  sElem.colElemFillGlow = GSLC_COL_BLACK;
  sElem.colElemFrame = GSLC_COL_BLUE;
  sElem.colElemFrameGlow = GSLC_COL_BLUE_LT4;

  // Now create the sub elements
  gslc_tsElemRef* pElemRef = NULL;

  // Determine offset coordinate of compound element so that we can
  // specify relative positioning during the sub-element Create() operations.
  int16_t nOffsetX = nX0 + pConfig->nFrameMargin;
  int16_t nOffsetY = nY0 + pConfig->nFrameMargin;

  // Reset value string
  memset(pXData->acValStr, 0, XKEYPAD_VAL_LEN);
  pXData->nValStrPos = 0;

  pXData->pacKeys = pConfig->pacKeys;
  pXData->pfuncCb = NULL;
  pXData->pfuncLookup = pfuncLookup;
  pXData->nTargetId = GSLC_ID_NONE;  // Default to no target defined
  pXData->bValPositive = true;
  pXData->bValDecimalPt = false;

  // Update config with constructor settings
  pConfig->nFontId = nFontId;
  pConfig->nOffsetX = nOffsetX;
  pConfig->nOffsetY = nOffsetY;


  // Copy content into local structure
  pXData->sConfig = *pConfig;

  // Create the keypad definition
  // -- Call XKeyPadCreateKeys()
  if (pfuncCreate != NULL) {
    (*pfuncCreate)(pGui, pXData);
  }

  // Now proceed to add the compound element to the page
  if (nPage != GSLC_PAGE_NONE) {
    pElemRef = gslc_ElemAdd(pGui, nPage, &sElem, GSLC_ELEMREF_DEFAULT);

    // Now propagate the parent relationship to enable a cascade
    // of redrawing from low-level elements to the top
    gslc_CollectSetParent(pGui, &pXData->sCollect, pElemRef);
    return pElemRef;
  }
  else {
    #if defined(DEBUG_LOG)
    GSLC_DEBUG2_PRINT("ERROR: gslc_ElemXKeyPadCreate(%s) Compound elements inside compound elements not supported\n", "");
    #endif
    return NULL;
  }

}

void gslc_ElemXKeyPadValSet(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, const char* pStrBuf)
{
  gslc_tsXKeyPad* pKeyPad = (gslc_tsXKeyPad*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_KEYPAD, __LINE__);
  if (!pKeyPad) return;

  // Copy over the new value string
  strncpy(pKeyPad->acValStr, pStrBuf, XKEYPAD_VAL_LEN);
  pKeyPad->acValStr[XKEYPAD_VAL_LEN - 1] = 0; // Force null terminate

  // Handle negation and floating point detection
  pKeyPad->bValPositive = true;
  if (pKeyPad->acValStr[0] == KEYPAD_DISP_NEGATIVE) {
    pKeyPad->bValPositive = false;
  }
  pKeyPad->bValDecimalPt = false;
  if (strchr(pKeyPad->acValStr, KEYPAD_DISP_DECIMAL_PT)) {
    pKeyPad->bValDecimalPt = true;
  }

  // Update the current buffer position
  pKeyPad->nValStrPos = strlen(pKeyPad->acValStr);

  // Find element associated with text field
  gslc_tsElemRef* pTxtElemRef = gslc_CollectFindElemById(pGui, &pKeyPad->sCollect, KEYPAD_ID_TXT);

  // Mark as needing redraw
  gslc_ElemSetRedraw(pGui,pTxtElemRef,GSLC_REDRAW_INC);
}

void gslc_ElemXKeyPadTargetIdSet(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, int16_t nTargetId)
{
  gslc_tsXKeyPad* pKeyPad = (gslc_tsXKeyPad*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_KEYPAD, __LINE__);
  if (!pKeyPad) return;
  pKeyPad->nTargetId = nTargetId;
}

int16_t gslc_ElemXKeyPadDataTargetIdGet(gslc_tsGui* pGui, void* pvData)
{
  if (pvData == NULL) {
    #if defined(DEBUG_LOG)
    GSLC_DEBUG2_PRINT("ERROR: gslc_ElemXKeyPadDataTargetIdGet() NULL data\n", "");
    #endif
    return GSLC_ID_NONE;
  }
  gslc_tsXKeyPadData* pData = (gslc_tsXKeyPadData*)pvData;
  return pData->nTargetId;
}

char* gslc_ElemXKeyPadDataValGet(gslc_tsGui* pGui, void* pvData)
{
  if (pvData == NULL) {
    #if defined(DEBUG_LOG)
    GSLC_DEBUG2_PRINT("ERROR: gslc_ElemXKeyPadDataValGet() NULL data\n", "");
    #endif
    return NULL;
  }
  gslc_tsXKeyPadData* pData = (gslc_tsXKeyPadData*)pvData;
  return pData->pStr;
}



// NOTE: API changed to pass nStrBufLen (total buffer size including terminator)
//       instead of nStrBufMax (max index value)
bool gslc_ElemXKeyPadValGet(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, char* pStrBuf, uint8_t nStrBufLen)
{
  gslc_tsXKeyPad* pKeyPad = (gslc_tsXKeyPad*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_KEYPAD, __LINE__);
  if (!pKeyPad) return false;

  // - Check for negation flag
  // - If negative, copy minus sign
  // - Copy the remainder of the string
  // FIXME: check for shortest of XKEYPAD_VAL_LEN & nStrBufLen
  char* pBufPtr = pStrBuf;
  uint8_t nMaxCopyLen = nStrBufLen - 1;
  // FIXME: Do we still need to do this step?
  if (!pKeyPad->bValPositive) {
    *pBufPtr = KEYPAD_DISP_NEGATIVE;
    pBufPtr++;
    nMaxCopyLen--;
  }
  strncpy(pBufPtr, pKeyPad->acValStr, nMaxCopyLen);
  pStrBuf[nStrBufLen-1] = '\0';  // Force termination

  // TODO: Do we need to reset the contents?

  return true;
}


// Redraw the KeyPad element
// - When drawing a KeyPad we do not clear the background.
//   We do redraw the sub-element collection.
bool gslc_ElemXKeyPadDraw(void* pvGui, void* pvElemRef, gslc_teRedrawType eRedraw)
{
  gslc_tsGui*       pGui  = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef*   pElemRef = (gslc_tsElemRef*)(pvElemRef);

  gslc_tsXKeyPad* pKeyPad = (gslc_tsXKeyPad*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_KEYPAD, __LINE__);
  if (!pKeyPad) return false;

  gslc_tsElem*    pElem = gslc_GetElemFromRef(pGui, pElemRef);

  // Draw any parts of the compound element itself
  if (eRedraw == GSLC_REDRAW_FULL) {
    // Force the fill to ensure the background doesn't bleed thorugh gaps
    // between the sub-elements
    gslc_DrawFillRect(pGui, pElem->rElem, pElem->colElemFill);
    if (pElem->nFeatures & GSLC_ELEM_FEA_FRAME_EN) {
      gslc_DrawFrameRect(pGui, pElem->rElem, pElem->colElemFrame);
    }
  }

  // Draw the sub-elements
  gslc_tsCollect* pCollect = &pKeyPad->sCollect;

  if (eRedraw != GSLC_REDRAW_NONE) {
    uint8_t sEventSubType = GSLC_EVTSUB_DRAW_NEEDED;
    if (eRedraw == GSLC_REDRAW_FULL) {
      sEventSubType = GSLC_EVTSUB_DRAW_FORCE;
    }
    gslc_tsEvent  sEvent = gslc_EventCreate(pGui, GSLC_EVT_DRAW, sEventSubType, (void*)(pCollect), NULL);
    gslc_CollectEvent(pGui, sEvent);
  }

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

// Change the sign of the number string
// - This function also performs in-place shifting of the content
void gslc_ElemXKeyPadValSetSign(gslc_tsGui* pGui, gslc_tsElemRef *pElemRef, bool bPositive)
{
  gslc_tsXKeyPad* pKeyPad = (gslc_tsXKeyPad*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_KEYPAD, __LINE__);
  if (!pKeyPad) return;

  char* pStrBuf = pKeyPad->acValStr;

  //GSLC_DEBUG2_PRINT("SetSign: old=%d new=%d\n", pKeyPad->bValPositive, bPositive);
  if (pKeyPad->bValPositive == bPositive) {
    // No change to sign
    return;
  }

  //GSLC_DEBUG2_PRINT("SetSign:   str_old=[%s] idx=%d\n", pStrBuf,pKeyPad->nValStrPos);

  if ((pKeyPad->bValPositive) && (!bPositive)) {
    // Change from positive to negative
    // - Shift string right one position, and replace first position with minus sign
    // - Increase current buffer position
    memmove(pStrBuf+1, pStrBuf, XKEYPAD_VAL_LEN-1);
    pStrBuf[0] = KEYPAD_DISP_NEGATIVE; // Overwrite with minus sign
    pStrBuf[XKEYPAD_VAL_LEN-1] = 0; // Force terminate
    pKeyPad->nValStrPos++; // Advance buffer position
  } else if ((!pKeyPad->bValPositive) && (bPositive)) {
    // Change from negative to positive
    // - Shift string left one position, overwriting the minus sign
    memmove(pStrBuf, pStrBuf+1, XKEYPAD_VAL_LEN-1);
    pStrBuf[XKEYPAD_VAL_LEN-1] = 0; // Force terminate
    if (pKeyPad->nValStrPos > 0) {
      // Expect that original position should be non-zero if previously minux
      pKeyPad->nValStrPos--; // Reduce buffer position
    }
  }

  //GSLC_DEBUG2_PRINT("SetSign:   str_new=[%s] idx=%d\n", pStrBuf,pKeyPad->nValStrPos);

  // Update sign state
  pKeyPad->bValPositive = bPositive;

  // The text string sub-element content has already been updated,
  // so now we can simply mark it for redraw
  gslc_tsElemRef* pTxtElemRef = gslc_CollectFindElemById(pGui, &pKeyPad->sCollect, KEYPAD_ID_TXT);
  gslc_ElemSetRedraw(pGui,pTxtElemRef,GSLC_REDRAW_INC);

}

bool ElemXKeyPadAddChar(gslc_tsGui* pGui, gslc_tsXKeyPad* pKeyPad, char ch)
{
  bool bRedraw = false;
  // Do we have space for this character?
  if (pKeyPad->nValStrPos < XKEYPAD_VAL_LEN - 1) {
    pKeyPad->acValStr[pKeyPad->nValStrPos] = ch;
    pKeyPad->nValStrPos++;
    pKeyPad->acValStr[pKeyPad->nValStrPos] = '\0'; // Zero terminate
    bRedraw = true;
  }
  return bRedraw;
}

// Handle the compound element main functionality
// - This routine is called by gslc_ElemEvent() to handle
//   any click events that resulted from the touch tracking process.
// - The code here will generally represent the core
//   functionality of the compound element and any communication
//   between sub-elements.
// - pvElemRef is a void pointer to the element ref being tracked. From
//   the pElemRefParent member we can get the parent/compound element
//   data structures.
bool gslc_ElemXKeyPadClick(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY)
{
  #if defined(DRV_TOUCH_NONE)
  return false;
  #else

  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem* pElem = gslc_GetElemFromRefD(pGui, pElemRef, __LINE__);

  // Fetch the parent of the clicked element which is the compound
  // element itself. This enables us to access the extra control data.
  gslc_tsElemRef*    pElemRefParent = pElem->pElemRefParent;
  if (pElemRefParent == NULL) {
    GSLC_DEBUG2_PRINT("ERROR: ElemXKeyPadClick(%s) parent ElemRef ptr NULL\n", "");
    return false;
  }

  gslc_tsXKeyPad* pKeyPad = (gslc_tsXKeyPad*)gslc_GetXDataFromRef(pGui, pElemRefParent, GSLC_TYPEX_KEYPAD, __LINE__);
  if (!pKeyPad) return false;

  //GSLC_DEBUG2_PRINT("KeyPad Click   Touch=%d\n", eTouch);

  // Handle the various button presses
  if (eTouch == GSLC_TOUCH_UP_IN) {
    // Get the tracked element ID
    gslc_tsElemRef* pElemRefTracked = pKeyPad->sCollect.pElemRefTracked;
    gslc_tsElem*    pElemTracked = gslc_GetElemFromRef(pGui, pElemRefTracked);

    int nSubElemId = pElemTracked->nId;
    int16_t nKeyInd = 0;
    bool bValRedraw = false;

    GSLC_CB_INPUT pfuncXInput = pKeyPad->pfuncCb;
    XKEYPAD_LOOKUP pfuncLookup = pKeyPad->pfuncLookup;
    gslc_tsXKeyPadData sKeyPadData;

    if (pfuncLookup != NULL) {
      // FIXME: ERROR
    }

    // Prepare the return data
    sKeyPadData.pStr = pKeyPad->acValStr;
    sKeyPadData.nTargetId = pKeyPad->nTargetId;

    //GSLC_DEBUG2_PRINT("KeyPad Click   ID=%d\n", nSubElemId);

    switch (nSubElemId) {

    case KEYPAD_ID_ENTER:
      //GSLC_DEBUG2_PRINT("KeyPad Key=ENT\n", "");
      //GSLC_DEBUG2_PRINT("KeyPad Done Str=[%s]\n", pKeyPad->acValStr);

   
      // Issue callback with Done status
      if (pfuncXInput != NULL) {
        (*pfuncXInput)(pvGui, (void*)(pElemRefParent), XKEYPAD_CB_STATE_DONE, (void*)(&sKeyPadData));
      }

      // Clear the contents
      memset(pKeyPad->acValStr, 0, XKEYPAD_VAL_LEN);
      pKeyPad->nValStrPos = 0;
      pKeyPad->nTargetId = GSLC_ID_NONE;
      break;

    case KEYPAD_ID_ESC:
      //GSLC_DEBUG2_PRINT("KeyPad Key=ESC\n", "");
      // Clear the contents
      memset(pKeyPad->acValStr, 0, XKEYPAD_VAL_LEN);
      pKeyPad->nValStrPos = 0;
      pKeyPad->nTargetId = GSLC_ID_NONE;    
      bValRedraw = true;
      //GSLC_DEBUG2_PRINT("KeyPad Done Str=[%s]\n", pKeyPad->acValStr);
      // Issue callback with Cancel status
      if (pfuncXInput != NULL) {
        (*pfuncXInput)(pvGui, (void*)(pElemRefParent), XKEYPAD_CB_STATE_CANCEL, (void*)(&sKeyPadData));
      }
      break;

    case KEYPAD_ID_DECIMAL:
      //GSLC_DEBUG2_PRINT("KeyPad Key=Decimal\n", "");
      if (!pKeyPad->sConfig.bFloatEn) break; // Ignore if floating point not enabled
      if (!pKeyPad->bValDecimalPt) {
        bValRedraw |= ElemXKeyPadAddChar(pGui, pKeyPad, KEYPAD_DISP_DECIMAL_PT);
      }
      break;

    case KEYPAD_ID_MINUS:
      //GSLC_DEBUG2_PRINT("KeyPad Key=Minus\n", "");
      if (!pKeyPad->sConfig.bSignEn) break; // Ignore if negative numbers not enabled
      // Toggle sign
      gslc_ElemXKeyPadValSetSign(pGui, pElemRefParent, pKeyPad->bValPositive ? false : true);
      bValRedraw = true;
      break;

    case KEYPAD_ID_BACKSPACE:
      //GSLC_DEBUG2_PRINT("KeyPad Key=BS\n", "");
      if (pKeyPad->nValStrPos < 1) break;
      pKeyPad->nValStrPos--;
      // Handle the special case of decimal point if floating point enabled
      if (pKeyPad->sConfig.bFloatEn) {
        if (pKeyPad->acValStr[pKeyPad->nValStrPos] == KEYPAD_DISP_DECIMAL_PT) {
          //GSLC_DEBUG2_PRINT("KeyPad Key=BS across decimal\n", "");
          pKeyPad->bValDecimalPt = false;
        }
      }
      if (pKeyPad->nValStrPos == 0) {
        memset(pKeyPad->acValStr, 0, XKEYPAD_VAL_LEN);
        pKeyPad->bValPositive = true;
      }
      pKeyPad->acValStr[pKeyPad->nValStrPos] = '\0';
      bValRedraw = true;
      break;

    case KEYPAD_ID_PERIOD:
    case KEYPAD_ID_SPACE:
    default:
      // Normal character
      //GSLC_DEBUG2_PRINT("KeyPad Key=Digit\n", "");
      // For basic buttons, we need to fetch the keypad string index
      nKeyInd = (*pfuncLookup)(pGui, nSubElemId);
      bValRedraw |= ElemXKeyPadAddChar(pGui, pKeyPad, pKeyPad->pacKeys[nKeyInd][0]);
      break;
    } // switch

    // Do we need to redraw the text field?
    if (bValRedraw) {
      pElemRef = gslc_CollectFindElemById(pGui, &pKeyPad->sCollect, KEYPAD_ID_TXT);
      gslc_ElemSetRedraw(pGui, pElemRef, GSLC_REDRAW_INC);
      // Issue callback with Update status
      if (pfuncXInput != NULL) {
        (*pfuncXInput)(pvGui, (void*)(pElemRefParent), XKEYPAD_CB_STATE_UPDATE, (void*)(&sKeyPadData));
      }
    } // bValRedraw

  } // eTouch

  return true;
  #endif // !DRV_TOUCH_NONE
}

bool gslc_ElemXKeyPadTouch(void* pvGui, void* pvElemRef, gslc_teTouch eTouch, int16_t nRelX, int16_t nRelY)
{
  #if defined(DRV_TOUCH_NONE)
  return false;
  #else

  if ((pvGui == NULL) || (pvElemRef == NULL)) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXKeyPadTouch";
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL, FUNCSTR);
    return false;
  }
  gslc_tsGui*           pGui = NULL;
  gslc_tsElemRef*       pElemRef = NULL;
  gslc_tsElem*          pElem = NULL;
  gslc_tsXKeyPad*       pKeyPad = NULL;


  // Typecast the parameters to match the GUI
  pGui = (gslc_tsGui*)(pvGui);
  pElemRef = (gslc_tsElemRef*)(pvElemRef);
  pElem = gslc_GetElemFromRef(pGui, pElemRef);
  pKeyPad = (gslc_tsXKeyPad*)(pElem->pXData);

  // Get Collection
  gslc_tsCollect* pCollect = &pKeyPad->sCollect;

  // Cascade the touch event to the sub-element collection
  return gslc_CollectTouchCompound(pvGui, pvElemRef, eTouch, nRelX, nRelY, pCollect);
  #endif // !DRV_TOUCH_NONE
}



void gslc_ElemXKeyPadCfgSetButtonSz(gslc_tsXKeyPadCfg* pConfig, int8_t nButtonSzW, int8_t nButtonSzH)
{
  pConfig->nButtonSzW = nButtonSzW;
  pConfig->nButtonSzH = nButtonSzH;
}

void gslc_ElemXKeyPadCfgSetFloatEn(gslc_tsXKeyPadCfg* pConfig, bool bEn)
{
  pConfig->bFloatEn = bEn;
}

void gslc_ElemXKeyPadCfgSetSignEn(gslc_tsXKeyPadCfg* pConfig, bool bEn)
{
  pConfig->bSignEn = bEn;
}

void gslc_ElemXKeyPadCfgSetRoundEn(gslc_tsXKeyPadCfg* pConfig, bool bEn)
{
  pConfig->bRoundEn = bEn;
}


// FIXME: Runtime API not fully functional yet - Do not use
void gslc_ElemXKeyPadSetFloatEn(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, bool bEn)
{
  gslc_tsXKeyPad* pKeyPad = (gslc_tsXKeyPad*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_KEYPAD, __LINE__);
  if (!pKeyPad) return;
  pKeyPad->sConfig.bFloatEn = bEn;
  // Mark as needing full redraw as button visibility may have changed
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_FULL);
}

// FIXME: Runtime API not fully functional yet - Do not use
void gslc_ElemXKeyPadSetSignEn(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, bool bEn)
{
  gslc_tsXKeyPad* pKeyPad = (gslc_tsXKeyPad*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_KEYPAD, __LINE__);
  if (!pKeyPad) return;
  pKeyPad->sConfig.bSignEn = bEn;
  // Mark as needing full redraw as button visibility may have changed
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_FULL);
}



#endif // GSLC_FEATURE_COMPOUND


// ============================================================================
