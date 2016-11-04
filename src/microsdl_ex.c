// =======================================================================
// microSDL library (extensions)
// - Calvin Hass
// - http:/www.impulseadventure.com/elec/microsdl-sdl-gui.html
//
// - Version 0.3.3    (2016/11/04)
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
int microSDL_ElemXGaugeCreate(microSDL_tsGui* pGui,int nElemId,int nPage,
  microSDL_tsXGauge* pXData,SDL_Rect rElem,
  int nMin,int nMax,int nVal,SDL_Color colGauge,bool bVert)
{
  microSDL_tsElem sElem;
  bool            bOk = true;
  if (pXData == NULL) { return MSDL_ID_NONE; }
  sElem = microSDL_ElemCreate(pGui,nElemId,nPage,MSDL_TYPEX_GAUGE,rElem,NULL,MSDL_FONT_NONE);
  sElem.bFrameEn        = true;
  sElem.bFillEn         = true;
  sElem.bClickEn        = false;          // Element is not "clickable"
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
    bOk = microSDL_ElemAdd(pGui,sElem);
  } else {
    // Save as temporary element
    pGui->psElem[MSDL_IND_TEMP] = sElem;
  }
  return (bOk)? sElem.nId : MSDL_ID_NONE;
}

// Update the gauge control's current position
void microSDL_ElemXGaugeUpdate(microSDL_tsGui* pGui,int nElemId,int nVal)
{
  // Fetch the control data elements
  int nElemInd = microSDL_ElemFindIndFromId(pGui,nElemId);
  if (!microSDL_ElemIndValid(pGui,nElemInd)) {
    fprintf(stderr,"ERROR: microSDL_ElemXGaugeUpdate() invalid ID=%d\n",nElemInd);
    return;
  }
  microSDL_tsElem* pElem = &pGui->psElem[nElemInd];
  microSDL_tsXGauge*  pGauge = (microSDL_tsXGauge*)(pElem->pXData);
  
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
    fprintf(stderr,"ERROR: microSDL_ElemXGaugeDraw() pXData is NULL\n");
    return false;
  }
  
  // Draw the background
  microSDL_FillRect(pGui,pElem->rElem,pElem->colElemFill);
  
  
  bool  bVert = pGauge->bGaugeVert;
  int   nMax = pGauge->nGaugeMax;
  int   nMin = pGauge->nGaugeMin;
  int   nRng = pGauge->nGaugeMax - pGauge->nGaugeMin;

  if (nRng == 0) {
    fprintf(stderr,"ERROR: microSDL_ElemXGaugeDraw() Zero gauge range [%d,%d]\n",nMin,nMax);
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
    fprintf(stderr,"ERROR: microSDL_ElemXGaugeDraw() Unsupported gauge range [%d,%d]\n",nMin,nMax);
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
int microSDL_ElemXCheckboxCreate(microSDL_tsGui* pGui,int nElemId,int nPage,
  microSDL_tsXCheckbox* pXData,SDL_Rect rElem,SDL_Color colCheck,bool bChecked)
{
  microSDL_tsElem sElem;
  bool            bOk = true;
  if (pXData == NULL) { return MSDL_ID_NONE; }
  sElem = microSDL_ElemCreate(pGui,nElemId,nPage,MSDL_TYPEX_CHECKBOX,rElem,NULL,MSDL_FONT_NONE);
  sElem.bFrameEn        = false;
  sElem.bFillEn         = true;
  sElem.bClickEn        = true;
  sElem.bGlowing        = false;
  pXData->bChecked      = bChecked;
  pXData->colCheck      = colCheck;  
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
    bOk = microSDL_ElemAdd(pGui,sElem);
  } else {
    // Save as temporary element
    pGui->psElem[MSDL_IND_TEMP] = sElem;
  }
  return (bOk)? sElem.nId : MSDL_ID_NONE;
}

