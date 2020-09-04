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
/// \file XRadial.c



// GUIslice library
#include "GUIslice.h"
#include "GUIslice_drv.h"

#include "elem/XRadial.h"

#include <stdio.h>

#include <math.h>   // For sin/cos

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
// Extended Element: Radial Gauge
// - A circular gauge that can be used to show direction or other
//   rotational values. Tick marks can be optionally drawn
//   around the gauge.
// - Size, color and fill of the needle can be configured.
// ============================================================================

// Create a radial gauge element and add it to the GUI element list
// - Defines default styling for the element
// - Defines callback for redraw but does not track touch/click
gslc_tsElemRef* gslc_ElemXRadialCreate(gslc_tsGui* pGui,int16_t nElemId,int16_t nPage,
  gslc_tsXRadial* pXData,gslc_tsRect rElem,
  int16_t nMin,int16_t nMax,int16_t nVal,gslc_tsColor colGauge)
{
  if ((pGui == NULL) || (pXData == NULL)) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXRadialCreate";
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return NULL;
  }
  gslc_tsElem     sElem;
  gslc_tsElemRef* pElemRef = NULL;
  sElem = gslc_ElemCreate(pGui,nElemId,nPage,GSLC_TYPEX_RADIAL,rElem,NULL,0,GSLC_FONT_NONE);
  sElem.nFeatures        |= GSLC_ELEM_FEA_FRAME_EN;
  sElem.nFeatures        |= GSLC_ELEM_FEA_FILL_EN;
  sElem.nFeatures        &= ~GSLC_ELEM_FEA_CLICK_EN;  // Element is not "clickable"
  sElem.nFeatures        &= ~GSLC_ELEM_FEA_GLOW_EN;
  sElem.nGroup            = GSLC_GROUP_ID_NONE;
  pXData->nMin            = nMin;
  pXData->nMax            = nMax;
  pXData->nVal            = nVal;
  pXData->bFlip           = false;
  pXData->colGauge        = colGauge;
  pXData->colTick         = GSLC_COL_GRAY;
  pXData->nTickCnt        = 8;
  pXData->nTickLen        = 5;
  pXData->nIndicLen       = 10;     // Dummy default to be overridden
  pXData->nIndicTip       = 3;      // Dummy default to be overridden
  pXData->bIndicFill      = false;
  sElem.pXData            = (void*)(pXData);
  sElem.pfuncXDraw        = &gslc_ElemXRadialDraw;
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


void gslc_ElemXRadialSetIndicator(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,gslc_tsColor colGauge,
    uint16_t nIndicLen,uint16_t nIndicTip,bool bIndicFill)
{
  if (pElemRef == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXRadialSetIndicator";
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return;
  }
  gslc_tsXRadial*  pGauge  = (gslc_tsXRadial*)gslc_GetXDataFromRef(pGui,pElemRef,GSLC_TYPEX_RADIAL,__LINE__);

  // Update the config
  pGauge->colGauge    = colGauge;
  pGauge->nIndicLen   = nIndicLen;
  pGauge->nIndicTip   = nIndicTip;
  pGauge->bIndicFill  = bIndicFill;

  // Just in case we were called at runtime, mark as needing redraw
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_FULL);
}

void gslc_ElemXRadialSetTicks(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,gslc_tsColor colTick,uint16_t nTickCnt,uint16_t nTickLen)
{
  if (pElemRef == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXRadialSetTicks";
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return;
  }
  gslc_tsXRadial*  pGauge  = (gslc_tsXRadial*)gslc_GetXDataFromRef(pGui,pElemRef,GSLC_TYPEX_RADIAL,__LINE__);

  // Update the config
  pGauge->colTick   = colTick;
  pGauge->nTickCnt  = nTickCnt;
  pGauge->nTickLen  = nTickLen;

  // Just in case we were called at runtime, mark as needing redraw
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_FULL);
}

