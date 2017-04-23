#ifndef _GUISLICE_EX_H_
#define _GUISLICE_EX_H_

#include "GUIslice.h"


// =======================================================================
// GUIslice library (extensions)
// - Calvin Hass
// - http://www.impulseadventure.com/elec/guislice-gui.html
// =======================================================================
//
// The MIT License
//
// Copyright 2017 Calvin Hass
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
    GSLC_TYPEX_SELNUM,      ///< SelNum extended element
    GSLC_TYPEX_TEXTBOX,     ///< Textbox extended element
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
/// \return Pointer to Element or NULL if failure
///
gslc_tsElem* gslc_ElemXGaugeCreate(gslc_tsGui* pGui,int16_t nElemId,int16_t nPage,
  gslc_tsXGauge* pXData,gslc_tsRect rElem,int16_t nMin,int16_t nMax,int16_t nVal,gslc_tsColor colGauge,bool bVert);


///
/// Configure the style of a Gauge element
/// - This function is used to select between one of several gauge types
///   (eg. progress bar, radial dial, etc.)
///
/// \param[in]  pElem:       Pointer to Element
/// \param[in]  nType:       Gauge style enumeration
///
/// \return none
///
void gslc_ElemXGaugeSetStyle(gslc_tsElem* pElem,gslc_teXGaugeStyle nType);


///
/// Configure the appearance of the Gauge indicator
///
/// \param[in]  pElem:       Pointer to Element
/// \param[in]  colGauge:    Color of the indicator
/// \param[in]  nIndicLen:   Length of the indicator
/// \param[in]  nIndicTip:   Size of the indicator tip
/// \param[in]  bIndicFill:  Fill in the indicator if true
///
/// \return none
///
void gslc_ElemXGaugeSetIndicator(gslc_tsElem* pElem,gslc_tsColor colGauge,
        uint16_t nIndicLen,uint16_t nTipSize,bool bIndicFill);


///
/// Configure the appearance of the Gauge ticks
///
/// \param[in]  pElem:       Pointer to Element
/// \param[in]  colTick:     Color of the gauge ticks
/// \param[in]  nTickCnt:    Number of ticks to draw around / along gauge
/// \param[in]  nTickLen:    Length of the tick marks to draw
///
/// \return none
///
void gslc_ElemXGaugeSetTicks(gslc_tsElem* pElem,gslc_tsColor colTick,uint16_t nTickCnt,uint16_t nTickLen);


///
/// Update a Gauge element's current value
/// - Note that min & max values are assigned in create()
///
/// \param[in]  pElem:       Pointer to Element
/// \param[in]  nVal:        New value to show in gauge
///
/// \return none
///
void gslc_ElemXGaugeUpdate(gslc_tsElem* pElem,int16_t nVal);


///
/// Set a Gauge element's fill direction
/// - Setting bFlip reverses the default fill direction
/// - Default fill direction for horizontal gauges: left-to-right
/// - Default fill direction for vertical gauges: bottom-to-top
/// 
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElem:       Pointer to Element
/// \param[in]  bFlip:       If set, reverse direction of fill from default
///
/// \return none
///
void gslc_ElemXGaugeSetFlip(gslc_tsElem* pElem,bool bFlip);


///
/// Draw a gauge element on the screen
/// - Called from gslc_ElemDraw()
///
/// \param[in]  pvGui:       Void ptr to GUI (typecast to gslc_tsGui*)
/// \param[in]  pvElem:      Void ptr to Element (typecast to gslc_tsElem*)
///
/// \return true if success, false otherwise
///
bool gslc_ElemXGaugeDraw(void* pvGui,void* pvElem);


///
/// Helper function to draw a gauge with style: progress bar
/// - Called from gslc_ElemXGaugeDraw()
///
/// \param[in]  pGui:        Ptr to GUI
/// \param[in]  pElem:       Ptr to Element
///
/// \return true if success, false otherwise
///
bool gslc_ElemXGaugeDrawProgressBar(gslc_tsGui* pGui,gslc_tsElem* pElem);


///
/// Helper function to draw a gauge with style: radial
/// - Called from gslc_ElemXGaugeDraw()
///
/// \param[in]  pGui:        Ptr to GUI
/// \param[in]  pElem:       Ptr to Element
///
/// \return true if success, false otherwise
///
bool gslc_ElemXGaugeDrawRadial(gslc_tsGui* pGui,gslc_tsElem* pElem);

