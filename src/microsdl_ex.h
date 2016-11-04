#ifndef _MICROSDL_EX_H_
#define _MICROSDL_EX_H_

// =======================================================================
// microSDL library (extensions)
// - Calvin Hass
// - http:/www.impulseadventure.com/elec/microsdl-sdl-gui.html
//
// - Version 0.3.2    (2016/11/03)
// =======================================================================

// Extended element definitions
// - This file extends the core microSDL functionality with
//   additional widget types

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


// Extended element data structures
// - These data structures are maintained in the microSDL_tsElem
//   structure via the pXData pointer

  
  
// ============================================================================
// Extended Element: Gauge
// ============================================================================

/// Extended data for Gauge element
typedef struct {
  int             nGaugeMin;    ///< Minimum control value
  int             nGaugeMax;    ///< Maximum control value
  int             nGaugeVal;    ///< Current control value
  SDL_Color       colGauge;     ///< Color of gauge fill bar
  bool            bGaugeVert;   ///< Vertical if true, else Horizontal
} microSDL_tsXGauge;


///
/// Create a Gauge Element
/// - Draws a horizontal or vertical box with a filled region
///   corresponding to the proportion that nVal represents
///   between nMin and nMax.
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nElemId:     Element ID to assign (0..32767 or MSDL_ID_AUTO to autogen)
/// \param[in]  nPage:       Page ID to attach element to
/// \param[in]  pXData:      Ptr to extended element data structure
/// \param[in]  rElem:       Rectangle coordinates defining gauge size
/// \param[in]  nMin:        Minimum value of gauge for nVal comparison
/// \param[in]  nMax:        Maximum value of gauge for nVal comparison
/// \param[in]  colGauge:    Color to fill the gauge with
/// \param[in]  bVert:       Flag to indicate vertical vs horizontal action
///                (true = vertical, false = horizontal)
///
/// \return The Element ID or MSDL_ID_NONE if failure
///
int microSDL_ElemXGaugeCreate(microSDL_tsGui* pGui,int nElemId,int nPage,
  microSDL_tsXGauge* pXData,SDL_Rect rElem,int nMin,int nMax,int nVal,SDL_Color colGauge,bool bVert);

///
/// Update a Gauge element's current value
/// - Note that min & max values are assigned in create()
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nElemId:     Element ID to update
/// \param[in]  nVal:        New value to show in gauge
///
/// \return none
///
void microSDL_ElemXGaugeUpdate(microSDL_tsGui* pGui,int nElemId,int nVal);

///
/// Draw a gauge element on the screen
/// - Called from microSDL_ElemDraw()
///
/// \param[in]  pvGui:       Void ptr to GUI (typecast to microSDL_tsGui*)
/// \param[in]  pvElem:      Void ptr to Element (typecast to microSDL_tsElem*)
///
/// \return true if success, false otherwise
///
bool microSDL_ElemXGaugeDraw(void* pvGui,void* pvElem);



// ============================================================================
// Extended Element: Checkbox
// ============================================================================

/// Extended data for Checkbox element
typedef struct {
  bool            bChecked;     ///< Indicates if it is selected (checked)
  SDL_Color       colCheck;     ///< Color of checked inner fill
} microSDL_tsXCheckbox;


///
/// Create a Checkbox Element
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nElemId:     Element ID to assign (0..32767 or MSDL_ID_AUTO to autogen)
/// \param[in]  nPage:       Page ID to attach element to
/// \param[in]  pXData:      Ptr to extended element data structure
/// \param[in]  rElem:       Rectangle coordinates defining checkbox size
/// \param[in]  colCheck:    Color for inner fill when checked
/// \param[in]  bChecked:    Default state
///
/// \return The Element ID or MSDL_ID_NONE if failure
///
int microSDL_ElemXCheckboxCreate(microSDL_tsGui* pGui,int nElemId,int nPage,
  microSDL_tsXCheckbox* pXData,SDL_Rect rElem,SDL_Color colCheck,bool bChecked);


///
/// Get a Checkbox element's current state
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nElemId:     Element ID to update
///
/// \return Current state
///
bool microSDL_ElemXCheckboxGetState(microSDL_tsGui* pGui,int nElemId);


///
/// Set a Checkbox element's current state
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nElemId:     Element ID to update
/// \param[in]  bChecked:    New state
///
/// \return none
///
void microSDL_ElemXCheckboxSetState(microSDL_tsGui* pGui,int nElemId,bool bChecked);


