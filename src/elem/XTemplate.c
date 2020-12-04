// =======================================================================
// GUIslice library (extensions template)
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
/// \file XTemplate.c



// GUIslice library
#include "GUIslice.h"
#include "GUIslice_drv.h"

#include "elem/XTemplate.h"

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
// Extended Element: Template example
// - Demonstrates minimum code to implement an extended element
// ============================================================================

// Create a text element and add it to the GUI element list
// - Defines default styling for the element
// - Defines callback for redraw and touch
gslc_tsElemRef* gslc_ElemXTemplateCreate(gslc_tsGui* pGui, int16_t nElemId, int16_t nPage,
  gslc_tsXTemplate* pXData, gslc_tsRect rElem, char* pStrBuf, uint8_t nStrBufMax, int16_t nFontId)
{
  if ((pGui == NULL) || (pXData == NULL)) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXTemplateCreate";
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return NULL;
  }
  gslc_tsElem     sElem;
  gslc_tsElemRef* pElemRef = NULL;
  sElem = gslc_ElemCreate(pGui,nElemId,nPage,GSLC_TYPEX_TEMPLATE,rElem,pStrBuf,nStrBufMax,nFontId);
  sElem.colElemFill       = GSLC_COL_BLACK;
  sElem.colElemFillGlow   = GSLC_COL_BLACK;
  sElem.colElemFrame      = GSLC_COL_GRAY;
  sElem.colElemFrameGlow  = GSLC_COL_GRAY;
  sElem.colElemText       = GSLC_COL_WHITE;
  sElem.colElemTextGlow   = GSLC_COL_WHITE;
  sElem.nFeatures        |= GSLC_ELEM_FEA_FILL_EN;
  sElem.nFeatures        &= ~GSLC_ELEM_FEA_FRAME_EN;
  sElem.nFeatures        |= GSLC_ELEM_FEA_CLICK_EN;
  sElem.nFeatures        |= GSLC_ELEM_FEA_GLOW_EN;
  sElem.eTxtAlign         = GSLC_ALIGN_MID_LEFT;

  sElem.nGroup            = GSLC_GROUP_ID_NONE;

  // TODO:
  // - Initialize any pxData members with the constructor parameters
  //   eg. pXData->nPos = nPos;

  sElem.pXData            = (void*)(pXData);
  // Specify the custom drawing callback
  sElem.pfuncXDraw        = &gslc_ElemXTemplateDraw;
  // Specify the custom touch tracking callback
  sElem.pfuncXTouch       = &gslc_ElemXTemplateTouch;

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


