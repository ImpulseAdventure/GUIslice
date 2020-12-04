// =======================================================================
// GUIslice library extension: Seekbar control
// - Paul Conti
// - Seekbar is a modern Slider Control with Android like style
// - Based on Calvin Hass's Slider control
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
/// \file XSeekbar.c



// GUIslice library
#include "GUIslice.h"
#include "GUIslice_drv.h"

#include "elem/XSeekbar.h"

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
// Extended Element: Seekbar
// - A linear slider control
// ============================================================================

// Create a slider element and add it to the GUI element list
// - Defines default styling for the element
// - Defines callback for redraw and touch
gslc_tsElemRef* gslc_ElemXSeekbarCreate(gslc_tsGui* pGui,int16_t nElemId,int16_t nPage,
  gslc_tsXSeekbar* pXData,gslc_tsRect rElem,int16_t nPosMin,int16_t nPosMax,int16_t nPos,
  uint8_t nProgressW,uint8_t nRemainW,uint8_t nThumbSz,
  gslc_tsColor colProgress,gslc_tsColor colRemain,gslc_tsColor colThumb,bool bVert)
{
  if ((pGui == NULL) || (pXData == NULL)) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXSeekbarCreate";
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return NULL;
  }
  gslc_tsElem     sElem;
  gslc_tsElemRef* pElemRef = NULL;
  sElem = gslc_ElemCreate(pGui,nElemId,nPage,GSLC_TYPEX_SEEKBAR,rElem,NULL,0,GSLC_FONT_NONE);
  sElem.nFeatures        &= ~GSLC_ELEM_FEA_FRAME_EN;
  sElem.nFeatures        |= GSLC_ELEM_FEA_FILL_EN;
  sElem.nFeatures        |= GSLC_ELEM_FEA_CLICK_EN;
  sElem.nFeatures        |= GSLC_ELEM_FEA_GLOW_EN;

  sElem.nGroup            = GSLC_GROUP_ID_NONE;

  // Range check on nPos
  if (nPos < nPosMin) { nPos = nPosMin; }
  if (nPos > nPosMax) { nPos = nPosMax; }

  pXData->bVert           = bVert;
  pXData->nPosMin         = nPosMin;
  pXData->nPosMax         = nPosMax;
  pXData->nPos            = nPos;
  pXData->nProgressW      = nProgressW;
  pXData->nRemainW        = nRemainW;
  pXData->nThumbSz        = nThumbSz;
  pXData->colProgress     = colProgress;
  pXData->colRemain       = colRemain;
  pXData->colThumb        = colThumb;
  pXData->bTrimThumb      = false;
  pXData->colTrim         = GSLC_COL_BLACK;
  pXData->bFrameThumb     = false;
  pXData->colFrame        = GSLC_COL_BLACK;
  pXData->nTickDiv        = 0;
  pXData->nTickLen        = 0;
  pXData->colTick         = GSLC_COL_BLACK;
  pXData->pfuncXPos       = NULL;
  sElem.pXData            = (void*)(pXData);
  // Specify the custom drawing callback
  sElem.pfuncXDraw        = &gslc_ElemXSeekbarDraw;
  // Specify the custom touch tracking callback
  sElem.pfuncXTouch       = &gslc_ElemXSeekbarTouch;

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

void gslc_ElemXSeekbarSetStyle(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,
        bool bTrimThumb,gslc_tsColor colTrim,bool bFrameThumb,gslc_tsColor colFrame,uint16_t nTickDiv,
        int16_t nTickLen,gslc_tsColor colTick)
{
  if (pElemRef == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXSeekbarSetStyle";
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return;
  }
  gslc_tsElem*    pElem = gslc_GetElemFromRef(pGui,pElemRef);
  gslc_tsXSeekbar* pSeekbar = (gslc_tsXSeekbar*)(pElem->pXData);

  pSeekbar->bTrimThumb  = bTrimThumb;
  pSeekbar->colTrim     = colTrim;
  pSeekbar->bFrameThumb = bFrameThumb;
  pSeekbar->colFrame    = colFrame;
  pSeekbar->nTickDiv    = nTickDiv;
  pSeekbar->nTickLen    = nTickLen;
  pSeekbar->colTick     = colTick;

  // Update
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_FULL);
}

