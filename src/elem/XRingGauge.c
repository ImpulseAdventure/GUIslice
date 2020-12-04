// =======================================================================
// GUIslice library (Ring control)
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
/// \file XRingGauge.c



// GUIslice library
#include "GUIslice.h"
#include "GUIslice_drv.h"

#include "elem/XRingGauge.h"

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
// Extended Element: Ring gauge
// ============================================================================

// Create a text element and add it to the GUI element list
// - Defines default styling for the element
// - Defines callback for redraw and touch
gslc_tsElemRef* gslc_ElemXRingGaugeCreate(gslc_tsGui* pGui, int16_t nElemId, int16_t nPage,
    gslc_tsXRingGauge* pXData, gslc_tsRect rElem, char* pStrBuf, uint8_t nStrBufMax, int16_t nFontId)
{
  if ((pGui == NULL) || (pXData == NULL)) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXRingGaugeCreate";
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return NULL;
  }
  gslc_tsElem     sElem;
  gslc_tsElemRef* pElemRef = NULL;
  sElem = gslc_ElemCreate(pGui,nElemId,nPage,GSLC_TYPEX_RING,rElem,pStrBuf,nStrBufMax,nFontId);
  sElem.colElemFill       = GSLC_COL_BLACK;
  sElem.colElemFrame      = GSLC_COL_BLUE;
  sElem.colElemText       = GSLC_COL_YELLOW;
  sElem.nFeatures         = GSLC_ELEM_FEA_FILL_EN;
  sElem.nFeatures        &= ~GSLC_ELEM_FEA_FRAME_EN;
  sElem.nFeatures        &= ~GSLC_ELEM_FEA_CLICK_EN;
  sElem.nFeatures        &= ~GSLC_ELEM_FEA_GLOW_EN;
  sElem.eTxtAlign         = GSLC_ALIGN_MID_MID;

  sElem.nGroup            = GSLC_GROUP_ID_NONE;

  // Provide default config
  pXData->nValMin = 0;
  pXData->nValMax = 100;
  pXData->nAngStart = 0;
  pXData->nAngRange = 360;
  pXData->nThickness = 10;

  pXData->nQuality = 72; // 360/72=5 degree segments

  pXData->bGradient = false;
  pXData->nSegGap = 0;
  pXData->colRing1 = GSLC_COL_BLUE_LT4;
  pXData->colRing2 = GSLC_COL_RED;
  pXData->colRingRemain = (gslc_tsColor) { 0, 0, 48 };

  pXData->nVal = 0;
  pXData->nValLast = 0;
  pXData->acStrLast[0] = 0;


  sElem.pXData            = (void*)(pXData);
  // Specify the custom drawing callback
  sElem.pfuncXDraw        = &gslc_ElemXRingGaugeDraw;
  // Specify the custom touch tracking callback
  sElem.pfuncXTouch = NULL;

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


