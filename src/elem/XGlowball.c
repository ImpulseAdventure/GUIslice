// =======================================================================
// GUIslice library extension: XGlowball
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
/// \file XGlowball.c



// GUIslice library
#include "GUIslice.h"
#include "GUIslice_drv.h"

#include "elem/XGlowball.h"

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
// Extended Element: XGlowball
// ============================================================================

// Create a XGlowball element and add it to the GUI element list
// - Defines default styling for the element
// - Defines callback for redraw and touch
gslc_tsElemRef* gslc_ElemXGlowballCreate(gslc_tsGui* pGui, int16_t nElemId, int16_t nPage,
  gslc_tsXGlowball* pXData, int16_t nMidX, int16_t nMidY, gslc_tsXGlowballRing* pRings, uint8_t nNumRings)
{
  if ((pGui == NULL) || (pXData == NULL)) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXGlowballCreate";
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return NULL;
  }
  gslc_tsElem     sElem;
  gslc_tsElemRef* pElemRef = NULL;

  // Calculate the actual element dimensions based on the ring defintion
  // - Note that we use the outer radius of the last ring
  int16_t nRadMax;
  gslc_tsRect rElem;
  nRadMax = pRings[nNumRings - 1].nRad2;
  rElem.w = 2 * nRadMax;
  rElem.h = 2 * nRadMax;
  rElem.x = nMidX - nRadMax;
  rElem.y = nMidY - nRadMax;

  sElem = gslc_ElemCreate(pGui,nElemId,nPage,GSLC_TYPEX_GLOW,rElem,NULL,0,GSLC_FONT_NONE);
  sElem.colElemFill       = GSLC_COL_BLACK;
  sElem.colElemFillGlow   = GSLC_COL_BLACK;
  sElem.colElemFrame      = GSLC_COL_GRAY;
  sElem.colElemFrameGlow  = GSLC_COL_GRAY;
  sElem.colElemText       = GSLC_COL_WHITE;
  sElem.colElemTextGlow   = GSLC_COL_WHITE;
  sElem.nFeatures        |= GSLC_ELEM_FEA_FILL_EN;
  sElem.nFeatures        &= ~GSLC_ELEM_FEA_FRAME_EN;
  sElem.nFeatures        |= GSLC_ELEM_FEA_CLICK_EN;
  sElem.nFeatures        |= GSLC_ELEM_FEA_GLOW_EN;
  sElem.eTxtAlign         = GSLC_ALIGN_MID_LEFT;

  sElem.nGroup            = GSLC_GROUP_ID_NONE;

  // Initialize any pxData members with default parameters
  pXData->nVal = 0;
  pXData->nQuality = 72;
  pXData->nAngStart = 0;
  pXData->nAngEnd = 360;
  pXData->pRings = pRings;
  pXData->nNumRings = nNumRings;
  pXData->nMidX = nMidX;
  pXData->nMidY = nMidY;
  pXData->colBg = GSLC_COL_BLACK;
  pXData->nValLast = 0;


  sElem.pXData            = (void*)(pXData);
  // Specify the custom drawing callback
  sElem.pfuncXDraw        = &gslc_ElemXGlowballDraw;
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


void drawXGlowballArc(gslc_tsGui* pGui, gslc_tsXGlowball* pGlowball, int16_t nMidX, int16_t nMidY, int16_t nRad1, int16_t nRad2, gslc_tsColor cArc, uint16_t nAngStart, uint16_t nAngEnd)
{
  gslc_tsPt anPts[4];
  // TODO: Cleanup
  int16_t nStep64 = 64*360 / pGlowball->nQuality;
  int16_t nAng64;
  int16_t nX, nY;
  int16_t nSegStart, nSegEnd;
  nSegStart = nAngStart * pGlowball->nQuality / 360;
  nSegEnd = nAngEnd * pGlowball->nQuality / 360;

  for (int16_t nSegInd = nSegStart; nSegInd < nSegEnd; nSegInd++) {
    nAng64 = nSegInd * nStep64;
    nAng64 = nAng64 % (360 * 64);

    gslc_PolarToXY(nRad1, nAng64, &nX, &nY);
    anPts[0] = (gslc_tsPt) { nMidX + nX, nMidY + nY };
    gslc_PolarToXY(nRad2, nAng64, &nX, &nY);
    anPts[1] = (gslc_tsPt) { nMidX + nX, nMidY + nY };
    gslc_PolarToXY(nRad2, nAng64 + nStep64, &nX, &nY);
    anPts[2] = (gslc_tsPt) { nMidX + nX, nMidY + nY };
    gslc_PolarToXY(nRad1, nAng64 + nStep64, &nX, &nY);
    anPts[3] = (gslc_tsPt) { nMidX + nX, nMidY + nY };

    gslc_DrawFillQuad(pGui, anPts, cArc);
  }
}


void drawXGlowballRing(gslc_tsGui* pGui, gslc_tsXGlowball* pGlowball, int16_t nMidX, int16_t nMidY, int16_t nVal, uint16_t nAngStart, uint16_t nAngEnd, bool bErase)
{
  int16_t nRad1, nRad2;
  gslc_tsColor cCol;

  if ((nVal <= 0) || (nVal > pGlowball->nNumRings)) {
    return;
  }

  nRad1 = pGlowball->pRings[nVal - 1].nRad1;
  nRad2 = pGlowball->pRings[nVal - 1].nRad2;
  cCol = (bErase) ? pGlowball->colBg : pGlowball->pRings[nVal - 1].cCol;

  drawXGlowballArc(pGui, pGlowball, nMidX, nMidY, nRad1, nRad2, cCol, nAngStart, nAngEnd);
}

