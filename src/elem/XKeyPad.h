#ifndef _GUISLICE_EX_XKEYPAD_H_
#define _GUISLICE_EX_XKEYPAD_H_

#include "GUIslice.h"


// =======================================================================
// GUIslice library extension: XKeyPad control
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
// ============================================================================
// Extended Element: KeyPad (Number Selector)
// - Demonstration of a compound element consisting of
//   a counter text field along with an increment and
//   decrement button.
// ============================================================================

// Define unique identifier for extended element type
// - Select any number above GSLC_TYPE_BASE_EXTEND
#define GSLC_TYPEX_KEYPAD GSLC_TYPE_BASE_EXTEND + 20

#define XKEYPAD_VAL_LEN  12  // Maximum buffer length for input string

// Define the status for GSLC_CB_INPUT callback
#define XKEYPAD_CB_STATE_DONE 1
#define XKEYPAD_CB_STATE_CANCEL 2
#define XKEYPAD_CB_STATE_UPDATE 3


// Define global list of button ID types
// - A given keypad may not use all of these
enum {
  // --------------------------------------------
  // --- Special Buttons
  KEYPAD_ID_BACKSPACE,
  KEYPAD_ID_PERIOD,
  KEYPAD_ID_SPACE,
  KEYPAD_ID_DECIMAL,
  KEYPAD_ID_MINUS,
  KEYPAD_ID_ESC,
  KEYPAD_ID_ENTER,
  // --- Basic Buttons
  KEYPAD_ID_BASIC_START=100,
  // --------------------------------------------
  // --- Extra elements
  KEYPAD_ID_TXT=200,        // Value string text area
};

/// Function for KeyPad creation
typedef int16_t (*XKEYPAD_LOOKUP)(gslc_tsGui* pGui, int16_t nKeyId);


// Extended element data structures
// - These data structures are maintained in the gslc_tsElem
//   structure via the pXData pointer

  /// Configuration for the KeyPad
  typedef struct {
    bool                bFloatEn;         ///< Enable floating point (ie. decimal point)
    bool                bSignEn;          ///< Enable negative numbers
    int8_t              nButtonSzW;       ///< Button width (in pixels)
    int8_t              nButtonSzH;       ///< Button height (in pixels)
    char**              pacKeys;          ///< Array of character strings for KeyPad labels

    // From constructor
    int16_t             nFontId;          ///< Configured font for KeyPad labels
    int16_t             nOffsetX;         ///< Configured offset (X direction) for buttons from parent container
    int16_t             nOffsetY;         ///< Configured offset (Y direction) for buttons from parent container

    int8_t              nFrameMargin;     ///< Margin around text value field
    uint8_t             nMaxCols;         ///< Maximum number of columns to occupy
    uint8_t             nMaxRows;         ///< Maximum number of rows to occupy
  } gslc_tsXKeyPadCfg;

  /// Extended data for KeyPad element
  typedef struct {

    // State
    uint8_t             nValStrPos;  ///< Counter for number digits stored
    char                acValStr[XKEYPAD_VAL_LEN];  ///< Where input is stored until enter button pressed
    bool                bValPositive;     ///< Value is positive if true, negative if false
    bool                bValDecimalPt;    ///> Value string includes decimal point

    // Config
    char**              pacKeys;          ///< Array of character strings for KeyPad labels
    gslc_tsXKeyPadCfg   sConfig;          ///< Configuration options

    GSLC_CB_INPUT       pfuncCb;          ///< Callback function for KeyPad actions
    XKEYPAD_LOOKUP      pfuncLookup;      ///< Callback function for converting key into key label

    // Internal sub-element members
    gslc_tsCollect      sCollect;         ///< Collection management for sub-elements
    uint8_t             nSubElemMax;      ///< Maximum number of sub-elements to create within KeyPad container
    gslc_tsElemRef*     psElemRef;        ///< Ptr to storage for sub-element references
    gslc_tsElem*        psElem;           ///< Ptr to storage for sub-elements

  } gslc_tsXKeyPad;


// Callback function for KeyPad creation
typedef void (*XKEYPAD_CREATE)(gslc_tsGui* pGui,gslc_tsXKeyPad* pXData);


  ///
  /// Add a key to the KeyPad control
  ///
  /// \param[in]  pGui:        Pointer to GUI
  /// \param[in]  pXData:      Ptr to extended element data structure
  /// \param[in]  nKeyId:      ID to associated with the key
  /// \param[in]  bTxtField:   Is this the text value field?
  /// \param[in]  nRow:        Element placement position (row index, 0 at top)
  /// \param[in]  nCol:        Element placement position (column index, 0 at left)
  /// \param[in]  nRowSpan:    Number of columns to occupy by element (1 for normal size, 2 for double width)
  /// \param[in]  nColSpan:    Number of rows to occupy by element (1 for normal size, 2 for double height)
  /// \param[in]  cColFill:    Fill color for element
  /// \param[in]  cColGlow:    Fill color for element when glowing
  ///
  /// \return none
  ///
