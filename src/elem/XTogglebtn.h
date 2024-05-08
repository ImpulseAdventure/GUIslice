#ifndef _GUISLICE_EX_XTOGGLEBTN_H_
#define _GUISLICE_EX_XTOGGLEBTN_H_

#include "GUIslice.h"


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
/// \file XTogglebtn.h

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// ============================================================================
// Extended Element: Togglebtn
// - Togglebtn 
//   Acts much like a checkbox but with styles that are similar to iOS and 
//   Android slider buttons.
// ============================================================================

// Define unique identifier for extended element type
// - Select any number above GSLC_TYPE_BASE_EXTEND
#define  GSLC_TYPEX_TOGGLEBTN GSLC_TYPE_BASE_EXTEND + 40

// Extended element data structures
// - These data structures are maintained in the gslc_tsElem
//   structure via the pXData pointer

/// Extended data for Togglebtn element
typedef struct {
  bool                        bOn;          ///< Indicates if button is ON or OFF
  int16_t                     nMyPageId;    ///< We need to track our page in case of grouping elements 
                                            ///< on a non current layer, like base layer
  gslc_tsColor                colThumb;     ///< Color of thumb
  gslc_tsColor                colOnState;   ///< Color of button in ON state
  gslc_tsColor                colOffState;  ///< Color of button in OFF state
  bool                        bCircular;    ///< Style of the toggle button circular or rectangular
  GSLC_CB_TOUCH               pfunctUser;   ///< User's Callback event to say element has changed
} gslc_tsXTogglebtn;


///
/// Create a Togglebtn button Element
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nElemId:     Element ID to assign (0..16383 or GSLC_ID_AUTO to autogen)
/// \param[in]  nPage:       Page ID to attach element to
/// \param[in]  pXData:      Ptr to extended element data structure
/// \param[in]  rElem:       Rectangle coordinates defining togglebtn size
/// \param[in]  colThumb:    Color of thumb
/// \param[in]  colOnState:  Color to indicate on position
/// \param[in]  colOffState: Color to indicate off position
/// \param[in]  bCircular:   Style of the toggle button circular or rectangular
/// \param[in]  bChecked:    Default state
/// \param[in]  cbTouch:     Callback for touch events
///
/// \return Pointer to Element reference or NULL if failure
///
gslc_tsElemRef* gslc_ElemXTogglebtnCreate(gslc_tsGui* pGui,int16_t nElemId,int16_t nPage,
  gslc_tsXTogglebtn* pXData,gslc_tsRect rElem,
  gslc_tsColor colThumb,gslc_tsColor colOnState,gslc_tsColor colOffState,
  bool bCircular,bool bChecked,GSLC_CB_TOUCH cbTouch);


///
/// Get a Togglebtn element's current state
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
///
/// \return Current state
///
bool gslc_ElemXTogglebtnGetState(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef);

///
/// Set a Togglebtn element's current state
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
/// \param[in]  bOn:    New state
///
/// \return none
///
void gslc_ElemXTogglebtnSetState(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,bool bOn);

///
/// Toggle a Togglebtn element's current state
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
///
/// \return none
///
void gslc_ElemXTogglebtnToggleState(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef);

///
/// Draw a Togglebtn element on the screen
/// - Called from gslc_ElemDraw()
///
/// \param[in]  pvGui:       Void ptr to GUI (typecast to gslc_tsGui*)
/// \param[in]  pvElemRef:   Void ptr to Element reference (typecast to gslc_tsElemRef*)
/// \param[in]  eRedraw:     Redraw mode
///
/// \return true if success, false otherwise
///
bool gslc_ElemXTogglebtnDraw(void* pvGui,void* pvElemRef,gslc_teRedrawType eRedraw);

///
/// Handle touch events to Togglebtn element
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
bool gslc_ElemXTogglebtnTouch(void* pvGui,void* pvElemRef,gslc_teTouch eTouch,int16_t nRelX,int16_t nRelY);

///
/// Find the togglebtn within a group that has been selected
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nGroupId:    Group ID to search
///
/// \return Element Ptr or NULL if none selected
///
gslc_tsElemRef* gslc_ElemXTogglebtnFindSelected(gslc_tsGui* pGui,int16_t nGroupId);

// ============================================================================
// ------------------------------------------------------------------------
// Read-only element macros
// ------------------------------------------------------------------------

// Macro initializers for Read-Only Elements in Flash/PROGMEM
//


/// \def gslc_ElemXTogglebtnCreate_P(pGui,nElemId,nPage,nX,nY,nW,nH,nGroup,bRadio_,nStyle_,colCheck_,bChecked_)
///
/// Create a Togglebtn button Element
///
/// \param[in]  pGui:         Pointer to GUI
/// \param[in]  nElemId:      Element ID to assign (0..16383 or GSLC_ID_AUTO to autogen)
/// \param[in]  nPage:        Page ID to attach element to
/// \param[in]  nX:           X coordinate of element
/// \param[in]  nY:           Y coordinate of element
/// \param[in]  nW:           Width of element
/// \param[in]  nH:           Height of element
/// \param[in]  colThumb_:    Color of thumb
/// \param[in]  colOnState_:  Color to indicate on position
/// \param[in]  colOffState_: Color to indicate off position
/// \param[in]  bCircular_:   Style of the toggle button circular or rectangular
/// \param[in]  bChecked_:    Default state
/// \param[in]  cbTouch:      Callback for touch events
///
/// \return none
///

