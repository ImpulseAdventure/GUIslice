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
/// \file XSlider.c



// GUIslice library
#include "GUIslice.h"
#include "GUIslice_drv.h"

#include "elem/XSlider.h"

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
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return NULL;
  }
  gslc_tsElem     sElem;
  gslc_tsElemRef* pElemRef = NULL;
  sElem = gslc_ElemCreate(pGui,nElemId,nPage,GSLC_TYPEX_SLIDER,rElem,NULL,0,GSLC_FONT_NONE);
  sElem.nFeatures        &= ~GSLC_ELEM_FEA_FRAME_EN;
  sElem.nFeatures        |= GSLC_ELEM_FEA_FILL_EN;
  sElem.nFeatures        |= GSLC_ELEM_FEA_CLICK_EN;
  sElem.nFeatures        |= GSLC_ELEM_FEA_GLOW_EN;
  sElem.nFeatures        |= GSLC_ELEM_FEA_EDIT_EN;
  sElem.nFeatures        |= GSLC_ELEM_FEA_FOCUS_EN;

  sElem.nGroup            = GSLC_GROUP_ID_NONE;

  // Range check on nPos
  if (nPos < nPosMin) { nPos = nPosMin; }
  if (nPos > nPosMax) { nPos = nPosMax; }

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