int gslc_ElemXSeekbarGetPos(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef)
{
  if (pElemRef == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXSeekbarGetPos";
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return 0;
  }
  gslc_tsElem*    pElem = gslc_GetElemFromRef(pGui,pElemRef);
  gslc_tsXSeekbar* pSeekbar = (gslc_tsXSeekbar*)(pElem->pXData);

  return pSeekbar->nPos;
}

// Update the slider control's current state
void gslc_ElemXSeekbarSetPos(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,int16_t nPos)
{
  if ((pGui == NULL) || (pElemRef == NULL)) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXSeekbarSetPos";
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return;
  }
  gslc_tsElem*      pElem = gslc_GetElemFromRef(pGui,pElemRef);
  gslc_tsXSeekbar*   pSeekbar = (gslc_tsXSeekbar*)(pElem->pXData);
  int16_t           nPosOld;
  // Clip position
  if (nPos < pSeekbar->nPosMin) { nPos = pSeekbar->nPosMin; }
  if (nPos > pSeekbar->nPosMax) { nPos = pSeekbar->nPosMax; }
  // Update
  nPosOld = pSeekbar->nPos;
  pSeekbar->nPos = nPos;

  // Only update if changed
  if (nPos != nPosOld) {
    // If any position callback is defined, call it now
    if (pSeekbar->pfuncXPos != NULL) {
      (*pSeekbar->pfuncXPos)((void*)(pGui),(void*)(pElemRef),nPos);
    }

    // Mark for redraw
    // - Only need incremental redraw
    gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_INC);
  }

}


// Assign the position callback function for a slider
void gslc_ElemXSeekbarSetPosFunc(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,GSLC_CB_XSEEKBAR_POS funcCb)
{
  if ((pElemRef == NULL) || (funcCb == NULL)) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXSeekbarSetPosFunc";
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return;
  }
  gslc_tsElem*      pElem = gslc_GetElemFromRef(pGui,pElemRef);
  gslc_tsXSeekbar*   pSeekbar = (gslc_tsXSeekbar*)(pElem->pXData);
  pSeekbar->pfuncXPos = funcCb;
}