#if (GSLC_USE_PROGMEM)

#define gslc_ElemXTogglebtnCreate_P(pGui,nElemId,nPage,nX,nY,nW,nH,colThumb_,colOnState_,colOffState_,bCircular_,bChecked_,cbTouch) \
  static const uint16_t nFeatures##nElemId = GSLC_ELEM_FEA_VALID | \
    GSLC_ELEM_FEA_GLOW_EN | GSLC_ELEM_FEA_CLICK_EN | GSLC_ELEM_FEA_FILL_EN; \
  static gslc_tsXTogglebtn sTogglebtn##nElemId;                   \
  sTogglebtn##nElemId.bOn = bChecked_;                            \
  sTogglebtn##nElemId.nMyPageId = nPage;                          \
  sTogglebtn##nElemId.colThumb = colThumb_;                       \
  sTogglebtn##nElemId.colOnState = colOnState_;                   \
  sTogglebtn##nElemId.colOffState = colOffState_;                 \
  sTogglebtn##nElemId.bCircular = bCircular_;                     \
  sTogglebtn##nElemId.pfunctUser = cbTouch;                       \
  static const gslc_tsElem sElem##nElemId PROGMEM = {             \
      nElemId,                                                    \
      nFeatures##nElemId,                                         \
      GSLC_TYPEX_TOGGLEBTN,                                       \
      (gslc_tsRect){nX,nY,nW,nH},                                 \
      -6999,                                                      \
      GSLC_COL_GRAY,GSLC_COL_BLACK,GSLC_COL_WHITE,GSLC_COL_BLACK, \
      (gslc_tsImgRef){NULL,NULL,GSLC_IMGREF_NONE,NULL},           \
      (gslc_tsImgRef){NULL,NULL,GSLC_IMGREF_NONE,NULL},           \
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
      (void*)(&sTogglebtn##nElemId),                              \
      NULL,                                                       \
      &gslc_ElemXTogglebtnDraw,                                   \
      &gslc_ElemXTogglebtnTouch,                                  \
      NULL,                                                       \
  };                                                              \
  gslc_ElemAdd(pGui,nPage,(gslc_tsElem*)&sElem##nElemId,          \
    (gslc_teElemRefFlags)(GSLC_ELEMREF_SRC_PROG | GSLC_ELEMREF_VISIBLE | GSLC_ELEMREF_REDRAW_FULL));


#else

#define gslc_ElemXTogglebtnCreate_P(pGui,nElemId,nPage,nX,nY,nW,nH,colThumb_,colOnState_,colOffState_,bCircular_,bChecked_,cbTouch) \
  static const uint16_t nFeatures##nElemId = GSLC_ELEM_FEA_VALID | \
    GSLC_ELEM_FEA_GLOW_EN | GSLC_ELEM_FEA_CLICK_EN | GSLC_ELEM_FEA_FILL_EN; \
  static gslc_tsXTogglebtn sTogglebtn##nElemId;                   \
  sTogglebtn##nElemId.bOn = bChecked_;                            \
  sTogglebtn##nElemId.nMyPageId = nPage;                          \
  sTogglebtn##nElemId.colThumb = colThumb_;                       \
  sTogglebtn##nElemId.colOnState = colOnState_;                   \
  sTogglebtn##nElemId.colOffState = colOffState_;                 \
  sTogglebtn##nElemId.bCircular = bCircular_;                     \
  sTogglebtn##nElemId.pfunctUser = cbTouch;                       \
  static const gslc_tsElem sElem##nElemId = {                     \
      nElemId,                                                    \
      nFeatures##nElemId,                                         \
      GSLC_TYPEX_TOGGLEBTN,                                       \
      (gslc_tsRect){nX,nY,nW,nH},                                 \
      -6999,                                                      \
      GSLC_COL_GRAY,GSLC_COL_BLACK,GSLC_COL_WHITE,GSLC_COL_BLACK, \
      (gslc_tsImgRef){NULL,NULL,GSLC_IMGREF_NONE,NULL},           \
      (gslc_tsImgRef){NULL,NULL,GSLC_IMGREF_NONE,NULL},           \
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
      (void*)(&sTogglebtn##nElemId),                              \
      NULL,                                                       \
      &gslc_ElemXTogglebtnDraw,                                   \
      &gslc_ElemXTogglebtnTouch,                                  \
      NULL,                                                       \
  };                                                              \
  gslc_ElemAdd(pGui,nPage,(gslc_tsElem*)&sElem##nElemId,          \
    (gslc_teElemRefFlags)(GSLC_ELEMREF_SRC_PROG | GSLC_ELEMREF_VISIBLE | GSLC_ELEMREF_REDRAW_FULL));

#endif


#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _GUISLICE_EX_XTOGGLEBTN_H_
