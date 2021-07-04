#ifndef _GUISLICE_EX_XCHECKBOX_H_
#define _GUISLICE_EX_XCHECKBOX_H_

#include "GUIslice.h"


// =======================================================================
// GUIslice library extension: Checkbox/Radio button control
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
/// \file XCheckbox.h

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// ============================================================================
// Extended Element: Checkbox
// ============================================================================

// Define unique identifier for extended element type
// - Select any number above GSLC_TYPE_BASE_EXTEND
#define  GSLC_TYPEX_CHECKBOX GSLC_TYPE_BASE_EXTEND + 1

/// Checkbox drawing style
typedef enum {
  GSLCX_CHECKBOX_STYLE_BOX,                 ///< Inner box
  GSLCX_CHECKBOX_STYLE_X,                   ///< Crossed
  GSLCX_CHECKBOX_STYLE_ROUND,               ///< Circular
} gslc_teXCheckboxStyle;

/// Callback function for checkbox/radio element state change
/// - nSelId:   Selected element's ID or GSLC_ID_NONE
/// - bChecked: Element was selected if true, false otherwise
typedef bool (*GSLC_CB_XCHECKBOX)(void* pvGui,void* pvElemRef,int16_t nSelId, bool bChecked);

// Extended element data structures
// - These data structures are maintained in the gslc_tsElem
//   structure via the pXData pointer

/// Extended data for Checkbox element
typedef struct {
  bool                        bRadio;       ///< Radio-button operation if true
  gslc_teXCheckboxStyle       nStyle;       ///< Drawing style for element
  bool                        bChecked;     ///< Indicates if it is selected (checked)
  gslc_tsColor                colCheck;     ///< Color of checked inner fill
  GSLC_CB_XCHECKBOX           pfuncXToggle; ///< Callback event to say element has changed
} gslc_tsXCheckbox;


///
/// Create a Checkbox or Radio button Element
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nElemId:     Element ID to assign (0..16383 or GSLC_ID_AUTO to autogen)
/// \param[in]  nPage:       Page ID to attach element to
/// \param[in]  pXData:      Ptr to extended element data structure
/// \param[in]  rElem:       Rectangle coordinates defining checkbox size
/// \param[in]  bRadio:      Radio-button functionality if true
/// \param[in]  nStyle:      Drawing style for checkbox / radio button
/// \param[in]  colCheck:    Color for inner fill when checked
/// \param[in]  bChecked:    Default state
///
/// \return Pointer to Element reference or NULL if failure
///
gslc_tsElemRef* gslc_ElemXCheckboxCreate(gslc_tsGui* pGui,int16_t nElemId,int16_t nPage,
  gslc_tsXCheckbox* pXData,gslc_tsRect rElem,bool bRadio,
  gslc_teXCheckboxStyle nStyle,gslc_tsColor colCheck,bool bChecked);


///
/// Get a Checkbox element's current state
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
///
/// \return Current state
///
bool gslc_ElemXCheckboxGetState(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef);

///
/// Set a Checkbox element's current state
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
/// \param[in]  bChecked:    New state
///
/// \return none
///
void gslc_ElemXCheckboxSetState(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,bool bChecked);

///
/// Find the checkbox within a group that has been checked
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nGroupId:    Group ID to search
///
/// \return Element Ptr or NULL if none checked
///
gslc_tsElemRef* gslc_ElemXCheckboxFindChecked(gslc_tsGui* pGui,int16_t nGroupId);

///
/// Toggle a Checkbox element's current state
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
///
/// \return none
///
void gslc_ElemXCheckboxToggleState(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef);

///
/// Assign the state callback function for a checkbox/radio button
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
/// \param[in]  pfuncCb:     Function pointer to callback routine (or NULL for none)
///
/// \return none
///
void gslc_ElemXCheckboxSetStateFunc(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, GSLC_CB_XCHECKBOX pfuncCb);

///
/// Draw a Checkbox element on the screen
/// - Called from gslc_ElemDraw()
///
/// \param[in]  pvGui:       Void ptr to GUI (typecast to gslc_tsGui*)
/// \param[in]  pvElemRef:   Void ptr to Element reference (typecast to gslc_tsElemRef*)
/// \param[in]  eRedraw:     Redraw mode
///
/// \return true if success, false otherwise
///
bool gslc_ElemXCheckboxDraw(void* pvGui,void* pvElemRef,gslc_teRedrawType eRedraw);

///
/// Handle touch events to Checkbox element
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
bool gslc_ElemXCheckboxTouch(void* pvGui,void* pvElemRef,gslc_teTouch eTouch,int16_t nRelX,int16_t nRelY);
// ============================================================================

// ------------------------------------------------------------------------
// Read-only element macros
// ------------------------------------------------------------------------

// Macro initializers for Read-Only Elements in Flash/PROGMEM
//


