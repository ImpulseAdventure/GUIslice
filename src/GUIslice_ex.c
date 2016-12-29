// =======================================================================
// GUIslice library (extensions)
// - Calvin Hass
// - http://www.impulseadventure.com/elec/microsdl-sdl-gui.html
// =======================================================================
//
// The MIT License
//
// Copyright 2016 Calvin Hass
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


#include <stdio.h>

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
gslc_tsElem* gslc_ElemXGaugeCreate(gslc_tsGui* pGui,int16_t nElemId,int16_t nPage,
  gslc_tsXGauge* pXData,gslc_tsRect rElem,
  int16_t nMin,int16_t nMax,int16_t nVal,gslc_tsColor colGauge,bool bVert)
{
  if ((pGui == NULL) || (pXData == NULL)) {
    debug_print("ERROR: ElemXGaugeCreate(%s) called with NULL ptr\n","");
    return NULL;
  }    
  gslc_tsElem   sElem;
  gslc_tsElem*  pElem = NULL;
  sElem = gslc_ElemCreate(pGui,nElemId,nPage,GSLC_TYPEX_GAUGE,rElem,NULL,0,GSLC_FONT_NONE);
  sElem.bFrameEn          = true;
  sElem.bFillEn           = true;
  sElem.bClickEn          = false;          // Element is not "clickable"
  sElem.nGroup            = GSLC_GROUP_ID_NONE;  
  pXData->nGaugeMin       = nMin;
  pXData->nGaugeMax       = nMax;
  pXData->nGaugeVal       = nVal;
  pXData->bGaugeVert      = bVert;
  pXData->colGauge        = colGauge;
  sElem.pXData            = (void*)(pXData);
  sElem.pfuncXDraw        = &gslc_ElemXGaugeDraw;
  sElem.pfuncXTouch       = NULL;           // No need to track touches
  sElem.colElemFill       = GSLC_COL_BLACK;
  sElem.colElemFillGlow   = GSLC_COL_BLACK;
  sElem.colElemFrame      = GSLC_COL_GRAY;
  sElem.colElemFrameGlow  = GSLC_COL_GRAY;  
  if (nPage != GSLC_PAGE_NONE) {
    pElem = gslc_ElemAdd(pGui,nPage,&sElem);
    return pElem;
  } else {
    // Save as temporary element
    pGui->sElemTmp = sElem;
    return &(pGui->sElemTmp);     
  }
}

// Update the gauge control's current position
void gslc_ElemXGaugeUpdate(gslc_tsElem* pElem,int16_t nVal)
{
  if (pElem == NULL) {
    debug_print("ERROR: ElemXGaugeUpdate(%s) called with NULL ptr\n","");
    return;
  }   
  gslc_tsXGauge*  pGauge  = (gslc_tsXGauge*)(pElem->pXData);
  
  // Update the data element
  int16_t nValOld = pGauge->nGaugeVal;
  pGauge->nGaugeVal = nVal;
  
  // Element needs redraw
  if (nVal != nValOld) {
    gslc_ElemSetRedraw(pElem,true);
  }
  
}

