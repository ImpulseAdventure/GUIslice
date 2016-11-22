#ifndef _GUISLICE_H_
#define _GUISLICE_H_

// =======================================================================
// GUIslice library
// - Calvin Hass
// - http://www.impulseadventure.com/elec/microsdl-sdl-gui.html
//
// - Version 0.7    (2016/11/21)
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


#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
  
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <inttypes.h>

  
// -----------------------------------------------------------------------
// Configuration
// -----------------------------------------------------------------------

#define GSLC_ELEM_STRLEN_MAX  120  // Max string length of text elements

// Enable for bitmap transparency and definition of color to use
#define GSLC_BMP_TRANS_EN     1               // 1 = enabled, 0 = disabled
#define GSLC_BMP_TRANS_RGB    0xFF,0x00,0xFF  // RGB color (default:pink)

// -----------------------------------------------------------------------
// Enumerations
// -----------------------------------------------------------------------

  /// Element ID enumerations
  /// - The Element ID is the primary means for user code to
  ///   reference a graphic element.
  /// - Application code can assign arbitrary Element ID values
  ///   in the range of 0...32767
  /// - Specifying GSLC_ID_AUTO to ElemCreate() requests that
  ///   GUIslice auto-assign an ID value for the Element. These
  ///   auto-assigned values will begin at GSLC_ID_AUTO_BASE.
  /// - Negative Element ID values are reserved 
  typedef enum {
    // Public usage
    GSLC_ID_USER_BASE       = 0,      ///< Starting Element ID for user assignments
    GSLC_ID_NONE            = -1999,  ///< No Element ID has been assigned
    GSLC_ID_AUTO,                     ///< Auto-assigned Element ID requested
    GSLC_ID_TEMP,                     ///< ID for Temporary Element
    // Internal usage
    GSLC_ID_AUTO_BASE       = 32768,  ///< Starting Element ID to start auto-assignment
                                      ///< (when GSLC_ID_AUTO is specified)            
  } gslc_teElemId;


  /// Page ID enumerations
  /// - The Page ID is the primary means for user code to
  ///   reference a specific page of elements.
  /// - Application code can assign arbitrary Page ID values
  ///   in the range of 0...32767
  /// - Negative Page ID values are reserved   
  typedef enum {
    // Public usage
    GSLC_PAGE_USER_BASE     = 0,      ///< Starting Page ID for user assignments
    // Internal usage
    GSLC_PAGE_NONE          = -2999,  ///< No Page ID has been assigned
  } gslc_tePageId;

  /// View ID enumerations
  /// - The View ID is the primary means for user code to
  ///   reference a specific viewport.
  /// - Application code can assign arbitrary View ID values
  ///   in the range of 0...32767
  /// - Negative View ID values are reserved   
  typedef enum {
    // Public usage
    GSLC_VIEW_ID_USER_BASE  = 0,      ///< Starting Page ID for user assignments
    GSLC_VIEW_ID_SCREEN     = -3999,  ///< Default Viewport ID used for main screen
    // Internal usage
    GSLC_VIEW_ID_NONE,                ///< No Viewport ID has been assigned
  } gslc_teViewId;

  /// Group ID enumerations
  typedef enum {
    // Public usage
    GSLC_GROUP_ID_USER_BASE = 0,      ///< Starting Group ID for user assignments
    // Internal usage
    GSLC_GROUP_ID_NONE      = -6999,  ///< No Group ID has been assigned
  } gslc_teGroupId;
  
  /// Font ID enumerations
  /// - The Font ID is the primary means for user code to
  ///   reference a specific font.
  /// - Application code can assign arbitrary Font ID values
  ///   in the range of 0...32767
  /// - Negative Font ID values are reserved    
  typedef enum {
    // Public usage
    GSLC_FONT_USER_BASE     = 0,      ///< Starting Font ID for user assignments
    GSLC_FONT_NONE          = -4999,  ///< No Font ID has been assigned
  } gslc_teFontId;
  

  /// View Index enumerations
  /// - The View Index is used for internal purposes as an offset
  //    into the GUI's array of views
  typedef enum {
    // Internal usage
    GSLC_VIEW_IND_NONE      = -5999,  ///< No Viewport Index is available
    GSLC_VIEW_IND_SCREEN,             ///< Default Viewport Index used for main screen
  } gslc_teViewInd;
  
  /// Element Index enumerations
  /// - The Element Index is used for internal purposes as an offset
  //    into the GUI's array of elements
  typedef enum {
    // Internal usage
    GSLC_IND_NONE           = -9999,  ///< No Element Index is available
    GSLC_IND_FIRST          = 0,      ///< User elements start at index 0
  } gslc_teElemInd;


/// Element type
typedef enum {
    // Core elements:
    GSLC_TYPE_NONE,         ///< No element type specified
    GSLC_TYPE_BKGND,        ///< Background element type
    GSLC_TYPE_BTN,          ///< Button element type
    GSLC_TYPE_TXT,          ///< Text label element type
    GSLC_TYPE_BOX,          ///< Box / frame element type
    GSLC_TYPE_LINE,         ///< Line element type
    // Extended elements:
    GSLC_TYPEX_GAUGE,       ///< Guage / progressbar extended element
    GSLC_TYPEX_CHECKBOX,    ///< Checkbox extended element
    GSLC_TYPEX_SLIDER,      ///< Slider extended element
    GSLC_TYPEX_SELNUM,      ///< SelNum extended element
} gslc_teType;

// Element text alignment
#define GSLC_ALIGNV_TOP       0x10                                ///< Vertical align to top
#define GSLC_ALIGNV_MID       0x20                                ///< Vertical align to middle
#define GSLC_ALIGNV_BOT       0x40                                ///< Vertical align to bottom
#define GSLC_ALIGNH_LEFT      0x01                                ///< Horizontal align to left
#define GSLC_ALIGNH_MID       0x02                                ///< Horizontal align to middle
#define GSLC_ALIGNH_RIGHT     0x04                                ///< Horizontal align to right
#define GSLC_ALIGN_TOP_LEFT   GSLC_ALIGNH_LEFT  | GSLC_ALIGNV_TOP ///< Align to top-left
#define GSLC_ALIGN_TOP_MID    GSLC_ALIGNH_MID   | GSLC_ALIGNV_TOP ///< Align to middle of top
#define GSLC_ALIGN_TOP_RIGHT  GSLC_ALIGNH_RIGHT | GSLC_ALIGNV_TOP ///< Align to top-right
#define GSLC_ALIGN_MID_LEFT   GSLC_ALIGNH_LEFT  | GSLC_ALIGNV_MID ///< Align to middle of left side
#define GSLC_ALIGN_MID_MID    GSLC_ALIGNH_MID   | GSLC_ALIGNV_MID ///< Align to center
#define GSLC_ALIGN_MID_RIGHT  GSLC_ALIGNH_RIGHT | GSLC_ALIGNV_MID ///< Align to middle of right side
#define GSLC_ALIGN_BOT_LEFT   GSLC_ALIGNH_LEFT  | GSLC_ALIGNV_BOT ///< Align to bottom-left
#define GSLC_ALIGN_BOT_MID    GSLC_ALIGNH_MID   | GSLC_ALIGNV_BOT ///< Align to middle of bottom
#define GSLC_ALIGN_BOT_RIGHT  GSLC_ALIGNH_RIGHT | GSLC_ALIGNV_BOT ///< Align to bottom-right