bool microSDL_ElemXCheckboxGetState(microSDL_tsGui* pGui,int nElemId)
{
 // Fetch the control data elements
  int nElemInd = microSDL_ElemFindIndFromId(pGui,nElemId);
  if (!microSDL_ElemIndValid(pGui,nElemInd)) {
    fprintf(stderr,"ERROR: microSDL_ElemXCheckboxUpdate() invalid ID=%d\n",nElemInd);
    return false;
  }
  microSDL_tsElem* pElem = &pGui->psElem[nElemInd];
  microSDL_tsXCheckbox*  pCheckbox = (microSDL_tsXCheckbox*)(pElem->pXData);
  
  return pCheckbox->bChecked;
}

// Update the checkbox control's current state
void microSDL_ElemXCheckboxSetState(microSDL_tsGui* pGui,int nElemId,bool bChecked)
{
  // Fetch the control data elements
  int nElemInd = microSDL_ElemFindIndFromId(pGui,nElemId);
  if (!microSDL_ElemIndValid(pGui,nElemInd)) {
    fprintf(stderr,"ERROR: microSDL_ElemXCheckboxUpdate() invalid ID=%d\n",nElemInd);
    return;
  }
  microSDL_tsElem* pElem = &pGui->psElem[nElemInd];
  microSDL_tsXCheckbox*  pCheckbox = (microSDL_tsXCheckbox*)(pElem->pXData);
   
  // Update the data element
  bool  bCheckedOld = pCheckbox->bChecked;
  pCheckbox->bChecked = bChecked;

  // Element needs redraw
  if (bChecked != bCheckedOld) {
    pElem->bNeedRedraw = true;
  }

}

// Toggle the checkbox control's state
void microSDL_ElemXCheckboxToggleState(microSDL_tsGui* pGui,int nElemId)
{
  // Fetch the control data elements
  int nElemInd = microSDL_ElemFindIndFromId(pGui,nElemId);
  if (!microSDL_ElemIndValid(pGui,nElemInd)) {
    fprintf(stderr,"ERROR: microSDL_ElemXCheckboxToggleState() invalid ID=%d\n",nElemInd);
    return;
  }
  microSDL_tsElem* pElem = &pGui->psElem[nElemInd];
  microSDL_tsXCheckbox*  pCheckbox = (microSDL_tsXCheckbox*)(pElem->pXData);
   
  // Update the data element
  bool bCheckNew = (pCheckbox->bChecked)?false:true;
  pCheckbox->bChecked = bCheckNew;

  // Element needs redraw
  pElem->bNeedRedraw = true;
  
}

