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
/// \file XGauge.c

// *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
// WARNING: The XGauge element has been replaced by XProgress / XRadial / XRamp
//          Please update your code according to the migration notes in:
//          https://github.com/ImpulseAdventure/GUIslice/pull/157
//          XGauge may be removed in a future release.
// *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-


// GUIslice library
#include "GUIslice.h"
#include "GUIslice_drv.h"

#include "elem/XGauge.h"

#include <stdio.h>

#include <math.h>   // For sin/cos in XGauge(RADIAL)

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
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
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
  GSLC_DEBUG_PRINT("NOTE: XGauge has been replaced by XProgress/XRadial/XRamp\n","");
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


void gslc_ElemXGaugeSetStyle(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,gslc_teXGaugeStyle nStyle)
{
  if (pElemRef == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXGaugeSetStyle";
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
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
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
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
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
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
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
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
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return;
  }

  // Fetch the element's extended data structure
  gslc_tsElem*    pElem = gslc_GetElemFromRef(pGui,pElemRef);
  gslc_tsXGauge*  pGauge  = (gslc_tsXGauge*)(pElem->pXData);
  if (pGauge == NULL) {
    GSLC_DEBUG2_PRINT("ERROR: gslc_ElemXGaugeSetFlip(%s) pXData is NULL\n","");
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
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
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
    GSLC_DEBUG2_PRINT("ERROR: ElemXGaugeDraw(%s) pXData is NULL\n","");
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

  uint32_t nScl = 1;
  int16_t nGaugeMid = 0;
  int16_t nLen = 0;
  int16_t nTmp = 0;
  int32_t nTmpL = 0;

  if (nRng == 0) {
    GSLC_DEBUG2_PRINT("ERROR: ElemXGaugeDraw() Zero gauge range [%d,%d]\n",nMin,nMax);
    return false;
  }

  if (bVert) {
    nScl = nElemH*32768/nRng;
  } else {
    nScl = nElemW*32768/nRng;
  }

  // Calculate the control midpoint/zeropoint (for display purposes)
  nTmpL = -((int32_t)nMin * (int32_t)nScl / 32768);
  nGaugeMid = (int16_t)nTmpL;


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
  nGaugeY1 = (nGaugeY1 > nElemY1)? nElemY1 : nGaugeY1;

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

  // Draw a frame around the gauge
  // - Only draw this during full redraw
  if (eRedraw == GSLC_REDRAW_FULL) {
    gslc_DrawFrameRect(pGui, pElem->rElem, pElem->colElemFrame);
  }

  // To avoid flicker, we only erase the portion of the gauge
  // that isn't "filled". Determine the gauge empty region and erase it
  // There are two empty regions (one in negative and one in positive)
  int16_t nEmptyPos;
  if (bVert) {
    // Empty Region #1 (negative)
    nEmptyPos = (nGaugeY0 > nElemY1) ? nElemY1 : nGaugeY0;
    rEmpty = (gslc_tsRect){nElemX0,nElemY0,nElemX1-nElemX0+1,nEmptyPos-nElemY0+1};
    rTmp = gslc_ExpandRect(rEmpty,-1,-1);
    gslc_DrawFillRect(pGui,rTmp,pElem->colElemFill);
    // Empty Region #2 (positive)
    nEmptyPos = (nGaugeY1 < nElemY0) ? nElemY0 : nGaugeY1;
    rEmpty = (gslc_tsRect){nElemX0,nEmptyPos,nElemX1-nElemX0+1,nElemY1-nEmptyPos+1};
    rTmp = gslc_ExpandRect(rEmpty,-1,-1);
    gslc_DrawFillRect(pGui,rTmp,pElem->colElemFill);
  } else {
    // Empty Region #1 (negative)
    nEmptyPos = (nGaugeX0 > nElemX1) ? nElemX1 : nGaugeX0;
    rEmpty = (gslc_tsRect){nElemX0,nElemY0,nEmptyPos-nElemX0+1,nElemY1-nElemY0+1};
    rTmp = gslc_ExpandRect(rEmpty,-1,-1);
    gslc_DrawFillRect(pGui, rTmp, pElem->colElemFill);
    // Empty Region #2 (positive)
    nEmptyPos = (nGaugeX1 < nElemX0) ? nElemX0 : nGaugeX1;
    rEmpty = (gslc_tsRect){nEmptyPos,nElemY0,nElemX1-nEmptyPos+1,nElemY1-nElemY0+1};
    rTmp = gslc_ExpandRect(rEmpty,-1,-1);
    gslc_DrawFillRect(pGui, rTmp, pElem->colElemFill);
  }

  // Draw the gauge fill region
  rGauge = (gslc_tsRect){nGaugeX0,nGaugeY0,nGaugeX1-nGaugeX0+1,nGaugeY1-nGaugeY0+1};
  rTmp = gslc_ExpandRect(rGauge,-1,-1);
  gslc_DrawFillRect(pGui,rTmp,pGauge->colGauge);


  // Draw the midpoint line
  if (bVert) {
    if (nElemY0 + nGaugeMid < nElemY1) {
      gslc_DrawLine(pGui, nElemX0, nElemY0 + nGaugeMid, nElemX1, nElemY0 + nGaugeMid, pElem->colElemFrame);
    }
  } else {
    if (nElemX0 + nGaugeMid < nElemX1) {
      gslc_DrawLine(pGui, nElemX0 + nGaugeMid, nElemY0, nElemX0 + nGaugeMid, nElemY1, pElem->colElemFrame);
    }
  }



  return true;
}

#if (GSLC_FEATURE_XGAUGE_RADIAL)
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
    GSLC_DEBUG2_PRINT("ERROR: ElemXRadialDraw() Zero range [%d,%d]\n",nMin,nMax);
    return false;
  }

  // Support reversing of direction
  // TODO: Clean up excess integer typecasting
  if (pGauge->bFlip) {
    n64Ang      = (int32_t)(nMax - nVal    )* 360*64 /nRng;
    n64AngLast  = (int32_t)(nMax - nValLast)* 360*64 /nRng;
  } else {
    n64Ang      = (int32_t)(nVal     - nMin)* 360*64 /nRng;
    n64AngLast  = (int32_t)(nValLast - nMin)* 360*64 /nRng;
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
    GSLC_DEBUG2_PRINT("ERROR: gslc_ElemXGaugeDrawRamp() Zero range [%d,%d]\n",nMin,nMax);
    return false;
  }

  uint32_t  nSclFX;
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
#endif // GLSC_FEATURE_XGAUGE_RAMP


// ============================================================================
