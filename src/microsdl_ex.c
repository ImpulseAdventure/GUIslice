// =======================================================================
// microSDL library (extensions)
// - Calvin Hass
// - http:/www.impulseadventure.com/elec/microsdl-sdl-gui.html
//
// - Version 0.4.1    (2016/11/08)
// =======================================================================


// MicroSDL library
#include "microsdl.h"
#include "microsdl_ex.h"

#include <stdio.h>

#include "SDL/SDL.h"
#include "SDL/SDL_getenv.h"
#include "SDL/SDL_ttf.h"


// ----------------------------------------------------------------------------
// Extended element definitions
// ----------------------------------------------------------------------------
//
// - This file extends the core microSDL functionality with
//   additional widget types
// - After adding any widgets to microsdl_ex, a unique
//   enumeration (MSDL_TYPEX_*) should be added to "microsdl.h"
//
//   TODO: Consider whether we should remove the need to update
//         these enumerations in "microsdl.h"; we could instead
//         define a single "MSDL_TYPEX" in microsdl.h but then
//         allow "microsdl_ex.h" to create a new set of unique
//         enumerations. This way extended elements could be created
//         in microsdl_ex and no changes at all would be required
//         in microsdl.

// ----------------------------------------------------------------------------


// ============================================================================
// Extended Element: Gauge
// - Basic progress bar with support for positive/negative state
//   and vertical / horizontal orientation.
// ============================================================================

// Create a gauge element and add it to the GUI element list
// - Defines default styling for the element
// - Defines callback for redraw but does not track touch/click
microSDL_tsElem* microSDL_ElemXGaugeCreate(microSDL_tsGui* pGui,int nElemId,int nPage,
  microSDL_tsXGauge* pXData,SDL_Rect rElem,
  int nMin,int nMax,int nVal,SDL_Color colGauge,bool bVert)
{
  if ((pGui == NULL) || (pXData == NULL)) {
    fprintf(stderr,"ERROR: ElemXGaugeCreate() called with NULL ptr\n");
    return NULL;
  }    
  microSDL_tsElem   sElem;
  microSDL_tsElem*  pElem = NULL;
  sElem = microSDL_ElemCreate(pGui,nElemId,nPage,MSDL_TYPEX_GAUGE,rElem,NULL,MSDL_FONT_NONE);
  sElem.bFrameEn        = true;
  sElem.bFillEn         = true;
  sElem.bClickEn        = false;          // Element is not "clickable"
  sElem.nGroup          = MSDL_GROUP_ID_NONE;  
  pXData->nGaugeMin     = nMin;
  pXData->nGaugeMax     = nMax;
  pXData->nGaugeVal     = nVal;
  pXData->bGaugeVert    = bVert;
  pXData->colGauge      = colGauge;
  sElem.pXData          = (void*)(pXData);
  sElem.pfuncXDraw      = &microSDL_ElemXGaugeDraw;
  sElem.pfuncXTouch     = NULL;           // No need to track touches
  sElem.colElemFrame    = MSDL_COL_GRAY;
  sElem.colElemFill     = MSDL_COL_BLACK;
  if (nPage != MSDL_PAGE_NONE) {
    pElem = microSDL_ElemAdd(pGui,sElem);
    return pElem;
  } else {
    // Save as temporary element
    pGui->psElem[MSDL_IND_TEMP] = sElem;
    return &(pGui->psElem[MSDL_IND_TEMP]);
  }
}

// Update the gauge control's current position
void microSDL_ElemXGaugeUpdate(microSDL_tsElem* pElem,int nVal)
{
  if (pElem == NULL) {
    fprintf(stderr,"ERROR: ElemXGaugeUpdate() called with NULL ptr\n");
    return;
  }   
  microSDL_tsXGauge*  pGauge  = (microSDL_tsXGauge*)(pElem->pXData);
  
  // Update the data element
  int nValOld = pGauge->nGaugeVal;
  pGauge->nGaugeVal = nVal;
  
  // Element needs redraw
  if (nVal != nValOld) {
    pElem->bNeedRedraw = true;
  }
  
}