// Redraw the gauge
// - Note that this redraw is for the entire element rect region
// - The Draw function parameters use void pointers to allow for
//   simpler callback function definition & scalability.
bool gslc_ElemXGaugeDraw(void* pvGui,void* pvElem)
{
  if ((pvGui == NULL) || (pvElem == NULL)) {
    debug_print("ERROR: ElemXGaugeDraw(%s) called with NULL ptr\n","");
    return false;
  }   
  
  // Typecast the parameters to match the GUI and element types
  gslc_tsGui*   pGui  = (gslc_tsGui*)(pvGui);
  gslc_tsElem*  pElem = (gslc_tsElem*)(pvElem);

  gslc_tsRect   rTmp;           // Temporary rect for drawing
  gslc_tsRect   rGauge;         // Filled portion of gauge
  gslc_tsRect   rEmpty;         // Empty portion of gauge
  int16_t       nElemX,nElemY;
  uint16_t      nElemW,nElemH;

  nElemX = pElem->rElem.x;
  nElemY = pElem->rElem.y;
  nElemW = pElem->rElem.w;
  nElemH = pElem->rElem.h;

  // TODO: Inset the region by one pixel to ensure we only
  //       redraw what is inside the framed region (to avoid flicker).
  
  rGauge.x = nElemX;
  rGauge.y = nElemY;
  
  // Fetch the element's extended data structure
  gslc_tsXGauge* pGauge;
  pGauge = (gslc_tsXGauge*)(pElem->pXData);
  if (pGauge == NULL) {
    debug_print("ERROR: ElemXGaugeDraw(%s) pXData is NULL\n","");
    return false;
  }
    
  bool    bVert = pGauge->bGaugeVert;
  int16_t nMax  = pGauge->nGaugeMax;
  int16_t nMin  = pGauge->nGaugeMin;
  int16_t nRng  = pGauge->nGaugeMax - pGauge->nGaugeMin;

  if (nRng == 0) {
    debug_print("ERROR: ElemXGaugeDraw() Zero gauge range [%d,%d]\n",nMin,nMax);
    return false;
  }
  // TODO: Change to fixed point
  float   fScl;
  if (bVert) {
    fScl = (float)nElemH/(float)nRng;
  } else {
    fScl = (float)nElemW/(float)nRng;
  }

  // Calculate the control midpoint (for display purposes)
  int16_t nGaugeMid;
  if ((nMin == 0) && (nMax > 0)) {
    nGaugeMid = 0;
  } else if ((nMin < 0) && (nMax > 0)) {
    nGaugeMid = -nMin*fScl;
  } else if ((nMin < 0) && (nMax == 0)) {
    nGaugeMid = -nMin*fScl;
  } else {
    debug_print("ERROR: ElemXGaugeDraw() Unsupported gauge range [%d,%d]\n",nMin,nMax);
    return false;
  }

  // Calculate the length of the bar
  int16_t nLen = pGauge->nGaugeVal * fScl;

  // Define the gauge's fill rectangle region
  // depending on the orientation (bVert) and whether
  // the current position is negative or positive.
  if (nLen >= 0) {
    if (bVert) {
      rGauge.h = nLen;
      rGauge.y = nElemY + nGaugeMid;
    } else {
      rGauge.w = nLen;
      rGauge.x = nElemX + nGaugeMid;
    }
  } else {
    if (bVert) {
      rGauge.y = nElemY + nGaugeMid+nLen;
      rGauge.h = -nLen;
    } else {
      rGauge.x = nElemX + nGaugeMid+nLen;
      rGauge.w = -nLen;
    }
  }
  if (bVert) {
    rGauge.w = nElemW;
    rGauge.x = nElemX;
  } else {
    rGauge.h = nElemH;
    rGauge.y = nElemY;
  }

  // Clip the region
  // TODO: Confirm that +/-1 adjustments are correct
  int16_t nRectClipX1 = rGauge.x;
  int16_t nRectClipX2 = rGauge.x+rGauge.w-1;
  int16_t nRectClipY1 = rGauge.y;
  int16_t nRectClipY2 = rGauge.y+rGauge.h-1;
  if (nRectClipX1 < nElemX)               { nRectClipX1 = nElemX;           }
  if (nRectClipX2 > nElemX+(int)nElemW-1) { nRectClipX2 = nElemX+nElemW-1;  }
  if (nRectClipY1 < nElemY)               { nRectClipY1 = nElemY;           }
  if (nRectClipY2 > nElemY+(int)nElemH-1) { nRectClipY2 = nElemY+nElemH-1;  }
  rGauge.x = nRectClipX1;
  rGauge.y = nRectClipY1;
  rGauge.w = nRectClipX2-nRectClipX1+1;
  rGauge.h = nRectClipY2-nRectClipY1+1;

  #ifdef DBG_LOG
  printf("Gauge: nMin=%4d nMax=%4d nRng=%d nVal=%4d fScl=%6.3f nGaugeMid=%4d RectX=%4d RectW=%4d\n",
    nMin,nMax,nRng,pGauge->nGaugeVal,fScl,nGaugeMid,rGauge.x,rGauge.w);
  #endif

  // To avoid flicker, we only erase the portion of the gauge
  // that isn't "filled". Determine the gauge empty region and erase it
  if (bVert) {
    rEmpty.y = (rGauge.y + rGauge.h);
    rEmpty.x = nElemX;
    rEmpty.h = (nElemH - rGauge.h);
    rEmpty.w = nElemW;
  } else {
    rEmpty.x = (rGauge.x + rGauge.w);
    rEmpty.y = nElemY;
    rEmpty.w = (nElemW - rGauge.w);
    rEmpty.h = nElemH;
  }
  rTmp = gslc_ExpandRect(rEmpty,-1,-1);    
  gslc_DrawFillRect(pGui,rTmp,pElem->colElemFill);  
  
  // Draw the gauge fill region
  rTmp = gslc_ExpandRect(rGauge,-1,-1);
  gslc_DrawFillRect(pGui,rTmp,pGauge->colGauge);
  

  // Draw the midpoint line
  gslc_tsRect   rMidLine;
  if (bVert) {
    rMidLine.x = nElemX;
    rMidLine.y = nElemY+nGaugeMid;
    rMidLine.w = nElemW;
    rMidLine.h = 1;
  } else {
    rMidLine.x = nElemX+nGaugeMid;
    rMidLine.y = nElemY;
    rMidLine.w = 1;
    rMidLine.h = nElemH;
  }
  
  // Paint the filled progress region
  gslc_DrawFillRect(pGui,rMidLine,pElem->colElemFrame);

  // Draw a frame around the gauge
  gslc_DrawFrameRect(pGui,pElem->rElem,pElem->colElemFrame);
  
  // Clear the redraw flag
  gslc_ElemSetRedraw(pElem,false);
  
  return true;
}



// ============================================================================
// Extended Element: Checkbox
// - Checkbox with custom handler for touch tracking which
//   enables glow to be defined whenever touch is tracked over
//   the element.
// ============================================================================

