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
/// \file XRamp.c



// GUIslice library
#include "GUIslice.h"
#include "GUIslice_drv.h"

#include "elem/XRamp.h"

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
// Extended Element: Ramp Gauge
// - Demonstration of a gradient ramp (green-yellow-red) visual
//   control similar to certain linear tachometers.
// - The ramp rises up and to the right according to the
//   current value.
// - Note that this element is mainly intended as a demonstration
//   example. Additional APIs would be recommended to make it
//   more configurable.
// ============================================================================

// Create a gauge element and add it to the GUI element list
// - Defines default styling for the element
// - Defines callback for redraw but does not track touch/click
gslc_tsElemRef* gslc_ElemXRampCreate(gslc_tsGui* pGui,int16_t nElemId,int16_t nPage,
  gslc_tsXRamp* pXData,gslc_tsRect rElem,
  int16_t nMin,int16_t nMax,int16_t nVal,gslc_tsColor colGauge,bool bVert)
{
  (void)colGauge; // Unused FIXME: Should be used
  (void)bVert; // Unused
  if ((pGui == NULL) || (pXData == NULL)) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXRampCreate";
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return NULL;
  }
  gslc_tsElem     sElem;
  gslc_tsElemRef* pElemRef = NULL;
  sElem = gslc_ElemCreate(pGui,nElemId,nPage,GSLC_TYPEX_RAMP,rElem,NULL,0,GSLC_FONT_NONE);
  sElem.nFeatures        |= GSLC_ELEM_FEA_FRAME_EN;
  sElem.nFeatures        |= GSLC_ELEM_FEA_FILL_EN;
  sElem.nFeatures        &= ~GSLC_ELEM_FEA_CLICK_EN;  // Element is not "clickable"
  sElem.nFeatures        &= ~GSLC_ELEM_FEA_GLOW_EN;
  sElem.nGroup            = GSLC_GROUP_ID_NONE;
  pXData->nMin            = nMin;
  pXData->nMax            = nMax;
  pXData->nVal            = nVal;
  sElem.pXData            = (void*)(pXData);
  sElem.pfuncXDraw        = &gslc_ElemXRampDraw;
  sElem.pfuncXTouch       = NULL;           // No need to track touches
  sElem.colElemFill       = GSLC_COL_BLACK;
  sElem.colElemFillGlow   = GSLC_COL_BLACK;
  sElem.colElemFrame      = GSLC_COL_GRAY;
  sElem.colElemFrameGlow  = GSLC_COL_GRAY;
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


// Update the gauge control's current position
void gslc_ElemXRampSetVal(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,int16_t nVal)
{
  if (pElemRef == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXRampSetVal";
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return;
  }
  gslc_tsXRamp*  pGauge  = (gslc_tsXRamp*)gslc_GetXDataFromRef(pGui,pElemRef,GSLC_TYPEX_RAMP,__LINE__);

  // Update the data element
  int16_t nValOld = pGauge->nVal;
  pGauge->nVal = nVal;

  // Element needs redraw
  if (nVal != nValOld) {
    // We only need an incremental redraw
    // NOTE: If the user configures the indicator to be
    //       long enough that it overlaps some of the gauge indicators
    //       then a full redraw should be done instead.
    gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_INC);
  }

}


// Redraw the gauge
// - Note that this redraw is for the entire element rect region
// - The Draw function parameters use void pointers to allow for
//   simpler callback function definition & scalability.
bool gslc_ElemXRampDraw(void* pvGui,void* pvElemRef,gslc_teRedrawType eRedraw)
{
  if ((pvGui == NULL) || (pvElemRef == NULL)) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXRampDraw";
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return false;
  }

  // Typecast the parameters to match the GUI and element types
  gslc_tsGui*       pGui      = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef*   pElemRef  = (gslc_tsElemRef*)(pvElemRef);

  // Fetch the element's extended data structure
  gslc_tsXRamp*  pGauge  = (gslc_tsXRamp*)gslc_GetXDataFromRef(pGui,pElemRef,GSLC_TYPEX_RAMP,__LINE__);

  gslc_ElemXRampDrawHelp(pGui,pElemRef,eRedraw);

  // Save as "last state" to support incremental erase/redraw
  pGauge->nValLast      = pGauge->nVal;
  pGauge->bValLastValid = true;

  // Clear the redraw flag
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_NONE);

  return true;
}


