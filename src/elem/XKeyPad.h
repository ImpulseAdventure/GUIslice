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
/// \file XKeyPad.h

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


// ============================================================================
// Extended Element: KeyPad (Number Selector)
// - Demonstration of a compound element consisting of
//   a counter text field along with an increment and
//   decrement button.
// ============================================================================

// ============================================================================
// User-configurable options
// - The following options can be adjusted by the user if needed

#define XKEYPAD_BUF_MAX  20  // Maximum buffer length for input string
#define XKEYPAD_KEY_LEN  6   // Maximum buffer length for a single key


// Select cursor appearance:
// - 1 : Colorized cursor, intended for monospaced fonts
// - 0 : Normal cursor, can use proportional fonts
#define XKEYPAD_CURSOR_ENHANCED   1    // Cursor appearance
#define XKEYPAD_CURSOR_CH         '^'  // Character to use for embedded cursor


// ============================================================================

// Define unique identifier for extended element type
// - Select any number above GSLC_TYPE_BASE_EXTEND
#define GSLC_TYPEX_KEYPAD GSLC_TYPE_BASE_EXTEND + 20

// Define the status for GSLC_CB_INPUT callback
#define XKEYPAD_CB_STATE_DONE   1
#define XKEYPAD_CB_STATE_CANCEL 2
#define XKEYPAD_CB_STATE_UPDATE 3

// Bit mask to define what needs to be redrawn
static const int RBIT_TXT     = 1;    // Redraw the text field
static const int RBIT_KEYONE  = 2;    // Redraw a single key (defined by nXKeyPadRedrawId)
static const int RBIT_KEYALL  = 4;    // Redraw all keys
static const int RBIT_CTRL    = 128;  // Redraw the entire control

// Definitions for XKeyPad redraw state (eRedrawState)
#define XKEYPAD_REDRAW_NONE   0                               // No need for a redraw
#define XKEYPAD_REDRAW_TXT    RBIT_TXT                        // Redraw the text field
#define XKEYPAD_REDRAW_KEY    RBIT_KEYONE                     // Redraw a single key (defined by nXKeyPadRedrawId)
#define XKEYPAD_REDRAW_ALL    RBIT_KEYALL+RBIT_TXT            // Redraw all keys and text field
#define XKEYPAD_REDRAW_FULL   RBIT_KEYALL+RBIT_TXT+RBIT_CTRL  // Redraw entire control (frame, all keys and text)

#define DEBUG_XKEYPAD         0 ///< Debug message for XKeyPad (1=enabled, 0=disabled)

/// Return status for XKeyPad
/// - Includes any pending redraw state
typedef struct {
  int16_t eRedrawState;         ///< XKeyPad pending redraw state
  int16_t nRedrawKeyId1;        ///< XKeyPad specific key (#1) to redraw (-1 for none)
  int16_t nRedrawKeyId2;        ///< XKeyPad specific key (#2) to redraw (-1 for none)
} gslc_tsXKeyPadResult;


// Define global list of common button ID types
// - A given keypad may not use all of these
enum {
  // --- Special Buttons (0..15)
  KEYPAD_ID_BACKSPACE = 0,
  KEYPAD_ID_SPACE,
  KEYPAD_ID_ESC,
  KEYPAD_ID_ENTER,
  KEYPAD_ID_SWAP_PAD,
  KEYPAD_ID_SCROLL_LEFT,
  KEYPAD_ID_SCROLL_RIGHT,  

  // --- Variant-specific special buttons
  //     These are defined within the variant's "-setup.c"
  //     And are reserved the ID range 16..31

  // --- Basic Buttons
  KEYPAD_ID_BASIC_START = 32,
  // --- Text field
  KEYPAD_ID_TXT = 240,
  // --- Terminators
  KEYPAD_ID__END = 255
};

