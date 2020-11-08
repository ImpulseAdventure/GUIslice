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
/// \file XSelNum.c



// GUIslice library
#include "GUIslice.h"
#include "GUIslice_drv.h"

#include "elem/XSelNum.h"

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


#if (GSLC_FEATURE_COMPOUND)
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
gslc_tsElemRef* gslc_ElemXSelNumCreate(gslc_tsGui* pGui,int16_t nElemId,int16_t nPage,
  gslc_tsXSelNum* pXData,gslc_tsRect rElem,int8_t nFontId)
{

  if ((pGui == NULL) || (pXData == NULL)) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXSelNumCreate";
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return NULL;
  }
  gslc_tsElem sElem;


  // Initialize composite element
  sElem = gslc_ElemCreate(pGui,nElemId,nPage,GSLC_TYPEX_SELNUM,rElem,NULL,0,GSLC_FONT_NONE);
  sElem.nFeatures        |= GSLC_ELEM_FEA_FRAME_EN;
  sElem.nFeatures        |= GSLC_ELEM_FEA_FILL_EN;
  sElem.nFeatures        |= GSLC_ELEM_FEA_CLICK_EN;
  sElem.nFeatures        &= ~GSLC_ELEM_FEA_GLOW_EN;  // Don't need to glow outer element
  sElem.nGroup            = GSLC_GROUP_ID_NONE;

  pXData->nCounter      = 0;

  // Determine the maximum number of elements that we can store
  // in the sub-element array. We do this at run-time with sizeof()
  // instead of using #define to avoid polluting the global namespace.
  int16_t nSubElemMax = sizeof(pXData->asElem) / sizeof(pXData->asElem[0]);

  // NOTE: The count parameters in CollectReset() must match the size of
  //       the asElem[] array. It is used for bounds checking when we
  //       add new elements.
  // NOTE: We only use RAM for subelement storage
  gslc_CollectReset(&pXData->sCollect,pXData->asElem,nSubElemMax,pXData->asElemRef,nSubElemMax);


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
  gslc_tsElemRef* pElemRefTmp   = NULL;
  gslc_tsElem*    pElemTmp      = NULL;
  gslc_tsElemRef* pElemRef      = NULL;

  // Determine offset coordinate of compound element so that we can
  // specify relative positioning during the sub-element Create() operations.
  int16_t nOffsetX = rElem.x;
  int16_t nOffsetY = rElem.y;

  gslc_tsRect rSubElem;

  rSubElem = (gslc_tsRect) { nOffsetX+40,nOffsetY+10,30,30 };
  rSubElem = gslc_ExpandRect(rSubElem, -1, -1);
  #if (GSLC_LOCAL_STR)
  pElemRefTmp = gslc_ElemCreateBtnTxt(pGui,SELNUM_ID_BTN_INC,GSLC_PAGE_NONE,
    rSubElem,"+",0,nFontId,&gslc_ElemXSelNumClick);
  #else
  gslc_StrCopy(pXData->acElemTxt[0],"+",SELNUM_STR_LEN);
  pElemRefTmp = gslc_ElemCreateBtnTxt(pGui,SELNUM_ID_BTN_INC,GSLC_PAGE_NONE,
    rSubElem,pXData->acElemTxt[0],SELNUM_STR_LEN,
    nFontId,&gslc_ElemXSelNumClick);
  #endif
  gslc_ElemSetCol(pGui,pElemRefTmp,(gslc_tsColor){0,0,192},(gslc_tsColor){0,0,128},(gslc_tsColor){0,0,224});
  gslc_ElemSetTxtCol(pGui,pElemRefTmp,GSLC_COL_WHITE);
  pElemTmp = gslc_GetElemFromRef(pGui,pElemRefTmp);
  gslc_CollectElemAdd(pGui,&pXData->sCollect,pElemTmp,GSLC_ELEMREF_DEFAULT);

  rSubElem = (gslc_tsRect) { nOffsetX+80,nOffsetY+10,30,30 };
  rSubElem = gslc_ExpandRect(rSubElem, -1, -1);
  #if (GSLC_LOCAL_STR)
  pElemRefTmp = gslc_ElemCreateBtnTxt(pGui,SELNUM_ID_BTN_DEC,GSLC_PAGE_NONE,
    rSubElem,"-",0,nFontId,&gslc_ElemXSelNumClick);
  #else
  gslc_StrCopy(pXData->acElemTxt[1],"-",SELNUM_STR_LEN);
  pElemRefTmp = gslc_ElemCreateBtnTxt(pGui,SELNUM_ID_BTN_DEC,GSLC_PAGE_NONE,
    rSubElem,pXData->acElemTxt[1],SELNUM_STR_LEN,
    nFontId,&gslc_ElemXSelNumClick);
  #endif
  gslc_ElemSetCol(pGui,pElemRefTmp,(gslc_tsColor){0,0,192},(gslc_tsColor){0,0,128},(gslc_tsColor){0,0,224});
  gslc_ElemSetTxtCol(pGui,pElemRefTmp,GSLC_COL_WHITE);
  pElemTmp = gslc_GetElemFromRef(pGui,pElemRefTmp);
  gslc_CollectElemAdd(pGui,&pXData->sCollect,pElemTmp,GSLC_ELEMREF_DEFAULT);

  rSubElem = (gslc_tsRect) { nOffsetX+10,nOffsetY+10,20,30 };
  rSubElem = gslc_ExpandRect(rSubElem, -1, -1);
  #if (GSLC_LOCAL_STR)
  pElemRefTmp = gslc_ElemCreateTxt(pGui,SELNUM_ID_TXT,GSLC_PAGE_NONE,
    rSubElem,"0",0,nFontId);
  #else
  gslc_StrCopy(pXData->acElemTxt[2],"0",SELNUM_STR_LEN);
  pElemRefTmp = gslc_ElemCreateTxt(pGui,SELNUM_ID_TXT,GSLC_PAGE_NONE,
    rSubElem,pXData->acElemTxt[2],SELNUM_STR_LEN,nFontId);
  #endif
  pElemTmp = gslc_GetElemFromRef(pGui,pElemRefTmp);
  gslc_CollectElemAdd(pGui,&pXData->sCollect,pElemTmp,GSLC_ELEMREF_DEFAULT);


  // Now proceed to add the compound element to the page
  if (nPage != GSLC_PAGE_NONE) {
    pElemRef = gslc_ElemAdd(pGui,nPage,&sElem,GSLC_ELEMREF_DEFAULT);

    // Now propagate the parent relationship to enable a cascade
    // of redrawing from low-level elements to the top
    gslc_CollectSetParent(pGui,&pXData->sCollect,pElemRef);

    return pElemRef;
  } else {
    GSLC_DEBUG2_PRINT("ERROR: ElemXSelNumCreate(%s) Compound elements inside compound elements not supported\n","");
    return NULL;

    // TODO: For now, disable compound elements within
    // compound elements. If we want to enable this, we
    // would probably use the temporary element reference
    // the GUI.
    // Save as temporary element for further processing
    //pGui->sElemTmp = sElem;   // Need fixing
    //return &(pGui->sElemTmp); // Need fixing
  }

}