// Create a checkbox element and add it to the GUI element list
// - Defines default styling for the element
// - Defines callback for redraw but does not track touch/click
gslc_tsElem* gslc_ElemXCheckboxCreate(gslc_tsGui* pGui,int16_t nElemId,int16_t nPage,
  gslc_tsXCheckbox* pXData,gslc_tsRect rElem,bool bRadio,gslc_teXCheckboxStyle nStyle,
  gslc_tsColor colCheck,bool bChecked)
{
  if ((pGui == NULL) || (pXData == NULL)) {
    debug_print("ERROR: ElemXCheckboxCreate(%s) called with NULL ptr\n","");
    return NULL;
  }      
  gslc_tsElem   sElem;
  gslc_tsElem*  pElem = NULL;
  sElem = gslc_ElemCreate(pGui,nElemId,nPage,GSLC_TYPEX_CHECKBOX,rElem,NULL,0,GSLC_FONT_NONE);
  sElem.bFrameEn          = false;
  sElem.bFillEn           = true;
  sElem.bClickEn          = true;
  sElem.bGlowEn           = true;
  sElem.bGlowing          = false;
  // Default group assignment. Can override later with ElemSetGroup()
  sElem.nGroup            = GSLC_GROUP_ID_NONE;
  // Save a pointer to the GUI in the extended element data
  // - We do this so that we can later perform actions on
  //   other elements (ie. when clicking radio button, others
  //   are deselected).
  // TODO: Replace this by using reference to pCollect
  pXData->pGui            = pGui;
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
    pElem = gslc_ElemAdd(pGui,nPage,&sElem);
    return pElem;
  } else {
    // Save as temporary element
    pGui->sElemTmp = sElem;
    return &(pGui->sElemTmp);     
  }
}

bool gslc_ElemXCheckboxGetState(gslc_tsElem* pElem)
{
  gslc_tsXCheckbox*  pCheckbox = (gslc_tsXCheckbox*)(pElem->pXData);
  return pCheckbox->bChecked;
}


// Determine which checkbox in the group has been "checked"
gslc_tsElem* gslc_ElemXCheckboxFindChecked(gslc_tsGui* pGui,int16_t nGroupId)
{
  int16_t             nCurInd;
  gslc_tsElem*        pCurElem = NULL;
  int8_t              nCurType;
  int16_t             nCurGroup;
  bool                bCurChecked;
  gslc_tsElem*        pFoundElem = NULL;

  if (pGui == NULL) {
    debug_print("ERROR: ElemXCheckboxFindChecked(%s) called with NULL ptr\n","");
    return NULL;
  }   
  for (nCurInd=GSLC_IND_FIRST;nCurInd<pGui->pCurPageCollect->nElemCnt;nCurInd++) {
    // Fetch extended data
    pCurElem      = &pGui->pCurPageCollect->asElem[nCurInd];
    nCurType      = pCurElem->nType;
    // Only want to proceed if it is a checkbox
    if (nCurType != GSLC_TYPEX_CHECKBOX) {
      continue;
    }

    nCurGroup     = pCurElem->nGroup;
    bCurChecked   = gslc_ElemXCheckboxGetState(pCurElem);
   
    // If this is in a different group, ignore it
    if (nCurGroup != nGroupId) {
      continue;
    }

    // Did we find an element in the group that was checked?
    if (bCurChecked) {
      pFoundElem = pCurElem;
      break;
    }
  } // nCurInd
  return pFoundElem;
}



// Update the checkbox control's current state
void gslc_ElemXCheckboxSetState(gslc_tsElem* pElem,bool bChecked)
{
  if (pElem == NULL) {
    debug_print("ERROR: ElemXCheckboxSetState(%s) called with NULL ptr\n","");
    return;
  }   
  gslc_tsXCheckbox*   pCheckbox = (gslc_tsXCheckbox*)(pElem->pXData);
  gslc_tsGui*         pGui      = pCheckbox->pGui;
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
    int16_t           nCurType;
    int16_t           nCurGroup;
    
    // We use the GUI pointer for access to other elements   
    for (nCurInd=GSLC_IND_FIRST;nCurInd<pGui->pCurPageCollect->nElemCnt;nCurInd++) {
      // Fetch extended data
      pCurElem      = &pGui->pCurPageCollect->asElem[nCurInd];
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
      gslc_ElemXCheckboxSetState(pCurElem,false);        

    } // nInd
      
  } // bRadio

  
  // Update our data element
  bool  bCheckedOld = pCheckbox->bChecked;
  pCheckbox->bChecked = bChecked;
  
  // Element needs redraw
  if (bChecked != bCheckedOld) {
    gslc_ElemSetRedraw(pElem,true);
  }

}

// Toggle the checkbox control's state
// TODO: Consider adding pGui to param so that we can
//       pass global context down to ElemXCheckboxSetState()
//       which would enable cross-element access (via group).
//       Current code is saving pGui in pXData for checkbox
//       which is undesirable.
void gslc_ElemXCheckboxToggleState(gslc_tsElem* pElem)
{  
  if (pElem == NULL) {
    debug_print("ERROR: ElemXCheckboxToggleState(%s) called with NULL ptr\n","");
    return;
  }     
  // Update the data element
  bool bCheckNew = (gslc_ElemXCheckboxGetState(pElem))? false : true;
  gslc_ElemXCheckboxSetState(pElem,bCheckNew);

  // Element needs redraw
  gslc_ElemSetRedraw(pElem,true);
  
}