///
/// Toggle a Checkbox element's current state
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nElemId:     Element ID to update
///
/// \return none
///
void microSDL_ElemXCheckboxToggleState(microSDL_tsGui* pGui,int nElemId);


///
/// Draw a Checkbox element on the screen
/// - Called from microSDL_ElemDraw()
///
/// \param[in]  pvGui:       Void ptr to GUI (typecast to microSDL_tsGui*)
/// \param[in]  pvElem:      Void ptr to Element (typecast to microSDL_tsElem*)
///
/// \return true if success, false otherwise
///
bool microSDL_ElemXCheckboxDraw(void* pvGui,void* pvElem);

///
/// Handle touch events to Checkbox element
/// - Called from microSDL_NotifyElemTouch()
///
/// /param[in]  pvGui:       Void ptr to GUI (typecast to microSDL_tsGui*)
/// /param[in]  eTouch:      Touch event type
/// /param[in]  nRelX:       Touch X coord relative to element
/// /param[in]  nRelY:       Touch Y coord relative to element
///
/// \return true if success, false otherwise
///
bool microSDL_ElemXCheckboxTouch(void* pvGui,microSDL_teTouch eTouch,int nRelX,int nRelY);


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
  SDL_Color       colTick;        ///< Style: color of ticks
  bool            bTrim;          ///< Style: show a trim color
  SDL_Color       colTrim;        ///< Style: color of trim
  // State
  int             nPos;           ///< Current position value of the slider
} microSDL_tsXSlider;


///
/// Create a Slider Element
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nElemId:     Element ID to assign (0..32767 or MSDL_ID_AUTO to autogen)
/// \param[in]  nPage:       Page ID to attach element to
/// \param[in]  pXData:      Ptr to extended element data structure
/// \param[in]  rElem:       Rectangle coordinates defining checkbox size
/// \param[in]  nPosMin:     Minimum position value
/// \param[in]  nPosMax:     Maximum position value
/// \param[in]  nPos:        Starting position value
/// \param[in]  nThumbSz:    Size of the thumb control
/// \param[in]  bVert:       Orientation (true for vertical)
///
/// \return The Element ID or MSDL_ID_NONE if failure
///
int microSDL_ElemXSliderCreate(microSDL_tsGui* pGui,int nElemId,int nPage,
  microSDL_tsXSlider* pXData,SDL_Rect rElem,int nPosMin,int nPosMax,int nPos,
  unsigned nThumbSz,bool bVert);


///
/// Set a Slider element's current position
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nElemId:     Element ID to update
/// \param[in]  bTrim:       Show a colored trim?
/// \param[in]  colTrim:     Color of trim
/// \param[in]  nTickDiv:    Number of tick divisions to show (0 for none)
/// \param[in]  nTickLen:    Length of tickmarks
/// \param[in]  colTick:     Color of ticks
///
/// \return none
///
void microSDL_ElemXSliderSetStyle(microSDL_tsGui* pGui,int nElemId,
        bool bTrim,SDL_Color colTrim,unsigned nTickDiv,
        int nTickLen,SDL_Color colTick);


///
/// Get a Slider element's current position
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nElemId:     Element ID to update
///
/// \return Current slider position
///
int microSDL_ElemXSliderGetPos(microSDL_tsGui* pGui,int nElemId);


///
/// Set a Slider element's current position
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nElemId:     Element ID to update
/// \param[in]  nPos:        New position value
///
/// \return none
///
void microSDL_ElemXSliderSetPos(microSDL_tsGui* pGui,int nElemId,int nPos);


///
/// Draw a Slider element on the screen
/// - Called from microSDL_ElemDraw()
///
/// \param[in]  pvGui:       Void ptr to GUI (typecast to microSDL_tsGui*)
/// \param[in]  pvElem:      Void ptr to Element (typecast to microSDL_tsElem*)
///
/// \return true if success, false otherwise
///
bool microSDL_ElemXSliderDraw(void* pvGui,void* pvElem);

///
/// Handle touch events to Slider element
/// - Called from microSDL_NotifyElemTouch()
///
/// \param[in]  pvGui:       Void ptr to GUI (typecast to microSDL_tsGui*)
/// \param[in]  eTouch:      Touch event type
/// \param[in]  nRelX:       Touch X coord relative to element
/// \param[in]  nRelY:       Touch Y coord relative to element
///
/// \return true if success, false otherwise
///
bool microSDL_ElemXSliderTouch(void* pvGui,microSDL_teTouch eTouch,int nRelX,int nRelY);



// ============================================================================



#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _MICROSDL_EX_H_

