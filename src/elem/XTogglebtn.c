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
/// \file XTogglebtn.c



// GUIslice library
#include "GUIslice.h"
#include "GUIslice_drv.h"

//#include "elem/XTogglebtn.h"
#include "XTogglebtn.h"

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
// Extended Element: Togglebtn
// - Togglebtn 
//   Acts much like a checkbox but with styles that are similar to iOS and 
//   Android slider buttons.
// ============================================================================

// Create a togglebtn element and add it to the GUI element list
gslc_tsElemRef* gslc_ElemXTogglebtnCreate(gslc_tsGui* pGui,int16_t nElemId,int16_t nPage,
  gslc_tsXTogglebtn* pXData,gslc_tsRect rElem,
  gslc_tsColor colThumb,gslc_tsColor colOnState,gslc_tsColor colOffState,
  bool bCircular,bool bChecked,GSLC_CB_TOUCH cbTouch)
{
  if ((pGui == NULL) || (pXData == NULL)) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXTogglebtnCreate";
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return NULL;
  }
  gslc_tsElem     sElem;
  gslc_tsElemRef* pElemRef = NULL;
  sElem = gslc_ElemCreate(pGui,nElemId,nPage,GSLC_TYPEX_TOGGLEBTN,rElem,NULL,0,GSLC_FONT_NONE);
  sElem.nFeatures        |= GSLC_ELEM_FEA_FRAME_EN;
  sElem.nFeatures        |= GSLC_ELEM_FEA_FILL_EN;
  sElem.nFeatures        |= GSLC_ELEM_FEA_CLICK_EN;
  sElem.nFeatures        |= GSLC_ELEM_FEA_GLOW_EN;
  sElem.nFeatures        |= GSLC_ELEM_FEA_FOCUS_EN;

  // Default group assignment. Can override later with ElemSetGroup()
  sElem.nGroup            = GSLC_GROUP_ID_NONE;

  // Define other extended data
  sElem.pXData            = (void*)(pXData);
  pXData->bOn             = bChecked;    // save on/off status
  pXData->pfunctUser      = cbTouch;     // save user's callback in our extra data
  pXData->nMyPageId       = nPage;       // save our page id for group by access later, if needed.
  pXData->colThumb        = colThumb;    // save thumb color
  pXData->colOnState      = colOnState;  // save on color
  pXData->colOffState     = colOffState; // save off color
  pXData->bCircular       = bCircular;   // save button style
  
  // Specify the custom drawing callback
  sElem.pfuncXDraw        = &gslc_ElemXTogglebtnDraw;
  
  // Specify the custom touch handler
  sElem.pfuncXTouch       = &gslc_ElemXTogglebtnTouch;
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

bool gslc_ElemXTogglebtnGetState(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef)
{
  gslc_tsXTogglebtn* pTogglebtn = (gslc_tsXTogglebtn*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_TOGGLEBTN, __LINE__);
  if (!pTogglebtn) return false;

  return pTogglebtn->bOn;
}

// Helper routine for gslc_ElemXTogglebtnSetState()
// - Updates the togglebtn control's state but does
//   not touch any other controls in the group
void gslc_ElemXTogglebtnSetStateHelp(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,bool bOn)
{
  gslc_tsXTogglebtn* pTogglebtn = (gslc_tsXTogglebtn*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_TOGGLEBTN, __LINE__);
  if (!pTogglebtn) return;

  // Update our data element
  bool  bStateOld = pTogglebtn->bOn;
  pTogglebtn->bOn = bOn;

  // Element needs redraw
  if (bOn != bStateOld) {
    // Only need an incremental redraw
    gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_INC);
  }

}