// Basic color definition
#define GSLC_COL_RED_DK4    (gslc_Color) {128,  0,  0}   ///< Red (dark4)
#define GSLC_COL_RED_DK3    (gslc_Color) {160,  0,  0}   ///< Red (dark3)
#define GSLC_COL_RED_DK2    (gslc_Color) {192,  0,  0}   ///< Red (dark2)
#define GSLC_COL_RED_DK1    (gslc_Color) {224,  0,  0}   ///< Red (dark1)
#define GSLC_COL_RED        (gslc_Color) {255,  0,  0}   ///< Red
#define GSLC_COL_RED_LT1    (gslc_Color) {255, 32, 32}   ///< Red (light1)
#define GSLC_COL_RED_LT2    (gslc_Color) {255, 64, 64}   ///< Red (light2)
#define GSLC_COL_RED_LT3    (gslc_Color) {255, 96, 96}   ///< Red (light3)
#define GSLC_COL_RED_LT4    (gslc_Color) {255,128,128}   ///< Red (light4)
#define GSLC_COL_GREEN_DK4  (gslc_Color) {  0,128,  0}   ///< Green (dark4)
#define GSLC_COL_GREEN_DK3  (gslc_Color) {  0,160,  0}   ///< Green (dark3)
#define GSLC_COL_GREEN_DK2  (gslc_Color) {  0,192,  0}   ///< Green (dark2)
#define GSLC_COL_GREEN_DK1  (gslc_Color) {  0,224,  0}   ///< Green (dark1)
#define GSLC_COL_GREEN      (gslc_Color) {  0,255,  0}   ///< Green
#define GSLC_COL_GREEN_LT1  (gslc_Color) { 32,255, 32}   ///< Green (light1)
#define GSLC_COL_GREEN_LT2  (gslc_Color) { 64,255, 64}   ///< Green (light2)
#define GSLC_COL_GREEN_LT3  (gslc_Color) { 96,255, 96}   ///< Green (light3)
#define GSLC_COL_GREEN_LT4  (gslc_Color) {128,255,128}   ///< Green (light4)
#define GSLC_COL_BLUE_DK4   (gslc_Color) {  0,  0,128}   ///< Blue (dark4)
#define GSLC_COL_BLUE_DK3   (gslc_Color) {  0,  0,160}   ///< Blue (dark3)
#define GSLC_COL_BLUE_DK2   (gslc_Color) {  0,  0,192}   ///< Blue (dark2)
#define GSLC_COL_BLUE_DK1   (gslc_Color) {  0,  0,224}   ///< Blue (dark1)
#define GSLC_COL_BLUE       (gslc_Color) {  0,  0,255}   ///< Blue
#define GSLC_COL_BLUE_LT1   (gslc_Color) { 32, 32,255}   ///< Blue (light1)
#define GSLC_COL_BLUE_LT2   (gslc_Color) { 64, 64,255}   ///< Blue (light2)
#define GSLC_COL_BLUE_LT3   (gslc_Color) { 96, 96,255}   ///< Blue (light3)
#define GSLC_COL_BLUE_LT4   (gslc_Color) {128,128,255}   ///< Blue (light4)
#define GSLC_COL_BLACK      (gslc_Color) {  0,  0,  0}   ///< Black
#define GSLC_COL_GRAY_DK3   (gslc_Color) { 32, 32, 32}   ///< Gray (dark)
#define GSLC_COL_GRAY_DK2   (gslc_Color) { 64, 64, 64}   ///< Gray (dark)
#define GSLC_COL_GRAY_DK1   (gslc_Color) { 96, 96, 96}   ///< Gray (dark)
#define GSLC_COL_GRAY       (gslc_Color) {128,128,128}   ///< Gray
#define GSLC_COL_GRAY_LT1   (gslc_Color) {160,160,160}   ///< Gray (light1)
#define GSLC_COL_GRAY_LT2   (gslc_Color) {192,192,192}   ///< Gray (light2)
#define GSLC_COL_GRAY_LT3   (gslc_Color) {224,224,224}   ///< Gray (light3)
#define GSLC_COL_WHITE      (gslc_Color) {255,255,255}   ///< White

#define GSLC_COL_YELLOW     (gslc_Color) {255,255,0}     ///< Yellow
#define GSLC_COL_YELLOW_DK  (gslc_Color) {64,64,0}       ///< Yellow (dark)
#define GSLC_COL_PURPLE     (gslc_Color) {128,0,128}     ///< Purple
#define GSLC_COL_CYAN       (gslc_Color) {0,255,255}     ///< Cyan
#define GSLC_COL_MAGENTA    (gslc_Color) {255,0,255}     ///< Magenta
#define GSLC_COL_TEAL       (gslc_Color) {0,128,128}     ///< Teal
#define GSLC_COL_ORANGE     (gslc_Color) {255,165,0}     ///< Orange
#define GSLC_COL_BROWN      (gslc_Color) {165,42,42}     ///< Brown



/// Touch event type for element touch tracking
typedef enum  {
  GSLC_TOUCH_NONE,
  GSLC_TOUCH_DOWN,      ///< Touch event (down) before associating with an element
  GSLC_TOUCH_MOVE,      ///< Touch event (move) before associating with an element
  GSLC_TOUCH_UP,        ///< Touch event (up) before associating with an element
  
  GSLC_TOUCH_DOWN_IN,   ///< Touch event (down) inside element, start tracking
  GSLC_TOUCH_MOVE_IN,   ///< Touch event (move/drag) inside tracked element
  GSLC_TOUCH_MOVE_OUT,  ///< Touch event (move/drag) outside tracked element
  GSLC_TOUCH_UP_IN,     ///< Touch event (up) inside tracked element
  GSLC_TOUCH_UP_OUT,    ///< Touch event (up) outside tracked element
} gslc_teTouch;

/// Event types
typedef enum {
  GSLC_EVT_NONE,          ///< No event; ignore
  GSLC_EVT_DRAW,          ///< Perform redraw
  GSLC_EVT_TOUCH,         ///< Track touch event
  GSLC_EVT_TICK,          ///< Perform background tick handling
  GSLV_EVT_CUSTOM         ///< Custom event
} gslc_teEventType;

typedef enum {
  GSLC_EVTSUB_NONE,
  GSLC_EVTSUB_DRAW_NEEDED,
  GSLC_EVTSUB_DRAW_FORCE        
} gslc_teEventSubType;

// -----------------------------------------------------------------------
// Forward declarations
// -----------------------------------------------------------------------

typedef struct gslc_tsElem  gslc_tsElem;
typedef struct gslc_tsEvent gslc_tsEvent;

// -----------------------------------------------------------------------
// Types
// -----------------------------------------------------------------------


/// Callback function for element drawing
typedef bool (*GSLC_CB_EVENT)(void* pvGui,gslc_tsEvent sEvent);
        
/// Callback function for element drawing
typedef bool (*GSLC_CB_DRAW)(void* pvGui,void* pvElem);

/// Callback function for element touch tracking
typedef bool (*GSLC_CB_TOUCH)(void* pvGui,void* pvElem,gslc_teTouch eTouch,int nX,int nY);

/// Callback function for element tick
typedef bool (*GSLC_CB_TICK)(void* pvGui,void* pvElem);


// -----------------------------------------------------------------------
// Structures
// -----------------------------------------------------------------------


/// Rectangular region. Defines X,Y corner coordinates plus dimensions.
typedef struct gslc_Rect {
  int16_t   x;    ///< X coordinate of cornder
  int16_t   y;    ///< Y coordinate of corner
  uint16_t  w;    ///< Width of region
  uint16_t  h;    ///< Height of region
} gslc_Rect;

/// Color structure. Defines RGB triplet.
typedef struct gslc_Color {
  uint8_t r;      ///< RGB red value
  uint8_t g;      ///< RGB green value
  uint8_t b;      ///< RGB blue value
  uint8_t unused; ///< Unused value to pad structure
} gslc_Color;

