#ifndef _GUISLICE_EX_XRAMP_H_
#define _GUISLICE_EX_XRAMP_H_

#include "GUIslice.h"


// =======================================================================
// GUIslice library extension: Ramp gauge
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
/// \file XRamp.h

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


// ============================================================================
// Extended Element: Ramp Gauge
// - Demonstration of a gradient ramp (green-yellow-red) visual
//   control similar to certain linear tachometers.
// - The ramp rises up and to the right according to the
//   current value.
// - Note that this element is mainly intended as a demonstration
//   example. Additional APIs would be recommended to make it
//   more configurable.
// ============================================================================

// Define unique identifier for extended element type
// - Select any number above GSLC_TYPE_BASE_EXTEND
#define  GSLC_TYPEX_RAMP GSLC_TYPE_BASE_EXTEND + 62


// Extended element data structures
// - These data structures are maintained in the gslc_tsElem
//   structure via the pXData pointer

/// Extended data for Gauge element
typedef struct {
  // Range config
  int16_t             nMin;           ///< Minimum control value
  int16_t             nMax;           ///< Maximum control value

  // Current value
  int16_t             nVal;           ///< Current control value
  // Previous value
  int16_t             nValLast;       ///< Last value
  bool                bValLastValid;  ///< Last value valid?

  // Appearance config

} gslc_tsXRamp;


///
/// Create a Ramp Gauge Element
/// - Draws a gauge element that represents a proportion (nVal)
///   between nMin and nMax.
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nElemId:     Element ID to assign (0..16383 or GSLC_ID_AUTO to autogen)
/// \param[in]  nPage:       Page ID to attach element to
/// \param[in]  pXData:      Ptr to extended element data structure
/// \param[in]  rElem:       Rectangle coordinates defining gauge size
/// \param[in]  nMin:        Minimum value of gauge for nVal comparison
/// \param[in]  nMax:        Maximum value of gauge for nVal comparison
/// \param[in]  nVal:        Starting value of gauge
/// \param[in]  colGauge:    Color for the gauge indicator
/// \param[in]  bVert:       Flag to indicate vertical vs horizontal action
///                          (true = vertical, false = horizontal)
///
/// \return Pointer to Element reference or NULL if failure
///
gslc_tsElemRef* gslc_ElemXRampCreate(gslc_tsGui* pGui,int16_t nElemId,int16_t nPage,
  gslc_tsXRamp* pXData,gslc_tsRect rElem,int16_t nMin,int16_t nMax,int16_t nVal,gslc_tsColor colGauge,bool bVert);



///
/// Update a Gauge element's current value
/// - Note that min & max values are assigned in create()
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
/// \param[in]  nVal:        New value to show in gauge
///
/// \return none
///
void gslc_ElemXRampSetVal(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,int16_t nVal);



///
/// Draw a gauge element on the screen
/// - Called from gslc_ElemDraw()
///
/// \param[in]  pvGui:       Void ptr to GUI (typecast to gslc_tsGui*)
/// \param[in]  pvElemRef:   Void ptr to Element reference (typecast to gslc_tsElemRef*)
/// \param[in]  eRedraw:     Redraw mode
///
/// \return true if success, false otherwise
///
bool gslc_ElemXRampDraw(void* pvGui,void* pvElemRef,gslc_teRedrawType eRedraw);


///
/// Helper function to draw a gauge with style: ramp
/// - Called from gslc_ElemXRampDraw()
///
/// \param[in]  pGui:        Ptr to GUI
/// \param[in]  pElemRef:    Ptr to Element reference
/// \param[in]  eRedraw:     Redraw status
///
/// \return true if success, false otherwise
///
bool gslc_ElemXRampDrawHelp(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,gslc_teRedrawType eRedraw);

// ============================================================================

// ------------------------------------------------------------------------
// Read-only element macros
// ------------------------------------------------------------------------

// Macro initializers for Read-Only Elements in Flash/PROGMEM
//


