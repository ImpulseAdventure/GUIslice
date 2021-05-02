// =======================================================================
// GUIslice library extension: Toggle button control
// - Paul Conti
// - Toggle button with Android and iOS like styles
// - Based on Calvin Hass' Checkbox control
// - https://www.impulseadventure.com/elec/guislice-gui.html
// - https://github.com/ImpulseAdventure/GUIslice
// =======================================================================
//
// The MIT License
//
// Copyright 2016-2020 Calvin Hass and Paul Conti
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
/// \file XToggleImgbtn.c



// GUIslice library
#include "GUIslice.h"
#include "GUIslice_drv.h"

#include "elem/XToggleImgbtn.h"

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
// Extended Element: ToggleImgbtn
// - ToggleImgbtn 
//   Acts much like a checkbox but with styles that are similar to iOS and 
//   Android slider buttons.
// ============================================================================

// Create a togglebtn element and add it to the GUI element list
gslc_tsElemRef* gslc_ElemXToggleImgbtnCreate(gslc_tsGui* pGui,int16_t nElemId,int16_t nPage,
  gslc_tsXToggleImgbtn* pXData,gslc_tsRect rElem, gslc_tsImgRef sImgRef,gslc_tsImgRef sImgRefSel,
  bool bOn,GSLC_CB_TOUCH cbTouch)
{
  if ((pGui == NULL) || (pXData == NULL)) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXToggleImgbtnCreate";
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return NULL;
  }
  gslc_tsElem     sElem;
  gslc_tsElemRef* pElemRef = NULL;
  sElem = gslc_ElemCreate(pGui,nElemId,nPage,GSLC_TYPEX_TOGGLEIMGBTN,rElem,NULL,0,GSLC_FONT_NONE);
  sElem.nFeatures        &= ~GSLC_ELEM_FEA_FRAME_EN;
  sElem.nFeatures        |= GSLC_ELEM_FEA_FILL_EN;
  sElem.nFeatures        |= GSLC_ELEM_FEA_CLICK_EN;
  sElem.nFeatures        |= GSLC_ELEM_FEA_GLOW_EN;
  sElem.nFeatures        |= GSLC_ELEM_FEA_FOCUS_EN;
  sElem.nFeatures        |= GSLC_ELEM_FEA_NOSHRINK; // Can't shrink due to image

  // Define other extended data
  sElem.pXData            = (void*)(pXData);
  pXData->bOn             = bOn;    // save on/off status
  pXData->pfunctUser      = cbTouch;     // save user's callback in our extra data
  pXData->nMyPageId       = nPage;       // save our page id for group by access later, if needed.
  
  // Update the normal and glowing images
  gslc_DrvSetElemImageNorm(pGui,&sElem,sImgRef);
  gslc_DrvSetElemImageGlow(pGui,&sElem,sImgRefSel);

  // Specify the custom drawing callback
  sElem.pfuncXDraw        = &gslc_ElemXToggleImgbtnDraw;
  
  // Specify the custom touch handler
  sElem.pfuncXTouch       = &gslc_ElemXToggleImgbtnTouch;
  sElem.colElemFill       = GSLC_COL_BLACK;
  sElem.colElemFillGlow   = GSLC_COL_BLACK;
  sElem.colElemFrame      = GSLC_COL_BLACK;
  sElem.colElemFrameGlow  = GSLC_COL_YELLOW;
  
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

bool gslc_ElemXToggleImgbtnGetState(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef)
{
  gslc_tsXToggleImgbtn* pToggleImgbtn = (gslc_tsXToggleImgbtn*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_TOGGLEIMGBTN, __LINE__);
  if (!pToggleImgbtn) return false;

  return pToggleImgbtn->bOn;
}

// Helper routine for gslc_ElemXToggleImgbtnSetState()
// - Updates the togglebtn control's state but does
//   not touch any other controls in the group
void gslc_ElemXToggleImgbtnSetStateHelp(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,bool bOn)
{
  gslc_tsXToggleImgbtn* pToggleImgbtn = (gslc_tsXToggleImgbtn*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_TOGGLEIMGBTN, __LINE__);
  if (!pToggleImgbtn) return;

  // Update our data element
  bool  bStateOld = pToggleImgbtn->bOn;
  pToggleImgbtn->bOn = bOn;

  // Element needs redraw
  if (bOn != bStateOld) {
    // Only need an incremental redraw
    gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_INC);
  }

}