/// \def gslc_ElemXCheckboxCreate_P(pGui,nElemId,nPage,nX,nY,nW,nH,nGroup,bRadio_,nStyle_,colCheck_,bChecked_)
///
/// Create a Checkbox or Radio button Element in Flash
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nElemId:     Unique element ID to assign
/// \param[in]  nPage:       Page ID to attach element to
/// \param[in]  nX:          X coordinate of element
/// \param[in]  nY:          Y coordinate of element
/// \param[in]  nW:          Width of element
/// \param[in]  nH:          Height of element
/// \param[in]  colFill:     Color for the control background fill
/// \param[in]  bFillEn:     True if background filled, false otherwise (recommend True)
/// \param[in]  nGroup:      Group ID that radio buttons belong to (else GSLC_GROUP_NONE)
/// \param[in]  bRadio_:     Radio-button functionality if true
/// \param[in]  nStyle_:     Drawing style for checkbox / radio button
/// \param[in]  colCheck_:   Color for inner fill when checked
/// \param[in]  bChecked_:   Default state
///
/// \return none
///

#if (GSLC_USE_PROGMEM)

#define gslc_ElemXCheckboxCreate_P(pGui,nElemId,nPage,nX,nY,nW,nH,colFill,bFillEn,nGroup,bRadio_,nStyle_,colCheck_,bChecked_) \
  static const uint16_t nFeatures##nElemId = GSLC_ELEM_FEA_VALID | \
    GSLC_ELEM_FEA_GLOW_EN | GSLC_ELEM_FEA_CLICK_EN | (bFillEn?GSLC_ELEM_FEA_FILL_EN:0); \
  static gslc_tsXCheckbox sCheckbox##nElemId;                     \
  sCheckbox##nElemId.bRadio = bRadio_;                            \
  sCheckbox##nElemId.bChecked = bChecked_;                        \
  sCheckbox##nElemId.colCheck = colCheck_;                        \
  sCheckbox##nElemId.nStyle = nStyle_;                            \
  static const gslc_tsElem sElem##nElemId PROGMEM = {             \
      nElemId,                                                    \
      nFeatures##nElemId,                                         \
      GSLC_TYPEX_CHECKBOX,                                        \
      (gslc_tsRect){nX,nY,nW,nH},                                 \
      nGroup,                                                     \
      GSLC_COL_GRAY,colFill,GSLC_COL_WHITE,GSLC_COL_BLACK, \
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
      (void*)(&sCheckbox##nElemId),                               \
      NULL,                                                       \
      &gslc_ElemXCheckboxDraw,                                    \
      &gslc_ElemXCheckboxTouch,                                   \
      NULL,                                                       \
  };                                                              \
  gslc_ElemAdd(pGui,nPage,(gslc_tsElem*)&sElem##nElemId,          \
    (gslc_teElemRefFlags)(GSLC_ELEMREF_SRC_PROG | GSLC_ELEMREF_VISIBLE | GSLC_ELEMREF_REDRAW_FULL));


#else

#define gslc_ElemXCheckboxCreate_P(pGui,nElemId,nPage,nX,nY,nW,nH,colFill,bFillEn,nGroup,bRadio_,nStyle_,colCheck_,bChecked_) \
  static const uint16_t nFeatures##nElemId = GSLC_ELEM_FEA_VALID | \
    GSLC_ELEM_FEA_GLOW_EN | GSLC_ELEM_FEA_CLICK_EN | (bFillEn?GSLC_ELEM_FEA_FILL_EN:0); \
  static gslc_tsXCheckbox sCheckbox##nElemId;                     \
  sCheckbox##nElemId.bRadio = bRadio_;                            \
  sCheckbox##nElemId.bChecked = bChecked_;                        \
  sCheckbox##nElemId.colCheck = colCheck_;                        \
  sCheckbox##nElemId.nStyle = nStyle_;                            \
  static const gslc_tsElem sElem##nElemId = {                     \
      nElemId,                                                    \
      nFeatures##nElemId,                                         \
      GSLC_TYPEX_CHECKBOX,                                        \
      (gslc_tsRect){nX,nY,nW,nH},                                 \
      nGroup,                                                     \
      GSLC_COL_GRAY,colFill,GSLC_COL_WHITE,GSLC_COL_BLACK, \
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
      (void*)(&sCheckbox##nElemId),                               \
      NULL,                                                       \
      &gslc_ElemXCheckboxDraw,                                    \
      &gslc_ElemXCheckboxTouch,                                   \
      NULL,                                                       \
  };                                                              \
  gslc_ElemAdd(pGui,nPage,(gslc_tsElem*)&sElem##nElemId,          \
    (gslc_teElemRefFlags)(GSLC_ELEMREF_SRC_CONST | GSLC_ELEMREF_VISIBLE | GSLC_ELEMREF_REDRAW_FULL));

#endif


#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _GUISLICE_EX_XCHECKBOX_H_

