#ifndef _GUISLICE_EX_H_
#define _GUISLICE_EX_H_

// =======================================================================
// GUIslice library (extensions)
// - Calvin Hass
// - http://www.impulseadventure.com/elec/microsdl-sdl-gui.html
//
// - Version 0.6.1    (2016/11/18)
// =======================================================================
//
// The MIT License
//
// Copyright 2016 Calvin Hass
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


// Extended element data structures
// - These data structures are maintained in the gslc_tsElem
//   structure via the pXData pointer

  
  
// ============================================================================
// Extended Element: Gauge
// ============================================================================

/// Extended data for Gauge element
typedef struct {
  int             nGaugeMin;    ///< Minimum control value
  int             nGaugeMax;    ///< Maximum control value
  int             nGaugeVal;    ///< Current control value
  gslc_Color      colGauge;     ///< Color of gauge fill bar
  bool            bGaugeVert;   ///< Vertical if true, else Horizontal
} gslc_tsXGauge;


///
/// Create a Gauge Element
/// - Draws a horizontal or vertical box with a filled region
///   corresponding to the proportion that nVal represents
///   between nMin and nMax.
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nElemId:     Element ID to assign (0..32767 or GSLC_ID_AUTO to autogen)
/// \param[in]  nPage:       Page ID to attach element to
/// \param[in]  pXData:      Ptr to extended element data structure
/// \param[in]  rElem:       Rectangle coordinates defining gauge size
/// \param[in]  nMin:        Minimum value of gauge for nVal comparison
/// \param[in]  nMax:        Maximum value of gauge for nVal comparison
/// \param[in]  colGauge:    Color to fill the gauge with
/// \param[in]  bVert:       Flag to indicate vertical vs horizontal action
///                          (true = vertical, false = horizontal)
///
/// \return Pointer to Element or NULL if failure
///
gslc_tsElem* gslc_ElemXGaugeCreate(gslc_tsGui* pGui,int nElemId,int nPage,
  gslc_tsXGauge* pXData,gslc_Rect rElem,int nMin,int nMax,int nVal,gslc_Color colGauge,bool bVert);

///
/// Update a Gauge element's current value
/// - Note that min & max values are assigned in create()
///
/// \param[in]  pElem:       Pointer to Element
/// \param[in]  nVal:        New value to show in gauge
///
/// \return none
///
void gslc_ElemXGaugeUpdate(gslc_tsElem* pElem,int nVal);

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
  gslc_Color                  colCheck;     ///< Color of checked inner fill
} gslc_tsXCheckbox;


///
/// Create a Checkbox Element
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nElemId:     Element ID to assign (0..32767 or GSLC_ID_AUTO to autogen)
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
gslc_tsElem* gslc_ElemXCheckboxCreate(gslc_tsGui* pGui,int nElemId,int nPage,
  gslc_tsXCheckbox* pXData,gslc_Rect rElem,bool bRadio,
  gslc_teXCheckboxStyle nStyle,gslc_Color colCheck,bool bChecked);


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
gslc_tsElem* gslc_ElemXCheckboxFindChecked(gslc_tsGui* pGui,int nGroupId);

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
/// - Called from gslc_NotifyElemTouch()
///
/// \param[in]  pvGui:       Void ptr to GUI (typecast to gslc_tsGui*)
/// \param[in]  pvElem:      Void ptr to Element (typecast to gslc_tsElem*)
/// \param[in]  eTouch:      Touch event type
/// \param[in]  nRelX:       Touch X coord relative to element
/// \param[in]  nRelY:       Touch Y coord relative to element
///
/// \return true if success, false otherwise
///
bool gslc_ElemXCheckboxTouch(void* pvGui,void* pvElem,gslc_teTouch eTouch,int nRelX,int nRelY);


// ============================================================================
// Extended Element: Slider
// ============================================================================

/// Extended data for Slider element
typedef struct {
  // Config
  bool            bVert;          ///< Orientation: true if vertical, else horizontal
  int             nThumbSz;       ///< Size of the thumb control
  int             nPosMin;        ///< Minimum position value of the slider
  int             nPosMax;        ///< Maximum position value of the slider
  // Style config
  unsigned        nTickDiv;       ///< Style: number of tickmark divisions (0 for none)
  int             nTickLen;       ///< Style: length of tickmarks
  gslc_Color      colTick;        ///< Style: color of ticks
  bool            bTrim;          ///< Style: show a trim color
  gslc_Color      colTrim;        ///< Style: color of trim
  // State
  int             nPos;           ///< Current position value of the slider
} gslc_tsXSlider;


