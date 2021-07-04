#ifndef _GUISLICE_EX_XGAUGE_H_
#define _GUISLICE_EX_XGAUGE_H_

#include "GUIslice.h"


// =======================================================================
// GUIslice library extension: Gauge control (See replacement warning below)
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
/// \file XGauge.h

// *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
// WARNING: The XGauge element has been replaced by XProgress / XRadial / XRamp
//          Please update your code according to the migration notes in:
//          https://github.com/ImpulseAdventure/GUIslice/pull/157
//          XGauge may be removed in a future release.
// *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


// ============================================================================
// Extended Element: Gauge
// ============================================================================

// Define unique identifier for extended element type
// - Select any number above GSLC_TYPE_BASE_EXTEND
#define  GSLC_TYPEX_GAUGE GSLC_TYPE_BASE_EXTEND + 0


/// Gauge drawing style
typedef enum {
    GSLCX_GAUGE_STYLE_PROG_BAR,  ///< Progress bar
    GSLCX_GAUGE_STYLE_RADIAL,    ///< Radial indicator
    GSLCX_GAUGE_STYLE_RAMP,      ///< Ramp indicator
} gslc_teXGaugeStyle;

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
  gslc_teXGaugeStyle  nStyle;         ///< Gauge sub-type
  gslc_tsColor        colGauge;       ///< Color of gauge fill bar
  gslc_tsColor        colTick;        ///< Color of gauge tick marks
  uint16_t            nTickCnt;       ///< Number of gauge tick marks
  uint16_t            nTickLen;       ///< Length of gauge tick marks
  bool                bVert;          ///< Vertical if true, else Horizontal
  bool                bFlip;          ///< Reverse direction of gauge
  uint16_t            nIndicLen;      ///< Indicator length
  uint16_t            nIndicTip;      ///< Size of tip at end of indicator
  bool                bIndicFill;     ///< Fill the indicator if true

} gslc_tsXGauge;


///
/// Create a Gauge Element
/// - Draws a gauge element that represents a proportion (nVal)
///   between nMin and nMax.
/// - Support gauge sub-types:
///   - GSLC_TYPEX_GAUGE_PROG_BAR: Horizontal or vertical box with filled region
///   - GSLC_TYPEX_GAUGE_RADIAL:   Radial / compass indicator
/// - Default appearance is a horizontal progress bar, but can be changed
///   with gslc_ElemXGaugeSetStyle())
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
gslc_tsElemRef* gslc_ElemXGaugeCreate(gslc_tsGui* pGui,int16_t nElemId,int16_t nPage,
  gslc_tsXGauge* pXData,gslc_tsRect rElem,int16_t nMin,int16_t nMax,int16_t nVal,gslc_tsColor colGauge,bool bVert);


///
/// Configure the style of a Gauge element
/// - This function is used to select between one of several gauge types
///   (eg. progress bar, radial dial, etc.)
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
/// \param[in]  nType:       Gauge style enumeration
///
/// \return none
///
void gslc_ElemXGaugeSetStyle(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,gslc_teXGaugeStyle nType);


///
/// Configure the appearance of the Gauge indicator
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
/// \param[in]  colGauge:    Color of the indicator
/// \param[in]  nIndicLen:   Length of the indicator
/// \param[in]  nIndicTip:   Size of the indicator tip
/// \param[in]  bIndicFill:  Fill in the indicator if true
///
/// \return none
///
void gslc_ElemXGaugeSetIndicator(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,gslc_tsColor colGauge,
        uint16_t nIndicLen,uint16_t nIndicTip,bool bIndicFill);


///
/// Configure the appearance of the Gauge ticks
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
/// \param[in]  colTick:     Color of the gauge ticks
/// \param[in]  nTickCnt:    Number of ticks to draw around / along gauge
/// \param[in]  nTickLen:    Length of the tick marks to draw
///
/// \return none
///
void gslc_ElemXGaugeSetTicks(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,gslc_tsColor colTick,uint16_t nTickCnt,uint16_t nTickLen);


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
void gslc_ElemXGaugeUpdate(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,int16_t nVal);


///
/// Set a Gauge element's fill direction
/// - Setting bFlip reverses the default fill direction
/// - Default fill direction for horizontal gauges: left-to-right
/// - Default fill direction for vertical gauges: bottom-to-top
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
/// \param[in]  bFlip:       If set, reverse direction of fill from default
///
/// \return none
///
void gslc_ElemXGaugeSetFlip(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,bool bFlip);


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
bool gslc_ElemXGaugeDraw(void* pvGui,void* pvElemRef,gslc_teRedrawType eRedraw);


///
/// Helper function to draw a gauge with style: progress bar
/// - Called from gslc_ElemXGaugeDraw()
///
/// \param[in]  pGui:        Ptr to GUI
/// \param[in]  pElemRef:    Ptr to Element reference
/// \param[in]  eRedraw:     Redraw status
///
/// \return true if success, false otherwise
///
bool gslc_ElemXGaugeDrawProgressBar(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,gslc_teRedrawType eRedraw);

#if (GSLC_FEATURE_XGAUGE_RADIAL)
///
/// Helper function to draw a gauge with style: radial
/// - Called from gslc_ElemXGaugeDraw()
///
/// \param[in]  pGui:        Ptr to GUI
/// \param[in]  pElemRef:    Ptr to Element reference
/// \param[in]  eRedraw:     Redraw status
///
/// \return true if success, false otherwise
///
bool gslc_ElemXGaugeDrawRadial(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,gslc_teRedrawType eRedraw);
#endif