/// Event structure
typedef struct gslc_tsEvent {
  gslc_teEventType  eType;    ///< Event type
  uint32_t          nSubType; ///< Event sub-type
  void*             pvScope;  ///< Event target scope (eg. Page,Collection,Event)
  void*             pvData;   ///< Generic data pointer for event. This member is
                              ///< used to either pass a pointer to a simple data
                              ///< datatype (such as Element or Collection) or
                              ///< to a another structure that contains multiple fields.
} gslc_tsEvent;

/// Structure used to pass touch data through event
typedef struct gslc_tsEventTouch {
  gslc_teTouch      eTouch;           ///< Touch state
  int16_t           nX;               ///< Touch X coordinate (absolute)
  int16_t           nY;               ///< Touch Y coordinate (absolute)
} gslc_tsEventTouch;



///
/// Element Struct
/// - Represents a single graphic element in the GUIslice environment
/// - A page is made up of a number of elements
/// - Each element is created with a user-specified ID for further
///   accesses (or GSLC_ID_AUTO for it to be auto-generated)
/// - Display order of elements in a page is based upon the creation order
/// - Extensions to the core element types is provided through the
///   pXData reference and pfuncX* callback functions.
///
typedef struct gslc_tsElem {

  int                 nId;              ///< Element ID specified by user
  bool                bValid;           ///< Element was created properly

  gslc_teType         nType;            ///< Element type enumeration
  gslc_Rect           rElem;            ///< Rect region containing element
  int                 nGroup;           ///< Group ID that the element belongs to  
  
  // Behavior settings
  bool                bGlowEn;          ///< Enable glowing visual state
  bool                bClickEn;         ///< Element accepts touch events
 

  // Style
  bool                bFrameEn;         ///< Element is drawn with frame
  bool                bFillEn;          ///< Element is drawn with inner fill.
                                        ///< This is also used during redraw to determine
                                        ///< if elements underneath are visible and must
                                        ///< be redrawn as well.
  
  gslc_Color          colElemFrame;     ///< Color for frame
  gslc_Color          colElemFill;      ///< Color for background fill
  gslc_Color          colElemFrameGlow; ///< Color to use for frame when glowing  
  gslc_Color          colElemFillGlow;  ///< Color to use for fill when glowing
  
  void*               pvSurfNorm;       ///< Surface ptr to draw (normal)
  void*               pvSurfGlow;       ///< Surface ptr to draw (glowing)

  /// Parent element reference. Used during redraw
  /// to notify parent elements that they require
  /// redraw as well. Primary usage is in compound
  /// elements.
  gslc_tsElem*        pElemParent;  
  
  // Text handling
  char                acStr[GSLC_ELEM_STRLEN_MAX];  ///< Text string to overlay
  gslc_Color          colElemText;      ///< Color of overlay text
  gslc_Color          colElemTextGlow;  ///< Color of overlay text when glowing
  unsigned            eTxtAlign;        ///< Alignment of overlay text
  unsigned            nTxtMargin;       ///< Margin of overlay text within rect region
  void*               pvTxtFont;        ///< Void ptr to Font for overlay text

  // Extended data elements
  void*               pXData;           ///< Ptr to extended data structure
  
  // Callback functions
  GSLC_CB_EVENT       pfuncXEvent;      ///< Callback func ptr for events
  GSLC_CB_DRAW        pfuncXDraw;       ///< Callback func ptr for drawing
  GSLC_CB_TOUCH       pfuncXTouch;      ///< Callback func ptr for touch
  GSLC_CB_TICK        pfuncXTick;       ///< Callback func ptr for timer/main loop tick
  
  // Current status
  bool                bNeedRedraw;      ///< Element needs to be redrawn
  bool                bGlowing;         ///< Element is currently glowing

} gslc_tsElem;


/// Element collection struct
/// - Collections are used to maintain a list of elements and any
///   touch tracking status.
/// - Pages and Compound Elements both instantiate a Collection
typedef struct {
  gslc_tsElem*          asElem;           ///< Array of elements
  unsigned              nElemMax;         ///< Maximum number of elements to allocate
  unsigned              nElemCnt;         ///< Number of elements allocated
  unsigned              nElemAutoIdNext;  ///< Next Element ID for auto-assignment
  
  gslc_tsElem*          pElemTracked;     ///< Element currently being touch-tracked (NULL for none)
  
  // Callback functions
  GSLC_CB_EVENT         pfuncXEvent;      ///< Callback func ptr for events  
  
} gslc_tsCollect;


/// Font reference structure
typedef struct {
  int     nId;      ///< Font ID specified by user
  void*   pvFont;   ///< Void ptr to the Font (type defined by driver)
} gslc_tsFont;


/// Viewport structure
/// - Viewports allow drawing operations to be done within
///   a local coordinate system and clipped to a region.
typedef struct {
  int               nId;        ///< View ID specified by user
  gslc_Rect         rView;      ///< Rect defining the viewport
  unsigned          nOriginX;   ///< Position within rView for local origin
  unsigned          nOriginY;   ///< Position within rView for local origin
} gslc_tsView;


/// Page structure
/// - A page contains a collection of elements
/// - Many redraw functions operate at a page level
/// - Maintains state as to whether redraw or screen flip is required
typedef struct {

  gslc_tsCollect      sCollect;             ///< Collection of elements on page
  
  int                 nPageId;              ///< Page identifier
  
  // Redraw
  bool                bPageNeedRedraw;      ///< Page require a redraw
  bool                bPageNeedFlip;        ///< Screen requires a page flip
  
  // Callback functions
  GSLC_CB_EVENT       pfuncXEvent;          ///< Callback func ptr for events   
  
} gslc_tsPage;


/// GUI structure
/// - Contains all GUI state and content
/// - Maintains list of one or more pages
typedef struct {

  gslc_tsFont*        asFont;           ///< Collection of loaded fonts
  unsigned            nFontMax;         ///< Maximum number of fonts to allocate
  unsigned            nFontCnt;         ///< Number of fonts allocated

  gslc_tsElem         sElemTmp;         ///< Temporary element
  
  gslc_tsView*        asView;           ///< Collection of viewports
  unsigned            nViewMax;         ///< Maximum number of viewports to allocate
  unsigned            nViewCnt;         ///< Number of viewports allocated
  int                 nViewIndCur;      ///< Currently-active viewport index

  int                 nTouchLastX;      ///< Last touch event X coord
  int                 nTouchLastY;      ///< Last touch event Y coord
  unsigned            nTouchLastPress;  ///< Last touch event pressure (0=none))

  // Touchscreen library interface
  #ifdef INC_TS
  struct tsdev*       ts;               ///< Ptr to touchscreen device
  #endif

 
  // Primary surface definitions
  void*               pvSurfScreen;     ///< Surface ptr for screen
  void*               pvSurfBkgnd;      ///< Surface ptr for background
  
  // Pages
  gslc_tsPage*        asPage;           ///< Array of pages
  unsigned            nPageMax;         ///< Maximum number of pages
  unsigned            nPageCnt;         ///< Current page index
  
  gslc_tsPage*        pCurPage;         ///< Currently active page
  gslc_tsCollect*     pCurPageCollect;  ///< Ptr to active page collection
  
  // Callback functions
  GSLC_CB_EVENT       pfuncXEvent;          ///< Callback func ptr for events 
  
} gslc_tsGui;



// ------------------------------------------------------------------------
// General Functions
// ------------------------------------------------------------------------

///
/// Get the GUIslice version number
///
/// \return String containing version number
///
char* gslc_GetVer(gslc_tsGui* pGui);