///
/// Create a Slider Element
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nElemId:     Element ID to assign (0..32767 or GSLC_ID_AUTO to autogen)
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
gslc_tsElem* gslc_ElemXSliderCreate(gslc_tsGui* pGui,int nElemId,int nPage,
  gslc_tsXSlider* pXData,gslc_Rect rElem,int nPosMin,int nPosMax,int nPos,
  unsigned nThumbSz,bool bVert);


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
        bool bTrim,gslc_Color colTrim,unsigned nTickDiv,
        int nTickLen,gslc_Color colTick);


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
/// \param[in]  pElem:       Pointer to Element
/// \param[in]  nPos:        New position value
///
/// \return none
///
void gslc_ElemXSliderSetPos(gslc_tsElem* pElem,int nPos);


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
/// - Called from gslc_NotifyElemTouch()
///
/// \param[in]  pvGui:       Void ptr to GUI (typecast to gslc_tsGui*)
/// \param[in]  pvElem:      Void ptr to Element (typecast to gslc_tsElem*)
/// \param[in]  eTouch:      Touch event type
/// \param[in]  nRelX:       Touch X coord relative to element
/// \param[in]  nRelY:       Touch Y coord relative to element
///
/// \return true if success, false otherwise
///
bool gslc_ElemXSliderTouch(void* pvGui,void* pvElem,gslc_teTouch eTouch,int nRelX,int nRelY);


// ============================================================================
// Extended Element: SelNum (Number Selector)
// - Demonstration of a compound element consisting of
//   a counter text field along with an increment and
//   decrement button.
// ============================================================================


/// Extended data for SelNum element
typedef struct {

  // Core functionality for SelNum
  int                 nCounter;       ///< Counter for demo purposes
   
  // Internal sub-element members
  gslc_tsCollect      sCollect;       ///< Collection management for sub-elements
  gslc_tsElem         asElem[4];      ///< Storage for sub-elements
} gslc_tsXSelNum;




///
/// Create a SelNum Element
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nElemId:     Element ID to assign (0..32767 or GSLC_ID_AUTO to autogen)
/// \param[in]  nPage:       Page ID to attach element to
/// \param[in]  pXData:      Ptr to extended element data structure
/// \param[in]  rElem:       Rectangle coordinates defining element size
/// \param[in]  nFontId:     Font ID to use for drawing the element
///
/// \return Pointer to Element or NULL if failure
///
gslc_tsElem* gslc_ElemXSelNumCreate(gslc_tsGui* pGui,int nElemId,int nPage,
  gslc_tsXSelNum* pXData,gslc_Rect rElem,int nFontId);


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
/// \param[in]  pSelElem:    Ptr to Element
///
/// \return Current counter value
///
int gslc_ElemXSelNumGetCounter(gslc_tsGui* pGui,gslc_tsXSelNum* pSelNum);


///
/// Set the current counter associated with SelNum
///
/// \param[in]  pSelElem:    Ptr to Element
/// \param[in]  nCount:      New counter value
///
/// \return none
///
void gslc_ElemXSelNumSetCounter(gslc_tsXSelNum* pSelNum,int nCount);


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
bool gslc_ElemXSelNumClick(void* pvGui,void *pvElem,gslc_teTouch eTouch,int nX,int nY);

///
/// Handle touch (up,down,move) events to SelNum element
/// - Called from gslc_NotifyElemTouch()
///
/// \param[in]  pvGui:       Void ptr to GUI (typecast to gslc_tsGui*)
/// \param[in]  pvElem:      Void ptr to Element (typecast to gslc_tsElem*)
/// \param[in]  eTouch:      Touch event type
/// \param[in]  nRelX:       Touch X coord relative to element
/// \param[in]  nRelY:       Touch Y coord relative to element
///
/// \return true if success, false otherwise
///
bool gslc_ElemXSelNumTouch(void* pvGui,void* pvElem,gslc_teTouch eTouch,int nRelX,int nRelY);


// ============================================================================



#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _GUISLICE_EX_H_

