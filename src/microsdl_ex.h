#ifndef _MICROSDL_EX_H_
#define _MICROSDL_EX_H_

// =======================================================================
// microSDL library (extensions)
// - Calvin Hass
// - http:/www.impulseadventure.com/elec/microsdl-sdl-gui.html
//
// - Version 0.3    (2016/10/31)
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

// Extended data for Gauge element
typedef struct {
  int             nGaugeMin;    // Minimum control value
  int             nGaugeMax;    // Maximum control value
  int             nGaugeVal;    // Current control value
  SDL_Color       colGauge;     // Color of gauge fill bar
  bool            bGaugeVert;   // Vertical if true, else Horizontal
} microSDL_tsXGauge;


//
// Create a Gauge Element
// - Draws a horizontal or vertical box with a filled region
//   corresponding to the proportion that nVal represents
//   between nMin and nMax.
//
// INPUT:
// - pGui:        Pointer to GUI
// - nElemId:     Element ID to assign (0..32767 or MSDL_ID_AUTO to autogen)
// - nPage:       Page ID to attach element to
// - pXData:      Ptr to extended element data structure
// - rElem:       Rectangle coordinates defining gauge size
// - nMin:        Minimum value of gauge for nVal comparison
// - nMax:        Maximum value of gauge for nVal comparison
// - colGauge:    Color to fill the gauge with
// - bVert:       Flag to indicate vertical vs horizontal action
//                (true = vertical, false = horizontal)
//
// RETURN:
// - The Element ID or MSDL_ID_NONE if failure
//
int microSDL_ElemXGaugeCreate(microSDL_tsGui* pGui,int nElemId,int nPage,
  microSDL_tsXGauge* pXData,SDL_Rect rElem,int nMin,int nMax,int nVal,SDL_Color colGauge,bool bVert);

//
// Update a Gauge element's current value
// - Note that min & max values are assigned in create()
//
// INPUT:
// - pGui:        Pointer to GUI
// - nElemId:     Element ID to update
// - nVal:        New value to show in gauge
//
// RETURN:
// - none
//
void microSDL_ElemXGaugeUpdate(microSDL_tsGui* pGui,int nElemId,int nVal);

//
// Draw a gauge element on the screen
// - Called from microSDL_ElemDraw()
//
// INPUT:
// - pvGui:       Void ptr to GUI (typecast to microSDL_tsGui*)
// - pvElem:      Void ptr to Element (typecast to microSDL_tsElem*)
//
// RETURN:
// - true if success, false otherwise
//
bool microSDL_ElemXGaugeDraw(void* pvGui,void* pvElem);

// ============================================================================


// ============================================================================
// Extended Element: Checkbox
// ============================================================================

// Extended data for Checkbox element
typedef struct {
  bool            bChecked;     // Vertical if true, else Horizontal
  bool            bGlowing;     // Indicates if touch is hovering over it
  SDL_Color       colCheck;     // Color of checked inner fill
} microSDL_tsXCheckbox;


//
// Create a Checkbox Element
//
// INPUT:
// - pGui:        Pointer to GUI
// - nElemId:     Element ID to assign (0..32767 or MSDL_ID_AUTO to autogen)
// - nPage:       Page ID to attach element to
// - pXData:      Ptr to extended element data structure
// - rElem:       Rectangle coordinates defining checkbox size
// - colCheck:    Color for inner fill when checked
// - bChecked:    Default state
//
// RETURN:
// - The Element ID or MSDL_ID_NONE if failure
//
int microSDL_ElemXCheckboxCreate(microSDL_tsGui* pGui,int nElemId,int nPage,
  microSDL_tsXCheckbox* pXData,SDL_Rect rElem,SDL_Color colCheck,bool bChecked);


//
// Get a Checkbox element's current state
//
// INPUT:
// - pGui:        Pointer to GUI
// - nElemId:     Element ID to update
//
// RETURN:
// - Current state
//
bool microSDL_ElemXCheckboxGetState(microSDL_tsGui* pGui,int nElemId);


//
// Set a Checkbox element's current state
//
// INPUT:
// - pGui:        Pointer to GUI
// - nElemId:     Element ID to update
// - bChecked:    New state
//
// RETURN:
// - none
//
void microSDL_ElemXCheckboxSetState(microSDL_tsGui* pGui,int nElemId,bool bChecked);


