// =======================================================================
// GUIslice library (extensions)
// - Calvin Hass
// - https://www.impulseadventure.com/elec/guislice-gui.html
// - https://github.com/ImpulseAdventure/GUIslice
// =======================================================================
//
// The MIT License
//
// Copyright 2018 Calvin Hass
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



// GUIslice library
#include "GUIslice.h"
#include "GUIslice_ex.h"
#include "GUIslice_drv.h"


#include <stdio.h>

#include <math.h>   // For sin/cos in XRadial

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
// - After adding any widgets to GUIslice_ex, a unique
//   enumeration (GSLC_TYPEX_*) should be added to "GUIslice.h"
//
//   TODO: Consider whether we should remove the need to update
//         these enumerations in "GUIslice.h"; we could instead
//         define a single "GSLC_TYPEX" in GUIslice.h but then
//         allow "GUIslice_ex.h" to create a new set of unique
//         enumerations. This way extended elements could be created
//         in GUIslice_ex and no changes at all would be required
//         in GUIslice.

// ----------------------------------------------------------------------------


// ============================================================================
// Extended Element: Gauge
// - Basic progress bar with support for positive/negative state
//   and vertical / horizontal orientation.
// ============================================================================

// Create a gauge element and add it to the GUI element list
// - Defines default styling for the element
// - Defines callback for redraw but does not track touch/click
gslc_tsElemRef* gslc_ElemXGaugeCreate(gslc_tsGui* pGui,int16_t nElemId,int16_t nPage,
  gslc_tsXGauge* pXData,gslc_tsRect rElem,
  int16_t nMin,int16_t nMax,int16_t nVal,gslc_tsColor colGauge,bool bVert)
{
  if ((pGui == NULL) || (pXData == NULL)) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXGaugeCreate";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return NULL;
  }
  gslc_tsElem     sElem;
  gslc_tsElemRef* pElemRef = NULL;
  sElem = gslc_ElemCreate(pGui,nElemId,nPage,GSLC_TYPEX_GAUGE,rElem,NULL,0,GSLC_FONT_NONE);
  sElem.nFeatures        |= GSLC_ELEM_FEA_FRAME_EN;
  sElem.nFeatures        |= GSLC_ELEM_FEA_FILL_EN;
  sElem.nFeatures        &= ~GSLC_ELEM_FEA_CLICK_EN;  // Element is not "clickable"
  sElem.nFeatures        &= ~GSLC_ELEM_FEA_GLOW_EN;
  sElem.nGroup            = GSLC_GROUP_ID_NONE;
  pXData->nMin            = nMin;
  pXData->nMax            = nMax;
  pXData->nVal            = nVal;
  pXData->nStyle          = GSLCX_GAUGE_STYLE_PROG_BAR;  // Default to progress bar
  pXData->bVert           = bVert;
  pXData->bFlip           = false;
  pXData->colGauge        = colGauge;
  pXData->colTick         = GSLC_COL_GRAY;
  pXData->nTickCnt        = 8;
  pXData->nTickLen        = 5;
  pXData->nIndicLen       = 10;     // Dummy default to be overridden
  pXData->nIndicTip       = 3;      // Dummy default to be overridden
  pXData->bIndicFill      = false;
  sElem.pXData            = (void*)(pXData);
  sElem.pfuncXDraw        = &gslc_ElemXGaugeDraw;
  sElem.pfuncXTouch       = NULL;           // No need to track touches
  sElem.colElemFill       = GSLC_COL_BLACK;
  sElem.colElemFillGlow   = GSLC_COL_BLACK;
  sElem.colElemFrame      = GSLC_COL_GRAY;
  sElem.colElemFrameGlow  = GSLC_COL_GRAY;
  if (nPage != GSLC_PAGE_NONE) {
    pElemRef = gslc_ElemAdd(pGui,nPage,&sElem,GSLC_ELEMREF_SRC_RAM);
    return pElemRef;
#if (GSLC_FEATURE_COMPOUND)
  } else {
    // Save as temporary element
    pGui->sElemTmp = sElem;
    pGui->sElemRefTmp.pElem = &(pGui->sElemTmp);
    pGui->sElemRefTmp.eElemFlags = GSLC_ELEMREF_SRC_RAM | GSLC_ELEMREF_REDRAW_FULL;
    return &(pGui->sElemRefTmp);
#endif
  }
  return NULL;
}


void gslc_ElemXGaugeSetStyle(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,gslc_teXGaugeStyle nStyle)
{
  if (pElemRef == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXGaugeSetStyle";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return;
  }
  gslc_tsElem*    pElem = gslc_GetElemFromRef(pGui,pElemRef);
  gslc_tsXGauge*  pGauge  = (gslc_tsXGauge*)(pElem->pXData);

  // Update the type element
  pGauge->nStyle = nStyle;

  // Just in case we were called at runtime, mark as needing redraw
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_FULL);
}

void gslc_ElemXGaugeSetIndicator(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,gslc_tsColor colGauge,
    uint16_t nIndicLen,uint16_t nIndicTip,bool bIndicFill)
{
  if (pElemRef == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXGaugeSetIndicator";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return;
  }
  gslc_tsElem*    pElem = gslc_GetElemFromRef(pGui,pElemRef);
  gslc_tsXGauge*  pGauge  = (gslc_tsXGauge*)(pElem->pXData);

  // Update the config
  pGauge->colGauge    = colGauge;
  pGauge->nIndicLen   = nIndicLen;
  pGauge->nIndicTip   = nIndicTip;
  pGauge->bIndicFill  = bIndicFill;

  // Just in case we were called at runtime, mark as needing redraw
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_FULL);
}

void gslc_ElemXGaugeSetTicks(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,gslc_tsColor colTick,uint16_t nTickCnt,uint16_t nTickLen)
{
  if (pElemRef == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXGaugeSetTicks";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return;
  }
  gslc_tsElem*    pElem = gslc_GetElemFromRef(pGui,pElemRef);
  gslc_tsXGauge*  pGauge  = (gslc_tsXGauge*)(pElem->pXData);

  // Update the config
  pGauge->colTick   = colTick;
  pGauge->nTickCnt  = nTickCnt;
  pGauge->nTickLen  = nTickLen;

  // Just in case we were called at runtime, mark as needing redraw
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_FULL);
}

// Update the gauge control's current position
void gslc_ElemXGaugeUpdate(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,int16_t nVal)
{
  if (pElemRef == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXGaugeUpdate";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return;
  }
  gslc_tsElem*    pElem = gslc_GetElemFromRef(pGui,pElemRef);
  gslc_tsXGauge*  pGauge  = (gslc_tsXGauge*)(pElem->pXData);

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

// Update the gauge's fill direction
// - Setting bFlip causes the gauge to be filled in the reverse direction
//   to the default
// - Default fill direction for horizontal gauges: left-to-right
// - Default fill direction for vertical gauges: bottom-to-top
void gslc_ElemXGaugeSetFlip(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,bool bFlip)
{
  if (pElemRef == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXGaugeSetFlip";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return;
  }

  // Fetch the element's extended data structure
  gslc_tsElem*    pElem = gslc_GetElemFromRef(pGui,pElemRef);
  gslc_tsXGauge*  pGauge  = (gslc_tsXGauge*)(pElem->pXData);
  if (pGauge == NULL) {
    GSLC_DEBUG_PRINT("ERROR: gslc_ElemXGaugeSetFlip(%s) pXData is NULL\n","");
    return;
  }
  pGauge->bFlip = bFlip;

  // Mark for redraw
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_FULL);

}


// Redraw the gauge
// - Note that this redraw is for the entire element rect region
// - The Draw function parameters use void pointers to allow for
//   simpler callback function definition & scalability.
bool gslc_ElemXGaugeDraw(void* pvGui,void* pvElemRef,gslc_teRedrawType eRedraw)
{
  if ((pvGui == NULL) || (pvElemRef == NULL)) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXGaugeDraw";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return false;
  }

  // Typecast the parameters to match the GUI and element types
  gslc_tsGui*       pGui      = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef*   pElemRef  = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem*      pElem = gslc_GetElemFromRef(pGui,pElemRef);

  // Fetch the element's extended data structure
  gslc_tsXGauge* pGauge;
  pGauge = (gslc_tsXGauge*)(pElem->pXData);
  if (pGauge == NULL) {
    GSLC_DEBUG_PRINT("ERROR: ElemXGaugeDraw(%s) pXData is NULL\n","");
    return false;
  }

  switch (pGauge->nStyle) {
    case GSLCX_GAUGE_STYLE_PROG_BAR:
      gslc_ElemXGaugeDrawProgressBar(pGui,pElemRef,eRedraw);
      break;
    case GSLCX_GAUGE_STYLE_RADIAL:
#if (GSLC_FEATURE_XGAUGE_RADIAL)
      gslc_ElemXGaugeDrawRadial(pGui,pElemRef,eRedraw);
#endif
      break;
    case GSLCX_GAUGE_STYLE_RAMP:
#if (GSLC_FEATURE_XGAUGE_RAMP)
      gslc_ElemXGaugeDrawRamp(pGui,pElemRef,eRedraw);
#endif
      break;
    default:
      // ERROR
      break;
  }

  // Save as "last state" to support incremental erase/redraw
  pGauge->nValLast      = pGauge->nVal;
  pGauge->bValLastValid = true;

  // Clear the redraw flag
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_NONE);

  return true;
}


