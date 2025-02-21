// =======================================================================
// GUIslice library (extensions)
// - Calvin Hass
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
/// \file XSpinner.c
///
/// Author: Paul Conti
/// Date:   2022-04-06
/// Modified to use Virtual Elements instead of Compound
///

// GUIslice library
#include "GUIslice.h"
#include "GUIslice_drv.h"

#include "elem/XSpinner.h"

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
// Extended Element: Spinner
// - Spinner element - a simple up/down counter
// - This is a element containing two virtual buttons and
//   a text area to represent the current value
// ============================================================================

// Private sub Element ID definitions
static const int16_t  SPINNER_ID_BTN_INC = 100;
static const int16_t  SPINNER_ID_BTN_DEC = 101;
static const int16_t  SPINNER_ID_TXT     = 102;

// Create a compound element
// - For now just two buttons and a text area
gslc_tsElemRef* gslc_ElemXSpinnerCreate(gslc_tsGui* pGui, int16_t nElemId, int16_t nPage, gslc_tsXSpinner* pXData,
  gslc_tsRect rElem, int16_t nMin, int16_t nMax, int16_t nVal, int16_t nIncr,
  int8_t nFontId, int8_t nButtonSz, GSLC_CB_INPUT cbInput)
{

  if ((pGui == NULL) || (pXData == NULL)) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXSpinnerCreate";
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL, FUNCSTR);
    return NULL;
  }

  // determine size of our text box
  // first calculate number of digits required
  char acTxtNum[XSPINNER_STR_LEN];
  (void)acTxtNum; // suppress unused warning
  int16_t nTxtBoxW = rElem.w - 2 * (nButtonSz);
  // Determine the maximum width of a digit, we will use button size for height.
  // now we can work out our rectangle  
  int16_t nTxtBoxH = rElem.h;

  int16_t nBtnPosY = rElem.y + (rElem.h - nButtonSz) / 2;

  gslc_tsElemRef* pElemRef = NULL;
  gslc_tsElem sElem;

  // Initialize composite element
  sElem = gslc_ElemCreate(pGui, nElemId, nPage, GSLC_TYPEX_SPINNER, rElem, NULL, 0, GSLC_FONT_NONE);
  sElem.nFeatures |= GSLC_ELEM_FEA_FRAME_EN;
  sElem.nFeatures |= GSLC_ELEM_FEA_FILL_EN;
  sElem.nFeatures |= GSLC_ELEM_FEA_CLICK_EN;
  sElem.nFeatures |= GSLC_ELEM_FEA_GLOW_EN;
  sElem.nFeatures |= GSLC_ELEM_FEA_FOCUS_EN;
  sElem.nGroup = GSLC_GROUP_ID_NONE;

  pXData->nCounter = nVal;
  pXData->nMin = nMin;
  pXData->nMax = nMax;
  pXData->nIncr = nIncr;
  pXData->pfuncXInput = cbInput;
  pXData->nFontId = nFontId;

  sElem.pXData = (void*)(pXData);

  // Specify the custom drawing callback
  sElem.pfuncXDraw = &gslc_ElemXSpinnerDraw;
  // Specify the custom touch tracking callback
  sElem.pfuncXTouch = &gslc_ElemXSpinnerTouch;

  sElem.colElemFill = GSLC_COL_BLACK;
  sElem.colElemFillGlow = GSLC_COL_BLACK;
  sElem.colElemFrame = GSLC_COL_GRAY;
  sElem.colElemFrameGlow = GSLC_COL_WHITE;

  // create our bounding rectangles for each virtual element
	
	// increment button
  pXData->rSubElemBtnInc.x = rElem.x+nTxtBoxW;
	pXData->rSubElemBtnInc.y = nBtnPosY;
	pXData->rSubElemBtnInc.w = nButtonSz;
	pXData->rSubElemBtnInc.h = nButtonSz;
  pXData->rSubElemBtnInc   = gslc_ExpandRect(pXData->rSubElemBtnInc, -1, -1);
	strcpy(pXData->acIncr,"\030");

	// decrement button
  pXData->rSubElemBtnDec.x = rElem.x+nTxtBoxW+nButtonSz;
	pXData->rSubElemBtnDec.y = nBtnPosY;
	pXData->rSubElemBtnDec.w = nButtonSz;
	pXData->rSubElemBtnDec.h = nButtonSz;
  pXData->rSubElemBtnDec   = gslc_ExpandRect(pXData->rSubElemBtnDec, -1, -1);
  strcpy(pXData->acDecr,"\031");

  // text field
  pXData->rSubElemTxt.x = rElem.x;
	pXData->rSubElemTxt.y = nBtnPosY;
	pXData->rSubElemTxt.w = nTxtBoxW;
	pXData->rSubElemTxt.h = nTxtBoxH;
  pXData->rSubElemTxt   = gslc_ExpandRect(pXData->rSubElemTxt, -1, -1);
  // set our intial value for our text field
  // TODO: Consider replacing the sprintf() with an optimized function to
  //        conserve RAM. Potentially leverage gslc_DebugPrintf().
  snprintf(pXData->acElemTxt[0], XSPINNER_STR_LEN - 1, "%d", nVal);

  // Now proceed to add the element to the page
  if (nPage != GSLC_PAGE_NONE) {
    pElemRef = gslc_ElemAdd(pGui,nPage,&sElem,GSLC_ELEMREF_DEFAULT);
    // save our ElemRef for the callback
    pXData->pElemRef = pElemRef;
    return pElemRef;
  }
  return NULL;

}