// Update the togglebtn control's state. If it's part of a group
// then also update the state of all other buttons in the group.
void gslc_ElemXTogglebtnSetState(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,bool bOn)
{
  gslc_tsXTogglebtn* pTogglebtn = (gslc_tsXTogglebtn*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_TOGGLEBTN, __LINE__);
  if (!pTogglebtn) return;

  gslc_tsElem* pElem = gslc_GetElemFromRef(pGui,pElemRef);

  int16_t             nGroup    = pElem->nGroup;
  int16_t             nElemId   = pElem->nId;

  if (bOn && pElem->nGroup != GSLC_GROUP_ID_NONE) {

    // If we are selecting a button that is already
    // selected, then skip further update events.
    // NOTE: This check is not very efficient, but it avoids
    // the creation of extra events.
    gslc_tsElemRef* pTmpRef = gslc_ElemXTogglebtnFindSelected(pGui, nGroup);
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
    gslc_tsPage* pPage = gslc_PageFindById(pGui, pTogglebtn->nMyPageId);
    if (pPage == NULL) {
      GSLC_DEBUG2_PRINT("ERROR: gslc_ElemXTogglebtnSetState() can't find page (ID=%d)\n", 
         pTogglebtn->nMyPageId);
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
      gslc_ElemXTogglebtnSetStateHelp(pGui,pCurElemRef,false);

    } // nInd

  } // bOn

  // Set the state of the current element
  gslc_ElemXTogglebtnSetStateHelp(pGui,pElemRef,bOn);
}

// Toggle the togglebtn control's state
void gslc_ElemXTogglebtnToggleState(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef)
{
  if (pElemRef == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXTogglebtnToggleState";
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return;
  }
  // Toggle the data element value
  bool bStateNew = (gslc_ElemXTogglebtnGetState(pGui,pElemRef))? false : true;
  gslc_ElemXTogglebtnSetState(pGui,pElemRef,bStateNew);
}

void gslc_ElemXTogglebtnDrawCircularHelp(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,gslc_tsXTogglebtn* pTogglebtn) 
{
  gslc_tsElem* pElem = gslc_GetElemFromRef(pGui,pElemRef);

  // frame enabled?
  bool bFrameEn  = pElem->nFeatures & GSLC_ELEM_FEA_FRAME_EN;

  // Determine the regions and colors based on element state
  gslc_tsRectState sState;
  gslc_ElemCalcRectState(pGui,pElemRef,&sState);

  // work out our circle positions
  uint16_t nRadius  = sState.rInner.h / 2;
  int16_t  nLeftX   = sState.rInner.x + nRadius;
  int16_t  nLeftY   = sState.rInner.y + nRadius;
  int16_t  nRightX  = sState.rInner.x + sState.rFull.w - nRadius -1;
  int16_t  nRightY  = sState.rInner.y + nRadius;
    
  if (pTogglebtn->bOn) {
    // draw our main body
    gslc_DrawFillRoundRect(pGui,sState.rInner,nRadius,pTogglebtn->colOnState);
    // place thumb on right-hand side
    gslc_DrawFillCircle(pGui,nRightX-1,nRightY,nRadius-1,pTogglebtn->colThumb);
    if (bFrameEn) {
      // NOTE: On HX8357 and ILI9341 with TFT_eSPI if we do rounded rect frame we get 
      // something that looks like two reversed parenthesis )button( around button 
      gslc_DrawFrameCircle(pGui,nRightX,nRightY,nRadius-1,sState.colFrm);
      //gslc_DrawFrameRoundRect(pGui,pElem->rElem,pElem->rElem.h,pElem->colElemFrame);
      gslc_DrawFrameRoundRect(pGui,sState.rInner,nRadius,sState.colFrm);
    }
  } else {
    // draw our main body
    gslc_DrawFillRoundRect(pGui,sState.rInner,nRadius,pTogglebtn->colOffState);
    // place thumb on left-hand side
    gslc_DrawFillCircle(pGui,nLeftX,nLeftY,nRadius-1,pTogglebtn->colThumb);
    if (bFrameEn) {
      gslc_DrawFrameCircle(pGui,nLeftX,nLeftY,nRadius-1,sState.colFrm);
      gslc_DrawFrameRoundRect(pGui,sState.rInner,nRadius,sState.colFrm);
    }
  }
}