// nAngStart & nAngEnd define the range (in degrees) for the XGlowball arcs
// - Angles are measured with 0 degrees at the top, incrementing clockwise
// - Note that the supported range is 0..510 degrees
// nVal represents the value to set (0.. NUM_RINGS)
// nMidX,nMidY define the center of the XGlowball
void drawXGlowball(gslc_tsGui* pGui,gslc_tsXGlowball* pGlowball, int16_t nMidX, int16_t nMidY, int16_t nVal, uint16_t nAngStart, uint16_t nAngEnd)
{
  int16_t nValLast = pGlowball->nValLast;
  int8_t nRingInd;

  if (nVal > nValLast) {
    for (nRingInd = nValLast + 1; nRingInd <= nVal; nRingInd++) {
      drawXGlowballRing(pGui, pGlowball, nMidX, nMidY, nRingInd, nAngStart, nAngEnd, false);
    }
  } else {
    for (nRingInd = nValLast; nRingInd > nVal; nRingInd--) {
      drawXGlowballRing(pGui, pGlowball, nMidX, nMidY, nRingInd, nAngStart, nAngEnd, true);
    }
  }
  pGlowball->nValLast = nVal;
}

void gslc_ElemXGlowballSetVal(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, int16_t nVal)
{
  if ((pGui == NULL) || (pElemRef == NULL)) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXGlowballSetVal";
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return;
  }
  gslc_tsElem*      pElem = gslc_GetElemFromRef(pGui,pElemRef);
  gslc_tsXGlowball* pGlowball = (gslc_tsXGlowball*)(pElem->pXData);

  // Update the value
  pGlowball->nVal = nVal;

  // Mark for redraw
  // - Only need incremental redraw
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_INC);
}

void gslc_ElemXGlowballSetAngles(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, int16_t nAngStart, int16_t nAngEnd)
{
  if ((pGui == NULL) || (pElemRef == NULL)) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXGlowballSetAngles";
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return;
  }
  gslc_tsElem*      pElem = gslc_GetElemFromRef(pGui,pElemRef);
  gslc_tsXGlowball* pGlowball = (gslc_tsXGlowball*)(pElem->pXData);

  // Update the angular ranges
  pGlowball->nAngStart = nAngStart;
  pGlowball->nAngEnd = nAngEnd;

  // Mark for redraw
  // - Force full redraw
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_FULL);
}

void gslc_ElemXGlowballSetQuality(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, uint16_t nQuality)
{
  if ((pGui == NULL) || (pElemRef == NULL)) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXGlowballSetQuality";
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return;
  }
  gslc_tsElem*      pElem = gslc_GetElemFromRef(pGui,pElemRef);
  gslc_tsXGlowball* pGlowball = (gslc_tsXGlowball*)(pElem->pXData);

  // Update the rendering quality setting
  pGlowball->nQuality = nQuality;

  // Mark for redraw
  // - Force full redraw
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_FULL);
}

void gslc_ElemXGlowballSetColorBack(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, gslc_tsColor colBg)
{
  if ((pGui == NULL) || (pElemRef == NULL)) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXGlowballSetColorBack";
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return;
  }
  gslc_tsElem*      pElem = gslc_GetElemFromRef(pGui,pElemRef);
  gslc_tsXGlowball* pGlowball = (gslc_tsXGlowball*)(pElem->pXData);

  // Update the background color
  pGlowball->colBg = colBg;

  // Mark for redraw
  // - Force full redraw
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_FULL);
}

// Redraw the element
// - Note that this redraw is for the entire element rect region
// - The Draw function parameters use void pointers to allow for
//   simpler callback function definition & scalability.
bool gslc_ElemXGlowballDraw(void* pvGui,void* pvElemRef,gslc_teRedrawType eRedraw)
{
  if ((pvGui == NULL) || (pvElemRef == NULL)) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXGlowballDraw";
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return false;
  }
  // Typecast the parameters to match the GUI and element types
  gslc_tsGui*       pGui  = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef*   pElemRef = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem*      pElem = gslc_GetElemFromRef(pGui,pElemRef);

  // Fetch the element's extended data structure
  gslc_tsXGlowball* pGlowball;
  pGlowball = (gslc_tsXGlowball*)(pElem->pXData);
  if (pGlowball == NULL) {
    GSLC_DEBUG2_PRINT("ERROR: ElemXGlowballDraw(%s) pXData is NULL\n","");
    return false;
  }

  // --------------------------------------------------------------------------
  // Init for default drawing
  // --------------------------------------------------------------------------

  int16_t   nElemX,nElemY;
  uint16_t  nElemW,nElemH;

  nElemX    = pElem->rElem.x;
  nElemY    = pElem->rElem.y;
  nElemW    = pElem->rElem.w;
  nElemH    = pElem->rElem.h;

  // Calculate center of XGlowball
  int16_t nMidX, nMidY;
  nMidX = nElemX + (nElemW / 2);
  nMidY = nElemY + (nElemH / 2);

  // Fetch the current value
  int16_t nVal = pGlowball->nVal;
  int16_t nAngStart = pGlowball->nAngStart;
  int16_t nAngEnd = pGlowball->nAngEnd;

  // Check to see if we need full redraw
  if (eRedraw == GSLC_REDRAW_FULL) {
    // Erase the region
	  gslc_DrawFillRect(pGui, pElem->rElem, pGlowball->colBg);
    // Now force redraw from zero level to current value
	  pGlowball->nValLast = 0;
  }

  // Draw the glowball
  drawXGlowball(pGui, pGlowball, nMidX, nMidY, nVal, nAngStart, nAngEnd);
  // --------------------------------------------------------------------------

  // Mark the element as no longer requiring redraw
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_NONE);

  return true;

}

// ============================================================================