bool gslc_ElemXGaugeDrawProgressBar(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,gslc_teRedrawType eRedraw)
{
  gslc_tsElem*    pElem = gslc_GetElemFromRef(pGui,pElemRef);
  gslc_tsXGauge*  pGauge  = (gslc_tsXGauge*)(pElem->pXData);

  gslc_tsRect   rTmp;           // Temporary rect for drawing
  gslc_tsRect   rGauge;         // Filled portion of gauge
  gslc_tsRect   rEmpty;         // Empty portion of gauge
  uint16_t      nElemW,nElemH;
  int16_t       nElemX0,nElemY0,nElemX1,nElemY1;
  int16_t       nGaugeX0,nGaugeY0,nGaugeX1,nGaugeY1;

  nElemX0 = pElem->rElem.x;
  nElemY0 = pElem->rElem.y;
  nElemX1 = pElem->rElem.x + pElem->rElem.w - 1;
  nElemY1 = pElem->rElem.y + pElem->rElem.h - 1;
  nElemW  = pElem->rElem.w;
  nElemH  = pElem->rElem.h;

  bool    bVert = pGauge->bVert;
  bool    bFlip = pGauge->bFlip;
  int16_t nMax  = pGauge->nMax;
  int16_t nMin  = pGauge->nMin;
  int16_t nRng  = pGauge->nMax - pGauge->nMin;

  uint16_t nScl;
  int16_t nGaugeMid;
  int16_t nLen;
  int16_t nTmp;
  int32_t nTmpL;

  if (nRng == 0) {
    GSLC_DEBUG_PRINT("ERROR: ElemXGaugeDraw() Zero gauge range [%d,%d]\n",nMin,nMax);
    return false;
  }

  if (bVert) {
    nScl = nElemH*32768/nRng;
  } else {
    nScl = nElemW*32768/nRng;
  }

  // Calculate the control midpoint (for display purposes)
  if ((nMin == 0) && (nMax >= 0)) {
    nGaugeMid = 0;
  } else if ((nMin < 0) && (nMax > 0)) {
    nTmpL     = -( (int32_t)nMin * (int32_t)nScl / 32768);
    nGaugeMid = (int16_t)nTmpL;
  } else if ((nMin < 0) && (nMax == 0)) {
    nTmpL     = -( (int32_t)nMin * (int32_t)nScl / 32768);
    nGaugeMid = (int16_t)nTmpL;
  } else {
    GSLC_DEBUG_PRINT("ERROR: ElemXGaugeDraw() Unsupported gauge range [%d,%d]\n",nMin,nMax);
    return false;
  }

  // Calculate the length of the bar
  // - Use long mult/divide to avoid need for floating point
  nTmpL = (int32_t)(pGauge->nVal) * (int32_t)(nScl) / 32768;
  nLen  = (int16_t)(nTmpL);

  // Define the gauge's fill rectangle region
  // depending on the orientation (bVert) and whether
  // the current position is negative or positive.
  if (nLen >= 0) {
    if (bVert) {
      nGaugeY0 = nElemY0 + nGaugeMid;
      nGaugeY1 = nElemY0 + nGaugeMid + nLen;
    } else {
      nGaugeX0 = nElemX0 + nGaugeMid;
      nGaugeX1 = nElemX0 + nGaugeMid + nLen;
    }
  } else {
    if (bVert) {
      nGaugeY0 = nElemY0 + nGaugeMid + nLen;
      nGaugeY1 = nElemY0 + nGaugeMid;
    } else {
      nGaugeX0 = nElemX0 + nGaugeMid + nLen;
      nGaugeX1 = nElemX0 + nGaugeMid;
    }
  }
  if (bVert) {
    nGaugeX0 = nElemX0;
    nGaugeX1 = nElemX1;
  } else {
    nGaugeY0 = nElemY0;
    nGaugeY1 = nElemY1;
  }


  // Clip the region
  nGaugeX0 = (nGaugeX0 < nElemX0)? nElemX0 : nGaugeX0;
  nGaugeY0 = (nGaugeY0 < nElemY0)? nElemY0 : nGaugeY0;
  nGaugeX1 = (nGaugeX1 > nElemX1)? nElemX1 : nGaugeX1;
  nGaugeX1 = (nGaugeX1 > nElemX1)? nElemX1 : nGaugeX1;

  // Support flipping of gauge directionality
  // - The bFlip flag reverses the fill direction
  // - Vertical gauges are flipped by default

  if (bVert && !bFlip) {
    nTmp      = nElemY0+(nElemY1-nGaugeY1);  // nTmp will be swapped into nGaugeY0
    nGaugeY1  = nElemY1-(nGaugeY0-nElemY0);
    nGaugeY0  = nTmp;
    nGaugeMid = nElemH-nGaugeMid-1;
  } else if (!bVert && bFlip) {
    nTmp      = nElemX0+(nElemX1-nGaugeX1);  // nTmp will be swapped into nGaugeX0
    nGaugeX1  = nElemX1-(nGaugeX0-nElemX0);
    nGaugeX0  = nTmp;
    nGaugeMid = nElemW-nGaugeMid-1;
  }

  #ifdef DBG_LOG
  //printf("Gauge: nMin=%4d nMax=%4d nRng=%d nVal=%4d fScl=%6.3f nGaugeMid=%4d RectX=%4d RectW=%4d\n",
  //  nMin,nMax,nRng,pGauge->nGaugeVal,fScl,nGaugeMid,rGauge.x,rGauge.w);
  #endif

  // To avoid flicker, we only erase the portion of the gauge
  // that isn't "filled". Determine the gauge empty region and erase it
  // There are two empty regions (one in negative and one in positive)
  if (bVert) {
    // Empty Region #1 (negative)
    rEmpty = (gslc_tsRect){nElemX0,nElemY0,nElemX1-nElemX0+1,nGaugeY0-nElemY0+1};
    rTmp = gslc_ExpandRect(rEmpty,-1,-1);
    gslc_DrawFillRect(pGui,rTmp,pElem->colElemFill);
    // Empty Region #2 (positive)
    rEmpty = (gslc_tsRect){nElemX0,nGaugeY1,nElemX1-nElemX0+1,nElemY1-nGaugeY1+1};
    rTmp = gslc_ExpandRect(rEmpty,-1,-1);
    gslc_DrawFillRect(pGui,rTmp,pElem->colElemFill);
  } else {
    // Empty Region #1 (negative)
    rEmpty = (gslc_tsRect){nElemX0,nElemY0,nGaugeX0-nElemX0+1,nElemY1-nElemY0+1};
    rTmp = gslc_ExpandRect(rEmpty,-1,-1);
    gslc_DrawFillRect(pGui,rTmp,pElem->colElemFill);
    // Empty Region #2 (positive)
    rEmpty = (gslc_tsRect){nGaugeX1,nElemY0,nElemX1-nGaugeX1+1,nElemY1-nElemY0+1};
    rTmp = gslc_ExpandRect(rEmpty,-1,-1);
    gslc_DrawFillRect(pGui,rTmp,pElem->colElemFill);
  }

  // Draw the gauge fill region
  rGauge = (gslc_tsRect){nGaugeX0,nGaugeY0,nGaugeX1-nGaugeX0+1,nGaugeY1-nGaugeY0+1};
  rTmp = gslc_ExpandRect(rGauge,-1,-1);
  gslc_DrawFillRect(pGui,rTmp,pGauge->colGauge);


  // Draw the midpoint line
  if (bVert) {
    gslc_DrawLine(pGui, nElemX0, nElemY0+nGaugeMid, nElemX1, nElemY0+nGaugeMid, pElem->colElemFrame);
  } else {
    gslc_DrawLine(pGui, nElemX0+nGaugeMid, nElemY0, nElemX0+nGaugeMid, nElemY1, pElem->colElemFrame);
  }


  // Draw a frame around the gauge
  gslc_DrawFrameRect(pGui,pElem->rElem,pElem->colElemFrame);

  return true;
}

#if (GSLC_FEATURE_XGAUGE_RAMP)
void gslc_ElemXGaugeDrawRadialHelp(gslc_tsGui* pGui,int16_t nX,int16_t nY,uint16_t nArrowLen,uint16_t nArrowSz,int16_t n64Ang,bool bFill,gslc_tsColor colFrame)
{
  int16_t   nTipX,nTipY;
  int16_t   nBaseX1,nBaseY1,nBaseX2,nBaseY2;
  int16_t   nTipBaseX,nTipBaseY;

  gslc_PolarToXY(nArrowLen,n64Ang,&nTipX,&nTipY);
  gslc_PolarToXY(nArrowLen-nArrowSz,n64Ang,&nTipBaseX,&nTipBaseY);
  gslc_PolarToXY(nArrowSz,n64Ang-90*64,&nBaseX1,&nBaseY1);
  gslc_PolarToXY(nArrowSz,n64Ang+90*64,&nBaseX2,&nBaseY2);

  if (!bFill) {
    // Framed
    gslc_DrawLine(pGui,nX+nBaseX1,nY+nBaseY1,nX+nBaseX1+nTipBaseX,nY+nBaseY1+nTipBaseY,colFrame);
    gslc_DrawLine(pGui,nX+nBaseX2,nY+nBaseY2,nX+nBaseX2+nTipBaseX,nY+nBaseY2+nTipBaseY,colFrame);
    gslc_DrawLine(pGui,nX+nBaseX1+nTipBaseX,nY+nBaseY1+nTipBaseY,nX+nTipX,nY+nTipY,colFrame);
    gslc_DrawLine(pGui,nX+nBaseX2+nTipBaseX,nY+nBaseY2+nTipBaseY,nX+nTipX,nY+nTipY,colFrame);
    gslc_DrawLine(pGui,nX+nBaseX1,nY+nBaseY1,nX+nBaseX2,nY+nBaseY2,colFrame);

  } else {
    // Filled
    gslc_tsPt asPt[4];

    // Main body of pointer
    asPt[0] = (gslc_tsPt){nX+nBaseX1,nY+nBaseY1};
    asPt[1] = (gslc_tsPt){nX+nBaseX1+nTipBaseX,nY+nBaseY1+nTipBaseY};
    asPt[2] = (gslc_tsPt){nX+nBaseX2+nTipBaseX,nY+nBaseY2+nTipBaseY};
    asPt[3] = (gslc_tsPt){nX+nBaseX2,nY+nBaseY2};
    gslc_DrawFillQuad(pGui,asPt,colFrame);

    // Tip of pointer
    asPt[0] = (gslc_tsPt){nX+nBaseX1+nTipBaseX,nY+nBaseY1+nTipBaseY};
    asPt[1] = (gslc_tsPt){nX+nTipX,nY+nTipY};
    asPt[2] = (gslc_tsPt){nX+nBaseX2+nTipBaseX,nY+nBaseY2+nTipBaseY};
    gslc_DrawFillTriangle(pGui,asPt[0].x,asPt[0].y,asPt[1].x,asPt[1].y,asPt[2].x,asPt[2].y,colFrame);

  }

}

bool gslc_ElemXGaugeDrawRadial(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,gslc_teRedrawType eRedraw)
{
  gslc_tsElem*    pElem = gslc_GetElemFromRef(pGui,pElemRef);
  gslc_tsXGauge*  pGauge  = (gslc_tsXGauge*)(pElem->pXData);

  uint16_t      nElemW,nElemH,nElemRad;
  int16_t       nElemX0,nElemY0,nElemX1,nElemY1;
  int16_t       nElemMidX,nElemMidY;
  nElemX0   = pElem->rElem.x;
  nElemY0   = pElem->rElem.y;
  nElemX1   = pElem->rElem.x + pElem->rElem.w - 1;
  nElemY1   = pElem->rElem.y + pElem->rElem.h - 1;
  nElemMidX = (nElemX0+nElemX1)/2;
  nElemMidY = (nElemY0+nElemY1)/2;
  nElemW    = pElem->rElem.w;
  nElemH    = pElem->rElem.h;
  nElemRad  = (nElemW>=nElemH)? nElemH/2 : nElemW/2;

  int16_t   nMax            = pGauge->nMax;
  int16_t   nMin            = pGauge->nMin;
  int16_t   nRng            = pGauge->nMax - pGauge->nMin;
  int16_t   nVal            = pGauge->nVal;
  int16_t   nValLast        = pGauge->nValLast;
  bool      bValLastValid   = pGauge->bValLastValid;
  uint16_t  nTickLen        = pGauge->nTickLen;
  uint16_t  nTickAng        = 360 / pGauge->nTickCnt;
  uint16_t  nArrowLen       = pGauge->nIndicLen;
  uint16_t  nArrowSize      = pGauge->nIndicTip;
  bool      bFill           = pGauge->bIndicFill;

  int16_t   n64Ang,n64AngLast;
  int16_t   nInd;


  if (nRng == 0) {
    GSLC_DEBUG_PRINT("ERROR: ElemXRadialDraw() Zero range [%d,%d]\n",nMin,nMax);
    return false;
  }

  // Support reversing of direction
  if (pGauge->bFlip) {
    n64Ang      = (nMax - nVal    )* 360*64 /nRng;
    n64AngLast  = (nMax - nValLast)* 360*64 /nRng;
  } else {
    n64Ang      = (nVal     - nMin)* 360*64 /nRng;
    n64AngLast  = (nValLast - nMin)* 360*64 /nRng;
  }

  // Clear old
  if (bValLastValid) {
    gslc_ElemXGaugeDrawRadialHelp(pGui,nElemMidX,nElemMidY,nArrowLen,nArrowSize,n64AngLast,bFill,pElem->colElemFill);
  }

  // Draw frame
  if (eRedraw == GSLC_REDRAW_FULL) {
    gslc_DrawFillCircle(pGui,nElemMidX,nElemMidY,nElemRad,pElem->colElemFill);  // Erase first
    gslc_DrawFrameCircle(pGui,nElemMidX,nElemMidY,nElemRad,pElem->colElemFrame);
    for (nInd=0;nInd<360;nInd+=nTickAng) {
      gslc_DrawLinePolar(pGui,nElemMidX,nElemMidY,nElemRad-nTickLen,nElemRad,nInd*64,pGauge->colTick);
    }
  }

  // Draw pointer
  gslc_ElemXGaugeDrawRadialHelp(pGui,nElemMidX,nElemMidY,nArrowLen,nArrowSize,n64Ang,bFill,pGauge->colGauge);

  return true;
}
#endif // GSLC_FEATURE_XGAUGE_RADIAL

#ifdef GSLC_FEATURE_XGAUGE_RAMP
bool gslc_ElemXGaugeDrawRamp(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,gslc_teRedrawType eRedraw)
{
  gslc_tsElem*    pElem = gslc_GetElemFromRef(pGui,pElemRef);
  gslc_tsXGauge*  pGauge  = (gslc_tsXGauge*)(pElem->pXData);

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
    GSLC_DEBUG_PRINT("ERROR: gslc_ElemXGaugeDrawRamp() Zero range [%d,%d]\n",nMin,nMax);
    return false;
  }

  uint16_t  nSclFX;
  uint16_t  nHeight;
  int32_t   nHeightTmp;
  uint16_t  nHeightBot;
  uint16_t  nX;
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

  nSclFX = nElemH*32767/(nElemW*nElemW);

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
        nColInd = nSegStart*1000/nElemW;
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
#endif // GLSC_FEATURE_XGAUGE_RAMP


// ============================================================================
// Extended Element: Checkbox
// - Checkbox with custom handler for touch tracking which
//   enables glow to be defined whenever touch is tracked over
//   the element.
// ============================================================================