// Redraw the compound element
// - When drawing a compound element, we clear the background
//   and then redraw the sub-element collection.
bool gslc_ElemXSelNumDraw(void* pvGui,void* pvElemRef,gslc_teRedrawType eRedraw)
{
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem* pElem = gslc_GetElemFromRef(pGui,pElemRef);
  gslc_tsXSelNum* pSelNum = (gslc_tsXSelNum*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_SELNUM, __LINE__);
  if (!pSelNum) return false;

  bool bGlow = (pElem->nFeatures & GSLC_ELEM_FEA_GLOW_EN) && gslc_ElemGetGlow(pGui,pElemRef);

  // Draw the compound element fill (background)
  // - Should only need to do this in full redraw
  if (eRedraw == GSLC_REDRAW_FULL) {
    gslc_DrawFillRect(pGui,pElem->rElem,(bGlow)?pElem->colElemFillGlow:pElem->colElemFill);
  }

  // Draw the sub-elements
  // - For now, force redraw of entire compound element
  gslc_tsCollect* pCollect = &pSelNum->sCollect;

  gslc_tsEvent  sEvent = gslc_EventCreate(pGui,GSLC_EVT_DRAW,GSLC_EVTSUB_DRAW_FORCE,(void*)(pCollect),NULL);
  gslc_CollectEvent(pGui,sEvent);

  // Optionally, draw a frame around the compound element
  // - This could instead be done by creating a sub-element
  //   of type box.
  // - We don't need to show any glowing of the compound element

  if (eRedraw == GSLC_REDRAW_FULL) {
    gslc_DrawFrameRect(pGui, pElem->rElem, (bGlow) ? pElem->colElemFrameGlow : pElem->colElemFrame);
  }

  // Clear the redraw flag
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_NONE);

  // Mark page as needing flip
  gslc_PageFlipSet(pGui,true);

  return true;
}

