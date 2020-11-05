#ifndef _GUISLICE_EX_XSPINNER_H_
#define _GUISLICE_EX_XSPINNER_H_

#include "GUIslice.h"


// =======================================================================
// GUIslice library extension: Spinner (number selection) control
// - Extension by Paul Conti, Calvin Hass
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
/// \file XSpinner.h

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus



#if (GSLC_FEATURE_COMPOUND)
// ============================================================================
// Extended Element: Spinner (Number Selector)
// - Demonstration of a compound element consisting of
//   a counter text field along with an increment and
//   decrement button.
// ============================================================================

// Define unique identifier for extended element type
// - Select any number above GSLC_TYPE_BASE_EXTEND
#define  GSLC_TYPEX_SPINNER GSLC_TYPE_BASE_EXTEND + 15

// Define the max number of sub-elements in compound element
#define XSPINNER_COMP_CNT 3

// Define the max string length to allocate for dynamic text elements
#define XSPINNER_STR_LEN  8

// Define the status for GSLC_CB_INPUT callback
#define XSPINNER_CB_STATE_UPDATE 3

// Extended element data structures
// - These data structures are maintained in the gslc_tsElem
//   structure via the pXData pointer
/// Extended data for Spinner element
typedef struct {

  // Core functionality for Spinner
  int16_t             nMin;           ///< Minimum control value
  int16_t             nMax;           ///< Maximum control value
  int16_t             nIncr;          ///< Increment by value
  int16_t             nCounter;       ///< Current value
  GSLC_CB_INPUT       pfuncXInput;    ///< Callback func ptr for input ready
  gslc_tsElemRef*     pElemRef;       ///< Save our ElemRef for the callback

  // Internal sub-element members
  gslc_tsCollect      sCollect;                      ///< Collection management for sub-elements
  gslc_tsElemRef      asElemRef[XSPINNER_COMP_CNT];   ///< Storage for sub-element references
  gslc_tsElem         asElem[XSPINNER_COMP_CNT];      ///< Storage for sub-elements

  // Provide storage for any dynamic text elements
  // Simple example here uses fixed-length character array
  char                acElemTxt[1][XSPINNER_STR_LEN]; ///< Storage for strings
  char                acIncr[2];       ///< Increment character string
  char                acDecr[2];       ///< Decrement character string

} gslc_tsXSpinner;




///
/// Create a Spinner Element
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nElemId:     Element ID to assign (0..16383 or GSLC_ID_AUTO to autogen)
/// \param[in]  nPage:       Page ID to attach element to
/// \param[in]  pXData:      Ptr to extended element data structure
/// \param[in]  rElem:       Rectangle coordinates defining overall size
/// \param[in]  nMin:        Minimum value of Spinner
/// \param[in]  nMax:        Maximum value of Spinner
/// \param[in]  nVal:        Starting value of Spinner
/// \param[in]  nIncr:       Increment Spinner by this value
/// \param[in]  nFontId:     Font ID to use for drawing the element
/// \param[in]  nButtonSz:   Size of individual buttons
/// \param[in]  cbInput:     Callback for touch events
///
/// \return Pointer to Element or NULL if failure
///
gslc_tsElemRef* gslc_ElemXSpinnerCreate(gslc_tsGui* pGui, int16_t nElemId, int16_t nPage, gslc_tsXSpinner* pXData,
  gslc_tsRect rElem, int16_t nMin, int16_t nMax, int16_t nVal, int16_t nIncr,
  int8_t nFontId, int8_t nButtonSz, GSLC_CB_INPUT cbInput);


///
/// Set Up and Down characters for the Spinner element
/// - Called during redraw
///
/// \param[in]  pvGui:       Void ptr to GUI (typecast to gslc_tsGui*)
/// \param[in]  pElemRef:    Ptr to ElementRef
/// \param[in]  cIncr:       Character to use to indicate incrementing the spinner
/// \param[in]  cDecr:       Character to use to indicate decrementing the spinner
///
/// \return true if success, false otherwise
///
bool gslc_ElemXSpinnerSetChars(void* pvGui,gslc_tsElemRef* pElemRef,uint8_t cIncr, uint8_t cDecr);


///
/// Draw a Spinner element on the screen
/// - Called during redraw
///
/// \param[in]  pvGui:       Void ptr to GUI (typecast to gslc_tsGui*)
/// \param[in]  pvElemRef:   Void ptr to Element reference (typecast to gslc_tsElemRef*)
/// \param[in]  eRedraw:     Redraw mode
///
/// \return true if success, false otherwise
///
bool gslc_ElemXSpinnerDraw(void* pvGui,void* pvElemRef,gslc_teRedrawType eRedraw);

///
/// Get the current counter associated with Spinner
///
/// \param[in]  pGui:        Ptr to GUI
/// \param[in]  pSpinner:     Ptr to Element
///
/// \return Current counter value
///
int gslc_ElemXSpinnerGetCounter(gslc_tsGui* pGui,gslc_tsXSpinner* pSpinner);


///
/// Set the current counter associated with Spinner
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pSpinner:     Ptr to Element
/// \param[in]  nCount:      New counter value
///
/// \return none
///
void gslc_ElemXSpinnerSetCounter(gslc_tsGui* pGui,gslc_tsXSpinner* pSpinner,int16_t nCount);


///
/// Handle a click event within the Spinner
/// - This is called internally by the Spinner touch handler
///
/// \param[in]  pvGui:       Void ptr to GUI (typecast to gslc_tsGui*)
/// \param[in]  pvElemRef    Void ptr to Element reference (typecast to gslc_tsElemRef*)
/// \param[in]  eTouch:      Touch event type
/// \param[in]  nX:          Touch X coord
/// \param[in]  nY:          Touch Y coord
///
/// \return none
///
bool gslc_ElemXSpinnerClick(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY);

///
/// Handle touch (up,down,move) events to Spinner element
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
bool gslc_ElemXSpinnerTouch(void* pvGui,void* pvElemRef,gslc_teTouch eTouch,int16_t nRelX,int16_t nRelY);

#endif // GLSC_COMPOUND

// ============================================================================

// ------------------------------------------------------------------------
// Read-only element macros
// ------------------------------------------------------------------------

// Macro initializers for Read-Only Elements in Flash/PROGMEM
//
#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _GUISLICE_EX_XSPINNER_H_