// Redraw the slider
// - Note that this redraw is for the entire element rect region
// - The Draw function parameters use void pointers to allow for
//   simpler callback function definition & scalability.
bool gslc_ElemXSeekbarDraw(void* pvGui,void* pvElemRef,gslc_teRedrawType eRedraw)
{
  (void)eRedraw; // Unused
  if ((pvGui == NULL) || (pvElemRef == NULL)) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXSeekbarDraw";
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return false;
  }
  // Typecast the parameters to match the GUI and element types
  gslc_tsGui*       pGui  = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef*   pElemRef = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem*      pElem = gslc_GetElemFromRef(pGui,pElemRef);

  // Fetch the element's extended data structure
  gslc_tsXSeekbar* pSeekbar;
  pSeekbar = (gslc_tsXSeekbar*)(pElem->pXData);
  if (pSeekbar == NULL) {
    GSLC_DEBUG2_PRINT("ERROR: ElemXSeekbarDraw(%s) pXData is NULL\n","");
    return false;
  }

  bool            bGlow       = (pElem->nFeatures & GSLC_ELEM_FEA_GLOW_EN) && gslc_ElemGetGlow(pGui,pElemRef);
  int16_t         nPos        = pSeekbar->nPos;
  int16_t         nPosMin     = pSeekbar->nPosMin;
  int16_t         nPosMax     = pSeekbar->nPosMax;
  uint8_t         nProgressW  = pSeekbar->nProgressW;
  uint8_t         nRemainW    = pSeekbar->nRemainW;
  uint8_t         nThumbSz    = pSeekbar->nThumbSz;
  bool            bVert       = pSeekbar->bVert;
  bool            bTrimThumb  = pSeekbar->bTrimThumb;
  bool            bFrameThumb = pSeekbar->bFrameThumb;
  uint16_t        nTickDiv    = pSeekbar->nTickDiv;
  int16_t         nTickLen    = pSeekbar->nTickLen;
  gslc_tsColor    colProgress = pSeekbar->colProgress;
  gslc_tsColor    colRemain   = pSeekbar->colRemain;
  gslc_tsColor    colThumb    = pSeekbar->colThumb;
  gslc_tsColor    colFrame    = pSeekbar->colFrame;
  gslc_tsColor    colTrim     = pSeekbar->colTrim;
  gslc_tsColor    colTick     = pSeekbar->colTick;

  // Range check on nPos
  if (nPos < nPosMin) { nPos = nPosMin; }
  if (nPos > nPosMax) { nPos = nPosMax; }

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
  // work out size of thumb circle
  int16_t  nLeftX   = rThumb.x+nThumbSz;
  int16_t  nLeftY   = rThumb.y + nThumbSz;
  
  // Draw the background
  // - TODO: To reduce flicker on unbuffered displays, one could consider
  //         redrawing only the thumb (last drawn position) with fill and
  //         then redraw other portions. This would prevent the
  //         track / ticks from flickering needlessly. A full redraw would
  //         be required if it was first draw action.
  gslc_DrawFillRect(pGui,pElem->rElem,(bGlow)?pElem->colElemFillGlow:pElem->colElemFill);
  
  // Draw the progress part of track
  if (!bVert) {
    gslc_tsRect rTrack = {
      nX0+nMargin,
      nYMid-(nProgressW/2),
      nCtrlPos,
      nProgressW
    };
    gslc_DrawFillRect(pGui,rTrack,colProgress);
  } else {
    gslc_tsRect rTrack = {
      nXMid-(nProgressW/2),
      nY0+nMargin,
      nProgressW,
      nCtrlPos
    };
    gslc_DrawFillRect(pGui,rTrack,colProgress);
  }

  // test for thumb trim color
  if (bTrimThumb) {
    // two color thumb
    gslc_DrawFillCircle(pGui,nLeftX,nLeftY,nThumbSz,colTrim);
    gslc_DrawFillCircle(pGui,nLeftX,nLeftY,3,colThumb);
  } else {
    // one solid color thumb
    gslc_DrawFillCircle(pGui,nLeftX,nLeftY,nThumbSz,colProgress);
  }
  if (bFrameThumb) {
    gslc_DrawFrameCircle(pGui,nLeftX,nLeftY,nThumbSz,colFrame);
  }
  
  // Draw the remaining part of track
  if (!bVert) {
    if (nRemainW == 1) {
      gslc_DrawLine(pGui,nX0+nMargin,nYMid,nX1-nMargin,nYMid,colRemain);
    } else {
      gslc_tsRect rRemain = {
        nX0+nMargin+nCtrlPos,
        nYMid-(nRemainW/2),
        nX1 - (nX0 + nCtrlPos+ nMargin*2) +1,
        nRemainW
      };
      gslc_DrawFillRect(pGui,rRemain,colRemain);
    }
  } else {
    if (nRemainW == 1) {
      gslc_DrawLine(pGui,nXMid,nY0+nMargin,nXMid,nY1-nMargin,colRemain);
    } else {
     gslc_tsRect rRemain = {
        nXMid-(nRemainW/2),
        nY0+nMargin+nCtrlPos,
        nRemainW,
        (nY1-nMargin)-(nY0+nMargin+nCtrlPos)
      };
      gslc_DrawFillRect(pGui,rRemain,colRemain);
    }
  }

  // Draw any ticks - we need to do this last or the ticks get over written
  // - Need at least one tick segment
  if (nTickDiv>=1) {
    uint16_t  nTickInd;
    int16_t   nTickOffset;
    for (nTickInd=0;nTickInd<=nTickDiv;nTickInd++) {
      nTickOffset = nTickInd * nCtrlRng / nTickDiv;
      if (!bVert) {
        gslc_DrawLine(pGui,nX0+nMargin+nTickOffset,nYMid-(nTickLen/2),
                nX0+nTickOffset+nMargin,nYMid+(nTickLen/2),colTick);
      } else {
        gslc_DrawLine(pGui,nXMid+(nTickLen/2),nY0+nTickOffset+nMargin,nXMid-(nTickLen/2),
                nY0+nTickOffset+nMargin,colTick);
      }
    }
  }

  // Clear the redraw flag
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_NONE);

  // Mark page as needing flip
  gslc_PageFlipSet(pGui,true);

  return true;
}


