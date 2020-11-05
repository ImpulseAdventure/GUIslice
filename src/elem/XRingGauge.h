#ifndef _GUISLICE_EX_XRING_H_
#define _GUISLICE_EX_XRING_H_

#include "GUIslice.h"


// =======================================================================
// GUIslice library extension: XRingGauge
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
/// \file XRingGauge.h

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


// ============================================================================
// Extended Element: XRingGauge
// - Creates display element similar to a donut-chart.
// - The element has an outer and inner radius to create a ring appearance.
// - The ring has an angular range defined by SetAngleRange, which means that
//   the ring can be configured to cover a full circle or just a portion of
//   a circle.
// - SetAngleRange defines the starting angle and direction of fill.
// - When drawing the ring within the angular range, it is composed of
//   an active region (the angular region from the start to the current
//   position value) and an inactive region (from the current value to the
//   end of the angular range). The inactive region can be hidden (by
//   setting it to the background color).
// - A text value can be drawn in the center of the ring, typically to
//   show the current value. The color defined by SetColorBackground() is
//   used when redrawing the text.
// ============================================================================

// Define unique identifier for extended element type
// - Select any number above GSLC_TYPE_BASE_EXTEND
#define  GSLC_TYPEX_RING GSLC_TYPE_BASE_EXTEND + 23


// Extended element data structures
// - These data structures are maintained in the gslc_tsElem
//   structure via the pXData pointer

#define XRING_STR_MAX 10

/// Extended data for XRingGauge element
typedef struct {
  // Config
  int16_t           nValMin;
  int16_t           nValMax;

  int16_t           nAngStart;
  int16_t           nAngRange;

  // Style config
  int16_t           nQuality;
  int8_t            nThickness;
  bool              bGradient;
  uint8_t           nSegGap;
  gslc_tsColor      colRing1;
  gslc_tsColor      colRing2;
  gslc_tsColor      colRingRemain;

  // State
  int16_t           nVal;           ///< Current position value
  int16_t           nValLast;       ///< Previous position value
  char              acStrLast[XRING_STR_MAX];

  // Callbacks

} gslc_tsXRingGauge;


///
/// Create an XRingGauge element
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nElemId:     Element ID to assign (0..16383 or GSLC_ID_AUTO to autogen)
/// \param[in]  nPage:       Page ID to attach element to
/// \param[in]  pXData:      Ptr to extended element data structure
/// \param[in]  rElem:       The square box that bounds the ring element. If a rectangular region is
///                          provided, then the ring control will be centered in the long axis.
/// \param[in]  pStrBuf:     String buffer to use for gauge inner text
/// \param[in]  nStrBufMax:  Maximum length of string buffer (pStrBuf)
/// \param[in]  nFontId:     Font ID to use for text display
///
/// \return Pointer to Element reference or NULL if failure
///
gslc_tsElemRef* gslc_ElemXRingGaugeCreate(gslc_tsGui* pGui, int16_t nElemId, int16_t nPage,
  gslc_tsXRingGauge* pXData, gslc_tsRect rElem, char* pStrBuf, uint8_t nStrBufMax, int16_t nFontId);

///
/// Draw the template element on the screen
/// - Called from gslc_ElemDraw()
///
/// \param[in]  pvGui:       Void ptr to GUI (typecast to gslc_tsGui*)
/// \param[in]  pvElemRef:   Void ptr to Element reference (typecast to gslc_tsElemRef*)
/// \param[in]  eRedraw:     Redraw mode
///
/// \return true if success, false otherwise
///
bool gslc_ElemXRingGaugeDraw(void* pvGui,void* pvElemRef,gslc_teRedrawType eRedraw);


///
/// Set an Ring Gauge current indicator value
///
/// Updates the current value of the ring gauge. The active region will be drawn up
/// to the position defined by nVal within the value range defined by SetValRange(nMin,nMax).
/// A SetVal() close to nMin will cause a very small active region to be drawn and a large
/// remainder drawn in the inactive color, whereas a SetVal() close to nMax will cause a
/// more complete active region to be drawn.When SetVal() equals nMax, the entire angular
/// range will be drawn in the active color (and no inactive region).
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
/// \param[in]  nVal:        New position value
///
/// \return none
///
void gslc_ElemXRingGaugeSetVal(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,int16_t nVal);

