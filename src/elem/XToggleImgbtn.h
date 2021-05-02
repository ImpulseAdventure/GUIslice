#ifndef _GUISLICE_EX_XTOGGLEIMGBTN_H_
#define _GUISLICE_EX_XTOGGLEIMGBTN_H_

#include "GUIslice.h"


// =======================================================================
// GUIslice library extension: Toggle button with images control
// - Paul Conti
// - Toggle button using images with Android and iOS like styles
// - Based on Calvin Hass' Checkbox control
// - https://www.impulseadventure.com/elec/guislice-gui.html
// - https://github.com/ImpulseAdventure/GUIslice
// =======================================================================
//
// The MIT License
//
// Copyright 2016-2021 Calvin Hass and Paul Conti
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
/// \file XToggleImgbtn.h

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// ============================================================================
// Extended Element: ToggleImgbtn
// - ToggleImgbtn 
//   Acts much like a checkbox but with styles that are similar to iOS and 
//   Android slider buttons.
// ============================================================================

// Define unique identifier for extended element type
// - Select any number above GSLC_TYPE_BASE_EXTEND
#define  GSLC_TYPEX_TOGGLEIMGBTN GSLC_TYPE_BASE_EXTEND + 41

// Extended element data structures
// - These data structures are maintained in the gslc_tsElem
//   structure via the pXData pointer

/// Extended data for ToggleImgbtn element
typedef struct {
  bool                        bOn;          ///< Indicates if button is ON or OFF
  int16_t                     nMyPageId;    ///< We need to track our page in case of grouping elements 
                                            ///< on a non current layer, like base layer
  GSLC_CB_TOUCH               pfunctUser;   ///< User's Callback event to say element has changed
} gslc_tsXToggleImgbtn;


///
/// Create a ToggleImgbtn button Element
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nElemId:     Element ID to assign (0..16383 or GSLC_ID_AUTO to autogen)
/// \param[in]  nPage:       Page ID to attach element to
/// \param[in]  pXData:      Ptr to extended element data structure
/// \param[in]  rElem:       Rectangle coordinates defining togglebtn size
/// \param[in]  sImgRef:     Image reference to load (unselected state)
/// \param[in]  sImgRefSel:  Image reference to load (selected state)
/// \param[in]  bOn:         Default state, On or Off
/// \param[in]  cbTouch:     Callback for touch events
///
/// \return Pointer to Element reference or NULL if failure
///
gslc_tsElemRef* gslc_ElemXToggleImgbtnCreate(gslc_tsGui* pGui,int16_t nElemId,int16_t nPage,
  gslc_tsXToggleImgbtn* pXData,gslc_tsRect rElem,gslc_tsImgRef sImgRef,gslc_tsImgRef sImgRefSel,
  bool bOn,GSLC_CB_TOUCH cbTouch);


///
/// Get a ToggleImgbtn element's current state
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
///
/// \return Current state
///
bool gslc_ElemXToggleImgbtnGetState(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef);

///
/// Set a ToggleImgbtn element's current state
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
/// \param[in]  bOn:         New state
///
/// \return none
///
void gslc_ElemXToggleImgbtnSetState(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,bool bOn);

///
/// Toggle a ToggleImgbtn element's current state
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
///
/// \return none
///
void gslc_ElemXToggleImgbtnToggleState(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef);

///
/// Draw a ToggleImgbtn element on the screen
/// - Called from gslc_ElemDraw()
///
/// \param[in]  pvGui:       Void ptr to GUI (typecast to gslc_tsGui*)
/// \param[in]  pvElemRef:   Void ptr to Element reference (typecast to gslc_tsElemRef*)
/// \param[in]  eRedraw:     Redraw mode
///
/// \return true if success, false otherwise
///
bool gslc_ElemXToggleImgbtnDraw(void* pvGui,void* pvElemRef,gslc_teRedrawType eRedraw);

///
/// Handle touch events to ToggleImgbtn element
/// - Called from gslc_ElemSendEventTouch()
///
/// \param[in]  pvGui:       Void ptr to GUI (typecast to gslc_tsGui*)
/// \param[in]  pvElemRef:   Void ptr to Element reference (typecast to gslc_tsElemRef*)
/// \param[in]  eTouch:      Touch event type
/// \param[in]  nRelX:       Touch X coord relative to element
/// \param[in]  nRelY:       Touch Y coord relative to element
///
/// \return true if success, false otherwise
///
bool gslc_ElemXToggleImgbtnTouch(void* pvGui,void* pvElemRef,gslc_teTouch eTouch,int16_t nRelX,int16_t nRelY);

///
/// Find the togglebtn within a group that has been selected
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nGroupId:    Group ID to search
///
/// \return Element Ptr or NULL if none selected
///
gslc_tsElemRef* gslc_ElemXToggleImgbtnFindSelected(gslc_tsGui* pGui,int16_t nGroupId);

// ============================================================================
// ------------------------------------------------------------------------
// Read-only element macros
// ------------------------------------------------------------------------

// Macro initializers for Read-Only Elements in Flash/PROGMEM
//