// Redraw the checkbox
// - Note that this redraw is for the entire element rect region
// - The Draw function parameters use void pointers to allow for
//   simpler callback function definition & scalability.
bool microSDL_ElemXCheckboxDraw(void* pvGui,void* pvElem)
{
  // Typecast the parameters to match the GUI and element types
  microSDL_tsGui*   pGui  = (microSDL_tsGui*)(pvGui);
  microSDL_tsElem*  pElem = (microSDL_tsElem*)(pvElem);
  
  SDL_Rect    rInner;
  
  // Fetch the element's extended data structure
  microSDL_tsXCheckbox* pCheckbox;
  pCheckbox = (microSDL_tsXCheckbox*)(pElem->pXData);
  if (pCheckbox == NULL) {
    fprintf(stderr,"ERROR: microSDL_ElemXCheckboxDraw() pXData is NULL\n");
    return false;
  }
  
  bool  bChecked = pCheckbox->bChecked;
  bool  bGlowing = pElem->bGlowing;
  
  // Draw the background
  microSDL_FillRect(pGui,pElem->rElem,pElem->colElemFill);

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
// - Without the callback, we would need to depend on the default
//   click event (nTrackElemIndClicked) and the main loop would
//   therefore be responsible for calling the toggle function and
//   redraw. For example:
//     if (nTrackElemIndClicked == E_ELEM_CHECK) {
//       microSDL_ElemXCheckboxToggleState(&m_gui,E_ELEM_CHECK);
//       microSDL_ElemDraw(&m_gui,E_ELEM_CHECK);
//     }
//   
bool microSDL_ElemXCheckboxTouch(void* pvGui,microSDL_teTouch eTouch,int nRelX,int nRelY)
{
  microSDL_tsGui*           pGui = NULL;
  microSDL_tsElem*          pElem = NULL;
  microSDL_tsXCheckbox*     pCheckbox = NULL;
  
  // Typecast the parameters to match the GUI
  pGui  = (microSDL_tsGui*)(pvGui);
  
  // Get our element (as it will be the one being tracked)
  int nElemId = pGui->nTrackElemIdStart;
  int nElemInd = microSDL_ElemFindIndFromId(pGui,nElemId);
  if (nElemInd == MSDL_IND_NONE) {
    // Shouldn't get here
    return false;
  }
  
  
  pElem = &(pGui->psElem[nElemInd]);
  pCheckbox = (microSDL_tsXCheckbox*)(pElem->pXData);  

  bool  bCheckedOld = pCheckbox->bChecked;
  bool  bGlowingOld = pElem->bGlowing;  
  
  switch(eTouch) {
    
    case MSDL_TOUCH_DOWN:
      // Start glowing as must be over it
      pElem->bGlowing = true;
      break;
      
    case MSDL_TOUCH_MOVE:
      // Glow only if current coordinate is within element
      if (microSDL_IsInWH(pGui,nRelX,nRelY,pElem->rElem.w,pElem->rElem.h)) {
        pElem->bGlowing = true;
      } else {
        pElem->bGlowing = false;
      }
      break;
      
    case MSDL_TOUCH_UP:
      // End glow
      pElem->bGlowing = false;
      // Accept "clicked" event if current coordinate is within element
      if (microSDL_IsInWH(pGui,nRelX,nRelY,pElem->rElem.w,pElem->rElem.h)) {
        // Toggle the state
        bool bCheckNew = (pCheckbox->bChecked)?false:true;
        pCheckbox->bChecked = bCheckNew;        
      }
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
int microSDL_ElemXSliderCreate(microSDL_tsGui* pGui,int nElemId,int nPage,
  microSDL_tsXSlider* pXData,SDL_Rect rElem,int nPosMin,int nPosMax,int nPos,
  unsigned nThumbSz,bool bVert)
{
  microSDL_tsElem sElem;
  bool            bOk = true;
  if (pXData == NULL) { return MSDL_ID_NONE; }
  sElem = microSDL_ElemCreate(pGui,nElemId,nPage,MSDL_TYPEX_SLIDER,rElem,NULL,MSDL_FONT_NONE);
  sElem.bFrameEn        = false;
  sElem.bFillEn         = true;
  sElem.bClickEn        = true;
  sElem.bGlowing        = false;
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
    bOk = microSDL_ElemAdd(pGui,sElem);
  } else {
    // Save as temporary element
    pGui->psElem[MSDL_IND_TEMP] = sElem;
  }
  return (bOk)? sElem.nId : MSDL_ID_NONE;
}

void microSDL_ElemXSliderSetStyle(microSDL_tsGui* pGui,int nElemId,
        bool bTrim,SDL_Color colTrim,unsigned nTickDiv,
        int nTickLen,SDL_Color colTick)
{
  // Fetch the control data elements
  int nElemInd = microSDL_ElemFindIndFromId(pGui,nElemId);
  if (!microSDL_ElemIndValid(pGui,nElemInd)) {
    fprintf(stderr,"ERROR: microSDL_ElemXSliderSetStyle() invalid ID=%d\n",nElemInd);
    return;
  }
  microSDL_tsElem* pElem = &pGui->psElem[nElemInd];
  microSDL_tsXSlider*  pSlider = (microSDL_tsXSlider*)(pElem->pXData);

  pSlider->bTrim      = bTrim;
  pSlider->colTrim    = colTrim;
  pSlider->nTickDiv   = nTickDiv;
  pSlider->nTickLen   = nTickLen;
  pSlider->colTick    = colTick;
  
  // Update
  pElem->bNeedRedraw      = true;
}

int microSDL_ElemXSliderGetPos(microSDL_tsGui* pGui,int nElemId)
{
 // Fetch the control data elements
  int nElemInd = microSDL_ElemFindIndFromId(pGui,nElemId);
  if (!microSDL_ElemIndValid(pGui,nElemInd)) {
    fprintf(stderr,"ERROR: microSDL_ElemXSliderGetPos() invalid ID=%d\n",nElemInd);
    return false;
  }
  microSDL_tsElem* pElem = &pGui->psElem[nElemInd];
  microSDL_tsXSlider*  pSlider = (microSDL_tsXSlider*)(pElem->pXData);
  
  return pSlider->nPos;
}

// Update the slider control's current state
void microSDL_ElemXSliderSetPos(microSDL_tsGui* pGui,int nElemId,int nPos)
{
  // Fetch the control data elements
  int nElemInd = microSDL_ElemFindIndFromId(pGui,nElemId);
  if (!microSDL_ElemIndValid(pGui,nElemInd)) {
    fprintf(stderr,"ERROR: microSDL_ElemXSliderSetPos() invalid ID=%d\n",nElemInd);
    return;
  }
  microSDL_tsElem*      pElem = &pGui->psElem[nElemInd];
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
  // Typecast the parameters to match the GUI and element types
  microSDL_tsGui*   pGui  = (microSDL_tsGui*)(pvGui);
  microSDL_tsElem*  pElem = (microSDL_tsElem*)(pvElem);
  
  // Fetch the element's extended data structure
  microSDL_tsXSlider* pSlider;
  pSlider = (microSDL_tsXSlider*)(pElem->pXData);
  if (pSlider == NULL) {
    fprintf(stderr,"ERROR: microSDL_ElemXSliderDraw() pXData is NULL\n");
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
    fprintf(stderr,"ERROR: microSDL_ElemXSliderDraw() bVert=true not supported yet\n");
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
bool microSDL_ElemXSliderTouch(void* pvGui,microSDL_teTouch eTouch,int nRelX,int nRelY)
{
  microSDL_tsGui*           pGui = NULL;
  microSDL_tsElem*          pElem = NULL;
  microSDL_tsXSlider*       pSlider = NULL;
  
  // Typecast the parameters to match the GUI
  pGui  = (microSDL_tsGui*)(pvGui);
  
  // Get our element (as it will be the one being tracked)
  int nElemId = pGui->nTrackElemIdStart;
  int nElemInd = microSDL_ElemFindIndFromId(pGui,nElemId);  
  if (nElemInd == MSDL_IND_NONE) {
    // Shouldn't get here
    return false;
  }
  
  pElem = &(pGui->psElem[nElemInd]);
  pSlider = (microSDL_tsXSlider*)(pElem->pXData);  

  bool  bGlowingOld = pElem->bGlowing;  
  int   nPosRng;
  int   nPos;
  
  switch(eTouch) {
    
    case MSDL_TOUCH_DOWN:
      // Start glowing as must be over it
      pElem->bGlowing = true;
      // Calc new position
      nPosRng = pSlider->nPosMax - pSlider->nPosMin;
      nPos = (nRelX * nPosRng / pElem->rElem.w) + pSlider->nPosMin;
      // Clip position
      if (nPos < pSlider->nPosMin) { nPos = pSlider->nPosMin; }
      if (nPos > pSlider->nPosMax) { nPos = pSlider->nPosMax; }     
      // Update
      pSlider->nPos = nPos;
      pElem->bNeedRedraw = true;
      break;
      
    case MSDL_TOUCH_MOVE:
      // Glow only if current coordinate is within element
      if (microSDL_IsInWH(pGui,nRelX,nRelY,pElem->rElem.w,pElem->rElem.h)) {
        pElem->bGlowing = true;
      } else {
        pElem->bGlowing = false;
      }
      // Calc new position
      nPosRng = pSlider->nPosMax - pSlider->nPosMin;
      nPos = (nRelX * nPosRng / pElem->rElem.w) + pSlider->nPosMin;
      // Clip position
      if (nPos < pSlider->nPosMin) { nPos = pSlider->nPosMin; }
      if (nPos > pSlider->nPosMax) { nPos = pSlider->nPosMax; }     
      // Update
      pSlider->nPos = nPos;
      pElem->bNeedRedraw = true;
      break;
      
    case MSDL_TOUCH_UP:
      // End glow
      pElem->bGlowing = false;
      // Accept "clicked" event if current coordinate is within element
      if (microSDL_IsInWH(pGui,nRelX,nRelY,pElem->rElem.w,pElem->rElem.h)) {
        // TODO: Anything to do when release? Perhaps issue callback?
      } else {
        // TODO: Maybe reset position of slider if released outside?        
      }
      pElem->bNeedRedraw = true;      
      break;
      
    default:
      return false;
      break;
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
//
// - Internal hardcoded sub-element IDs:
//   - 100 : Button (increment)
//   - 101 : Button (decrement)
//   - 102 : Counter text
// ============================================================================


// Create a compound element
// - For now just two buttons
int microSDL_ElemXSelNumCreate(microSDL_tsGui* pGui,int nElemId,int nPage,
  microSDL_tsXSelNum* pXData,SDL_Rect rElem,int nFontId)
{
  microSDL_tsElem sElem;
  int             nSubElemId;
  int             nSubInd;
  bool            bOk = true;
  if (pXData == NULL) { return MSDL_ID_NONE; }
  
  // Determine relative coordinate offset
  int   nOffsetX = rElem.x;
  int   nOffsetY = rElem.y;
  
  // Initialize composite element
  sElem = microSDL_ElemCreate(pGui,nElemId,nPage,MSDL_TYPEX_SELNUM,rElem,NULL,MSDL_FONT_NONE);
  sElem.bFrameEn        = true;
  sElem.bFillEn         = true;
  sElem.bClickEn        = true;
  sElem.bGlowing        = false;

  pXData->nCounter            = 0;
  
  pXData->nSubElemCnt         = 0;
  pXData->nTrackSubIdStart    = MSDL_ID_NONE;
  pXData->nTrackSubIndStart   = MSDL_IND_NONE;
  pXData->nTrackSubIdClicked  = MSDL_ID_NONE;

  // Clear the sub-elements
  for (nSubInd=0;nSubInd<3;nSubInd++) {
    pXData->anElemId[nSubInd] = MSDL_ID_NONE;
    microSDL_ResetElem(&(pXData->asElem[nSubInd]));
  }
  
  sElem.pXData          = (void*)(pXData);
  // Specify the custom drawing callback
  sElem.pfuncXDraw      = &microSDL_ElemXSelNumDraw;
  // Specify the custom touch tracking callback
  sElem.pfuncXTouch     = &microSDL_ElemXSelNumTouch;
  sElem.colElemFrame    = MSDL_COL_GRAY;
  sElem.colElemFill     = MSDL_COL_BLACK;
  sElem.colElemGlow     = MSDL_COL_WHITE; 

  int nSubElemCnt = 0;

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
  // - For demonstration purposes, the sub-element IDs have
  //   been hardcoded without #defines.
  
  nSubElemId = microSDL_ElemCreateBtnTxt(pGui,100,MSDL_PAGE_NONE,
    (SDL_Rect){nOffsetX+40,nOffsetY+10,30,30},"+",nFontId);
  microSDL_ElemSetCol(pGui,MSDL_ID_TEMP,(SDL_Color){0,0,192},(SDL_Color){0,0,128},(SDL_Color){0,0,224}); 
  microSDL_ElemSetTxtCol(pGui,MSDL_ID_TEMP,MSDL_COL_WHITE);  
  pXData->anElemId[nSubElemCnt] = nSubElemId;
  pXData->asElem[nSubElemCnt++] = microSDL_ElemGetTemp(pGui);
  
  nSubElemId = microSDL_ElemCreateBtnTxt(pGui,101,MSDL_PAGE_NONE,
    (SDL_Rect){nOffsetX+80,nOffsetY+10,30,30},"-",nFontId);
  microSDL_ElemSetCol(pGui,MSDL_ID_TEMP,(SDL_Color){0,0,192},(SDL_Color){0,0,128},(SDL_Color){0,0,224}); 
  microSDL_ElemSetTxtCol(pGui,MSDL_ID_TEMP,MSDL_COL_WHITE);  
  pXData->anElemId[nSubElemCnt] = nSubElemId;
  pXData->asElem[nSubElemCnt++] = microSDL_ElemGetTemp(pGui);
  
  nSubElemId = microSDL_ElemCreateTxt(pGui,102,MSDL_PAGE_NONE,
    (SDL_Rect){nOffsetX+10,nOffsetY+10,20,30},"",nFontId);
  pXData->anElemId[nSubElemCnt] = nSubElemId;
  pXData->asElem[nSubElemCnt++] = microSDL_ElemGetTemp(pGui);
  
  // Record the number of sub-elements
  pXData->nSubElemCnt = nSubElemCnt;

  // Now proceed to add to page
  if (nPage != MSDL_PAGE_NONE) {  
    bOk = microSDL_ElemAdd(pGui,sElem);
  } else {
    // Save as temporary element
    pGui->psElem[MSDL_IND_TEMP] = sElem;
  }
  return (bOk)? sElem.nId : MSDL_ID_NONE;
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
  // Typecast the parameters to match the GUI and element types
  microSDL_tsGui*   pGui  = (microSDL_tsGui*)(pvGui);
  microSDL_tsElem*  pElem = (microSDL_tsElem*)(pvElem);
  int               nSubInd;
  
  // Fetch the element's extended data structure
  microSDL_tsXSelNum* pSelNum;
  pSelNum = (microSDL_tsXSelNum*)(pElem->pXData);
  if (pSelNum == NULL) {
    fprintf(stderr,"ERROR: microSDL_ElemXSelNumDraw() pXData is NULL\n");
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
  if (pSelNum == NULL) {
    return 0;
  }
  return pSelNum->nCounter;
}

int microSDL_ElemXSelNumFindIndById(microSDL_tsXSelNum* pSelNum,int nElemId)
{
  unsigned  nSubInd;
  int       nFound = MSDL_IND_NONE;
  for (nSubInd=0;nSubInd<pSelNum->nSubElemCnt;nSubInd++) {
    if (pSelNum->asElem[nSubInd].nId == nElemId) {
      nFound = nSubInd;
    }
  }
  return nFound;  
}

// Handle the compound element main functionality
// - This routine is called by microSDL_ElemXSelNumTouch() to handle
//   any click events that resulted from the touch tracking process.
// - The code here will generally represent the core
//   functionality of the compound element and any communication
//   between sub-elements.
void microSDL_ElemXSelNumClick(microSDL_tsGui* pGui,microSDL_tsXSelNum* pSelNum)
{
  if (pSelNum == NULL) {
    return;
  }
  int nElemIdClicked = pSelNum->nTrackSubIdClicked;
  int nCounter = pSelNum->nCounter;
  
  // Get sub-element index of main elements
  //int nBtnIncId = 100;
  //int nBtnDecId = 101;
  //int nTxtId = 102;
  //int nBtnIncInd = microSDL_ElemXSelNumFindIndById(pSelNum,nBtnIncId);
  //int nBtnDecInd = microSDL_ElemXSelNumFindIndById(pSelNum,nBtnDecId);
  //int nTxtInd = microSDL_ElemXSelNumFindIndById(pSelNum,nTxtId);
  
  switch (nElemIdClicked) {
    
    // Increment Button
    case 100:
      // Increment
      if (nCounter<100) {
        nCounter++;
      }
      pSelNum->nCounter = nCounter;
      // TODO: Need to replace the hardcoded sub-element index
      //       with a lookup from the sub-element ID (eg. nBtnIncInd)
      sprintf(pSelNum->asElem[2].acStr,"%d",nCounter);
      pSelNum->asElem[2].bNeedRedraw = true;      
      break;
      
    // Decrement Button
    case 101:
      // Decrement
      if (nCounter>0) {
        nCounter--;
      }
      pSelNum->nCounter = nCounter;      
      // TODO: Need to replace the hardcoded sub-element index
      //       with a lookup from the sub-element ID  (eg. nBtnDecInd)
      sprintf(pSelNum->asElem[2].acStr,"%d",nCounter);
      pSelNum->asElem[2].bNeedRedraw = true;
      break;
      
    default:
      break;
  }
  // Reset the clicked indicator
  pSelNum->nTrackSubIdClicked = MSDL_ID_NONE;
}

// Touch event callback for a compound element
// - We redirect the touch events to the appropriate sub-element
bool microSDL_ElemXSelNumTouch(void* pvGui,microSDL_teTouch eTouch,int nRelX,int nRelY)
{
  microSDL_tsGui*     pGui = NULL;
  microSDL_tsElem*    pElem = NULL;
  microSDL_tsXSelNum* pSelNum = NULL;
  
  int                 nSubInd;
  
  // Typecast the parameters to match the GUI
  pGui  = (microSDL_tsGui*)(pvGui);
  
  // Get our element (as it will be the one being tracked)
  int nElemId = pGui->nTrackElemIdStart;
  int nElemInd = microSDL_ElemFindIndFromId(pGui,nElemId);
  if (nElemInd == MSDL_IND_NONE) {
    // Shouldn't get here
    return false;
  }
  
  
  pElem = &(pGui->psElem[nElemInd]);
  pSelNum = (microSDL_tsXSelNum*)(pElem->pXData);  

  // Get absolute coordinate
  int nAbsX = pElem->rElem.x + nRelX;
  int nAbsY = pElem->rElem.y + nRelY;
  
  int   nCurTrackSubIndStart = pSelNum->nTrackSubIndStart;
  switch(eTouch) {
    
    case MSDL_TOUCH_DOWN:
      // Determine what sub-element we just touched on
      for (nSubInd=0;nSubInd<pSelNum->nSubElemCnt;nSubInd++) {
        if (microSDL_IsInRect(pGui,nAbsX,nAbsY,pSelNum->asElem[nSubInd].rElem)) {
          pSelNum->nTrackSubIndStart = nSubInd;
          // Mark it as glowing
          pSelNum->asElem[nSubInd].bGlowing = true;
          pSelNum->asElem[nSubInd].bNeedRedraw = true;
        }
      }
      break;
      
    case MSDL_TOUCH_MOVE:
      if (nCurTrackSubIndStart == MSDL_IND_NONE) {
        break;
      }
      // Continue to glow if still over tracked element
      // Glow only if current coordinate is within element
      if (microSDL_IsInRect(pGui,nAbsX,nAbsY,pSelNum->asElem[nCurTrackSubIndStart].rElem)) {
        // Still over tracked item
        // Mark it as glowing
        pSelNum->asElem[nCurTrackSubIndStart].bGlowing = true;
        pSelNum->asElem[nCurTrackSubIndStart].bNeedRedraw = true;
      } else {
        // Not over tracked item
        // Disable glow
        pSelNum->asElem[nCurTrackSubIndStart].bGlowing = false;
        pSelNum->asElem[nCurTrackSubIndStart].bNeedRedraw = true;
      }
      break;
      
    case MSDL_TOUCH_UP:
      if (nCurTrackSubIndStart == MSDL_IND_NONE) {
        break;
      }
      // End glow
      pSelNum->asElem[nCurTrackSubIndStart].bGlowing = false;
      pSelNum->asElem[nCurTrackSubIndStart].bNeedRedraw = true;
      // Accept "clicked" event if current coordinate is within element
      if (microSDL_IsInRect(pGui,nAbsX,nAbsY,pSelNum->asElem[nCurTrackSubIndStart].rElem)) {
        // Touch-up within tracked sub-element
        pSelNum->nTrackSubIdClicked = pSelNum->asElem[nCurTrackSubIndStart].nId;
        // Clear the tracking
        pSelNum->nTrackSubIndStart = MSDL_IND_NONE;
        // Handle click event for sub-element
        microSDL_ElemXSelNumClick(pGui,pSelNum);
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