enum {
  E_XKEYPAD_TYPE_BASIC,       // Basic button (single character)
  E_XKEYPAD_TYPE_SPECIAL,     // Special button, static content
  E_XKEYPAD_TYPE_TXT,         // Text field
  E_XKEYPAD_TYPE_UNUSED,      // Unused key
  E_XKEYPAD_TYPE_END,         // Terminator indicator (end of key list)
};

typedef enum {
  E_XKEYPAD_ATTRIB_FOCUS,
  E_XKEYPAD_ATTRIB_GLOW,
} gslc_tsXKeyPadAttrib;


// Extended element data structures
// - These data structures are maintained in the gslc_tsElem
//   structure via the pXData pointer

  /// Key information. Defines everything we need to know about a particular key.
  typedef struct gslc_tsKey {
    uint8_t             nId;       ///< Unique identifier
    uint8_t             nRow;      ///< Row to place the key (0 is top-most)
    uint8_t             nCol;      ///< Column to place the key (0 is left-most)
    uint8_t             nRowSpan;  ///< Number of rows that key takes up (in units of nButtonSzH pixels)
    uint8_t             nColSpan;  ///< Number of columns that key takes up (in units of nButtonSzW pixels)
    uint8_t             nType;     ///< Key type
  } gslc_tsKey;

  /// Key Label strings for special buttons
  /// - Includes ID and string
  typedef struct {
    uint8_t             nId;      /// Unique key ID
    const char*         pLabel;   /// Key label string (zero terminated)
  } gslc_tsLabelSpecial;

  // Internal callback functions for KeyPad
  typedef void (*GSLC_CB_XKEYPAD_RESET)(void* pvKeyPadConfig);
  typedef void (*GSLC_CB_XKEYPAD_TXT_INIT)(void* pvKeyPad);
  typedef void (*GSLC_CB_XKEYPAD_LABEL_GET)(void* pvKeyPad,uint8_t nId,uint8_t nStrMax,char* pStr);
  typedef void (*GSLC_CB_XKEYPAD_SYTLE_GET)(void* pvKeyPad,uint8_t nId, bool* bVisible, gslc_tsColor* pcolTxt, gslc_tsColor *pcolFrame, gslc_tsColor *pcolFill, gslc_tsColor *pcolGlow);
  typedef void (*GSLC_CB_XKEYPAD_BTN_EVT)(void* pvKeyPad,uint8_t nId,gslc_tsXKeyPadResult* psResult);

  /// Configuration for the KeyPad
  typedef struct {
    uint8_t             nDispMax;         ///< Maximum length to display

	  bool                bRoundEn;         ///< Enable rounded corners
    int8_t              nButtonSzW;       ///< Button width (in pixels)
    int8_t              nButtonSzH;       ///< Button height (in pixels)
    int8_t              nButtonSpaceX;    ///< Button X spacing (in pixels)
    int8_t              nButtonSpaceY;    ///< Button Y spacing (in pixels)
    gslc_tsKey*         pLayout;          ///< Current selected layout
    gslc_tsKey**        pLayouts;         ///< Key Positions for each KeyPad layout
    int8_t              eLayoutDef;       ///< Default KeyPad layout (type gslc_teXKeyPadSel)
    int8_t              eLayoutSel;       ///< Current KeyPad layout (type gslc_teXKeyPadSel)
    
    // From constructor
    int16_t             nFontId;          ///< Configured font for KeyPad labels
    int16_t             nOffsetX;         ///< Configured offset (X direction) for buttons from parent container
    int16_t             nOffsetY;         ///< Configured offset (Y direction) for buttons from parent container

    int8_t              nFrameMargin;     ///< Margin around text value field
    uint8_t             nMaxCols;         ///< Maximum number of columns to occupy
    uint8_t             nMaxRows;         ///< Maximum number of rows to occupy

    GSLC_CB_XKEYPAD_RESET       pfuncReset;     ///< Callback function whenever keypad needs to be reset
    GSLC_CB_XKEYPAD_TXT_INIT    pfuncTxtInit;   ///< Callback function whenever text string initialized
    GSLC_CB_XKEYPAD_LABEL_GET   pfuncLabelGet;  ///< Callback function to get a key label
    GSLC_CB_XKEYPAD_SYTLE_GET   pfuncStyleGet;  ///< Callback function to get a key's style
    GSLC_CB_XKEYPAD_BTN_EVT     pfuncBtnEvt;    ///< Callback function to handle a key

  } gslc_tsXKeyPadCfg;

  /// Input callback data structure
  /// - This struct is returned in GSLC_CB_INPUT when the
  ///   KeyPad edits are complete, and is used to provide
  ///   the resulting edited value.
  typedef struct {
    char*               pStr;             ///< Final value of edited value field
    gslc_tsElemRef*     pTargetRef;       ///< Target element reference to receive the value
  } gslc_tsXKeyPadData;

  /// Extended data for KeyPad element
  typedef struct {

    // State
    uint8_t             nBufferMax;                 ///< Maximum number of characters stored in edit value string
    uint8_t             nBufferLen;                 ///< Current number of characters stored in edit value string
    char                acBuffer[XKEYPAD_BUF_MAX];  ///< Buffer storage for edit value string
    uint8_t             nCursorPos;                 ///< Cursor position within the buffer
    uint8_t             nScrollPos;                 ///< Display offset within the buffer

    gslc_tsXKeyPadResult  sRedraw;        ///< Pending redraw state
    int16_t               nFocusKeyInd;   ///< Indicate key in focus (GSLC_IND_NONE if none)
    int16_t               nGlowKeyInd;    ///< Indicate key in glow (GSLC_IND_NONE if none)

    // Config
    gslc_tsXKeyPadCfg*  pConfig;          ///< Ptr to config struct (may be derived variant)

    GSLC_CB_INPUT       pfuncCb;          ///< Callback function for KeyPad actions
    gslc_tsElemRef*     pTargetRef;       ///< Target element ref associated with keypad (GSLC_CB_INPUT)
    
  } gslc_tsXKeyPad;

  ///
  /// Provide default initialization for the base XKeyPad
  /// - These defaults will be overwritten by variant-specific
  ///   initialization or through user configuration APIs.
  ///
  /// \param[in]  pConfig:     Ptr to the KeyPad base config structure
  ///
  /// \return none
  ///
  void gslc_ElemXKeyPadCfgInit(gslc_tsXKeyPadCfg* pConfig);

  ///
  /// Find a key ID within a KeyPad label array and return
  /// it's index into the array.
  /// - It is expected that the KeyPad label array is
  ///   terminated with KEYPAD_ID__END
  ///
  /// \param[in]  pKeys:       Ptr to the Keypad label array
  /// \param[in]  nKeyId:      Key ID to look for
  ///
  /// \return the index into the array if the ID was found or
  ///         -1 if the key ID was not found
  ///
  int16_t gslc_XKeyPadLookupId(gslc_tsKey* pKeys, uint8_t nKeyId);


  ///
  /// Find a key ID within a KeyPad special label array and return
  /// it's index into the array.
  /// - It is expected that the KeyPad label array is
  ///   terminated with KEYPAD_ID__END
  ///
  /// \param[in]  pLabels:     Ptr to the Keypad special label array
  /// \param[in]  nKeyId:      Key ID to look for
  ///
  /// \return the index into the array if the ID was found or
  ///         -1 if the key ID was not found
  ///
  int16_t gslc_XKeyPadLookupSpecialId(gslc_tsLabelSpecial* pLabels, uint8_t nKeyId);

  ///
  /// Add a character to the KeyPad text field at the specified offset (nPos).
  /// Providing an offset equal to the end of the existing buffer length
  /// will cause the addition to the end, whereas an offset within the
  /// buffer will cause an insert.
  /// - An addition that causes the buffer length to exceed the
  ///   maximum allowed will result in the end of the resulting
  ///   buffer to be truncated.
  /// - Typically the addition will be done at the current
  ///   cursor position.
  /// - If the insertion is ahead of the cursor, then the
  ///   cursor position may be increased.
  ///
  /// \param[in]  pKeyPad:     Ptr to the KeyPad
  /// \param[in]  ch:          Character to add
  /// \param[in]  nPos:        Buffer position for the insertion
  ///
  /// \return true if the text field should be redrawn,
  ///         false if no redraw is needed (ie. no change)
  ///
  bool gslc_XKeyPadTxtAddCh(gslc_tsXKeyPad* pKeyPad,char ch,uint8_t nPos);

  ///
  /// Add a string to the KeyPad text field at the specified offset (nPos).
  /// Providing an offset equal to the end of the existing buffer length
  /// will cause the addition to the end, whereas an offset within the
  /// buffer will cause an insert.
  /// - An addition that causes the buffer length to exceed the
  ///   maximum allowed will result in the end of the resulting
  ///   buffer to be truncated.
  /// - Typically the addition will be done at the current
  ///   cursor position.
  /// - If the insertion is ahead of the cursor, then the
  ///   cursor position may be increased.
  /// - This routine may be useful when adding multi-byte characters
  ///   for future support of foreign characters.
  ///
  /// \param[in]  pKeyPad:     Ptr to the KeyPad
  /// \param[in]  pStr:        String to add
  /// \param[in]  nPos:        Buffer position for the insertion
  ///
  /// \return true if the text field should be redrawn,
  ///         false if no redraw is needed (ie. no change)
  ///
  bool gslc_XKeyPadTxtAddStr(gslc_tsXKeyPad* pKeyPad,const char* pStr,uint8_t nPos);

  ///
  /// Remove a character from the KeyPad text field at the specified offset (nPos).
  /// - Typically the addition will be done at the current
  ///   cursor position.
  /// - If the removal is ahead of the cursor, then the
  ///   cursor position may be decreased.
  ///
  /// \param[in]  pKeyPad:     Ptr to the KeyPad
  /// \param[in]  nPos:        Buffer position for the removal
  ///
  /// \return true if the text field should be redrawn,
  ///         false if no redraw is needed (ie. no change)
  ///
  bool gslc_XKeyPadTxtDelCh(gslc_tsXKeyPad* pKeyPad,uint8_t nPos);

  ///
  /// Select a new KeyPad layout
  /// - Multiple KeyPad layouts can share the same key
  ///   key definition array (eg. KEYPAD_LAYOUT)
  /// - This function returns an indication of whether
  ///   a full KeyPad control redraw is required, ie.
  ///   the KeyPad layout definition has changed. With
  ///   changes in the KeyPad definition, there may be
  ///   a different number of visible keys or arrangements,
  ///   necessitating a background redraw.
  ///
  /// \param[in]  pConfig:     Ptr to the KeyPad configuration
  /// \param[in]  eLayoutSel:  Layout index to select
  ///
  /// \return true if a full redraw should be performed
  ///
  bool gslc_XKeyPadLayoutSet(gslc_tsXKeyPadCfg* pConfig,int8_t eLayoutSel);


  ///
  /// Calculate the overall dimensions of the KeyPad control encompassing
  /// all available layouts for the KeyPad, leveraging the computation
  /// in gslc_XKeyPadSizeGet().
  ///
  /// \param[in]  pLayouts:    Ptr to the array of KeyPad layouts
  /// \param[in]  nNumLayouts: Number of layouts in pLayouts
  /// \param[out] pnRows:      Ptr for the number of rows
  /// \param[out] pnCols:      Ptr for the number of columns
  ///
  /// \return none
  ///
  void gslc_XKeyPadSizeAllGet(gslc_tsKey** pLayouts, uint8_t nNumLayouts, uint8_t* pnRows, uint8_t* pnCols);


  ///
  /// Calculate the overall dimensions of the KeyPad control encompassing
  /// the text field and key buttons. The dimension is calculated in
  /// units of the configured key size (width and height), and accounts
  /// for any column spans. It also returns the index of the first and
  /// last keys on the keypad.
  ///
  /// \param[in]  pLayout:     Ptr to the KeyPad layout
  /// \param[out] pnRows:      Ptr for the number of rows
  /// \param[out] pnCols:      Ptr for the number of columns
  /// \param[out] pnIndFirst:  Ptr for the index of first key
  /// \param[out] pnIndLast:   Ptr for the index of last key
  ///
  /// \return none
  ///
  void gslc_XKeyPadSizeGet(gslc_tsKey* pLayout, uint8_t* pnRows, uint8_t* pnCols, int8_t* pnIndFirst, int8_t* pnIndLast);

  ///
  /// Draw a key to the screen
  ///
  /// \param[in]  pGui:        Pointer to GUI
  /// \param[in]  pXData:      Ptr to extended element data structure
  /// \param[in]  pKey:        Ptr to key being drawn
  /// \param[in]  bGlow:       Indicate if key is in glow state
  /// \param[in]  bFocus:      Indicate if key is in focus state
  ///
  /// \return none
  ///
  void gslc_XKeyPadDrawKey(gslc_tsGui* pGui, gslc_tsXKeyPad* pXData, gslc_tsKey* pKey, bool bGlow, bool bFocus);

  ///
  /// Create a KeyPad Element
  ///
  /// \param[in]  pGui:          Pointer to GUI
  /// \param[in]  nElemId:       Element ID to assign (0..16383 or GSLC_ID_AUTO to autogen)
  /// \param[in]  nPage:         Page ID to attach element to
  /// \param[in]  pXData:        Ptr to extended element data structure
  /// \param[in]  nX0:           X KeyPad Starting Coordinate 
  /// \param[in]  nY0:           Y KeyPad Starting Coordinate 
  /// \param[in]  nFontId:       Font ID to use for drawing the element
  /// \param[in]  pConfig:       Pointer to base Config options
  ///
  /// \return Pointer to Element or NULL if failure
  ///
  gslc_tsElemRef* gslc_XKeyPadCreateBase(gslc_tsGui* pGui, int16_t nElemId, int16_t nPage,
    gslc_tsXKeyPad* pXData, int16_t nX0, int16_t nY0, int8_t nFontId, gslc_tsXKeyPadCfg* pConfig);


  ///
  /// Set the current value for the editable text field
  ///
  /// \param[in]  pGui:        Pointer to GUI
  /// \param[in]  pElemRef:    Ptr to KeyPad Element reference
  /// \param[in]  pStrBuf:     String to copy into keypad
  ///
  /// \return none
  ///
  void gslc_ElemXKeyPadValSet(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, const char* pStrBuf);


  ///
  /// Set target element reference for KeyPad return value
  /// - The Target Reference is used in the GSLC_CB_INPUT callback so that the user
  ///   has the context needed to determine which field should be edited
  ///   with the contents of the KeyPad edit field
  /// - It is expected that the user will call this function when showing
  ///   the KeyPad popup dialog
  ///
  /// \param[in]  pGui:       Pointer to GUI
  /// \param[in]  pElemRef:   Ptr to KeyPad Element reference
  /// \param[in]  pTargetRef: Element reference for target of KeyPad value
  ///
  /// \return none
  ///
  void gslc_ElemXKeyPadTargetRefSet(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, gslc_tsElemRef* pTargetRef);


  ///
  /// Fetch the current value string associated with KeyPad element
  ///
  /// \param[in]  pGui:        Pointer to GUI
  /// \param[in]  pElemRef:    Ptr to KeyPad Element reference
  /// \param[out] pStrBuf:     String buffer 
  /// \param[in]  nStrBufMax:  Maximum length of string buffer (pStrBuf) including terminator
  ///
  /// \return true if success, false otherwise
  ///
  bool gslc_ElemXKeyPadValGet(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, char* pStrBuf, uint8_t nStrBufMax);

  ///
  /// Fetch the final value string of the KeyPad from a callback
  ///
  /// \param[in]  pGui:       Pointer to GUI
  /// \param[out] pvData :    Void ptr to callback data structure
  ///
  /// \return Pointer to edited character string
  ///
  char* gslc_ElemXKeyPadDataValGet(gslc_tsGui* pGui, void* pvData);


  ///
  /// Fetch the element target ID associated with this KeyPad
  ///
  /// \param[in]  pGui:       Pointer to GUI
  /// \param[in]  pvData :    Void ptr to callback data structure
  ///
  /// \return Target Element ID or GSLC_ID_NONE if unspecified
  ///
  int16_t gslc_ElemXKeyPadDataTargetIdGet(gslc_tsGui* pGui, void* pvData);

  /// \todo Doc
  void gslc_XKeyPadPendRedrawReset(gslc_tsXKeyPadResult* pResult);

  /// \todo Doc
  void gslc_XKeyPadPendRedrawAddKey(gslc_tsXKeyPadResult* pResult,int16_t nId);

  /// \todo Doc
  void gslc_XKeyPadPendRedrawAddTxt(gslc_tsXKeyPadResult* pResult);

  /// \todo Doc
  void gslc_XKeyPadTrackSet(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,int16_t nInd,gslc_tsXKeyPadAttrib eAttrib);

  /// \todo Doc
  void gslc_XKeyPadFocusSetDefault(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef);

  /// \todo Doc
  void gslc_XKeyPadRedrawUpdate(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef);

  ///
  /// Draw a KeyPad element on the screen
  /// - Called during redraw
  ///
  /// \param[in]  pvGui:       Void ptr to GUI (typecast to gslc_tsGui*)
  /// \param[in]  pvElemRef:   Void ptr to Element reference (typecast to gslc_tsElemRef*)
  /// \param[in]  eRedraw:     Redraw mode
  ///
  /// \return true if success, false otherwise
  ///
  bool gslc_XKeyPadDraw(void* pvGui, void* pvElemRef, gslc_teRedrawType eRedraw);

  ///
  /// Handle touch (up,down,move) events to KeyPad element
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
  bool gslc_XKeyPadTouch(void* pvGui, void* pvElemRef, gslc_teTouch eTouch, int16_t nRelX, int16_t nRelY);

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
  /// Update the KeyPad configuration to enable rounded button corners
  ///
  /// \param[inout] pConfig:     Pointer to the XKeyPad base config structure
  /// \param[in]    bEn:         Enable rounded corners
  ///
  /// \return none
  ///
  void gslc_ElemXKeyPadCfgSetRoundEn(gslc_tsXKeyPadCfg* pConfig,bool bEn);

  ///
  /// Update the KeyPad configuration to define the KeyPad button sizing
  ///
  /// \param[inout] pConfig:     Pointer to the XKeyPad base config structure
  /// \param[in]    nButtonSzW:  Width of buttons in pixels
  /// \param[in]    nButtonSzH:  Height of buttons in pixels
  ///
  /// \return none
  ///
  void gslc_ElemXKeyPadCfgSetButtonSz(gslc_tsXKeyPadCfg* pConfig, int8_t nButtonSzW, int8_t nButtonSzH);

  ///
  /// Update the KeyPad configuration to define the KeyPad button spacing
  /// - This defines the inset amount (X and Y) from the Button Size
  /// - A spacing of (1,1) will mean that the button is drawn with a
  ///   1 pixel margin around the grid defined by the Button Size
  ///
  /// \param[inout] pConfig:     Pointer to the XKeyPad base config structure
  /// \param[in]    nSpaceX:     Amount to inset button in horizontal direction (pixels)
  /// \param[in]    nSpaceY:     Amount to inset button in vertical direction (pixels)
  ///
  /// \return none
  ///
  void gslc_ElemXKeyPadCfgSetButtonSpace(gslc_tsXKeyPadCfg* pConfig, int8_t nSpaceX, int8_t nSpaceY);


  /// Draw a virtual Text Element
  /// - Creates a text string with filled background
  ///
  /// \param[in]  pGui:        Pointer to GUI
  /// \param[in]  rElem:       Rectangle coordinates defining element size
  /// \param[in]  pKeyPad:     Pointer to KeyPad struct
  /// \param[in]  cColFrame:   Frame color for element
  /// \param[in]  cColFill:    Fill color for element
  /// \param[in]  cColTxt:     Text color for element
  ///
  /// \return none
  ///
  void gslc_XKeyPadDrawVirtualTxt(gslc_tsGui* pGui,gslc_tsRect rElem,gslc_tsXKeyPad* pKeyPad,
    gslc_tsColor cColFrame, gslc_tsColor cColFill, gslc_tsColor cColTxt);

  ///
  /// Draw a virtual textual Button Element
  ///
  /// \param[in]  pGui:          Pointer to GUI
  /// \param[in]  rElem:         Rectangle coordinates defining element size
  /// \param[in]  pStrBuf:       String to copy into element
  /// \param[in]  nStrBufMax:    Maximum length of string buffer (pStrBuf). 
  /// \param[in]  nFontId:       Font ID to use for text display
  /// \param[in]  cColFrame:     Frame color for element
  /// \param[in]  cColFill:      Fill color for element
  /// \param[in]  cColFillGlow:  Fill color for element when glowing/focused
  /// \param[in]  cColTxt:       Text color for element
  /// \param[in]  bRoundedEn:    Use Rounded Corners?
  /// \param[in]  bGlow:         Indicate btn is in glow state
  /// \param[in]  bFocus:        Indicate btn is in focus state
  ///
  /// \return none
  ///
  void gslc_XKeyPadDrawVirtualBtn(gslc_tsGui* pGui, gslc_tsRect rElem,
    char* pStrBuf,uint8_t nStrBufMax,int16_t nFontId, gslc_tsColor cColFrame,
    gslc_tsColor cColFill, gslc_tsColor cColFillGlow, gslc_tsColor cColTxt,
    bool bRoundedEn, bool bGlow, bool bFocus);


  ///
  /// Trigger a KeyPad popup and associate it with a text element
  /// - This function also updates the maximum KeyPad buffer length
  ///   to match that of the target text element, up to the maximum
  ///   XKEYPAD_BUF_MAX.
  ///
  /// \param[in]  pGui:        Pointer to GUI
  /// \param[in]  pKeyPadRef:  Pointer to KeyPad element reference
  /// \param[in]  nPgPopup:    Page enum that contains the popup to show
  /// \param[in]  pTxtRef:     Pointer to associated text field element reference
  ///
  /// \return none
  ///
  void gslc_ElemXKeyPadInputAsk(gslc_tsGui* pGui, gslc_tsElemRef* pKeyPadRef, int16_t nPgPopup, gslc_tsElemRef* pTxtRef);

  ///
  /// Complete a KeyPad popup by retrieving the input data and storing it in the text element
  ///
  /// \param[in]  pGui:        Pointer to GUI
  /// \param[in]  pTxtRef:     Pointer to associated text field element reference
  /// \param[in]  pvCbData:    Void pointer to callback function's pvData
  ///
  /// \return The text string that was fetched from the KeyPad (NULL terminated)
  ///
  char* gslc_ElemXKeyPadInputGet(gslc_tsGui* pGui, gslc_tsElemRef* pTxtRef, void* pvCbData);

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

