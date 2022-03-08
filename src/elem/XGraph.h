#ifndef _GUISLICE_EX_XGRAPH_H_
#define _GUISLICE_EX_XGRAPH_H_

#include "GUIslice.h"


// =======================================================================
// GUIslice library extension: Graph control
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
/// \file XGraph.h

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


// ============================================================================
// Extended Element: Graph
// ============================================================================

// Define unique identifier for extended element type
// - Select any number above GSLC_TYPE_BASE_EXTEND
#define  GSLC_TYPEX_GRAPH GSLC_TYPE_BASE_EXTEND + 5


/// Gauge drawing style
typedef enum {
  GSLCX_GRAPH_STYLE_DOT,        ///< Dot
  GSLCX_GRAPH_STYLE_LINE,       ///< Line
  GSLCX_GRAPH_STYLE_FILL,       ///< Filled
} gslc_teXGraphStyle;

// Extended element data structures
// - These data structures are maintained in the gslc_tsElem
//   structure via the pXData pointer

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

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _GUISLICE_EX_XGRAPH_H_