//xxx TODO DOC
bool gslc_ElemXGaugeDrawRamp(gslc_tsGui* pGui,gslc_tsElem* pElem);

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
  gslc_tsGui*                 pGui;         ///< Ptr to GUI (for radio group control)
  bool                        bRadio;       ///< Radio-button operation if true
  gslc_teXCheckboxStyle       nStyle;       ///< Drawing style for element
  bool                        bChecked;     ///< Indicates if it is selected (checked)
  gslc_tsColor                colCheck;     ///< Color of checked inner fill
} gslc_tsXCheckbox;


///
/// Create a Checkbox Element
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
/// \return Element pointer or NULL if failure
///
gslc_tsElem* gslc_ElemXCheckboxCreate(gslc_tsGui* pGui,int16_t nElemId,int16_t nPage,
  gslc_tsXCheckbox* pXData,gslc_tsRect rElem,bool bRadio,
  gslc_teXCheckboxStyle nStyle,gslc_tsColor colCheck,bool bChecked);


///
/// Get a Checkbox element's current state
///
/// \param[in]  pElem:       Pointer to Element
///
/// \return Current state
///
bool gslc_ElemXCheckboxGetState(gslc_tsElem* pElem);


///
/// Set a Checkbox element's current state
///
/// \param[in]  pElem:       Pointer to Element
/// \param[in]  bChecked:    New state
///
/// \return none
///
void gslc_ElemXCheckboxSetState(gslc_tsElem* pElem,bool bChecked);

///
/// Find the checkbox within a group that has been checked
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nGroupId:    Group ID to search
///
/// \return Element Ptr or NULL if none checked
///
gslc_tsElem* gslc_ElemXCheckboxFindChecked(gslc_tsGui* pGui,int16_t nGroupId);

///
/// Toggle a Checkbox element's current state
///
/// \param[in]  pElem:       Pointer to Element
///
/// \return none
///
void gslc_ElemXCheckboxToggleState(gslc_tsElem* pElem);


///
/// Draw a Checkbox element on the screen
/// - Called from gslc_ElemDraw()
///
/// \param[in]  pvGui:       Void ptr to GUI (typecast to gslc_tsGui*)
/// \param[in]  pvElem:      Void ptr to Element (typecast to gslc_tsElem*)
///
/// \return true if success, false otherwise
///
bool gslc_ElemXCheckboxDraw(void* pvGui,void* pvElem);

///
/// Handle touch events to Checkbox element
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
bool gslc_ElemXCheckboxTouch(void* pvGui,void* pvElem,gslc_teTouch eTouch,int16_t nRelX,int16_t nRelY);


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
/// \return Element pointer or NULL if failure
///
gslc_tsElem* gslc_ElemXSliderCreate(gslc_tsGui* pGui,int16_t nElemId,int16_t nPage,
  gslc_tsXSlider* pXData,gslc_tsRect rElem,int16_t nPosMin,int16_t nPosMax,int16_t nPos,
  uint16_t nThumbSz,bool bVert);


///
/// Set a Slider element's current position
///
/// \param[in]  pElem:       Pointer to Element
/// \param[in]  bTrim:       Show a colored trim?
/// \param[in]  colTrim:     Color of trim
/// \param[in]  nTickDiv:    Number of tick divisions to show (0 for none)
/// \param[in]  nTickLen:    Length of tickmarks
/// \param[in]  colTick:     Color of ticks
///
/// \return none
///
void gslc_ElemXSliderSetStyle(gslc_tsElem* pElem,
        bool bTrim,gslc_tsColor colTrim,uint16_t nTickDiv,
        int16_t nTickLen,gslc_tsColor colTick);


///
/// Get a Slider element's current position
///
/// \param[in]  pElem:       Pointer to Element
///
/// \return Current slider position
///
int gslc_ElemXSliderGetPos(gslc_tsElem* pElem);


///
/// Set a Slider element's current position
/// 
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElem:       Pointer to Element
/// \param[in]  nPos:        New position value
///
/// \return none
///
void gslc_ElemXSliderSetPos(gslc_tsGui* pGui,gslc_tsElem* pElem,int16_t nPos);

///
/// Assign the position callback function for a slider
///
/// \param[in]  pElem:       Pointer to element
/// \param[in]  funcCb:      Function pointer to position routine (or NULL for none)
///
/// \return none
///
void gslc_ElemXSliderSetPosFunc(gslc_tsElem* pElem,GSLC_CB_XSLIDER_POS funcCb);

///
/// Draw a Slider element on the screen
/// - Called from gslc_ElemDraw()
///
/// \param[in]  pvGui:       Void ptr to GUI (typecast to gslc_tsGui*)
/// \param[in]  pvElem:      Void ptr to Element (typecast to gslc_tsElem*)
///
/// \return true if success, false otherwise
///
bool gslc_ElemXSliderDraw(void* pvGui,void* pvElem);