///
/// Defines the angular range of the gauge, including both the active
/// and inactive regions.
///
/// - nStart defines the angle at the beginning of the active region.
/// - The current position marks the end of the active region and the
///   beginning of the inactive region.
/// - nRange defines the angular range from the start of the active
///   region to the end of the inactive region. In most cases, a
///   range of 360 degrees is used.
/// - All angles are measured in units of degrees.
/// - Angles are measured with 0 at the top, 90 towards the right,
///   180 towards the bottom, 270 towards the left, etc.
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
/// \param[in]  nStart:      Define angle of start of active region (measured in degrees)
/// \param[in]  nRange:      Define angular range from strt of active region
///                          to end of the inactive region (measured in degrees)
/// \param[in]  bClockwise:  Defines the direction in which the active
///                          region grows (true for clockwise) [FORCED TRUE, FOR FUTURE IMPLEMENTATION]
///
/// \return none
///
void gslc_ElemXRingGaugeSetAngleRange(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, int16_t nStart, int16_t nRange, bool bClockwise);


/// Defines the range of values that may be passed into SetVal(), used to
/// scale the input to SetVal().
/// - Default is 0..100.
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
/// \param[in]  nValMin:     Minimum value
/// \param[in]  nValMax:     Maximum value
///
/// \return none
///
void gslc_ElemXRingGaugeSetValRange(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, int16_t nValMin, int16_t nValMax);


/// Defines the thickness of the ring arcs. More specifically, it defines the reduction
/// in radius from the outer radius to the inner radius in pixels.
/// - Default thickness is 10 pixels
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
/// \param[in]  nThickness:  Thickness of ring
///
/// \return none
///
void gslc_ElemXRingGaugeSetThickness(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, int8_t nThickness);

/// Sets the quality of the ring drawing by defining the number of segments that are used when
/// rendering a 360 degree gauge.The larger the number, the more segments are used and the smoother the curve.
/// A larger ring gauge may need a higher quality number to maintain a smoothed curve appearance.
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
/// \param[in]  nSegments:   Number of arc segments to render a complete circle. The
///                          higher the value, the smoother the ring.
///                          Note that 360/nSegments should be an integer result,
///                          thus the allowable quality settings are: 360 (max quality),
///                          180, 120, 90, 72, 60, 45, 40, 36 (low quality), etc. 
///
/// \return none
///
void gslc_ElemXRingGaugeSetQuality(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, uint16_t nSegments);

/// Defines the color of the inactive region to be a flat (constant) color.
/// The inactive color is often set to be the same as the background but it can
/// be set to a different color to indicate the remainder of the value range that is yet to be filled.
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
/// \param[in]  colInactive: Color of inactive region
///
/// \return none
///
void gslc_ElemXRingGaugeSetColorInactive(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, gslc_tsColor colInactive);

/// Defines the color of the active region to be a flat (constant) color.
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
/// \param[in]  colActive:   Color of active region
///
/// \return none
///
void gslc_ElemXRingGaugeSetColorActiveFlat(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, gslc_tsColor colActive);

/// Defines the color of the active region to be a gradient using two color stops. The
/// active region will be filled according to the proportion between nMin and nMax.
/// The gradient is defined by a linear RGB blend between the two color stops(colStart and colEnd)
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
/// \param[in]  colStart:    Starting color of gradient fill
/// \param[in]  colEnd:      Ending color of gradient fill
///
/// \return none
///
void gslc_ElemXRingGaugeSetColorActiveGradient(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, gslc_tsColor colStart, gslc_tsColor colEnd);

// ============================================================================

// ------------------------------------------------------------------------
// Read-only element macros
// ------------------------------------------------------------------------

// Macro initializers for Read-Only Elements in Flash/PROGMEM
//


#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _GUISLICE_EX_XRING_H_