/// \def gslc_ElemXToggleImgbtnCreate_P(pGui,nElemId,nPage,nX,nY,nW,nH,sImgRef_,sImgRefSel_,bOn_,cbTouch)
///
/// Create a ToggleImgbtn button Element
///
/// \param[in]  pGui:         Pointer to GUI
/// \param[in]  nElemId:      Element ID to assign (0..16383 or GSLC_ID_AUTO to autogen)
/// \param[in]  nPage:        Page ID to attach element to
/// \param[in]  nX:           X coordinate of element
/// \param[in]  nY:           Y coordinate of element
/// \param[in]  nW:           Width of element
/// \param[in]  nH:           Height of element
/// \param[in]  sImgRef_:     Image reference to load (unselected state)
/// \param[in]  sImgRefSel_:  Image reference to load (selected state)
/// \param[in]  bOn_:         Default state, On or Off
/// \param[in]  cbTouch:      Callback for touch events
///
/// \return none
///

#if (GSLC_USE_PROGMEM)

#define gslc_ElemXToggleImgbtnCreate_P(pGui,nElemId,nPage,nX,nY,nW,nH,sImgRef_,sImgRefSel_,bOn_,cbTouch) \
  static const uint16_t nFeatures##nElemId = GSLC_ELEM_FEA_VALID | \
    GSLC_ELEM_FEA_GLOW_EN | GSLC_ELEM_FEA_CLICK_EN | GSLC_ELEM_FEA_FILL_EN; \
  static gslc_tsXToggleImgbtn sToggleImgbtn##nElemId;                   \
  sToggleImgbtn##nElemId.bOn = bOn_;                            \
  sToggleImgbtn##nElemId.nMyPageId = nPage;                          \
  sToggleImgbtn##nElemId.pfunctUser = cbTouch;                       \
  static const gslc_tsElem sElem##nElemId PROGMEM = {             \
      nElemId,                                                    \
      nFeatures##nElemId,                                         \
      GSLC_TYPEX_TOGGLEIMGBTN,                                       \
      (gslc_tsRect){nX,nY,nW,nH},                                 \
      0,                                                          \
      GSLC_COL_GRAY,GSLC_COL_BLACK,GSLC_COL_WHITE,GSLC_COL_BLACK, \
      sImgRef_,           \
      sImgRefSel_,           \
      NULL,                                                       \
      NULL,                                                       \
      0,                                                          \
      (gslc_teTxtFlags)(GSLC_TXT_DEFAULT),                        \
      GSLC_COL_WHITE,                                             \
      GSLC_COL_WHITE,                                             \
      GSLC_ALIGN_MID_MID,                                         \
      0,                                                          \
      0,                                                          \
      NULL,                                                       \
      (void*)(&sToggleImgbtn##nElemId),                              \
      NULL,                                                       \
      &gslc_ElemXToggleImgbtnDraw,                                   \
      &gslc_ElemXToggleImgbtnTouch,                                  \
      NULL,                                                       \
  };                                                              \
  gslc_ElemAdd(pGui,nPage,(gslc_tsElem*)&sElem##nElemId,          \
    (gslc_teElemRefFlags)(GSLC_ELEMREF_SRC_PROG | GSLC_ELEMREF_VISIBLE | GSLC_ELEMREF_REDRAW_FULL));


#else

#define gslc_ElemXToggleImgbtnCreate_P(pGui,nElemId,nPage,nX,nY,nW,nH,sImgRef_,sImgRefSel_,bOn_,cbTouch) \
  static const uint16_t nFeatures##nElemId = GSLC_ELEM_FEA_VALID | \
    GSLC_ELEM_FEA_GLOW_EN | GSLC_ELEM_FEA_CLICK_EN | GSLC_ELEM_FEA_FILL_EN; \
  static gslc_tsXToggleImgbtn sToggleImgbtn##nElemId;                   \
  sToggleImgbtn##nElemId.bOn = bOn_;                            \
  sToggleImgbtn##nElemId.nMyPageId = nPage;                          \
  sToggleImgbtn##nElemId.pfunctUser = cbTouch;                       \
  static const gslc_tsElem sElem##nElemId = {                     \
      nElemId,                                                    \
      nFeatures##nElemId,                                         \
      GSLC_TYPEX_TOGGLEIMGBTN,                                       \
      (gslc_tsRect){nX,nY,nW,nH},                                 \
      0,                                                          \
      GSLC_COL_GRAY,GSLC_COL_BLACK,GSLC_COL_WHITE,GSLC_COL_BLACK, \
      sImgRef_,           \
      sImgRefSel_,           \
      NULL,                                                       \
      NULL,                                                       \
      0,                                                          \
      (gslc_teTxtFlags)(GSLC_TXT_DEFAULT),                        \
      GSLC_COL_WHITE,                                             \
      GSLC_COL_WHITE,                                             \
      GSLC_ALIGN_MID_MID,                                         \
      0,                                                          \
      0,                                                          \
      NULL,                                                       \
      (void*)(&sToggleImgbtn##nElemId),                              \
      NULL,                                                       \
      &gslc_ElemXToggleImgbtnDraw,                                   \
      &gslc_ElemXToggleImgbtnTouch,                                  \
      NULL,                                                       \
  };                                                              \
  gslc_ElemAdd(pGui,nPage,(gslc_tsElem*)&sElem##nElemId,          \
    (gslc_teElemRefFlags)(GSLC_ELEMREF_SRC_PROG | GSLC_ELEMREF_VISIBLE | GSLC_ELEMREF_REDRAW_FULL));

#endif


#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _GUISLICE_EX_XTOGGLEIMGBTN_H_
