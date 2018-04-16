#ifndef _GUISLICE_EX_H_
#define _GUISLICE_EX_H_

#include "GUIslice.h"


// =======================================================================
// GUIslice library (extensions)
// - Calvin Hass
// - https://www.impulseadventure.com/elec/guislice-gui.html
// - https://github.com/ImpulseAdventure/GUIslice
// =======================================================================
//
// The MIT License
//
// Copyright 2018 Calvin Hass
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


// Extended element definitions
// - This file extends the core GUIslice functionality with
//   additional widget types

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// Extended element enumerations

/// Extended Element types
typedef enum {
    // Extended elements:
    GSLC_TYPEX_GAUGE        ///< Gauge extended element
            = GSLC_TYPE_BASE_EXTEND,  // Continue from gslc_teTypeBase enum range
    GSLC_TYPEX_CHECKBOX,    ///< Checkbox extended element
    GSLC_TYPEX_SLIDER,      ///< Slider extended element
#if (GSLC_FEATURE_COMPOUND)
    GSLC_TYPEX_SELNUM,      ///< SelNum extended element
#endif
    GSLC_TYPEX_TEXTBOX,     ///< Textbox extended element
    GSLC_TYPEX_GRAPH,       ///< Graph extended element
} gslc_teTypeExtend;


// Extended element data structures
// - These data structures are maintained in the gslc_tsElem
//   structure via the pXData pointer



// ============================================================================
// Extended Element: Gauge
// ============================================================================

/// Gauge drawing style
typedef enum {
    GSLCX_GAUGE_STYLE_PROG_BAR,  ///< Progress bar
    GSLCX_GAUGE_STYLE_RADIAL,    ///< Radial indicator
    GSLCX_GAUGE_STYLE_RAMP,      ///< Ramp indicator
} gslc_teXGaugeStyle;

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
// Extended Element: Checkbox
// ============================================================================

/// Checkbox drawing style
typedef enum {
  GSLCX_CHECKBOX_STYLE_BOX,                 ///< Inner box
  GSLCX_CHECKBOX_STYLE_X,                   ///< Crossed
  GSLCX_CHECKBOX_STYLE_ROUND,               ///< Circular
} gslc_teXCheckboxStyle;

