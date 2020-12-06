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
/// \file XGraph.c



// GUIslice library
#include "GUIslice.h"
#include "GUIslice_drv.h"

#include "elem/XGraph.h"

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

gslc_tsElemRef* gslc_ElemXGraphCreate(gslc_tsGui* pGui,int16_t nElemId,int16_t nPage,
  gslc_tsXGraph* pXData,gslc_tsRect rElem,int16_t nFontId,int16_t* pBuf,
  uint16_t nBufMax,gslc_tsColor colGraph)
{
  if ((pGui == NULL) || (pXData == NULL)) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXGraphCreate";
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
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
  pXData->nPlotIndMax   = pXData->nWndWidth;


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
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
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
    GSLC_DEBUG2_PRINT("ERROR: ElemXGraphDraw(%s) pXData is NULL\n","");
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
  uint16_t          nPixX,nPixY,nPixYBase,nPixYOffset;
  gslc_tsColor      colGraph;

  uint16_t          nScrollMax;

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

  uint16_t nPlotInd = 0;
  uint16_t nIndMax = 0;
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

    // Calculate Y value
    nPixYOffset = (nDataVal >= 0)? nDataVal : 0;
    // Clip plot Y value
    if (nPixYOffset > pBox->nWndHeight) { nPixYOffset = pBox->nWndHeight; }

    // Calculate final Y coordinate
    nPixY       = nPixYBase - nPixYOffset;

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