// Create a checkbox element and add it to the GUI element list
// - Defines default styling for the element
// - Defines callback for redraw but does not track touch/click
gslc_tsElemRef* gslc_ElemXCheckboxCreate(gslc_tsGui* pGui,int16_t nElemId,int16_t nPage,
  gslc_tsXCheckbox* pXData,gslc_tsRect rElem,bool bRadio,gslc_teXCheckboxStyle nStyle,
  gslc_tsColor colCheck,bool bChecked)
{
  if ((pGui == NULL) || (pXData == NULL)) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXCheckboxCreate";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return NULL;
  }
  gslc_tsElem     sElem;
  gslc_tsElemRef* pElemRef = NULL;
  sElem = gslc_ElemCreate(pGui,nElemId,nPage,GSLC_TYPEX_CHECKBOX,rElem,NULL,0,GSLC_FONT_NONE);
  sElem.nFeatures        &= ~GSLC_ELEM_FEA_FRAME_EN;
  sElem.nFeatures        |= GSLC_ELEM_FEA_FILL_EN;
  sElem.nFeatures        |= GSLC_ELEM_FEA_CLICK_EN;
  sElem.nFeatures        |= GSLC_ELEM_FEA_GLOW_EN;

  // Default group assignment. Can override later with ElemSetGroup()
  sElem.nGroup            = GSLC_GROUP_ID_NONE;

  // Define other extended data
  pXData->bRadio          = bRadio;
  pXData->bChecked        = bChecked;
  pXData->colCheck        = colCheck;
  pXData->nStyle          = nStyle;
  sElem.pXData            = (void*)(pXData);
  // Specify the custom drawing callback
  sElem.pfuncXDraw        = &gslc_ElemXCheckboxDraw;
  // Specify the custom touch tracking callback
  // - NOTE: This is optional (and can be set to NULL).
  //   See the discussion under gslc_ElemXCheckboxTouch()
  sElem.pfuncXTouch       = &gslc_ElemXCheckboxTouch;
  sElem.colElemFill       = GSLC_COL_BLACK;
  sElem.colElemFillGlow   = GSLC_COL_BLACK;
  sElem.colElemFrame      = GSLC_COL_GRAY;
  sElem.colElemFrameGlow  = GSLC_COL_WHITE;
  if (nPage != GSLC_PAGE_NONE) {
    pElemRef = gslc_ElemAdd(pGui,nPage,&sElem,GSLC_ELEMREF_SRC_RAM);
    return pElemRef;
#if (GSLC_FEATURE_COMPOUND)
  } else {
    // Save as temporary element
    pGui->sElemTmp = sElem;
    pGui->sElemRefTmp.pElem = &(pGui->sElemTmp);
    pGui->sElemRefTmp.eElemFlags = GSLC_ELEMREF_SRC_RAM | GSLC_ELEMREF_REDRAW_FULL;
    return &(pGui->sElemRefTmp);
#endif
  }
  return NULL;
}

bool gslc_ElemXCheckboxGetState(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef)
{
  gslc_tsElem*        pElem = gslc_GetElemFromRef(pGui,pElemRef);
  gslc_tsXCheckbox*   pCheckbox  = (gslc_tsXCheckbox*)(pElem->pXData);

  return pCheckbox->bChecked;
}


// Determine which checkbox in the group has been "checked"
gslc_tsElemRef* gslc_ElemXCheckboxFindChecked(gslc_tsGui* pGui,int16_t nGroupId)
{
  int16_t             nCurInd;
  gslc_tsElemRef*     pCurElemRef = NULL;
  gslc_tsElem*        pCurElem = NULL;
  int16_t             nCurType;
  int16_t             nCurGroup;
  bool                bCurChecked;
  gslc_tsElemRef*     pFoundElemRef = NULL;

  if (pGui->pCurPage == NULL) {
    return NULL; // No page added yet
  }

  if (pGui == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXCheckboxFindChecked";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return NULL;
  }
  for (nCurInd=0;nCurInd<pGui->pCurPageCollect->nElemCnt;nCurInd++) {
    // Fetch extended data
    pCurElemRef   = &(pGui->pCurPageCollect->asElemRef[nCurInd]);
    pCurElem      = gslc_GetElemFromRef(pGui,pCurElemRef);
    nCurType      = pCurElem->nType;
    // Only want to proceed if it is a checkbox
    if (nCurType != GSLC_TYPEX_CHECKBOX) {
      continue;
    }

    nCurGroup     = pCurElem->nGroup;
    bCurChecked   = gslc_ElemXCheckboxGetState(pGui,pCurElemRef);

    // If this is in a different group, ignore it
    if (nCurGroup != nGroupId) {
      continue;
    }

    // Did we find an element in the group that was checked?
    if (bCurChecked) {
      pFoundElemRef = pCurElemRef;
      break;
    }
  } // nCurInd
  return pFoundElemRef;
}

// Helper routine for gslc_ElemXCheckboxSetState()
// - Updates the checkbox/radio control's state but does
//   not touch any other controls in the group
void gslc_ElemXCheckboxSetStateHelp(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,bool bChecked)
{
  if (pElemRef == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXCheckboxSetStateHelp";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return;
  }
  gslc_tsElem*        pElem = gslc_GetElemFromRef(pGui,pElemRef);
  gslc_tsXCheckbox*   pCheckbox = (gslc_tsXCheckbox*)(pElem->pXData);


  // Update our data element
  bool  bCheckedOld = pCheckbox->bChecked;
  pCheckbox->bChecked = bChecked;

  // Element needs redraw
  if (bChecked != bCheckedOld) {
    // Only need an incremental redraw
    gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_INC);
  }

}

// Update the checkbox/radio control's state. If it is a radio button
// then also update the state of all other buttons in the group.
void gslc_ElemXCheckboxSetState(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,bool bChecked)
{
  if (pElemRef == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXCheckboxSetState";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return;
  }
  if (pGui->pCurPage == NULL) {
    return; // No page added yet
  }

  gslc_tsElem*        pElem = gslc_GetElemFromRef(pGui,pElemRef);
  gslc_tsXCheckbox*   pCheckbox = (gslc_tsXCheckbox*)(pElem->pXData);
  bool                bRadio    = pCheckbox->bRadio;
  int16_t             nGroup    = pElem->nGroup;
  int16_t             nElemId   = pElem->nId;

  // If radio-button style and we are selecting an element
  // then proceed to deselect any other selected items in the group.
  // Note that SetState calls itself to deselect other items so it
  // is important to qualify this logic with bChecked=true
  if (bRadio && bChecked) {
    int16_t           nCurInd;
    int16_t           nCurId;
    gslc_tsElem*      pCurElem = NULL;
    gslc_tsElemRef*   pCurElemRef = NULL;
    int16_t           nCurType;
    int16_t           nCurGroup;

    // We use the GUI pointer for access to other elements
    // NOTE: There is an assumption that we are calling ElemXCheckboxSetState
    //       on a checkbox on the current page.
    for (nCurInd=0;nCurInd<pGui->pCurPageCollect->nElemRefCnt;nCurInd++) {
      // Fetch extended data
      pCurElemRef   = &pGui->pCurPageCollect->asElemRef[nCurInd];
      pCurElem      = gslc_GetElemFromRef(pGui,pCurElemRef);

      // FIXME: Handle pCurElemRef->eElemFlags
      nCurId        = pCurElem->nId;
      nCurType      = pCurElem->nType;

      // Only want to proceed if it is a checkbox
      if (nCurType != GSLC_TYPEX_CHECKBOX) {
        continue;
      }

      nCurGroup     = pCurElem->nGroup;

      // If this is in a different group, ignore it
      if (nCurGroup != nGroup) {
        continue;
      }

      // Is this our element? If so, ignore the deselect operation
      if (nCurId == nElemId) {
        continue;
      }

      // Deselect all other elements
      gslc_ElemXCheckboxSetStateHelp(pGui,pCurElemRef,false);

    } // nInd

  } // bRadio

  // Set the state of the current element
  gslc_ElemXCheckboxSetStateHelp(pGui,pElemRef,bChecked);
}

// Toggle the checkbox control's state
// TODO: Consider adding pGui to param so that we can
//       pass global context down to ElemXCheckboxSetState()
//       which would enable cross-element access (via group).
//       Current code is saving pGui in pXData for checkbox
//       which is undesirable.
void gslc_ElemXCheckboxToggleState(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef)
{
  if (pElemRef == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXCheckboxToggleState";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return;
  }
  // Update the data element
  bool bCheckNew = (gslc_ElemXCheckboxGetState(pGui,pElemRef))? false : true;
  gslc_ElemXCheckboxSetState(pGui,pElemRef,bCheckNew);

  // Element needs redraw
  // - Only incremental is needed
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_INC);
}

// Redraw the checkbox
// - Note that this redraw is for the entire element rect region
// - The Draw function parameters use void pointers to allow for
//   simpler callback function definition & scalability.
bool gslc_ElemXCheckboxDraw(void* pvGui,void* pvElemRef,gslc_teRedrawType eRedraw)
{
  if ((pvGui == NULL) || (pvElemRef == NULL)) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXCheckboxDraw";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return false;
  }
  // Typecast the parameters to match the GUI and element types
  gslc_tsGui*     pGui      = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef  = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem*    pElem     = gslc_GetElemFromRef(pGui,pElemRef);

  gslc_tsRect   rInner;

  // Fetch the element's extended data structure
  gslc_tsXCheckbox* pCheckbox;
  pCheckbox = (gslc_tsXCheckbox*)(pElem->pXData);
  if (pCheckbox == NULL) {
    GSLC_DEBUG_PRINT("ERROR: ElemXCheckboxDraw(%s) pXData is NULL\n","");
    return false;
  }

  bool bChecked  = pCheckbox->bChecked;
  gslc_teXCheckboxStyle   nStyle = pCheckbox->nStyle;
  bool bGlow     = (pElem->nFeatures & GSLC_ELEM_FEA_GLOW_EN) && gslc_ElemGetGlow(pGui,pElemRef);

  // Draw the background
  gslc_DrawFillRect(pGui,pElem->rElem,pElem->colElemFill);

  // Generic coordinate calcs
  int16_t nX0,nY0,nX1,nY1,nMidX,nMidY;
  nX0 = pElem->rElem.x;
  nY0 = pElem->rElem.y;
  nX1 = pElem->rElem.x + pElem->rElem.w - 1;
  nY1 = pElem->rElem.y + pElem->rElem.h - 1;
  nMidX = (nX0+nX1)/2;
  nMidY = (nY0+nY1)/2;
  if (nStyle == GSLCX_CHECKBOX_STYLE_BOX) {
    // Draw the center indicator if checked
    rInner = gslc_ExpandRect(pElem->rElem,-5,-5);
    if (bChecked) {
      // If checked, fill in the inner region
      gslc_DrawFillRect(pGui,rInner,pCheckbox->colCheck);
    } else {
      // Assume the background fill has already been done so
      // we don't need to do anything more in the unchecked case
    }
    // Draw a frame around the checkbox
    gslc_DrawFrameRect(pGui,pElem->rElem,(bGlow)?pElem->colElemFrameGlow:pElem->colElemFrame);

  } else if (nStyle == GSLCX_CHECKBOX_STYLE_X) {
    // Draw an X through center if checked
    if (bChecked) {
      gslc_DrawLine(pGui,nX0,nY0,nX1,nY1,pCheckbox->colCheck);
      gslc_DrawLine(pGui,nX0,nY1,nX1,nY0,pCheckbox->colCheck);
    }
    // Draw a frame around the checkbox
    gslc_DrawFrameRect(pGui,pElem->rElem,(bGlow)?pElem->colElemFrameGlow:pElem->colElemFrame);

  } else if (nStyle == GSLCX_CHECKBOX_STYLE_ROUND) {
    // Draw inner circle if checked
    if (bChecked) {
      gslc_DrawFillCircle(pGui,nMidX,nMidY,5,pCheckbox->colCheck);
    }
    // Draw a frame around the checkbox
    gslc_DrawFrameCircle(pGui,nMidX,nMidY,(pElem->rElem.w/2),(bGlow)?pElem->colElemFrameGlow:pElem->colElemFrame);

  }

  // Clear the redraw flag
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_NONE);

  // Mark page as needing flip
  gslc_PageFlipSet(pGui,true);

  return true;
}

// This callback function is called by gslc_ElemSendEventTouch()
// after any touch event
// - NOTE: Adding this touch callback is optional. Without it, we
//   can still have a functional checkbox, but doing the touch
//   tracking allows us to change the glow state of the element
//   dynamically, as well as updating the checkbox state if the
//   user releases over it (ie. a click event).
//
bool gslc_ElemXCheckboxTouch(void* pvGui,void* pvElemRef,gslc_teTouch eTouch,int16_t nRelX,int16_t nRelY)
{
#if defined(DRV_TOUCH_NONE)
  return false;
#else

  if ((pvGui == NULL) || (pvElemRef == NULL)) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXCheckboxTouch";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return false;
  }
  gslc_tsGui*           pGui = NULL;
  gslc_tsElemRef*       pElemRef = NULL;
  gslc_tsElem*          pElem = NULL;
  gslc_tsXCheckbox*     pCheckbox = NULL;

  // Typecast the parameters to match the GUI
  pGui  = (gslc_tsGui*)(pvGui);
  pElemRef = (gslc_tsElemRef*)(pvElemRef);
  pElem = gslc_GetElemFromRef(pGui,pElemRef);
  pCheckbox = (gslc_tsXCheckbox*)(pElem->pXData);

  bool  bRadio      = pCheckbox->bRadio;
  bool  bCheckedOld = pCheckbox->bChecked;
  bool  bGlowingOld = gslc_ElemGetGlow(pGui,pElemRef);

  switch(eTouch) {

    case GSLC_TOUCH_DOWN_IN:
      gslc_ElemSetGlow(pGui,pElemRef,true);
      break;

    case GSLC_TOUCH_MOVE_IN:
      gslc_ElemSetGlow(pGui,pElemRef,true);
      break;
    case GSLC_TOUCH_MOVE_OUT:
      gslc_ElemSetGlow(pGui,pElemRef,false);
      break;

    case GSLC_TOUCH_UP_IN:
      gslc_ElemSetGlow(pGui,pElemRef,false);
      // Now that we released on element, update the state
      bool  bCheckNew;
      if (bRadio) {
        // Radio button action: set
        bCheckNew = true;
      } else {
        // Checkbox button action: toggle
        bCheckNew = (pCheckbox->bChecked)?false:true;
      }
      gslc_ElemXCheckboxSetState(pGui,pElemRef,bCheckNew);
      break;
    case GSLC_TOUCH_UP_OUT:
      gslc_ElemSetGlow(pGui,pElemRef,false);
      break;

    default:
      return false;
      break;
  }

  // If the checkbox changed state, redraw
  bool  bChanged = false;
  if (gslc_ElemGetGlow(pGui,pElemRef) != bGlowingOld) { bChanged = true; }
  if (pCheckbox->bChecked != bCheckedOld) { bChanged = true; }
  if (bChanged) {
    // Incremental redraw
    gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_INC);
  }

  return true;
  #endif // !DRV_TOUCH_NONE
}