///
/// Handle touch events to Slider element
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
bool gslc_ElemXSliderTouch(void* pvGui,void* pvElem,gslc_teTouch eTouch,int16_t nRelX,int16_t nRelY);


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
gslc_tsElem* gslc_ElemXSelNumCreate(gslc_tsGui* pGui,int16_t nElemId,int16_t nPage,
  gslc_tsXSelNum* pXData,gslc_tsRect rElem,int8_t nFontId);


///
/// Draw a SelNum element on the screen
/// - Called during redraw
///
/// \param[in]  pvGui:       Void ptr to GUI (typecast to gslc_tsGui*)
/// \param[in]  pvElem:      Void ptr to Element (typecast to gslc_tsElem*)
///
/// \return true if success, false otherwise
///
bool gslc_ElemXSelNumDraw(void* pvGui,void* pvElem);


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
/// \param[in]  pSelNum:     Ptr to Element
/// \param[in]  nCount:      New counter value
///
/// \return none
///
void gslc_ElemXSelNumSetCounter(gslc_tsXSelNum* pSelNum,int16_t nCount);


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
bool gslc_ElemXSelNumClick(void* pvGui,void *pvElem,gslc_teTouch eTouch,int16_t nX,int16_t nY);

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
bool gslc_ElemXSelNumTouch(void* pvGui,void* pvElem,gslc_teTouch eTouch,int16_t nRelX,int16_t nRelY);


// ============================================================================
// Extended Element: Textbox
// ============================================================================

/// Definitions for textbox special inline codes
#define GSLC_XTEXTBOX_CODE_COL_SET    187
#define GSLC_XTEXTBOX_CODE_COL_RESET  188


/// Extended data for Textbox element
typedef struct {
  // Config
  gslc_tsGui*                 pGui;         ///< Ptr to GUI (for radio group control)
  char*                       pBuf;         ///< Ptr to the text buffer (circular buffer))
  uint8_t                     nMargin;      ///< Margin for text area within element rect
  bool                        bWrapEn;      ///< Enable for line wrapping
  
  uint16_t                    nBufRows;     ///< Number of rows in buffer
  uint16_t                    nBufCols;     ///< Number of columns in buffer
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
/// \return Element pointer or NULL if failure
///
gslc_tsElem* gslc_ElemXTextboxCreate(gslc_tsGui* pGui,int16_t nElemId,int16_t nPage,
  gslc_tsXTextbox* pXData,gslc_tsRect rElem,int16_t nFontId,char* pBuf,
        uint16_t nBufRows,uint16_t nBufCols);


///
/// Draw a Textbox element on the screen
/// - Called from gslc_ElemDraw()
///
/// \param[in]  pvGui:       Void ptr to GUI (typecast to gslc_tsGui*)
/// \param[in]  pvElem:      Void ptr to Element (typecast to gslc_tsElem*)
///
/// \return true if success, false otherwise
///
bool gslc_ElemXTextboxDraw(void* pvGui,void* pvElem);

/// Add a text string to the textbox
/// - If it includes a newline then the buffer will
///   advance to the next row
///
/// \param[in]  pvGui:       Void ptr to GUI (typecast to gslc_tsGui*)
/// \param[in]  pvElem:      Void ptr to Element (typecast to gslc_tsElem*)
///
/// \return none
///
void gslc_ElemXTextboxAdd(gslc_tsElem* pElem,char* pTxt);

///
/// Insert a color set code into the current buffer position
///
/// \param[in]  pElem:       Pointer to element
/// \param[in]  nCol:        Color to assign for next text written to textbox
///
/// \return none
///
void gslc_ElemXTextboxColSet(gslc_tsElem* pElem,gslc_tsColor nCol);

///
/// Insert a color reset code into the current buffer position
///
/// \param[in]  pElem:       Pointer to element
///
/// \return none
///
void gslc_ElemXTextboxColReset(gslc_tsElem* pElem);

///
/// Enable or disable line wrap within textbox
///
/// \param[in]  pElem:       Pointer to element
/// \param[in]  bWrapEn:     Enable line wrap if true
///
/// \return none
///
void gslc_ElemXTextboxWrapSet(gslc_tsElem* pElem,bool bWrapEn);


///
/// Set the textbox scroll position (nScrollPos) as a fraction of
/// nScrollMax
///
/// \param[in]  pElem:       Pointer to element
/// \param[in]  nScrollPos:  New scroll position
/// \param[in]  nScrollMax:  Maximum scroll position
///
/// \return none
///
void gslc_ElemXTextboxScrollSet(gslc_tsElem* pElem,uint8_t nScrollPos,uint8_t nScrollMax);

// ============================================================================


#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _GUISLICE_EX_H_

