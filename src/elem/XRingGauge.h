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
// Copyright 2016-2019 Calvin Hass
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
// - NOTE: This element type is new and the APIs are subject to change
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
  int16_t           nPosMin;
  int16_t           nPosMax;

  // Style config
  uint16_t          nDeg64PerSeg;
  int8_t            nThickness;
  bool              bGradient;
  uint8_t           nSegGap;
  gslc_tsColor      colRing1;
  gslc_tsColor      colRing2;
  gslc_tsColor      colRingRemain;

  // State
  int16_t           nPos;           ///< Current position value
  int16_t           nPosLast;       ///< Previous position value
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
/// \param[in]  rElem:       Rectangle coordinates defining element size
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
/// \param[in]  pvElemRef:   Void ptr to Element (typecast to gslc_tsElemRef*)
/// \param[in]  eRedraw:     Redraw mode
///
/// \return true if success, false otherwise
///
bool gslc_ElemXRingGaugeDraw(void* pvGui,void* pvElemRef,gslc_teRedrawType eRedraw);


///
/// Set an XRingGauge element's current position
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
/// \param[in]  nPos:        New position value
///
/// \return none
///
void gslc_ElemXRingGaugeSetPos(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,int16_t nPos);

/// \todo
void gslc_ElemXRingGaugeSetRange(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, int16_t nPosMin, int16_t nPosMax);
void gslc_ElemXRingGaugeSetThickness(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, int8_t nThickness);
void gslc_ElemXRingGaugeSetQuality(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, uint16_t nSegments);
void gslc_ElemXRingGaugeSetRingColorInactive(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, gslc_tsColor colInactive);
void gslc_ElemXRingGaugeSetRingColorFlat(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, gslc_tsColor colActive);
void gslc_ElemXRingGaugeSetRingColorGradient(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, gslc_tsColor colStart, gslc_tsColor colEnd);

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

