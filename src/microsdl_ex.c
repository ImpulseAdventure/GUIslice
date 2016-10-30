// =======================================================================
// microSDL library (extensions)
// - Calvin Hass
// - http:/www.impulseadventure.com/elec/microsdl-sdl-gui.html
//
// - Version 0.2.4    (2016/??/??)
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
// ============================================================================

// Create a gauge element and add it to the GUI element list
// - Defines default styling for the element
// - Defines callback for redraw but does not track touch/click
int microSDL_ElemXGaugeCreate(microSDL_tsGui* pGui,int nElemId,int nPage,
  microSDL_tsXGauge* pXData,SDL_Rect rElem,
  int nMin,int nMax,int nVal,SDL_Color colGauge,bool bVert)
{
  microSDL_tsElem sElem;
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
  bool bOk = microSDL_ElemAdd(pGui,sElem);
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
  pGauge->nGaugeVal = nVal;
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
  microSDL_tsXGauge* pXGauge;
  pXGauge = (microSDL_tsXGauge*)(pElem->pXData);
  if (pXGauge == NULL) {
    fprintf(stderr,"ERROR: microSDL_ElemXGaugeDraw() pXData is NULL\n");
    return false;
  }
  bool  bVert = pXGauge->bGaugeVert;
  int   nMax = pXGauge->nGaugeMax;
  int   nMin = pXGauge->nGaugeMin;
  int   nRng = pXGauge->nGaugeMax - pXGauge->nGaugeMin;

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
  int nLen = pXGauge->nGaugeVal * fScl;

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
  // TODO: Determine if we need to adjust by -1
  int nRectClipX1 = rGauge.x;
  int nRectClipX2 = rGauge.x+rGauge.w;
  int nRectClipY1 = rGauge.y;
  int nRectClipY2 = rGauge.y+rGauge.h;
  if (nRectClipX1 < nElemX)             { nRectClipX1 = nElemX;         }
  if (nRectClipX2 > nElemX+(int)nElemW) { nRectClipX2 = nElemX+nElemW;  }
  if (nRectClipY1 < nElemY)             { nRectClipY1 = nElemY;         }
  if (nRectClipY2 > nElemY+(int)nElemH) { nRectClipY2 = nElemY+nElemH;  }
  rGauge.x = nRectClipX1;
  rGauge.y = nRectClipY1;
  rGauge.w = nRectClipX2-nRectClipX1;
  rGauge.h = nRectClipY2-nRectClipY1;

  #ifdef DBG_LOG
  printf("Gauge: nMin=%4d nMax=%4d nRng=%d nVal=%4d fScl=%6.3f nGaugeMid=%4d RectX=%4d RectW=%4d\n",
    nMin,nMax,nRng,pXGauge->nGaugeVal,fScl,nGaugeMid,rGauge.x,rGauge.w);
  #endif

  // Draw the gauge fill region
  microSDL_FillRect(pGui,rGauge,pXGauge->colGauge);

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
  
  // Draw a frame around the gauge
  microSDL_FillRect(pGui,rMidLine,pElem->colElemFrame);

  return true;
}


// ============================================================================
// Extended Element: Checkbox
// ============================================================================

// Create a checkbox element and add it to the GUI element list
// - Defines default styling for the element
// - Defines callback for redraw but does not track touch/click
int microSDL_ElemXCheckboxCreate(microSDL_tsGui* pGui,int nElemId,int nPage,
  microSDL_tsXCheckbox* pXData,SDL_Rect rElem,SDL_Color colCheck,bool bChecked)
{
  microSDL_tsElem sElem;
  if (pXData == NULL) { return MSDL_ID_NONE; }
  sElem = microSDL_ElemCreate(pGui,nElemId,nPage,MSDL_TYPEX_CHECKBOX,rElem,NULL,MSDL_FONT_NONE);
  sElem.bFrameEn        = true;
  sElem.bFillEn         = true;
  sElem.bClickEn        = true;
  pXData->bChecked      = bChecked;
  pXData->colCheck      = colCheck;
  sElem.pXData          = (void*)(pXData);
  sElem.pfuncXDraw      = &microSDL_ElemXCheckboxDraw;
  sElem.pfuncXTouch     = NULL;           // No need to track touches
  sElem.colElemFrame    = MSDL_COL_GRAY;
  sElem.colElemFill     = MSDL_COL_BLACK;
  bool bOk = microSDL_ElemAdd(pGui,sElem);
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
  pCheckbox->bChecked = bChecked;
  
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
  microSDL_tsXCheckbox* pXCheckbox;
  pXCheckbox = (microSDL_tsXCheckbox*)(pElem->pXData);
  if (pXCheckbox == NULL) {
    fprintf(stderr,"ERROR: microSDL_ElemXCheckboxDraw() pXData is NULL\n");
    return false;
  }
  
  bool  bChecked = pXCheckbox->bChecked;
  rInner = microSDL_ExpandRect(pElem->rElem,-5,-5);
  if (bChecked) {
    // If checked, fill in the inner region
    microSDL_FillRect(pGui,rInner,pXCheckbox->colCheck);
  } else {
    // Assume the background fill has already been done so
    // we don't need to do anything more in the unchecked case
  }
  
  // Draw a frame around the checkbox
  microSDL_FrameRect(pGui,pElem->rElem,pElem->colElemFrame);

  return true;
}


// ============================================================================