// Redraw the checkbox
// - Note that this redraw is for the entire element rect region
// - The Draw function parameters use void pointers to allow for
//   simpler callback function definition & scalability.
bool gslc_ElemXCheckboxDraw(void* pvGui,void* pvElem)
{
  if ((pvGui == NULL) || (pvElem == NULL)) {
    debug_print("ERROR: ElemXCheckboxDraw(%s) called with NULL ptr\n","");
    return false;
  }   
  // Typecast the parameters to match the GUI and element types
  gslc_tsGui*   pGui  = (gslc_tsGui*)(pvGui);
  gslc_tsElem*  pElem = (gslc_tsElem*)(pvElem);
  
  gslc_tsRect   rInner;
  
  // Fetch the element's extended data structure
  gslc_tsXCheckbox* pCheckbox;
  pCheckbox = (gslc_tsXCheckbox*)(pElem->pXData);
  if (pCheckbox == NULL) {
    debug_print("ERROR: ElemXCheckboxDraw(%s) pXData is NULL\n","");
    return false;
  }
  
  bool                    bChecked  = pCheckbox->bChecked;
  gslc_teXCheckboxStyle   nStyle    = pCheckbox->nStyle;
  bool                    bGlow     = pElem->bGlowEn && pElem->bGlowing; 
  
  // Draw the background
  gslc_DrawFillRect(pGui,pElem->rElem,(bGlow)?pElem->colElemFillGlow:pElem->colElemFill);

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
      // TODO: A FillCircle() may look better here
      gslc_DrawFrameCircle(pGui,nMidX,nMidY,5,pCheckbox->colCheck);    
    }
    // Draw a frame around the checkbox     
    gslc_DrawFrameCircle(pGui,nMidX,nMidY,(pElem->rElem.w/2),(bGlow)?pElem->colElemFrameGlow:pElem->colElemFrame); 
    
  }
  
  // Clear the redraw flag
  gslc_ElemSetRedraw(pElem,false);
  
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
bool gslc_ElemXCheckboxTouch(void* pvGui,void* pvElem,gslc_teTouch eTouch,int16_t nRelX,int16_t nRelY)
{
  if ((pvGui == NULL) || (pvElem == NULL)) {
    debug_print("ERROR: ElemXCheckboxTouch(%s) called with NULL ptr\n","");
    return false;
  }       
  //gslc_tsGui*         pGui = NULL;
  gslc_tsElem*          pElem = NULL;
  gslc_tsXCheckbox*     pCheckbox = NULL;
  
  // Typecast the parameters to match the GUI
  //pGui  = (gslc_tsGui*)(pvGui);
  pElem = (gslc_tsElem*)(pvElem);
  pCheckbox = (gslc_tsXCheckbox*)(pElem->pXData);  

  bool  bRadio      = pCheckbox->bRadio;
  bool  bCheckedOld = pCheckbox->bChecked;
  bool  bGlowingOld = pElem->bGlowing;  
  
  switch(eTouch) {
    
    case GSLC_TOUCH_DOWN_IN:
      pElem->bGlowing = true;
      break;
      
    case GSLC_TOUCH_MOVE_IN:
      pElem->bGlowing = true;
      break;
    case GSLC_TOUCH_MOVE_OUT:
      pElem->bGlowing = false;
      break;

    case GSLC_TOUCH_UP_IN:
      pElem->bGlowing = false;
      // Now that we released on element, update the state
      bool  bCheckNew;
      if (bRadio) {
        // Radio button action: set
        bCheckNew = true;
      } else {
        // Checkbox button action: toggle
        bCheckNew = (pCheckbox->bChecked)?false:true;
      }
      gslc_ElemXCheckboxSetState(pElem,bCheckNew);
      break;
    case GSLC_TOUCH_UP_OUT:
      pElem->bGlowing = false;
      break;
      
    default:
      return false;
      break;
  }
  
  // If the checkbox changed state, redraw
  bool  bChanged = false;
  if (pElem->bGlowing != bGlowingOld) { bChanged = true; }
  if (pCheckbox->bChecked != bCheckedOld) { bChanged = true; }
  if (bChanged) {
    gslc_ElemSetRedraw(pElem,true);
  }
  
  return true;

}

// ============================================================================
// Extended Element: Slider
// - A linear slider control
// ============================================================================

// Create a slider element and add it to the GUI element list
// - Defines default styling for the element
// - Defines callback for redraw and touch
gslc_tsElem* gslc_ElemXSliderCreate(gslc_tsGui* pGui,int16_t nElemId,int16_t nPage,
  gslc_tsXSlider* pXData,gslc_tsRect rElem,int16_t nPosMin,int16_t nPosMax,int16_t nPos,
  uint16_t nThumbSz,bool bVert)
{
  if ((pGui == NULL) || (pXData == NULL)) {
    debug_print("ERROR: ElemXSliderCreate(%s) called with NULL ptr\n","");
    return NULL;
  }     
  gslc_tsElem   sElem;
  gslc_tsElem*  pElem = NULL;
  sElem = gslc_ElemCreate(pGui,nElemId,nPage,GSLC_TYPEX_SLIDER,rElem,NULL,0,GSLC_FONT_NONE);
  sElem.bFrameEn          = false;
  sElem.bFillEn           = true;
  sElem.bClickEn          = true;
  sElem.bGlowEn           = true;  
  sElem.bGlowing          = false;
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
    pElem = gslc_ElemAdd(pGui,nPage,&sElem);
    return pElem;
  } else {
    // Save as temporary element
    pGui->sElemTmp = sElem;
    return &(pGui->sElemTmp);       
  }
}