bool gslc_ElemXSpinnerSetChars(void* pvGui,gslc_tsElemRef* pElemRef,uint8_t cIncr, uint8_t cDecr)
{
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);
  gslc_tsElem* pElem = gslc_GetElemFromRefD(pGui, pElemRef, __LINE__);
  if (!pElem) return false;
  gslc_tsXSpinner* pSpinner = (gslc_tsXSpinner*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_SPINNER, __LINE__);
  if (!pSpinner) return false;

  pSpinner->acIncr[0] = cIncr;
  pSpinner->acIncr[1] = '\0';
  pSpinner->acDecr[0] = cDecr;
  pSpinner->acDecr[1] = '\0';
  
  return true;  
}

void gslc_XSpinnerDrawVirtualTxt(gslc_tsGui* pGui,gslc_tsRect rElem,int16_t nFontId,char* pValStr,int8_t eTxtAlign,
  gslc_tsColor cColFrame, gslc_tsColor cColFill, gslc_tsColor cColTxt)
{
  gslc_tsElem  sElem;
  gslc_tsElem* pVirtualElem = &sElem;

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
  pVirtualElem->pTxtFont          = gslc_FontGet(pGui,nFontId);

  pVirtualElem->eTxtFlags         = GSLC_TXT_DEFAULT;
  pVirtualElem->eTxtFlags         = (pVirtualElem->eTxtFlags & ~GSLC_TXT_ALLOC) | GSLC_TXT_ALLOC_EXT;
  pVirtualElem->eTxtAlign         = eTxtAlign;
  pVirtualElem->nTxtMarginX       = 2;
  pVirtualElem->nTxtMarginY       = 2;
  
  // Render the virtual element
  gslc_tsElemRef sElemRef;
  sElemRef.pElem = pVirtualElem;
  sElemRef.eElemFlags = GSLC_ELEMREF_SRC_RAM | GSLC_ELEMREF_VISIBLE;

 // In GSLC_LOCAL_STR mode, pStrBuf is a local character
 // array, so we need to perform a deep string copy here.
 // When not in GSLC_LOCAL_STR mode, pStrBuf is just a
 // pointer, so we can simply update the pointer.
 #if (GSLC_LOCAL_STR)
	 // Deep copy
	 gslc_StrCopy(pVirtualElem->pStrBuf,pValStr,GSLC_LOCAL_STR_LEN);
 #else
	 // Shallow copy 
	 pVirtualElem->nStrBufMax = 0; // Read-only string buffer
	 pVirtualElem->pStrBuf = pValStr;
 #endif
   gslc_ElemDrawByRef(pGui,&sElemRef,GSLC_REDRAW_FULL);

}