// Update the gauge control's current position
void gslc_ElemXRadialSetVal(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,int16_t nVal)
{
  if (pElemRef == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXRadialSetVal";
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return;
  }
  gslc_tsXRadial*  pGauge  = (gslc_tsXRadial*)gslc_GetXDataFromRef(pGui,pElemRef,GSLC_TYPEX_RADIAL,__LINE__);

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

// Update the gauge's rotation direction
// - Setting bFlip reverses the rotation direction
// - Default rotation is clockwise. When bFlip is set, uses counter-clockwise
void gslc_ElemXRadialSetFlip(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,bool bFlip)
{
  if (pElemRef == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXRadialSetFlip";
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return;
  }

  // Fetch the element's extended data structure
  gslc_tsXRadial*  pGauge  = (gslc_tsXRadial*)gslc_GetXDataFromRef(pGui,pElemRef,GSLC_TYPEX_RADIAL,__LINE__);
  if (pGauge == NULL) {
    GSLC_DEBUG2_PRINT("ERROR: gslc_ElemXRadialSetFlip(%s) pXData is NULL\n","");
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
bool gslc_ElemXRadialDraw(void* pvGui,void* pvElemRef,gslc_teRedrawType eRedraw)
{
  if ((pvGui == NULL) || (pvElemRef == NULL)) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXRadialDraw";
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return false;
  }

  // Typecast the parameters to match the GUI and element types
  gslc_tsGui*       pGui      = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef*   pElemRef  = (gslc_tsElemRef*)(pvElemRef);

  // Fetch the element's extended data structure
  gslc_tsXRadial*  pGauge  = (gslc_tsXRadial*)gslc_GetXDataFromRef(pGui,pElemRef,GSLC_TYPEX_RADIAL,__LINE__);

  gslc_ElemXRadialDrawRadial(pGui,pElemRef,eRedraw);

  // Save as "last state" to support incremental erase/redraw
  pGauge->nValLast      = pGauge->nVal;
  pGauge->bValLastValid = true;

  // Clear the redraw flag
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_NONE);

  return true;
}


void gslc_ElemXRadialDrawRadialHelp(gslc_tsGui* pGui,int16_t nX,int16_t nY,uint16_t nArrowLen,uint16_t nArrowSz,int16_t n64Ang,bool bFill,gslc_tsColor colFrame)
{
  int16_t   nTipX,nTipY;
  int16_t   nBaseX1,nBaseY1,nBaseX2,nBaseY2;
  int16_t   nTipBaseX,nTipBaseY;

  gslc_PolarToXY(nArrowLen,n64Ang,&nTipX,&nTipY);
  gslc_PolarToXY(nArrowLen-nArrowSz,n64Ang,&nTipBaseX,&nTipBaseY);
  gslc_PolarToXY(nArrowSz,n64Ang-90*64,&nBaseX1,&nBaseY1);
  gslc_PolarToXY(nArrowSz,n64Ang+90*64,&nBaseX2,&nBaseY2);

  // FIXME: There appears to be a wrapping bug in the trigonometry
  //        calculations associated with the bottom-right corner
  //        of the pointer body when angles approach 359 degrees.

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

bool gslc_ElemXRadialDrawRadial(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,gslc_teRedrawType eRedraw)
{
  gslc_tsElem*    pElem = gslc_GetElemFromRef(pGui,pElemRef);
  gslc_tsXRadial*  pGauge  = (gslc_tsXRadial*)gslc_GetXDataFromRef(pGui,pElemRef,GSLC_TYPEX_RADIAL,__LINE__);

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
    gslc_ElemXRadialDrawRadialHelp(pGui,nElemMidX,nElemMidY,nArrowLen,nArrowSize,n64AngLast,bFill,pElem->colElemFill);
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
  gslc_ElemXRadialDrawRadialHelp(pGui,nElemMidX,nElemMidY,nArrowLen,nArrowSize,n64Ang,bFill,pGauge->colGauge);

  return true;
}


// ============================================================================
