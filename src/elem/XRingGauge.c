// =======================================================================
// GUIslice library (Ring control)
// - Calvin Hass
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
/// \file XRingGauge.c



// GUIslice library
#include "GUIslice.h"
#include "GUIslice_drv.h"

#include "elem/XRingGauge.h"

#include <stdio.h>

#if (GSLC_USE_PROGMEM)
    #include <avr/pgmspace.h>
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
  pXData->nPosMin = 0;
  pXData->nPosMax = 100;
  pXData->nThickness = 10;

  pXData->nDeg64PerSeg = 5 * 64; // Defaul to 5 degree segments
  pXData->bGradient = false;
  pXData->nSegGap = 0;
  pXData->colRing1 = GSLC_COL_BLUE_LT4;
  pXData->colRing2 = GSLC_COL_RED;
  pXData->colRingRemain = (gslc_tsColor) { 0, 0, 48 };

  pXData->nPos = 0;
  pXData->nPosLast = 0;
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
bool gslc_ElemXRingGaugeDraw(void* pvGui,void* pvElemRef,gslc_teRedrawType eRedraw)
{
  gslc_tsGui* pGui = (gslc_tsGui*)pvGui;
  gslc_tsElemRef* pElemRef = (gslc_tsElemRef*)pvElemRef;
  gslc_tsElem* pElem = gslc_GetElemFromRefD(pGui, pElemRef, __LINE__);
  gslc_tsXRingGauge* pXRingGauge = (gslc_tsXRingGauge*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_RING, __LINE__);
  if (!pXRingGauge) return false;


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

  int16_t nVal = pXRingGauge->nPos;
  int16_t nValLast = pXRingGauge->nPosLast;

  gslc_tsColor colRingActive1 = pXRingGauge->colRing1;
  gslc_tsColor colRingActive2 = pXRingGauge->colRing2;
  gslc_tsColor colRingInactive = pXRingGauge->colRingRemain;
  gslc_tsColor colBg = pElem->colElemFill; // Background color used for text clearance
  gslc_tsColor colStep;

  // Calculate the ring center and radius
  int16_t nMidX = nElemX + nElemW / 2;
  int16_t nMidY = nElemY + nElemH / 2;
  int16_t nRad2 = (nElemW < nElemH) ? nElemW / 2 : nElemH / 2;
  int16_t nRad1 = nRad2 - pXRingGauge->nThickness;

  // --------------------------------------------------------------------------

  gslc_tsPt anPts[4]; // Storage for points in segment quadrilaterla
  int16_t nAng64Start,nAng64End;
  int16_t nX, nY;

  // Calculate segment ranges
  // - TODO: Handle nPosMin, nPosMax
  // - TODO: Rewrite to use nDeg64PerSeg more effectively
  // - FIXME: Handle case with nDeg64PerSeg < 64 (ie. <1 degree)
  int16_t nStep64 = pXRingGauge->nDeg64PerSeg; // Trig functions work on 1/64 degree units
  int16_t nStepAng = nStep64 / 64;
  uint32_t nValSegs = ((uint32_t)nVal * 360 / 100) / nStepAng; // Segment index of current value
  uint32_t nLastSegs = ((uint32_t)nValLast * 360 / 100) / nStepAng; // Segment index of previous value
  int16_t nMaxSegs = 360 / nStepAng; // Final segment index of circle

  // Determine whether we should draw the full range (full redraw)
  // or a smaller, updated region (incremental redraw)
  bool bInc = (eRedraw == GSLC_REDRAW_INC) ? true : false;
  int16_t nSegStart, nSegEnd;
  if (bInc) {
    if (nVal > nValLast) {
      nSegStart = nLastSegs;
      nSegEnd = nValSegs;
    } else {
      nSegStart = nValSegs;
      nSegEnd = nLastSegs;
    }
  } else {
    nSegStart = 0;
    nSegEnd = nMaxSegs;
  }
  //GSLC_DEBUG2_PRINT("DBG: redraw inc=%d last=%d cur=%d segs %d..%d\n", bInc,nValLast,nVal,nSegStart, nSegEnd);

  // TODO: Consider drawing in reverse order if (nVal < nValLast)
  for (uint16_t nSegInd = nSegStart; nSegInd < nSegEnd; nSegInd++) {
    nAng64Start = nSegInd * nStep64;
    nAng64End = nAng64Start + nStep64;

    // Convert polar coordinates into cartesian
    gslc_PolarToXY(nRad1, nAng64Start, &nX, &nY);
    anPts[0] = (gslc_tsPt) { nMidX + nX, nMidY + nY };
    gslc_PolarToXY(nRad2, nAng64Start, &nX, &nY);
    anPts[1] = (gslc_tsPt) { nMidX + nX, nMidY + nY };
    gslc_PolarToXY(nRad2, nAng64End, &nX, &nY);
    anPts[2] = (gslc_tsPt) { nMidX + nX, nMidY + nY };
    gslc_PolarToXY(nRad1, nAng64End, &nX, &nY);
    anPts[3] = (gslc_tsPt) { nMidX + nX, nMidY + nY };

    // Adjust color depending on which segment we are rendering
    if (nSegInd < nValSegs) {
		  if (pXRingGauge->bGradient) {
        // Gradient coloring
			  uint16_t nGradPos = 1000.0 * nSegInd / nMaxSegs;
			  colStep = gslc_ColorBlend2(colRingActive1, colRingActive2, 500, nGradPos);
		  } else {
        // Flat coloring
			  colStep = colRingActive1;
		  }
    } else {
      colStep = colRingInactive;
    }

    // TODO: Support gapped regions between segments

    // Draw the quadrilateral representing the circle segment
    gslc_DrawFillQuad(pGui, anPts, colStep);

  }

  // --------------------------------------------------------------------------
  // Text overlays
  // --------------------------------------------------------------------------

  // Draw text string if defined
  if (pElem->pStrBuf) {
    gslc_tsColor  colTxt    = (bGlowNow)? pElem->colElemTextGlow : pElem->colElemText;
    int16_t       nMargin   = pElem->nTxtMargin;

    // Erase old string content using "background" color
    if (strlen(pXRingGauge->acStrLast) != 0) {
      gslc_DrawTxtBase(pGui, pXRingGauge->acStrLast, pElem->rElem, pElem->pTxtFont, pElem->eTxtFlags,
        pElem->eTxtAlign, colBg, GSLC_COL_BLACK, nMargin, nMargin);
    }

    // Draw new string content
    gslc_DrawTxtBase(pGui, pElem->pStrBuf, pElem->rElem, pElem->pTxtFont, pElem->eTxtFlags,
      pElem->eTxtAlign, colTxt, GSLC_COL_BLACK, nMargin, nMargin);

    // Save a copy of the new string content so we can support future erase
    strncpy(pXRingGauge->acStrLast, pElem->pStrBuf, XRING_STR_MAX);
    pXRingGauge->acStrLast[XRING_STR_MAX - 1] = 0; // Force null terminator

  } // pStrBuf

  // Save the position to enable future incremental calculations
  pXRingGauge->nPosLast = pXRingGauge->nPos;


  // --------------------------------------------------------------------------

  // Mark the element as no longer requiring redraw
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_NONE);

  return true;

}