// Redraw the element
// - Note that this redraw is for the entire element rect region
// - The Draw function parameters use void pointers to allow for
//   simpler callback function definition & scalability.
bool gslc_ElemXRingGaugeDraw(void* pvGui, void* pvElemRef, gslc_teRedrawType eRedraw)
{
  gslc_tsGui* pGui = (gslc_tsGui*)pvGui;
  gslc_tsElemRef* pElemRef = (gslc_tsElemRef*)pvElemRef;
  gslc_tsElem* pElem = gslc_GetElemFromRefD(pGui, pElemRef, __LINE__);
  gslc_tsXRingGauge* pXRingGauge = (gslc_tsXRingGauge*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_RING, __LINE__);
  if (!pXRingGauge) return false;


  // --------------------------------------------------------------------------
  // Init for default drawing
  // --------------------------------------------------------------------------

  bool      bGlowEn, bGlowing, bGlowNow;
  int16_t   nElemX, nElemY;
  uint16_t  nElemW, nElemH;

  nElemX = pElem->rElem.x;
  nElemY = pElem->rElem.y;
  nElemW = pElem->rElem.w;
  nElemH = pElem->rElem.h;
  bGlowEn = pElem->nFeatures & GSLC_ELEM_FEA_GLOW_EN; // Does the element support glow state?
  bGlowing = gslc_ElemGetGlow(pGui, pElemRef); // Element should be glowing (if enabled)
  bGlowNow = bGlowEn & bGlowing; // Element is currently glowing

  int16_t nVal = pXRingGauge->nVal;
  int16_t nValLast = pXRingGauge->nValLast;
  int16_t nValMin = pXRingGauge->nValMin;
  int16_t nValMax = pXRingGauge->nValMax;
  int16_t nValRange = (nValMax == nValMin)? 1 : (nValMax - nValMin); // Guard against div/0
  int16_t nAngStart = pXRingGauge->nAngStart;
  int16_t nAngRange = pXRingGauge->nAngRange;
  //int16_t nAngEnd = nAngStart + nAngRange;
  int16_t nQuality = pXRingGauge->nQuality;

  gslc_tsColor colRingActive1 = pXRingGauge->colRing1;
  gslc_tsColor colRingActive2 = pXRingGauge->colRing2;
  gslc_tsColor colRingInactive = pXRingGauge->colRingRemain;
  bool bGradient = pXRingGauge->bGradient;
  gslc_tsColor colBg = pElem->colElemFill; // Background color used for text clearance

  // Calculate the ring center and radius
  int16_t nMidX = nElemX + nElemW / 2;
  int16_t nMidY = nElemY + nElemH / 2;
  int16_t nRad2 = (nElemW < nElemH) ? nElemW / 2 : nElemH / 2;
  int16_t nRad1 = nRad2 - pXRingGauge->nThickness;

  // --------------------------------------------------------------------------

  // Determine whether we should draw the full range (full redraw)
  // or a smaller, updated region (incremental redraw)
  bool bInc = (eRedraw == GSLC_REDRAW_INC) ? true : false;

  int16_t nDrawStart = 0;
  int16_t nDrawVal = 0;
  int16_t nDrawEnd = 0;

  bool bDrawActive = false;
  bool bDrawInactive = false;
  if (bInc) {
    if (nVal > nValLast) {
      // Incremental redraw: adding value, so draw with active color
      bDrawActive = true;
      nDrawStart = (int32_t)(nValLast - nValMin) * nAngRange / nValRange;
      nDrawVal = (int32_t)(nVal - nValMin) * nAngRange / nValRange;
    }
    else {
      // Incremental redraw: reducing value, so draw with inactive color
      bDrawInactive = true;
      nDrawVal = (int32_t)(nVal - nValMin) * nAngRange / nValRange;
      nDrawEnd = (int32_t)(nValLast - nValMin) * nAngRange / nValRange;
    }
  } else {
    // Full redraw: draw both active and inactive regions
    bDrawActive = true;
    bDrawInactive = true;
    nDrawStart = 0;
    nDrawVal = (int32_t)(nVal - nValMin) * nAngRange / nValRange;
    nDrawEnd = nAngRange;
  }
  
  #if defined(DBG_REDRAW)
  GSLC_DEBUG2_PRINT("\n\nRingDraw: Val=%d ValLast=%d ValRange=%d PosMin=%d PosMax=%d Q=%d\n", nVal, nValLast, nValRange, nValMin, nValMax, nQuality);
  GSLC_DEBUG2_PRINT("RingDraw:  Inc=%d ValLast=%d Val=%d Ang=%d..%d (Range=%d)\n", bInc,nValLast,nVal,nAngStart, nAngStart+nAngRange,nAngRange); //CAL!
  GSLC_DEBUG2_PRINT("RingDraw:  DrawActive=%d DrawInactive=%d DrawStart=%d DrawVal=%d DrawEnd=%d\n",bDrawActive,bDrawInactive,nDrawStart,nDrawVal,nDrawEnd);//CAL!
  #endif

  // Adjust for start of angular range
  nDrawStart += nAngStart;
  nDrawVal += nAngStart;
  nDrawEnd += nAngStart;

  if (bDrawActive) {
    if (bGradient) {
      #if defined(DBG_REDRAW)
      GSLC_DEBUG2_PRINT("RingDraw:   ActiveG  start=%d end=%d astart=%d arange=%d\n", nDrawStart, nDrawVal,nAngStart,nAngRange);
      #endif
      gslc_DrawFillGradSector(pGui, nQuality, nMidX, nMidY,
        nRad1, nRad2, colRingActive1, colRingActive2, nDrawStart, nDrawVal, nAngStart, nAngRange);
    } else {
      #if defined(DBG_REDRAW)
      GSLC_DEBUG2_PRINT("RingDraw:   Active   start=%d end=%d\n", nDrawStart, nDrawVal);
      #endif
      gslc_DrawFillSector(pGui, nQuality, nMidX, nMidY,
        nRad1, nRad2, colRingActive1, nDrawStart, nDrawVal);
    }
  }

  if (bDrawInactive) {
    #if defined(DBG_REDRAW)
    GSLC_DEBUG2_PRINT("RingDraw:   Inactive start=%d end=%d\n", nDrawEnd, nDrawVal);
    #endif
    // Since we are erasing, we will reverse the redraw direction (swap Val & End)
    gslc_DrawFillSector(pGui, nQuality, nMidX, nMidY,
      nRad1, nRad2, colRingInactive, nDrawEnd, nDrawVal);
  }

  // --------------------------------------------------------------------------
  // Text overlays
  // --------------------------------------------------------------------------

  // Draw text string if defined
  if (pElem->pStrBuf) {
    gslc_tsColor  colTxt    = (bGlowNow)? pElem->colElemTextGlow : pElem->colElemText;
    int8_t        nMarginX  = pElem->nTxtMarginX;
    int8_t        nMarginY  = pElem->nTxtMarginY;

    // Erase old string content using "background" color
    if (strlen(pXRingGauge->acStrLast) != 0) {
      gslc_DrawTxtBase(pGui, pXRingGauge->acStrLast, pElem->rElem, pElem->pTxtFont, pElem->eTxtFlags,
        pElem->eTxtAlign, colBg, GSLC_COL_BLACK, nMarginX, nMarginY);
    }

    // Draw new string content
    gslc_DrawTxtBase(pGui, pElem->pStrBuf, pElem->rElem, pElem->pTxtFont, pElem->eTxtFlags,
      pElem->eTxtAlign, colTxt, GSLC_COL_BLACK, nMarginX, nMarginY);

    // Save a copy of the new string content so we can support future erase
    gslc_StrCopy(pXRingGauge->acStrLast, pElem->pStrBuf, XRING_STR_MAX);

  } // pStrBuf

  // Save the position to enable future incremental calculations
  pXRingGauge->nValLast = pXRingGauge->nVal;


  // --------------------------------------------------------------------------

  // Mark the element as no longer requiring redraw
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_NONE);

  return true;

}