// Update the togglebtn control's state. If it's part of a group
// then also update the state of all other buttons in the group.
void gslc_ElemXToggleImgbtnSetState(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,bool bOn)
{
  gslc_tsXToggleImgbtn* pToggleImgbtn = (gslc_tsXToggleImgbtn*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_TOGGLEIMGBTN, __LINE__);
  if (!pToggleImgbtn) return;

  gslc_tsElem* pElem = gslc_GetElemFromRef(pGui,pElemRef);

  int16_t             nGroup    = pElem->nGroup;
  int16_t             nElemId   = pElem->nId;

  if (bOn && pElem->nGroup != GSLC_GROUP_ID_NONE) {

    // If we are selecting a button that is already
    // selected, then skip further update events.
    // NOTE: This check is not very efficient, but it avoids
    // the creation of extra events.
    gslc_tsElemRef* pTmpRef = gslc_ElemXToggleImgbtnFindSelected(pGui, nGroup);
    if (pTmpRef == pElemRef) {
      // Same element, so skip
      return;
    }

    // Proceed to deselect any other selected items in the group.
    // Note that SetState calls itself to deselect other items so it
    // is important to qualify this logic with bOn=true
    uint16_t          nCurInd;
    int16_t           nCurId;
    gslc_tsElem*      pCurElem = NULL;
    gslc_tsElemRef*   pCurElemRef = NULL;
    int16_t           nCurGroup;

    /* 
     * The elements must be grouped on the same layer but do not need to be on the current
     * page.  This allows us to place grouped elements on the base page.
     * p conti.
     */
    // Find our page layer
    gslc_tsPage* pPage = gslc_PageFindById(pGui, pToggleImgbtn->nMyPageId);
    if (pPage == NULL) {
      GSLC_DEBUG2_PRINT("ERROR: gslc_ElemXToggleImgbtnSetState() can't find page (ID=%d)\n", 
         pToggleImgbtn->nMyPageId);
      return;
    }

    gslc_tsCollect* pCollect = &pPage->sCollect;
    for (nCurInd=0;nCurInd<pCollect->nElemRefCnt;nCurInd++) {
      // Fetch extended data
      pCurElemRef   = &pCollect->asElemRef[nCurInd];
      pCurElem      = gslc_GetElemFromRef(pGui,pCurElemRef);

      // NOTE: Sorry but I have no idea what this FIXME is talking about - p conti
      // FIXME: Handle pCurElemRef->eElemFlags 
      nCurId        = pCurElem->nId;

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
      gslc_ElemSetGlow(pGui,pCurElemRef,false);  // trurn off glow state
      gslc_ElemXToggleImgbtnSetStateHelp(pGui,pCurElemRef,false);

    } // nInd

  } // bOn

  // Set the state of the current element
  gslc_ElemXToggleImgbtnSetStateHelp(pGui,pElemRef,bOn);
}

// Toggle the togglebtn control's state
void gslc_ElemXToggleImgbtnToggleState(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef)
{
  if (pElemRef == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXToggleImgbtnToggleState";
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return;
  }
  // Toggle the data element value
  bool bStateNew = (gslc_ElemXToggleImgbtnGetState(pGui,pElemRef))? false : true;
  gslc_ElemXToggleImgbtnSetState(pGui,pElemRef,bStateNew);
}

// Redraw the togglebtn
// - Note that this redraw is for the entire element rect region
// - The Draw function parameters use void pointers to allow for
//   simpler callback function definition & scalability.