void gslc_ElemXSliderSetStyle(gslc_tsElem* pElem,
        bool bTrim,gslc_tsColor colTrim,uint16_t nTickDiv,
        int16_t nTickLen,gslc_tsColor colTick)
{
  if (pElem == NULL) {
    debug_print("ERROR: ElemXSliderSetStyle(%s) called with NULL ptr\n","");
    return;
  }   
  gslc_tsXSlider*  pSlider = (gslc_tsXSlider*)(pElem->pXData);

  pSlider->bTrim      = bTrim;
  pSlider->colTrim    = colTrim;
  pSlider->nTickDiv   = nTickDiv;
  pSlider->nTickLen   = nTickLen;
  pSlider->colTick    = colTick;
  
  // Update
  gslc_ElemSetRedraw(pElem,true);
}

int gslc_ElemXSliderGetPos(gslc_tsElem* pElem)
{
  if (pElem == NULL) {
    debug_print("ERROR: ElemXSliderGetPos(%s) called with NULL ptr\n","");
    return 0;
  }     
  gslc_tsXSlider*  pSlider = (gslc_tsXSlider*)(pElem->pXData);
  
  return pSlider->nPos;
}

// Update the slider control's current state
void gslc_ElemXSliderSetPos(gslc_tsGui* pGui,gslc_tsElem* pElem,int16_t nPos)
{
  if ((pGui == NULL) || (pElem == NULL)) {
    debug_print("ERROR: ElemXSliderSetPos(%s) called with NULL ptr\n","");
    return;
  }       
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
      (*pSlider->pfuncXPos)((void*)(pGui),(void*)(pElem),nPos);
    }  
    
    // Mark for redraw
    gslc_ElemSetRedraw(pElem,true);
  }
  
}


///
/// Assign the position callback function for a slider
///
/// \param[in]  pElem:       Pointer to element
/// \param[in]  funcCb:      Function pointer to position routine (or NULL for none)
///
/// \return none
///
void gslc_ElemXSliderSetPosFunc(gslc_tsElem* pElem,GSLC_CB_XSLIDER_POS funcCb)
{
  if ((pElem == NULL) || (funcCb == NULL)) {
    debug_print("ERROR: ElemXSliderSetPosFunc(%s) called with NULL ptr\n","");
    return;
  }
  gslc_tsXSlider*   pSlider = (gslc_tsXSlider*)(pElem->pXData);
  pSlider->pfuncXPos = funcCb;
}


