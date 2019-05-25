#ifndef _GUISLICE_EX_XKEYPAD_ALPHA_H_
#define _GUISLICE_EX_XKEYPAD_ALPHA_H_

#include "GUIslice.h"

#include "elem/XKeyPad.h"

// =======================================================================
// GUIslice library extension: XKeyPad control (alpha entry)
// - Paul Conti, Calvin Hass
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
/// \file XKeyPad.h

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#if (GSLC_FEATURE_COMPOUND)

// Define number of buttons & elements
// - Refer to the definitions in the XKeyPad_*.c file
#define XKEYPADALPHA_BTN_BASIC 26
#define XKEYPADALPHA_ELEM_MAX (6 + XKEYPADALPHA_BTN_BASIC)

// ============================================================================
// Extended Element: KeyPad Character entry
// - NOTE: The XKeyPad_Alpha extends the XKeyPad base element
// ============================================================================

typedef struct {
  // Base XKeyPad struct
  // - The base type must appear at the top of the derived struct
  gslc_tsXKeyPad      sKeyPad;                             ///< Base XKeyPad element

  gslc_tsElemRef      asElemRef[XKEYPADALPHA_ELEM_MAX];    ///< Storage for sub-element references
  gslc_tsElem         asElem[XKEYPADALPHA_ELEM_MAX];       ///< Storage for sub-elements
} gslc_tsXKeyPad_Alpha;

  ///
  /// Create a KeyPad Element
  ///
  /// \param[in]  pGui:        Pointer to GUI
  /// \param[in]  nElemId:     Element ID to assign (0..16383 or GSLC_ID_AUTO to autogen)
  /// \param[in]  nPage:       Page ID to attach element to
  /// \param[in]  pXData:      Ptr to extended element data structure
  /// \param[in]  nX0:         X KeyPad Starting Coordinate 
  /// \param[in]  nY0:         Y KeyPad Starting Coordinate 
  /// \param[in]  nFontId:     Font ID to use for drawing the element
  /// \param[in]  pConfig:     Ptr to config options
  ///
  /// \return Pointer to Element or NULL if failure
  ///
  gslc_tsElemRef* gslc_ElemXKeyPadCreate_Alpha(gslc_tsGui* pGui, int16_t nElemId, int16_t nPage,
    gslc_tsXKeyPad_Alpha* pXData, int16_t nX0, int16_t nY0, int8_t nFontId, gslc_tsXKeyPadCfg* pConfig);


  ///
  /// Initialize the KeyPad config structure
  /// - This routine should be called to initialize the configuration
  ///   data structure before calling any of the KeyPad config APIs
  ///
  /// \return Initialized KeyPad config structure
  ///
  gslc_tsXKeyPadCfg gslc_ElemXKeyPadCfgInit_Alpha();

// ============================================================================

#endif // GSLC_FEATURE_COMPOUND

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _GUISLICE_EX_XKEYPAD_ALPHA_H_