void gslc_ElemXSliderSetStyle(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,
        bool bTrim,gslc_tsColor colTrim,uint16_t nTickDiv,
        int16_t nTickLen,gslc_tsColor colTick)
{
  if (pElemRef == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXSliderSetStyle";
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
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

void gslc_ElemXSliderSetSnapEn(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,bool bSnapEn)
{
  if (pElemRef == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXSliderSetSnapEn";
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return;
  }
  gslc_tsElem*    pElem = gslc_GetElemFromRef(pGui,pElemRef);
  gslc_tsXSlider* pSlider = (gslc_tsXSlider*)(pElem->pXData);

  pSlider->bSnapEn = bSnapEn;
}


int gslc_ElemXSliderGetPos(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef)
{
  if (pElemRef == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXSliderGetPos";
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
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
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return;
  }
  gslc_tsElem*      pElem = gslc_GetElemFromRef(pGui,pElemRef);
  gslc_tsXSlider*   pSlider = (gslc_tsXSlider*)(pElem->pXData);
  int16_t           nPosOld;
  // Clip position
  if (nPos < pSlider->nPosMin) { nPos = pSlider->nPosMin; }
  else if (nPos > pSlider->nPosMax) { nPos = pSlider->nPosMax; }
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
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
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
  (void)eRedraw; // Unused
  if ((pvGui == NULL) || (pvElemRef == NULL)) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXSliderDraw";
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
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
    GSLC_DEBUG2_PRINT("ERROR: ElemXSliderDraw(%s) pXData is NULL\n","");
    return false;
  }

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

  // Determine the regions and colors based on element state
  gslc_tsRectState sState;
  gslc_ElemCalcRectState(pGui,pElemRef,&sState);

  // Range check on nPos
  if (nPos < nPosMin) { nPos = nPosMin; }
  else if (nPos > nPosMax) { nPos = nPosMax; }

  // We use an "inner" region for the drawing to allow
  // for the inclusion of a frame. (not expected to be used)

  int16_t nX0,nY0,nX1,nY1,nXMid,nYMid;
  nX0 = sState.rInner.x;
  nY0 = sState.rInner.y;
  nX1 = sState.rInner.x + sState.rInner.w - 1;
  nY1 = sState.rInner.y + sState.rInner.h - 1;
  nXMid = (nX0+nX1)/2;
  nYMid = (nY0+nY1)/2;

  // Scale the current position
  int16_t nPosRng = nPosMax-nPosMin;
  // TODO: Check for nPosRng=0, reversed min/max
  int16_t nPosOffset = nPos-nPosMin;

  // Provide some margin so thumb doesn't exceed control bounds
  // TODO: Handle nCtrlRng <= 0
  int16_t nMargin   = nThumbSz;
  int16_t nCtrlRng;
  if (!bVert) {
    nCtrlRng = (nX1-nMargin)-(nX0+nMargin);
  } else {
    nCtrlRng = (nY1-nMargin)-(nY0+nMargin);
  }

  int16_t nCtrlPos  = (int16_t)((int32_t)nPosOffset * (int32_t)nCtrlRng / (int32_t)nPosRng) + nMargin;

  // Draw the background
  // - TODO: To reduce flicker on unbuffered displays, one could consider
  //         redrawing only the thumb (last drawn position) with fill and
  //         then redraw other portions. This would prevent the
  //         track / ticks from flickering needlessly. A full redraw would
  //         be required if it was first draw action.

  // Note that we are using colBack instead of colInner since we
  // don't want to transition to a glow state
  gslc_DrawFillRect(pGui,sState.rFull,sState.colBack);

  // Draw any ticks
  // - Need at least one tick segment
  if (nTickDiv>=1) {
    uint16_t  nTickInd;
    int16_t   nTickOffset;
    for (nTickInd=0;nTickInd<=nTickDiv;++nTickInd) {
      nTickOffset = (int16_t)((int32_t)nTickInd * (int32_t)nCtrlRng / (int32_t)nTickDiv);
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
    gslc_DrawLine(pGui,nX0+nMargin,nYMid,nX1-nMargin,nYMid,sState.colFrm);
    // Optionally draw a trim line
    if (bTrim) {
      gslc_DrawLine(pGui,nX0+nMargin,nYMid+1,nX1-nMargin,nYMid+1,colTrim);
    }

  } else {
    // Make the track highlight during glow
    gslc_DrawLine(pGui,nXMid,nY0+nMargin,nXMid,nY1-nMargin,sState.colFrm);
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
  gslc_DrawFillRect(pGui,rThumb,sState.colInner);
  gslc_DrawFrameRect(pGui,rThumb,sState.colFrm);
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
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
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

  gslc_tsRect rElemInner = pElem->rElem;
  rElemInner = gslc_ExpandRect(rElemInner,-1,-1);

  bool    bGlowingOld = gslc_ElemGetGlow(pGui,pElemRef);
  bool    bEditingOld = gslc_ElemGetEdit(pGui,pElemRef);
  int16_t nPosRng;
  int16_t nPos = 0;
  bool    bUpdatePos = false;
  bool    bIndexed = false;

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

    case GSLC_TOUCH_FOCUS_SELECT:
      gslc_ElemSetEdit(pGui,pElemRef,!bEditingOld);
      break;

    case GSLC_TOUCH_SET_REL:
    case GSLC_TOUCH_SET_ABS:
      bIndexed = true;
      bUpdatePos = true;
      break;

    default:
      return false;
      break;
  }


  // If we need to update the slider position, calculate the value
  // and perform the update
  if (bUpdatePos) {

    if (bIndexed) {
      // The position is changed by direct control (eg. keyboard)
      // instead of touch coordinates

      // FIXME: Change the following to be either absolute or relative
      // value assignment instead of inc/dec. Then the user code can
      // define what the magnitude and direction should be.

      if (eTouch == GSLC_TOUCH_SET_REL) {
        // Overload the "nRelY" parameter
        nPos = pSlider->nPos;
        nPos += nRelY;
        nPos = (nPos > pSlider->nPosMax)? pSlider->nPosMax : nPos;
        nPos = (nPos < pSlider->nPosMin)? pSlider->nPosMin : nPos;
      } else if (eTouch == GSLC_TOUCH_SET_ABS) {
        // Overload the "nRelY" parameter
        nPos = nRelY;
        nPos = (nPos > pSlider->nPosMax)? pSlider->nPosMax : nPos;
        nPos = (nPos < pSlider->nPosMin)? pSlider->nPosMin : nPos;
      }
    } else {
      // Perform additional range checking
      nRelX = (nRelX < 0)? 0 : nRelX;
      nRelY = (nRelY < 0)? 0 : nRelY;

      // Only support touch snap functionality if enabled
      // and tick divisions have been set.
      bool bDoSnap = pSlider->bSnapEn && (pSlider->nTickDiv > 0);
      int32_t nTickSz = 0;

      // Calc new position
      nPosRng = pSlider->nPosMax - pSlider->nPosMin;
      if (!pSlider->bVert) {
        if (bDoSnap) {
          nTickSz = (int32_t)pElem->rElem.w / (int32_t)pSlider->nTickDiv;
          nRelX = nTickSz * (int32_t)( (nRelX + (nTickSz/2) ) / nTickSz ); 
        }
        nPos = (int16_t)((int32_t)nRelX * (int32_t)nPosRng / (int32_t)pElem->rElem.w) + pSlider->nPosMin;
      } else {
        if (bDoSnap) {
          nTickSz = (int32_t)pElem->rElem.h / (int32_t)pSlider->nTickDiv;
          nRelY = nTickSz * (int32_t)( (nRelY + (nTickSz/2) ) / nTickSz ); 
        }
        nPos = (int16_t)((int32_t)nRelY * (int32_t)nPosRng / (int32_t)pElem->rElem.h) + pSlider->nPosMin;
      }
    }
    // Update the slider
    gslc_ElemXSliderSetPos(pGui,pElemRef,nPos);
  }

  // If the slider changed state, redraw
  if ((gslc_ElemGetGlow(pGui,pElemRef) != bGlowingOld) ||
      (gslc_ElemGetEdit(pGui,pElemRef) != bEditingOld)) {
    gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_FULL);
  }

  return true;
  #endif // !DRV_TOUCH_NONE
}

// ============================================================================
