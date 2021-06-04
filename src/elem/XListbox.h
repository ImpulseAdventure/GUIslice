#ifndef _GUISLICE_EX_XLISTBOX_H_
#define _GUISLICE_EX_XLISTBOX_H_

#include "GUIslice.h"


// =======================================================================
// GUIslice library extension: Listbox control
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
/// \file XListbox.h

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


// ============================================================================
// Extended Element: Listbox
// - NOTE: The XListbox element is in beta development.
//         Therefore, its API is subject to change.
// ============================================================================

// Define unique identifier for extended element type
// - Select any number above GSLC_TYPE_BASE_EXTEND
#define  GSLC_TYPEX_LISTBOX GSLC_TYPE_BASE_EXTEND + 10

// Define constants specific to the control
#define XLISTBOX_SEL_NONE       -9  // Indicator for "no selection"
#define XLISTBOX_SIZE_AUTO      -1  // Indicator for "auto-size"
#define XLISTBOX_BUF_OH_R        2  // Listbox buffer overhead per row

/// Callback function for Listbox feedback
typedef bool (*GSLC_CB_XLISTBOX_SEL)(void* pvGui,void* pvElem,int16_t nSel);

// Extended element data structures
// - These data structures are maintained in the gslc_tsElem
//   structure via the pXData pointer

/// Extended data for Listbox element
typedef struct {

  // Config
  uint8_t*        pBufItems;      ///< Buffer containing items
  uint16_t        nBufItemsMax;   ///< Max size of buffer containing items
  uint16_t        nBufItemsPos;   ///< Current buffer position
  int16_t         nItemCnt;       ///< Number of items in the list

  // Style config
  int8_t          nCols;          ///< Number of columns
  int8_t          nRows;          ///< Number of columns (or XLSITBOX_SIZE_AUTO to calculate)
  bool            bNeedRecalc;    ///< Determine if sizing may need recalc
  int8_t          nMarginW;       ///< Margin inside main listbox area (X offset)
  int8_t          nMarginH;       ///< Margin inside main listbox area (Y offset)
  int16_t         nItemW;         ///< Width of listbox item
  int16_t         nItemH;         ///< Height of listbox item
  int8_t          nItemGap;       ///< Gap between listbox items
  gslc_tsColor    colGap;         ///< Gap color
  bool            bItemAutoSizeW; ///< Enable auto-sizing of items (in width)
  bool            bItemAutoSizeH; ///< Enable auto-sizing of items (in height)

  // State
  int16_t         nItemCurSel;      ///< Currently selected item (XLISTBOX_SEL_NONE for none)
  int16_t         nItemCurSelLast;  ///< Old selected item to redraw (XLISTBOX_SEL_NONE for none)
  int16_t         nItemSavedSel;    ///< Persistent selected item (ie. saved selection)
  int16_t         nItemTop;         ///< Item to show at top of list after scrolling (0 is default)
  bool            bGlowLast;        ///< Last glow state
  bool            bFocusLast;       ///< Last focus state // TODO: Merge with bGlowLast

  // Callbacks
  GSLC_CB_XLISTBOX_SEL pfuncXSel; ///< Callback func ptr for selection update

} gslc_tsXListbox;


///
/// Create a Listbox Element
///
/// \param[in]  pGui:          Pointer to GUI
/// \param[in]  nElemId:       Element ID to assign (0..16383 or GSLC_ID_AUTO to autogen)
/// \param[in]  nPage:         Page ID to attach element to
/// \param[in]  pXData:        Ptr to extended element data structure
/// \param[in]  rElem:         Rectangle coordinates defining checkbox size
/// \param[in]  nFontId:       Font ID for item display
/// \param[in]  pBufItems:     Pointer to buffer that will contain list of items
/// \param[in]  nBufItemsMax:  Max size of buffer for list of items (pBufItems)
/// \param[in]  nSelDefault:   Default item to select
///
/// \return Pointer to Element reference or NULL if failure
///
gslc_tsElemRef* gslc_ElemXListboxCreate(gslc_tsGui* pGui, int16_t nElemId, int16_t nPage,
  gslc_tsXListbox* pXData, gslc_tsRect rElem, int16_t nFontId, uint8_t* pBufItems,
  uint16_t nBufItemsMax, int16_t nSelDefault);


///
/// Configure the number of rows & columns to display in the listbox
///
/// \param[in]  pGui:          Pointer to GUI
/// \param[in]  pElemRef:      Ptr to Element Reference to update
/// \param[in]  nRows:         Number of rows (>= 1, or XLISTBOX_SIZE_AUTO to base on content)
/// \param[in]  nCols:         Number of columns (>= 1)
///
/// \return none
///
void gslc_ElemXListboxSetSize(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, int8_t nRows, int8_t nCols);


///
/// Configure the margin inside the listbox
/// - Defines the region bewteen the element rect and the inner listbox items
///
/// \param[in]  pGui:          Pointer to GUI
/// \param[in]  pElemRef:      Ptr to Element Reference to update
/// \param[in]  nMarginW:      Set the margin (horizontal) inside the listbox (0 for none)
/// \param[in]  nMarginH:      Set the margin (horizontal) inside the listbox (0 for none)
///
/// \return none
///
void gslc_ElemXListboxSetMargin(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, int8_t nMarginW, int8_t nMarginH);

