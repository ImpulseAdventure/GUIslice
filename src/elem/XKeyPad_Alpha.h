#ifndef _GUISLICE_EX_XKEYPAD_ALPHA_H_
#define _GUISLICE_EX_XKEYPAD_ALPHA_H_

#include "GUIslice.h"

#include "elem/XKeyPad.h"

// =======================================================================
// GUIslice library extension: XKeyPad control (alphanumeric entry)
// - Paul Conti, Calvin Hass
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
/// \file XKeyPad_Alpha.h

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


// ============================================================================
// Extended Element: KeyPad Alphanumeric entry
// - NOTE: The XKeyPad_Alpha extends the XKeyPad base element
// ============================================================================

// Variant XKeyPad config structure
typedef struct {
    // Common configuration
    gslc_tsXKeyPadCfg   sBaseCfg;         ///< KeyPad base config struct

    // Variant-specific configuration

    // Variant-specific state
    
} gslc_tsXKeyPadCfg_Alpha;

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
    gslc_tsXKeyPad* pXData, int16_t nX0, int16_t nY0, int8_t nFontId, gslc_tsXKeyPadCfg_Alpha* pConfig);


  ///
  /// Initialize the KeyPad config structure
  /// - This routine should be called to initialize the configuration
  ///   data structure before calling any of the KeyPad config APIs
  ///
  /// \return Initialized KeyPad config structure
  ///
  gslc_tsXKeyPadCfg_Alpha gslc_ElemXKeyPadCfgInit_Alpha();

  ///
  /// Callback function to reset internal state
  ///
  /// \param[in]  pvConfig:    Void ptr to the KeyPad config
  ///
  /// \return none
  ///
  void gslc_ElemXKeyPadReset_Alpha(void* pvConfig);


  ///
  /// Callback function to update internal state whenever the text
  /// field is manually set via gslc_ElemXKeyPadValSet().
  /// - This is used to ensure any KeyPad variant state can be
  ///   kept in sync with the text string.
  /// - For example, if a numeric KeyPad is initiaized with a
  ///   string that contains a minus sign, an internal negation
  ///   flag might be set.
  ///
  /// \param[in]  pvKeyPad:    Void ptr to the KeyPad
  ///
  /// \return none
  ///
  void gslc_ElemXKeyPadTxtInit_Alpha(void* pvKeyPad);


  ///
  /// Callback function to retrieve the label associated with
  /// a KeyPad button. This is called during the drawing of
  /// the KeyPad layout.
  ///
  /// \param[in]  pvKeyPad:    Void ptr to the KeyPad
  /// \param[in]  nId:         KeyPad key ID
  /// \param[in]  nStrMax:     Maximum length of return string (including NULL)
  /// \param[out] pStr:        Buffer for the returned label
  ///
  /// \return none
  ///
  void gslc_ElemXKeyPadLabelGet_Alpha(void* pvKeyPad,uint8_t nId,uint8_t nStrMax,char* pStr);


  ///
  /// Callback function to retrieve the style associated with
  /// a KeyPad button. This is called during the drawing of
  /// the KeyPad layout.
  /// - This function is used to assign the color and visibility
  ///   state of the keys at runtime.
  /// - This function can also be used to change the appearance
  ///   dynamically, according to internal state (eg. dimmed buttons).
  ///
  /// \param[in]  pvKeyPad:    Void ptr to the KeyPad
  /// \param[in]  nId:         KeyPad key ID
  /// \param[out] pbVisible:   The returned visibility state
  /// \param[out] pcolTxt:     The returned text color
  /// \param[out] pcolFrame:   The returned key's frame color
  /// \param[out] pcolFill:    The returned key's fill color
  /// \param[out] pcolGlow:    The returned key's fill color when highlighted
  ///
  /// \return none
  ///
  void gslc_ElemXKeyPadStyleGet_Alpha(void* pvKeyPad,uint8_t nId, bool* pbVisible, gslc_tsColor* pcolTxt, gslc_tsColor* pcolFrame, gslc_tsColor* pcolFill, gslc_tsColor* pcolGlow);


  ///
  /// Callback function activated when a key has been pressed.
  /// This callback is used to enable the KeyPad variant to
  /// handle any events associated with the key press and
  /// update any internal state.
  /// - The callback is also used to determine whether any
  ///   redraw actions need to be taken.
  ///
  /// \param[in]  pvKeyPad:    Void ptr to the KeyPad
  /// \param[in]  nId:         KeyPad key ID
  /// \param[out] psResult:    The returned state vector (including redraw)
  ///
  /// \return none
  ///
  void gslc_ElemXKeyPadBtnEvt_Alpha(void* pvKeyPad,uint8_t nId,gslc_tsXKeyPadResult* psResult);

// ============================================================================

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _GUISLICE_EX_XKEYPAD_ALPHA_H_