void gslc_ElemXRingGaugeSetPos(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, int16_t nPos)
{
  gslc_tsXRingGauge* pXRingGauge = (gslc_tsXRingGauge*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_RING, __LINE__);
  if (!pXRingGauge) return;

  int16_t           nPosOld;

  // Clip position
  if (nPos < pXRingGauge->nPosMin) { nPos = pXRingGauge->nPosMin; }
  if (nPos > pXRingGauge->nPosMax) { nPos = pXRingGauge->nPosMax; }

  // Update
  nPosOld = pXRingGauge->nPos;
  pXRingGauge->nPos = nPos;

  // Only update if changed
  if (nPos != nPosOld) {
    // Mark for redraw
    // - Only need incremental redraw
    gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_INC);
  }

}

void gslc_ElemXRingGaugeSetRange(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, int16_t nPosMin, int16_t nPosMax)
{
  gslc_tsXRingGauge* pXRingGauge = (gslc_tsXRingGauge*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_RING, __LINE__);
  if (!pXRingGauge) return;

  pXRingGauge->nPosMin = nPosMin;
  pXRingGauge->nPosMax = nPosMax;

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

void gslc_ElemXRingGaugeSetRingColorFlat(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, gslc_tsColor colActive)
{
  gslc_tsXRingGauge* pXRingGauge = (gslc_tsXRingGauge*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_RING, __LINE__);
  if (!pXRingGauge) return;

  pXRingGauge->bGradient = false;
  pXRingGauge->colRing1 = colActive;
  pXRingGauge->colRing2 = colActive;

  // Mark for full redraw
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_FULL);
}

void gslc_ElemXRingGaugeSetRingColorGradient(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, gslc_tsColor colStart, gslc_tsColor colEnd)
{
  gslc_tsXRingGauge* pXRingGauge = (gslc_tsXRingGauge*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_RING, __LINE__);
  if (!pXRingGauge) return;

  pXRingGauge->bGradient = true;
  pXRingGauge->colRing1 = colStart;
  pXRingGauge->colRing2 = colEnd;

  // Mark for full redraw
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_FULL);
}


void gslc_ElemXRingGaugeSetRingColorInactive(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, gslc_tsColor colInactive)
{
  gslc_tsXRingGauge* pXRingGauge = (gslc_tsXRingGauge*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_RING, __LINE__);
  if (!pXRingGauge) return;

  pXRingGauge->bGradient = true;
  pXRingGauge->colRingRemain = colInactive;

  // Mark for full redraw
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_FULL);
}



void gslc_ElemXRingGaugeSetQuality(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, uint16_t nSegments)
{
  gslc_tsXRingGauge* pXRingGauge = (gslc_tsXRingGauge*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_RING, __LINE__);
  if (!pXRingGauge) return;

  // Convert from number of segments to degrees
  uint16_t nDeg64PerSeg = 360 * 64 / nSegments;
  pXRingGauge->nDeg64PerSeg = nDeg64PerSeg;

  // Mark for full redraw
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_FULL);
}


// ============================================================================