// Redraw the slider
// - Note that this redraw is for the entire element rect region
// - The Draw function parameters use void pointers to allow for
//   simpler callback function definition & scalability.
bool gslc_ElemXSliderDraw(void* pvGui,void* pvElem)
{
  if ((pvGui == NULL) || (pvElem == NULL)) {
    debug_print("ERROR: ElemXSliderDraw(%s) called with NULL ptr\n","");
    return false;
  }     
  // Typecast the parameters to match the GUI and element types
  gslc_tsGui*   pGui  = (gslc_tsGui*)(pvGui);
  gslc_tsElem*  pElem = (gslc_tsElem*)(pvElem);
  
  // Fetch the element's extended data structure
  gslc_tsXSlider* pSlider;
  pSlider = (gslc_tsXSlider*)(pElem->pXData);
  if (pSlider == NULL) {
    debug_print("ERROR: ElemXSliderDraw(%s) pXData is NULL\n","");
    return false;
  }
  
  bool            bGlow     = pElem->bGlowEn && pElem->bGlowing; 
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
  
  if (bVert) {
    debug_print("ERROR: ElemXSliderDraw(%s) bVert=true not supported yet\n","");
    return false;
  }
  
  int16_t nX0,nY0,nX1,nY1,nYMid;
  nX0 = pElem->rElem.x;
  nY0 = pElem->rElem.y;
  nX1 = pElem->rElem.x + pElem->rElem.w - 1;
  nY1 = pElem->rElem.y + pElem->rElem.h - 1;
  nYMid = (nY0+nY1)/2;
  
  // Scale the current position
  int16_t nPosRng = nPosMax-nPosMin;
  // TODO: Check for nPosRng=0, reversed min/max
  int16_t nPosOffset = nPos-nPosMin;
  
  // Provide some margin so thumb doesn't exceed control bounds
  int16_t nMargin   = nThumbSz;
  int16_t nCtrlRng  = (nX1-nMargin)-(nX0+nMargin);
  int16_t nCtrlPos  = (nPosOffset*nCtrlRng/nPosRng)+nMargin;
  
  
  // Draw the background
  gslc_DrawFillRect(pGui,pElem->rElem,(bGlow)?pElem->colElemFillGlow:pElem->colElemFill);

  // Draw any ticks
  if (nTickDiv>0) {
    uint16_t  nTickInd;
    int16_t   nTickGap = nCtrlRng/(nTickDiv-1);
    for (nTickInd=0;nTickInd<=nTickDiv;nTickInd++) {
      gslc_DrawLine(pGui,nX0+nMargin+nTickInd*nTickGap,nYMid,
              nX0+nMargin+nTickInd*nTickGap,nYMid+nTickLen,colTick);
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
    // TODO:
  }
  

  int16_t       nCtrlX0,nCtrlY0;
  gslc_tsRect   rThumb;
  nCtrlX0   = nX0+nCtrlPos-nThumbSz;
  nCtrlY0   = nYMid-nThumbSz;
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
  gslc_ElemSetRedraw(pElem,false);
  
  // Mark page as needing flip
  gslc_PageFlipSet(pGui,true);

  return true;
}


// This callback function is called by gslc_ElemSendEventTouch()
// after any touch event
bool gslc_ElemXSliderTouch(void* pvGui,void* pvElem,gslc_teTouch eTouch,int16_t nRelX,int16_t nRelY)
{
  if ((pvGui == NULL) || (pvElem == NULL)) {
    debug_print("ERROR: ElemXSliderTouch(%s) called with NULL ptr\n","");
    return false;
  }    
  gslc_tsGui*           pGui = NULL;
  gslc_tsElem*          pElem = NULL;
  gslc_tsXSlider*       pSlider = NULL;
  
  // Typecast the parameters to match the GUI
  pGui      = (gslc_tsGui*)(pvGui);
  pElem     = (gslc_tsElem*)(pvElem);
  pSlider   = (gslc_tsXSlider*)(pElem->pXData);  

  bool    bGlowingOld = pElem->bGlowing;  
  int16_t nPosRng;
  int16_t nPos;
  bool    bUpdatePos = false;
  
  switch(eTouch) {
    
    case GSLC_TOUCH_DOWN_IN:
      // Start glowing as must be over it
      pElem->bGlowing = true;
      bUpdatePos = true;
      break;
      
    case GSLC_TOUCH_MOVE_IN:
      pElem->bGlowing = true;
      bUpdatePos = true;
      break;
    case GSLC_TOUCH_MOVE_OUT:
      pElem->bGlowing = false;
      bUpdatePos = true;
      break;
      
    case GSLC_TOUCH_UP_IN:
      // End glow
      pElem->bGlowing = false;
      break;
    case GSLC_TOUCH_UP_OUT:
      // End glow
      pElem->bGlowing = false;
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
    nPos = (nRelX * nPosRng / pElem->rElem.w) + pSlider->nPosMin;
    // Update the slider
    gslc_ElemXSliderSetPos(pGui,pElem,nPos);
  }
  
  // If the slider changed state, redraw
  if (pElem->bGlowing != bGlowingOld) {
    gslc_ElemSetRedraw(pElem,true);
  }
  
  return true;
}



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
gslc_tsElem* gslc_ElemXSelNumCreate(gslc_tsGui* pGui,int16_t nElemId,int16_t nPage,
  gslc_tsXSelNum* pXData,gslc_tsRect rElem,int8_t nFontId)
{
  if ((pGui == NULL) || (pXData == NULL)) {
    debug_print("ERROR: ElemXSelNumCreate(%s) called with NULL ptr\n","");
    return NULL;
  }   
  gslc_tsElem sElem;
  
  
  // Initialize composite element
  sElem = gslc_ElemCreate(pGui,nElemId,nPage,GSLC_TYPEX_SELNUM,rElem,NULL,0,GSLC_FONT_NONE);
  sElem.bFrameEn        = true;
  sElem.bFillEn         = true;
  sElem.bClickEn        = true;
  sElem.bGlowEn         = false;  // Don't need to glow the outer element
  sElem.bGlowing        = false;
  sElem.nGroup          = GSLC_GROUP_ID_NONE;  

  pXData->nCounter      = 0;
   
  // Determine the maximum number of elements that we can store
  // in the sub-element array. We do this at run-time with sizeof()
  // instead of using #define to avoid polluting the global namespace.
  int16_t nSubElemMax = sizeof(pXData->asElem) / sizeof(pXData->asElem[0]);
  
  // NOTE: The last parameter in CollectReset() must match the size of
  //       the asElem[] array. It is used for bounds checking when we
  //       add new elements.
  gslc_CollectReset(&pXData->sCollect,pXData->asElem,nSubElemMax);

  
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
  gslc_tsElem*    pElemTmp  = NULL;
  gslc_tsElem*    pElem     = NULL;

  // Determine offset coordinate of compound element so that we can
  // specify relative positioning during the sub-element Create() operations.
  int16_t nOffsetX = rElem.x;
  int16_t nOffsetY = rElem.y;  

  #if (GSLC_LOCAL_STR)
  pElemTmp = gslc_ElemCreateBtnTxt(pGui,SELNUM_ID_BTN_INC,GSLC_PAGE_NONE,
    (gslc_tsRect){nOffsetX+40,nOffsetY+10,30,30},"+",0,nFontId,&gslc_ElemXSelNumClick);
  #else
  strncpy(pXData->acElemTxt[0],"+",SELNUM_STR_LEN-1);
  pElemTmp = gslc_ElemCreateBtnTxt(pGui,SELNUM_ID_BTN_INC,GSLC_PAGE_NONE,
    (gslc_tsRect){nOffsetX+40,nOffsetY+10,30,30},pXData->acElemTxt[0],SELNUM_STR_LEN,
    nFontId,&gslc_ElemXSelNumClick);
  #endif
  gslc_ElemSetCol(pElemTmp,(gslc_tsColor){0,0,192},(gslc_tsColor){0,0,128},(gslc_tsColor){0,0,224}); 
  gslc_ElemSetTxtCol(pElemTmp,GSLC_COL_WHITE);
  pElem = gslc_CollectElemAdd(&pXData->sCollect,pElemTmp);

  #if (GSLC_LOCAL_STR)
  pElemTmp = gslc_ElemCreateBtnTxt(pGui,SELNUM_ID_BTN_DEC,GSLC_PAGE_NONE,
    (gslc_tsRect){nOffsetX+80,nOffsetY+10,30,30},"-",0,nFontId,&gslc_ElemXSelNumClick);
  #else
  strncpy(pXData->acElemTxt[1],"-",SELNUM_STR_LEN-1);
  pElemTmp = gslc_ElemCreateBtnTxt(pGui,SELNUM_ID_BTN_DEC,GSLC_PAGE_NONE,
    (gslc_tsRect){nOffsetX+80,nOffsetY+10,30,30},pXData->acElemTxt[1],SELNUM_STR_LEN,
    nFontId,&gslc_ElemXSelNumClick);
  #endif
  gslc_ElemSetCol(pElemTmp,(gslc_tsColor){0,0,192},(gslc_tsColor){0,0,128},(gslc_tsColor){0,0,224}); 
  gslc_ElemSetTxtCol(pElemTmp,GSLC_COL_WHITE);  
  pElem = gslc_CollectElemAdd(&pXData->sCollect,pElemTmp);

  #if (GSLC_LOCAL_STR)
  pElemTmp = gslc_ElemCreateTxt(pGui,SELNUM_ID_TXT,GSLC_PAGE_NONE,
    (gslc_tsRect){nOffsetX+10,nOffsetY+10,20,30},"0",0,nFontId);
  #else
  strncpy(pXData->acElemTxt[2],"0",SELNUM_STR_LEN-1);
  pElemTmp = gslc_ElemCreateTxt(pGui,SELNUM_ID_TXT,GSLC_PAGE_NONE,
    (gslc_tsRect){nOffsetX+10,nOffsetY+10,20,30},pXData->acElemTxt[2],SELNUM_STR_LEN,nFontId);
  #endif
  pElem = gslc_CollectElemAdd(&pXData->sCollect,pElemTmp);


  // Now proceed to add the compound element to the page
  if (nPage != GSLC_PAGE_NONE) {
    pElem = gslc_ElemAdd(pGui,nPage,&sElem);
    
    // Now propagate the parent relationship to enable a cascade
    // of redrawing from low-level elements to the top
    gslc_CollectSetParent(&pXData->sCollect,pElem);
    
    return pElem;
  } else {
    // Save as temporary element for further processing
    pGui->sElemTmp = sElem;
    return &(pGui->sElemTmp);        
  }
}


// Redraw the compound element
// - When drawing a compound element, we clear the background
//   and then redraw the sub-element collection.
bool gslc_ElemXSelNumDraw(void* pvGui,void* pvElem)
{
  if ((pvGui == NULL) || (pvElem == NULL)) {
    debug_print("ERROR: ElemXSelNumDraw(%s) called with NULL ptr\n","");
    return false;
  }     
  // Typecast the parameters to match the GUI and element types
  gslc_tsGui*   pGui  = (gslc_tsGui*)(pvGui);
  gslc_tsElem*  pElem = (gslc_tsElem*)(pvElem);
  bool          bGlow = pElem->bGlowEn && pElem->bGlowing; 
  
  // Fetch the element's extended data structure
  gslc_tsXSelNum* pSelNum;
  pSelNum = (gslc_tsXSelNum*)(pElem->pXData);
  if (pSelNum == NULL) {
    debug_print("ERROR: ElemXSelNumDraw(%s) pXData is NULL\n","");
    return false;
  }
  
  // Draw the compound element fill (background)
  gslc_DrawFillRect(pGui,pElem->rElem,(bGlow)?pElem->colElemFillGlow:pElem->colElemFill);
  
  // Draw the sub-elements
  // - For now, force redraw of entire compound element
  gslc_tsCollect* pCollect = &pSelNum->sCollect;
  
  gslc_tsEvent  sEvent = gslc_EventCreate(GSLC_EVT_DRAW,GSLC_EVTSUB_DRAW_FORCE,(void*)(pCollect),NULL);
  gslc_CollectEvent(pGui,sEvent);

  // Optionally, draw a frame around the compound element
  // - This could instead be done by creating a sub-element
  //   of type box.
  // - We don't need to show any glowing of the compound element
  
  gslc_DrawFrameRect(pGui,pElem->rElem,(bGlow)?pElem->colElemFrameGlow:pElem->colElemFrame);
  
  // Clear the redraw flag
  gslc_ElemSetRedraw(pElem,false);
  
  // Mark page as needing flip
  gslc_PageFlipSet(pGui,true);

  return true;
}

// Fetch the current value of the element's counter
int gslc_ElemXSelNumGetCounter(gslc_tsGui* pGui,gslc_tsXSelNum* pSelNum)
{
  if ((pGui == NULL) || (pSelNum == NULL)) {
    debug_print("ERROR: ElemXSelNumGetCounter(%s) called with NULL ptr\n","");
    return 0;
  }     
  return pSelNum->nCounter;
}

void gslc_ElemXSelNumSetCounter(gslc_tsXSelNum* pSelNum,int16_t nCount)
{
  if (pSelNum == NULL) {
    debug_print("ERROR: ElemXSelNumSetCounter(%s) called with NULL ptr\n","");
    return;
  }
  pSelNum->nCounter = nCount;
  
  // Determine new counter text
  
  char  acStrNew[GSLC_LOCAL_STR_LEN];
  snprintf(acStrNew,GSLC_LOCAL_STR_LEN,"%d",pSelNum->nCounter);
  
  // Update the element
  gslc_tsElem* pElem = gslc_CollectFindElemById(&pSelNum->sCollect,SELNUM_ID_TXT);
  gslc_ElemSetTxtStr(pElem,acStrNew);

}


// Handle the compound element main functionality
// - This routine is called by gslc_ElemEvent() to handle
//   any click events that resulted from the touch tracking process.
// - The code here will generally represent the core
//   functionality of the compound element and any communication
//   between sub-elements.
// - pvElem is a void pointer to the element being tracked. From
//   the pElemParent member we can get the parent/compound element
//   data structures.
bool gslc_ElemXSelNumClick(void* pvGui,void *pvElem,gslc_teTouch eTouch,int16_t nX,int16_t nY)
{
  if ((pvGui == NULL) || (pvElem == NULL)) {
    debug_print("ERROR: ElemXSelNumClick(%s) called with NULL ptr\n","");
    return false;
  }  
  //gslc_tsGui*   pGui      = (gslc_tsGui*)(pvGui);
  gslc_tsElem*    pElem     = (gslc_tsElem*)(pvElem);
  
  // Fetch the parent of the clicked element which is the compound
  // element itself. This enables us to access the extra control data.
  gslc_tsElem*    pElemParent = pElem->pElemParent;
  if (pElemParent == NULL) {
    debug_print("ERROR: ElemXSelNumClick(%s) parent Elem ptr NULL\n","");
    return false;
  }
  
  gslc_tsXSelNum* pSelNum   = (gslc_tsXSelNum*)(pElemParent->pXData);
  if (pSelNum == NULL) {
    debug_print("ERROR: ElemXSelNumClick() element (ID=%d) has NULL pXData\n",pElem->nId);
    return false;
  }
  
  // Begin the core compound element functionality 
  int nCounter  = pSelNum->nCounter;

  // Handle the various button presses
  if (eTouch == GSLC_TOUCH_UP_IN) {
    
    // Get the tracked element ID
    int nSubElemId = pSelNum->sCollect.pElemTracked->nId;
    
    if (nSubElemId == SELNUM_ID_BTN_INC) {
      // Increment button
      if (nCounter<100) {
        nCounter++;
      }
      gslc_ElemXSelNumSetCounter(pSelNum,nCounter);  

    } else if (nSubElemId == SELNUM_ID_BTN_DEC) {
      // Decrement button
      if (nCounter>0) {
        nCounter--;
      }
      gslc_ElemXSelNumSetCounter(pSelNum,nCounter);

    }
  } // eTouch
  
  return true;
}

bool gslc_ElemXSelNumTouch(void* pvGui,void* pvElem,gslc_teTouch eTouch,int16_t nRelX,int16_t nRelY)
{
  if ((pvGui == NULL) || (pvElem == NULL)) {
    debug_print("ERROR: ElemXSelNumTouch(%s) called with NULL ptr\n","");
    return false;
  }    
  gslc_tsGui*           pGui = NULL;
  gslc_tsElem*          pElem = NULL;
  gslc_tsXSelNum*       pSelNum = NULL;

  
  // Typecast the parameters to match the GUI
  pGui  = (gslc_tsGui*)(pvGui);
  pElem = (gslc_tsElem*)(pvElem);
  pSelNum = (gslc_tsXSelNum*)(pElem->pXData);  
  

  // Handle any compound element operations
  switch(eTouch) {
    case GSLC_TOUCH_DOWN_IN:
    case GSLC_TOUCH_MOVE_IN:
      gslc_ElemSetGlow(pElem,true);
      break;
    case GSLC_TOUCH_MOVE_OUT:
    case GSLC_TOUCH_UP_IN:
    case GSLC_TOUCH_UP_OUT:
    default:
      gslc_ElemSetGlow(pElem,false);
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
  if (gslc_CollectGetRedraw(pCollect)) {
    gslc_ElemSetRedraw(pElem,true);
  }
  
  
  return true;

}


// ============================================================================