void gslc_ElemXTogglebtnDrawRectangularHelp(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,gslc_tsXTogglebtn* pTogglebtn) 
{
  gslc_tsElem* pElem = gslc_GetElemFromRef(pGui,pElemRef);
  // frame enabled?
  bool bFrameEn  = pElem->nFeatures & GSLC_ELEM_FEA_FRAME_EN;

  // Determine the regions and colors based on element state
  gslc_tsRectState sState;
  gslc_ElemCalcRectState(pGui,pElemRef,&sState);

  // Draw a frame around the checkbox
  gslc_DrawFrameRect(pGui,sState.rFull,sState.colFrm);

  // Work out the sizes of the inner rectangles 
  gslc_tsRect rSquare = {
    sState.rFull.x,
    sState.rFull.y,
    sState.rFull.h, // force a square
    sState.rFull.h
  };

  if (pTogglebtn->bOn) {
    gslc_DrawFillRect(pGui,sState.rInner,pTogglebtn->colOnState);
    // place thumb on left-hand side
    gslc_DrawFillRect(pGui,rSquare,pTogglebtn->colThumb);
    if (bFrameEn) {
      gslc_DrawFrameRect(pGui,sState.rFull,sState.colFrm);
      gslc_DrawFrameRect(pGui,rSquare,sState.colFrm);
    }
  } else {
    gslc_DrawFillRect(pGui,sState.rInner,pTogglebtn->colOffState);
    // place thumb on right-hand side
    rSquare.x = sState.rInner.x + sState.rInner.w - sState.rInner.h - 1;
    gslc_DrawFillRect(pGui,rSquare,pTogglebtn->colThumb);
    if (bFrameEn) {
      gslc_DrawFrameRect(pGui,sState.rFull,sState.colFrm);
      gslc_DrawFrameRect(pGui,rSquare,sState.colFrm);
    }
  }

}

// Redraw the togglebtn
// - Note that this redraw is for the entire element rect region
// - The Draw function parameters use void pointers to allow for
//   simpler callback function definition & scalability.

bool gslc_ElemXTogglebtnDraw(void* pvGui,void* pvElemRef,gslc_teRedrawType eRedraw)
{
  (void)eRedraw; // Unused
  // Typecast the parameters to match the GUI and element types
  gslc_tsGui*       pGui  = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef*   pElemRef = (gslc_tsElemRef*)(pvElemRef);

  gslc_tsXTogglebtn* pTogglebtn = (gslc_tsXTogglebtn*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_TOGGLEBTN, __LINE__);
  if (!pTogglebtn) {
    GSLC_DEBUG2_PRINT("ERROR: gslc_ElemXTogglebtnDraw(%s) pXData is NULL\n","");
    return false;
  }

  //gslc_tsElem* pElem = gslc_GetElemFromRef(pGui,pElemRef);
  //gslc_DrawFillRect(pGui,pElem->rElem,pElem->colElemFill);
 
  if (pTogglebtn->bCircular) {
    gslc_ElemXTogglebtnDrawCircularHelp(pGui, pElemRef, pTogglebtn);
  } else {
    gslc_ElemXTogglebtnDrawRectangularHelp(pGui, pElemRef, pTogglebtn);
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
bool gslc_ElemXTogglebtnTouch(void* pvGui,void* pvElemRef,gslc_teTouch eTouch,int16_t nRelX,int16_t nRelY)
{
#if defined(DRV_TOUCH_NONE)
  return false;
#else

  // Typecast the parameters to match the GUI and element types
  gslc_tsGui*       pGui  = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef*   pElemRef = (gslc_tsElemRef*)(pvElemRef);

  gslc_tsXTogglebtn* pTogglebtn = (gslc_tsXTogglebtn*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_TOGGLEBTN, __LINE__);
  if (!pTogglebtn) return false;

  bool  bStateOld = pTogglebtn->bOn;
  
  switch(eTouch) {

    case GSLC_TOUCH_UP_IN:
      // Now that we released on element, update the state
      // Togglebtn button action: toggle
      gslc_ElemXTogglebtnToggleState(pGui,pElemRef);
      break;
    default:
      return false;
      break;
  }

  // If the togglebtn changed state, redraw and notify user
  if (pTogglebtn->bOn != bStateOld) {
    // Now send the callback notification
    (*pTogglebtn->pfunctUser)((void*)(pGui), (void*)(pElemRef), eTouch, nRelX, nRelY);
    // Incremental redraw
    gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_INC);
  }

  return true;
  #endif // !DRV_TOUCH_NONE
}

// Determine which togglebtn in the group is selected "on"
gslc_tsElemRef* gslc_ElemXTogglebtnFindSelected(gslc_tsGui* pGui,int16_t nGroupId)
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
    static const char GSLC_PMEM FUNCSTR[] = "ElemXTogglebtnFindChecked";
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
    if (nCurType != GSLC_TYPEX_TOGGLEBTN) {
      continue;
    }

    nCurGroup     = pCurElem->nGroup;
    bCurSelected   = gslc_ElemXTogglebtnGetState(pGui,pCurElemRef);

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