///
/// Configure the size of the listbox items
///
/// \param[in]  pGui:          Pointer to GUI
/// \param[in]  pElemRef:      Ptr to Element Reference to update
/// \param[in]  nItemW:        Set the width of a listbox item (or -1 to auto-size)
/// \param[in]  nItemH:        Set the height of a listbox item
///
/// \return none
///
void gslc_ElemXListboxItemsSetSize(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, int16_t nItemW, int16_t nItemH);

///
/// Configure the gap between listbox items
///
/// \param[in]  pGui:          Pointer to GUI
/// \param[in]  pElemRef:      Ptr to Element Reference to update
/// \param[in]  nGap:          Set the gap between listbox items (0 for none)
/// \param[in]  colGap:        Set the color of the gap between listbox items
///
/// \return none
///
void gslc_ElemXListboxItemsSetGap(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, int8_t nGap, gslc_tsColor colGap);


///
/// Empty the listbox of all items
///
/// \param[in]  pGui:          Pointer to GUI
/// \param[in]  pElemRef:      Ptr to Element Reference to update
///
/// \return none
///
void gslc_ElemXListboxReset(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef);


///
/// Add an item to the listbox
///
/// \param[in]  pGui:          Pointer to GUI
/// \param[in]  pElemRef:      Ptr to Element Reference to update
/// \param[in]  pStrItem:      String to use when creating the listbox item
///
/// \return true if OK, false if fail (eg. insufficient buffer storage)
///
bool gslc_ElemXListboxAddItem(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, const char* pStrItem);

///
/// Insert an item in the listbox at a specific position
///
/// \param[in]  pGui:          Pointer to GUI
/// \param[in]  pElemRef:      Ptr to Element Reference to update
/// \param[in]  nInsertPos:    Insertion position
/// \param[in]  pStrItem:      String to use when creating the listbox item
///
/// \return true if OK, false if fail (eg. insufficient buffer storage)
///
bool gslc_ElemXListboxInsertItemAt(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, uint16_t nInsertPos, 
  const char* pStrItem);

///
/// Insert an item in the listbox at a specific position
///
/// \param[in]  pGui:          Pointer to GUI
/// \param[in]  pElemRef:      Ptr to Element Reference to update
/// \param[in]  nDeletePos:    Position to delete
///
/// \return true if OK, false if fail
///
bool gslc_ElemXListboxDeleteItemAt(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, uint16_t nDeletePos);

///
/// Get the indexed listbox item
///
/// \param[in]  pGui:          Pointer to GUI
/// \param[in]  pElemRef:      Ptr to Element Reference to update
/// \param[in]  nItemCurSel:   Item index to fetch
/// \param[out] pStrItem:      Ptr to the string buffer to receive the item
/// \param[in]  nStrItemLen:   Maximum buffer length of pStrItem
///
/// \return true if success, false if fail (eg. can't locate item)
///
bool gslc_ElemXListboxGetItem(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, int16_t nItemCurSel, 
  char* pStrItem, uint8_t nStrItemLen);


///
/// Get the number of items in the listbox
///
/// \param[in]  pGui:          Pointer to GUI
/// \param[in]  pElemRef:      Ptr to Element Reference to update
///
/// \return Number of items
///
int16_t gslc_ElemXListboxGetItemCnt(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef);

///
/// Draw a Listbox element on the screen
/// - Called from gslc_ElemDraw()
///
/// \param[in]  pvGui:       Void ptr to GUI (typecast to gslc_tsGui*)
/// \param[in]  pvElemRef:   Void ptr to Element reference (typecast to gslc_tsElemRef*)
/// \param[in]  eRedraw:     Redraw mode
///
/// \return true if success, false otherwise
///
bool gslc_ElemXListboxDraw(void* pvGui,void* pvElemRef,gslc_teRedrawType eRedraw);

///
/// Handle touch events to Listbox element
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
bool gslc_ElemXListboxTouch(void* pvGui,void* pvElemRef,gslc_teTouch eTouch,int16_t nRelX,int16_t nRelY);


///
/// Get a Listbox element's current selection
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
///
/// \return Current Listbox selection (or -1 if none)
///
int16_t gslc_ElemXListboxGetSel(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef);


///
/// Set a Listbox element's current selection
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
/// \param[in]  nItemCurSel:    Listbox item to select (or -1 for none)
///
/// \return true if success, false if fail
///
bool gslc_ElemXListboxSetSel(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, int16_t nItemCurSel);

///
/// Set the Listbox scroll position
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
/// \param[in]  nScrollPos:  Scroll the listbox so that the nScrollPos item is at the top (0 default)
///
/// \return true if success, false if fail
///
bool gslc_ElemXListboxSetScrollPos(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, uint16_t nScrollPos);


///
/// Assign the selection callback function for a Listbox
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
/// \param[in]  funcCb:      Function pointer to selection routine (or NULL for none)
///
/// \return none
///
void gslc_ElemXListboxSetSelFunc(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,GSLC_CB_XLISTBOX_SEL funcCb);

// ============================================================================

// ------------------------------------------------------------------------
// Read-only element macros
// ------------------------------------------------------------------------

// Macro initializers for Read-Only Elements in Flash/PROGMEM
//



#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _GUISLICE_EX_XLISTBOX_H_