// ============================================================================
// Extended Element: Slider
// - A linear slider control
// ============================================================================

// Create a slider element and add it to the GUI element list
// - Defines default styling for the element
// - Defines callback for redraw and touch
gslc_tsElemRef* gslc_ElemXSliderCreate(gslc_tsGui* pGui,int16_t nElemId,int16_t nPage,
  gslc_tsXSlider* pXData,gslc_tsRect rElem,int16_t nPosMin,int16_t nPosMax,int16_t nPos,
  uint16_t nThumbSz,bool bVert)
{
  if ((pGui == NULL) || (pXData == NULL)) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXSliderCreate";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return NULL;
  }
  gslc_tsElem     sElem;
  gslc_tsElemRef* pElemRef = NULL;
  sElem = gslc_ElemCreate(pGui,nElemId,nPage,GSLC_TYPEX_SLIDER,rElem,NULL,0,GSLC_FONT_NONE);
  sElem.nFeatures        &= ~GSLC_ELEM_FEA_FRAME_EN;
  sElem.nFeatures        |= GSLC_ELEM_FEA_FILL_EN;
  sElem.nFeatures        |= GSLC_ELEM_FEA_CLICK_EN;
  sElem.nFeatures        |= GSLC_ELEM_FEA_GLOW_EN;

  sElem.nGroup            = GSLC_GROUP_ID_NONE;
  pXData->nPosMin         = nPosMin;
  pXData->nPosMax         = nPosMax;
  pXData->nPos            = nPos;
  pXData->nThumbSz        = nThumbSz;
  pXData->bVert           = bVert;
  pXData->bTrim           = false;
  pXData->colTrim         = GSLC_COL_BLACK;
  pXData->nTickDiv        = 0;
  pXData->pfuncXPos       = NULL;
  sElem.pXData            = (void*)(pXData);
  // Specify the custom drawing callback
  sElem.pfuncXDraw        = &gslc_ElemXSliderDraw;
  // Specify the custom touch tracking callback
  sElem.pfuncXTouch       = &gslc_ElemXSliderTouch;

  sElem.colElemFill       = GSLC_COL_BLACK;
  sElem.colElemFillGlow   = GSLC_COL_BLACK;
  sElem.colElemFrame      = GSLC_COL_GRAY;
  sElem.colElemFrameGlow  = GSLC_COL_WHITE;

  if (nPage != GSLC_PAGE_NONE) {
    pElemRef = gslc_ElemAdd(pGui,nPage,&sElem,GSLC_ELEMREF_SRC_RAM);
    return pElemRef;
#if (GSLC_FEATURE_COMPOUND)
  } else {
    // Save as temporary element
    pGui->sElemTmp = sElem;
    pGui->sElemRefTmp.pElem = &(pGui->sElemTmp);
    pGui->sElemRefTmp.eElemFlags = GSLC_ELEMREF_SRC_RAM | GSLC_ELEMREF_REDRAW_FULL;
    return &(pGui->sElemRefTmp);
#endif
  }
  return NULL;
}

void gslc_ElemXSliderSetStyle(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,
        bool bTrim,gslc_tsColor colTrim,uint16_t nTickDiv,
        int16_t nTickLen,gslc_tsColor colTick)
{
  if (pElemRef == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXSliderSetStyle";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return;
  }
  gslc_tsElem*    pElem = gslc_GetElemFromRef(pGui,pElemRef);
  gslc_tsXSlider* pSlider = (gslc_tsXSlider*)(pElem->pXData);

  pSlider->bTrim      = bTrim;
  pSlider->colTrim    = colTrim;
  pSlider->nTickDiv   = nTickDiv;
  pSlider->nTickLen   = nTickLen;
  pSlider->colTick    = colTick;

  // Update
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_FULL);
}

int gslc_ElemXSliderGetPos(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef)
{
  if (pElemRef == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXSliderGetPos";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return 0;
  }
  gslc_tsElem*    pElem = gslc_GetElemFromRef(pGui,pElemRef);
  gslc_tsXSlider* pSlider = (gslc_tsXSlider*)(pElem->pXData);

  return pSlider->nPos;
}

// Update the slider control's current state
void gslc_ElemXSliderSetPos(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,int16_t nPos)
{
  if ((pGui == NULL) || (pElemRef == NULL)) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXSliderSetPos";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return;
  }
  gslc_tsElem*      pElem = gslc_GetElemFromRef(pGui,pElemRef);
  gslc_tsXSlider*   pSlider = (gslc_tsXSlider*)(pElem->pXData);
  int16_t           nPosOld;
  // Clip position
  if (nPos < pSlider->nPosMin) { nPos = pSlider->nPosMin; }
  if (nPos > pSlider->nPosMax) { nPos = pSlider->nPosMax; }
  // Update
  nPosOld = pSlider->nPos;
  pSlider->nPos = nPos;

  // Only update if changed
  if (nPos != nPosOld) {
    // If any position callback is defined, call it now
    if (pSlider->pfuncXPos != NULL) {
      (*pSlider->pfuncXPos)((void*)(pGui),(void*)(pElemRef),nPos);
    }

    // Mark for redraw
    // - Only need incremental redraw
    gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_INC);
  }

}


// Assign the position callback function for a slider
void gslc_ElemXSliderSetPosFunc(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,GSLC_CB_XSLIDER_POS funcCb)
{
  if ((pElemRef == NULL) || (funcCb == NULL)) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXSliderSetPosFunc";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return;
  }
  gslc_tsElem*      pElem = gslc_GetElemFromRef(pGui,pElemRef);
  gslc_tsXSlider*   pSlider = (gslc_tsXSlider*)(pElem->pXData);
  pSlider->pfuncXPos = funcCb;
}


// Redraw the slider
// - Note that this redraw is for the entire element rect region
// - The Draw function parameters use void pointers to allow for
//   simpler callback function definition & scalability.
bool gslc_ElemXSliderDraw(void* pvGui,void* pvElemRef,gslc_teRedrawType eRedraw)
{
  if ((pvGui == NULL) || (pvElemRef == NULL)) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXSliderDraw";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return false;
  }
  // Typecast the parameters to match the GUI and element types
  gslc_tsGui*       pGui  = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef*   pElemRef = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem*      pElem = gslc_GetElemFromRef(pGui,pElemRef);

  // Fetch the element's extended data structure
  gslc_tsXSlider* pSlider;
  pSlider = (gslc_tsXSlider*)(pElem->pXData);
  if (pSlider == NULL) {
    GSLC_DEBUG_PRINT("ERROR: ElemXSliderDraw(%s) pXData is NULL\n","");
    return false;
  }

  bool            bGlow     = (pElem->nFeatures & GSLC_ELEM_FEA_GLOW_EN) && gslc_ElemGetGlow(pGui,pElemRef);
  int16_t         nPos      = pSlider->nPos;
  int16_t         nPosMin   = pSlider->nPosMin;
  int16_t         nPosMax   = pSlider->nPosMax;
  bool            bVert     = pSlider->bVert;
  int16_t         nThumbSz  = pSlider->nThumbSz;
  bool            bTrim     = pSlider->bTrim;
  gslc_tsColor    colTrim   = pSlider->colTrim;
  uint16_t        nTickDiv  = pSlider->nTickDiv;
  int16_t         nTickLen  = pSlider->nTickLen;
  gslc_tsColor    colTick   = pSlider->colTick;

  int16_t nX0,nY0,nX1,nY1,nXMid,nYMid;
  nX0 = pElem->rElem.x;
  nY0 = pElem->rElem.y;
  nX1 = pElem->rElem.x + pElem->rElem.w - 1;
  nY1 = pElem->rElem.y + pElem->rElem.h - 1;
  nXMid = (nX0+nX1)/2;
  nYMid = (nY0+nY1)/2;

  // Scale the current position
  int16_t nPosRng = nPosMax-nPosMin;
  // TODO: Check for nPosRng=0, reversed min/max
  int16_t nPosOffset = nPos-nPosMin;

  // Provide some margin so thumb doesn't exceed control bounds
  int16_t nMargin   = nThumbSz;
  int16_t nCtrlRng;
  if (!bVert) {
    nCtrlRng = (nX1-nMargin)-(nX0+nMargin);
  } else {
    nCtrlRng = (nY1-nMargin)-(nY0+nMargin);
  }
  int16_t nCtrlPos  = (nPosOffset*nCtrlRng/nPosRng)+nMargin;


  // Draw the background
  // - TODO: To reduce flicker on unbuffered displays, one could consider
  //         redrawing only the thumb (last drawn position) with fill and
  //         then redraw other portions. This would prevent the
  //         track / ticks from flickering needlessly. A full redraw would
  //         be required if it was first draw action.
  gslc_DrawFillRect(pGui,pElem->rElem,(bGlow)?pElem->colElemFillGlow:pElem->colElemFill);

  // Draw any ticks
  // - Need at least one tick segment
  if (nTickDiv>=1) {
    uint16_t  nTickInd;
    int16_t   nTickOffset;
    for (nTickInd=0;nTickInd<=nTickDiv;nTickInd++) {
      nTickOffset = nTickInd * nCtrlRng / nTickDiv;
      if (!bVert) {
        gslc_DrawLine(pGui,nX0+nMargin+ nTickOffset,nYMid,
                nX0+nMargin + nTickOffset,nYMid+nTickLen,colTick);
      } else {
        gslc_DrawLine(pGui,nXMid,nY0+nMargin+ nTickOffset,
                nXMid+nTickLen,nY0+nMargin + nTickOffset,colTick);
      }
    }
  }


  // Draw the track
  if (!bVert) {
    // Make the track highlight during glow
    gslc_DrawLine(pGui,nX0+nMargin,nYMid,nX1-nMargin,nYMid,
            bGlow? pElem->colElemFrameGlow : pElem->colElemFrame);
    // Optionally draw a trim line
    if (bTrim) {
      gslc_DrawLine(pGui,nX0+nMargin,nYMid+1,nX1-nMargin,nYMid+1,colTrim);
    }

  } else {
    // Make the track highlight during glow
    gslc_DrawLine(pGui,nXMid,nY0+nMargin,nXMid,nY1-nMargin,
            bGlow? pElem->colElemFrameGlow : pElem->colElemFrame);
    // Optionally draw a trim line
    if (bTrim) {
      gslc_DrawLine(pGui,nXMid+1,nY0+nMargin,nXMid+1,nY1-nMargin,colTrim);
    }
  }


  int16_t       nCtrlX0,nCtrlY0;
  gslc_tsRect   rThumb;
  if (!bVert) {
    nCtrlX0   = nX0+nCtrlPos-nThumbSz;
    nCtrlY0   = nYMid-nThumbSz;
  } else {
    nCtrlX0   = nXMid-nThumbSz;
    nCtrlY0   = nY0+nCtrlPos-nThumbSz;
  }
  rThumb.x  = nCtrlX0;
  rThumb.y  = nCtrlY0;
  rThumb.w  = 2*nThumbSz;
  rThumb.h  = 2*nThumbSz;

  // Draw the thumb control
  gslc_DrawFillRect(pGui,rThumb,(bGlow)?pElem->colElemFillGlow:pElem->colElemFill);
  gslc_DrawFrameRect(pGui,rThumb,(bGlow)?pElem->colElemFrameGlow:pElem->colElemFrame);
  if (bTrim) {
    gslc_tsRect  rThumbTrim;
    rThumbTrim = gslc_ExpandRect(rThumb,-1,-1);
    gslc_DrawFrameRect(pGui,rThumbTrim,pSlider->colTrim);
  }


  // Clear the redraw flag
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_NONE);

  // Mark page as needing flip
  gslc_PageFlipSet(pGui,true);

  return true;
}