/// Extended data for Checkbox element
typedef struct {
  bool                        bRadio;       ///< Radio-button operation if true
  gslc_teXCheckboxStyle       nStyle;       ///< Drawing style for element
  bool                        bChecked;     ///< Indicates if it is selected (checked)
  gslc_tsColor                colCheck;     ///< Color of checked inner fill
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
// Extended Element: Slider
// ============================================================================

/// Callback function for slider feedback
typedef bool (*GSLC_CB_XSLIDER_POS)(void* pvGui,void* pvElem,int16_t nPos);

/// Extended data for Slider element
typedef struct {
  // Config
  bool            bVert;          ///< Orientation: true if vertical, else horizontal
  int16_t         nThumbSz;       ///< Size of the thumb control
  int16_t         nPosMin;        ///< Minimum position value of the slider
  int16_t         nPosMax;        ///< Maximum position value of the slider
  // Style config
  uint16_t        nTickDiv;       ///< Style: number of tickmark divisions (0 for none)
  int16_t         nTickLen;       ///< Style: length of tickmarks
  gslc_tsColor    colTick;        ///< Style: color of ticks
  bool            bTrim;          ///< Style: show a trim color
  gslc_tsColor    colTrim;        ///< Style: color of trim
  // State
  int16_t         nPos;           ///< Current position value of the slider
  // Callbacks
  GSLC_CB_XSLIDER_POS pfuncXPos;  ///< Callback func ptr for position update
} gslc_tsXSlider;


///
/// Create a Slider Element
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nElemId:     Element ID to assign (0..16383 or GSLC_ID_AUTO to autogen)
/// \param[in]  nPage:       Page ID to attach element to
/// \param[in]  pXData:      Ptr to extended element data structure
/// \param[in]  rElem:       Rectangle coordinates defining checkbox size
/// \param[in]  nPosMin:     Minimum position value
/// \param[in]  nPosMax:     Maximum position value
/// \param[in]  nPos:        Starting position value
/// \param[in]  nThumbSz:    Size of the thumb control
/// \param[in]  bVert:       Orientation (true for vertical)
///
/// \return Pointer to Element reference or NULL if failure
///
gslc_tsElemRef* gslc_ElemXSliderCreate(gslc_tsGui* pGui,int16_t nElemId,int16_t nPage,
  gslc_tsXSlider* pXData,gslc_tsRect rElem,int16_t nPosMin,int16_t nPosMax,int16_t nPos,
  uint16_t nThumbSz,bool bVert);


///
/// Set a Slider element's current position
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
/// \param[in]  bTrim:       Show a colored trim?
/// \param[in]  colTrim:     Color of trim
/// \param[in]  nTickDiv:    Number of tick divisions to show (0 for none)
/// \param[in]  nTickLen:    Length of tickmarks
/// \param[in]  colTick:     Color of ticks
///
/// \return none
///
void gslc_ElemXSliderSetStyle(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,
        bool bTrim,gslc_tsColor colTrim,uint16_t nTickDiv,
        int16_t nTickLen,gslc_tsColor colTick);


///
/// Get a Slider element's current position
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
///
/// \return Current slider position
///
int gslc_ElemXSliderGetPos(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef);


///
/// Set a Slider element's current position
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
/// \param[in]  nPos:        New position value
///
/// \return none
///
void gslc_ElemXSliderSetPos(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,int16_t nPos);

///
/// Assign the position callback function for a slider
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
/// \param[in]  funcCb:      Function pointer to position routine (or NULL for none)
///
/// \return none
///
void gslc_ElemXSliderSetPosFunc(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,GSLC_CB_XSLIDER_POS funcCb);

///
/// Draw a Slider element on the screen
/// - Called from gslc_ElemDraw()
///
/// \param[in]  pvGui:       Void ptr to GUI (typecast to gslc_tsGui*)
/// \param[in]  pvElemRef:   Void ptr to Element (typecast to gslc_tsElemRef*)
/// \param[in]  eRedraw:     Redraw mode
///
/// \return true if success, false otherwise
///
bool gslc_ElemXSliderDraw(void* pvGui,void* pvElemRef,gslc_teRedrawType eRedraw);

///
/// Handle touch events to Slider element
/// - Called from gslc_ElemSendEventTouch()
///
/// \param[in]  pvGui:       Void ptr to GUI (typecast to gslc_tsGui*)
/// \param[in]  pvElemRef:   Void ptr to Element ref (typecast to gslc_tsElemRef*)
/// \param[in]  eTouch:      Touch event type
/// \param[in]  nRelX:       Touch X coord relative to element
/// \param[in]  nRelY:       Touch Y coord relative to element
///
/// \return true if success, false otherwise
///
bool gslc_ElemXSliderTouch(void* pvGui,void* pvElemRef,gslc_teTouch eTouch,int16_t nRelX,int16_t nRelY);


#if (GSLC_FEATURE_COMPOUND)
// ============================================================================
// Extended Element: SelNum (Number Selector)
// - Demonstration of a compound element consisting of
//   a counter text field along with an increment and
//   decrement button.
// ============================================================================

#define SELNUM_STR_LEN  6

/// Extended data for SelNum element
typedef struct {

  // Core functionality for SelNum
  int16_t             nCounter;       ///< Counter for demo purposes

  // Internal sub-element members
  gslc_tsCollect      sCollect;       ///< Collection management for sub-elements
  gslc_tsElemRef      asElemRef[4];   ///< Storage for sub-element references
  gslc_tsElem         asElem[4];      ///< Storage for sub-elements

  #if (GSLC_LOCAL_STR == 0)
  // If elements don't provide their own internal string buffer, then
  // we need to provide storage here in the extended data of the compound
  // element. For now, simply provide a fixed-length memory buffer.
  char                acElemTxt[4][SELNUM_STR_LEN]; ///< Storage for strings
  #endif
} gslc_tsXSelNum;




///
/// Create a SelNum Element
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nElemId:     Element ID to assign (0..16383 or GSLC_ID_AUTO to autogen)
/// \param[in]  nPage:       Page ID to attach element to
/// \param[in]  pXData:      Ptr to extended element data structure
/// \param[in]  rElem:       Rectangle coordinates defining element size
/// \param[in]  nFontId:     Font ID to use for drawing the element
///
/// \return Pointer to Element or NULL if failure
///
gslc_tsElemRef* gslc_ElemXSelNumCreate(gslc_tsGui* pGui,int16_t nElemId,int16_t nPage,
  gslc_tsXSelNum* pXData,gslc_tsRect rElem,int8_t nFontId);


///
/// Draw a SelNum element on the screen
/// - Called during redraw
///
/// \param[in]  pvGui:       Void ptr to GUI (typecast to gslc_tsGui*)
/// \param[in]  pvElem:      Void ptr to Element (typecast to gslc_tsElem*)
/// \param[in]  eRedraw:     Redraw mode
///
/// \return true if success, false otherwise
///
bool gslc_ElemXSelNumDraw(void* pvGui,void* pvElemRef,gslc_teRedrawType eRedraw);


///
/// Get the current counter associated with SelNum
///
/// \param[in]  pGui:        Ptr to GUI
/// \param[in]  pSelNum:     Ptr to Element
///
/// \return Current counter value
///
int gslc_ElemXSelNumGetCounter(gslc_tsGui* pGui,gslc_tsXSelNum* pSelNum);


///
/// Set the current counter associated with SelNum
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pSelNum:     Ptr to Element
/// \param[in]  nCount:      New counter value
///
/// \return none
///
void gslc_ElemXSelNumSetCounter(gslc_tsGui* pGui,gslc_tsXSelNum* pSelNum,int16_t nCount);


///
/// Handle a click event within the SelNum
/// - This is called internally by the SelNum touch handler
///
/// \param[in]  pvGui:       Void ptr to GUI (typecast to gslc_tsGui*)
/// \param[in]  pvElem:      Void ptr to Element (typecast to gslc_tsElem*)
/// \param[in]  eTouch:      Touch event type
/// \param[in]  nX:          Touch X coord
/// \param[in]  nY:          Touch Y coord
///
/// \return none
///
bool gslc_ElemXSelNumClick(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY);

///
/// Handle touch (up,down,move) events to SelNum element
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
bool gslc_ElemXSelNumTouch(void* pvGui,void* pvElemRef,gslc_teTouch eTouch,int16_t nRelX,int16_t nRelY);
#endif // GLSC_COMPOUND

// ============================================================================
// Extended Element: Textbox
// ============================================================================

/// Definitions for textbox special inline codes
#define GSLC_XTEXTBOX_CODE_COL_SET    187
#define GSLC_XTEXTBOX_CODE_COL_RESET  188


/// Extended data for Textbox element
typedef struct {
  // Config
  char*                       pBuf;         ///< Ptr to the text buffer (circular buffer))
  uint8_t                     nMargin;      ///< Margin for text area within element rect
  bool                        bWrapEn;      ///< Enable for line wrapping

  uint16_t                    nBufRows;     ///< Number of rows in buffer
  uint16_t                    nBufCols;     ///< Number of columns in buffer
  bool                        bScrollEn;    ///< Enable for scrollbar
  uint16_t                    nScrollPos;   ///< Current scrollbar position

  // Precalculated params
  uint8_t                     nChSizeX;     ///< Width of characters (pixels)
  uint8_t                     nChSizeY;     ///< Height of characters (pixels)
  uint8_t                     nWndCols;     ///< Window X size
  uint8_t                     nWndRows;     ///< Window Y size
  // Current status
  uint8_t                     nCurPosX;     ///< Cursor X position
  uint8_t                     nCurPosY;     ///< Cursor Y position
  uint8_t                     nBufPosX;     ///< Buffer X position
  uint8_t                     nBufPosY;     ///< Buffer Y position
  uint8_t                     nWndRowStart; ///< First row of current window

} gslc_tsXTextbox;



///
/// Create a Textbox Element
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nElemId:     Element ID to assign (0..16383 or GSLC_ID_AUTO to autogen)
/// \param[in]  nPage:       Page ID to attach element to
/// \param[in]  pXData:      Ptr to extended element data structure
/// \param[in]  rElem:       Rectangle coordinates defining checkbox size
/// \param[in]  nFontId:     Font ID to use for text area
/// \param[in]  pBuf:        Ptr to text buffer (already allocated)
///                          with size (nBufRows*nBufCols) chars
/// \param[in]  nBufRows:    Number of rows in buffer
/// \param[in]  nBufCols:    Number of columns in buffer (incl special codes)
///
/// \return Pointer to Element reference or NULL if failure
///
gslc_tsElemRef* gslc_ElemXTextboxCreate(gslc_tsGui* pGui,int16_t nElemId,int16_t nPage,
  gslc_tsXTextbox* pXData,gslc_tsRect rElem,int16_t nFontId,char* pBuf,
        uint16_t nBufRows,uint16_t nBufCols);


/// Reset the contents of the textbox
/// - Clears the buffer and resets the position
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
///
/// \return none
///
void gslc_ElemXTextboxReset(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef);


///
/// Draw a Textbox element on the screen
/// - Called from gslc_ElemDraw()
///
/// \param[in]  pvGui:       Void ptr to GUI (typecast to gslc_tsGui*)
/// \param[in]  pvElemRef:   Void ptr to Element reference (typecast to gslc_tsElemRef*)
/// \param[in]  eRedraw:     Redraw mode
///
/// \return true if success, false otherwise
///
bool gslc_ElemXTextboxDraw(void* pvGui,void* pvElemRef,gslc_teRedrawType eRedraw);

/// Add a text string to the textbox
/// - If it includes a newline then the buffer will
///   advance to the next row
/// - If wrap has been enabled, then a newline will
///   be forced
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
/// \param[in]  pTxt         Pointer to text string (null-terminated)
///
/// \return none
///
void gslc_ElemXTextboxAdd(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,char* pTxt);

///
/// Insert a color set code into the current buffer position
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
/// \param[in]  nCol:        Color to assign for next text written to textbox
///
/// \return none
///
void gslc_ElemXTextboxColSet(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,gslc_tsColor nCol);

///
/// Insert a color reset code into the current buffer position
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
///
/// \return none
///
void gslc_ElemXTextboxColReset(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef);

///
/// Enable or disable line wrap within textbox
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
/// \param[in]  bWrapEn:     Enable line wrap if true
///
/// \return none
///
void gslc_ElemXTextboxWrapSet(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,bool bWrapEn);


///
/// Set the textbox scroll position (nScrollPos) as a fraction of
/// nScrollMax
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
/// \param[in]  nScrollPos:  New scroll position
/// \param[in]  nScrollMax:  Maximum scroll position
///
/// \return none
///
void gslc_ElemXTextboxScrollSet(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,uint8_t nScrollPos,uint8_t nScrollMax);



// ============================================================================
// Extended Element: Graph
// ============================================================================

/// Graph drawing style
typedef enum {
  GSLCX_GRAPH_STYLE_DOT,        ///< Dot
  GSLCX_GRAPH_STYLE_LINE,       ///< Line
  GSLCX_GRAPH_STYLE_FILL,       ///< Filled
} gslc_teXGraphStyle;

/// Extended data for Graph element
typedef struct {
  // Config
  int16_t*                  pBuf;           ///< Ptr to the data buffer (circular buffer))
  uint8_t                   nMargin;        ///< Margin for graph area within element rect
  gslc_tsColor              colGraph;       ///< Color of the graph
  gslc_teXGraphStyle        eStyle;         ///< Style of the graph

  uint16_t                  nBufMax;        ///< Maximum number of points in buffer
  bool                      bScrollEn;      ///< Enable for scrollbar
  uint16_t                  nScrollPos;     ///< Current scrollbar position

  uint16_t                  nWndHeight;     ///< Visible window height
  uint16_t                  nWndWidth;      ///< Visible window width
  int16_t                   nPlotValMax;    ///< Visible window maximum value
  int16_t                   nPlotValMin;    ///< Visible window minimum value
  uint16_t                  nPlotIndMax;    ///< Number of data points to show in window

  // Current status
  uint16_t                  nBufCnt;        ///< Number of points in buffer
  uint16_t                  nPlotIndStart;  ///< First row of current window

} gslc_tsXGraph;



///
/// Create a Graph Element
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nElemId:     Element ID to assign (0..16383 or GSLC_ID_AUTO to autogen)
/// \param[in]  nPage:       Page ID to attach element to
/// \param[in]  pXData:      Ptr to extended element data structure
/// \param[in]  rElem:       Rectangle coordinates defining checkbox size
/// \param[in]  nFontId:     Font ID to use for graph area
/// \param[in]  pBuf:        Ptr to data buffer (already allocated)
///                          with size (nBufMax) int16_t
/// \param[in]  nBufRows:    Maximum number of points in buffer
/// \param[in]  colGraph:    Color of the graph
///
/// \return Pointer to Element reference or NULL if failure
///
gslc_tsElemRef* gslc_ElemXGraphCreate(gslc_tsGui* pGui,int16_t nElemId,int16_t nPage,
  gslc_tsXGraph* pXData,gslc_tsRect rElem,int16_t nFontId,int16_t* pBuf,
  uint16_t nBufRows,gslc_tsColor colGraph);

///
/// Set the graph's additional drawing characteristics
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
/// \param[in]  eStyle:      Drawing style for the graph
/// \param[in]  nMargin:     Margin to provide around graph area inside frame
///
/// \return none
///
void gslc_ElemXGraphSetStyle(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,
        gslc_teXGraphStyle eStyle,uint8_t nMargin);

///
/// Set the graph's drawing range
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
/// \param[in]  nYMin:       Minimum Y value to draw
/// \param[in]  nYMax:       Maximum Y value to draw
///
/// \return none
///
void gslc_ElemXGraphSetRange(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,
        int16_t nYMin,int16_t nYMax);

///
/// Draw a Graph element on the screen
/// - Called from gslc_ElemDraw()
///
/// \param[in]  pvGui:       Void ptr to GUI (typecast to gslc_tsGui*)
/// \param[in]  pvElemRef:   Void ptr to Element reference (typecast to gslc_tsElemRef*)
/// \param[in]  eRedraw:     Redraw mode
///
/// \return true if success, false otherwise
///
bool gslc_ElemXGraphDraw(void* pvGui,void* pvElemRef,gslc_teRedrawType eRedraw);

/// Add a value to the graph at the latest position
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
/// \param[in]  nVal:        Data value to add
///
/// \return none
///
void gslc_ElemXGraphAdd(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,int16_t nVal);


///
/// Set the graph scroll position (nScrollPos) as a fraction of
/// nScrollMax
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
/// \param[in]  nScrollPos:  New scroll position
/// \param[in]  nScrollMax:  Maximum scroll position
///
/// \return none
///
void gslc_ElemXGraphScrollSet(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,uint8_t nScrollPos,uint8_t nScrollMax);


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


/// \def gslc_ElemXSliderCreate_P(pGui,nElemId,nPage,nX,nY,nW,nH,
///      nPosMin_,nPosMax_,nPos_,nThumbSz_,bVert_,colFrame_,colFill_)
///
/// Create a Slider Element in Flash
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nElemId:     Unique element ID to assign
/// \param[in]  nPage:       Page ID to attach element to
/// \param[in]  nX:          X coordinate of element
/// \param[in]  nY:          Y coordinate of element
/// \param[in]  nW:          Width of element
/// \param[in]  nH:          Height of element
/// \param[in]  nPosMin_:    Minimum position value
/// \param[in]  nPosMax_:    Maximum position value
/// \param[in]  nPos_:       Starting position value
/// \param[in]  nThumbSz_:   Size of the thumb control
/// \param[in]  bVert_:      Orientation (true for vertical)
/// \param[in]  colFrame_:   Color of the element frame
/// \param[in]  colFill_:    Color of the element fill
///
/// \return none
///


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

#define gslc_ElemXCheckboxCreate_P(pGui,nElemId,nPage,nX,nY,nW,nH,colFill,bFillEn,nGroup,bRadio_,nStyle_,colCheck_,bChecked_) \
  static const uint8_t nFeatures##nElemId = GSLC_ELEM_FEA_VALID | \
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
      NULL,                                                       \
      (void*)(&sCheckbox##nElemId),                               \
      NULL,                                                       \
      &gslc_ElemXCheckboxDraw,                                    \
      &gslc_ElemXCheckboxTouch,                                   \
      NULL,                                                       \
  };                                                              \
  gslc_ElemAdd(pGui,nPage,(gslc_tsElem*)&sElem##nElemId,          \
    (gslc_teElemRefFlags)(GSLC_ELEMREF_SRC_PROG | GSLC_ELEMREF_REDRAW_FULL));


#define gslc_ElemXSliderCreate_P(pGui,nElemId,nPage,nX,nY,nW,nH, \
    nPosMin_,nPosMax_,nPos_,nThumbSz_,bVert_,colFrame_,colFill_) \
  static const uint8_t nFeatures##nElemId = GSLC_ELEM_FEA_VALID | \
    GSLC_ELEM_FEA_GLOW_EN | GSLC_ELEM_FEA_CLICK_EN | GSLC_ELEM_FEA_FILL_EN; \
  static gslc_tsXSlider sSlider##nElemId;                         \
  sSlider##nElemId.bVert = bVert_;                                \
  sSlider##nElemId.nThumbSz = nThumbSz_;                          \
  sSlider##nElemId.nPosMin = nPosMin_;                            \
  sSlider##nElemId.nPosMax = nPosMax_;                            \
  sSlider##nElemId.nTickDiv = 0;                                  \
  sSlider##nElemId.nTickLen = 0;                                  \
  sSlider##nElemId.colTick = GSLC_COL_WHITE;                      \
  sSlider##nElemId.bTrim = false;                                 \
  sSlider##nElemId.colTrim = GSLC_COL_BLACK;                      \
  sSlider##nElemId.nPos = nPos_;                                  \
  sSlider##nElemId.pfuncXPos = NULL;                              \
  static const gslc_tsElem sElem##nElemId PROGMEM = {             \
      nElemId,                                                    \
      nFeatures##nElemId,                                         \
      GSLC_TYPEX_SLIDER,                                          \
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
      NULL,                                                       \
      (void*)(&sSlider##nElemId),                                 \
      NULL,                                                       \
      &gslc_ElemXSliderDraw,                                      \
      &gslc_ElemXSliderTouch,                                     \
      NULL,                                                       \
  };                                                              \
  gslc_ElemAdd(pGui,nPage,(gslc_tsElem*)&sElem##nElemId,          \
    (gslc_teElemRefFlags)(GSLC_ELEMREF_SRC_PROG | GSLC_ELEMREF_REDRAW_FULL));