///
/// Configure environment variables suitable for
/// default driver operations.
///
/// - NOTE: Users with other hardware may need to
///   supply their own initialization routine
///   as these defaults may not be suitable.
///
/// \param[in]  acDevFb:    Path to framebuffer device (NULL if none)
/// \param[in]  acDevTouch: Path to touchscreen device (NULL if none)
///
/// \return None
///
void gslc_InitEnv(char* acDevFb,char* acDevTouch);

///
/// Initialize the GUIslice library
/// - Configures the primary screen surface(s)
/// - Initializes font support
///
/// PRE:
/// - The environment variables should be configured before
///   calling gslc_Init(). This can be done with gslc_InitEnv()
///   or manually in user function.
///
/// \param[in]  pGui:      Pointer to GUI
/// \param[in]  asPage:    Pointer to Page array
/// \param[in]  nMaxPage:  Size of Page array
/// \param[in]  asFont:    Pointer to Font array
/// \param[in]  nMaxFont:  Size of Font array
/// \param[in]  asView:    Pointer to View array
/// \param[in]  nMaxView:  Size of View array
///
/// \return true if success, false if fail
///
bool gslc_Init(gslc_tsGui* pGui,gslc_tsPage* asPage,unsigned nMaxPage,gslc_tsFont* asFont,unsigned nMaxFont,gslc_tsView* asView,unsigned nMaxView);




///
/// Exit the GUIslice environment
/// - Calls SDL Quit to clean up any initialized subsystems
///   and also deletes any created elements or fonts
///
/// \param[in]  pGui:    Pointer to GUI
///
/// \return None
///
void gslc_Quit(gslc_tsGui* pGui);


///
/// Perform main GUIslice handling functions
/// - Handles any touch events
/// - Performs any necessary screen redraw
///
/// \param[in]  pGui:    Pointer to GUI
///
/// \return None
///
void gslc_Update(gslc_tsGui* pGui);


///
/// Create an event structure
///
/// \param[in]  eType:      Event type (draw, touch, tick, etc.)
/// \param[in]  nSubType:   Refinement of event type (or 0 if unused)
/// \param[in]  pvScope:    Void ptr to object receiving event so that
///                         the event handler will have the context
/// \param[in]  pvData:     Void ptr to additional data associated with
///                         the event (eg. coordinates for touch events)
///
/// \return None
///
gslc_tsEvent gslc_EventCreate(gslc_teEventType eType,uint32_t nSubType,void* pvScope,void* pvData);


// ------------------------------------------------------------------------
// Graphics General Functions
// ------------------------------------------------------------------------


///
/// Determine if a coordinate is inside of a rectangular region.
/// - This routine is useful in determining if a touch
///   coordinate is inside of a button.
///
/// \param[in]  nSelX:       X coordinate to test
/// \param[in]  nSelY:       X coordinate to test
/// \param[in]  rRect:       Rectangular region to compare against
///
/// \return true if inside region, false otherwise
///
bool gslc_IsInRect(int nSelX,int nSelY,gslc_Rect rRect);


///
/// Expand or contract a rectangle in width and/or height (equal
/// amounts on both side), based on the centerpoint of the rectangle.
///
/// \param[in]  rRect:       Rectangular region before resizing
/// \param[in]  nExpandW:    Number of pixels to expand the width (if positive)
///                          of contract the width (if negative)
/// \param[in]  nExpandH:    Number of pixels to expand the height (if positive)
///                          of contract the height (if negative)
///
/// \return gslc_Rect() with resized dimensions
///
gslc_Rect gslc_ExpandRect(gslc_Rect rRect,int16_t nExpandW,int16_t nExpandH);


///
/// Determine if a coordinate is inside of a width x height region.
/// - This routine is useful in determining if a relative coordinate
///   is within a given W x H dimension
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nSelX:       X coordinate to test
/// \param[in]  nSelY:       X coordinate to test
/// \param[in]  nWidth:      Width to test against
/// \param[in]  nHeight:     Height to test against
///
/// \return true if inside region, false otherwise
///
bool gslc_IsInWH(gslc_tsGui* pGui,int nSelX,int nSelY,uint16_t nWidth,uint16_t nHeight);




// ------------------------------------------------------------------------
// Graphics Primitive Functions
// - These routines cause immediate drawing to occur on the
//   primary screen
// ------------------------------------------------------------------------


///
/// Set a pixel on the active screen to the given color with lock
/// - Calls upon gslc_DrvDrawSetPixelRaw() but wraps with a surface lock lock
/// - If repeated access is needed, use gslc_DrvDrawSetPixelRaw() instead
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nX:          Pixel X coordinate to set
/// \param[in]  nY:          Pixel Y coordinate to set
/// \param[in]  nPixelCol:   Color pixel value ot assign
/// \param[in]  bMapEn:      Support viewport remapping
///
/// \return none
///
void gslc_DrawSetPixel(gslc_tsGui* pGui,int16_t nX,int16_t nY,gslc_Color nCol,bool bMapEn);


///
/// Draw an arbitrary line using Bresenham's algorithm
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nX0:         X coordinate of line startpoint
/// \param[in]  nY0:         Y coordinate of line startpoint
/// \param[in]  nX1:         X coordinate of line endpoint
/// \param[in]  nY1:         Y coordinate of line endpoint
/// \param[in]  nCol:        Color RGB value for the line
///
/// \return none
///
void gslc_DrawLine(gslc_tsGui* pGui,int16_t nX0,int16_t nY0,int16_t nX1,int16_t nY1,gslc_Color nCol);


///
/// Draw a horizontal line
/// - Note that direction of line is in +ve X axis
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nX:          X coordinate of line startpoint
/// \param[in]  nY:          Y coordinate of line startpoint
/// \param[in]  nW:          Width of line (in +X direction)
/// \param[in]  nCol:        Color RGB value for the line
///
/// \return none
///
void gslc_DrawLineH(gslc_tsGui* pGui,int16_t nX, int16_t nY, uint16_t nW,gslc_Color nCol);


///
/// Draw a vertical line
/// - Note that direction of line is in +ve Y axis
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nX:          X coordinate of line startpoint
/// \param[in]  nY:          Y coordinate of line startpoint
/// \param[in]  nH:          Height of line (in +Y direction)
/// \param[in]  nCol:        Color RGB value for the line
///
/// \return none
///
void gslc_DrawLineV(gslc_tsGui* pGui,int16_t nX, int16_t nY, uint16_t nH,gslc_Color nCol);


///
/// Draw a framed rectangle
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  rRect:       Rectangular region to frame
/// \param[in]  nCol:        Color RGB value for the frame
///
/// \return none
///
void gslc_DrawFrameRect(gslc_tsGui* pGui,gslc_Rect rRect,gslc_Color nCol);


///
/// Draw a filled rectangle
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  rRect:       Rectangular region to fill
/// \param[in]  nCol:        Color RGB value to fill
///
/// \return none
///
void gslc_DrawFillRect(gslc_tsGui* pGui,gslc_Rect rRect,gslc_Color nCol);

///
/// Draw a framed circle
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nMidX:       Center X coordinate
/// \param[in]  nMidY:       Center Y coordinate
/// \param[in]  nRadius:     Radius of circle
/// \param[in]  nCol:        Color RGB value for the frame
///
/// \return none
///
void gslc_DrawFrameCircle(gslc_tsGui* pGui,int16_t nMidX,int16_t nMidY,
  uint16_t nRadius,gslc_Color nCol);



// -----------------------------------------------------------------------
// Font Functions
// -----------------------------------------------------------------------