// This callback function is called by gslc_ElemSendEventTouch()
// after any touch event
bool gslc_ElemXSliderTouch(void* pvGui,void* pvElemRef,gslc_teTouch eTouch,int16_t nRelX,int16_t nRelY)
{
#if defined(DRV_TOUCH_NONE)
  return false;
#else

  if ((pvGui == NULL) || (pvElemRef == NULL)) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXSliderTouch";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return false;
  }
  gslc_tsGui*           pGui = NULL;
  gslc_tsElemRef*       pElemRef = NULL;
  gslc_tsElem*          pElem = NULL;
  gslc_tsXSlider*       pSlider = NULL;

  // Typecast the parameters to match the GUI
  pGui      = (gslc_tsGui*)(pvGui);
  pElemRef  = (gslc_tsElemRef*)(pvElemRef);
  pElem     = gslc_GetElemFromRef(pGui,pElemRef);
  pSlider   = (gslc_tsXSlider*)(pElem->pXData);

  bool    bGlowingOld = gslc_ElemGetGlow(pGui,pElemRef);
  int16_t nPosRng;
  int16_t nPos;
  bool    bUpdatePos = false;

  switch(eTouch) {

    case GSLC_TOUCH_DOWN_IN:
      // Start glowing as must be over it
      gslc_ElemSetGlow(pGui,pElemRef,true);
      bUpdatePos = true;
      break;

    case GSLC_TOUCH_MOVE_IN:
      gslc_ElemSetGlow(pGui,pElemRef,true);
      bUpdatePos = true;
      break;
    case GSLC_TOUCH_MOVE_OUT:
      gslc_ElemSetGlow(pGui,pElemRef,false);
      bUpdatePos = true;
      break;

    case GSLC_TOUCH_UP_IN:
      // End glow
      gslc_ElemSetGlow(pGui,pElemRef,false);
      break;
    case GSLC_TOUCH_UP_OUT:
      // End glow
      gslc_ElemSetGlow(pGui,pElemRef,false);
      break;

    default:
      return false;
      break;
  }

  // Perform additional range checking
  nRelX = (nRelX < 0)? 0 : nRelX;
  nRelY = (nRelY < 0)? 0 : nRelY;


  // If we need to update the slider position, calculate the value
  // and perform the update
  if (bUpdatePos) {
    // Calc new position
    nPosRng = pSlider->nPosMax - pSlider->nPosMin;
    if (!pSlider->bVert) {
      nPos = (nRelX * nPosRng / pElem->rElem.w) + pSlider->nPosMin;
    } else {
      nPos = (nRelY * nPosRng / pElem->rElem.h) + pSlider->nPosMin;
    }
    // Update the slider
    gslc_ElemXSliderSetPos(pGui,pElemRef,nPos);
  }

  // If the slider changed state, redraw
  if (gslc_ElemGetGlow(pGui,pElemRef) != bGlowingOld) {
    gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_FULL);
  }

  return true;
  #endif // !DRV_TOUCH_NONE
}


#if (GSLC_FEATURE_COMPOUND)
// ============================================================================
// Extended Element: SelNum
// - SelNum (Select Number) element demonstrates a simple up/down counter
// - This is a compound element containing two buttons and
//   a text area to represent the current count
// ============================================================================

// Private sub Element ID definitions
static const int16_t  SELNUM_ID_BTN_INC = 100;
static const int16_t  SELNUM_ID_BTN_DEC = 101;
static const int16_t  SELNUM_ID_TXT     = 102;

// Create a compound element
// - For now just two buttons and a text area
gslc_tsElemRef* gslc_ElemXSelNumCreate(gslc_tsGui* pGui,int16_t nElemId,int16_t nPage,
  gslc_tsXSelNum* pXData,gslc_tsRect rElem,int8_t nFontId)
{

  if ((pGui == NULL) || (pXData == NULL)) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXSelNumCreate";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return NULL;
  }
  gslc_tsElem sElem;


  // Initialize composite element
  sElem = gslc_ElemCreate(pGui,nElemId,nPage,GSLC_TYPEX_SELNUM,rElem,NULL,0,GSLC_FONT_NONE);
  sElem.nFeatures        |= GSLC_ELEM_FEA_FRAME_EN;
  sElem.nFeatures        |= GSLC_ELEM_FEA_FILL_EN;
  sElem.nFeatures        |= GSLC_ELEM_FEA_CLICK_EN;
  sElem.nFeatures        &= ~GSLC_ELEM_FEA_GLOW_EN;  // Don't need to glow outer element
  sElem.nGroup            = GSLC_GROUP_ID_NONE;

  pXData->nCounter      = 0;

  // Determine the maximum number of elements that we can store
  // in the sub-element array. We do this at run-time with sizeof()
  // instead of using #define to avoid polluting the global namespace.
  int16_t nSubElemMax = sizeof(pXData->asElem) / sizeof(pXData->asElem[0]);

  // NOTE: The count parameters in CollectReset() must match the size of
  //       the asElem[] array. It is used for bounds checking when we
  //       add new elements.
  // NOTE: We only use RAM for subelement storage
  gslc_CollectReset(&pXData->sCollect,pXData->asElem,nSubElemMax,pXData->asElemRef,nSubElemMax);


  sElem.pXData            = (void*)(pXData);
  // Specify the custom drawing callback
  sElem.pfuncXDraw        = &gslc_ElemXSelNumDraw;
  // Specify the custom touch tracking callback
  sElem.pfuncXTouch       = &gslc_ElemXSelNumTouch;

  sElem.colElemFill       = GSLC_COL_BLACK;
  sElem.colElemFillGlow   = GSLC_COL_BLACK;
  sElem.colElemFrame      = GSLC_COL_GRAY;
  sElem.colElemFrameGlow  = GSLC_COL_WHITE;


  // Now create the sub elements
  // - Ensure page is set to GSLC_PAGE_NONE so that
  //   we create the element struct but not add it to a specific page.
  // - When we create an element with GSLC_PAGE_NONE it is
  //   saved in the GUI's temporary element storage.
  // - When we have finished creating / styling the element, we then
  //   copy it into the permanent sub-element storage

  // - The element IDs assigned to the sub-elements are
  //   arbitrary (with local scope in the compound element),
  //   so they don't need to be unique globally across the GUI.
  gslc_tsElemRef* pElemRefTmp   = NULL;
  gslc_tsElem*    pElemTmp      = NULL;
  gslc_tsElemRef* pElemRef      = NULL;

  // Determine offset coordinate of compound element so that we can
  // specify relative positioning during the sub-element Create() operations.
  int16_t nOffsetX = rElem.x;
  int16_t nOffsetY = rElem.y;

  #if (GSLC_LOCAL_STR)
  pElemRefTmp = gslc_ElemCreateBtnTxt(pGui,SELNUM_ID_BTN_INC,GSLC_PAGE_NONE,
    (gslc_tsRect){nOffsetX+40,nOffsetY+10,30,30},"+",0,nFontId,&gslc_ElemXSelNumClick);
  #else
  strncpy(pXData->acElemTxt[0],"+",SELNUM_STR_LEN-1);
  pElemRefTmp = gslc_ElemCreateBtnTxt(pGui,SELNUM_ID_BTN_INC,GSLC_PAGE_NONE,
    (gslc_tsRect){nOffsetX+40,nOffsetY+10,30,30},pXData->acElemTxt[0],SELNUM_STR_LEN,
    nFontId,&gslc_ElemXSelNumClick);
  #endif
  gslc_ElemSetCol(pGui,pElemRefTmp,(gslc_tsColor){0,0,192},(gslc_tsColor){0,0,128},(gslc_tsColor){0,0,224});
  gslc_ElemSetTxtCol(pGui,pElemRefTmp,GSLC_COL_WHITE);
  pElemTmp = gslc_GetElemFromRef(pGui,pElemRefTmp);
  gslc_CollectElemAdd(pGui,&pXData->sCollect,pElemTmp,GSLC_ELEMREF_SRC_RAM);

  #if (GSLC_LOCAL_STR)
  pElemRefTmp = gslc_ElemCreateBtnTxt(pGui,SELNUM_ID_BTN_DEC,GSLC_PAGE_NONE,
    (gslc_tsRect){nOffsetX+80,nOffsetY+10,30,30},"-",0,nFontId,&gslc_ElemXSelNumClick);
  #else
  strncpy(pXData->acElemTxt[1],"-",SELNUM_STR_LEN-1);
  pElemRefTmp = gslc_ElemCreateBtnTxt(pGui,SELNUM_ID_BTN_DEC,GSLC_PAGE_NONE,
    (gslc_tsRect){nOffsetX+80,nOffsetY+10,30,30},pXData->acElemTxt[1],SELNUM_STR_LEN,
    nFontId,&gslc_ElemXSelNumClick);
  #endif
  gslc_ElemSetCol(pGui,pElemRefTmp,(gslc_tsColor){0,0,192},(gslc_tsColor){0,0,128},(gslc_tsColor){0,0,224});
  gslc_ElemSetTxtCol(pGui,pElemRefTmp,GSLC_COL_WHITE);
  pElemTmp = gslc_GetElemFromRef(pGui,pElemRefTmp);
  gslc_CollectElemAdd(pGui,&pXData->sCollect,pElemTmp,GSLC_ELEMREF_SRC_RAM);

  #if (GSLC_LOCAL_STR)
  pElemRefTmp = gslc_ElemCreateTxt(pGui,SELNUM_ID_TXT,GSLC_PAGE_NONE,
    (gslc_tsRect){nOffsetX+10,nOffsetY+10,20,30},"0",0,nFontId);
  #else
  strncpy(pXData->acElemTxt[2],"0",SELNUM_STR_LEN-1);
  pElemRefTmp = gslc_ElemCreateTxt(pGui,SELNUM_ID_TXT,GSLC_PAGE_NONE,
    (gslc_tsRect){nOffsetX+10,nOffsetY+10,20,30},pXData->acElemTxt[2],SELNUM_STR_LEN,nFontId);
  #endif
  pElemTmp = gslc_GetElemFromRef(pGui,pElemRefTmp);
  gslc_CollectElemAdd(pGui,&pXData->sCollect,pElemTmp,GSLC_ELEMREF_SRC_RAM);


  // Now proceed to add the compound element to the page
  if (nPage != GSLC_PAGE_NONE) {
    pElemRef = gslc_ElemAdd(pGui,nPage,&sElem,GSLC_ELEMREF_SRC_RAM);

    // Now propagate the parent relationship to enable a cascade
    // of redrawing from low-level elements to the top
    gslc_CollectSetParent(pGui,&pXData->sCollect,pElemRef);

    return pElemRef;
  } else {
    GSLC_DEBUG_PRINT("ERROR: ElemXSelNumCreate(%s) Compound elements inside compound elements not supported\n","");
    return NULL;

    // TODO: For now, disable compound elements within
    // compound elements. If we want to enable this, we
    // would probably use the temporary element reference
    // the GUI.
    // Save as temporary element for further processing
    //pGui->sElemTmp = sElem;   // Need fixing
    //return &(pGui->sElemTmp); // Need fixing
  }

}


// Redraw the compound element
// - When drawing a compound element, we clear the background
//   and then redraw the sub-element collection.
bool gslc_ElemXSelNumDraw(void* pvGui,void* pvElemRef,gslc_teRedrawType eRedraw)
{
  if ((pvGui == NULL) || (pvElemRef == NULL)) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXSelNumDraw";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return false;
  }
  // Typecast the parameters to match the GUI and element types
  gslc_tsGui*     pGui      = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef  = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem*    pElem     = gslc_GetElemFromRef(pGui,pElemRef);

  bool bGlow = (pElem->nFeatures & GSLC_ELEM_FEA_GLOW_EN) && gslc_ElemGetGlow(pGui,pElemRef);

  // Fetch the element's extended data structure
  gslc_tsXSelNum* pSelNum;
  pSelNum = (gslc_tsXSelNum*)(pElem->pXData);
  if (pSelNum == NULL) {
    GSLC_DEBUG_PRINT("ERROR: ElemXSelNumDraw(%s) pXData is NULL\n","");
    return false;
  }

  // Draw the compound element fill (background)
  // - Should only need to do this in full redraw
  if (eRedraw == GSLC_REDRAW_FULL) {
    gslc_DrawFillRect(pGui,pElem->rElem,(bGlow)?pElem->colElemFillGlow:pElem->colElemFill);
  }

  // Draw the sub-elements
  // - For now, force redraw of entire compound element
  gslc_tsCollect* pCollect = &pSelNum->sCollect;

  gslc_tsEvent  sEvent = gslc_EventCreate(pGui,GSLC_EVT_DRAW,GSLC_EVTSUB_DRAW_FORCE,(void*)(pCollect),NULL);
  gslc_CollectEvent(pGui,sEvent);

  // Optionally, draw a frame around the compound element
  // - This could instead be done by creating a sub-element
  //   of type box.
  // - We don't need to show any glowing of the compound element

  gslc_DrawFrameRect(pGui,pElem->rElem,(bGlow)?pElem->colElemFrameGlow:pElem->colElemFrame);

  // Clear the redraw flag
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_NONE);

  // Mark page as needing flip
  gslc_PageFlipSet(pGui,true);

  return true;
}