#define gslc_ElemXGaugeCreate_P(pGui,nElemId,nPage,nX,nY,nW,nH,\
    nMin_,nMax_,nVal_,colFrame_,colFill_,colGauge_,bVert_) \
  static const uint8_t nFeatures##nElemId = GSLC_ELEM_FEA_VALID | \
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
      NULL,                                                       \
      (void*)(&sGauge##nElemId),                                  \
      NULL,                                                       \
      &gslc_ElemXGaugeDraw,                                       \
      NULL,                                                       \
      NULL,                                                       \
  };                                                              \
  gslc_ElemAdd(pGui,nPage,(gslc_tsElem*)&sElem##nElemId,          \
    (gslc_teElemRefFlags)(GSLC_ELEMREF_SRC_PROG | GSLC_ELEMREF_REDRAW_FULL));

#else

#define gslc_ElemXCheckboxCreate_P(pGui,nElemId,nPage,nX,nY,nW,nH,colFill,bFillEn,nGroup,bRadio_,nStyle_,colCheck_,bChecked_) \
  static const uint8_t nFeatures##nElemId = GSLC_ELEM_FEA_VALID | \
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
      NULL,                                                       \
      (void*)(&sCheckbox##nElemId),                               \
      NULL,                                                       \
      &gslc_ElemXCheckboxDraw,                                    \
      &gslc_ElemXCheckboxTouch,                                   \
      NULL,                                                       \
  };                                                              \
  gslc_ElemAdd(pGui,nPage,(gslc_tsElem*)&sElem##nElemId,          \
    (gslc_teElemRefFlags)(GSLC_ELEMREF_SRC_CONST | GSLC_ELEMREF_REDRAW_FULL));