// Redraw the element
// - Note that this redraw is for the entire element rect region
// - The Draw function parameters use void pointers to allow for
//   simpler callback function definition & scalability.
bool gslc_ElemXTemplateDraw(void* pvGui,void* pvElemRef,gslc_teRedrawType eRedraw)
{
  (void)eRedraw; // Unused
  if ((pvGui == NULL) || (pvElemRef == NULL)) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXTemplateDraw";
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return false;
  }
  // Typecast the parameters to match the GUI and element types
  gslc_tsGui*       pGui  = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef*   pElemRef = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem*      pElem = gslc_GetElemFromRef(pGui,pElemRef);

  // Fetch the element's extended data structure
  gslc_tsXTemplate* pTemplate = (gslc_tsXTemplate*)(pElem->pXData);
  if (pTemplate == NULL) {
    GSLC_DEBUG2_PRINT("ERROR: ElemXTemplateDraw(%s) pXData is NULL\n","");
    return false;
  }

  // Template example drawing routine implements formatted text field

  // --------------------------------------------------------------------------
  // Init for default drawing
  // --------------------------------------------------------------------------

  bool      bGlowEn,bGlowing,bGlowNow;
  //int16_t   nElemX,nElemY;
  //uint16_t  nElemW,nElemH;

  //nElemX    = pElem->rElem.x;
  //nElemY    = pElem->rElem.y;
  //nElemW    = pElem->rElem.w;
  //nElemH    = pElem->rElem.h;
  bGlowEn   = pElem->nFeatures & GSLC_ELEM_FEA_GLOW_EN; // Does the element support glow state?
  bGlowing  = gslc_ElemGetGlow(pGui,pElemRef); // Element should be glowing (if enabled)
  bGlowNow  = bGlowEn & bGlowing; // Element is currently glowing
  gslc_tsColor colBg = GSLC_COL_BLACK;

  // --------------------------------------------------------------------------
  // Background
  // --------------------------------------------------------------------------

  // Fill in the background
  gslc_tsRect rElemInner = pElem->rElem;
  // - If both fill and frame are enabled then contract
  //   the fill region slightly so that we don't overdraw
  //   the frame (prevent unnecessary flicker).
  if ((pElem->nFeatures & GSLC_ELEM_FEA_FILL_EN) && (pElem->nFeatures & GSLC_ELEM_FEA_FRAME_EN)) {
    // NOTE: If the region is already too small to shrink (eg. w=1 or h=1)
    // then a zero dimension box will be returned by ExpandRect() and not drawn
    rElemInner = gslc_ExpandRect(rElemInner,-1,-1);
  }
  // - This also changes the fill color if selected and glow state is enabled
  if (pElem->nFeatures & GSLC_ELEM_FEA_FILL_EN) {
    if (bGlowEn && bGlowing) {
      colBg = pElem->colElemFillGlow;
    } else {
      colBg = pElem->colElemFill;
    }
    if (pElem->nFeatures & GSLC_ELEM_FEA_ROUND_EN) {
      gslc_DrawFillRoundRect(pGui, rElemInner, pGui->nRoundRadius, colBg);
    } else {
      gslc_DrawFillRect(pGui, rElemInner, colBg);
    }
  } else {
    // TODO: If unfilled, then we might need
    // to redraw the background layer(s)
  }

  // --------------------------------------------------------------------------
  // Frame
  // --------------------------------------------------------------------------

  // Frame the region
  if (pElem->nFeatures & GSLC_ELEM_FEA_FRAME_EN) {
    if (pElem->nFeatures & GSLC_ELEM_FEA_ROUND_EN) {
      gslc_DrawFrameRoundRect(pGui, pElem->rElem, pGui->nRoundRadius, pElem->colElemFrame);
    } else {
      gslc_DrawFrameRect(pGui, pElem->rElem, pElem->colElemFrame);
    }
  }

  // --------------------------------------------------------------------------
  // Text overlays
  // --------------------------------------------------------------------------

  // Draw text string if defined
  if (pElem->pStrBuf) {
    gslc_tsColor  colTxt    = (bGlowNow)? pElem->colElemTextGlow : pElem->colElemText;
    int8_t        nMarginX  = pElem->nTxtMarginX;
    int8_t        nMarginY  = pElem->nTxtMarginY;

    gslc_DrawTxtBase(pGui, pElem->pStrBuf, pElem->rElem, pElem->pTxtFont, pElem->eTxtFlags,
      pElem->eTxtAlign, colTxt, colBg, nMarginX, nMarginY);
  }

  // --------------------------------------------------------------------------

  // Mark the element as no longer requiring redraw
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_NONE);

  return true;

}


// This callback function is called by gslc_ElemSendEventTouch()
// after any touch event
bool gslc_ElemXTemplateTouch(void* pvGui,void* pvElemRef,gslc_teTouch eTouch,int16_t nRelX,int16_t nRelY)
{
  (void)nRelX; // Unused
  (void)nRelY; // Unused
#if defined(DRV_TOUCH_NONE)
  return false;
#else

  // Define any specific touch handling here

  if ((pvGui == NULL) || (pvElemRef == NULL)) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXTemplateTouch";
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return false;
  }
  gslc_tsGui*           pGui = NULL;
  gslc_tsElemRef*       pElemRef = NULL;
  //gslc_tsElem*          pElem = NULL;
  //gslc_tsXTemplate*     pTemplate = NULL;

  // Typecast the parameters to match the GUI
  pGui      = (gslc_tsGui*)(pvGui);
  pElemRef  = (gslc_tsElemRef*)(pvElemRef);
  //pElem     = gslc_GetElemFromRef(pGui,pElemRef);
  //pTemplate = (gslc_tsXTemplate*)(pElem->pXData);

  bool    bGlowingOld = gslc_ElemGetGlow(pGui,pElemRef);

  // Example touch handler simply tracks "glowing" status

  switch(eTouch) {

    case GSLC_TOUCH_DOWN_IN:
      // Start glowing as must be over it
      gslc_ElemSetGlow(pGui,pElemRef,true);
      break;

    case GSLC_TOUCH_MOVE_IN:
      gslc_ElemSetGlow(pGui,pElemRef,true);
      break;
    case GSLC_TOUCH_MOVE_OUT:
      gslc_ElemSetGlow(pGui,pElemRef,false);
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
      gslc_ElemSetGlow(pGui,pElemRef,true);
      break;

    default:
      return false;
      break;
  }

  // If the slider changed state, redraw
  if (gslc_ElemGetGlow(pGui,pElemRef) != bGlowingOld) {
    gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_FULL);
  }

  return true;

  #endif // !DRV_TOUCH_NONE
}

// ============================================================================