void XKeyPadAddKeyElem(gslc_tsGui* pGui, gslc_tsXKeyPad* pXData, int16_t nKeyId, bool bTxtField, int16_t nRow, int16_t nCol,
  int8_t nRowSpan, int8_t nColSpan, gslc_tsColor cColFill, gslc_tsColor cColGlow);

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
  /// \param[in]  sConfig:     Config options
  ///
  /// \return Pointer to Element or NULL if failure
  ///
  gslc_tsElemRef* gslc_ElemXKeyPadCreateBase(gslc_tsGui* pGui, int16_t nElemId, int16_t nPage,
    gslc_tsXKeyPad* pXData, int16_t nX0, int16_t nY0, int8_t nFontId, gslc_tsXKeyPadCfg* pConfig,
    XKEYPAD_CREATE pfuncCreate, XKEYPAD_LOOKUP pfuncLookup);


  ///
  /// Set the current value for the editable text field
  ///
  /// \param[in]  pGui:        Pointer to GUI
  /// \param[in]  pKeyPad:     Ptr to KeyPad Element
  /// \param[in]  pStrBuf:     String to copy into keypad
  ///
  /// \return none
  ///
  void gslc_ElemXKeyPadValSet(gslc_tsGui* pGui, gslc_tsXKeyPad* pKeyPad, const char* pStrBuf);


  ///
  /// Set the current output string buffer associated with NumericInput element
  ///
  /// \param[in]  pGui:        Pointer to GUI
  /// \param[in]  pKeyPad:     Ptr to KeyPad Element
  /// \param[in]  pStrBuf:     String to copy into element
  /// \param[in]  nStrBufMax:  Maximum length of string buffer (pStrBuf)
  ///
  /// \return none
  ///
  bool gslc_ElemXKeyPadValGet(gslc_tsGui* pGui, gslc_tsXKeyPad* pKeyPad, char* pStrBuf, uint8_t nStrBufMax);

  ///
  /// Draw a KeyPad element on the screen
  /// - Called during redraw
  ///
  /// \param[in]  pvGui:       Void ptr to GUI (typecast to gslc_tsGui*)
  /// \param[in]  pvElem:      Void ptr to Element (typecast to gslc_tsElem*)
  /// \param[in]  eRedraw:     Redraw mode
  ///
  /// \return true if success, false otherwise
  ///
  bool gslc_ElemXKeyPadDraw(void* pvGui, void* pvElemRef, gslc_teRedrawType eRedraw);

  ///
  /// Handle a click event within the KeyPad
  /// - This is called internally by the KeyPad touch handler
  ///
  /// \param[in]  pvGui:       Void ptr to GUI (typecast to gslc_tsGui*)
  /// \param[in]  pvElem:      Void ptr to Element (typecast to gslc_tsElem*)
  /// \param[in]  eTouch:      Touch event type
  /// \param[in]  nX:          Touch X coord
  /// \param[in]  nY:          Touch Y coord
  ///
  /// \return none
  ///
  bool gslc_ElemXKeyPadClick(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY);

  ///
  /// Handle touch (up,down,move) events to KeyPad element
  /// - Called from gslc_ElemSendEventTouch()
  ///
  /// \param[in]  pvGui:       Void ptr to GUI (typecast to gslc_tsGui*)
  /// \param[in]  pvElem:      Void ptr to Element (typecast to gslc_tsElem*)
  /// \param[in]  eTouch:      Touch event type
  /// \param[in]  nRelX:       Touch X coord relative to element
  /// \param[in]  nRelY:       Touch Y coord relative to element
  ///
  /// \return true if success, false otherwise
  ///
  bool gslc_ElemXKeyPadTouch(void* pvGui, void* pvElemRef, gslc_teTouch eTouch, int16_t nRelX, int16_t nRelY);

  ///
  /// Set the callback function associated with the KeyPad
  /// - This function will be called during updates and OK / Cancel
  ///
  /// \param[in]  pGui:        Pointer to GUI
  /// \param[in]  pElemRef:    Pointer to Element Reference for KeyPad
  /// \param[in]  pfuncCb:     Callback function pointer
  ///
  /// \return none
  ///
  void gslc_ElemXKeyPadValSetCb(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, GSLC_CB_INPUT pfuncCb);

  ///
  /// Update the KeyPad configuration to enable floating point numbers
  /// - Effectively disables/enables the decimal point button & handling
  ///
  /// \param[in]  pGui:        Pointer to GUI
  /// \param[in]  pConfig:     Pointer to the XKeyPad config structure
  /// \param[in]  bEn:         Enable flag (true if floating point enabled)
  ///
  /// \return none
  ///
  void gslc_ElemXKeyPadCfgSetFloatEn(gslc_tsXKeyPadCfg* pConfig,bool bEn);

  ///
  /// Update the KeyPad configuration to enable negative numbers
  /// - Effectively disables/enables the sign button & handling
  ///
  /// \param[in]  pGui:        Pointer to GUI
  /// \param[in]  pConfig:     Pointer to the XKeyPad config structure
  /// \param[in]  bEn:         Enable flag (true if negative numbers enabled)
  ///
  /// \return none
  ///
  void gslc_ElemXKeyPadCfgSetSignEn(gslc_tsXKeyPadCfg* pConfig,bool bEn);

  ///
  /// Update the KeyPad configuration to define the KeyPad button sizing
  ///
  /// \param[in]  pGui:        Pointer to GUI
  /// \param[in]  pConfig:     Pointer to the XKeyPad config structure
  /// \param[in]  nButtonSzW:  Width of buttons in pixels
  /// \param[in]  nButtonSzH:  Width of buttons in pixels
  ///
  /// \return none
  ///
  void gslc_ElemXKeyPadCfgSetButtonSz(gslc_tsXKeyPadCfg* pConfig, int8_t nButtonSzW, int8_t nButtonSzH);

  #endif // GSLC_FEATURE_COMPOUND

// ============================================================================

// ------------------------------------------------------------------------
// Read-only element macros
// ------------------------------------------------------------------------

// Macro initializers for Read-Only Elements in Flash/PROGMEM
//
#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _GUISLICE_EX_XKEYPAD_H_