// Redraw the gauge
// - Note that this redraw is for the entire element rect region
// - The Draw function parameters use void pointers to allow for
//   simpler callback function definition & scalability.
bool microSDL_ElemXGaugeDraw(void* pvGui,void* pvElem)
{
  if ((pvGui == NULL) || (pvElem == NULL)) {
    fprintf(stderr,"ERROR: ElemXGaugeDraw() called with NULL ptr\n");
    return false;
  }   
  
  // Typecast the parameters to match the GUI and element types
  microSDL_tsGui*   pGui  = (microSDL_tsGui*)(pvGui);
  microSDL_tsElem*  pElem = (microSDL_tsElem*)(pvElem);
  
  SDL_Rect    rGauge;
  int         nElemX,nElemY;
  unsigned    nElemW,nElemH;

  nElemX = pElem->rElem.x;
  nElemY = pElem->rElem.y;
  nElemW = pElem->rElem.w;
  nElemH = pElem->rElem.h;

  rGauge.x = nElemX;
  rGauge.y = nElemY;

  // Fetch the element's extended data structure
  microSDL_tsXGauge* pGauge;
  pGauge = (microSDL_tsXGauge*)(pElem->pXData);
  if (pGauge == NULL) {
    fprintf(stderr,"ERROR: ElemXGaugeDraw() pXData is NULL\n");
    return false;
  }
  
  // Draw the background
  microSDL_FillRect(pGui,pElem->rElem,pElem->colElemFill);
  
  
  bool  bVert = pGauge->bGaugeVert;
  int   nMax = pGauge->nGaugeMax;
  int   nMin = pGauge->nGaugeMin;
  int   nRng = pGauge->nGaugeMax - pGauge->nGaugeMin;

  if (nRng == 0) {
    fprintf(stderr,"ERROR: ElemXGaugeDraw() Zero gauge range [%d,%d]\n",nMin,nMax);
    return false;
  }
  float   fScl;
  if (bVert) {
    fScl = (float)nElemH/(float)nRng;
  } else {
    fScl = (float)nElemW/(float)nRng;
  }

  // Calculate the control midpoint (for display purposes)
  int nGaugeMid;
  if ((nMin == 0) && (nMax > 0)) {
    nGaugeMid = 0;
  } else if ((nMin < 0) && (nMax > 0)) {
    nGaugeMid = -nMin*fScl;
  } else if ((nMin < 0) && (nMax == 0)) {
    nGaugeMid = -nMin*fScl;
  } else {
    fprintf(stderr,"ERROR: ElemXGaugeDraw() Unsupported gauge range [%d,%d]\n",nMin,nMax);
    return false;
  }

  // Calculate the length of the bar
  int nLen = pGauge->nGaugeVal * fScl;

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
  int nRectClipX1 = rGauge.x;
  int nRectClipX2 = rGauge.x+rGauge.w-1;
  int nRectClipY1 = rGauge.y;
  int nRectClipY2 = rGauge.y+rGauge.h-1;
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

  // Draw the gauge fill region
  microSDL_FillRect(pGui,rGauge,pGauge->colGauge);

  // Draw the midpoint line
  SDL_Rect    rMidLine;
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
  microSDL_FillRect(pGui,rMidLine,pElem->colElemFrame);

  // Draw a frame around the gauge
  microSDL_FrameRect(pGui,pElem->rElem,pElem->colElemFrame);
  
  // Clear the redraw flag
  pElem->bNeedRedraw = false;
  
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
microSDL_tsElem* microSDL_ElemXCheckboxCreate(microSDL_tsGui* pGui,int nElemId,int nPage,
  microSDL_tsXCheckbox* pXData,SDL_Rect rElem,bool bRadio,microSDL_teXCheckboxStyle nStyle,
  SDL_Color colCheck,bool bChecked)
{
  if ((pGui == NULL) || (pXData == NULL)) {
    fprintf(stderr,"ERROR: ElemXCheckboxCreate() called with NULL ptr\n");
    return NULL;
  }      
  microSDL_tsElem   sElem;
  microSDL_tsElem*  pElem = NULL;
  sElem = microSDL_ElemCreate(pGui,nElemId,nPage,MSDL_TYPEX_CHECKBOX,rElem,NULL,MSDL_FONT_NONE);
  sElem.bFrameEn        = false;
  sElem.bFillEn         = true;
  sElem.bClickEn        = true;
  sElem.bGlowing        = false;
  // Default group assignment. Can override later with ElemSetGroup()
  sElem.nGroup          = MSDL_GROUP_ID_NONE;
  // Save a pointer to the GUI in the extended element data
  // - We do this so that we can later perform actions on
  //   other elements (ie. when clicking radio button, others
  //   are deselected).
  pXData->pGui          = pGui;
  // Define other extended data
  pXData->bRadio        = bRadio;
  pXData->bChecked      = bChecked;
  pXData->colCheck      = colCheck;
  pXData->nStyle        = nStyle;
  sElem.pXData          = (void*)(pXData);
  // Specify the custom drawing callback
  sElem.pfuncXDraw      = &microSDL_ElemXCheckboxDraw;
  // Specify the custom touch tracking callback
  // - NOTE: This is optional (and can be set to NULL).
  //   See the discussion under microSDL_ElemXCheckboxTouch()
  sElem.pfuncXTouch     = &microSDL_ElemXCheckboxTouch;
  sElem.colElemFrame    = MSDL_COL_GRAY;
  sElem.colElemFill     = MSDL_COL_BLACK;
  sElem.colElemGlow     = MSDL_COL_WHITE;  
  if (nPage != MSDL_PAGE_NONE) {
    pElem = microSDL_ElemAdd(pGui,sElem);
    return pElem;
  } else {
    // Save as temporary element
    pGui->psElem[MSDL_IND_TEMP] = sElem;
    return &(pGui->psElem[MSDL_IND_TEMP]);
  }
}

bool microSDL_ElemXCheckboxGetState(microSDL_tsElem* pElem)
{
  microSDL_tsXCheckbox*  pCheckbox = (microSDL_tsXCheckbox*)(pElem->pXData);
  return pCheckbox->bChecked;
}


// Determine which checkbox in group has been "checked"
int microSDL_ElemXCheckboxFindChecked(microSDL_tsGui* pGui,int nGroupId)
{
  int                     nCurInd;
  int                     nFoundId = MSDL_ID_NONE;
  microSDL_tsElem*        pCurElem = NULL;
  microSDL_teType         nCurType;
  int                     nCurGroup;
  bool                    bCurChecked;

  if (pGui == NULL) {
    fprintf(stderr,"ERROR: ElemXCheckboxFindChecked() called with NULL ptr\n");
    return MSDL_ID_NONE;
  }   
  for (nCurInd=MSDL_IND_FIRST;nCurInd<pGui->nElemCnt;nCurInd++) {
    // Fetch extended data
    pCurElem      = &pGui->psElem[nCurInd];
    nCurType      = pCurElem->nType;
    // Only want to proceed if it is a checkbox
    if (nCurType != MSDL_TYPEX_CHECKBOX) {
      continue;
    }

    nCurGroup     = pCurElem->nGroup;
    bCurChecked   = microSDL_ElemXCheckboxGetState(pCurElem);
   
    // If this is in a different group, ignore it
    if (nCurGroup != nGroupId) {
      continue;
    }

    // Did we find an element in the group that was checked?
    if (bCurChecked) {
      nFoundId = pCurElem->nId;
      break;
    }
  } // nCurInd
  return nFoundId;
}



// Update the checkbox control's current state
void microSDL_ElemXCheckboxSetState(microSDL_tsElem* pElem,bool bChecked)
{
  if (pElem == NULL) {
    fprintf(stderr,"ERROR: ElemXCheckboxSetState() called with NULL ptr\n");
    return;
  }   
  microSDL_tsXCheckbox*   pCheckbox = (microSDL_tsXCheckbox*)(pElem->pXData);
  microSDL_tsGui*         pGui      = pCheckbox->pGui;
  bool                    bRadio    = pCheckbox->bRadio;
  int                     nGroup    = pElem->nGroup;
  int                     nElemId   = pElem->nId;
  
  
  // If radio-button style and we are selecting an element
  // then proceed to deselect any other selected items in the group.
  // Note that SetState calls itself to deselect other items so it
  // is important to qualify this logic with bChecked=true
  if (bRadio && bChecked) {
    int                   nCurInd;
    int                   nCurId;
    microSDL_tsElem*      pCurElem = NULL;
    microSDL_teType       nCurType;
    int                   nCurGroup;
    
    // We use the GUI pointer for access to other elements   
    for (nCurInd=MSDL_IND_FIRST;nCurInd<pGui->nElemCnt;nCurInd++) {
      // Fetch extended data
      pCurElem      = &pGui->psElem[nCurInd];
      nCurId        = pCurElem->nId;
      nCurType      = pCurElem->nType;
      
      // Only want to proceed if it is a checkbox
      if (nCurType != MSDL_TYPEX_CHECKBOX) {
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
      microSDL_ElemXCheckboxSetState(pCurElem,false);        

    } // nInd
      
  } // bRadio

  
  // Update our data element
  bool  bCheckedOld = pCheckbox->bChecked;
  pCheckbox->bChecked = bChecked;
  
  // Element needs redraw
  if (bChecked != bCheckedOld) {
    pElem->bNeedRedraw = true;
  }

}

// Toggle the checkbox control's state
void microSDL_ElemXCheckboxToggleState(microSDL_tsElem* pElem)
{  
  if (pElem == NULL) {
    fprintf(stderr,"ERROR: ElemXCheckboxToggleState() called with NULL ptr\n");
    return;
  }     
  // Update the data element
  bool bCheckNew = (microSDL_ElemXCheckboxGetState(pElem))? false : true;
  microSDL_ElemXCheckboxSetState(pElem,bCheckNew);

  // Element needs redraw
  pElem->bNeedRedraw = true;
  
}

// Redraw the checkbox
// - Note that this redraw is for the entire element rect region
// - The Draw function parameters use void pointers to allow for
//   simpler callback function definition & scalability.
bool microSDL_ElemXCheckboxDraw(void* pvGui,void* pvElem)
{
  if ((pvGui == NULL) || (pvElem == NULL)) {
    fprintf(stderr,"ERROR: ElemXCheckboxDraw() called with NULL ptr\n");
    return false;
  }   
  // Typecast the parameters to match the GUI and element types
  microSDL_tsGui*   pGui  = (microSDL_tsGui*)(pvGui);
  microSDL_tsElem*  pElem = (microSDL_tsElem*)(pvElem);
  
  SDL_Rect    rInner;
  
  // Fetch the element's extended data structure
  microSDL_tsXCheckbox* pCheckbox;
  pCheckbox = (microSDL_tsXCheckbox*)(pElem->pXData);
  if (pCheckbox == NULL) {
    fprintf(stderr,"ERROR: ElemXCheckboxDraw() pXData is NULL\n");
    return false;
  }
  
  bool                      bChecked  = pCheckbox->bChecked;
  microSDL_teXCheckboxStyle nStyle    = pCheckbox->nStyle;
  bool                      bGlowing  = pElem->bGlowing;
  
  // Draw the background
  microSDL_FillRect(pGui,pElem->rElem,pElem->colElemFill);

  // Generic coordinate calcs
  int nX0,nY0,nX1,nY1,nMidX,nMidY;
  nX0 = pElem->rElem.x;
  nY0 = pElem->rElem.y;
  nX1 = pElem->rElem.x + pElem->rElem.w - 1;
  nY1 = pElem->rElem.y + pElem->rElem.h - 1;  
  nMidX = (nX0+nX1)/2;
  nMidY = (nY0+nY1)/2;
  if (nStyle == MSDLX_CHECKBOX_STYLE_BOX) {
    // Draw the center indicator if checked
    rInner = microSDL_ExpandRect(pElem->rElem,-5,-5);
    if (bChecked) {
      // If checked, fill in the inner region
      microSDL_FillRect(pGui,rInner,pCheckbox->colCheck);
    } else {
      // Assume the background fill has already been done so
      // we don't need to do anything more in the unchecked case
    }
    // Draw a frame around the checkbox
    if (bGlowing) {
      microSDL_FrameRect(pGui,pElem->rElem,pElem->colElemGlow);  
    } else {
      microSDL_FrameRect(pGui,pElem->rElem,pElem->colElemFrame);
    }    
  } else if (nStyle == MSDLX_CHECKBOX_STYLE_X) {
    // Draw an X through center if checked
    if (bChecked) {
      microSDL_Line(pGui,nX0,nY0,nX1,nY1,pCheckbox->colCheck);
      microSDL_Line(pGui,nX0,nY1,nX1,nY0,pCheckbox->colCheck);
    }
    // Draw a frame around the checkbox
    if (bGlowing) {
      microSDL_FrameRect(pGui,pElem->rElem,pElem->colElemGlow);  
    } else {
      microSDL_FrameRect(pGui,pElem->rElem,pElem->colElemFrame);
    }        
  } else if (nStyle == MSDLX_CHECKBOX_STYLE_ROUND) {
    // Draw inner circle if checked
    if (bChecked) {
      // TODO: A FillCircle() may look better here
      microSDL_FrameCircle(pGui,nMidX,nMidY,5,pCheckbox->colCheck);    
    }
    // Draw a frame around the checkbox
    if (bGlowing) {
      microSDL_FrameCircle(pGui,nMidX,nMidY,(pElem->rElem.w/2),pElem->colElemGlow);  
    } else {
      microSDL_FrameCircle(pGui,nMidX,nMidY,(pElem->rElem.w/2),pElem->colElemFrame);  
    }        
    
  }
  
  // Clear the redraw flag
  pElem->bNeedRedraw = false;
  
  // Mark page as needing flip
  microSDL_PageFlipSet(pGui,true);

  return true;
}

// This callback function is called by microSDL_NotifyElemTouch()
// after any touch event
// - NOTE: Adding this touch callback is optional. Without it, we
//   can still have a functional checkbox, but doing the touch
//   tracking allows us to change the glow state of the element
//   dynamically, as well as updating the checkbox state if the
//   user releases over it (ie. a click event).
//   
bool microSDL_ElemXCheckboxTouch(void* pvGui,void* pvElem,microSDL_teTouch eTouch,int nRelX,int nRelY)
{
  if ((pvGui == NULL) || (pvElem == NULL)) {
    fprintf(stderr,"ERROR: ElemXCheckboxTouch() called with NULL ptr\n");
    return false;
  }       
  //microSDL_tsGui*           pGui = NULL;
  microSDL_tsElem*          pElem = NULL;
  microSDL_tsXCheckbox*     pCheckbox = NULL;
  
  // Typecast the parameters to match the GUI
  //pGui  = (microSDL_tsGui*)(pvGui);
  pElem = (microSDL_tsElem*)(pvElem);
  pCheckbox = (microSDL_tsXCheckbox*)(pElem->pXData);  

  bool  bRadio      = pCheckbox->bRadio;
  bool  bCheckedOld = pCheckbox->bChecked;
  bool  bGlowingOld = pElem->bGlowing;  
  
  switch(eTouch) {
    
    case MSDL_TOUCH_DOWN_IN:
      pElem->bGlowing = true;
      break;
      
    case MSDL_TOUCH_MOVE_IN:
      pElem->bGlowing = true;
      break;
    case MSDL_TOUCH_MOVE_OUT:
      pElem->bGlowing = false;
      break;

      
    case MSDL_TOUCH_UP_IN:
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
      microSDL_ElemXCheckboxSetState(pElem,bCheckNew);
      break;
    case MSDL_TOUCH_UP_OUT:
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
    pElem->bNeedRedraw = true;
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
microSDL_tsElem* microSDL_ElemXSliderCreate(microSDL_tsGui* pGui,int nElemId,int nPage,
  microSDL_tsXSlider* pXData,SDL_Rect rElem,int nPosMin,int nPosMax,int nPos,
  unsigned nThumbSz,bool bVert)
{
  if ((pGui == NULL) || (pXData == NULL)) {
    fprintf(stderr,"ERROR: ElemXSliderCreate() called with NULL ptr\n");
    return NULL;
  }     
  microSDL_tsElem   sElem;
  microSDL_tsElem*  pElem = NULL;
  sElem = microSDL_ElemCreate(pGui,nElemId,nPage,MSDL_TYPEX_SLIDER,rElem,NULL,MSDL_FONT_NONE);
  sElem.bFrameEn        = false;
  sElem.bFillEn         = true;
  sElem.bClickEn        = true;
  sElem.bGlowing        = false;
  sElem.nGroup          = MSDL_GROUP_ID_NONE;  
  pXData->nPosMin       = nPosMin;
  pXData->nPosMax       = nPosMax;
  pXData->nPos          = nPos;
  pXData->nThumbSz      = nThumbSz;
  pXData->bVert         = bVert;
  pXData->bTrim         = false;
  pXData->colTrim       = MSDL_COL_BLACK;
  pXData->nTickDiv      = 0;
  sElem.pXData          = (void*)(pXData);
  // Specify the custom drawing callback
  sElem.pfuncXDraw      = &microSDL_ElemXSliderDraw;
  // Specify the custom touch tracking callback
  sElem.pfuncXTouch     = &microSDL_ElemXSliderTouch;
  sElem.colElemFrame    = MSDL_COL_GRAY;
  sElem.colElemFill     = MSDL_COL_BLACK;
  sElem.colElemGlow     = MSDL_COL_WHITE;
  if (nPage != MSDL_PAGE_NONE) {
    pElem = microSDL_ElemAdd(pGui,sElem);
    return pElem;
  } else {
    // Save as temporary element
    pGui->psElem[MSDL_IND_TEMP] = sElem;
    return &(pGui->psElem[MSDL_IND_TEMP]);
  }
}

void microSDL_ElemXSliderSetStyle(microSDL_tsElem* pElem,
        bool bTrim,SDL_Color colTrim,unsigned nTickDiv,
        int nTickLen,SDL_Color colTick)
{
  if (pElem == NULL) {
    fprintf(stderr,"ERROR: ElemXSliderSetStyle() called with NULL ptr\n");
    return;
  }   
  microSDL_tsXSlider*  pSlider = (microSDL_tsXSlider*)(pElem->pXData);

  pSlider->bTrim      = bTrim;
  pSlider->colTrim    = colTrim;
  pSlider->nTickDiv   = nTickDiv;
  pSlider->nTickLen   = nTickLen;
  pSlider->colTick    = colTick;
  
  // Update
  pElem->bNeedRedraw      = true;
}

int microSDL_ElemXSliderGetPos(microSDL_tsElem* pElem)
{
  if (pElem == NULL) {
    fprintf(stderr,"ERROR: ElemXSliderGetPos() called with NULL ptr\n");
    return 0;
  }     
  microSDL_tsXSlider*  pSlider = (microSDL_tsXSlider*)(pElem->pXData);
  
  return pSlider->nPos;
}

// Update the slider control's current state
void microSDL_ElemXSliderSetPos(microSDL_tsElem* pElem,int nPos)
{
  if (pElem == NULL) {
    fprintf(stderr,"ERROR: ElemXSliderSetPos() called with NULL ptr\n");
    return;
  }       
  microSDL_tsXSlider*   pSlider = (microSDL_tsXSlider*)(pElem->pXData);
  int                   nPosOld;
  // Clip position
  if (nPos < pSlider->nPosMin) { nPos = pSlider->nPosMin; }
  if (nPos > pSlider->nPosMax) { nPos = pSlider->nPosMax; }     
  // Update
  nPosOld = pSlider->nPos;
  pSlider->nPos = nPos;
  // Only update if changed
  if (nPos != nPosOld) {
    pElem->bNeedRedraw = true;
  }
  
}


// Redraw the slider
// - Note that this redraw is for the entire element rect region
// - The Draw function parameters use void pointers to allow for
//   simpler callback function definition & scalability.
bool microSDL_ElemXSliderDraw(void* pvGui,void* pvElem)
{
  if ((pvGui == NULL) || (pvElem == NULL)) {
    fprintf(stderr,"ERROR: ElemXSliderDraw() called with NULL ptr\n");
    return false;
  }     
  // Typecast the parameters to match the GUI and element types
  microSDL_tsGui*   pGui  = (microSDL_tsGui*)(pvGui);
  microSDL_tsElem*  pElem = (microSDL_tsElem*)(pvElem);
  
  // Fetch the element's extended data structure
  microSDL_tsXSlider* pSlider;
  pSlider = (microSDL_tsXSlider*)(pElem->pXData);
  if (pSlider == NULL) {
    fprintf(stderr,"ERROR: ElemXSliderDraw() pXData is NULL\n");
    return false;
  }
  
  bool      bGlowing  = pElem->bGlowing; 
  int       nPos      = pSlider->nPos;
  int       nPosMin   = pSlider->nPosMin;
  int       nPosMax   = pSlider->nPosMax;
  bool      bVert     = pSlider->bVert;
  int       nThumbSz  = pSlider->nThumbSz;
  bool      bTrim     = pSlider->bTrim;
  SDL_Color colTrim   = pSlider->colTrim;
  unsigned  nTickDiv  = pSlider->nTickDiv;
  int       nTickLen  = pSlider->nTickLen;
  SDL_Color colTick   = pSlider->colTick;
  
  if (bVert) {
    fprintf(stderr,"ERROR: ElemXSliderDraw() bVert=true not supported yet\n");
    return false;
  }
  
  int   nX0,nY0,nX1,nY1,nYMid;
  nX0 = pElem->rElem.x;
  nY0 = pElem->rElem.y;
  nX1 = pElem->rElem.x + pElem->rElem.w - 1;
  nY1 = pElem->rElem.y + pElem->rElem.h - 1;
  nYMid = (nY0+nY1)/2;
  
  // Scale the current position
  int nPosRng = nPosMax-nPosMin;
  // TODO: Check for nPosRng=0, reversed min/max
  int nPosOffset = nPos-nPosMin;
  
  // Provide some margin so thumb doesn't exceed control bounds
  int nMargin = nThumbSz;
  int nCtrlRng = (nX1-nMargin)-(nX0+nMargin);
  int nCtrlPos = (nPosOffset*nCtrlRng/nPosRng)+nMargin;
  
  
  // Draw the background
  microSDL_FillRect(pGui,pElem->rElem,pElem->colElemFill);

  // Draw any ticks
  if (nTickDiv>0) {
    unsigned  nTickInd;
    int       nTickGap = nCtrlRng/(nTickDiv-1);
    for (nTickInd=0;nTickInd<=nTickDiv;nTickInd++) {
      microSDL_Line(pGui,nX0+nMargin+nTickInd*nTickGap,nYMid,
              nX0+nMargin+nTickInd*nTickGap,nYMid+nTickLen,colTick);
    }
  }  
  
  // Draw the track
  if (!bVert) {
      microSDL_Line(pGui,nX0+nMargin,nYMid,nX1-nMargin,nYMid,
              (bGlowing)? pElem->colElemGlow : pElem->colElemFrame);
      if (bTrim) {
        microSDL_Line(pGui,nX0+nMargin,nYMid+1,nX1-nMargin,nYMid+1,colTrim);
      }
    
  } else {
    // TODO:
  }
  


  int nCtrlX0,nCtrlY0;
  nCtrlX0 = nX0+nCtrlPos-nThumbSz;
  nCtrlY0 = nYMid-nThumbSz;
  SDL_Rect  rThumb;
  rThumb.x = nCtrlX0;
  rThumb.y = nCtrlY0;
  rThumb.w = 2*nThumbSz;
  rThumb.h = 2*nThumbSz;
  
  // Draw the thumb control
  microSDL_FillRect(pGui,rThumb,pElem->colElemFill);
  if (bGlowing) {
    microSDL_FrameRect(pGui,rThumb,pElem->colElemGlow);
  } else {
    microSDL_FrameRect(pGui,rThumb,pElem->colElemFrame);    
  }
  if (bTrim) {
    SDL_Rect  rThumbTrim;
    rThumbTrim = microSDL_ExpandRect(rThumb,-1,-1);
    microSDL_FrameRect(pGui,rThumbTrim,pSlider->colTrim);
  }
    


  // Clear the redraw flag
  pElem->bNeedRedraw = false;
  
  // Mark page as needing flip
  microSDL_PageFlipSet(pGui,true);

  return true;
}


// This callback function is called by microSDL_NotifyElemTouch()
// after any touch event
bool microSDL_ElemXSliderTouch(void* pvGui,void* pvElem,microSDL_teTouch eTouch,int nRelX,int nRelY)
{
  if ((pvGui == NULL) || (pvElem == NULL)) {
    fprintf(stderr,"ERROR: ElemXSliderTouch() called with NULL ptr\n");
    return false;
  }    
  //microSDL_tsGui*         pGui = NULL;
  microSDL_tsElem*          pElem = NULL;
  microSDL_tsXSlider*       pSlider = NULL;
  
  // Typecast the parameters to match the GUI
  //pGui    = (microSDL_tsGui*)(pvGui);
  pElem     = (microSDL_tsElem*)(pvElem);
  pSlider   = (microSDL_tsXSlider*)(pElem->pXData);  

  bool  bGlowingOld = pElem->bGlowing;  
  int   nPosRng;
  int   nPos;
  bool  bUpdatePos = false;
  
  switch(eTouch) {
    
    case MSDL_TOUCH_DOWN_IN:
      // Start glowing as must be over it
      pElem->bGlowing = true;
      bUpdatePos = true;
      break;
      
    case MSDL_TOUCH_MOVE_IN:
      pElem->bGlowing = true;
      bUpdatePos = true;
      break;
    case MSDL_TOUCH_MOVE_OUT:
      pElem->bGlowing = false;
      bUpdatePos = true;
      break;
      
    case MSDL_TOUCH_UP_IN:
      // End glow
      pElem->bGlowing = false;
      // TODO: Consider adding extra callback here?
      break;
    case MSDL_TOUCH_UP_OUT:
      // End glow
      pElem->bGlowing = false;
      break;
      
    default:
      return false;
      break;
  }
  
  // Do we need to update the slider position?
  if (bUpdatePos) {
    // Calc new position
    nPosRng = pSlider->nPosMax - pSlider->nPosMin;
    nPos = (nRelX * nPosRng / pElem->rElem.w) + pSlider->nPosMin;
    // Clip position
    if (nPos < pSlider->nPosMin) { nPos = pSlider->nPosMin; }
    if (nPos > pSlider->nPosMax) { nPos = pSlider->nPosMax; }     
    // Update
    pSlider->nPos = nPos;
    pElem->bNeedRedraw = true;    
  }
  
  // If the checkbox changed state, redraw
  bool  bChanged = false;
  if (pElem->bGlowing != bGlowingOld) { bChanged = true; }
  if (bChanged) {
    pElem->bNeedRedraw = true;
  }
  
  return true;

}



// ============================================================================
// Extended Element: SelNum
// - Checkbox with custom handler for touch tracking which
//   enables glow to be defined whenever touch is tracked over
//   the element.
// ============================================================================


// Create a compound element
// - For now just two buttons and a text area
microSDL_tsElem* microSDL_ElemXSelNumCreate(microSDL_tsGui* pGui,int nElemId,int nPage,
  microSDL_tsXSelNum* pXData,SDL_Rect rElem,int nFontId)
{
  if ((pGui == NULL) || (pXData == NULL)) {
    fprintf(stderr,"ERROR: ElemXSelNumCreate() called with NULL ptr\n");
    return NULL;
  }   
  microSDL_tsElem sElem;
  int             nSubInd;
  
  // Determine relative coordinate offset
  int   nOffsetX = rElem.x;
  int   nOffsetY = rElem.y;
  
  // Initialize composite element
  sElem = microSDL_ElemCreate(pGui,nElemId,nPage,MSDL_TYPEX_SELNUM,rElem,NULL,MSDL_FONT_NONE);
  sElem.bFrameEn        = true;
  sElem.bFillEn         = true;
  sElem.bClickEn        = true;
  sElem.bGlowing        = false;
  sElem.nGroup          = MSDL_GROUP_ID_NONE;  

  pXData->nCounter            = 0;
  
  pXData->nSubElemCnt         = 0;
  pXData->nTrackSubIdStart    = MSDL_ID_NONE;
  pXData->nTrackSubIndStart   = MSDL_IND_NONE;

  // Determine the maximum number of elements that we can store
  // in the sub-element array. We do this at run-time with sizeof()
  // instead of using #define to avoid polluting the global namespace.
  int nSubElemMax = sizeof(pXData->asElem) / sizeof(pXData->asElem[0]);
  
  // Clear the sub-elements
  for (nSubInd=0;nSubInd<nSubElemMax;nSubInd++) {
    microSDL_ResetElem(&(pXData->asElem[nSubInd]));
  }
  
  sElem.pXData          = (void*)(pXData);
  // Specify the custom drawing callback
  sElem.pfuncXDraw      = &microSDL_ElemXSelNumDraw;
  // Specify the custom touch tracking callback
  sElem.pfuncXTouch    = &microSDL_ElemXSelNumTouch;
  sElem.colElemFrame    = MSDL_COL_GRAY;
  sElem.colElemFill     = MSDL_COL_BLACK;
  sElem.colElemGlow     = MSDL_COL_WHITE; 

  // Start sub-element index at 1 since index 0 is reserved
  // for any temporary element creation
  int nSubElemCnt = MSDL_IND_FIRST;
  
 
  // Now create the sub elements
  // - Ensure page is set to MSDL_PAGE_NONE so that
  //   we create the element struct but not add it to a specific page.
  // - When we create an element with MSDL_PAGE_NONE it is
  //   saved in the GUI's temporary element storage.
  // - When we have finished creating / styling the element, we then
  //   copy it into the permanent sub-element storage
  
  // - The element IDs assigned to the sub-elements are
  //   arbitrary (with local scope in the compound element),
  //   so they don't need to be unique globally across the GUI.
  microSDL_tsElem*  pElemTmp = NULL;
  microSDL_tsElem*  pElem = NULL;

  pElemTmp = microSDL_ElemCreateBtnTxt(pGui,SELNUM_ID_BTN_INC,MSDL_PAGE_NONE,
    (SDL_Rect){nOffsetX+40,nOffsetY+10,30,30},"+",nFontId,&microSDL_ElemXSelNumClick);
  microSDL_ElemSetCol(pElemTmp,(SDL_Color){0,0,192},(SDL_Color){0,0,128},(SDL_Color){0,0,224}); 
  microSDL_ElemSetTxtCol(pElemTmp,MSDL_COL_WHITE);
  if (nSubElemCnt<nSubElemMax) {
    pXData->asElem[nSubElemCnt++] = microSDL_ElemGetTemp(pGui);
  }
  
  pElemTmp = microSDL_ElemCreateBtnTxt(pGui,SELNUM_ID_BTN_DEC,MSDL_PAGE_NONE,
    (SDL_Rect){nOffsetX+80,nOffsetY+10,30,30},"-",nFontId,&microSDL_ElemXSelNumClick);
  microSDL_ElemSetCol(pElemTmp,(SDL_Color){0,0,192},(SDL_Color){0,0,128},(SDL_Color){0,0,224}); 
  microSDL_ElemSetTxtCol(pElemTmp,MSDL_COL_WHITE);  
  if (nSubElemCnt<nSubElemMax) {
    pXData->asElem[nSubElemCnt++] = microSDL_ElemGetTemp(pGui);
  }
  
  pElemTmp = microSDL_ElemCreateTxt(pGui,SELNUM_ID_TXT,MSDL_PAGE_NONE,
    (SDL_Rect){nOffsetX+10,nOffsetY+10,20,30},"",nFontId);
  if (nSubElemCnt<nSubElemMax) {
    pXData->asElem[nSubElemCnt++] = microSDL_ElemGetTemp(pGui);
  }
  
  // Record the number of sub-elements
  pXData->nSubElemCnt = nSubElemCnt;

  // Now proceed to add to page
  if (nPage != MSDL_PAGE_NONE) {
    pElem = microSDL_ElemAdd(pGui,sElem);
    return pElem;
  } else {
    // Save as temporary element
    pGui->psElem[MSDL_IND_TEMP] = sElem;
    return &(pGui->psElem[MSDL_IND_TEMP]);
  }
}


// Redraw the compound element
// - When drawing a compound element, we clear the
//   background then call the individual element
//   drawing routines. But as they are not stored
//   in the main GUI's element storage we have to
//   use ElemDrawByRef() which accepts a pointer to
//   the sub-element.
bool microSDL_ElemXSelNumDraw(void* pvGui,void* pvElem)
{
  if ((pvGui == NULL) || (pvElem == NULL)) {
    fprintf(stderr,"ERROR: ElemXSelNumDraw() called with NULL ptr\n");
    return false;
  }     
  // Typecast the parameters to match the GUI and element types
  microSDL_tsGui*   pGui  = (microSDL_tsGui*)(pvGui);
  microSDL_tsElem*  pElem = (microSDL_tsElem*)(pvElem);
  int               nSubInd;
  
  // Fetch the element's extended data structure
  microSDL_tsXSelNum* pSelNum;
  pSelNum = (microSDL_tsXSelNum*)(pElem->pXData);
  if (pSelNum == NULL) {
    fprintf(stderr,"ERROR: ElemXSelNumDraw() pXData is NULL\n");
    return false;
  }
  
  
  // Draw the background
  microSDL_FillRect(pGui,pElem->rElem,pElem->colElemFill);
  
  // Draw the sub-elements
  // - We use ElemDrwByRef() since the sub-elements aren't
  //   stored in the main GUI's element array
  for (nSubInd=0;nSubInd<pSelNum->nSubElemCnt;nSubInd++) {
    microSDL_ElemDrawByRef(pGui,&(pSelNum->asElem[nSubInd]));
  }

  // Optionally, draw a frame around the compound element
  // - This could instead be done by creating a sub-element
  //   of type box.
  // - We don't need to show any glowing of the compound element
  microSDL_FrameRect(pGui,pElem->rElem,pElem->colElemFrame);

  
  // Clear the redraw flag
  pElem->bNeedRedraw = false;
  
  // Mark page as needing flip
  microSDL_PageFlipSet(pGui,true);

  return true;
}

// Fetch the current value of the element's counter
int microSDL_ElemXSelNumGetCounter(microSDL_tsGui* pGui,microSDL_tsXSelNum* pSelNum)
{
  if ((pGui == NULL) || (pSelNum == NULL)) {
    fprintf(stderr,"ERROR: ElemXSelNumGetCounter() called with NULL ptr\n");
    return 0;
  }     
  return pSelNum->nCounter;
}

void microSDL_ElemXSelNumSetCounter(microSDL_tsGui* pGui,microSDL_tsXSelNum* pSelNum,int nCount)
{
  if ((pGui == NULL) || (pSelNum == NULL)) {
    fprintf(stderr,"ERROR: ElemXSelNumSetCounter() called with NULL ptr\n");
    return;
  }
  pSelNum->nCounter = nCount;
  
  // TODO: Ideally, we would leverage the existing ElemSetTxtStr() function
  //       but that one operates on elements within GUI array only.
  int nIND_TXT = microSDL_ElemFindIndFromId(pSelNum->asElem,pSelNum->nSubElemCnt,SELNUM_ID_TXT);
  sprintf(pSelNum->asElem[nIND_TXT].acStr,"%d",pSelNum->nCounter);
  pSelNum->asElem[nIND_TXT].bNeedRedraw = true;
}


// Handle the compound element main functionality
// - This routine is called by microSDL_ElemXSelNumTouch() to handle
//   any click events that resulted from the touch tracking process.
// - The code here will generally represent the core
//   functionality of the compound element and any communication
//   between sub-elements.
bool microSDL_ElemXSelNumClick(void* pvGui,void *pvElem,microSDL_teTouch eTouch,int nX,int nY)
{
  if ((pvGui == NULL) || (pvElem == NULL)) {
    fprintf(stderr,"ERROR: ElemXSelNumClick() called with NULL ptr\n");
    return false;
  }  
  microSDL_tsGui*     pGui    = (microSDL_tsGui*)(pvGui);
  microSDL_tsElem*    pElem   = (microSDL_tsElem*)(pvElem);
  microSDL_tsXSelNum* pSelNum = (microSDL_tsXSelNum*)(pElem->pXData);
  int nCounter  = pSelNum->nCounter;

  if (eTouch == MSDL_TOUCH_UP_IN) {

    // Get currently selected sub-element ID
    int nSubElemId = pSelNum->nTrackSubIdStart;
    if (nSubElemId == SELNUM_ID_BTN_INC) {
      // Increment button
      if (nCounter<100) {
        nCounter++;
      }
      microSDL_ElemXSelNumSetCounter(pGui,pSelNum,nCounter);  

    } else if (nSubElemId == SELNUM_ID_BTN_DEC) {
      // Decrement button
      if (nCounter>0) {
        nCounter--;
      }
      microSDL_ElemXSelNumSetCounter(pGui,pSelNum,nCounter);

    }
  } // eTouch
  
  return true;
}



bool microSDL_ElemXSelNumTouch(void* pvGui,void* pvElem,microSDL_teTouch eTouch,int nRelX,int nRelY)
{
  if ((pvGui == NULL) || (pvElem == NULL)) {
    fprintf(stderr,"ERROR: ElemXSelNumTouch() called with NULL ptr\n");
    return false;
  }    
  microSDL_tsGui*           pGui = NULL;
  microSDL_tsElem*          pElem = NULL;
  microSDL_tsXSelNum*       pSelNum = NULL;
  
  // Callback function pointer for sub-element
  MSDL_CB_TOUCH   pfuncXTouch;
  
  // Typecast the parameters to match the GUI
  pGui  = (microSDL_tsGui*)(pvGui);
  pElem = (microSDL_tsElem*)(pvElem);
  pSelNum = (microSDL_tsXSelNum*)(pElem->pXData);  
  
  // Get absolute coordinate
  int nAbsX = pElem->rElem.x + nRelX;
  int nAbsY = pElem->rElem.y + nRelY;
  
  int   nCurTrackSubIndStart = pSelNum->nTrackSubIndStart;
  microSDL_tsElem* pSubElem = NULL;
  
  if (nCurTrackSubIndStart != MSDL_IND_NONE) {
    pSubElem = &(pSelNum->asElem[nCurTrackSubIndStart]);
  }
  
  int nSubInd;

  // Handle any main element operations
  switch(eTouch) {
    case MSDL_TOUCH_DOWN_IN:
    case MSDL_TOUCH_MOVE_IN:
      pElem->bGlowing = true;
      break;
    case MSDL_TOUCH_MOVE_OUT:
    case MSDL_TOUCH_UP_IN:
    case MSDL_TOUCH_UP_OUT:
    default:
      pElem->bGlowing = false;
      break;
  }
  
  // Handle any sub-element operations
  switch(eTouch) {
    
    case MSDL_TOUCH_DOWN_IN:
      // Determine the sub-element we are over
      for (nSubInd=0;nSubInd<pSelNum->nSubElemCnt;nSubInd++) {
        if (microSDL_IsInRect(pGui,nAbsX,nAbsY,pSelNum->asElem[nSubInd].rElem)) {
          pSelNum->nTrackSubIndStart = nSubInd;
          pSelNum->nTrackSubIdStart = pSelNum->asElem[nSubInd].nId;
          // Mark it as glowing
          pSelNum->asElem[nSubInd].bGlowing = true;
          pSelNum->asElem[nSubInd].bNeedRedraw = true;
        }
      }
      break;
      
    case MSDL_TOUCH_MOVE_IN:
      if (pSubElem == NULL) { break; }      
      if (microSDL_IsInRect(pGui,nAbsX,nAbsY,pSubElem->rElem)) {
        // Still over tracked item
        // Mark it as glowing
        pSubElem->bGlowing = true;
        pSubElem->bNeedRedraw = true;
      } else {
        // Not over tracked item
        // Disable glow
        pSubElem->bGlowing = false;
        pSubElem->bNeedRedraw = true;    
      }   
      break;
      
    case MSDL_TOUCH_MOVE_OUT:
    case MSDL_TOUCH_UP_OUT:
      if (pSubElem == NULL) { break; }   
      // We are even outside compound element, so disable glow
      pSubElem->bGlowing = false;
      pSubElem->bNeedRedraw = true;  
      break;
        
    case MSDL_TOUCH_UP_IN:
      // Touch up
      if (pSubElem == NULL) { break; }      
      pSubElem->bGlowing = false;
      pSubElem->bNeedRedraw = true;        
      if (microSDL_IsInRect(pGui,nAbsX,nAbsY,pSubElem->rElem)) {
        // Released over tracked element, treat as click event
        // Fetch the sub-element callback (if enabled)
        pfuncXTouch = pSubElem->pfuncXTouch;
        if (pfuncXTouch != NULL) {
          // Callback
          // Note that the callback function is passed a pointer
          // to the compound element, not the sub-element. This
          // allows the handler to interact with other sub-elements if required.
          (*pfuncXTouch)((void*)(pGui),(void*)(pElem),eTouch,nRelX,nRelY);
        }
        // Clear tracked sub-element
        pSelNum->nTrackSubIndStart = MSDL_IND_NONE;        
      } else {
        // Released outside tracked element, ignore
      }
      break;
      
    default:
      return false;
      break;
  }
  
  // To keep it simple, always redraw
  pElem->bNeedRedraw = true;
  
  return true;

}



// ============================================================================