#define gslc_ElemXSliderCreate_P(pGui,nElemId,nPage,nX,nY,nW,nH, \
    nPosMin_,nPosMax_,nPos_,nThumbSz_,bVert_,colFrame_,colFill_) \
  static const uint8_t nFeatures##nElemId = GSLC_ELEM_FEA_VALID | \
    GSLC_ELEM_FEA_GLOW_EN | GSLC_ELEM_FEA_CLICK_EN | GSLC_ELEM_FEA_FILL_EN; \
  static gslc_tsXSlider sSlider##nElemId;                         \
  sSlider##nElemId.bVert = bVert_;                                \
  sSlider##nElemId.nThumbSz = nThumbSz_;                          \
  sSlider##nElemId.nPosMin = nPosMin_;                            \
  sSlider##nElemId.nPosMax = nPosMax_;                            \
  sSlider##nElemId.nTickDiv = 0;                                  \
  sSlider##nElemId.nTickLen = 0;                                  \
  sSlider##nElemId.colTick = GSLC_COL_WHITE;                      \
  sSlider##nElemId.bTrim = false;                                 \
  sSlider##nElemId.colTrim = GSLC_COL_BLACK;                      \
  sSlider##nElemId.nPos = nPos_;                                  \
  sSlider##nElemId.pfuncXPos = NULL;                              \
  static const gslc_tsElem sElem##nElemId = {                     \
      nElemId,                                                    \
      nFeatures##nElemId,                                         \
      GSLC_TYPEX_SLIDER,                                          \
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
      NULL,                                                       \
      (void*)(&sSlider##nElemId),                                 \
      NULL,                                                       \
      &gslc_ElemXSliderDraw,                                      \
      &gslc_ElemXSliderTouch,                                     \
      NULL,                                                       \
  };                                                              \
  gslc_ElemAdd(pGui,nPage,(gslc_tsElem*)&sElem##nElemId,          \
    (gslc_teElemRefFlags)(GSLC_ELEMREF_SRC_CONST | GSLC_ELEMREF_REDRAW_FULL));


#define gslc_ElemXGaugeCreate_P(pGui,nElemId,nPage,nX,nY,nW,nH,\
    nMin_,nMax_,nVal_,colFrame_,colFill_,colGauge_,bVert_) \
  static const uint8_t nFeatures##nElemId = GSLC_ELEM_FEA_VALID | \
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
      NULL,                                                       \
      (void*)(&sGauge##nElemId),                                  \
      NULL,                                                       \
      &gslc_ElemXGaugeDraw,                                       \
      NULL,                                                       \
      NULL,                                                       \
  };                                                              \
  gslc_ElemAdd(pGui,nPage,(gslc_tsElem*)&sElem##nElemId,          \
    (gslc_teElemRefFlags)(GSLC_ELEMREF_SRC_CONST | GSLC_ELEMREF_REDRAW_FULL));

#endif

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _GUISLICE_EX_H_