// Fetch the current value of the element's counter
int gslc_ElemXSelNumGetCounter(gslc_tsGui* pGui,gslc_tsXSelNum* pSelNum)
{
  if ((pGui == NULL) || (pSelNum == NULL)) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXSelNumGetCounter";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return 0;
  }
  return pSelNum->nCounter;
}


void gslc_ElemXSelNumSetCounter(gslc_tsGui* pGui,gslc_tsXSelNum* pSelNum,int16_t nCount)
{
  if (pSelNum == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXSelNumSetCounter";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return;
  }
  pSelNum->nCounter = nCount;

  // Determine new counter text
  // FIXME: Consider replacing the printf() with an optimized function to
  //        conserve RAM. Potentially leverage GSLC_DEBUG_PRINT().
  char  acStrNew[GSLC_LOCAL_STR_LEN];
  snprintf(acStrNew,GSLC_LOCAL_STR_LEN,"%hd",pSelNum->nCounter);

  // Update the element
  gslc_tsElemRef* pElemRef = gslc_CollectFindElemById(pGui,&pSelNum->sCollect,SELNUM_ID_TXT);
  gslc_ElemSetTxtStr(pGui,pElemRef,acStrNew);

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
bool gslc_ElemXSelNumClick(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY)
{
#if defined(DRV_TOUCH_NONE)
  return false;
#else

  if ((pvGui == NULL) || (pvElemRef == NULL)) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXSelNumClick";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return false;
  }
  gslc_tsGui*     pGui      = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef  = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem*    pElem     = gslc_GetElemFromRef(pGui,pElemRef);


  // Fetch the parent of the clicked element which is the compound
  // element itself. This enables us to access the extra control data.
  gslc_tsElemRef*    pElemRefParent = pElem->pElemRefParent;
  if (pElemRefParent == NULL) {
    GSLC_DEBUG_PRINT("ERROR: ElemXSelNumClick(%s) parent ElemRef ptr NULL\n","");
    return false;
  }

  gslc_tsElem*    pElemParent = gslc_GetElemFromRef(pGui,pElemRefParent);
  gslc_tsXSelNum* pSelNum     = (gslc_tsXSelNum*)(pElemParent->pXData);
  if (pSelNum == NULL) {
    GSLC_DEBUG_PRINT("ERROR: ElemXSelNumClick() element (ID=%d) has NULL pXData\n",pElem->nId);
    return false;
  }

  // Begin the core compound element functionality
  int nCounter  = pSelNum->nCounter;

  // Handle the various button presses
  if (eTouch == GSLC_TOUCH_UP_IN) {

    // Get the tracked element ID
    gslc_tsElemRef* pElemRefTracked = pSelNum->sCollect.pElemRefTracked;
    gslc_tsElem*    pElemTracked    = gslc_GetElemFromRef(pGui,pElemRefTracked);
    int nSubElemId = pElemTracked->nId;

    if (nSubElemId == SELNUM_ID_BTN_INC) {
      // Increment button
      if (nCounter<100) {
        nCounter++;
      }
      gslc_ElemXSelNumSetCounter(pGui,pSelNum,nCounter);

    } else if (nSubElemId == SELNUM_ID_BTN_DEC) {
      // Decrement button
      if (nCounter>0) {
        nCounter--;
      }
      gslc_ElemXSelNumSetCounter(pGui,pSelNum,nCounter);

    }
  } // eTouch

  return true;
#endif // !DRV_TOUCH_NONE
}

bool gslc_ElemXSelNumTouch(void* pvGui,void* pvElemRef,gslc_teTouch eTouch,int16_t nRelX,int16_t nRelY)
{
#if defined(DRV_TOUCH_NONE)
  return false;
#else

  if ((pvGui == NULL) || (pvElemRef == NULL)) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXSelNumTouch";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return false;
  }
  gslc_tsGui*           pGui = NULL;
  gslc_tsElemRef*       pElemRef = NULL;
  gslc_tsElem*          pElem = NULL;
  gslc_tsXSelNum*       pSelNum = NULL;


  // Typecast the parameters to match the GUI
  pGui      = (gslc_tsGui*)(pvGui);
  pElemRef  = (gslc_tsElemRef*)(pvElemRef);
  pElem     = gslc_GetElemFromRef(pGui,pElemRef);
  pSelNum   = (gslc_tsXSelNum*)(pElem->pXData);


  // Handle any compound element operations
  switch(eTouch) {
    case GSLC_TOUCH_DOWN_IN:
    case GSLC_TOUCH_MOVE_IN:
      gslc_ElemSetGlow(pGui,pElemRef,true);
      break;
    case GSLC_TOUCH_MOVE_OUT:
    case GSLC_TOUCH_UP_IN:
    case GSLC_TOUCH_UP_OUT:
    default:
      gslc_ElemSetGlow(pGui,pElemRef,false);
      break;
  }

  // Handle any sub-element operations

  // Get Collection
  gslc_tsCollect* pCollect = &pSelNum->sCollect;

  // Now reset the in/out status of the touch event since
  // we are now looking for coordinates of a sub-element
  // rather than the compound element. gslc_CollectTouch()
  // is responsible for determining in/out status at the
  // next level down in the element hierarchy.
  eTouch &= ~GSLC_TOUCH_INOUT_MASK;

  // Cascade the touch event to the sub-element collection
  // - Note that we use absolute coordinates
  gslc_tsEventTouch sEventTouch;
  sEventTouch.nX      = pElem->rElem.x + nRelX;
  sEventTouch.nY      = pElem->rElem.y + nRelY;
  sEventTouch.eTouch  = eTouch;
  gslc_CollectTouch(pGui,pCollect,&sEventTouch);

  // Mark compound element as needing redraw if any
  // sub-element needs redraw
  if (gslc_CollectGetRedraw(pGui,pCollect)) {
    gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_FULL);
  }

  return true;
  #endif // !DRV_TOUCH_NONE
}
#endif // GSLC_FEATURE_COMPOUND

// ============================================================================

gslc_tsElemRef* gslc_ElemXTextboxCreate(gslc_tsGui* pGui,int16_t nElemId,int16_t nPage,
  gslc_tsXTextbox* pXData,gslc_tsRect rElem,int16_t nFontId,char* pBuf,
    uint16_t nBufRows,uint16_t nBufCols)
{
  if ((pGui == NULL) || (pXData == NULL)) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXTextboxCreate";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return NULL;
  }
  gslc_tsElem     sElem;
  gslc_tsElemRef* pElemRef = NULL;
  sElem = gslc_ElemCreate(pGui,nElemId,nPage,GSLC_TYPEX_TEXTBOX,rElem,NULL,0,nFontId);
  sElem.nFeatures        |= GSLC_ELEM_FEA_FRAME_EN;
  sElem.nFeatures        |= GSLC_ELEM_FEA_FILL_EN;
  sElem.nFeatures        &= ~GSLC_ELEM_FEA_CLICK_EN;
  sElem.nFeatures        &= ~GSLC_ELEM_FEA_GLOW_EN;
  // Default group assignment. Can override later with ElemSetGroup()
  sElem.nGroup            = GSLC_GROUP_ID_NONE;

  // Define other extended data
  pXData->pBuf            = pBuf;
  pXData->nMargin         = 5;
  pXData->bWrapEn         = true;
  pXData->nCurPosX        = 0;
  pXData->nCurPosY        = 0;

  pXData->nBufRows        = nBufRows;
  pXData->nBufCols        = nBufCols;
  pXData->nBufPosX        = 0;
  pXData->nBufPosY        = 0;
  pXData->nWndRowStart    = 0;


  // Clear the buffer
  memset(pBuf,0,nBufRows*nBufCols*sizeof(char));

  // Precalculate certain parameters
  // Determine the maximum size of a character
  // - For now, assume we are using a monospaced font and derive
  //   text pixel coords from the size of a worst-case character.
  // - TODO: Update to determine worst-case character (might not be "%")
  // - TODO: Update to handle offset values fom DrvGetTxtSize()
  int16_t       nChOffsetX,nChOffsetY;
  uint16_t      nChSzW,nChSzH;
  char          acMono[2] = "%";
  gslc_DrvGetTxtSize(pGui,sElem.pTxtFont,(char*)&acMono,sElem.eTxtFlags,&nChOffsetX,&nChOffsetY,&nChSzW,&nChSzH);
  pXData->nWndCols = (rElem.w - (2*pXData->nMargin)) / nChSzW;
  pXData->nWndRows = (rElem.h - (2*pXData->nMargin)) / nChSzH;
  pXData->nChSizeX = nChSzW;
  pXData->nChSizeY = nChSzH;

  // Determine if scrollbar should be enabled
  if (pXData->nWndRows >= pXData->nBufRows) {
    // Disable scrollbar as the window is larger
    // than the number of rows in the buffer
    pXData->bScrollEn   = false;
    pXData->nScrollPos  = 0;
  } else {
    // Scrollbar is enabled
    pXData->bScrollEn   = true;
    pXData->nScrollPos  = 0;
  }

  sElem.pXData            = (void*)(pXData);

  // Specify the custom drawing callback
  sElem.pfuncXDraw        = &gslc_ElemXTextboxDraw;
  sElem.pfuncXTouch       = NULL;
  sElem.colElemFill       = GSLC_COL_BLACK;
  sElem.colElemFillGlow   = GSLC_COL_BLACK;
  sElem.colElemFrame      = GSLC_COL_GRAY;
  sElem.colElemFrameGlow  = GSLC_COL_WHITE;
  if (nPage != GSLC_PAGE_NONE) {
    pElemRef = gslc_ElemAdd(pGui,nPage,&sElem,GSLC_ELEMREF_SRC_RAM);
    return pElemRef;
#if (GSLC_FEATURE_COMPOUND)
  } else {
    // Save as temporary element
    pGui->sElemTmp = sElem;
    pGui->sElemRefTmp.pElem = &(pGui->sElemTmp);
    pGui->sElemRefTmp.eElemFlags = GSLC_ELEMREF_SRC_RAM | GSLC_ELEMREF_REDRAW_FULL;
    return &(pGui->sElemRefTmp);
#endif
  }
  return NULL;
}


void gslc_ElemXTextboxReset(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef)
{
  if (pElemRef == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXTextboxReset";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return;
  }
  gslc_tsXTextbox*  pBox;
  gslc_tsElem*      pElem = gslc_GetElemFromRef(pGui,pElemRef);
  pBox = (gslc_tsXTextbox*)(pElem->pXData);

  // Reset the positional state
  pBox->nCurPosX        = 0;
  pBox->nCurPosY        = 0;
  pBox->nBufPosX        = 0;
  pBox->nBufPosY        = 0;
  pBox->nWndRowStart    = 0;

  // Clear the buffer
  memset(pBox->pBuf,0,pBox->nBufRows*pBox->nBufCols*sizeof(char));

  // Set the redraw flag
  // - Only need incremental redraw
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_INC);
}

// Advance the buffer writer to the next line
// The window is also shifted if we are eating the first row
void gslc_ElemXTextboxLineWrAdv(gslc_tsGui* pGui,gslc_tsXTextbox* pBox)
{
  pBox->nBufPosX = 0;
  pBox->nBufPosY++;

  // Wrap the pointers around end of buffer
  pBox->nBufPosY      = pBox->nBufPosY % pBox->nBufRows;

  // Did the buffer write pointer start to encroach upon
  // the visible window region? If so, shift the window
  if (pBox->nBufPosY == pBox->nWndRowStart) {
    // Advance the window (with wrap if needed)
    pBox->nWndRowStart = (pBox->nWndRowStart + 1) % pBox->nBufRows;
  }
}

void gslc_ElemXTextboxScrollSet(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,uint8_t nScrollPos,uint8_t nScrollMax)
{

  gslc_tsXTextbox*  pBox;
  gslc_tsElem*      pElem = gslc_GetElemFromRef(pGui,pElemRef);
  pBox = (gslc_tsXTextbox*)(pElem->pXData);

  // Ensure scrollbar is enabled
  if (!pBox->bScrollEn) {
    // Scrollbar is disabled, so ignore
    return;
  }

  // Assign proportional value based on visible window region
  uint16_t nScrollPosOld = pBox->nScrollPos;
  pBox->nScrollPos = nScrollPos * (pBox->nBufRows - pBox->nWndRows) / nScrollMax;

  // Set the redraw flag
  // - Only need incremental redraw
  // - Only redraw if changed actual scroll row
  if (pBox->nScrollPos != nScrollPosOld) {
    gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_INC);
  }
}