//
// Toggle a Checkbox element's current state
//
// INPUT:
// - pGui:        Pointer to GUI
// - nElemId:     Element ID to update
//
// RETURN:
// - none
//
void microSDL_ElemXCheckboxToggleState(microSDL_tsGui* pGui,int nElemId);


//
// Draw a checkbox element on the screen
// - Called from microSDL_ElemDraw()
//
// INPUT:
// - pvGui:       Void ptr to GUI (typecast to microSDL_tsGui*)
// - pvElem:      Void ptr to Element (typecast to microSDL_tsElem*)
//
// RETURN:
// - true if success, false otherwise
//
bool microSDL_ElemXCheckboxDraw(void* pvGui,void* pvElem);


// ============================================================================
// Extended Element: Checkbox1
// - NOTE: The type name "microSDL_tsXCheckbox1" will be removed in an
//   upcoming release and replace "microSDL_tsXCheckbox".
//   - microSDL_tsXCheckbox is the simpler implementation without touch
//     tracking. It requires more user-code handling in the main loop.
//   - microSDL_tsXCheckbox1 is the more complete implementation with
//     touch tracking. It does not require any user-code handling in
//     the main loop.
// - Both versions of the checkbox code will be left intact for a short
//   while for instructional purposes during beta-testing / construction
//   of new element types.
// - At a later stage, I expect to remove the simpler implementation
//   and replace it with the more complete implementation and then
//   only use the type "microSDL_tsXCheckbox"
//   
// ============================================================================

// Extended data for Checkbox1 element
typedef struct {
  bool            bChecked;     // Vertical if true, else Horizontal
  bool            bGlowing;     // Indicates if touch is hovering over it
  SDL_Color       colCheck;     // Color of checked inner fill
} microSDL_tsXCheckbox1;


//
// Create a Checkbox1 Element
//
// INPUT:
// - pGui:        Pointer to GUI
// - nElemId:     Element ID to assign (0..32767 or MSDL_ID_AUTO to autogen)
// - nPage:       Page ID to attach element to
// - pXData:      Ptr to extended element data structure
// - rElem:       Rectangle coordinates defining checkbox size
// - colCheck:    Color for inner fill when checked
// - bChecked:    Default state
//
// RETURN:
// - The Element ID or MSDL_ID_NONE if failure
//
int microSDL_ElemXCheckbox1Create(microSDL_tsGui* pGui,int nElemId,int nPage,
  microSDL_tsXCheckbox1* pXData,SDL_Rect rElem,SDL_Color colCheck,bool bChecked);


//
// Get a Checkbox1 element's current state
//
// INPUT:
// - pGui:        Pointer to GUI
// - nElemId:     Element ID to update
//
// RETURN:
// - Current state
//
bool microSDL_ElemXCheckbox1GetState(microSDL_tsGui* pGui,int nElemId);


//
// Set a Checkbox1 element's current state
//
// INPUT:
// - pGui:        Pointer to GUI
// - nElemId:     Element ID to update
// - bChecked:    New state
//
// RETURN:
// - none
//
void microSDL_ElemXCheckbox1SetState(microSDL_tsGui* pGui,int nElemId,bool bChecked);


//
// Toggle a Checkbox1 element's current state
//
// INPUT:
// - pGui:        Pointer to GUI
// - nElemId:     Element ID to update
//
// RETURN:
// - none
//
void microSDL_ElemXCheckbox1ToggleState(microSDL_tsGui* pGui,int nElemId);


//
// Draw a Checkbox1 element on the screen
// - Called from microSDL_ElemDraw()
//
// INPUT:
// - pvGui:       Void ptr to GUI (typecast to microSDL_tsGui*)
// - pvElem:      Void ptr to Element (typecast to microSDL_tsElem*)
//
// RETURN:
// - true if success, false otherwise
//
bool microSDL_ElemXCheckbox1Draw(void* pvGui,void* pvElem);

//
// Handle touch events to Checkbox1 element
// - Called from microSDL_NotifyElemTouch()
//
// INPUT:
// - pvGui:       Void ptr to GUI (typecast to microSDL_tsGui*)
// - eTouch:      Touch event type
// - nRelX:       Touch X coord relative to element
// - nRelY:       Touch Y coord relative to element
//
// RETURN:
// - true if success, false otherwise
//
bool microSDL_ElemXCheckbox1Touch(void* pvGui,microSDL_teTouch eTouch,int nRelX,int nRelY);


// ============================================================================



#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _MICROSDL_EX_H_