// This callback function is called by gslc_ElemSendEventTouch()
// after any touch event
bool gslc_ElemXSeekbarTouch(void* pvGui,void* pvElemRef,gslc_teTouch eTouch,int16_t nRelX,int16_t nRelY)
{
#if defined(DRV_TOUCH_NONE)
  return false;
#else

  if ((pvGui == NULL) || (pvElemRef == NULL)) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXSeekbarTouch";
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return false;
  }
  gslc_tsGui*           pGui = NULL;
  gslc_tsElemRef*       pElemRef = NULL;
  gslc_tsElem*          pElem = NULL;
  gslc_tsXSeekbar*       pSeekbar = NULL;

  // Typecast the parameters to match the GUI
  pGui      = (gslc_tsGui*)(pvGui);
  pElemRef  = (gslc_tsElemRef*)(pvElemRef);
  pElem     = gslc_GetElemFromRef(pGui,pElemRef);
  pSeekbar   = (gslc_tsXSeekbar*)(pElem->pXData);

  bool    bGlowingOld = gslc_ElemGetGlow(pGui,pElemRef);
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

    case GSLC_TOUCH_SET_REL:
    case GSLC_TOUCH_SET_ABS:
      bIndexed = true;
      gslc_ElemSetGlow(pGui,pElemRef,true);
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
        nPos = pSeekbar->nPos;
        nPos += nRelY;
        nPos = (nPos > pSeekbar->nPosMax)? pSeekbar->nPosMax : nPos;
        nPos = (nPos < pSeekbar->nPosMin)? pSeekbar->nPosMin : nPos;
      } else if (eTouch == GSLC_TOUCH_SET_ABS) {
        // Overload the "nRelY" parameter
        nPos = nRelY;
        nPos = (nPos > pSeekbar->nPosMax)? pSeekbar->nPosMax : nPos;
        nPos = (nPos < pSeekbar->nPosMin)? pSeekbar->nPosMin : nPos;
      }
    } else {
      // Perform additional range checking
      nRelX = (nRelX < 0)? 0 : nRelX;
      nRelY = (nRelY < 0)? 0 : nRelY;

      // Calc new position
      nPosRng = pSeekbar->nPosMax - pSeekbar->nPosMin;
      if (!pSeekbar->bVert) {
        nPos = (nRelX * nPosRng / pElem->rElem.w) + pSeekbar->nPosMin;
      } else {
        nPos = (nRelY * nPosRng / pElem->rElem.h) + pSeekbar->nPosMin;
      }
    }
    // Update the slider
    gslc_ElemXSeekbarSetPos(pGui,pElemRef,nPos);
  }

  // If the slider changed state, redraw
  if (gslc_ElemGetGlow(pGui,pElemRef) != bGlowingOld) {
    gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_FULL);
  }

  return true;
  #endif // !DRV_TOUCH_NONE
}

// ============================================================================