// Write a character to the buffer
// - Advance the write ptr, wrap if needed
// - If encroach upon buffer read ptr, then drop the oldest line from the buffer
// NOTE: This should not be called with newline char!
void gslc_ElemXTextboxBufAdd(gslc_tsGui* pGui,gslc_tsXTextbox* pBox,unsigned char chNew,bool bAdvance)
{
  // Ensure that we haven't gone past end of line
  // - Note that we have to leave one extra byte for the line terminator (NULL)
  if ((pBox->nBufPosX+1) >= pBox->nBufCols) {
    if (pBox->bWrapEn) {
      // Perform line wrap
      // - Force a null at the end of the current line first
      pBox->pBuf[pBox->nBufPosY * pBox->nBufCols + (pBox->nBufCols-1)] = 0;
      gslc_ElemXTextboxLineWrAdv(pGui,pBox);
    } else {
      // Ignore the write
      return;
    }
  }

  uint16_t    nBufPos = pBox->nBufPosY * pBox->nBufCols + pBox->nBufPosX;

  // Add the character
  pBox->pBuf[nBufPos] = chNew;

  // Optionally advance the pointer
  // - The only time we don't advance is if we added NULL
  //   but note that in some special commands there may be
  //   zero values added, so we still need to advance these
  if (bAdvance) {
    pBox->nBufPosX++;
  }

}

void gslc_ElemXTextboxColSet(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,gslc_tsColor nCol)
{
#if (GSLC_FEATURE_XTEXTBOX_EMBED == 0)
  GSLC_DEBUG_PRINT("ERROR: gslc_ElemXTextboxColSet() not enabled. Requires GSLC_FEATURE_XTEXTBOX_EMBED=1 %s\n","");
  return;
#else
  gslc_tsXTextbox*  pBox = NULL;
  gslc_tsElem*      pElem = gslc_GetElemFromRef(pGui,pElemRef);
  pBox = (gslc_tsXTextbox*)(pElem->pXData);

  // Ensure that there are enough free columns in current
  // buffer row to accommodate the color code (4 bytes)
  if (pBox->nBufPosX +4 >= pBox->nBufCols) {
    // Not enough space for the code, so ignore it
    GSLC_DEBUG_PRINT("ERROR: gslc_ElemXTextboxColSet() not enough cols [Pos=%u Cols=%u]\n",pBox->nBufPosX,pBox->nBufCols);
    return;
  }

  gslc_ElemXTextboxBufAdd(pGui,pBox,GSLC_XTEXTBOX_CODE_COL_SET,true);
  gslc_ElemXTextboxBufAdd(pGui,pBox,nCol.r,true);
  gslc_ElemXTextboxBufAdd(pGui,pBox,nCol.g,true);
  gslc_ElemXTextboxBufAdd(pGui,pBox,nCol.b,true);
#endif
}

void gslc_ElemXTextboxColReset(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef)
{
#if (GSLC_FEATURE_XTEXTBOX_EMBED == 0)
  GSLC_DEBUG_PRINT("ERROR: gslc_ElemXTextboxColReset() not enabled. Requires GSLC_FEATURE_XTEXTBOX_EMBED=1 %s\n","");
  return;
#else
  gslc_tsXTextbox*  pBox = NULL;
  gslc_tsElem*      pElem = gslc_GetElemFromRef(pGui,pElemRef);
  pBox = (gslc_tsXTextbox*)(pElem->pXData);
  gslc_ElemXTextboxBufAdd(pGui,pBox,GSLC_XTEXTBOX_CODE_COL_RESET,true);
#endif
}

void gslc_ElemXTextboxWrapSet(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,bool bWrapEn)
{
  gslc_tsXTextbox*  pBox = NULL;
  gslc_tsElem*      pElem = gslc_GetElemFromRef(pGui,pElemRef);
  pBox = (gslc_tsXTextbox*)(pElem->pXData);
  pBox->bWrapEn = bWrapEn;
}


void gslc_ElemXTextboxAdd(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,char* pTxt)
{
  gslc_tsXTextbox*  pBox = NULL;
  gslc_tsElem*      pElem = gslc_GetElemFromRef(pGui,pElemRef);
  pBox = (gslc_tsXTextbox*)(pElem->pXData);

  // Warn the user about mode compatibility
#if (GSLC_FEATURE_XTEXTBOX_EMBED)
  static bool bWarned = false;  // Warn only once
  bool bEncUtf8 = ((pElem->eTxtFlags & GSLC_TXT_ENC) == GSLC_TXT_ENC_UTF8);
  if ((!bWarned) && (bEncUtf8)) {
    // Continue to render the text, but issue warning to the user
    GSLC_DEBUG_PRINT("WARNING: ElemXTextboxAdd(%s) UTF-8 encoding not supported in GSLC_FEATURE_XTEXTBOX_EMBED=1 mode\n","");
    bWarned = true;
  }
#endif

  // Add null-terminated string to the bottom of the buffer
  // If the string exceeds the buffer length then it will wrap
  // back to the beginning.
  // TODO: Ensure that buffer wrap doesn't encroach upon visible region!
  // TODO: Assert (pBox)
  bool            bDone = false;
  uint16_t        nTxtPos = 0;
  unsigned char   chNext;

  if (pTxt == NULL) { bDone = true; }
  while (!bDone) {
    chNext = pTxt[nTxtPos];
    nTxtPos++;
    if (chNext == 0) {
      // Reached terminator character
      // Add terminator to buffer but don't advance write pointer
      // since we want next write to overwrite this
      gslc_ElemXTextboxBufAdd(pGui,pBox,0,false);

      bDone = true;
      continue;
    }

    // FIXME: It is possible that the following check may no longer be
    // appropriate when using UTF-8 encoding mode.
    if (chNext == '\n') {
      // Terminate the line
      gslc_ElemXTextboxBufAdd(pGui,pBox,0,false);
      // Advance the writer by one line
      gslc_ElemXTextboxLineWrAdv(pGui,pBox);
    } else {
      // TODO: Check to see if we are in mask/truncate state
      // Note that this routine also handles line wrap
      gslc_ElemXTextboxBufAdd(pGui,pBox,chNext,true);
    }
  }

  // Set the redraw flag
  // - Only need incremental redraw
  // - TODO: Detect case of single line-update and limit redraw
  //         to line instead of redrawing entire control. Whenever
  //         the line is advanced, the full control content should
  //         be redrawn.
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_INC);
}

bool gslc_ElemXTextboxDraw(void* pvGui,void* pvElemRef,gslc_teRedrawType eRedraw)
{
  if ((pvGui == NULL) || (pvElemRef == NULL)) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXTextboxDraw";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return false;
  }
  // Typecast the parameters to match the GUI and element types
  gslc_tsGui*     pGui      = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef  = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem*    pElem     = gslc_GetElemFromRef(pGui,pElemRef);

  // Fetch the element's extended data structure
  gslc_tsXTextbox* pBox;
  pBox = (gslc_tsXTextbox*)(pElem->pXData);
  if (pBox == NULL) {
    GSLC_DEBUG_PRINT("ERROR: ElemXTextboxDraw(%s) pXData is NULL\n","");
    return false;
  }

  bool     bGlow     = (pElem->nFeatures & GSLC_ELEM_FEA_GLOW_EN) && gslc_ElemGetGlow(pGui,pElemRef);
  bool     bFrameEn  = (pElem->nFeatures & GSLC_ELEM_FEA_FRAME_EN);

  // Draw the frame
  if (eRedraw == GSLC_REDRAW_FULL) {
    if (bFrameEn) {
      gslc_DrawFrameRect(pGui,pElem->rElem,pElem->colElemFrame);
    }
  }

  // Clear the background (inset from frame)
  gslc_tsRect rInner = gslc_ExpandRect(pElem->rElem,-1,-1);
  gslc_DrawFillRect(pGui,rInner,(bGlow)?pElem->colElemFillGlow:pElem->colElemFill);

  uint16_t          nBufPos = 0;

  uint16_t          nTxtPixX;
  uint16_t          nTxtPixY;
  gslc_tsColor      colTxt;
  //bool            bEncUtf8;

  // Determine what encoding method is used for text
  // Not used at the moment
  //bEncUtf8 = ((pElem->eTxtFlags & GSLC_TXT_ENC) == GSLC_TXT_ENC_UTF8);

  // Initialize color state
  colTxt = pElem->colElemText;

  // Calculate the starting row for the window
  uint16_t nWndRowStartScr = pBox->nWndRowStart;

  // Only correct for scrollbar position if enabled
  if (pBox->bScrollEn) {
    nWndRowStartScr = (pBox->nWndRowStart + pBox->nScrollPos) % pBox->nBufRows;
  }

#if (GSLC_FEATURE_XTEXTBOX_EMBED == 0)

  // Normal mode support (no embedded text color)
  // - This mode is much faster and is able to support UTF-8 text encoding

  uint8_t nCurY = 0;

  uint8_t nOutRow = 0;
  uint8_t nMaxRow = 0;

  nMaxRow = (pBox->nBufRows < pBox->nWndRows)? pBox->nBufRows : pBox->nWndRows;
  for (nOutRow=0;nOutRow<nMaxRow;nOutRow++) {

    // Calculate row offset after accounting for buffer wrap
    // and current window starting offset
    uint16_t nRowCur = nWndRowStartScr + nOutRow;
    nRowCur = nRowCur % pBox->nBufRows;

    // NOTE: At the start of buffer fill where we have
    // only written a couple rows, we don't stop reading
    // across all of the rows. We are dependent upon
    // the reset to initialize all rows with NULL terminator
    // so that we don't show garbage.

    nBufPos = nRowCur * pBox->nBufCols;

    nTxtPixX = pElem->rElem.x + pBox->nMargin + 0 * pBox->nChSizeX;
    nTxtPixY = pElem->rElem.y + pBox->nMargin + nCurY * pBox->nChSizeY;
    gslc_DrvDrawTxt(pGui,nTxtPixX,nTxtPixY,pElem->pTxtFont,(char*)&(pBox->pBuf[nBufPos]),pElem->eTxtFlags,colTxt);

    nCurY++;
  } // nOutRow

#else

  // Embedded color mode support
  // - This mode supports inline changing of text color
  // - However, it does not support UTF-8 character encoding
  // - It is also slower since rendering is per-character

  enum              {TBOX_NORM, TBOX_COL_SET};
  int16_t           eTBoxState = TBOX_NORM;
  uint16_t          nTBoxStateCnt = 0;

  unsigned char     chNext;
  uint8_t           nCurX = 0;
  uint8_t           nCurY = 0;

  uint8_t nOutRow = 0;
  uint8_t nOutCol = 0;
  uint8_t nMaxCol = 0;
  uint8_t nMaxRow = 0;
  bool    bRowDone = false;
  nMaxCol = (pBox->nBufCols < pBox->nWndCols)? pBox->nBufCols : pBox->nWndCols;
  nMaxRow = (pBox->nBufRows < pBox->nWndRows)? pBox->nBufRows : pBox->nWndRows;
  for (nOutRow=0;nOutRow<nMaxRow;nOutRow++) {
    bRowDone = false;
    nCurX = 0;
    for (nOutCol=0;(!bRowDone)&&(nOutCol<nMaxCol);nOutCol++) {

      // Calculate row offset after accounting for buffer wrap
      // and current window starting offset
      uint16_t nRowCur = nWndRowStartScr + nOutRow;
      nRowCur = nRowCur % pBox->nBufRows;

      // NOTE: At the start of buffer fill where we have
      // only written a couple rows, we don't stop reading
      // across all of the rows. We are dependent upon
      // the reset to initialize all rows with NULL terminator
      // so that we don't show garbage.

      nBufPos = nRowCur * pBox->nBufCols + nOutCol;
      chNext = pBox->pBuf[nBufPos];

      if (eTBoxState == TBOX_NORM) {
        if (chNext == 0) {
          // Reached early terminator
          bRowDone = true;
          continue;
        } else if (chNext == GSLC_XTEXTBOX_CODE_COL_SET) {
          // Set color (enter FSM)
          eTBoxState = TBOX_COL_SET;
          nTBoxStateCnt = 0;
        } else if (chNext == GSLC_XTEXTBOX_CODE_COL_RESET) {
          // Reset color
          colTxt = pElem->colElemText;
        } else {

          // Render the character
          // TODO: Optimize by coalescing all characters in row before calling DrvDrawTxt
          //       - Note that this would make it harder to change aspects (such as color)
          //         in mid-line.
          char  acChToDraw[2] = "";
          acChToDraw[0] = chNext;
          acChToDraw[1] = 0;
          nTxtPixX = pElem->rElem.x + pBox->nMargin + nCurX * pBox->nChSizeX;
          nTxtPixY = pElem->rElem.y + pBox->nMargin + nCurY * pBox->nChSizeY;
          gslc_DrvDrawTxt(pGui,nTxtPixX,nTxtPixY,pElem->pTxtFont,(char*)&acChToDraw,pElem->eTxtFlags,colTxt);

          nCurX++;

        }

      } else if (eTBoxState == TBOX_COL_SET) {
        nTBoxStateCnt++;
        if      (nTBoxStateCnt == 1) { colTxt.r = chNext; }
        else if (nTBoxStateCnt == 2) { colTxt.g = chNext; }
        else if (nTBoxStateCnt == 3) {
          colTxt.b = chNext;
          eTBoxState = TBOX_NORM;
        }
      } // eTBoxState

    } // nOutCol
    nCurY++;
  } // nOutRow