#if (GSLC_FEATURE_XGAUGE_RAMP)
///
/// Helper function to draw a gauge with style: ramp
/// - Called from gslc_ElemXGaugeDraw()
///
/// \param[in]  pGui:        Ptr to GUI
/// \param[in]  pElemRef:    Ptr to Element reference
/// \param[in]  eRedraw:     Redraw status
///
/// \return true if success, false otherwise
///
bool gslc_ElemXGaugeDrawRamp(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,gslc_teRedrawType eRedraw);
#endif

// ============================================================================

// ------------------------------------------------------------------------
// Read-only element macros
// ------------------------------------------------------------------------

// Macro initializers for Read-Only Elements in Flash/PROGMEM
//


/// \def gslc_ElemXGaugeCreate_P(pGui,nElemId,nPage,nX,nY,nW,nH,
///      nMin_,nMax_,nVal_,colFrame_,colFill_,colGauge_,bVert_)
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
/// \param[in]  colGauge_:   Color for the gauge indicator
/// \param[in]  bVert_:      Flag to indicate vertical vs horizontal action
///                          (true = vertical, false = horizontal)
///
/// \return none
///


#if (GSLC_USE_PROGMEM)


#define gslc_ElemXGaugeCreate_P(pGui,nElemId,nPage,nX,nY,nW,nH,\
    nMin_,nMax_,nVal_,colFrame_,colFill_,colGauge_,bVert_) \
  static const uint16_t nFeatures##nElemId = GSLC_ELEM_FEA_VALID | \
    GSLC_ELEM_FEA_GLOW_EN | GSLC_ELEM_FEA_FILL_EN; \
  static gslc_tsXGauge sGauge##nElemId;                           \
  sGauge##nElemId.nMin = nMin_;                                   \
  sGauge##nElemId.nMax = nMax_;                                   \
  sGauge##nElemId.nVal = nVal_;                                   \
  sGauge##nElemId.nValLast = nVal_;                               \
  sGauge##nElemId.bValLastValid = false;                          \
  sGauge##nElemId.nStyle = GSLCX_GAUGE_STYLE_PROG_BAR;            \
  sGauge##nElemId.colGauge = colGauge_;                           \
  sGauge##nElemId.colTick = GSLC_COL_GRAY;                        \
  sGauge##nElemId.nTickCnt = 8;                                   \
  sGauge##nElemId.nTickLen = 5;                                   \
  sGauge##nElemId.bVert = bVert_;                                 \
  sGauge##nElemId.bFlip = false;                                  \
  sGauge##nElemId.nIndicLen = 10;                                 \
  sGauge##nElemId.nIndicTip = 3;                                  \
  sGauge##nElemId.bIndicFill = false;                             \
  static const gslc_tsElem sElem##nElemId PROGMEM = {             \
      nElemId,                                                    \
      nFeatures##nElemId,                                         \
      GSLC_TYPEX_GAUGE,                                           \
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
      &gslc_ElemXGaugeDraw,                                       \
      NULL,                                                       \
      NULL,                                                       \
  };                                                              \
  gslc_ElemAdd(pGui,nPage,(gslc_tsElem*)&sElem##nElemId,          \
    (gslc_teElemRefFlags)(GSLC_ELEMREF_SRC_PROG | GSLC_ELEMREF_VISIBLE | GSLC_ELEMREF_REDRAW_FULL));

#else


#define gslc_ElemXGaugeCreate_P(pGui,nElemId,nPage,nX,nY,nW,nH,\
    nMin_,nMax_,nVal_,colFrame_,colFill_,colGauge_,bVert_) \
  static const uint16_t nFeatures##nElemId = GSLC_ELEM_FEA_VALID | \
    GSLC_ELEM_FEA_GLOW_EN | GSLC_ELEM_FEA_FILL_EN; \
  static gslc_tsXGauge sGauge##nElemId;                           \
  sGauge##nElemId.nMin = nMin_;                                   \
  sGauge##nElemId.nMax = nMax_;                                   \
  sGauge##nElemId.nVal = nVal_;                                   \
  sGauge##nElemId.nValLast = nVal_;                               \
  sGauge##nElemId.bValLastValid = false;                          \
  sGauge##nElemId.nStyle = GSLCX_GAUGE_STYLE_PROG_BAR;            \
  sGauge##nElemId.colGauge = colGauge_;                           \
  sGauge##nElemId.colTick = GSLC_COL_GRAY;                        \
  sGauge##nElemId.nTickCnt = 8;                                   \
  sGauge##nElemId.nTickLen = 5;                                   \
  sGauge##nElemId.bVert = bVert_;                                 \
  sGauge##nElemId.bFlip = false;                                  \
  sGauge##nElemId.nIndicLen = 10;                                 \
  sGauge##nElemId.nIndicTip = 3;                                  \
  sGauge##nElemId.bIndicFill = false;                             \
  static const gslc_tsElem sElem##nElemId = {                     \
      nElemId,                                                    \
      nFeatures##nElemId,                                         \
      GSLC_TYPEX_GAUGE,                                           \
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
      &gslc_ElemXGaugeDraw,                                       \
      NULL,                                                       \
      NULL,                                                       \
  };                                                              \
  gslc_ElemAdd(pGui,nPage,(gslc_tsElem*)&sElem##nElemId,          \
    (gslc_teElemRefFlags)(GSLC_ELEMREF_SRC_CONST | GSLC_ELEMREF_VISIBLE | GSLC_ELEMREF_REDRAW_FULL));

#endif

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _GUISLICE_EX_XGAUGE_H_