bool gslc_ElemXToggleImgbtnDraw(void* pvGui,void* pvElemRef,gslc_teRedrawType eRedraw)
{
  (void)eRedraw; // Unused
  // Typecast the parameters to match the GUI and element types
  gslc_tsGui*       pGui  = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef*   pElemRef = (gslc_tsElemRef*)(pvElemRef);

  gslc_tsXToggleImgbtn* pToggleImgbtn = (gslc_tsXToggleImgbtn*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_TOGGLEIMGBTN, __LINE__);
  if (!pToggleImgbtn) {
    GSLC_DEBUG_PRINT("ERROR: gslc_ElemXToggleImgbtnDraw(%s) pXData is NULL\n","");
    return false;
  }

  gslc_tsElem* pElem = gslc_GetElemFromRef(pGui,pElemRef);
  bool bOk = false;

  // Determine the regions and colors based on element state
  gslc_tsRectState sState;
  gslc_ElemCalcRectState(pGui,pElemRef,&sState);

  // TODO:
  // - Could add support for drawing a frame (size=sState.rFull)

  // Draw any images associated with element
  int16_t nInnerX = sState.rInner.x;
  int16_t nInnerY = sState.rInner.y;
  if (pToggleImgbtn->bOn) {
    // Glow image might be NULL
    if (pElem->sImgRefGlow.eImgFlags != GSLC_IMGREF_NONE) {
      bOk = gslc_DrvDrawImage(pGui,nInnerX,nInnerY,pElem->sImgRefGlow);
    } else {
      bOk = gslc_DrvDrawImage(pGui,nInnerX,nInnerY,pElem->sImgRefNorm);
    }
  } else {
    bOk = gslc_DrvDrawImage(pGui,nInnerX,nInnerY,pElem->sImgRefNorm);
  }

  if (!bOk) {
    GSLC_DEBUG2_PRINT("ERROR: gslc_ElemXToggleImgbtnDraw failed\n","");
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
//   can still have a functional togglebtn, but doing the touch
//   tracking allows us to change the glow state of the element
//   dynamically, as well as updating the togglebtn state if the
//   user releases over it (ie. a click event).
//
bool gslc_ElemXToggleImgbtnTouch(void* pvGui,void* pvElemRef,gslc_teTouch eTouch,int16_t nRelX,int16_t nRelY)
{
#if defined(DRV_TOUCH_NONE)
  return false;
#else

  // Typecast the parameters to match the GUI and element types
  gslc_tsGui*       pGui  = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef*   pElemRef = (gslc_tsElemRef*)(pvElemRef);

  gslc_tsXToggleImgbtn* pToggleImgbtn = (gslc_tsXToggleImgbtn*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_TOGGLEIMGBTN, __LINE__);
  if (!pToggleImgbtn) return false;

  bool  bStateOld = pToggleImgbtn->bOn;
  
  switch(eTouch) {

    case GSLC_TOUCH_UP_IN:
      // Now that we released on element, update the state
      // ToggleImgbtn button action: toggle
      gslc_ElemXToggleImgbtnToggleState(pGui,pElemRef);
      break;
    default:
      return false;
      break;
  }

  // If the togglebtn changed state, redraw and notify user
  if (pToggleImgbtn->bOn != bStateOld) {
    // Now send the callback notification
    (*pToggleImgbtn->pfunctUser)((void*)(pGui), (void*)(pElemRef), eTouch, nRelX, nRelY);
    // Incremental redraw
    gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_INC);
  }

  return true;
  #endif // !DRV_TOUCH_NONE
}

// Determine which togglebtn in the group is selected "on"
gslc_tsElemRef* gslc_ElemXToggleImgbtnFindSelected(gslc_tsGui* pGui,int16_t nGroupId)
{
  uint16_t            nCurInd;
  gslc_tsElemRef*     pCurElemRef = NULL;
  gslc_tsElem*        pCurElem = NULL;
  int16_t             nCurType;
  int16_t             nCurGroup;
  bool                bCurSelected;
  gslc_tsElemRef*     pFoundElemRef = NULL;

  // Operate on current page
  // TODO: Support other page layers
  gslc_tsPage* pPage = pGui->apPageStack[GSLC_STACK_CUR];
  if (pPage == NULL) {
    return NULL; // No page added yet
  }
  
  if (pGui == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXToggleImgbtnFindChecked";
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return NULL;
  }

  gslc_tsCollect* pCollect = &pPage->sCollect;
  for (nCurInd=0;nCurInd<pCollect->nElemCnt;nCurInd++) {
    // Fetch extended data
    pCurElemRef   = &(pCollect->asElemRef[nCurInd]);
    pCurElem      = gslc_GetElemFromRef(pGui,pCurElemRef);
    nCurType      = pCurElem->nType;
    // Only want to proceed if it is a togglebtn
    if (nCurType != GSLC_TYPEX_TOGGLEIMGBTN) {
      continue;
    }

    nCurGroup     = pCurElem->nGroup;
    bCurSelected   = gslc_ElemXToggleImgbtnGetState(pGui,pCurElemRef);

    // If this is in a different group, ignore it
    if (nCurGroup != nGroupId) {
      continue;
    }

    // Did we find an element in the group that was checked?
    if (bCurSelected) {
      pFoundElemRef = pCurElemRef;
      break;
    }
  } // nCurInd
  return pFoundElemRef;
}

// ============================================================================