#endif // GSLC_FEATURE_XTEXTBOX_EMBED

  // Clear the redraw flag
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_NONE);

  // Mark page as needing flip
  gslc_PageFlipSet(pGui,true);

  return true;
}

// ============================================================================

gslc_tsElemRef* gslc_ElemXGraphCreate(gslc_tsGui* pGui,int16_t nElemId,int16_t nPage,
  gslc_tsXGraph* pXData,gslc_tsRect rElem,int16_t nFontId,int16_t* pBuf,
  uint16_t nBufMax,gslc_tsColor colGraph)
{
  if ((pGui == NULL) || (pXData == NULL)) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXGraphCreate";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return NULL;
  }
  gslc_tsElem     sElem;
  gslc_tsElemRef* pElemRef = NULL;
  sElem = gslc_ElemCreate(pGui,nElemId,nPage,GSLC_TYPEX_GRAPH,rElem,NULL,0,nFontId);
  sElem.nFeatures        |= GSLC_ELEM_FEA_FRAME_EN;
  sElem.nFeatures        |= GSLC_ELEM_FEA_FILL_EN;
  sElem.nFeatures        &= ~GSLC_ELEM_FEA_CLICK_EN;
  sElem.nFeatures        &= ~GSLC_ELEM_FEA_GLOW_EN;

  // Default group assignment. Can override later with ElemSetGroup()
  sElem.nGroup            = GSLC_GROUP_ID_NONE;

  // Define other extended data
  pXData->pBuf            = pBuf;
  pXData->nMargin         = 5;

  pXData->nBufMax         = nBufMax;
  pXData->nBufCnt         = 0;
  pXData->nPlotIndStart   = 0;

  pXData->colGraph        = colGraph;
  pXData->eStyle          = GSLCX_GRAPH_STYLE_DOT;

  // Define the visible region of the window
  // - The range in value can be overridden by the user
  pXData->nWndHeight = rElem.h - (2*pXData->nMargin);
  pXData->nWndWidth  = rElem.w - (2*pXData->nMargin);

  // Default scale is
  // - Each data point (buffer row) gets 1 pixel in X direction
  // - Data value is directly mapped to height in Y direction
  pXData->nPlotValMin   = 0;
  pXData->nPlotValMax   = pXData->nWndHeight;
  pXData->nPlotIndMax    = pXData->nWndWidth;


  // Clear the buffer
  memset(pBuf,0,nBufMax*sizeof(int16_t));


  // Determine if scrollbar should be enabled
  if (pXData->nPlotIndMax >= pXData->nBufMax) {
    // Disable scrollbar as the window is larger
    // than the number of rows in the buffer
    pXData->bScrollEn   = false;
    pXData->nScrollPos  = 0;
  } else {
    // Scrollbar is enabled
    pXData->bScrollEn   = true;
    pXData->nScrollPos  = pXData->nBufMax - pXData->nPlotIndMax;
  }

  sElem.pXData            = (void*)(pXData);

  // Specify the custom drawing callback
  sElem.pfuncXDraw        = &gslc_ElemXGraphDraw;
  sElem.pfuncXTouch       = NULL;
  sElem.colElemFill       = GSLC_COL_BLACK;
  sElem.colElemFillGlow   = GSLC_COL_BLACK;
  sElem.colElemFrame      = GSLC_COL_GRAY;
  sElem.colElemFrameGlow  = GSLC_COL_WHITE;
  if (nPage != GSLC_PAGE_NONE) {
    pElemRef = gslc_ElemAdd(pGui,nPage,&sElem,GSLC_ELEMREF_SRC_RAM);
    return pElemRef;
#if (GSLC_FEATURE_COMPOUND)
  } else {
    // Save as temporary element
    pGui->sElemTmp = sElem;
    pGui->sElemRefTmp.pElem = &(pGui->sElemTmp);
    pGui->sElemRefTmp.eElemFlags = GSLC_ELEMREF_SRC_RAM | GSLC_ELEMREF_REDRAW_FULL;
    return &(pGui->sElemRefTmp);
#endif
  }
  return NULL;
}


void gslc_ElemXGraphSetStyle(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,
        gslc_teXGraphStyle eStyle,uint8_t nMargin)
{
  gslc_tsXGraph*  pBox;
  gslc_tsElem*    pElem = gslc_GetElemFromRef(pGui,pElemRef);
  pBox = (gslc_tsXGraph*)(pElem->pXData);

  pBox->eStyle  = eStyle;
  pBox->nMargin = nMargin;

  // TODO: Recalculate the window extents and defaults
  // - Note that ElemXGraphSetStyle() was not intended to be
  //   called after the control is already in use/displayed

  // Set the redraw flag
  // - Force full redraw
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_FULL);

}

// TODO: Support scaling in X direction
void gslc_ElemXGraphSetRange(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,
        int16_t nYMin,int16_t nYMax)
{
  gslc_tsXGraph*    pBox;
  gslc_tsElem*      pElem = gslc_GetElemFromRef(pGui,pElemRef);
  pBox = (gslc_tsXGraph*)(pElem->pXData);

  pBox->nPlotValMax = nYMax;
  pBox->nPlotValMin = nYMin;

  // Set the redraw flag
  // - As we are changing the scale, force a full redraw
  //   since incremental redraws may make assumption about
  //   prior coordinate scaling
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_FULL);

}


void gslc_ElemXGraphScrollSet(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,uint8_t nScrollPos,uint8_t nScrollMax)
{

  gslc_tsXGraph*  pBox;
  gslc_tsElem*    pElem = gslc_GetElemFromRef(pGui,pElemRef);
  pBox = (gslc_tsXGraph*)(pElem->pXData);

  // Ensure scrollbar is enabled
  if (!pBox->bScrollEn) {
    // Scrollbar is disabled, so ignore
    return;
  }

  // Assign proportional value based on visible window region
  uint16_t nScrollPosOld = pBox->nScrollPos;
  pBox->nScrollPos = nScrollPos * (pBox->nBufMax - pBox->nPlotIndMax) / nScrollMax;

  // Set the redraw flag
  // - Only need incremental redraw
  // - Only redraw if changed actual scroll row
  if (pBox->nScrollPos != nScrollPosOld) {
    gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_INC);
  }
}


// Write a data value to the buffer
// - Advance the write ptr, wrap if needed
// - If encroach upon buffer read ptr, then drop the oldest line from the buffer
void gslc_ElemXGraphAdd(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,int16_t nVal)
{
  gslc_tsXGraph*  pBox = NULL;
  gslc_tsElem*    pElem = gslc_GetElemFromRef(pGui,pElemRef);
  pBox = (gslc_tsXGraph*)(pElem->pXData);

  // Add the data value
  pBox->pBuf[pBox->nBufCnt] = nVal;

  // Advance the pointer
  // - Wrap the pointers around end of buffer
  pBox->nBufCnt = (pBox->nBufCnt+1) % pBox->nBufMax;

  // Set the redraw flag
  // - Only need incremental redraw
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_INC);
}


bool gslc_ElemXGraphDraw(void* pvGui,void* pvElemRef,gslc_teRedrawType eRedraw)
{
  if ((pvGui == NULL) || (pvElemRef == NULL)) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXGraphDraw";
    GSLC_DEBUG_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return false;
  }
  // Typecast the parameters to match the GUI and element types
  gslc_tsGui*     pGui      = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef  = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem*    pElem     = gslc_GetElemFromRef(pGui,pElemRef);

  // Fetch the element's extended data structure
  gslc_tsXGraph* pBox;
  pBox = (gslc_tsXGraph*)(pElem->pXData);
  if (pBox == NULL) {
    GSLC_DEBUG_PRINT("ERROR: ElemXGraphDraw(%s) pXData is NULL\n","");
    return false;
  }

  bool     bGlow     = (pElem->nFeatures & GSLC_ELEM_FEA_GLOW_EN) && gslc_ElemGetGlow(pGui,pElemRef);
  bool     bFrameEn  = (pElem->nFeatures & GSLC_ELEM_FEA_FRAME_EN);

  // Draw the frame
  if (eRedraw == GSLC_REDRAW_FULL) {
    if (bFrameEn) {
      gslc_DrawFrameRect(pGui,pElem->rElem,pElem->colElemFrame);
    }
  }

  // Clear the background (inset from frame)
  // TODO: Support incremental redraw in which case we don't
  //       erase the inner region. Instead we would just erase
  //       old values and redraw new ones
  gslc_tsRect rInner = gslc_ExpandRect(pElem->rElem,-1,-1);
  gslc_DrawFillRect(pGui,rInner,(bGlow)?pElem->colElemFillGlow:pElem->colElemFill);

  int16_t           nDataVal;
  uint16_t          nCurX = 0;
  int16_t           nPixX,nPixY,nPixYBase,nPixYOffset;
  gslc_tsColor      colGraph;

  uint8_t           nScrollMax;

  // Initialize color state
  colGraph  = pBox->colGraph;

  // Calculate the current window position based on
  // the current buffer write pointer and scroll
  // position
  nScrollMax           = pBox->nBufMax - pBox->nPlotIndMax;
  pBox->nPlotIndStart  = pBox->nBufMax + pBox->nBufCnt;
  pBox->nPlotIndStart -= pBox->nPlotIndMax;
  // Only correct for scrollbar position if enabled
  if (pBox->bScrollEn) {
    pBox->nPlotIndStart -= (nScrollMax - pBox->nScrollPos);
  }
  pBox->nPlotIndStart  = pBox->nPlotIndStart % pBox->nBufMax;

  uint8_t nPlotInd = 0;
  uint8_t nIndMax = 0;
  nIndMax = (pBox->nBufMax < pBox->nPlotIndMax)? pBox->nBufMax : pBox->nPlotIndMax;
  for (nPlotInd=0;nPlotInd<nIndMax;nPlotInd++) {

    // Calculate row offset after accounting for buffer wrap
    // and current window starting offset
    uint16_t nBufInd = pBox->nPlotIndStart + nPlotInd;
    nBufInd = nBufInd % pBox->nBufMax;

    // NOTE: At the start of buffer fill when we have
    // only written a few values, we will continue to read
    // values out of the buffer so we are dependent upon
    // the reset to initialize the buffer to zero.

    nDataVal = pBox->pBuf[nBufInd];

    // Clip the value to the plot range
    if      (nDataVal > pBox->nPlotValMax) { nDataVal = pBox->nPlotValMax; }
    else if (nDataVal < pBox->nPlotValMin) { nDataVal = pBox->nPlotValMin; }

    // TODO: Make nCurX a scaled version of nPlotInd
    // - Support different modes for mapping multiple data points
    //   a single X coordinate and mapping a single data point
    //   to multiple X coordinates
    // - For now, just have a 1:1 correspondence between data
    //   points and the X coordinate
    nCurX = nPlotInd;

    // TODO: Scale data value

    // TODO: Consider supporting various color mapping modes
    //colGraph = gslc_ColorBlend2(GSLC_COL_BLACK,GSLC_COL_WHITE,500,nDataVal*500/200);

    // Determine the drawing coordinates
    nPixX       = pElem->rElem.x + pBox->nMargin + nCurX;
    nPixYBase   = pElem->rElem.y - pBox->nMargin + pElem->rElem.h-1;

    // Calculate Y coordinate
    nPixYOffset = nDataVal;

    // Clip plot Y coordinate
    if (nPixY > pBox->nWndHeight) { nPixY = pBox->nWndHeight; }
    if (nPixY < 0)                { nPixY = 0;                }

    // Calculate final Y coordinate
    nPixY       = pElem->rElem.y - pBox->nMargin + pElem->rElem.h-1 - nPixYOffset;


    // Render the datapoints
    if (pBox->eStyle == GSLCX_GRAPH_STYLE_DOT) {
      gslc_DrawSetPixel(pGui,nPixX,nPixY,colGraph);
    } else if (pBox->eStyle == GSLCX_GRAPH_STYLE_LINE) {
    } else if (pBox->eStyle == GSLCX_GRAPH_STYLE_FILL) {
      gslc_DrawLine(pGui,nPixX,nPixYBase,nPixX,nPixY,colGraph);
    }

  }

  // Clear the redraw flag
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_NONE);

  // Mark page as needing flip
  gslc_PageFlipSet(pGui,true);

  return true;
}

// ============================================================================
