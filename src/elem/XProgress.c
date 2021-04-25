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
/// \file XProgress.c



// GUIslice library
#include "GUIslice.h"
#include "GUIslice_drv.h"

#include "elem/XProgress.h"

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
// Extended Element: Progress Bar
// - Basic progress bar with support for vertical / horizontal orientation and
//   fill direction. Also provides an indicator of negative regions, depending
//   on the configured range.
// ============================================================================

// Create a gauge element and add it to the GUI element list
// - Defines default styling for the element
// - Defines callback for redraw but does not track touch/click
gslc_tsElemRef* gslc_ElemXProgressCreate(gslc_tsGui* pGui,int16_t nElemId,int16_t nPage,
  gslc_tsXProgress* pXData,gslc_tsRect rElem,
  int16_t nMin,int16_t nMax,int16_t nVal,gslc_tsColor colGauge,bool bVert)
{
  if ((pGui == NULL) || (pXData == NULL)) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXProgressCreate";
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return NULL;
  }
  gslc_tsElem     sElem;
  gslc_tsElemRef* pElemRef = NULL;
  sElem = gslc_ElemCreate(pGui,nElemId,nPage,GSLC_TYPEX_PROGRESS,rElem,NULL,0,GSLC_FONT_NONE);
  sElem.nFeatures        |= GSLC_ELEM_FEA_FRAME_EN;
  sElem.nFeatures        |= GSLC_ELEM_FEA_FILL_EN;
  sElem.nFeatures        &= ~GSLC_ELEM_FEA_CLICK_EN;  // Element is not "clickable"
  sElem.nFeatures        &= ~GSLC_ELEM_FEA_GLOW_EN;
  sElem.nGroup            = GSLC_GROUP_ID_NONE;
  pXData->nMin            = nMin;
  pXData->nMax            = nMax;
  pXData->nVal            = nVal;
  pXData->bVert           = bVert;
  pXData->bFlip           = false;
  pXData->colGauge        = colGauge;
  sElem.pXData            = (void*)(pXData);
  sElem.pfuncXDraw        = &gslc_ElemXProgressDraw;
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
void gslc_ElemXProgressSetVal(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,int16_t nVal)
{
  if (pElemRef == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXProgressSetVal";
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return;
  }
  gslc_tsXProgress*  pGauge  = (gslc_tsXProgress*)gslc_GetXDataFromRef(pGui,pElemRef,GSLC_TYPEX_PROGRESS,__LINE__);

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
void gslc_ElemXProgressSetFlip(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,bool bFlip)
{
  if (pElemRef == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXProgressSetFlip";
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return;
  }

  // Fetch the element's extended data structure
  gslc_tsXProgress*  pGauge  = (gslc_tsXProgress*)gslc_GetXDataFromRef(pGui,pElemRef,GSLC_TYPEX_PROGRESS,__LINE__);

  pGauge->bFlip = bFlip;

  // Mark for redraw
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_FULL);

}

// Update the gauge's color
void gslc_ElemXProgressSetGaugeCol(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,gslc_tsColor colGauge)
{
  if (pElemRef == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXProgressSetGaugeCol";
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return;
  }

  // Fetch the element's extended data structure
  gslc_tsXProgress*  pGauge  = (gslc_tsXProgress*)gslc_GetXDataFromRef(pGui,pElemRef,GSLC_TYPEX_PROGRESS,__LINE__);

  pGauge->colGauge = colGauge;

  // Mark for redraw
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_INC);

}



// Redraw the gauge
// - Note that this redraw is for the entire element rect region
// - The Draw function parameters use void pointers to allow for
//   simpler callback function definition & scalability.
bool gslc_ElemXProgressDraw(void* pvGui,void* pvElemRef,gslc_teRedrawType eRedraw)
{
  if ((pvGui == NULL) || (pvElemRef == NULL)) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXProgressDraw";
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return false;
  }

  // Typecast the parameters to match the GUI and element types
  gslc_tsGui*       pGui      = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef*   pElemRef  = (gslc_tsElemRef*)(pvElemRef);

  // Fetch the element's extended data structure
  gslc_tsXProgress*  pGauge  = (gslc_tsXProgress*)gslc_GetXDataFromRef(pGui,pElemRef,GSLC_TYPEX_PROGRESS,__LINE__);

  gslc_ElemXProgressDrawHelp(pGui,pElemRef,eRedraw);

  // Save as "last state" to support incremental erase/redraw
  pGauge->nValLast      = pGauge->nVal;
  pGauge->bValLastValid = true;

  // Clear the redraw flag
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_NONE);

  return true;
}


bool gslc_ElemXProgressDrawHelp(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,gslc_teRedrawType eRedraw)
{
  gslc_tsElem*       pElem   = gslc_GetElemFromRef(pGui,pElemRef);
  gslc_tsXProgress*  pGauge  = (gslc_tsXProgress*)gslc_GetXDataFromRef(pGui,pElemRef,GSLC_TYPEX_PROGRESS,__LINE__);

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
    GSLC_DEBUG2_PRINT("ERROR: ElemXProgressDraw() Zero gauge range [%d,%d]\n",nMin,nMax);
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

// ============================================================================