///
/// Load a font into the local font cache and assign
/// font ID (nFontId).
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nFontId:     ID to use when referncing this font
/// \param[in]  acFontName:  Filename path to the font
/// \param[in]  nFontSz:     Typeface size to use
///
/// \return true if load was successful, false otherwise
///
bool gslc_FontAdd(gslc_tsGui* pGui,int nFontId, const char* acFontName, unsigned nFontSz);


///
/// Fetch a font from its ID value
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nFontId:     ID value used to reference the font (supplied
///                          originally to gslc_FontAdd()
///
/// \return A void pointer to the font data or NULL if error
///         The type is defined by the driver.
///
void* gslc_FontGet(gslc_tsGui* pGui,int nFontId);




// ------------------------------------------------------------------------
// Page Functions
// ------------------------------------------------------------------------

///
/// Common event handler function for a page
///
/// \param[in]  pvGui:       Void pointer to GUI
/// \param[in]  sEvent:      Event data structure
///
/// \return true if success, false if fail
/// 
bool gslc_PageEvent(void* pvGui,gslc_tsEvent sEvent);

///
/// Assign the event callback function for a page
///
/// \param[in]  pPage:       Pointer to page
/// \param[in]  funcCb:      Function pointer to event routine (or NULL for default))
///
/// \return none
///
void gslc_PageSetEventFunc(gslc_tsPage* pPage,GSLC_CB_EVENT funcCb);

///
/// Fetch the current page ID
///
/// \param[in]  pGui:        Pointer to GUI
///
/// \return Page ID
///

int gslc_GetPageCur(gslc_tsGui* pGui);


///
/// Select a new page for display
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nPageId:     Page ID to select as current
///
/// \return none
///
void gslc_SetPageCur(gslc_tsGui* pGui,int nPageId);


///
/// Update the need-redraw status for the current page
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  bRedraw:     True if redraw required, false otherwise
///
/// \return none
///
void gslc_PageRedrawSet(gslc_tsGui* pGui,bool bRedraw);


///
/// Get the need-redraw status for the current page
///
/// \param[in]  pGui:        Pointer to GUI
///
/// \return True if redraw required, false otherwise
///
bool gslc_PageRedrawGet(gslc_tsGui* pGui);


///
/// Redraw all elements on the active page. Only the
/// elements that have been marked as needing redraw are
/// rendered unless the entire page has been marked as
/// needing redraw (in which case everything is drawn)
///
/// \param[in]  pGui:        Pointer to GUI
///
/// \return none
///
void gslc_PageRedrawGo(gslc_tsGui* pGui);

///
/// Indicate whether the screen requires page flip
/// - This is generally called with bNeeded=true whenever
///   drawing has been done to the active page. Page flip
///   is actually performed later when calling PageFlipGo().
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  bNeeded:     True if screen requires page flip
///
/// \return None
///
void gslc_PageFlipSet(gslc_tsGui* pGui,bool bNeeded);


///
/// Get state of pending page flip state
///
/// \param[in]  pGui:        Pointer to GUI
///
/// \return True if screen requires page flip
///
bool gslc_PageFlipGet(gslc_tsGui* pGui);


///
/// Update the visible screen if page has been marked for flipping
/// - On some hardware this can trigger a double-buffering
///   page flip.
///
/// \param[in]  pGui:        Pointer to GUI
///
/// \return None
///
void gslc_PageFlipGo(gslc_tsGui* pGui);


/// Add a page to the GUI
/// - This call associates an element array with the collection within the page
/// - Once a page has been added to the GUI, elements can be added to the page
///   by specifying the same page ID
///
/// \param[in]  pGui:         Pointer to GUI
/// \param[in]  nPageId:      Page ID to assign
/// \param[in]  psElem:       Element array storage to associate with the page
/// \param[in]  nMaxElem:     Maximum number of elements that can be added to page
///
/// \return none
///
void gslc_PageAdd(gslc_tsGui* pGui,int nPageId,gslc_tsElem* psElem,unsigned nMaxElem);

/// Find a page in the GUI by its ID
///
/// \param[in]  pGui:         Pointer to GUI
/// \param[in]  nPageId:      Page ID to search
///
/// \return Ptr to a page or NULL if none found
///
gslc_tsPage* gslc_PageFindById(gslc_tsGui* pGui,int nPageId);

/// Find an element in the GUI by its Page ID and Element ID
///
/// \param[in]  pGui:         Pointer to GUI
/// \param[in]  nPageId:      Page ID to search
/// \param[in]  nElemId:      Element ID to search
///
/// \return Ptr to an element or NULL if none found
///
gslc_tsElem* gslc_PageFindElemById(gslc_tsGui* pGui,int nPageId,int nElemId);

/// Perform a redraw calculation on the page to determine if additional
/// elements should also be redrawn. This routine checks to see if any
/// transparent elements have been marked as needing redraw. If so, the
/// whole page may be marked as needing redraw (or at least the other
/// elements that have been exposed underneath).
///
/// \param[in]  pGui:         Pointer to GUI
///
/// \return none
///
void gslc_PageRedrawCalc(gslc_tsGui* pGui);


// ------------------------------------------------------------------------
// Element Creation Functions
// ------------------------------------------------------------------------


///
/// Create a Text Element
/// - Draws a text string with filled background
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nElemId:     Element ID to assign (0..32767 or GSLC_ID_AUTO to autogen)
/// \param[in]  nPage:       Page ID to attach element to
/// \param[in]  rElem:       Rectangle coordinates defining text background size
/// \param[in]  pStr:        String to copy into element
/// \param[in]  nFontId:     Font ID to use for text display
///
/// \return Pointer to the Element or NULL if failure
///
gslc_tsElem* gslc_ElemCreateTxt(gslc_tsGui* pGui,int nElemId,int nPage,
  gslc_Rect rElem,const char* pStr,int nFontId);


///
/// Create a textual Button Element
/// - Creates a clickable element that has a textual label
///   with frame and fill
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nElemId:     Element ID to assign (0..32767 or GSLC_ID_AUTO to autogen)
/// \param[in]  nPage:       Page ID to attach element to
/// \param[in]  rElem:       Rectangle coordinates defining text background size
/// \param[in]  acStr:       String to copy into element
/// \param[in]  nFontId:     Font ID to use for text display
/// \param[in]  cbTouch:     Callback for touch events
///
/// \return Pointer to the Element or NULL if failure
///
gslc_tsElem* gslc_ElemCreateBtnTxt(gslc_tsGui* pGui,int nElemId,int nPage,
  gslc_Rect rElem,const char* acStr,int nFontId,GSLC_CB_TOUCH cbTouch);

///
/// Create a graphical Button Element
/// - Creates a clickable element that uses a BMP image with
///   no frame or fill
/// - Transparency is supported by bitmap color (0xFF00FF)
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nElemId:     Element ID to assign (0..32767 or GSLC_ID_AUTO to autogen)
/// \param[in]  nPage:       Page ID to attach element to
/// \param[in]  rElem:       Rectangle coordinates defining image size
/// \param[in]  acImg:       Filename of BMP image to load (unselected state)
/// \param[in]  acImgSel:    Filename of BMP image to load (selected state)
/// \param[in]  cbTouch:     Callback for touch events
///
/// \return Pointer to the Element or NULL if failure
///
gslc_tsElem* gslc_ElemCreateBtnImg(gslc_tsGui* pGui,int nElemId,int nPage,
  gslc_Rect rElem,const char* acImg,const char* acImgSel,GSLC_CB_TOUCH cbTouch);


///
/// Create a Box Element
/// - Draws a box with frame and fill
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nElemId:     Element ID to assign (0..32767 or GSLC_ID_AUTO to autogen)
/// \param[in]  nPage:       Page ID to attach element to
/// \param[in]  rElem:       Rectangle coordinates defining box size
///
/// \return Pointer to the Element or NULL if failure
///
gslc_tsElem* gslc_ElemCreateBox(gslc_tsGui* pGui,int nElemId,int nPage,gslc_Rect rElem);

///
/// Create an image Element
/// - Draws an image
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nElemId:     Element ID to assign (0..32767 or GSLC_ID_AUTO to autogen)
/// \param[in]  nPage:       Page ID to attach element to
/// \param[in]  rElem:       Rectangle coordinates defining box size
/// \param[in]  acImg:       Filename of BMP image to load
///
/// \return Pointer to the Element or NULL if failure
///
gslc_tsElem* gslc_ElemCreateImg(gslc_tsGui* pGui,int nElemId,int nPage,gslc_Rect rElem,
  const char* acImg);



// ------------------------------------------------------------------------
// Element Update Functions
// ------------------------------------------------------------------------


/// Get an Element ID from an element structure
///
/// \param[in]  pElem:       Pointer to element structure
///
/// \return ID of element or GSLC_ID_NONE if not found
///
int gslc_ElemGetId(gslc_tsElem* pElem);


///
/// Set the fill state for an Element
///
/// \param[in]  pElem:       Pointer to Element
/// \param[in]  bFillEn:     True if filled, false otherwise
///
/// \return none
///
void gslc_ElemSetFillEn(gslc_tsElem* pElem,bool bFillEn);

///
/// Set the frame state for an Element
///
/// \param[in]  pElem:       Pointer to Element
/// \param[in]  bFrameEn:    True if framed, false otherwise
///
/// \return none
///
void gslc_ElemSetFrameEn(gslc_tsElem* pElem,bool bFrameEn);

///
/// Update the common color selection for an Element
///
/// \param[in]  pElem:       Pointer to Element
/// \param[in]  colFrame:    Color for the frame
/// \param[in]  colFill:     Color for the fill
/// \param[in]  colFillGlow: Color for the fill when glowing
///
/// \return none
///
void gslc_ElemSetCol(gslc_tsElem* pElem,gslc_Color colFrame,gslc_Color colFill,gslc_Color colFillGlow);

///
/// Update the common color selection for glowing state of an Element
///
/// \param[in]  pElem:        Pointer to Element
/// \param[in]  colFrameGlow: Color for the frame when glowing
/// \param[in]  colFillGlow:  Color for the fill when glowing
/// \param[in]  colTxtGlow    Color for the text when glowing
///
/// \return none
///
void gslc_ElemSetGlowCol(gslc_tsElem* pElem,gslc_Color colFrameGlow,gslc_Color colFillGlow,gslc_Color colTxtGlow);


///
/// Set the group ID for an element
/// - Typically used to associate radio button elements together
///
/// \param[in]  pElem:       Pointer to Element
/// \param[in]  nGroupId:    Group ID to assign
///
/// \return none
///
void gslc_ElemSetGroup(gslc_tsElem* pElem,int nGroupId);


///
/// Get the group ID for an element
///
/// \param[in]  pElem:       Pointer to Element
///
/// \return Group ID or GSLC_GROUP_ID_NONE if unassigned
///
int gslc_ElemGetGroup(gslc_tsElem* pElem);


/// Set the alignment of a textual element (horizontal and vertical)
///
/// \param[in]  pElem:       Pointer to Element
/// \param[in]  nAlign:      Alignment to specify:
///                           - GSLC_ALIGN_TOP_LEFT
///                           - GSLC_ALIGN_TOP_MID
///                           - GSLC_ALIGN_TOP_RIGHT
///                           - GSLC_ALIGN_MID_LEFT
///                           - GSLC_ALIGN_MID_MID
///                           - GSLC_ALIGN_MID_RIGHT
///                           - GSLC_ALIGN_BOT_LEFT
///                           - GSLC_ALIGN_BOT_MID
///                           - GSLC_ALIGN_BOT_RIGHT
///
/// \return none
///
void gslc_ElemSetTxtAlign(gslc_tsElem* pElem,unsigned nAlign);

/// Set the margin around of a textual element
///
/// \param[in]  pElem:       Pointer to Element
/// \param[in]  nMargin:     Number of pixels gap to leave surrounding text
///
/// \return none
///
void gslc_ElemSetTxtMargin(gslc_tsElem* pElem,unsigned nMargin);

///
/// Update the text string associated with an Element ID
///
/// \param[in]  pElem:       Pointer to Element
/// \param[in]  pStr:        String to copy into element
///
/// \return none
///
void gslc_ElemSetTxtStr(gslc_tsElem* pElem,const char* pStr);

///
/// Update the text string color associated with an Element ID
///
/// \param[in]  pElem:       Pointer to Element
/// \param[in]  colVal:      RGB color to change to
///
/// \return none
///
void gslc_ElemSetTxtCol(gslc_tsElem* pElem,gslc_Color colVal);


///
/// Update the Font selected for an Element's text
///
/// \param[in]  pElem:       Pointer to Element
/// \param[in]  nFontId:     Font ID to select
///
/// \return none
///
void gslc_ElemUpdateFont(gslc_tsGui* pGui,gslc_tsElem* pElem,int nFontId);

///
/// Update the need-redraw status for an element
///
/// \param[in]  pElem:       Pointer to Element
/// \param[in]  bRedraw:     True if redraw required, false otherwise
///
/// \return none
///
void gslc_ElemSetRedraw(gslc_tsElem* pElem,bool bRedraw);

///
/// Update the glowing enable for an element
///
/// \param[in]  pElem:      Pointer to Element
/// \param[in]  bGlowEn:    True if element should support glowing
///
/// \return none
///
void gslc_ElemSetGlowEn(gslc_tsElem* pElem,bool bGlowEn);

///
/// Copy style settings from one element to another
///
/// \param[in]  pElemSrc:   Pointer to source Element
/// \param[in]  pElemDest:  Pointer to destination Element
///
/// \return none
///
void gslc_ElemSetStyleFrom(gslc_tsElem* pElemSrc,gslc_tsElem* pElemDest);

///
/// Get the glowing enable for an element
///
/// \param[in]  pElem:       Pointer to Element
///
/// \return True if element supports glowing
///
bool gslc_ElemGetGlowEn(gslc_tsElem* pElem);


///
/// Update the glowing indicator for an element
///
/// \param[in]  pElem:       Pointer to Element
/// \param[in]  bGlowing:    True if element is glowing
///
/// \return none
///
void gslc_ElemSetGlow(gslc_tsElem* pElem,bool bGlowing);


///
/// Get the glowing indicator for an element
///
/// \param[in]  pElem:       Pointer to Element
///
/// \return True if element is glowing
///
bool gslc_ElemGetGlow(gslc_tsElem* pElem);

///
/// Assign the event callback function for a element
///
/// \param[in]  pElem:       Pointer to element
/// \param[in]  funcCb:      Function pointer to event routine (or NULL for default))
///
/// \return none
/// 
void gslc_ElemSetEventFunc(gslc_tsElem* pElem,GSLC_CB_EVENT funcCb);


///
/// Assign the drawing callback function for an element
/// - This allows the user to override the default rendering for
///   an element, enabling the creation of a custom element
///
/// \param[in]  pElem:       Pointer to Element
/// \param[in]  funcCb:      Function pointer to drawing routine (or NULL for default))
///
/// \return none
///
void gslc_ElemSetDrawFunc(gslc_tsElem* pElem,GSLC_CB_DRAW funcCb);

///
/// Assign the tick callback function for an element
/// - This allows the user to provide background updates to
///   an element triggered by the main loop call to gslc_Update()
///
/// \param[in]  pElem:       Pointer to Element
/// \param[in]  funcCb:      Function pointer to tick routine (or NULL for none))
///
/// \return none
///
void gslc_ElemSetTickFunc(gslc_tsElem* pElem,GSLC_CB_TICK funcCb);


///
/// Determine if a coordinate is inside of an element
/// - This routine is useful in determining if a touch
///   coordinate is inside of a button.
///
/// \param[in]  nSelX:        X coordinate to test
/// \param[in]  nSelY:        Y coordinate to test
/// \param[in]  bOnlyClickEn: Only output true if element was also marked
///                           as "clickable" (eg. bClickEn=true)
///
/// \return true if inside element, false otherwise
///
bool gslc_ElemOwnsCoord(gslc_tsElem* pElem,int nX,int nY,bool bOnlyClickEn);


// ------------------------------------------------------------------------
// Element Event Handling Functions
// ------------------------------------------------------------------------

///
/// Common event handler function for an element
///
/// \param[in]  pvGui:       Void pointer to GUI
/// \param[in]  sEvent:      Event data structure
///
/// \return true if success, false if fail
/// 
bool gslc_ElemEvent(void* pvGui,gslc_tsEvent sEvent);


// ------------------------------------------------------------------------
// Element Drawing Functions
// ------------------------------------------------------------------------

///
/// Draw an element to the active display
/// - Element is referenced by a page ID and element ID
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nPageId:     ID of page containing element
/// \param[in]  nElemId:     ID of element
///
/// \return none
///
void gslc_ElemDraw(gslc_tsGui* pGui,int nPageId,int nElemId);



// ------------------------------------------------------------------------
// Viewport Functions
// ------------------------------------------------------------------------

///
/// Create a Viewport
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nViewId:     View ID to assign (0..32767)
/// \param[in]  rView:       Rectangle coordinates defining viewport position
/// \param[in]  nOriginX:    Relative X coordinates of origin
/// \param[in]  nOriginY:    Relative Y coordinates of origin
///
/// \return The Viewport ID or GSLC_VIEW_ID_NONE if failure
///
int gslc_ViewCreate(gslc_tsGui* pGui,int nViewId,gslc_Rect rView,unsigned nOriginX,unsigned nOriginY);


///
/// Set the origin for a Viewport
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nViewId:     Viewport ID to update
/// \param[in]  nOriginX:    Relative X coordinates of origin
/// \param[in]  nOriginY:    Relative Y coordinates of origin
///
/// \return true if success, false if failure
///
bool gslc_ViewSetOrigin(gslc_tsGui* pGui,int nViewId,unsigned nOriginX,unsigned nOriginY);

///
/// Sets the currently active view
/// - This function is used to swap between drawing within
///   a viewport (using local coordinates and clipping) and
///   the main view (the screen).
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nViewId:     Viewport ID to select or GSLC_VIEW_ID_SCREEN for main display
///
/// \return none
///
void gslc_ViewSet(gslc_tsGui* pGui,int nViewId);


// ------------------------------------------------------------------------
// Element Collection Functions
// ------------------------------------------------------------------------


/// Reset the members of an element collection
///
/// \param[in]  pCollect:     Pointer to the collection
/// \param[in]  asElem:       Array of elements to associate with the collection
/// \param[in]  nElemMax:     Maximum number of element in the element array
///
/// \return none
/// 
void gslc_CollectReset(gslc_tsCollect* pCollect,gslc_tsElem* asElem,unsigned nElemMax);


/// Add an element to a collection
/// - Note that the contents of pElem are copied to the collection's
///   element array so the pElem pointer can be discarded are the
///   call is complete.
///
/// \param[in]  pCollect:     Pointer to the collection
/// \param[in]  pElem:        Ptr to the element to add
///
/// \return Pointer to the element in the collection that has been added
///         or NULL if there was an error
/// 
gslc_tsElem* gslc_CollectElemAdd(gslc_tsCollect* pCollect,gslc_tsElem* pElem);


/// Find an element in a collection by its Element ID
///
/// \param[in]  pCollect:     Pointer to the collection
/// \param[in]  nElemId:      Element ID to search for
///
/// \return Pointer to the element in the collection that was found
///         or NULL if no matches found
/// 
gslc_tsElem* gslc_CollectFindElemById(gslc_tsCollect* pCollect,int nElemId);


/// Find an element in a collection by a coordinate coordinate
/// - A match is found if the element is "clickable" (bClickEn=true)
///   and the coordinate falls within the element's bounds (rElem).
///
/// \param[in]  pCollect:     Pointer to the collection
/// \param[in]  nX:           Absolute X coordinate to use for search
/// \param[in]  nY:           Absolute Y coordinate to use for search
///
/// \return Pointer to the element in the collection that was found
///         or NULL if no matches found
/// 
gslc_tsElem* gslc_CollectFindElemFromCoord(gslc_tsCollect* pCollect,int nX, int nY);


/// Allocate the next available Element ID in a collection
///
/// \param[in]  pCollect:     Pointer to the collection
///
/// \return Element ID that is reserved for use
/// 
int gslc_CollectGetNextId(gslc_tsCollect* pCollect);


/// Get the element within a collection that is currently being tracked
///
/// \param[in]  pCollect:     Pointer to the collection
///
/// \return Pointer to the element in the collection that is
///         currently being tracked or NULL if no elements
///         are being tracked
///
gslc_tsElem* gslc_CollectGetElemTracked(gslc_tsCollect* pCollect);


/// Set the element within a collection that is currently being tracked
///
/// \param[in]  pCollect:     Pointer to the collection
/// \param[in]  pElem:        Ptr to element to mark as being tracked
///
/// \return none
///
void gslc_CollectSetElemTracked(gslc_tsCollect* pCollect,gslc_tsElem* pElem);


/// Assign the parent element reference to all elements within a collection
/// - This is generally used in the case of compound elements where updates to
///   a sub-element should cause the parent (compound element) to be redrawn
///   as well.)
///
/// \param[in]  pCollect:     Pointer to the collection
/// \param[in]  pElemParent:  Ptr to element that is the parent
///
/// \return none
///
void gslc_CollectSetParent(gslc_tsCollect* pCollect,gslc_tsElem* pElemParent);


// ------------------------------------------------------------------------
// Collect Event Handlers
// ------------------------------------------------------------------------

///
/// Assign the event callback function for an element collection
///
/// \param[in]  pCollect:    Pointer to collection
/// \param[in]  funcCb:      Function pointer to event routine (or NULL for default))
///
/// \return none
/// 
void gslc_CollectSetEventFunc(gslc_tsCollect* pCollect,GSLC_CB_EVENT funcCb);

///
/// Common event handler function for an element collection
///
/// \param[in]  pvGui:       Void pointer to GUI
/// \param[in]  sEvent:      Event data structure
///
/// \return true if success, false if fail
/// 
bool gslc_CollectEvent(void* pvGui,gslc_tsEvent sEvent);


/// Handle touch events within the element collection
///
/// \param[in]  pGui:         Pointer to the GUI
/// \param[in]  pCollect:     Ptr to the element collection
/// \param[in]  pEventTouch:  Ptr to the touch event structure
///
/// \return none
///
void gslc_CollectTouch(gslc_tsGui* pGui,gslc_tsCollect* pCollect,gslc_tsEventTouch* pEventTouch);


// ------------------------------------------------------------------------
// Tracking Functions
// ------------------------------------------------------------------------


///
/// Handles a touch event and performs the necessary
/// tracking, glowing and selection actions depending
/// on the press state.
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pPage:       Pointer to current page
/// \param[in]  nX:          X coordinate of touch event
/// \param[in]  nY:          Y coordinate of touch event
/// \param[in]  nPress:      Pressure level of touch event (0 for none, else touch)
///
/// \return none
///
void gslc_TrackTouch(gslc_tsGui* pGui,gslc_tsPage* pPage,int nX,int nY,unsigned nPress);




// ------------------------------------------------------------------------
// Touchscreen Functions
// ------------------------------------------------------------------------

#ifdef INC_TS

///
/// Initialize the touchscreen device
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  acDev:       Device path to touchscreen
///                  eg. "/dev/input/touchscreen"
///
/// \return true if successful
///
bool gslc_InitTs(gslc_tsGui* pGui,const char* acDev);


///
/// Get the last touch event from the tslib handler
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[out] pnX:         Ptr to X coordinate of last touch event
/// \param[out] pnY:         Ptr to Y coordinate of last touch event
/// \param[out] pnPress:     Ptr to  Pressure level of last touch event (0 for none, >0 for touch)
///
/// \return non-zero if an event was detected or 0 otherwise
///
int gslc_GetTsTouch(gslc_tsGui* pGui,int* pnX, int* pnY, unsigned* pnPress);


#endif // INC_TS




// ------------------------------------------------------------------------
// Private Functions
// - The following functions are generally not required for
//   typical users of GUIslice. However, for advanced usage
//   more direct access may be required.
// ------------------------------------------------------------------------


///
/// Create a new element with default styling
///
/// \param[in]  pGui:         Pointer to GUI
/// \param[in]  nElemId:      User-supplied ID for referencing this element
///                           (or GSLC_ID_AUTO to auto-generate)
/// \param[in]  nPageId:      The page ID on which this page should be associated
/// \param[in]  nType:        Enumeration that indicates the type of element
///                           that is requested for creation. The type adjusts
///                           the visual representation and default styling.
/// \param[in]  rElem:        Rectangle region framing the element
/// \param[in]  pStr:         Pointer to string for textual elements
/// \param[in]  nFontId:      Font ID for textual elements
///
/// \return Initialized structure
///
gslc_tsElem gslc_ElemCreate(gslc_tsGui* pGui,int nElemId,int nPageId,gslc_teType nType,
  gslc_Rect rElem,const char* pStr,int nFontId);


///
/// Add the Element to the list of generated elements
/// in the GUI environment.
/// - NOTE: The content of pElem is copied so the pointer
///         can be released after the call.
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nPageId:     Page ID to add element to (GSLC_PAGE_NONE to skip in
///                          case of temporary creation for compound elements)
/// \param[in]  pElem:       Pointer to Element to add
///
/// \return Pointer to Element or NULL if fail
///
gslc_tsElem* gslc_ElemAdd(gslc_tsGui* pGui,int nPageId,gslc_tsElem* pElem);



///
/// Set an element to use a bitmap image
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElem:       Pointer to Element to update
/// \param[in]  acImage:     String of filename for image
/// \param[in]  acImageSel:  String of filename for image (selected)
///
/// \return none
///
void gslc_ElemSetImage(gslc_tsGui* pGui,gslc_tsElem* pElem,const char* acImage,
  const char* acImageSel);

///
/// Configure the background to use a bitmap image
/// - The background is used when redrawing the entire page
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pStrFname:   Filename to BMP file
///
/// \return true if success, false if fail
///
bool gslc_SetBkgndImage(gslc_tsGui* pGui,char* pStrFname);


///
/// Configure the background to use a solid color
/// - The background is used when redrawing the entire page
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nCol:        RGB Color to use
///
/// \return true if success, false if fail
///
bool gslc_SetBkgndColor(gslc_tsGui* pGui,gslc_Color nCol);

/// Draw an element to the active display
/// - Element is referenced by an element pointer
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElem:       Ptr to Element to draw
///
/// \return true if success, false otherwise
///
bool gslc_ElemDrawByRef(gslc_tsGui* pGui,gslc_tsElem* pElem);



///
/// Free up any surfaces associated with the GUI,
/// pages, collections and elements. Also frees
/// up any fonts.
/// - Called by gslc_Quit()
/// 
/// \param[in]  pGui:         Pointer to GUI
/// 
/// \return none
///
void gslc_GuiDestruct(gslc_tsGui* pGui);


///
/// Free up any members associated with a page
/// 
/// \param[in]  pPage:        Pointer to Page
/// 
/// \return none
///
void gslc_PageDestruct(gslc_tsPage* pPage);

///
/// Free up any members associated with an element collection
/// 
/// \param[in]  pCollect:     Pointer to collection
/// 
/// \return none
///
void gslc_CollectDestruct(gslc_tsCollect* pCollect);

///
/// Free up any members associated with an element
/// 
/// \param[in]  pElem:        Pointer to element
/// 
/// \return none
///
void gslc_ElemDestruct(gslc_tsElem* pElem);

///
/// Perform range check on Viewport Index
/// - Range checking is done before View array access
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nViewInd:    Viewport index
///
/// \return true if index is in range, false otherwise
///
bool gslc_ViewIndValid(gslc_tsGui* pGui,int nViewInd);

///
/// Look up the Viewport Index from the Viewport ID
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nViewId:     ID of the viewport to find
///
/// \return Index of viewport or GSLC_VIEW_IND_NONE if not found
///
int gslc_ViewFindIndFromId(gslc_tsGui* pGui,int nViewId);

///
/// Remap a coordinate from local to global using the
/// viewport settings. The input coordinates will be
/// replaced by the global coordinates.
///
/// \param[in]    pGui:        Pointer to GUI
/// \param[in]    nViewId:     ID of the viewport
/// \param[inout] pnX:         Ptr to X coordinate
/// \param[inout] pnY:         Ptr to Y coordinate
///
/// \return none
///
void gslc_ViewRemapPt(gslc_tsGui* pGui,int16_t* pnX,int16_t* pnY);

///
/// Remap a rectangle from local to global using the
/// viewport settings. The input rectangle coordinates will be
/// replaced by the global coordinates.
///
/// \param[in]    pGui:        Pointer to GUI
/// \param[in]    nViewId:     ID of the viewport
/// \param[inout] prRect:      Ptr to Rectangular coordinates to update
///
/// \return none
///
void gslc_ViewRemapRect(gslc_tsGui* pGui,gslc_Rect* prRect);


///
/// Trigger an element's touch event. This is an optional
/// behavior useful in some extended element types.
///
/// \param[in]  pGui:         Pointer to GUI
/// \param[in]  pElemTracked: Pointer to tracked Element (or NULL for none))
/// \param[in]  eTouch:       Touch event type
/// \param[in]  nX:           X coordinate of event (absolute coordinate)
/// \param[in]  nY:           Y coordinate of event (absolute coordinate)
///
/// \return true if success, false if error
///
bool gslc_ElemSendEventTouch(gslc_tsGui* pGui,gslc_tsElem* pElemTracked,
        gslc_teTouch eTouch,int nX,int nY);


///
/// Initialize a Font struct
///
/// \param[in]  pFont:       Pointer to Font
///
/// \return none
///
void gslc_ResetFont(gslc_tsFont* pFont);

///
/// Initialize an Element struct
///
/// \param[in]  pElem:       Pointer to Element
///
/// \return none
///
void gslc_ResetElem(gslc_tsElem* pElem);

///
/// Initialize a View struct
///
/// \param[in]  pView:       Pointer to Viewport
///
/// \return none
///
void gslc_ResetView(gslc_tsView* pView);




// =======================




#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _GUISLICE_H_
