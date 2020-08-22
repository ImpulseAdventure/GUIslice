#ifndef _GUISLICE_EX_XGLOWBALL_H_
#define _GUISLICE_EX_XGLOWBALL_H_

#include "GUIslice.h"


// =======================================================================
// GUIslice library extension: XGlowball
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
/// \file XGlowball.h

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


// ============================================================================
// Extended Element: Example template
// ============================================================================

// Define unique identifier for extended element type
// - Select any number above GSLC_TYPE_BASE_EXTEND
#define  GSLC_TYPEX_GLOW GSLC_TYPE_BASE_EXTEND + 24

// Defines the attributes of each ring in the XGlowball gauge
typedef struct {
  uint8_t        nRad1;      // Inner radius
  uint8_t        nRad2;      // Outer radius
  gslc_tsColor   cCol;       // Fill color
} gslc_tsXGlowballRing;

// Extended element data structures
// - These data structures are maintained in the gslc_tsElem
//   structure via the pXData pointer

/// Extended data for Slider element
typedef struct {
  // Config
  int16_t               nMidX;          ///< Gauge midpoint X coord
  int16_t               nMidY;          ///< Gauge midpoint Y coord
  gslc_tsXGlowballRing* pRings;         ///< Ring definition array
  uint8_t               nNumRings;      ///< Number of rings in definition
  // Style config
  uint16_t              nQuality;       ///< Rendering quality (number of segments / rotation)
  int16_t               nAngStart;      ///< Starting angle (0..510 degrees)
  int16_t               nAngEnd;        ///< Ending angle (0..510 degrees)
  gslc_tsColor          colBg;          ///< Background color (for redraw)
  // State
  int16_t               nVal;           ///< Current value
  int16_t               nValLast;       ///< Previous value
  // Callbacks
} gslc_tsXGlowball;


///
/// Create a XGlowball element
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nElemId:     Element ID to assign (0..16383 or GSLC_ID_AUTO to autogen)
/// \param[in]  nPage:       Page ID to attach element to
/// \param[in]  pXData:      Ptr to extended element data structure
/// \param[in]  nMidX:       Center X coordinate
/// \param[in]  nMidY:       Center Y coordinate
/// \param[in]  pRings:      Pointer to tsXGlowballRing structure array defining appearance
/// \param[in]  nNumRings:   Number of rings in pRings array
///
/// \return Pointer to Element reference or NULL if failure
///
gslc_tsElemRef* gslc_ElemXGlowballCreate(gslc_tsGui* pGui, int16_t nElemId, int16_t nPage,
  gslc_tsXGlowball* pXData, int16_t nMidX, int16_t nMidY, gslc_tsXGlowballRing* pRings, uint8_t nNumRings);


///
/// Draw the XGlowball element on the screen
/// - Called from gslc_ElemDraw()
///
/// \param[in]  pvGui:       Void ptr to GUI (typecast to gslc_tsGui*)
/// \param[in]  pvElemRef:   Void ptr to Element (typecast to gslc_tsElemRef*)
/// \param[in]  eRedraw:     Redraw mode
///
/// \return true if success, false otherwise
///
bool gslc_ElemXGlowballDraw(void* pvGui,void* pvElemRef,gslc_teRedrawType eRedraw);

// XGlowball draw helper routines
void drawXGlowballArc(gslc_tsGui* pGui, gslc_tsXGlowball* pGlowball, int16_t nMidX, int16_t nMidY, int16_t nRad1, int16_t nRad2, gslc_tsColor cArc, uint16_t nAngStart, uint16_t nAngEnd);
void drawXGlowballRing(gslc_tsGui* pGui, gslc_tsXGlowball* pGlowball, int16_t nMidX, int16_t nMidY, int16_t nVal, uint16_t nAngStart, uint16_t nAngEnd, bool bErase);
void drawXGlowball(gslc_tsGui* pGui, gslc_tsXGlowball* pGlowball, int16_t nMidX, int16_t nMidY, int16_t nVal, uint16_t nAngStart, uint16_t nAngEnd);

void gslc_ElemXGlowballSetAngles(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, int16_t nAngStart, int16_t nAngEnd);
void gslc_ElemXGlowballSetVal(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, int16_t nVal);
void gslc_ElemXGlowballSetQuality(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, uint16_t nQuality);
void gslc_ElemXGlowballSetColorBack(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, gslc_tsColor colBg);

// ============================================================================

// ------------------------------------------------------------------------
// Read-only element macros
// ------------------------------------------------------------------------

// Macro initializers for Read-Only Elements in Flash/PROGMEM
//


#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _GUISLICE_EX_XGLOWBALL_H_