// Fetch the current value of the element's counter
int gslc_ElemXSelNumGetCounter(gslc_tsGui* pGui,gslc_tsXSelNum* pSelNum)
{
  if ((pGui == NULL) || (pSelNum == NULL)) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXSelNumGetCounter";
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return 0;
  }
  return pSelNum->nCounter;
}


void gslc_ElemXSelNumSetCounter(gslc_tsGui* pGui,gslc_tsXSelNum* pSelNum,int16_t nCount)
{
  if (pSelNum == NULL) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXSelNumSetCounter";
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return;
  }
  pSelNum->nCounter = nCount;

  // Determine new counter text
  // FIXME: Consider replacing the printf() with an optimized function to
  //        conserve RAM. Potentially leverage GSLC_DEBUG2_PRINT().
  char  acStrNew[GSLC_LOCAL_STR_LEN];
  snprintf(acStrNew,GSLC_LOCAL_STR_LEN,"%hd",pSelNum->nCounter);

  // Update the element
  gslc_tsElemRef* pElemRef = gslc_CollectFindElemById(pGui,&pSelNum->sCollect,SELNUM_ID_TXT);
  gslc_ElemSetTxtStr(pGui,pElemRef,acStrNew);

}


// Handle the compound element main functionality
// - This routine is called by gslc_ElemEvent() to handle
//   any click events that resulted from the touch tracking process.
// - The code here will generally represent the core
//   functionality of the compound element and any communication
//   between sub-elements.
// - pvElemRef is a void pointer to the element ref being tracked. From
//   the pElemRefParent member we can get the parent/compound element
//   data structures.
bool gslc_ElemXSelNumClick(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY)
{
#if defined(DRV_TOUCH_NONE)
  return false;
#else

  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem* pElem = gslc_GetElemFromRefD(pGui, pElemRef, __LINE__);
  if (!pElem) return false;

  // Fetch the parent of the clicked element which is the compound
  // element itself. This enables us to access the extra control data.
  gslc_tsElemRef*    pElemRefParent = pElem->pElemRefParent;
  if (pElemRefParent == NULL) {
    GSLC_DEBUG2_PRINT("ERROR: ElemXSelNumClick(%s) parent ElemRef ptr NULL\n","");
    return false;
  }

  gslc_tsElem*    pElemParent = gslc_GetElemFromRef(pGui,pElemRefParent);
  gslc_tsXSelNum* pSelNum     = (gslc_tsXSelNum*)(pElemParent->pXData);
  if (pSelNum == NULL) {
    GSLC_DEBUG2_PRINT("ERROR: ElemXSelNumClick() element (ID=%d) has NULL pXData\n",pElem->nId);
    return false;
  }

  // Begin the core compound element functionality
  int nCounter  = pSelNum->nCounter;

  // Handle the various button presses
  if (eTouch == GSLC_TOUCH_UP_IN) {

    // Get the tracked element ID
    gslc_tsElemRef* pElemRefTracked = pSelNum->sCollect.pElemRefTracked;
    gslc_tsElem*    pElemTracked    = gslc_GetElemFromRef(pGui,pElemRefTracked);
    int nSubElemId = pElemTracked->nId;

    if (nSubElemId == SELNUM_ID_BTN_INC) {
      // Increment button
      if (nCounter < 100) {
        nCounter++;
      }
      gslc_ElemXSelNumSetCounter(pGui,pSelNum,nCounter);

    } else if (nSubElemId == SELNUM_ID_BTN_DEC) {
      // Decrement button
      if (nCounter > 0) {
        nCounter--;
      }
      gslc_ElemXSelNumSetCounter(pGui,pSelNum,nCounter);

    }
  } // eTouch

  return true;
#endif // !DRV_TOUCH_NONE
}

bool gslc_ElemXSelNumTouch(void* pvGui,void* pvElemRef,gslc_teTouch eTouch,int16_t nRelX,int16_t nRelY)
{
#if defined(DRV_TOUCH_NONE)
  return false;
#else

  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsXSelNum* pSelNum = (gslc_tsXSelNum*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_SELNUM, __LINE__);
  if (!pSelNum) return false;

  // Get Collection
  gslc_tsCollect* pCollect = &pSelNum->sCollect;
  return gslc_CollectTouchCompound(pvGui, pvElemRef, eTouch, nRelX, nRelY, pCollect);

  #endif // !DRV_TOUCH_NONE
}
#endif // GSLC_FEATURE_COMPOUND


// ============================================================================