void gslc_XSpinnerDrawVirtualBtn(gslc_tsGui* pGui, gslc_tsRect rElem,
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

// Redraw the compound element
// - When drawing a compound element, we clear the background
//   and then redraw the sub-element collection.
bool gslc_ElemXSpinnerDraw(void* pvGui,void* pvElemRef,gslc_teRedrawType eRedraw)
{
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem* pElem = gslc_GetElemFromRef(pGui,pElemRef);
  gslc_tsXSpinner* pSpinner = (gslc_tsXSpinner*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_SPINNER, __LINE__);
  if (pSpinner == NULL) {
    return false;
  }

  bool bGlow = (pElem->nFeatures & GSLC_ELEM_FEA_GLOW_EN) && gslc_ElemGetGlow(pGui,pElemRef);

  // Draw the element fill (background)
  // - Should only need to do this in full redraw
  if (eRedraw == GSLC_REDRAW_FULL) {
    gslc_DrawFillRect(pGui,pElem->rElem,(bGlow)?pElem->colElemFillGlow:pElem->colElemFill);
  }

  // Optionally, draw a frame around the element
  if (eRedraw == GSLC_REDRAW_FULL) {
    gslc_DrawFrameRect(pGui, pElem->rElem, (bGlow) ? pElem->colElemFrameGlow : pElem->colElemFrame);
  }

  // draw our virtual elements
  
  // Create dynamic button sub-element
	gslc_XSpinnerDrawVirtualBtn(pGui, pSpinner->rSubElemBtnInc, pSpinner->acIncr, 2, pSpinner->nFontId,
    GSLC_COL_BLUE_DK2, GSLC_COL_BLUE_DK4, GSLC_COL_BLUE_DK1, GSLC_COL_WHITE,
    false, false, false);

  // Create dynamic button sub-element
	gslc_XSpinnerDrawVirtualBtn(pGui, pSpinner->rSubElemBtnDec, pSpinner->acDecr, 2, pSpinner->nFontId,
    GSLC_COL_BLUE_DK2, GSLC_COL_BLUE_DK4, GSLC_COL_BLUE_DK1, GSLC_COL_WHITE,
    false, false, false);

  // Create dynamic text sub-element
	gslc_XSpinnerDrawVirtualTxt(pGui, pSpinner->rSubElemTxt, pSpinner->nFontId, pSpinner->acElemTxt[0],
	  GSLC_ALIGN_MID_MID, GSLC_COL_BLUE_DK2, GSLC_COL_BLACK, GSLC_COL_WHITE);

  // Clear the redraw flag
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_NONE);

  // Mark page as needing flip
  gslc_PageFlipSet(pGui,true);

  return true;
}

// Fetch the current value of the element's counter
int gslc_ElemXSpinnerGetCounter(gslc_tsGui* pGui,gslc_tsXSpinner* pSpinner)
{
  if ((pGui == NULL) || (pSpinner == NULL)) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXSpinnerGetCounter";
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return 0;
  }
  return pSpinner->nCounter;
}


void gslc_ElemXSpinnerSetCounter(gslc_tsGui* pGui,gslc_tsXSpinner* pSpinner,int16_t nCount)
{
  if (pSpinner == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXSpinnerSetCounter";
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return;
  }
  pSpinner->nCounter = nCount;

  // Update counter text
  // TODO: Consider replacing the printf() with an optimized function to
  //        conserve RAM. Potentially leverage gslc_DebugPrintf().
  snprintf(pSpinner->acElemTxt[0],GSLC_LOCAL_STR_LEN,"%hd",pSpinner->nCounter);

}

// Map touch event a it bounding rectangle position that matches one
// of the virtual sub-elements of this control.  Once its found
// return the sub-element ID (defined at the top of this sourcefile) to the caller.
// returns -1 if no buttons touched (text field). 
int16_t gslc_XSpinnerMapEvent(gslc_tsGui* pGui, gslc_tsXSpinner* pConfig, int16_t nRelX, int16_t nRelY)
{

  //GSLC_DEBUG_PRINT("[gslc_XSpinnerMapEvent] nRelX=%d nRelY=%d\n",nRelX,nRelY);

  // Scan for buttons
  if (gslc_IsInRect(nRelX,nRelY,pConfig->rSubElemBtnInc)) {
    //GSLC_DEBUG_PRINT("match: ID=SPINNER_ID_BTN_INC\n", "");
    return SPINNER_ID_BTN_INC;
  }
  if (gslc_IsInRect(nRelX,nRelY,pConfig->rSubElemBtnDec)) {
    //GSLC_DEBUG_PRINT("match: ID=SPINNER_ID_BTN_DEC\n", "");
    return SPINNER_ID_BTN_DEC;
  }

  // text field touched
  return -1;
}

