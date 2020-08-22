#ifndef _GUISLICE_EX_XTEXTBOX_H_
#define _GUISLICE_EX_XTEXTBOX_H_

#include "GUIslice.h"


// =======================================================================
// GUIslice library extension: Textbox control
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
/// \file XTextbox.h

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus



// ============================================================================
// Extended Element: Textbox
// ============================================================================

// Define unique identifier for extended element type
// - Select any number above GSLC_TYPE_BASE_EXTEND
#define  GSLC_TYPEX_TEXTBOX GSLC_TYPE_BASE_EXTEND + 4

/// Definitions for textbox special inline codes
#define GSLC_XTEXTBOX_CODE_COL_SET    187
#define GSLC_XTEXTBOX_CODE_COL_RESET  188

#define XTEXTBOX_REDRAW_NONE   -1
#define XTEXTBOX_REDRAW_ALL    -2

// Extended element data structures
// - These data structures are maintained in the gslc_tsElem
//   structure via the pXData pointer

/// Extended data for Textbox element
typedef struct {
  // Config
  char*                       pBuf;         ///< Ptr to the text buffer (circular buffer))
  int8_t                      nMarginX;     ///< Margin for text area within element rect (X)
  int8_t                      nMarginY;     ///< Margin for text area within element rect (Y)
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
  // Redraw
  int16_t                     nRedrawRow;   ///< Specific row to update in redraw (if not -1)

} gslc_tsXTextbox;



///
/// Create a Textbox Element
/// - The textbox is a scrolling window designed for displaying multi-line
///   text using a monospaced font. A character buffer is defined by nBufRows*nBufCols
///   to capture the added text. If the allocation buffer is larger than the
///   display size (defined by rElem), then a scrollbar will be shown.
/// - Support for changing color within a row can be enabled with GSLC_FEATURE_XTEXTBOX_EMBED 1
/// - Note that each color change command will consume 4 of the available "column" bytes.
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nElemId:     Element ID to assign (0..16383 or GSLC_ID_AUTO to autogen)
/// \param[in]  nPage:       Page ID to attach element to
/// \param[in]  pXData:      Ptr to extended element data structure
/// \param[in]  rElem:       Rectangle coordinates defining textbox size
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

// ------------------------------------------------------------------------
// Read-only element macros
// ------------------------------------------------------------------------

// Macro initializers for Read-Only Elements in Flash/PROGMEM
//

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _GUISLICE_EX_XTEXTBOX_H_