void gslc_ElemXRingGaugeSetVal(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, int16_t nVal)
{
  gslc_tsXRingGauge* pXRingGauge = (gslc_tsXRingGauge*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_RING, __LINE__);
  if (!pXRingGauge) return;

  int16_t           nValOld;

  // Clip position
  if (nVal < pXRingGauge->nValMin) { nVal = pXRingGauge->nValMin; }
  if (nVal > pXRingGauge->nValMax) { nVal = pXRingGauge->nValMax; }

  // Update
  nValOld = pXRingGauge->nVal;
  pXRingGauge->nVal = nVal;

  // Only update if changed
  if (nVal != nValOld) {
    // Mark for redraw
    // - Only need incremental redraw
    gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_INC);
  }

}

void gslc_ElemXRingGaugeSetValRange(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, int16_t nValMin, int16_t nValMax)
{
  gslc_tsXRingGauge* pXRingGauge = (gslc_tsXRingGauge*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_RING, __LINE__);
  if (!pXRingGauge) return;

  pXRingGauge->nValMin = nValMin;
  pXRingGauge->nValMax = nValMax;

  // Mark for full redraw
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_FULL);
}

void gslc_ElemXRingGaugeSetAngleRange(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, int16_t nStart, int16_t nRange, bool bClockwise)
{
  (void)bClockwise; // Unused TODO: Support rotation reversal
  gslc_tsXRingGauge* pXRingGauge = (gslc_tsXRingGauge*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_RING, __LINE__);
  if (!pXRingGauge) return;

  pXRingGauge->nAngStart = nStart;
  pXRingGauge->nAngRange = nRange;

  nRange = (nRange == 0) ? 1 : nRange; // Guard against div/0

  // TODO: Support bClockwise

  // Mark for full redraw
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_FULL);
}

void gslc_ElemXRingGaugeSetThickness(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, int8_t nThickness)
{
  gslc_tsXRingGauge* pXRingGauge = (gslc_tsXRingGauge*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_RING, __LINE__);
  if (!pXRingGauge) return;

  pXRingGauge->nThickness = nThickness;

  // Mark for full redraw
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_FULL);
}

void gslc_ElemXRingGaugeSetColorActiveFlat(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, gslc_tsColor colActive)
{
  gslc_tsXRingGauge* pXRingGauge = (gslc_tsXRingGauge*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_RING, __LINE__);
  if (!pXRingGauge) return;

  pXRingGauge->bGradient = false;
  pXRingGauge->colRing1 = colActive;
  pXRingGauge->colRing2 = colActive;

  // Mark for full redraw
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_FULL);
}

void gslc_ElemXRingGaugeSetColorActiveGradient(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, gslc_tsColor colStart, gslc_tsColor colEnd)
{
  gslc_tsXRingGauge* pXRingGauge = (gslc_tsXRingGauge*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_RING, __LINE__);
  if (!pXRingGauge) return;

  pXRingGauge->bGradient = true;
  pXRingGauge->colRing1 = colStart;
  pXRingGauge->colRing2 = colEnd;

  // Mark for full redraw
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_FULL);
}


void gslc_ElemXRingGaugeSetColorInactive(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, gslc_tsColor colInactive)
{
  gslc_tsXRingGauge* pXRingGauge = (gslc_tsXRingGauge*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_RING, __LINE__);
  if (!pXRingGauge) return;

  pXRingGauge->colRingRemain = colInactive;

  // Mark for full redraw
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_FULL);
}



void gslc_ElemXRingGaugeSetQuality(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, uint16_t nSegments)
{
  gslc_tsXRingGauge* pXRingGauge = (gslc_tsXRingGauge*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_RING, __LINE__);
  if (!pXRingGauge) return;

  nSegments = (nSegments == 0) ? 72 : nSegments; // Guard against div/0 with default
  pXRingGauge->nQuality = nSegments;

  // Mark for full redraw
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_FULL);
}


// ============================================================================