/// \def gslc_ElemXRampCreate_P(pGui,nElemId,nPage,nX,nY,nW,nH,
///      nMin_,nMax_,nVal_,colFrame_,colFill_)
///
/// Create a Gauge Element in Flash
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nElemId:     Unique element ID to assign
/// \param[in]  nPage:       Page ID to attach element to
/// \param[in]  nX:          X coordinate of element
/// \param[in]  nY:          Y coordinate of element
/// \param[in]  nW:          Width of element
/// \param[in]  nH:          Height of element
/// \param[in]  nMin_:       Minimum value of gauge for nVal comparison
/// \param[in]  nMax_:       Maximum value of gauge for nVal comparison
/// \param[in]  nVal_:       Starting value of gauge
/// \param[in]  colFrame_:   Color for the gauge frame
/// \param[in]  colFill_:    Color for the gauge background fill
///
/// \return none
///


#if (GSLC_USE_PROGMEM)


#define gslc_ElemXRampCreate_P(pGui,nElemId,nPage,nX,nY,nW,nH,\
    nMin_,nMax_,nVal_,colFrame_,colFill_) \
  static const uint16_t nFeatures##nElemId = GSLC_ELEM_FEA_VALID | \
    GSLC_ELEM_FEA_GLOW_EN | GSLC_ELEM_FEA_FILL_EN; \
  static gslc_tsXRamp sGauge##nElemId;                           \
  sGauge##nElemId.nMin = nMin_;                                   \
  sGauge##nElemId.nMax = nMax_;                                   \
  sGauge##nElemId.nVal = nVal_;                                   \
  sGauge##nElemId.nValLast = nVal_;                               \
  sGauge##nElemId.bValLastValid = false;                          \
  static const gslc_tsElem sElem##nElemId PROGMEM = {             \
      nElemId,                                                    \
      nFeatures##nElemId,                                         \
      GSLC_TYPEX_RAMP,                                           \
      (gslc_tsRect){nX,nY,nW,nH},                                 \
      GSLC_GROUP_ID_NONE,                                         \
      colFrame_,colFill_,colFrame_,colFill_,                      \
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
      (void*)(&sGauge##nElemId),                                  \
      NULL,                                                       \
      &gslc_ElemXRampDraw,                                       \
      NULL,                                                       \
      NULL,                                                       \
  };                                                              \
  gslc_ElemAdd(pGui,nPage,(gslc_tsElem*)&sElem##nElemId,          \
    (gslc_teElemRefFlags)(GSLC_ELEMREF_SRC_PROG | GSLC_ELEMREF_VISIBLE | GSLC_ELEMREF_REDRAW_FULL));

#else


#define gslc_ElemXRampCreate_P(pGui,nElemId,nPage,nX,nY,nW,nH,\
    nMin_,nMax_,nVal_,colFrame_,colFill_) \
  static const uint16_t nFeatures##nElemId = GSLC_ELEM_FEA_VALID | \
    GSLC_ELEM_FEA_GLOW_EN | GSLC_ELEM_FEA_FILL_EN; \
  static gslc_tsXRamp sGauge##nElemId;                           \
  sGauge##nElemId.nMin = nMin_;                                   \
  sGauge##nElemId.nMax = nMax_;                                   \
  sGauge##nElemId.nVal = nVal_;                                   \
  sGauge##nElemId.nValLast = nVal_;                               \
  sGauge##nElemId.bValLastValid = false;                          \
  static const gslc_tsElem sElem##nElemId = {                     \
      nElemId,                                                    \
      nFeatures##nElemId,                                         \
      GSLC_TYPEX_RAMP,                                           \
      (gslc_tsRect){nX,nY,nW,nH},                                 \
      GSLC_GROUP_ID_NONE,                                         \
      colFrame_,colFill_,colFrame_,colFill_,                      \
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
      (void*)(&sGauge##nElemId),                                  \
      NULL,                                                       \
      &gslc_ElemXRampDraw,                                       \
      NULL,                                                       \
      NULL,                                                       \
  };                                                              \
  gslc_ElemAdd(pGui,nPage,(gslc_tsElem*)&sElem##nElemId,          \
    (gslc_teElemRefFlags)(GSLC_ELEMREF_SRC_CONST | GSLC_ELEMREF_VISIBLE | GSLC_ELEMREF_REDRAW_FULL));

#endif

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _GUISLICE_EX_XRAMP_H_