bool gslc_ElemXRampDrawHelp(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,gslc_teRedrawType eRedraw)
{
  gslc_tsElem*   pElem   = gslc_GetElemFromRef(pGui,pElemRef);
  gslc_tsXRamp*  pGauge  = (gslc_tsXRamp*)gslc_GetXDataFromRef(pGui,pElemRef,GSLC_TYPEX_RAMP,__LINE__);

  uint16_t      nElemW,nElemH;
  int16_t       nElemX0,nElemY1;
  nElemX0   = pElem->rElem.x;
  nElemY1   = pElem->rElem.y + pElem->rElem.h - 1;
  nElemW    = pElem->rElem.w;
  nElemH    = pElem->rElem.h;

  int16_t   nMax            = pGauge->nMax;
  int16_t   nMin            = pGauge->nMin;
  int16_t   nRng            = pGauge->nMax - pGauge->nMin;
  int16_t   nVal            = pGauge->nVal;
  int16_t   nValLast        = pGauge->nValLast;
  bool      bValLastValid   = pGauge->bValLastValid;
  int16_t   nInd;

  if (nRng == 0) {
    GSLC_DEBUG2_PRINT("ERROR: gslc_ElemXRampDrawHelp() Zero range [%d,%d]\n",nMin,nMax);
    return false;
  }

  uint32_t  nSclFX;
  uint16_t  nHeight;
  int32_t   nHeightTmp;
  uint16_t  nHeightBot;
  int16_t   nX;
  uint16_t  nColInd;

  // Calculate region to draw or clear
  bool      bModeErase;
  int16_t   nValStart;
  int16_t   nValEnd;
  if ((eRedraw == GSLC_REDRAW_INC) && (!bValLastValid)) {
    // - If the request was incremental (GSLC_REDRAW_INC) but
    //   the last value wasn't marked as valid (!bValLastValid)
    //   then we want to force a full redraw.
    // - We don't expect to enter here since bValLastValid
    //   should always be set after we perform our first
    //   redraw.
    eRedraw = GSLC_REDRAW_FULL;
  }
  if (eRedraw == GSLC_REDRAW_FULL) {
    // If we haven't drawn anything before, draw full range from zero
    bModeErase  = false;
    nValStart   = 0;
    nValEnd     = nVal;
  } else {
    if (nVal >= nValLast) {
      // As we are advancing the control, we just draw the new range
      bModeErase  = false;
      nValStart   = nValLast;
      nValEnd     = nVal;
    } else {
      // Since we are retracting the control, we erase the new range
      bModeErase  = true;
      nValStart   = nVal;
      nValEnd     = nValLast;
    }
  }

  // Calculate the scaled gauge position
  // - TODO: Also support reversing of direction
  int16_t   nPosXStart,nPosXEnd;
  nPosXStart  = (nValStart - nMin)*nElemW/nRng;
  nPosXEnd    = (nValEnd   - nMin)*nElemW/nRng;

  nSclFX = (uint32_t)nElemH*32767/(nElemW*nElemW);

  for (nX=nPosXStart;nX<nPosXEnd;nX++) {
    nInd = nElemW-nX;
    nHeightTmp = nSclFX * nInd*nInd /32767;
    nHeight = nElemH-nHeightTmp;
    if (nHeight >= 20) {
      nHeightBot = nHeight-20;
    } else {
      nHeightBot = 0;
    }
    gslc_tsColor  nCol;
    uint16_t      nSteps = 10;
    uint16_t      nGap = 3;

    if (nSteps == 0) {
      nColInd = nX*1000/nElemW;
      nCol = gslc_ColorBlend3(GSLC_COL_GREEN,GSLC_COL_YELLOW,GSLC_COL_RED,500,nColInd);
    } else {
      uint16_t  nBlockLen,nSegLen,nSegInd,nSegOffset,nSegStart;
      nBlockLen = (nElemW-(nSteps-1)*nGap)/nSteps;
      nSegLen = nBlockLen + nGap;
      nSegInd = nX/nSegLen;
      nSegOffset = nX % nSegLen;
      nSegStart = nSegInd * nSegLen;

      if (nSegOffset <= nBlockLen) {
        // Inside block
        nColInd = (uint32_t)nSegStart*1000/nElemW;
        nCol = gslc_ColorBlend3(GSLC_COL_GREEN,GSLC_COL_YELLOW,GSLC_COL_RED,500,nColInd);

      } else {
        // Inside gap
        // - No draw
        nCol = pElem->colElemFill;
      }

    }

    if (bModeErase) {
      nCol = pElem->colElemFill;
    }
    gslc_DrawLine(pGui,nElemX0+nX,nElemY1-nHeightBot,nElemX0+nX,nElemY1-nHeight,nCol);

  }

  return true;
}


// ============================================================================