// - This routine is called by gslc_ElemEvent() to handle
//   any click events that resulted from the touch tracking process.
// - pvElemRef is a void pointer to the element ref being tracked 
//   containing multiple virtual elements. 
bool gslc_ElemXSpinnerTouch(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nRelX,int16_t nRelY)
{
#if defined(DRV_TOUCH_NONE)
  return false;
#else

  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem* pElem = gslc_GetElemFromRef(pGui,pElemRef);
  gslc_tsRect rElem = pElem->rElem;

  gslc_tsXSpinner* pSpinner = (gslc_tsXSpinner*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_SPINNER, __LINE__);
  if (pSpinner == NULL) {
    GSLC_DEBUG2_PRINT("ERROR: gslc_ElemXSpinnerTouch() element (ID=%d) has NULL pXData\n",pElem->nId);
    return false;
  }

	// setup variables
  bool  bGlowingOld = gslc_ElemGetGlow(pGui,pElemRef);
  bool  bFocusedOld = gslc_ElemGetFocus(pGui,pElemRef);
  int16_t nSubElemId = 0;
  int16_t nAbsX = nRelX + rElem.x;
  int16_t nAbsY = nRelY + rElem.y;
  bool  bChanged = false;
  int nCounter  = pSpinner->nCounter;

  // Begin the core touch functionality
  // by Handling the various button presses
  GSLC_CB_INPUT pfuncXInput = NULL;
  switch(eTouch) {

    case GSLC_TOUCH_DOWN_IN:
      // Start glowing as must be over it
      gslc_ElemSetGlow(pGui,pElemRef,true);
      break;

    case GSLC_TOUCH_MOVE_IN:
      gslc_ElemSetGlow(pGui,pElemRef,true);
      break;

    case GSLC_TOUCH_MOVE_OUT:
      gslc_ElemSetGlow(pGui,pElemRef,false);
      break;

    case GSLC_TOUCH_UP_IN:
      // End glow
      gslc_ElemSetGlow(pGui,pElemRef,false);
			// Determine which virtual field matches the touch
			nSubElemId = gslc_XSpinnerMapEvent(pGui, pSpinner, nAbsX, nAbsY);
			if (nSubElemId == SPINNER_ID_BTN_INC) {
				// Increment button
				if (nCounter < pSpinner->nMax) {
					nCounter += pSpinner->nIncr;
				}
				gslc_ElemXSpinnerSetCounter(pGui,pSpinner,nCounter);

			} else if (nSubElemId == SPINNER_ID_BTN_DEC) {
				// Decrement button
				if (nCounter > pSpinner->nMin) {
					nCounter -= pSpinner->nIncr;
				}
				gslc_ElemXSpinnerSetCounter(pGui,pSpinner,nCounter);

			}

      bChanged = true;
			// Invoke the callback function
			pfuncXInput = pSpinner->pfuncXInput;
			if (pfuncXInput != NULL) {
				(*pfuncXInput)(pvGui, (void*)(pSpinner->pElemRef), XSPINNER_CB_STATE_UPDATE, NULL);
			}
      break;
    case GSLC_TOUCH_UP_OUT:
      // End glow
      gslc_ElemSetGlow(pGui,pElemRef,false);
      break;

    default:
      return false;
  }

  // If the spinner changed state, redraw
  if (bChanged || 
	    gslc_ElemGetGlow(pGui,pElemRef)  != bGlowingOld ||
//			gslc_ElemGetEdit(pGui, pElemRef) != bEditingOld ||
			gslc_ElemGetFocus(pGui,pElemRef) != bFocusedOld ){
    gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_FULL);
  }

  return true;
#endif // !DRV_TOUCH_NONE
}


// ============================================================================
