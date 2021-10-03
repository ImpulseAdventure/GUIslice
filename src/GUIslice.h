#ifndef _GUISLICE_H_
#define _GUISLICE_H_

// =======================================================================
// GUIslice library
// - Calvin Hass
// - https://www.impulseadventure.com/elec/guislice-gui.html
// - https://github.com/ImpulseAdventure/GUIslice
//
// - Version 0.17.0
// =======================================================================
//
// The MIT License
//
// Copyright 2016-2021 Calvin Hass
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
/// \file GUIslice.h


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

// Import configuration ( which will import a sub-config depending on device type)
#include "GUIslice_config.h"

// Provide an alias for PROGMEM so that we can disable
// it on devices that don't use it when defining constant
// strings in FLASH memory.
#if (GSLC_USE_PROGMEM)
  #define GSLC_PMEM PROGMEM
#else
  #define GSLC_PMEM
#endif

// Provide default for FOCUS_ON_TOUCH
#if !defined(GSLC_FEATURE_FOCUS_ON_TOUCH)
  #define GSLC_FEATURE_FOCUS_ON_TOUCH 1
#endif


// -----------------------------------------------------------------------
// Globals
// -----------------------------------------------------------------------

// Type for user-provided debug output function
typedef int16_t (*GSLC_CB_DEBUG_OUT)(char ch);

/// Global debug output function
/// - The user assigns this function via gslc_InitDebug()
extern GSLC_CB_DEBUG_OUT g_pfDebugOut;


// -----------------------------------------------------------------------
// Constants
// -----------------------------------------------------------------------
#define GSLC_2PI  6.28318530718

// -----------------------------------------------------------------------
// Enumerations
// -----------------------------------------------------------------------

  /// Element ID enumerations
  /// - The Element ID is the primary means for user code to
  ///   reference a graphic element.
  /// - Application code can assign arbitrary Element ID values
  ///   in the range of 0...16383
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
    GSLC_ID_AUTO_BASE       = 16384,  ///< Starting Element ID to start auto-assignment
                                      ///< (when GSLC_ID_AUTO is specified)
  } gslc_teElemId;


  /// Page ID enumerations
  /// - The Page ID is the primary means for user code to
  ///   reference a specific page of elements.
  /// - Application code can assign arbitrary Page ID values
  ///   in the range of 0...16383
  /// - Negative Page ID values are reserved
  typedef enum {
    // Public usage
    GSLC_PAGE_USER_BASE     = 0,      ///< Starting Page ID for user assignments
    // Internal usage
    GSLC_PAGE_NONE          = -2999,  ///< No Page ID has been assigned
  } gslc_tePageId;

  /// Define page stack
  typedef enum {
    GSLC_STACK_BASE = 0,               ///< Base page
    GSLC_STACK_CUR,                    ///< Current page
    GSLC_STACK_OVERLAY,                ///< Overlay page (eg. popups)

    GSLC_STACK__MAX                    ///< Defines maximum number of pages in stack
  } gslc_teStackPage;

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
  ///   in the range of 0...16383
  /// - Negative Font ID values are reserved
  typedef enum {
    // Public usage
    GSLC_FONT_USER_BASE     = 0,      ///< Starting Font ID for user assignments
    GSLC_FONT_NONE          = -4999,  ///< No Font ID has been assigned
  } gslc_teFontId;


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

    /// Base value for extended type enumerations
    GSLC_TYPE_BASE_EXTEND = 0x1000
} gslc_teTypeCore;

/// Element features type
#define GSLC_ELEM_FEA_NOSHRINK  0x0100      ///< Element can't be shrunk (eg. contains image)
#define GSLC_ELEM_FEA_VALID     0x0080      ///< Element record is valid
#define GSLC_ELEM_FEA_FOCUS_EN  0x0040      ///< Element can accept focus
#define GSLC_ELEM_FEA_EDIT_EN   0x0020      ///< Element supports edit
#define GSLC_ELEM_FEA_ROUND_EN  0x0010      ///< Element is drawn with a rounded profile
#define GSLC_ELEM_FEA_CLICK_EN  0x0008      ///< Element accepts touch presses
#define GSLC_ELEM_FEA_GLOW_EN   0x0004      ///< Element supports glowing state
#define GSLC_ELEM_FEA_FRAME_EN  0x0002      ///< Element is drawn with a frame
#define GSLC_ELEM_FEA_FILL_EN   0x0001      ///< Element is drawn with a fill
#define GSLC_ELEM_FEA_NONE      0x0000      ///< Element default (no features set))


/// Element text alignment
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


/// Basic color definition
#define GSLC_COL_RED_DK4    (gslc_tsColor) {128,  0,  0}   ///< Red (dark4)
#define GSLC_COL_RED_DK3    (gslc_tsColor) {160,  0,  0}   ///< Red (dark3)
#define GSLC_COL_RED_DK2    (gslc_tsColor) {192,  0,  0}   ///< Red (dark2)
#define GSLC_COL_RED_DK1    (gslc_tsColor) {224,  0,  0}   ///< Red (dark1)
#define GSLC_COL_RED        (gslc_tsColor) {255,  0,  0}   ///< Red
#define GSLC_COL_RED_LT1    (gslc_tsColor) {255, 32, 32}   ///< Red (light1)
#define GSLC_COL_RED_LT2    (gslc_tsColor) {255, 64, 64}   ///< Red (light2)
#define GSLC_COL_RED_LT3    (gslc_tsColor) {255, 96, 96}   ///< Red (light3)
#define GSLC_COL_RED_LT4    (gslc_tsColor) {255,128,128}   ///< Red (light4)
#define GSLC_COL_GREEN_DK4  (gslc_tsColor) {  0,128,  0}   ///< Green (dark4)
#define GSLC_COL_GREEN_DK3  (gslc_tsColor) {  0,160,  0}   ///< Green (dark3)
#define GSLC_COL_GREEN_DK2  (gslc_tsColor) {  0,192,  0}   ///< Green (dark2)
#define GSLC_COL_GREEN_DK1  (gslc_tsColor) {  0,224,  0}   ///< Green (dark1)
#define GSLC_COL_GREEN      (gslc_tsColor) {  0,255,  0}   ///< Green
#define GSLC_COL_GREEN_LT1  (gslc_tsColor) { 32,255, 32}   ///< Green (light1)
#define GSLC_COL_GREEN_LT2  (gslc_tsColor) { 64,255, 64}   ///< Green (light2)
#define GSLC_COL_GREEN_LT3  (gslc_tsColor) { 96,255, 96}   ///< Green (light3)
#define GSLC_COL_GREEN_LT4  (gslc_tsColor) {128,255,128}   ///< Green (light4)
#define GSLC_COL_BLUE_DK4   (gslc_tsColor) {  0,  0,128}   ///< Blue (dark4)
#define GSLC_COL_BLUE_DK3   (gslc_tsColor) {  0,  0,160}   ///< Blue (dark3)
#define GSLC_COL_BLUE_DK2   (gslc_tsColor) {  0,  0,192}   ///< Blue (dark2)
#define GSLC_COL_BLUE_DK1   (gslc_tsColor) {  0,  0,224}   ///< Blue (dark1)
#define GSLC_COL_BLUE       (gslc_tsColor) {  0,  0,255}   ///< Blue
#define GSLC_COL_BLUE_LT1   (gslc_tsColor) { 32, 32,255}   ///< Blue (light1)
#define GSLC_COL_BLUE_LT2   (gslc_tsColor) { 64, 64,255}   ///< Blue (light2)
#define GSLC_COL_BLUE_LT3   (gslc_tsColor) { 96, 96,255}   ///< Blue (light3)
#define GSLC_COL_BLUE_LT4   (gslc_tsColor) {128,128,255}   ///< Blue (light4)
#define GSLC_COL_BLACK      (gslc_tsColor) {  0,  0,  0}   ///< Black
#define GSLC_COL_GRAY_DK4   (gslc_tsColor) { 16, 16, 16}   ///< Gray (dark4)
#define GSLC_COL_GRAY_DK3   (gslc_tsColor) { 32, 32, 32}   ///< Gray (dark3)
#define GSLC_COL_GRAY_DK2   (gslc_tsColor) { 64, 64, 64}   ///< Gray (dark2)
#define GSLC_COL_GRAY_DK1   (gslc_tsColor) { 96, 96, 96}   ///< Gray (dark1)
#define GSLC_COL_GRAY       (gslc_tsColor) {128,128,128}   ///< Gray
#define GSLC_COL_GRAY_LT1   (gslc_tsColor) {160,160,160}   ///< Gray (light1)
#define GSLC_COL_GRAY_LT2   (gslc_tsColor) {192,192,192}   ///< Gray (light2)
#define GSLC_COL_GRAY_LT3   (gslc_tsColor) {224,224,224}   ///< Gray (light3)
#define GSLC_COL_GRAY_LT4   (gslc_tsColor) {240,240,240}   ///< Gray (light4)
#define GSLC_COL_WHITE      (gslc_tsColor) {255,255,255}   ///< White

#define GSLC_COL_YELLOW     (gslc_tsColor) {255,255,0}     ///< Yellow
#define GSLC_COL_YELLOW_DK  (gslc_tsColor) {64,64,0}       ///< Yellow (dark)
#define GSLC_COL_PURPLE     (gslc_tsColor) {128,0,128}     ///< Purple
#define GSLC_COL_CYAN       (gslc_tsColor) {0,255,255}     ///< Cyan
#define GSLC_COL_MAGENTA    (gslc_tsColor) {255,0,255}     ///< Magenta
#define GSLC_COL_TEAL       (gslc_tsColor) {0,128,128}     ///< Teal
#define GSLC_COL_ORANGE     (gslc_tsColor) {255,165,0}     ///< Orange
#define GSLC_COL_BROWN      (gslc_tsColor) {165,42,42}     ///< Brown

// Monochrome definitions
#define GSLC_COLMONO_BLACK  (gslc_tsColor) {255,255,255}   ///< Black
#define GSLC_COLMONO_WHITE  (gslc_tsColor) {  0,  0,  0}   ///< White



/// Raw input event types: touch, key, GPIOs
typedef enum {
  GSLC_INPUT_NONE,          ///< No input event
  GSLC_INPUT_TOUCH,         ///< Touch / mouse event
  GSLC_INPUT_KEY_DOWN,      ///< Key press down / pin input asserted
  GSLC_INPUT_KEY_UP,        ///< Key press up (released)
  GSLC_INPUT_PIN_ASSERT,    ///< GPIO pin input asserted (eg. set to 1 / High)
  GSLC_INPUT_PIN_DEASSERT,  ///< GPIO pin input deasserted (eg. set to 0 / Low)
} gslc_teInputRawEvent;

/// External input mode. Dictates how directional controls
/// affect the interaction with the GUI elements.
typedef enum {
  GSLC_INPUTMODE_NAV,       ///< External input is in navigation mode
  GSLC_INPUTMODE_EDIT,      ///< External input is in element edit mode
} gslc_teInputMode;


/// GUI Action Requested
/// These actions are usually the result of an InputMap lookup
typedef enum {
  GSLC_ACTION_UNDEF,        ///< Invalid action
  GSLC_ACTION_NONE,         ///< No action to perform
  GSLC_ACTION_FOCUS_PREV,   ///< Advance focus to the previous GUI element
  GSLC_ACTION_FOCUS_NEXT,   ///< Advance focus to the next GUI element
  GSLC_ACTION_PRESELECT,    ///< Pre-Select the currently focused GUI element (glow)
  GSLC_ACTION_SELECT,       ///< Select the currently focused GUI element
  GSLC_ACTION_SET_REL,      ///< Adjust value (relative) of focused element  
  GSLC_ACTION_SET_ABS,      ///< Adjust value (absolute) of focused element
  GSLC_ACTION_DEBUG         ///< Internal debug action
} gslc_teAction;


/// General purpose pin/button constants
typedef enum {
  GSLC_PIN_BTN_A,           ///< Button A (short press)
  GSLC_PIN_BTN_A_LONG,      ///< Button A (long press)
  GSLC_PIN_BTN_B,           ///< Button B (short press)
  GSLC_PIN_BTN_B_LONG,      ///< Button B (long press)
  GSLC_PIN_BTN_C,           ///< Button C (short press)
  GSLC_PIN_BTN_C_LONG,      ///< Button C (long press)
  GSLC_PIN_BTN_D,           ///< Button D (short press)
  GSLC_PIN_BTN_D_LONG,      ///< Button D (long press)
  GSLC_PIN_BTN_E,           ///< Button E (short press)
  GSLC_PIN_BTN_E_LONG,      ///< Button E (long press)
  GSLC_PIN_BTN_UP,          ///< Button Up (short press)
  GSLC_PIN_BTN_DOWN,        ///< Button Down (short press)
  GSLC_PIN_BTN_LEFT,        ///< Button Left (short press)
  GSLC_PIN_BTN_RIGHT,       ///< Button Right (short press)
  GSLC_PIN_BTN_SEL,         ///< Button Select (short press)
} gslc_tePin;


/// Processed event from input raw events and actions
typedef enum {
  GSLC_TOUCH_NONE         = 0,          ///< No touch event active

  // Indexed state (ie. key/GPIO selects specific element vs touch coordinate)
  GSLC_TOUCH_TYPE_MASK    = (1<<7),     ///< Mask for type: coord/direct mode
  GSLC_TOUCH_COORD        = (0<<7),     ///< Event based on touch coordinate
  GSLC_TOUCH_DIRECT       = (1<<7),     ///< Event based on specific element index (keyboard/GPIO action)

  GSLC_TOUCH_SUBTYPE_MASK =                     (15<<0),    ///< Mask for subtype

  // Coordinate-based events
  GSLC_TOUCH_DOWN            = GSLC_TOUCH_COORD  | ( 1<<0),    ///< Touch event (down)
  GSLC_TOUCH_DOWN_IN         = GSLC_TOUCH_COORD  | ( 2<<0),    ///< Touch event (down inside tracked element)
  GSLC_TOUCH_DOWN_OUT        = GSLC_TOUCH_COORD  | ( 3<<0),    ///< Touch event (down outside tracked element)
  GSLC_TOUCH_UP              = GSLC_TOUCH_COORD  | ( 4<<0),    ///< Touch event (up)
  GSLC_TOUCH_UP_IN           = GSLC_TOUCH_COORD  | ( 5<<0),    ///< Touch event (up inside tracked element)
  GSLC_TOUCH_UP_OUT          = GSLC_TOUCH_COORD  | ( 6<<0),    ///< Touch event (up outside tracked element)
  GSLC_TOUCH_MOVE            = GSLC_TOUCH_COORD  | ( 7<<0),    ///< Touch event (move)
  GSLC_TOUCH_MOVE_IN         = GSLC_TOUCH_COORD  | ( 8<<0),    ///< Touch event (move inside tracked element)
  GSLC_TOUCH_MOVE_OUT        = GSLC_TOUCH_COORD  | ( 9<<0),    ///< Touch event (move outside tracked element)
  // Index-based events
  GSLC_TOUCH_FOCUS_ON        = GSLC_TOUCH_DIRECT | ( 1<<0),    ///< Direct event focus on element
  GSLC_TOUCH_FOCUS_OFF       = GSLC_TOUCH_DIRECT | ( 2<<0),    ///< Direct event focus away from focused element
  GSLC_TOUCH_FOCUS_PRESELECT = GSLC_TOUCH_DIRECT | ( 3<<0),    ///< Direct event select focus element (glow before select)
  GSLC_TOUCH_FOCUS_SELECT    = GSLC_TOUCH_DIRECT | ( 4<<0),    ///< Direct event select focus element
  GSLC_TOUCH_SET_REL         = GSLC_TOUCH_DIRECT | ( 5<<0),    ///< Direct event set value (relative) on focus element
  GSLC_TOUCH_SET_ABS         = GSLC_TOUCH_DIRECT | ( 6<<0),    ///< Direct event set value (absolute) on focus element

} gslc_teTouch;

/// Status of a module's initialization
typedef enum {
  GSLC_INITSTAT_UNDEF = 0,       ///< Module status has not been defined yet
  GSLC_INITSTAT_INACTIVE,        ///< Module is not enabled
  GSLC_INITSTAT_FAIL,            ///< Module is enabled but failed to init
  GSLC_INITSTAT_ACTIVE           ///< Module is enabled and initalized OK
} gslc_teInitStat;



/// Additional definitions for Touch Handling
/// These macros define the transforms used in remapping the touchscreen
/// inputs on the basis of the GUI nRotation setting.
  #define TOUCH_ROTATION_DATA 0x6350
  #define TOUCH_ROTATION_SWAPXY(rotation) ((( TOUCH_ROTATION_DATA >> ((rotation&0x03)*4) ) >> 2 ) & 0x01 )
  #define TOUCH_ROTATION_FLIPX(rotation)  ((( TOUCH_ROTATION_DATA >> ((rotation&0x03)*4) ) >> 1 ) & 0x01 )
  #define TOUCH_ROTATION_FLIPY(rotation)  ((( TOUCH_ROTATION_DATA >> ((rotation&0x03)*4) ) >> 0 ) & 0x01 )


/// Event types
typedef enum {
  GSLC_EVT_NONE,          ///< No event; ignore
  GSLC_EVT_DRAW,          ///< Perform redraw
  GSLC_EVT_TOUCH,         ///< Track touch event
  GSLC_EVT_TICK,          ///< Perform background tick handling
  GSLV_EVT_CUSTOM         ///< Custom event
} gslc_teEventType;

/// Event sub-types
typedef enum {
  GSLC_EVTSUB_NONE,
  GSLC_EVTSUB_DRAW_NEEDED,  ///< Incremental redraw (as needed)
  GSLC_EVTSUB_DRAW_FORCE    ///< Force a full redraw
} gslc_teEventSubType;

/// Redraw types
typedef enum {
  GSLC_REDRAW_NONE,         ///< No redraw requested
  GSLC_REDRAW_FULL,         ///< Full redraw of element requested
  GSLC_REDRAW_INC,          ///< Incremental redraw of element requested
  GSLC_REDRAW_FOCUS         ///< Only focus redraw requested
} gslc_teRedrawType;


/// Font Reference types
/// - The Font Reference type defines the way in which a
///   font is selected. In some device targets (such as
///   LINUX SDL) a filename to a font file is provided. In
///   others (such as Arduino, ESP8266), a pointer is given to a
///   font structure (or NULL for default).
typedef enum {
  GSLC_FONTREF_FNAME,      ///< Font reference is a filename (full path)
  GSLC_FONTREF_PTR         ///< Font reference is a pointer to a font structure
} gslc_teFontRefType;


/// Font Reference modes
/// - The Font Reference mode defines the source for the
///   selected font. For graphics libraries that offer
///   multiple types of fonts, this can be used to differentiate
///   between a default font, hardware fonts, software fonts, etc.
/// - The encoding between the different modes is driver-specific.
typedef enum {
  GSLC_FONTREF_MODE_DEFAULT, ///< Default font mode
  GSLC_FONTREF_MODE_1,       ///< Font mode 1
  GSLC_FONTREF_MODE_2,       ///< Font mode 2
  GSLC_FONTREF_MODE_3        ///< Font mode 3
} gslc_teFontRefMode;


/// Element reference flags: Describes characteristics of an element
/// - Primarily used to support relocation of elements to Flash memory (PROGMEM)
typedef enum {
  GSLC_ELEMREF_NONE        = 0,       ///< No element defined
  // Define element sources
  GSLC_ELEMREF_SRC_RAM     = (1<<0),  ///< Element is read/write
                                      ///< Stored in RAM (internal element array))
                                      ///< Access directly
  GSLC_ELEMREF_SRC_PROG    = (2<<0),  ///< Element is read-only / const
                                      ///< Stored in FLASH (external to element array)
                                      ///< Access via PROGMEM
  GSLC_ELEMREF_SRC_CONST   = (3<<0),  ///< Element is read-only / const
                                      ///< Stored in FLASH (external to element array)
                                      ///< Access directly
  // Element State
  GSLC_ELEMREF_REDRAW_NONE = (0<<4),  ///< No redraw requested
  GSLC_ELEMREF_REDRAW_FULL = (1<<4),  ///< Full redraw of element requested
  GSLC_ELEMREF_REDRAW_INC  = (2<<4),  ///< Incremental redraw of element requested
  GSLC_ELEMREF_REDRAW_FOCUS= (3<<4),  ///< Focus-only redraw of element requested

  GSLC_ELEMREF_EDITING     = (1<<2),  ///< Element is in edit state (1=edit, 0=navigate)
  GSLC_ELEMREF_FOCUSED     = (1<<3),  ///< Element state is focused
  GSLC_ELEMREF_GLOWING     = (1<<6),  ///< Element state is glowing
  GSLC_ELEMREF_VISIBLE     = (1<<7),  ///< Element is currently shown (ie. visible)

  // Mask values for bitfield comparisons
  GSLC_ELEMREF_SRC            = (3<<0),   ///< Mask for Source flags [bits 1,0]
  GSLC_ELEMREF_REDRAW_MASK    = (3<<4),   ///< Mask for Redraw flags [bits 5,4]

} gslc_teElemRefFlags;

/// Define the default element reference flags for new elements
#define GSLC_ELEMREF_DEFAULT  (GSLC_ELEMREF_SRC_RAM | GSLC_ELEMREF_VISIBLE)


/// Image reference flags: Describes characteristics of an image reference
typedef enum {
  GSLC_IMGREF_NONE        = 0,        ///< No image defined
  // Define image sources
  GSLC_IMGREF_SRC_FILE    = (1<<0),   ///< Image is stored in file system
  GSLC_IMGREF_SRC_SD      = (2<<0),   ///< Image is stored on SD card
  GSLC_IMGREF_SRC_RAM     = (3<<0),   ///< Image is stored in RAM
  GSLC_IMGREF_SRC_PROG    = (4<<0),   ///< Image is stored in program memory (PROGMEM)
  // Define image types

  GSLC_IMGREF_FMT_BMP24   = (1<<4),   ///< Image format is BMP (24-bit)
  GSLC_IMGREF_FMT_BMP16   = (2<<4),   ///< Image format is BMP (16-bit RGB565)
  GSLC_IMGREF_FMT_RAW1    = (3<<4),   ///< Image format is raw monochrome (1-bit)
  GSLC_IMGREF_FMT_JPG     = (4<<4),   ///< Image format is JPG (ESP32/ESP8366)

  // Mask values for bitfield comparisons
  GSLC_IMGREF_SRC         = (7<<0),   ///< Mask for Source flags
  GSLC_IMGREF_FMT         = (7<<4),   ///< Mask for Format flags
} gslc_teImgRefFlags;

/// Text reference flags: Describes the characteristics of a text string
/// (ie. whether internal to element or external and RAM vs Flash).)
///
/// Supported flag combinations are:
/// - ALLOC_NONE
/// - ALLOC_INT | MEM_RAM
/// - ALLOC_EXT | MEM_RAM
/// - ALLOC_EXT | MEM_PROG
typedef enum {
  // Values
  GSLC_TXT_MEM_RAM        = (0<<0),   ///< Text string is in SRAM    (read-write)
  GSLC_TXT_MEM_PROG       = (1<<0),   ///< Text string is in PROGMEM (read-only)
  GSLC_TXT_ALLOC_NONE     = (0<<2),   ///< No text string present
  GSLC_TXT_ALLOC_INT      = (1<<2),   ///< Text string allocated in internal element memory (GSLC_STR_LOCAL=1)
  GSLC_TXT_ALLOC_EXT      = (2<<2),   ///< Text string allocated in external memory (GSLC_STR_LOCAL=0), ie. user code
  GSLC_TXT_ENC_PLAIN      = (0<<4),   ///< Encoding is plain text (LATIN1))
  GSLC_TXT_ENC_UTF8       = (1<<4),   ///< Encoding is UTF-8

  // Masks
  GSLC_TXT_MEM            = (3<<0),   ///< Mask for updating text memory type
  GSLC_TXT_ALLOC          = (3<<2),   ///< Mask for updating location of text string buffer allocation
  GSLC_TXT_ENC            = (3<<4),   ///< Mask for updating text encoding
  // Defaults
  GSLC_TXT_DEFAULT        = GSLC_TXT_MEM_RAM | GSLC_TXT_ALLOC_NONE | GSLC_TXT_ENC_PLAIN,
} gslc_teTxtFlags;


// -----------------------------------------------------------------------
// Forward declarations
// -----------------------------------------------------------------------

typedef struct gslc_tsElem  gslc_tsElem;
typedef struct gslc_tsEvent gslc_tsEvent;


// -----------------------------------------------------------------------
// Callback functions
// -----------------------------------------------------------------------


/// Callback function for element drawing
typedef bool (*GSLC_CB_EVENT)(void* pvGui,gslc_tsEvent sEvent);

/// Callback function for element drawing
typedef bool (*GSLC_CB_DRAW)(void* pvGui,void* pvElemRef,gslc_teRedrawType eRedraw);

/// Callback function for element touch tracking
typedef bool (*GSLC_CB_TOUCH)(void* pvGui,void* pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY);

/// Callback function for element tick
typedef bool (*GSLC_CB_TICK)(void* pvGui,void* pvElemRef);

/// Callback function for pin polling
typedef bool (*GSLC_CB_PIN_POLL)(void* pvGui,int16_t* pnPinInd,int16_t* pnPinVal);

/// Callback function for element input ready
typedef bool (*GSLC_CB_INPUT)(void* pvGui,void* pvElemRef,int16_t nStatus,void* pvData);

// -----------------------------------------------------------------------
// Structures
// -----------------------------------------------------------------------


/// Rectangular region. Defines X,Y corner coordinates plus dimensions.
typedef struct gslc_tsRect {
  int16_t   x;    ///< X coordinate of corner
  int16_t   y;    ///< Y coordinate of corner
  uint16_t  w;    ///< Width of region
  uint16_t  h;    ///< Height of region
} gslc_tsRect;


/// Define point coordinates
typedef struct gslc_tsPt {
  int16_t   x;        ///< X coordinate
  int16_t   y;        ///< Y coordinate
} gslc_tsPt;

/// Color structure. Defines RGB triplet.
typedef struct gslc_tsColor {
  uint8_t r;      ///< RGB red value
  uint8_t g;      ///< RGB green value
  uint8_t b;      ///< RGB blue value
} gslc_tsColor;

/// State associated with an element's region
/// - This struct is used for gslc_ElemCalcRectState()
/// - Accounts for various rects including
///   focus, frame and internal content
/// - Also contains the various colors associated
///   with each region.
typedef struct gslc_tsRectState {
  gslc_tsRect rFocus;
  gslc_tsRect rFull;
  gslc_tsRect rInner;
  gslc_tsColor colFocus;
  gslc_tsColor colFrm;
  gslc_tsColor colInner;
  gslc_tsColor colBack;
  gslc_tsColor colTxtFore;
  gslc_tsColor colTxtBack;
} gslc_tsRectState;

/// Event structure
typedef struct gslc_tsEvent {
  gslc_teEventType  eType;    ///< Event type
  uint8_t           nSubType; ///< Event sub-type
  void*             pvScope;  ///< Event target scope (eg. Page,Collection,Event)
  void*             pvData;   ///< Generic data pointer for event. This member is
                              ///< used to either pass a pointer to a simple data
                              ///< datatype (such as Element or Collection) or
                              ///< to a another structure that contains multiple fields.
} gslc_tsEvent;

/// Structure used to pass touch data through event
typedef struct gslc_tsEventTouch {
  gslc_teTouch      eTouch;           ///< Touch state
  int16_t           nX;               ///< Touch X coordinate (or param1)
  int16_t           nY;               ///< Touch Y coordinate (or param2)
} gslc_tsEventTouch;

/// Font reference structure
typedef struct {
  int16_t               nId;            ///< Font ID specified by user
  gslc_teFontRefType    eFontRefType;   ///< Font reference type
  gslc_teFontRefMode    eFontRefMode;   ///< Font reference mode
  const void*           pvFont;         ///< Void ptr to the font reference (type defined by driver)
  uint16_t              nSize;          ///< Font size
} gslc_tsFont;


/// Image reference structure
typedef struct {
  const unsigned char*  pImgBuf;    ///< Pointer to input image buffer in memory [RAM,FLASH]
  const char*           pFname;     ///< Pathname to input image file [FILE,SD]
  gslc_teImgRefFlags    eImgFlags;  ///< Image reference flags
  void*                 pvImgRaw;   ///< Ptr to raw output image data (for pre-loaded images)
} gslc_tsImgRef;


/// Element reference structure
typedef struct {
  gslc_tsElem*          pElem;      ///< Pointer to element in memory [RAM,FLASH]
  gslc_teElemRefFlags   eElemFlags; ///< Element reference flags
} gslc_tsElemRef;

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

  int16_t             nId;              ///< Element ID specified by user
  uint16_t            nFeatures;        ///< Element feature vector (appearance/behavior))

  int16_t             nType;            ///< Element type enumeration
  gslc_tsRect         rElem;            ///< Rect region containing element
  int16_t             nGroup;           ///< Group ID that the element belongs to

  gslc_tsColor        colElemFrame;     ///< Color for frame
  gslc_tsColor        colElemFill;      ///< Color for background fill
  gslc_tsColor        colElemFrameGlow; ///< Color to use for frame when glowing
  gslc_tsColor        colElemFillGlow;  ///< Color to use for fill when glowing

  gslc_tsImgRef       sImgRefNorm;      ///< Image reference to draw (normal)
  gslc_tsImgRef       sImgRefGlow;      ///< Image reference to draw (glowing)

  // if (GSLC_FEATURE_COMPOUND)
  /// Parent element reference. Used during redraw
  /// to notify parent elements that they require
  /// redraw as well. Primary usage is in compound
  /// elements.
  /// NOTE: Although this field is only used in GLSC_COMPOUND mode, it
  ///       is not wrapped in an ifdef because the ElemCreate*_P()
  ///       function macros currently initialize this field.
  gslc_tsElemRef*     pElemRefParent;
  // endif

  // Text handling
#if (GSLC_LOCAL_STR)
  char                pStrBuf[GSLC_LOCAL_STR_LEN];  ///< Text string to overlay
#else
  char*               pStrBuf;          ///< Ptr to text string buffer to overlay
#endif
  uint8_t             nStrBufMax;       ///< Size of string buffer
  gslc_teTxtFlags     eTxtFlags;        ///< Flags associated with text buffer


  gslc_tsColor        colElemText;      ///< Color of overlay text
  gslc_tsColor        colElemTextGlow;  ///< Color of overlay text when glowing
  int8_t              eTxtAlign;        ///< Alignment of overlay text
  int8_t              nTxtMarginX;      ///< Margin of overlay text within rect region (x offset)
  int8_t              nTxtMarginY;      ///< Margin of overlay text within rect region (y offset)
  gslc_tsFont*        pTxtFont;         ///< Ptr to Font for overlay text

  // Extended data elements
  void*               pXData;           ///< Ptr to extended data structure

  // Callback functions
  GSLC_CB_EVENT       pfuncXEvent;      ///< UNUSED: Callback func ptr for event tree (draw,touch,tick)

  GSLC_CB_DRAW        pfuncXDraw;       ///< Callback func ptr for custom drawing
  GSLC_CB_TOUCH       pfuncXTouch;      ///< Callback func ptr for touch
  GSLC_CB_TICK        pfuncXTick;       ///< Callback func ptr for timer/main loop tick
} gslc_tsElem;




/// Element collection struct
/// - Collections are used to maintain a list of elements and any
///   touch tracking status.
/// - Pages and Compound Elements both instantiate a Collection
typedef struct {
  gslc_tsElem*          asElem;           ///< Array of elements
  uint16_t              nElemMax;         ///< Maximum number of elements to allocate (in RAM)
  uint16_t              nElemCnt;         ///< Number of elements allocated
  int16_t               nElemAutoIdNext;  ///< Next Element ID for auto-assignment

  gslc_tsElemRef*       asElemRef;        ///< Array of element references
  uint16_t              nElemRefMax;      ///< Maximum number of element references to allocate
  uint16_t              nElemRefCnt;      ///< Number of element references allocated

  //uint8_t             nPageInd;         ///< Page index in page stack

  // Touch tracking
  gslc_tsElemRef*       pElemRefTracked;  ///< Element reference currently being touch-tracked (NULL for none)
  int16_t               nElemIndTracked;  ///< Element index currently being touch-tracked (GSLC_IND_NONE for none)

  // Callback functions
  //GSLC_CB_EVENT         pfuncXEvent;      ///< UNUSED: Callback func ptr for events

} gslc_tsCollect;




/// Page structure
/// - A page contains a collection of elements
/// - Many redraw functions operate at a page level
/// - Maintains state as to whether redraw or screen flip is required
typedef struct {

  gslc_tsCollect      sCollect;             ///< Collection of elements on page

  int16_t             nPageId;              ///< Page identifier

  // Callback functions
  //GSLC_CB_EVENT       pfuncXEvent;          ///< UNUSED: Callback func ptr for events

  // Bounding region
  gslc_tsRect         rBounds;              ///< Bounding rect for page elements

} gslc_tsPage;


/// Input mapping
/// - Describes mapping from keyboard or GPIO input
///   to a GUI action (such as changing the current
///   element focus)
/// - This is generally used to support keyboard or GPIO
///   control over the GUI operation
typedef struct {
  gslc_teInputRawEvent  eEvent;               ///< The input event
  int16_t               nVal;                 ///< The value associated with the input event
  gslc_teAction         eAction;              ///< Resulting action
  int16_t               nActionVal;           ///< The value for the output action
} gslc_tsInputMap;


/// GUI structure
/// - Contains all GUI state and content
/// - Maintains list of one or more pages
typedef struct {

  uint16_t            nDispW;           ///< Width of the display (pixels)
  uint16_t            nDispH;           ///< Height of the display (pixels)
  uint16_t            nDisp0W;          ///< Width of the display (pixels) in native orientation
  uint16_t            nDisp0H;          ///< Height of the display (pixels) in native orientation
  uint8_t             nDispDepth;       ///< Bit depth of display (bits per pixel)

  // NOTE: The following config is currently only used by the Adafruit-GFX & TFT_eSPI drivers
  uint8_t             nRotation;       ///< Adafruit GFX Rotation of display
  #if !defined(DRV_TOUCH_NONE)
    // Touch remapping
    uint8_t           nTouchRotation;  ///< Touchscreen rotation offset vs display 
    uint8_t           nSwapXY;         ///< Adafruit GFX Touch Swap x and y axes
    uint8_t           nFlipX;          ///< Adafruit GFX Touch Flip x axis
    uint8_t           nFlipY;          ///< Adafruit GFX Touch Flip x axis
    // Calibration for resistive touch displays
    int16_t           nTouchCalXMin;   ///< Calibration X minimum reading
    int16_t           nTouchCalXMax;   ///< Calibration X maximum reading
    int16_t           nTouchCalYMin;   ///< Calibration Y minimum reading
    int16_t           nTouchCalYMax;   ///< Calibration Y maximum reading
    int16_t           nTouchCalPressMin;  ///< Calibration minimum pressure threshold
    int16_t           nTouchCalPressMax;  ///< Calibration maximum pressure threshold
  #endif

  gslc_tsFont*        asFont;           ///< Collection of loaded fonts
  uint8_t             nFontMax;         ///< Maximum number of fonts to allocate
  uint8_t             nFontCnt;         ///< Number of fonts allocated

  uint8_t             nRoundRadius;     ///< Radius for rounded elements

  gslc_tsColor        sTransCol;        ///< Color used for transparent image regions (GSLC_BMP_TRANS_EN=1)

#if (GSLC_FEATURE_COMPOUND)
  gslc_tsElem         sElemTmp;         ///< Temporary element
  gslc_tsElemRef      sElemRefTmp;      ///< Temporary element reference
#endif

  gslc_tsElem         sElemTmpProg;     ///< Temporary element for Flash compatibility

  gslc_teInitStat     eInitStatTouch;   ///< Status of touch initialization
  // TODO: exclude these in DRV_TOUCH_NONE
  int16_t             nTouchLastX;      ///< Last touch event X coord
  int16_t             nTouchLastY;      ///< Last touch event Y coord
  uint16_t            nTouchLastPress;  ///< Last touch event pressure (0=none))
  bool                bTouchRemapEn;    ///< Enable touch remapping?
  bool                bTouchRemapYX;    ///< Enable touch controller swapping of X & Y


  void*               pvDriver;         ///< Driver-specific members (gslc_tsDriver*)
  bool                bRedrawNeeded;    ///< Does anything on page require redraw?
  bool                bRedrawPartialEn; ///< Driver supports partial page redraw.
                                        ///< If true, only changed elements are redrawn
                                        ///< during next page redraw command.
                                        ///< If false, entire page is redrawn when any
                                        ///< element has been updated prior to next
                                        ///< page redraw command.

  // Pending events
  bool                bEventPending;    ///< Is there an event pending?
  gslc_tsEventTouch   sEventTouchPend;  ///< A touch event that has been deferred (if bEventPending=true)
  gslc_tsEvent        sEventPend;       ///< An event that has been deferred (if bEventPending=true)

  // Primary surface definitions
  gslc_tsImgRef       sImgRefBkgnd;     ///< Image reference for background

  uint8_t             nFrameRateCnt;    ///< Diagnostic frame rate count
  uint8_t             nFrameRateStart;  ///< Diagnostic frame rate timestamp


  // Pages
  gslc_tsPage*        asPage;           ///< Array of all pages defined in system
  uint8_t             nPageMax;         ///< Maximum number of pages that can be defined
  uint8_t             nPageCnt;         ///< Current number of pages defined

  gslc_tsPage*        apPageStack[GSLC_STACK__MAX];       ///< Stack of pages
  bool                abPageStackActive[GSLC_STACK__MAX]; ///< Whether page in stack can receive touch events
  bool                abPageStackDoDraw[GSLC_STACK__MAX]; ///< Whether page in stack is still actively drawn

  // Redraw of screen (ie. across page stack)
  bool                bScreenNeedRedraw; ///< Screen requires a redraw
  bool                bScreenNeedFlip;   ///< Screen requires a page flip

  // Current clip region
  bool                bInvalidateEn;     ///< A region of the display has been invalidated
  gslc_tsRect         rInvalidateRect;   ///< The rect region that has been invalidated

  // Callback functions
  //GSLC_CB_EVENT       pfuncXEvent;      ///< UNUSED: Callback func ptr for events
  GSLC_CB_PIN_POLL    pfuncPinPoll;     ///< Callback func ptr for pin polling


  // Key/pin input control mapping
  gslc_tsInputMap*    asInputMap;       ///< Array of input maps
  uint8_t             nInputMapMax;     ///< Maximum number of input maps
  uint8_t             nInputMapCnt;     ///< Current number of input maps
  uint8_t             nInputMode;       ///< Input mode: 0=navigate, 1=edit

  #if (GSLC_FEATURE_INPUT)
  // Input navigation / focus state
  int16_t             nFocusPageInd;    ///< Index of page in stack currently in focus
  gslc_tsPage*        pFocusPage;       ///< Page ptr currently in focus
  gslc_tsElemRef*     pFocusElemRef;    ///< Reference to element in focus
  int16_t             nFocusElemInd;    ///< Index of element in page currently in focus
  int16_t             nFocusElemMax;    ///< Max number of elements in page in focus
  gslc_tsColor        colFocusNone;     ///< Focus frame color when not in focus (typically background color)
  gslc_tsColor        colFocus;         ///< Focus frame color when in focus
  gslc_tsColor        colFocusEdit;     ///< Focus frame color when in focus and edit mode

  // Saved focus state (for popups)
  int16_t             nFocusSavedPageInd; ///< Focus page index saved prior to popup/overlay
  int16_t             nFocusSavedElemInd; ///< Focus element index saved prior to popup/overlay
  #endif // GSLC_FEATURE_INPUT

} gslc_tsGui;


#define GSLC_MIN(a,b) (a<b)?a:b
#define GSLC_MAX(a,b) (a>b)?a:b

// ------------------------------------------------------------------------
/// \defgroup _GeneralFunc_ General Functions
/// General functions for configuring the GUI
/// @{
// ------------------------------------------------------------------------

///
/// Get the GUIslice version number
///
/// \param[in]  pGui:      Pointer to GUI
///
/// \return String containing version number
///
char* gslc_GetVer(gslc_tsGui* pGui);

///
/// Get the GUIslice display driver name
///
/// \param[in]  pGui:      Pointer to GUI
///
/// \return String containing driver name
///
const char* gslc_GetNameDisp(gslc_tsGui* pGui);

///
/// Get the GUIslice touch driver name
///
/// \param[in]  pGui:      Pointer to GUI
///
/// \return String containing driver name
///
const char* gslc_GetNameTouch(gslc_tsGui* pGui);

///
/// Get the native display driver instance
/// - This can be useful to access special commands
///   available in the selected driver.
///
/// \param[in]  pGui:      Pointer to GUI
///
/// \return Void pointer to the display driver instance.
///         This pointer should be typecast to the particular
///         driver being used. If no driver was created then
///         this function will return NULL.
///
void* gslc_GetDriverDisp(gslc_tsGui* pGui);

///
/// Get the native touch driver instance
/// - This can be useful to access special commands
///   available in the selected driver.
///
/// \param[in]  pGui:      Pointer to GUI
///
/// \return Void pointer to the touch driver instance.
///         This pointer should be typecast to the particular
///         driver being used. If no driver was created then
///         this function will return NULL.
///
void* gslc_GetDriverTouch(gslc_tsGui* pGui);

///
/// Initialize the GUIslice library
/// - Configures the primary screen surface(s)
/// - Initializes font support
///
/// PRE:
/// - The environment variables should be configured before
///   calling gslc_Init().
///
/// \param[in]  pGui:      Pointer to GUI
/// \param[in]  pvDriver:  Void pointer to Driver struct (gslc_tsDriver*)
/// \param[in]  asPage:    Pointer to Page array
/// \param[in]  nMaxPage:  Size of Page array
/// \param[in]  asFont:    Pointer to Font array
/// \param[in]  nMaxFont:  Size of Font array
///
/// \return true if success, false if fail
///
bool gslc_Init(gslc_tsGui* pGui,void* pvDriver,gslc_tsPage* asPage,uint8_t nMaxPage,gslc_tsFont* asFont,uint8_t nMaxFont);


///
/// Initialize debug output
/// - Defines the user function used for debug/error output
/// - pfunc is responsible for outputing a single character
/// - For Arduino, this user function would typically call Serial.print()
///
/// \param[in]  pfunc:     Pointer to user character-out function
///
/// \return none
///
void gslc_InitDebug(GSLC_CB_DEBUG_OUT pfunc);


///
/// Optimized printf routine for GUIslice debug/error output
/// - Only supports '%s','%d','%u' tokens
/// - Calls on the output function configured in gslc_InitDebug()
///
/// \param[in]  pFmt:      Format string to use for printing
/// \param[in]  ...:       Variable parameter list
///
/// \return none
void gslc_DebugPrintf(const char* pFmt, ...);


///
/// Dynamically change rotation, automatically adapt touchscreen axes swap/flip
///
/// The function assumes that the touchscreen settings for swap and flip
/// in the GUIslice config are valid for the configured GSLC_ROTATE.
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nRotation:   Screen Rotation value (0, 1, 2 or 3)
///
/// \return true if success, false otherwise
///
bool gslc_GuiRotate(gslc_tsGui* pGui, uint8_t nRotation);


///
/// Exit the GUIslice environment
/// - Calls lower-level destructors to clean up any initialized subsystems
///   and deletes any created elements or fonts
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
/// Configure the background to use a bitmap image
/// - The background is used when redrawing the entire page
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  sImgRef:     Image reference
///
/// \return true if success, false if fail
///
bool gslc_SetBkgndImage(gslc_tsGui* pGui,gslc_tsImgRef sImgRef);

///
/// Configure the background to use a solid color
/// - The background is used when redrawing the entire page
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nCol:        RGB Color to use
///
/// \return true if success, false if fail
///
bool gslc_SetBkgndColor(gslc_tsGui* pGui,gslc_tsColor nCol);

///
/// Configure the color to use for image transparency
/// - Drawing a BMP with transparency enabled will cause
///   regions in this specific color to appear transparent
/// - This API overrides the config option GSLC_BMP_TRANS_RGB
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nCol:        RGB Color to use
///
/// \return true if success, false if fail
///
bool gslc_SetTransparentColor(gslc_tsGui* pGui,gslc_tsColor nCol);

///
/// Get the current the clipping rectangle
///
/// \param[in]  pGui:        Pointer to GUI
///
/// \return rect for active clipping region
///
gslc_tsRect gslc_GetClipRect(gslc_tsGui* pGui);

///
/// Set the clipping rectangle for further drawing
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pRect:       Pointer to Rect for clipping (or NULL for entire screen)
///
/// \return true if success, false if error
///
bool gslc_SetClipRect(gslc_tsGui* pGui,gslc_tsRect* pRect);



// ------------------------------------------------------------------------
/// @}
/// \defgroup _GraphicsGenFunc_ Graphics General Functions
/// Helper functions that support graphics operations
/// @{
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
bool gslc_IsInRect(int16_t nSelX,int16_t nSelY,gslc_tsRect rRect);


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
/// \return gslc_tsRect() with resized dimensions
///
gslc_tsRect gslc_ExpandRect(gslc_tsRect rRect,int16_t nExpandW,int16_t nExpandH);


///
/// Determine if a coordinate is inside of a width x height region.
/// - This routine is useful in determining if a relative coordinate
///   is within a given W x H dimension
///
/// \param[in]  nSelX:       X coordinate to test
/// \param[in]  nSelY:       X coordinate to test
/// \param[in]  nWidth:      Width to test against
/// \param[in]  nHeight:     Height to test against
///
/// \return true if inside region, false otherwise
///
bool gslc_IsInWH(int16_t nSelX,int16_t nSelY,uint16_t nWidth,uint16_t nHeight);

///
/// Expand a rect to include another rect
/// - This routine can be useful to modify an invalidation region to
///   include another modified element
///
/// \param[in]  pRect:    Initial rect region
/// \param[in]  rAddRect: Rectangle to add to the rect region
///
/// \return none
///
void gslc_UnionRect(gslc_tsRect* pRect, gslc_tsRect rAddRect);

///
/// Reset the invalidation region
///
/// \param[in]  pGui:        Pointer to GUI
///
/// \return none
///
void gslc_InvalidateRgnReset(gslc_tsGui* pGui);

///
/// Include an entire page (eg. from a page stack) in the invalidation region
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pPage:       Pointer to page
///
/// \return none
///
void gslc_InvalidateRgnPage(gslc_tsGui* pGui, gslc_tsPage* pPage);

///
/// Mark the entire screen as invalidated
///
/// \param[in]  pGui:   Pointer to GUI
///
/// \return none
///
void gslc_InvalidateRgnScreen(gslc_tsGui* pGui);

///
/// Add a rectangular region to the invalidation region
/// - This is usually called when an element has been modified
///
/// \param[in]  pGui:     Pointer to GUI
/// \param[in]  rAddRect: Rectangle to add to the invalidation region
///
/// \return none
///
void gslc_InvalidateRgnAdd(gslc_tsGui* pGui, gslc_tsRect rAddRect);

///
/// Perform basic clipping of a single point to a clipping region
///
/// \param[in]  pClipRect:   Pointer to clipping region
/// \param[in]  nX:          X coordinate of point
/// \param[in]  nY:          Y coordinate of point
///
/// \return true if point is visible, false if it should be discarded
///
bool gslc_ClipPt(gslc_tsRect* pClipRect,int16_t nX,int16_t nY);

///
/// Perform basic clipping of a line to a clipping region
/// - Implements Cohen-Sutherland algorithm
/// - Coordinates in parameter list are modified to fit the region
///
/// \param[in]    pClipRect:   Pointer to clipping region
/// \param[inout] pnX0:        Ptr to X coordinate of line start
/// \param[inout] pnY0:        Ptr to Y coordinate of line start
/// \param[inout] pnX1:        Ptr to X coordinate of line end
/// \param[inout] pnY1:        Ptr to Y coordinate of line end
///
/// \return true if line is visible, false if it should be discarded
///
bool gslc_ClipLine(gslc_tsRect* pClipRect,int16_t* pnX0,int16_t* pnY0,int16_t* pnX1,int16_t* pnY1);

///
/// Perform basic clipping of a rectangle to a clipping region
/// - Coordinates in parameter rect are modified to fit the region
///
/// \param[in]    pClipRect:   Pointer to clipping region
/// \param[inout] pRect:       Ptr to rectangle
///
/// \return true if rect is visible, false if it should be discarded
///
bool gslc_ClipRect(gslc_tsRect* pClipRect,gslc_tsRect* pRect);


///
/// Create an image reference to a bitmap file in LINUX filesystem
///
/// \param[in]   pFname       Pointer to filename string of image in filesystem
/// \param[in]   eFmt         Image format
///
/// \return Loaded image reference
///
gslc_tsImgRef gslc_GetImageFromFile(const char* pFname,gslc_teImgRefFlags eFmt);


///
/// Create an image reference to a bitmap file in SD card
///
/// \param[in]   pFname       Pointer to filename string of image in SD card
/// \param[in]   eFmt         Image format
///
/// \return Loaded image reference
///
gslc_tsImgRef gslc_GetImageFromSD(const char* pFname,gslc_teImgRefFlags eFmt);


///
/// Create an image reference to a bitmap in SRAM
///
/// \param[in]   pImgBuf      Pointer to image buffer in memory
/// \param[in]   eFmt         Image format
///
/// \return Loaded image reference
///
gslc_tsImgRef gslc_GetImageFromRam(unsigned char* pImgBuf,gslc_teImgRefFlags eFmt);


///
/// Create an image reference to a bitmap in program memory (PROGMEM)
///
/// \param[in]   pImgBuf      Pointer to image buffer in memory
/// \param[in]   eFmt         Image format
///
/// \return Loaded image reference
///
gslc_tsImgRef gslc_GetImageFromProg(const unsigned char* pImgBuf,gslc_teImgRefFlags eFmt);




///
/// Convert polar coordinate to cartesian
///
/// \param[in]   nRad         Radius of ray
/// \param[in]   n64Ang       Angle of ray (in units of 1/64 degrees, 0 is up)
/// \param[out]  nDX          X offset for ray end
/// \param[out]  nDY          Y offset for ray end
///
/// \return none
///
void gslc_PolarToXY(uint16_t nRad,int16_t n64Ang,int16_t* nDX,int16_t* nDY);


///
/// Calculate fixed-point sine function from fractional degrees
/// - Depending on configuration, the result is derived from either
///   floating point math library or fixed point lookup table.
/// - gslc_sinFX(nAngDeg*64)/32768.0 = sin(nAngDeg*2pi/360)
///
/// \param[in]   n64Ang       Angle (in units of 1/64 degrees)
///
/// \return Fixed-point sine result. Signed 16-bit; divide by 32768
///         to get the actual value.
///
int16_t gslc_sinFX(int16_t n64Ang);


///
/// Calculate fixed-point cosine function from fractional degrees
/// - Depending on configuration, the result is derived from either
///   floating point math library or fixed point lookup table.
/// - gslc_cosFX(nAngDeg*64)/32768.0 = cos(nAngDeg*2pi/360)
///
/// \param[in]   n64Ang       Angle (in units of 1/64 degrees)
///
/// \return Fixed-point cosine result. Signed 16-bit; divide by 32768
///         to get the actual value.
///
int16_t gslc_cosFX(int16_t n64Ang);

///
/// Create a color based on a blend between two colors
///
/// \param[in]  colStart:    Starting color
/// \param[in]  colEnd:      Ending color
/// \param[in]  nMidAmt:     Position (0..1000) between start and end color at which the
///                          midpoint between colors should appear. Normally set to 500 (half-way).
/// \param[in]  nBlendAmt:   The position (0..1000) between start and end at which we
///                          want to calculate the resulting blended color.
///
/// \return Blended color
///
gslc_tsColor gslc_ColorBlend2(gslc_tsColor colStart,gslc_tsColor colEnd,uint16_t nMidAmt,uint16_t nBlendAmt);

///
/// Create a color based on a blend between three colors
///
/// \param[in]  colStart:    Starting color
/// \param[in]  colMid:      Intermediate color
/// \param[in]  colEnd:      Ending color
/// \param[in]  nMidAmt:     Position (0..1000) between start and end color at which the
///                          intermediate color should appear.
/// \param[in]  nBlendAmt:   The position (0..1000) between start and end at which we
///                          want to calculate the resulting blended color.
///
/// \return Blended color
///
gslc_tsColor gslc_ColorBlend3(gslc_tsColor colStart,gslc_tsColor colMid,gslc_tsColor colEnd,uint16_t nMidAmt,uint16_t nBlendAmt);

///
/// Check whether two colors are equal
///
/// \param[in]  a:    First color
/// \param[in]  b:    Second color
///
/// \return True iff a and b are the same color.
///
bool gslc_ColorEqual(gslc_tsColor a,gslc_tsColor b);


// ------------------------------------------------------------------------
/// @}
/// \defgroup _GraphicsPrimFunc_ Graphics Primitive Functions
/// These routines cause immediate drawing to occur on the
/// primary screen
/// @{
// ------------------------------------------------------------------------


///
/// Set a pixel on the active screen to the given color with lock
/// - Calls upon gslc_DrvDrawSetPixelRaw() but wraps with a surface lock lock
/// - If repeated access is needed, use gslc_DrvDrawSetPixelRaw() instead
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nX:          Pixel X coordinate to set
/// \param[in]  nY:          Pixel Y coordinate to set
/// \param[in]  nCol:        Color pixel value to assign
///
/// \return none
///
void gslc_DrawSetPixel(gslc_tsGui* pGui,int16_t nX,int16_t nY,gslc_tsColor nCol);


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
void gslc_DrawLine(gslc_tsGui* pGui,int16_t nX0,int16_t nY0,int16_t nX1,int16_t nY1,gslc_tsColor nCol);


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
void gslc_DrawLineH(gslc_tsGui* pGui,int16_t nX, int16_t nY, uint16_t nW,gslc_tsColor nCol);


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
void gslc_DrawLineV(gslc_tsGui* pGui,int16_t nX, int16_t nY, uint16_t nH,gslc_tsColor nCol);


///
/// Draw a polar ray segment
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nX:          X coordinate of line startpoint
/// \param[in]  nY:          Y coordinate of line startpoint
/// \param[in]  nRadStart:   Starting radius of line
/// \param[in]  nRadEnd:     Ending radius of line
/// \param[in]  n64Ang:      Angle of ray (degrees * 64). 0 is up, +90*64 is to right
///                          From -180*64 to +180*64
/// \param[in]  nCol:        Color RGB value for the line
///
/// \return none
///
void gslc_DrawLinePolar(gslc_tsGui* pGui,int16_t nX,int16_t nY,uint16_t nRadStart,uint16_t nRadEnd,int16_t n64Ang,gslc_tsColor nCol);


///
/// Draw a framed rectangle
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  rRect:       Rectangular region to frame
/// \param[in]  nCol:        Color RGB value for the frame
///
/// \return none
///
void gslc_DrawFrameRect(gslc_tsGui* pGui,gslc_tsRect rRect,gslc_tsColor nCol);

///
/// Draw a framed rounded rectangle
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  rRect:       Rectangular region to frame
/// \param[in]  nRadius:     Radius for the rounded corners
/// \param[in]  nCol:        Color RGB value for the frame
///
/// \return none
///
void gslc_DrawFrameRoundRect(gslc_tsGui* pGui, gslc_tsRect rRect, int16_t nRadius, gslc_tsColor nCol);

///
/// Draw a filled rectangle
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  rRect:       Rectangular region to fill
/// \param[in]  nCol:        Color RGB value to fill
///
/// \return none
///
void gslc_DrawFillRect(gslc_tsGui* pGui,gslc_tsRect rRect,gslc_tsColor nCol);

///
/// Draw a filled rounded rectangle
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  rRect:       Rectangular region to fill
/// \param[in]  nRadius:     Radius for the rounded corners
/// \param[in]  nCol:        Color RGB value to fill
///
/// \return none
///
void gslc_DrawFillRoundRect(gslc_tsGui* pGui, gslc_tsRect rRect, int16_t nRadius, gslc_tsColor nCol);

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
  uint16_t nRadius,gslc_tsColor nCol);

///
/// Draw a filled circle
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nMidX:       Center X coordinate
/// \param[in]  nMidY:       Center Y coordinate
/// \param[in]  nRadius:     Radius of circle
/// \param[in]  nCol:        Color RGB value for the fill
///
/// \return none
///
void gslc_DrawFillCircle(gslc_tsGui* pGui,int16_t nMidX,int16_t nMidY,
  uint16_t nRadius,gslc_tsColor nCol);


///
/// Draw a framed triangle
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nX0:         X Coordinate #1
/// \param[in]  nY0:         Y Coordinate #1
/// \param[in]  nX1:         X Coordinate #2
/// \param[in]  nY1:         Y Coordinate #2
/// \param[in]  nX2:         X Coordinate #3
/// \param[in]  nY2:         Y Coordinate #3
/// \param[in]  nCol:        Color RGB value for the frame
///
/// \return true if success, false if error
///
void gslc_DrawFrameTriangle(gslc_tsGui* pGui,int16_t nX0,int16_t nY0,
    int16_t nX1,int16_t nY1,int16_t nX2,int16_t nY2,gslc_tsColor nCol);


///
/// Draw a filled triangle
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nX0:         X Coordinate #1
/// \param[in]  nY0:         Y Coordinate #1
/// \param[in]  nX1:         X Coordinate #2
/// \param[in]  nY1:         Y Coordinate #2
/// \param[in]  nX2:         X Coordinate #3
/// \param[in]  nY2:         Y Coordinate #3
/// \param[in]  nCol:        Color RGB value for the fill
///
/// \return true if success, false if error
///
void gslc_DrawFillTriangle(gslc_tsGui* pGui,int16_t nX0,int16_t nY0,
    int16_t nX1,int16_t nY1,int16_t nX2,int16_t nY2,gslc_tsColor nCol);


///
/// Draw a framed quadrilateral
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  psPt:        Pointer to array of 4 points
/// \param[in]  nCol:        Color RGB value for the frame
///
/// \return true if success, false if error
///
void gslc_DrawFrameQuad(gslc_tsGui* pGui,gslc_tsPt* psPt,gslc_tsColor nCol);


///
/// Draw a filled quadrilateral
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  psPt:        Pointer to array of 4 points
/// \param[in]  nCol:        Color RGB value for the frame
///
/// \return true if success, false if error
///
void gslc_DrawFillQuad(gslc_tsGui* pGui,gslc_tsPt* psPt,gslc_tsColor nCol);

///
/// Draw a gradient filled sector of a circle with support for inner and outer radius
/// - Can be used to create a ring or pie chart
/// - Note that the gradient fill is defined by both the color stops (cArcStart..cArcEnd) as well as
///   a gradient angular range (nAngGradStart..nAngGradStart+nAngGradRange). This gradient angular
///   range can be differeng from the drawing angular range (nAngSegStart..nAngSecEnd) to enable
///   more advanced control styling / updates.
///
/// \param[in]  pGui:          Pointer to GUI
/// \param[in]  nQuality:      Number of segments used to depict a full circle.
///                            The higher the value, the smoother the resulting
///                            arcs. A value of 72 provides 360/72=5 degrees per
///                            segment which is a reasonable compromise between
///                            smoothness and performance. Note that 360/nQuality
///                            should be an integer result, thus the allowable
///                            quality settings are: 360 (max quality), 180, 120,
///                            90, 72, 60, 45, 40, 36 (low quality), etc. 
/// \param[in]  nMidX:         Midpoint X coordinate of circle
/// \param[in]  nMidY:         Midpoint Y coordinate of circle
/// \param[in]  nRad1:         Inner sector radius (0 for sector / pie, non-zero for ring)
/// \param[in]  nRad2:         Outer sector radius. Delta from nRad1 defines ring thickness.
/// \param[in]  cArcStart:     Start color for gradient fill (with angular range defined by nAngGradStart,nAngGradRange)
/// \param[in]  cArcEnd:       End color for gradient fill
/// \param[in]  nAngSecStart:  Angle of start of sector drawing (0 at top), measured in degrees.
/// \param[in]  nAngSecEnd:    Angle of end of sector drawing (0 at top), measured in degrees.
/// \param[in]  nAngGradStart: For gradient fill, defines the starting angle associated with the starting color (cArcStart)
/// \param[in]  nAngGradRange: For gradient fill, defines the angular range associated with the start-to-end color range (cArcStart..cArcEnd)
///
/// \return none
///
void gslc_DrawFillGradSector(gslc_tsGui* pGui, int16_t nQuality, int16_t nMidX, int16_t nMidY, int16_t nRad1, int16_t nRad2,
  gslc_tsColor cArcStart, gslc_tsColor cArcEnd, int16_t nAngSecStart, int16_t nAngSecEnd, int16_t nAngGradStart, int16_t nAngGradRange);

///
/// Draw a flat filled sector of a circle with support for inner and outer radius
/// - Can be used to create a ring or pie chart
///
///
/// \param[in]  pGui:          Pointer to GUI
/// \param[in]  nQuality:      Number of segments used to depict a full circle.
///                            The higher the value, the smoother the resulting
///                            arcs. A value of 72 provides 360/72=5 degrees per
///                            segment which is a reasonable compromise between
///                            smoothness and performance.
/// \param[in]  nMidX:         Midpoint X coordinate of circle
/// \param[in]  nMidY:         Midpoint Y coordinate of circle
/// \param[in]  nRad1:         Inner sector radius (0 for sector / pie, non-zero for ring)
/// \param[in]  nRad2:         Outer sector radius. Delta from nRad1 defines ring thickness.
/// \param[in]  cArc:          Color for flat fill
/// \param[in]  nAngSecStart:  Angle of start of sector drawing (0 at top), measured in degrees.
/// \param[in]  nAngSecEnd:    Angle of end of sector drawing (0 at top), measured in degrees.
///
/// \return none
///
void gslc_DrawFillSector(gslc_tsGui* pGui, int16_t nQuality, int16_t nMidX, int16_t nMidY, int16_t nRad1, int16_t nRad2,
  gslc_tsColor cArc, int16_t nAngSecStart, int16_t nAngSecEnd);

// -----------------------------------------------------------------------
/// @}
/// \defgroup _Font_ Font Functions
/// Functions that load fonts
/// @{
// -----------------------------------------------------------------------

///
/// Load a font into the local font cache and assign
/// font ID (nFontId).
/// - Font is stored into next available internal array element
/// - NOTE: Use FontSet() instead
///
/// \param[in]  pGui:           Pointer to GUI
/// \param[in]  nFontId:        ID to use when referencing this font
/// \param[in]  eFontRefType:   Font reference type (eg. filename or pointer)
/// \param[in]  pvFontRef:      Reference pointer to identify the font. In the case of SDL
///                             mode, it is a filepath to the font file. In the case of Arduino
///                             it is a pointer value to the font bitmap array (GFXFont)
/// \param[in]  nFontSz:        Typeface size to use (only used in SDL mode)
///
/// \return true if load was successful, false otherwise
///
bool gslc_FontAdd(gslc_tsGui* pGui,int16_t nFontId,gslc_teFontRefType eFontRefType,
    const void* pvFontRef,uint16_t nFontSz);

///
/// Load a font into the local font cache and store as font ID (nFontId)
/// - Font is stored into index nFontId, so nFontId must be from
///   separate font enum (0-based).
/// - Example:
///   enum { E_FONT_BTN, E_FONT_TXT, MAX_FONT };
///
/// \param[in]  pGui:           Pointer to GUI
/// \param[in]  nFontId:        ID to use when referencing this font
/// \param[in]  eFontRefType:   Font reference type (eg. filename or pointer)
/// \param[in]  pvFontRef:      Reference pointer to identify the font. In the case of SDL
///                             mode, it is a filepath to the font file. In the case of Arduino
///                             it is a pointer value to the font bitmap array (GFXFont)
/// \param[in]  nFontSz:        Typeface size to use (only used in SDL mode)
///
/// \return true if load was successful, false otherwise
///
bool gslc_FontSet(gslc_tsGui* pGui, int16_t nFontId, gslc_teFontRefType eFontRefType,
  const void* pvFontRef, uint16_t nFontSz);

///
/// Fetch a font from its ID value
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nFontId:     ID value used to reference the font (supplied
///                          originally to gslc_FontAdd()
///
/// \return A pointer to the font structure or NULL if error
///
gslc_tsFont* gslc_FontGet(gslc_tsGui* pGui,int16_t nFontId);



///
/// Set the font operating mode
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nFontId:     ID value used to reference the font (supplied
///                          originally to gslc_FontAdd()
/// \param[in,out]  eFontMode:   Font mode to assign to this font
///
/// \return true if success
///
bool gslc_FontSetMode(gslc_tsGui* pGui, int16_t nFontId, gslc_teFontRefMode eFontMode);


// ------------------------------------------------------------------------
/// @}
/// \defgroup _Page_ Page Functions
/// Functions that operate at the page level
/// @{
// ------------------------------------------------------------------------

///
/// Fetch the current page ID
///
/// \param[in]  pGui:        Pointer to GUI
///
/// \return Page ID
///

int gslc_GetPageCur(gslc_tsGui* pGui);


///
/// Assign a page to the page stack
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nStackPos:   Position to update in the page stack (0..GSLC_STACK__MAX-1)
/// \param[in]  nPageId:     Page ID to select as current
///
/// \return none
///
void gslc_SetStackPage(gslc_tsGui* pGui,uint8_t nStackPos,int16_t nPageId);


///
/// Change the status of a page in a page stack
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nStackPos:   Position to update in the page stack (0..GSLC_STACK__MAX-1)
/// \param[in]  bActive:     Indicate if page should receive touch events
/// \param[in]  bDoDraw:     Indicate if page should continue to be redrawn. If pages in
///                          the stack are overlapping and an element in a lower layer
///                          continues to receive updates, then the element may "show through"
///                          the layers above it. In such cases where pages in the stack
///                          are overlapping and lower pages contain dynamically updating
///                          elements, it may be best to disable redraw while the overlapping
///                          page is visible (by setting bDoDraw to false).
///
/// \return none
///
void gslc_SetStackState(gslc_tsGui* pGui, uint8_t nStackPos, bool bActive, bool bDoDraw);


///
/// Assigns a page for the base layer in the page stack
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nPageId:     Page ID to select (or GSLC_PAGE_NONE to disable)
///
/// \return none
///
void gslc_SetPageBase(gslc_tsGui* pGui, int16_t nPageId);


///
/// Select a page for the current layer in the page stack
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nPageId:     Page ID to select
///
/// \return none
///
void gslc_SetPageCur(gslc_tsGui* pGui,int16_t nPageId);


///
/// Select a page for the overlay layer in the page stack
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nPageId:     Page ID to select (or GSLC_PAGE_NONE to disable)
///
/// \return none
///
void gslc_SetPageOverlay(gslc_tsGui* pGui,int16_t nPageId);


///
/// Show a popup dialog
/// - Popup dialogs use the overlay layer in the page stack
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nPageId:     Page ID to use as the popup dialog
/// \param[in]  bModal:      If true, popup is modal (other layers won't accept touch).
///                          If false, popup is modeless (other layers still accept touch)
///
/// \return none
///
void gslc_PopupShow(gslc_tsGui* pGui, int16_t nPageId, bool bModal);


///
/// Hides the currently active popup dialog
///
/// \param[in]  pGui:        Pointer to GUI
///
/// \return none
///
void gslc_PopupHide(gslc_tsGui* pGui);


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



/// Add a page to the GUI
/// - This call associates an element array with the collection within the page
/// - Once a page has been added to the GUI, elements can be added to the page
///   by specifying the same page ID
///
/// \param[in]  pGui:         Pointer to GUI
/// \param[in]  nPageId:      Page ID to assign
/// \param[in]  psElem:       Internal element array storage to associate with the page
/// \param[in]  nMaxElem:     Maximum number of elements that can be added to the
///                           internal element array (ie. RAM))
/// \param[in]  psElemRef:    Internal element reference array storage to
///                           associate with the page. All elements, whether they
///                           are located in the internal element array or in
///                           external Flash (PROGMEM) storage, require an entry
///                           in the element reference array.
/// \param[in]  nMaxElemRef:  Maximum number of elements in the reference array.
///                           This is effectively the maximum number of elements
///                           that can appear on a page, irrespective of whether
///                           it is stored in RAM or Flash (PROGMEM).
///
/// \return none
///
void gslc_PageAdd(gslc_tsGui* pGui,int16_t nPageId,gslc_tsElem* psElem,uint16_t nMaxElem,
        gslc_tsElemRef* psElemRef,uint16_t nMaxElemRef);

/// Find an element in the GUI by its Page ID and Element ID
///
/// \param[in]  pGui:         Pointer to GUI
/// \param[in]  nPageId:      Page ID to search
/// \param[in]  nElemId:      Element ID to search
///
/// \return Ptr to an element or NULL if none found
///
gslc_tsElemRef* gslc_PageFindElemById(gslc_tsGui* pGui,int16_t nPageId,int16_t nElemId);


// ------------------------------------------------------------------------
/// @}
/// \defgroup _Elem_ Element Functions
/// Functions that are used to create and manipulate elements
/// @{
// ------------------------------------------------------------------------


// ------------------------------------------------------------------------
/// \defgroup _ElemCreate_ Element: Creation Functions
/// Functions that create GUI elements
/// @{
// ------------------------------------------------------------------------


///
/// Create a Text Element
/// - Draws a text string with filled background
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nElemId:     Element ID to assign (0..16383 or GSLC_ID_AUTO to autogen)
/// \param[in]  nPage:       Page ID to attach element to
/// \param[in]  rElem:       Rectangle coordinates defining text background size
/// \param[in]  pStrBuf:     String to copy into element
/// \param[in]  nStrBufMax:  Maximum length of string buffer (pStrBuf). Only applicable
///                          if GSLC_LOCAL_STR=0. Ignored if GSLC_LOCAL_STR=1.)
/// \param[in]  nFontId:     Font ID to use for text display
///
/// \return Pointer to the Element reference or NULL if failure
///
gslc_tsElemRef* gslc_ElemCreateTxt(gslc_tsGui* pGui,int16_t nElemId,int16_t nPage,
  gslc_tsRect rElem,char* pStrBuf,uint8_t nStrBufMax,int16_t nFontId);


///
/// Create a textual Button Element
/// - Creates a clickable element that has a textual label
///   with frame and fill
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nElemId:     Element ID to assign (0..16383 or GSLC_ID_AUTO to autogen)
/// \param[in]  nPage:       Page ID to attach element to
/// \param[in]  rElem:       Rectangle coordinates defining text background size
/// \param[in]  pStrBuf:     String to copy into element
/// \param[in]  nStrBufMax:  Maximum length of string buffer (pStrBuf). Only applicable
///                          if GSLC_LOCAL_STR=0. Ignored if GSLC_LOCAL_STR=1.)
/// \param[in]  nFontId:     Font ID to use for text display
/// \param[in]  cbTouch:     Callback for touch events
///
/// \return Pointer to the Element reference or NULL if failure
///
gslc_tsElemRef* gslc_ElemCreateBtnTxt(gslc_tsGui* pGui,int16_t nElemId,int16_t nPage,
  gslc_tsRect rElem,char* pStrBuf,uint8_t nStrBufMax,
  int16_t nFontId,GSLC_CB_TOUCH cbTouch);

///
/// Create a graphical Button Element
/// - Creates a clickable element that uses a BMP image with
///   no frame or fill
/// - Transparency is supported by bitmap color (0xFF00FF)
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nElemId:     Element ID to assign (0..16383 or GSLC_ID_AUTO to autogen)
/// \param[in]  nPage:       Page ID to attach element to
/// \param[in]  rElem:       Rectangle coordinates defining image size
/// \param[in]  sImgRef:     Image reference to load (unselected state)
/// \param[in]  sImgRefSel:  Image reference to load (selected state)
/// \param[in]  cbTouch:     Callback for touch events
///
/// \return Pointer to the Element reference or NULL if failure
///
gslc_tsElemRef* gslc_ElemCreateBtnImg(gslc_tsGui* pGui,int16_t nElemId,int16_t nPage,
  gslc_tsRect rElem,gslc_tsImgRef sImgRef,gslc_tsImgRef sImgRefSel,GSLC_CB_TOUCH cbTouch);


///
/// Create a Box Element
/// - Draws a box with frame and fill
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nElemId:     Element ID to assign (0..16383 or GSLC_ID_AUTO to autogen)
/// \param[in]  nPage:       Page ID to attach element to
/// \param[in]  rElem:       Rectangle coordinates defining box size
///
/// \return Pointer to the Element reference or NULL if failure
///
gslc_tsElemRef* gslc_ElemCreateBox(gslc_tsGui* pGui,int16_t nElemId,int16_t nPage,gslc_tsRect rElem);

///
/// Create a Line Element
/// - Draws a line with fill color
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nElemId:     Element ID to assign (0..16383 or GSLC_ID_AUTO to autogen)
/// \param[in]  nPage:       Page ID to attach element to
/// \param[in]  nX0:         X coordinate of line startpoint
/// \param[in]  nY0:         Y coordinate of line startpoint
/// \param[in]  nX1:         X coordinate of line endpoint
/// \param[in]  nY1:         Y coordinate of line endpoint
///
/// \return Pointer to the Element reference or NULL if failure
///
gslc_tsElemRef* gslc_ElemCreateLine(gslc_tsGui* pGui,int16_t nElemId,int16_t nPage,int16_t nX0,int16_t nY0,int16_t nX1,int16_t nY1);

///
/// Create an image Element
/// - Draws an image
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nElemId:     Element ID to assign (0..16383 or GSLC_ID_AUTO to autogen)
/// \param[in]  nPage:       Page ID to attach element to
/// \param[in]  rElem:       Rectangle coordinates defining box size
/// \param[in]  sImgRef:     Image reference to load
///
/// \return Pointer to the Element reference or NULL if failure
///
gslc_tsElemRef* gslc_ElemCreateImg(gslc_tsGui* pGui,int16_t nElemId,int16_t nPage,gslc_tsRect rElem,
  gslc_tsImgRef sImgRef);


// ------------------------------------------------------------------------
/// @}
/// \defgroup _ElemGen_ Element: General Functions
/// General-purpose functions that operate on Elements
/// @{
// ------------------------------------------------------------------------


///
/// Get an Element ID from an element structure
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference structure
///
/// \return ID of element or GSLC_ID_NONE if not found
///
int gslc_ElemGetId(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef);


// ------------------------------------------------------------------------
/// @}
/// \defgroup _ElemUpd_ Element: Update Functions
/// Functions that configure or modify an existing eleemnt
/// @{
// ------------------------------------------------------------------------

///
/// Set the fill state for an Element
/// - If not filled, the element can support transparency against an
///   arbitrary background, but this can require full screen redraws
///   if the element is updated.
/// - If filled, the background fill color can be changed by gslc_ElemSetCol()
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
/// \param[in]  bFillEn:     True if filled, false otherwise
///
/// \return none
///
void gslc_ElemSetFillEn(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,bool bFillEn);

///
/// Set the frame state for an Element
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
/// \param[in]  bFrameEn:    True if framed, false otherwise
///
/// \return none
///
void gslc_ElemSetFrameEn(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,bool bFrameEn);

///
/// Set the rounded frame/fill state for an Element
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
/// \param[in]  bRoundEn:    True if rounded, false otherwise
///
/// \return none
///
void gslc_ElemSetRoundEn(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, bool bRoundEn);

///
/// Update the common color selection for an Element
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
/// \param[in]  colFrame:    Color for the frame
/// \param[in]  colFill:     Color for the fill
/// \param[in]  colFillGlow: Color for the fill when glowing
///
/// \return none
///
void gslc_ElemSetCol(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,gslc_tsColor colFrame,gslc_tsColor colFill,gslc_tsColor colFillGlow);

///
/// Update the common color selection for glowing state of an Element
///
/// \param[in]  pGui:         Pointer to GUI
/// \param[in]  pElemRef:     Pointer to Element reference
/// \param[in]  colFrameGlow: Color for the frame when glowing
/// \param[in]  colFillGlow:  Color for the fill when glowing
/// \param[in]  colTxtGlow    Color for the text when glowing
///
/// \return none
///
void gslc_ElemSetGlowCol(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,gslc_tsColor colFrameGlow,gslc_tsColor colFillGlow,gslc_tsColor colTxtGlow);


///
/// Set the group ID for an element
/// - Typically used to associate radio button elements together
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
/// \param[in]  nGroupId:    Group ID to assign
///
/// \return none
///
void gslc_ElemSetGroup(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,int nGroupId);


///
/// Get the group ID for an element
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
///
/// \return Group ID or GSLC_GROUP_ID_NONE if unassigned
///
int gslc_ElemGetGroup(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef);


///
/// Set the position and size for an element
/// - This updates the element's rectangular region, which can be used
///   to relocate or resize an element at runtime
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
/// \param[in]  rElem:       Rect region (top-left coord, width, height)
///
/// \return none
///
void gslc_ElemSetRect(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,gslc_tsRect rElem);


///
/// Get the rectangular region for an element
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
///
/// \return Rect region of an element
///
gslc_tsRect gslc_ElemGetRect(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef);


/// Set the alignment of a textual element (horizontal and vertical)
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
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
void gslc_ElemSetTxtAlign(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,unsigned nAlign);

/// Set the margin around of a textual element
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
/// \param[in]  nMargin:     Number of pixels gap to leave surrounding text
///
/// \return none
///
void gslc_ElemSetTxtMargin(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,unsigned nMargin);

/// Set the margin around of a textual element (X & Y offsets can be different)
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
/// \param[in]  nMarginX:    Number of pixels gap to offset text horizontally
/// \param[in]  nMarginY:    Number of pixels gap to offset text vertically
///
/// \return none
///
void gslc_ElemSetTxtMarginXY(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,int8_t nMarginX,int8_t nMarginY);

///
/// Helper routine to perform string deep copy
/// - Includes termination
/// - Similar to strncpy() except:
///   - nDstLen is the total buffer size (including terminator)
///   - A terminator is added at the end of the buffer
///
/// \param[inout] pDstStr:    Pointer to destination buffer
/// \param[in]    nDstLen:    Size of destination buffer (includes NULL)
/// \param[in]    pSrcStr:    Pointer to source buffer
///
/// \return none
///
void gslc_StrCopy(char* pDstStr,const char* pSrcStr,uint16_t nDstLen);

///
/// Update the text string associated with an Element
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
/// \param[in]  pStr:        String to copy into element
///
/// \return none
///
void gslc_ElemSetTxtStr(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,const char* pStr);


///
/// Fetch the current text string associated with an Element
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
///
/// \return Pointer to character array string
///
char* gslc_ElemGetTxtStr(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef);


///
/// Update the text string color associated with an Element ID
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
/// \param[in]  colVal:      RGB color to change to
///
/// \return none
///
void gslc_ElemSetTxtCol(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,gslc_tsColor colVal);


///
/// Update the text string location in memory
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
/// \param[in]  eFlags:      Flags associated with text memory location (GSLC_TXT_MEM_*)
///
/// \return none
///
void gslc_ElemSetTxtMem(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,gslc_teTxtFlags eFlags);


///
/// Update the text string encoding mode
/// - This function can be used to indicate that the element's text string is
///   encoded in UTF-8, which supports extended / foreign character maps
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
/// \param[in]  eFlags:      Flags associated with text encoding (GSLC_TXT_ENC_*)
///
/// \return none
///
void gslc_ElemSetTxtEnc(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,gslc_teTxtFlags eFlags);


///
/// Update the Font selected for an Element's text
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
/// \param[in]  nFontId:     Font ID to select
///
/// \return none
///
void gslc_ElemUpdateFont(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,int nFontId);

///
/// Update the need-redraw status for an element
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
/// \param[in]  eRedraw:     Redraw state to set
///
/// \return none
///
void gslc_ElemSetRedraw(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,gslc_teRedrawType eRedraw);

///
/// Get the need-redraw status for an element
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
///
/// \return Redraw status
///
gslc_teRedrawType gslc_ElemGetRedraw(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef);

///
/// Update the glowing enable for an element
///
/// \param[in]  pGui:       Pointer to GUI
/// \param[in]  pElemRef:   Pointer to Element reference
/// \param[in]  bGlowEn:    True if element should support glowing
///
/// \return none
///
void gslc_ElemSetGlowEn(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,bool bGlowEn);

///
/// Update the click enable for an element
///
/// \param[in]  pGui:       Pointer to GUI
/// \param[in]  pElemRef:   Pointer to Element reference
/// \param[in]  bClickEn:   True if element should support click events
///
/// \return none
///
void gslc_ElemSetClickEn(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,bool bClickEn);

///
/// Update the touch function callback for an element
///
/// \param[in]  pGui:       Pointer to GUI
/// \param[in]  pElemRef:   Pointer to Element reference
/// \param[in]  funcCb:     Pointer to the touch callback function
///
/// \return none
///
void gslc_ElemSetTouchFunc(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, GSLC_CB_TOUCH funcCb);

///
/// Copy style settings from one element to another
///
/// \param[in]  pGui:          Pointer to GUI
/// \param[in]  pElemRefSrc:   Pointer to source Element reference
/// \param[in]  pElemRefDest:  Pointer to destination Element reference
///
/// \return none
///
void gslc_ElemSetStyleFrom(gslc_tsGui* pGui,gslc_tsElemRef* pElemRefSrc,gslc_tsElemRef* pElemRefDest);

///
/// Reset the element region state struct
///
/// \param[out] pState:        Pointer to the element region structure
///
/// \return none
///
void gslc_ResetRectState(gslc_tsRectState *pState);

///
/// Calculate the element region state struct
/// - Establishes the size of the frame and inner regions
/// - Determines the color of various parts of the element
///   (focus rect, frame, text, fll, etc.)
/// - The region state is calculated based upon the
///   element's attributes, including the frame enable,
///   ability to support focus, whether the contents can
///   be shrunk to accommodate a frame, and the current
///   state of focus/edit/etc.
///
/// \param[in]  pGui:          Pointer to GUI
/// \param[in]  pElemRef:      Pointer to Element reference
/// \param[out] pState:        Pointer to element region state
///
/// \return none
///
void gslc_ElemCalcRectState(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, gslc_tsRectState* pState);

///
/// Calculate the change in dimensions of an element to account for
/// any change in focus and/or frame attributes. It also takes
/// into account the "NoShrink" attribute which indicates that
/// an element's contents can't be reduced in size (eg. because
/// they contain a fixed-sized image).
/// - This routine is usually called whenever an element is created
///   and also whenever size-impacting features are adjusted
///   (eg. ElemSetFrameEn).
///
/// \param[in]  pGui:          Pointer to GUI
/// \param[in]  pElemRef:      Pointer to Element reference
///
/// \return The increase (or decrease) in size of the element
///
int8_t gslc_ElemCalcResizeForFocus(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef);

///
/// Increase or decrease the size of an element's region
/// - Update the redraw status as needed
///
/// \param[in]  pGui:          Pointer to GUI
/// \param[in]  pElemRef:      Pointer to Element reference
/// \param[in]  nDelta:        The increase of element size (negative for decrease)
///
/// \return none
///
void gslc_ElemGrowRect(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, int8_t nDelta);

///
/// Get the glowing enable for an element
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
///
/// \return True if element supports glowing
///
bool gslc_ElemGetGlowEn(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef);


///
/// Update the glowing indicator for an element
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
/// \param[in]  bGlowing:    True if element is glowing
///
/// \return none
///
void gslc_ElemSetGlow(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,bool bGlowing);

///
/// Get the glowing indicator for an element
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
///
/// \return True if element is glowing
///
bool gslc_ElemGetGlow(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef);

///
/// Get the focus enable for an element
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
///
/// \return True if element supports focus
///
bool gslc_ElemGetFocusEn(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef);

///
/// Set the focus enable for an element
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
/// \param[in]  bFocusEn:    Enable focus if 1, 0 if not supported
///
/// \return none
///
void gslc_ElemSetFocusEn(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,bool bFocusEn);

///
/// Update the focused indicator for an element
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
/// \param[in]  bFocused:    True if element is focused
///
/// \return none
///
void gslc_ElemSetFocus(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,bool bFocused);

///
/// Get the focused indicator for an element
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
///
/// \return True if element is focused
///
bool gslc_ElemGetFocus(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef);

///
/// Update the editing indicator for an element
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
/// \param[in]  bEditing:    True if element is being edited
///
/// \return none
///
void gslc_ElemSetEdit(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,bool bEditing);

///
/// Get the editing indicator for an element
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
///
/// \return True if element is being edited
///
bool gslc_ElemGetEdit(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef);

///
/// Update the visibility status for an element
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
/// \param[in]  bVisible:    True if element is shown, false if hidden
///
/// \return none
///
void gslc_ElemSetVisible(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,bool bVisible);

///
/// Get the visibility status for an element
/// - Note that the visibility state is independent of
///   whether or not the page associated with the element
///   is actively displayed.
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
///
/// \return True if element is marked as visible, false if hidden
///
bool gslc_ElemGetVisible(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef);

///
/// Determine whether an element is visible on the screen
/// - This function takes into account both the element's
///   "Visible" state as well as whether the element's
///   associated page is active in the page stack.
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
///
/// \return True if element appears on the screen, false otherwise
///
bool gslc_ElemGetOnScreen(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef);

/* UNUSED
///
/// Assign the event callback function for a element
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
/// \param[in]  funcCb:      Function pointer to event routine (or NULL for default))
///
/// \return none
///
void gslc_ElemSetEventFunc(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,GSLC_CB_EVENT funcCb);
*/


///
/// Assign the drawing callback function for an element
/// - This allows the user to override the default rendering for
///   an element, enabling the creation of a custom element
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
/// \param[in]  funcCb:      Function pointer to drawing routine (or NULL for default))
///
/// \return none
///
void gslc_ElemSetDrawFunc(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,GSLC_CB_DRAW funcCb);

///
/// Assign the tick callback function for an element
/// - This allows the user to provide background updates to
///   an element triggered by the main loop call to gslc_Update()
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference
/// \param[in]  funcCb:      Function pointer to tick routine (or NULL for none))
///
/// \return none
///
void gslc_ElemSetTickFunc(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,GSLC_CB_TICK funcCb);


///
/// Determine if a coordinate is inside of an element
/// - This routine is useful in determining if a touch
///   coordinate is inside of a button.
///
/// \param[in]  pGui:         Pointer to GUI
/// \param[in]  pElemRef:     Element reference used for boundary test
/// \param[in]  nX:           X coordinate to test
/// \param[in]  nY:           Y coordinate to test
/// \param[in]  bOnlyClickEn: Only output true if element was also marked
///                           as "clickable" (eg. bClickEn=true)
///
/// \return true if inside element, false otherwise
///
bool gslc_ElemOwnsCoord(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,int16_t nX,int16_t nY,bool bOnlyClickEn);





/// @} // End of _Elem_

#if !defined(DRV_TOUCH_NONE)

// ------------------------------------------------------------------------
/// @}
/// \defgroup _Touch_ Touchscreen Functions
/// Functions that configure and respond to a touch device
/// @{
// ------------------------------------------------------------------------

// Define the transformation matrix from rotation (0..3) to SWAP / FLIPX / FLIPY
#define TOUCH_ROTATION_DATA 0x6350
#define TOUCH_ROTATION_SWAPXY(rotation) ((( TOUCH_ROTATION_DATA >> ((rotation&0x03)*4) ) >> 2 ) & 0x01 )
#define TOUCH_ROTATION_FLIPX(rotation)  ((( TOUCH_ROTATION_DATA >> ((rotation&0x03)*4) ) >> 1 ) & 0x01 )
#define TOUCH_ROTATION_FLIPY(rotation)  ((( TOUCH_ROTATION_DATA >> ((rotation&0x03)*4) ) >> 0 ) & 0x01 )


///
/// Initialize the touchscreen device driver
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  acDev:       Device path to touchscreen (or "" if not applicable))
///                          eg. "/dev/input/touchscreen"
///
/// \return true if successful
///
bool gslc_InitTouch(gslc_tsGui* pGui,const char* acDev);


///
/// Initialize the touchscreen device driver
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[out] pnX:         Ptr to int to contain latest touch X coordinate
/// \param[out] pnY:         Ptr to int to contain latest touch Y coordinate
/// \param[out] pnPress:     Ptr to int to contain latest touch pressure value
/// \param[out] peInputEvent Indication of event type
/// \param[out] pnInputVal   Additional data for event type
///
/// \return true if touch event, false otherwise
///
bool gslc_GetTouch(gslc_tsGui* pGui, int16_t* pnX, int16_t* pnY, uint16_t* pnPress, gslc_teInputRawEvent* peInputEvent, int16_t* pnInputVal);


///
/// Configure touchscreen remapping
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  bEn:         Enable touchscreen remapping?
///
/// \return none
///
void gslc_SetTouchRemapEn(gslc_tsGui* pGui, bool bEn);

///
/// Configure touchscreen calibration remapping values
/// - Only used if calibration remapping has been enabled
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nXMin:       Resistive touchscreen X_MIN calibration value
/// \param[in]  nXMax:       Resistive touchscreen X_MAX calibration value
/// \param[in]  nYMin:       Resistive touchscreen Y_MIN calibration value
/// \param[in]  nYMax:       Resistive touchscreen Y_MAX calibration value
///
/// \return none
///
void gslc_SetTouchRemapCal(gslc_tsGui* pGui,uint16_t nXMin, uint16_t nXMax, uint16_t nYMin, uint16_t nYMax);

///
/// Configure touchscreen calibration pressure values
/// - Only used if calibration remapping has been enabled
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nPressMin:   Resistive touchscreen pressure min value
/// \param[in]  nPressMax:   Resistive touchscreen pressure max value
///
/// \return none
///
void gslc_SetTouchPressCal(gslc_tsGui* pGui,uint16_t nPressMin, uint16_t nPressMax);

///
/// Configure touchscreen XY swap
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  bSwap:       Enable touchscreen XY swap
///
/// \return none
///
void gslc_SetTouchRemapYX(gslc_tsGui* pGui, bool bSwap);

#endif // !DRV_TOUCH_NONE

// ------------------------------------------------------------------------
/// @}
/// \defgroup _Input_ Input Mapping Functions
/// Functions that handle GPIO / pin and keyboard input
/// @{
// ------------------------------------------------------------------------


///
/// Specify the callback function that is used to collect
/// the state of any external inputs (eg. buttons,
/// pins, encoders, etc.)
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pfunc:       Pointer to the callback function
///
/// \return none
///
void gslc_SetPinPollFunc(gslc_tsGui* pGui, GSLC_CB_PIN_POLL pfunc);

///
/// Specify the mapping between external pin inputs (fetched
/// by the SetPinPollFunc() callback and the GUI actions.
/// - This is used to enable external controls to navigate
///   and manipulate the GUI.
///
/// \param[in]  pGui:         Pointer to GUI
/// \param[in]  asInputMap:   Pointer to the input mapping table
/// \param[in]  nInputMapMax: Total number of entries in mapping table
///
/// \return none
///
void gslc_InitInputMap(gslc_tsGui* pGui,gslc_tsInputMap* asInputMap,uint8_t nInputMapMax);

///
/// Add an entry into the external input mapping table
///
/// \param[in]  pGui:         Pointer to GUI
/// \param[in]  eInputEvent:  The event to detect
/// \param[in]  nInputVal:    The value associated with the detected event
/// \param[in]  eAction:      The action to take in the GUI
/// \param[in]  nActionVal:   An optional parameter to associate with the GUI action
///
/// \return none
///
void gslc_InputMapAdd(gslc_tsGui* pGui,gslc_teInputRawEvent eInputEvent,int16_t nInputVal,gslc_teAction eAction,int16_t nActionVal);

///
/// Find the currently focused element
///
/// \param[in]  pGui:        Pointer to GUI
///
/// \return Element reference of focused widget, or NULL if none
///
gslc_tsElemRef* gslc_FocusElemGet(gslc_tsGui* pGui);

///
/// Advance the focus to the next page in the page stack
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  bNext:       Advance to next page if true, previous if false
///
/// \return none
///
void gslc_FocusPageStep(gslc_tsGui* pGui,bool bNext);

///
/// Advance the focus to the next element in the focused page
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  bNext:       Advance to next element if true, previous if false
///
/// \return none
///
int16_t gslc_FocusElemStep(gslc_tsGui* pGui,bool bNext);

///
/// Change the focus to the indexed element on the specified page
/// - First clear any existing focus before setting a new focus
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nPageInd:    The index of the page containing the element
/// \param[in]  nElemInd:    The index of the element containing the element
/// \param[in]  bFocus:      If true, enables the focus on the specified
///                          element after clearing the focus on the old
///                          element. If false, no focus is enabled after
///                          clearing any existing focus.
///
/// \return none
///
void gslc_FocusElemIndSet(gslc_tsGui* pGui,int16_t nPageInd,int16_t nElemInd,bool bFocus);

///
/// Change the focus to the currently-tracked element
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pCollect:    The active element collection to examine
///
/// \return none
///
void gslc_FocusSetToTrackedElem(gslc_tsGui* pGui,gslc_tsCollect* pCollect);

// ------------------------------------------------------------------------
/// @}
/// \defgroup _GenMacro_ General Purpose Macros
/// Macros that are used throughout the GUI for debug
/// @{
// ------------------------------------------------------------------------

// Create debug macro to selectively include the output code

/// \def GSLC_DEBUG_PRINT(fmt, ...)
///
/// Macro to enable optional debug output
/// - Supports printf formatting via gslc_DebugPrintf()
/// - Supports storing the format string in PROGMEM
/// - Note that at least one variable argument must be provided
///   to the macro after the format string. This is a limitation
///   of the macro definition. If no parameters are needed, then
///   simply pass 0. For example:
///     GSLC_DEBUG_PRINT("Loaded OK",0);
///
/// \param[in]  sFmt:       Format string for debug message
///

#if (GSLC_USE_PROGMEM)
  // Debug print macro for CPUs that support PROGMEM (Flash)
  #define GSLC_DEBUG_PRINT(sFmt, ...)                           \
          do {                                                  \
            if (DEBUG_ERR) {                                    \
              gslc_DebugPrintf(PSTR(sFmt),__VA_ARGS__);         \
            }                                                   \
          } while (0)
  #define GSLC_DEBUG2_PRINT(sFmt, ...)                          \
          do {                                                  \
            if (DEBUG_ERR >= 2) {                               \
              gslc_DebugPrintf(PSTR(sFmt),__VA_ARGS__);         \
            }                                                   \
          } while (0)
#else
  // Debug print macro for CPUs that don't support PROGMEM (Flash)
  #define GSLC_DEBUG_PRINT(sFmt, ...)                           \
          do {                                                  \
            if (DEBUG_ERR) {                                    \
              gslc_DebugPrintf(sFmt,__VA_ARGS__);               \
            }                                                   \
          } while (0)
  #define GSLC_DEBUG2_PRINT(sFmt, ...)                          \
          do {                                                  \
            if (DEBUG_ERR >= 2) {                               \
              gslc_DebugPrintf(sFmt,__VA_ARGS__);               \
            }                                                   \
          } while (0)
#endif

  #define GSLC_DEBUG_PRINT_CONST(sFmt, ...)                     \
          do {                                                  \
            if (DEBUG_ERR) {                                    \
              gslc_DebugPrintf(sFmt,__VA_ARGS__);               \
            }                                                   \
          } while (0)
  #define GSLC_DEBUG2_PRINT_CONST(sFmt, ...)                    \
          do {                                                  \
            if (DEBUG_ERR >= 2) {                               \
              gslc_DebugPrintf(sFmt,__VA_ARGS__);               \
            }                                                   \
          } while (0)



// ------------------------------------------------------------------------
/// @}
/// \defgroup _MacroFlash_ Flash-based Element Macros
/// Macros that represent element creation routines based in FLASH memory
/// @{
// ------------------------------------------------------------------------

// Macro initializers for Read-Only Elements in Flash/PROGMEM
//
// - Generally useful in Arduino sketches targeting low-RAM CPUs (such as ATmega328)
// - These macros perform initialization of a static tsElem variable (located
//   in PROGMEM). If a string is required, then it too is allocated in
//   PROGMEM space.
// - The benefit of this macro is in providing significant RAM savings for
//   programs that instantiate a number of read-only GUI elements. The net
//   cost of each read-only GUI element is approximately 4B when using these
//   methods.
// - Unlike the normal ElemCreate*() functions, these ElemCreate*_P() macros
//   don't return a tsElem pointer. Since the element definition is in
//   Flash/PROGMEM, it is read-only and no updates should be attempted.
// - These macros have to match the current definition and order of gslc_tsElem
//   exactly, so any changes to the element definition must be reflected here
//   as well.


/// \def gslc_ElemCreateTxt_P(pGui,nElemId,nPage,nX,nY,nW,nH,strTxt,pFont,colTxt,colFrame,colFill,nAlignTxt,bFrameEn,bFillEn)
///
/// Create a read-only text element
///
/// \param[in]  pGui:       Pointer to GUI
/// \param[in]  nElemId:    Unique element ID to assign
/// \param[in]  nPage:      Page ID to attach element to
/// \param[in]  nX:         X coordinate of element
/// \param[in]  nY:         Y coordinate of element
/// \param[in]  nW:         Width of element
/// \param[in]  nH:         Height of element
/// \param[in]  strTxt:     Text string to display
/// \param[in]  pFont:      Pointer to font resource
/// \param[in]  colTxt:     Color for the text
/// \param[in]  colFrame:   Color for the frame
/// \param[in]  colFill:    Color for the fill
/// \param[in]  nAlignTxt:  Text alignment
/// \param[in]  bFrameEn:   True if framed, false otherwise
/// \param[in]  bFillEn:    True if filled, false otherwise
///

/// \def gslc_ElemCreateTxt_P_R(pGui,nElemId,nPage,nX,nY,nW,nH,strTxt,strLength,pFont,colTxt,colFrame,colFill,nAlignTxt,bFrameEn,bFillEn)
///
/// Create a read-write text element (element in Flash, string in RAM)
///
/// \param[in]  pGui:       Pointer to GUI
/// \param[in]  nElemId:    Unique element ID to assign
/// \param[in]  nPage:      Page ID to attach element to
/// \param[in]  nX:         X coordinate of element
/// \param[in]  nY:         Y coordinate of element
/// \param[in]  nW:         Width of element
/// \param[in]  nH:         Height of element
/// \param[in]  strTxt:     Text string to display
/// \param[in]  strLength:  Length of text string
/// \param[in]  pFont:      Pointer to font resource
/// \param[in]  colTxt:     Color for the text
/// \param[in]  colFrame:   Color for the frame
/// \param[in]  colFill:    Color for the fill
/// \param[in]  nAlignTxt:  Text alignment
/// \param[in]  bFrameEn:   True if framed, false otherwise
/// \param[in]  bFillEn:    True if filled, false otherwise
///

/// \def gslc_ElemCreateTxt_P_R_ext(pGui,nElemId,nPage,nX,nY,nW,nH,strTxt,strLength,pFont,colTxt,colTxtGlow,colFrame,colFill,nAlignTxt,nMarginX,nMarginY,bFrameEn,bFillEn,bClickEn,bGlowEn,pfuncXEvent,pfuncXDraw,pfuncXTouch,pfuncXTick)
///
/// Create a read-write text element (element in Flash, string in RAM)
/// with extended customization options.
///
/// \param[in]  pGui:       Pointer to GUI
/// \param[in]  nElemId:    Unique element ID to assign
/// \param[in]  nPage:      Page ID to attach element to
/// \param[in]  nX:         X coordinate of element
/// \param[in]  nY:         Y coordinate of element
/// \param[in]  nW:         Width of element
/// \param[in]  nH:         Height of element
/// \param[in]  strTxt:     Text string to display
/// \param[in]  strLength:  Length of text string
/// \param[in]  pFont:      Pointer to font resource
/// \param[in]  colTxt:     Color for the text
/// \param[in]  colTxtGlow: Color for the text when glowing
/// \param[in]  colFrame:   Color for the frame
/// \param[in]  colFill:    Color for the fill
/// \param[in]  nAlignTxt:  Text alignment
/// \param[in]  nMarginX:   Text margin (X offset)
/// \param[in]  nMarginY:   Text margin (Y offset)
/// \param[in]  bFrameEn:   True if framed, false otherwise
/// \param[in]  bFillEn:    True if filled, false otherwise
/// \param[in]  bClickEn:   True if accept click events, false otherwise
/// \param[in]  bGlowEn:    True if supports glow state, false otherwise
/// \param[in]  pfuncXEvent:  Callback function ptr for Event
/// \param[in]  pfuncXDraw:   Callback function ptr for Redraw
/// \param[in]  pfuncXTouch:  Callback function ptr for Touch
/// \param[in]  pfuncXTick:   Callback function ptr for Timer tick
///

/// \def gslc_ElemCreateBox_P(pGui,nElemId,nPage,nX,nY,nW,nH,colFrame,colFill,bFrameEn,bFillEn,pfuncXDraw,pfuncXTick)
///
/// Create a read-only box element
///
/// \param[in]  pGui:       Pointer to GUI
/// \param[in]  nElemId:    Unique element ID to assign
/// \param[in]  nPage:      Page ID to attach element to
/// \param[in]  nX:         X coordinate of element
/// \param[in]  nY:         Y coordinate of element
/// \param[in]  nW:         Width of element
/// \param[in]  nH:         Height of element
/// \param[in]  colFrame:   Color for the frame
/// \param[in]  colFill:    Color for the fill
/// \param[in]  bFrameEn:   True if framed, false otherwise
/// \param[in]  bFillEn:    True if filled, false otherwise
/// \param[in]  pfuncXDraw: Pointer to custom draw callback (or NULL if default)
/// \param[in]  pfuncXTick: Pointer to custom tick callback (or NULL if default)
///


/// \def gslc_ElemCreateLine_P(pGui,nElemId,nPage,nX0,nY0,nX1,nY1,colFill)
///
/// Create a read-only line element
///
/// \param[in]  pGui:       Pointer to GUI
/// \param[in]  nElemId:    Unique element ID to assign
/// \param[in]  nPage:      Page ID to attach element to
/// \param[in]  nX0:        X coordinate of line start
/// \param[in]  nY0:        Y coordinate of line start
/// \param[in]  nX1:        X coordinate of line end
/// \param[in]  nY1:        Y coordinate of line end
/// \param[in]  colFill:    Color for the line


/// \def gslc_ElemCreateBtnTxt_P(pGui,nElemId,nPage,nX,nY,nW,nH,strTxt,pFont,colTxt,colFrame,colFill,colFrameGlow,colFillGlow,nAlignTxt,bFrameEn,bFillEn,callFunc,extraData)
///
/// Create a text button element
///
/// \param[in]  pGui:         Pointer to GUI
/// \param[in]  nElemId:      Unique element ID to assign
/// \param[in]  nPage:        Page ID to attach element to
/// \param[in]  nX:           X coordinate of element
/// \param[in]  nY:           Y coordinate of element
/// \param[in]  nW:           Width of element
/// \param[in]  nH:           Height of element
/// \param[in]  strTxt:       Text string to display
/// \param[in]  pFont:        Pointer to font resource
/// \param[in]  colTxt:       Color for the text
/// \param[in]  colFrame:     Color for the frame
/// \param[in]  colFill:      Color for the fill
/// \param[in]  colFrameGlow: Color for the frame when glowing
/// \param[in]  colFillGlow:  Color for the fill when glowing
/// \param[in]  nAlignTxt:    Text alignment
/// \param[in]  bFrameEn:     True if framed, false otherwise
/// \param[in]  bFillEn:      True if filled, false otherwise
/// \param[in]  callFunc:     Callback function for button press
/// \param[in]  extraData:    Ptr to extended data structure
///


#if (GSLC_USE_PROGMEM)

#define gslc_ElemCreateTxt_P(pGui,nElemId,nPage,nX,nY,nW,nH,strTxt,pFont,colTxt,colFrame,colFill,nAlignTxt,bFrameEn,bFillEn) \
  static const char str##nElemId[] PROGMEM = strTxt;              \
  static const uint16_t nFeatures##nElemId = GSLC_ELEM_FEA_VALID | \
    (bFrameEn?GSLC_ELEM_FEA_FRAME_EN:0) | (bFillEn?GSLC_ELEM_FEA_FILL_EN:0); \
  static const gslc_tsElem sElem##nElemId PROGMEM = {             \
      nElemId,                                                    \
      nFeatures##nElemId,                                         \
      GSLC_TYPE_TXT,                                              \
      (gslc_tsRect){nX,nY,nW,nH},                                 \
      GSLC_GROUP_ID_NONE,                                         \
      colFrame,colFill,GSLC_COL_BLACK,GSLC_COL_BLACK,             \
      (gslc_tsImgRef){NULL,NULL,GSLC_IMGREF_NONE,NULL},           \
      (gslc_tsImgRef){NULL,NULL,GSLC_IMGREF_NONE,NULL},           \
      NULL,                                                       \
      (char*)str##nElemId,                                        \
      0,                                                          \
      (gslc_teTxtFlags)(GSLC_TXT_MEM_PROG | GSLC_TXT_ALLOC_EXT),  \
      colTxt,                                                     \
      colTxt,                                                     \
      nAlignTxt,                                                  \
      0,                                                          \
      0,                                                          \
      pFont,                                                      \
      NULL,                                                       \
      NULL,                                                       \
      NULL,                                                       \
      NULL,                                                       \
      NULL,                                                       \
  };                                                              \
  gslc_ElemAdd(pGui,nPage,(gslc_tsElem*)&sElem##nElemId,          \
    (gslc_teElemRefFlags)(GSLC_ELEMREF_SRC_PROG | GSLC_ELEMREF_VISIBLE | GSLC_ELEMREF_REDRAW_FULL));


#define gslc_ElemCreateTxt_P_R(pGui,nElemId,nPage,nX,nY,nW,nH,strTxt,strLength,pFont,colTxt,colFrame,colFill,nAlignTxt,bFrameEn,bFillEn) \
  static const uint16_t nFeatures##nElemId = GSLC_ELEM_FEA_VALID | \
    (bFrameEn?GSLC_ELEM_FEA_FRAME_EN:0) | (bFillEn?GSLC_ELEM_FEA_FILL_EN:0); \
  static const gslc_tsElem sElem##nElemId PROGMEM = {             \
      nElemId,                                                    \
      nFeatures##nElemId,                                         \
      GSLC_TYPE_TXT,                                              \
      (gslc_tsRect){nX,nY,nW,nH},                                 \
      GSLC_GROUP_ID_NONE,                                         \
      colFrame,colFill,GSLC_COL_BLACK,GSLC_COL_BLACK,             \
      (gslc_tsImgRef){NULL,NULL,GSLC_IMGREF_NONE,NULL},           \
      (gslc_tsImgRef){NULL,NULL,GSLC_IMGREF_NONE,NULL},           \
      NULL,                                                       \
      (char*)strTxt,                                              \
      strLength,                                                  \
      (gslc_teTxtFlags)(GSLC_TXT_MEM_RAM | GSLC_TXT_ALLOC_EXT),   \
      colTxt,                                                     \
      colTxt,                                                     \
      nAlignTxt,                                                  \
      0,                                                          \
      0,                                                          \
      pFont,                                                      \
      NULL,                                                       \
      NULL,                                                       \
      NULL,                                                       \
      NULL,                                                       \
      NULL,                                                       \
  };                                                              \
  gslc_ElemAdd(pGui,nPage,(gslc_tsElem*)&sElem##nElemId,          \
    (gslc_teElemRefFlags)(GSLC_ELEMREF_SRC_PROG | GSLC_ELEMREF_VISIBLE | GSLC_ELEMREF_REDRAW_FULL));

#define gslc_ElemCreateTxt_P_R_ext(pGui,nElemId,nPage,nX,nY,nW,nH,strTxt,strLength,pFont,colTxt,colTxtGlow,colFrame,colFill,nAlignTxt,nMarginX,nMarginY,bFrameEn,bFillEn,bClickEn,bGlowEn,pfuncXEvent,pfuncXDraw,pfuncXTouch,pfuncXTick) \
  static const uint16_t nFeatures##nElemId = GSLC_ELEM_FEA_VALID | \
    (bFrameEn?GSLC_ELEM_FEA_FRAME_EN:0) | (bFillEn?GSLC_ELEM_FEA_FILL_EN:0) | (bClickEn?GSLC_ELEM_FEA_CLICK_EN:0) | (bGlowEn?GSLC_ELEM_FEA_GLOW_EN:0); \
  static const gslc_tsElem sElem##nElemId PROGMEM = {             \
      nElemId,                                                    \
      nFeatures##nElemId,                                         \
      GSLC_TYPE_TXT,                                              \
      (gslc_tsRect){nX,nY,nW,nH},                                 \
      GSLC_GROUP_ID_NONE,                                         \
      colFrame,colFill,GSLC_COL_BLACK,GSLC_COL_BLACK,             \
      (gslc_tsImgRef){NULL,NULL,GSLC_IMGREF_NONE,NULL},           \
      (gslc_tsImgRef){NULL,NULL,GSLC_IMGREF_NONE,NULL},           \
      NULL,                                                       \
      (char*)strTxt,                                              \
      strLength,                                                  \
      (gslc_teTxtFlags)(GSLC_TXT_MEM_RAM | GSLC_TXT_ALLOC_EXT),   \
      colTxt,                                                     \
      colTxtGlow,                                                 \
      nAlignTxt,                                                  \
      nMarginX,                                                   \
      nMarginY,                                                   \
      pFont,                                                      \
      NULL,                                                       \
      pfuncXEvent,                                                \
      pfuncXDraw,                                                 \
      pfuncXTouch,                                                \
      pfuncXTick,                                                 \
  };                                                              \
  gslc_ElemAdd(pGui,nPage,(gslc_tsElem*)&sElem##nElemId,          \
    (gslc_teElemRefFlags)(GSLC_ELEMREF_SRC_PROG | GSLC_ELEMREF_VISIBLE | GSLC_ELEMREF_REDRAW_FULL));



#define gslc_ElemCreateBox_P(pGui,nElemId,nPage,nX,nY,nW,nH,colFrame,colFill,bFrameEn,bFillEn,pfuncXDraw,pfuncXTick) \
  static const uint16_t nFeatures##nElemId = GSLC_ELEM_FEA_VALID | \
    (bFrameEn?GSLC_ELEM_FEA_FRAME_EN:0) | (bFillEn?GSLC_ELEM_FEA_FILL_EN:0); \
  static const gslc_tsElem sElem##nElemId PROGMEM = {             \
      nElemId,                                                    \
      nFeatures##nElemId,                                         \
      GSLC_TYPE_BOX,                                              \
      (gslc_tsRect){nX,nY,nW,nH},                                 \
      GSLC_GROUP_ID_NONE,                                         \
      colFrame,colFill,GSLC_COL_BLACK,GSLC_COL_BLACK,             \
      (gslc_tsImgRef){NULL,NULL,GSLC_IMGREF_NONE,NULL},           \
      (gslc_tsImgRef){NULL,NULL,GSLC_IMGREF_NONE,NULL},           \
      NULL,                                                       \
      NULL,                                                       \
      0,                                                          \
      GSLC_TXT_DEFAULT,                                           \
      GSLC_COL_WHITE,                                             \
      GSLC_COL_WHITE,                                             \
      GSLC_ALIGN_MID_MID,                                         \
      0,                                                          \
      0,                                                          \
      NULL,                                                       \
      NULL,                                                       \
      NULL,                                                       \
      pfuncXDraw,                                                 \
      NULL,                                                       \
      pfuncXTick,                                                 \
  };                                                              \
  gslc_ElemAdd(pGui,nPage,(gslc_tsElem*)&sElem##nElemId,          \
    (gslc_teElemRefFlags)(GSLC_ELEMREF_SRC_PROG | GSLC_ELEMREF_VISIBLE | GSLC_ELEMREF_REDRAW_FULL));

#define gslc_ElemCreateLine_P(pGui,nElemId,nPage,nX0,nY0,nX1,nY1,colFill) \
  static const uint16_t nFeatures##nElemId = GSLC_ELEM_FEA_VALID;  \
  static const gslc_tsElem sElem##nElemId PROGMEM = {             \
      nElemId,                                                    \
      nFeatures##nElemId,                                         \
      GSLC_TYPE_LINE,                                             \
      (gslc_tsRect){nX0,nY0,nX1-nX0+1,nY1-nY0+1},                 \
      GSLC_GROUP_ID_NONE,                                         \
      colFill,colFill,GSLC_COL_BLACK,GSLC_COL_BLACK,              \
      (gslc_tsImgRef){NULL,NULL,GSLC_IMGREF_NONE,NULL},           \
      (gslc_tsImgRef){NULL,NULL,GSLC_IMGREF_NONE,NULL},           \
      NULL,                                                       \
      NULL,                                                       \
      0,                                                          \
      GSLC_TXT_DEFAULT,                                           \
      GSLC_COL_WHITE,                                             \
      GSLC_COL_WHITE,                                             \
      GSLC_ALIGN_MID_MID,                                         \
      0,                                                          \
      0,                                                          \
      NULL,                                                       \
      NULL,                                                       \
      NULL,                                                       \
      NULL,                                                       \
      NULL,                                                       \
      NULL,                                                       \
  };                                                              \
  gslc_ElemAdd(pGui,nPage,(gslc_tsElem*)&sElem##nElemId,          \
    (gslc_teElemRefFlags)(GSLC_ELEMREF_SRC_PROG | GSLC_ELEMREF_VISIBLE | GSLC_ELEMREF_REDRAW_FULL));

#define gslc_ElemCreateBtnTxt_P(pGui,nElemId,nPage,nX,nY,nW,nH,strTxt,pFont,colTxt,colFrame,colFill,colFrameGlow,colFillGlow,nAlignTxt,bFrameEn,bFillEn,callFunc,extraData) \
  static const char str##nElemId[] PROGMEM = strTxt;              \
  static const uint16_t nFeatures##nElemId = GSLC_ELEM_FEA_VALID | \
    GSLC_ELEM_FEA_CLICK_EN | GSLC_ELEM_FEA_GLOW_EN |              \
    (bFrameEn?GSLC_ELEM_FEA_FRAME_EN:0) | (bFillEn?GSLC_ELEM_FEA_FILL_EN:0); \
  static const gslc_tsElem sElem##nElemId PROGMEM = {             \
      nElemId,                                                    \
      nFeatures##nElemId,                                         \
      GSLC_TYPE_BTN,                                              \
      (gslc_tsRect){nX,nY,nW,nH},                                 \
      GSLC_GROUP_ID_NONE,                                         \
      colFrame,colFill,colFrameGlow,colFillGlow,                  \
      (gslc_tsImgRef){NULL,NULL,GSLC_IMGREF_NONE,NULL},           \
      (gslc_tsImgRef){NULL,NULL,GSLC_IMGREF_NONE,NULL},           \
      NULL,                                                       \
      (char*)str##nElemId,                                        \
      0,                                                          \
      (gslc_teTxtFlags)(GSLC_TXT_MEM_PROG | GSLC_TXT_ALLOC_EXT),  \
      colTxt,                                                     \
      colTxt,                                                     \
      nAlignTxt,                                                  \
      0,                                                          \
      0,                                                          \
      pFont,                                                      \
      (void*)extraData,                                           \
      NULL,                                                       \
      NULL,                                                       \
      callFunc,                                                   \
      NULL,                                                       \
  };                                                              \
  gslc_ElemAdd(pGui,nPage,(gslc_tsElem*)&sElem##nElemId,          \
    (gslc_teElemRefFlags)(GSLC_ELEMREF_SRC_PROG | GSLC_ELEMREF_VISIBLE | GSLC_ELEMREF_REDRAW_FULL));

#define gslc_ElemCreateBtnTxt_P_R(pGui,nElemId,nPage,nX,nY,nW,nH,strTxt,strLength,pFont,colTxt,colFrame,colFill,colFrameGlow,colFillGlow,nAlignTxt,bFrameEn,bFillEn,callFunc,extraData) \
  static const uint16_t nFeatures##nElemId = GSLC_ELEM_FEA_VALID | \
    GSLC_ELEM_FEA_CLICK_EN | GSLC_ELEM_FEA_GLOW_EN |              \
    (bFrameEn?GSLC_ELEM_FEA_FRAME_EN:0) | (bFillEn?GSLC_ELEM_FEA_FILL_EN:0); \
  static const gslc_tsElem sElem##nElemId PROGMEM = {             \
      nElemId,                                                    \
      nFeatures##nElemId,                                         \
      GSLC_TYPE_BTN,                                              \
      (gslc_tsRect){nX,nY,nW,nH},                                 \
      GSLC_GROUP_ID_NONE,                                         \
      colFrame,colFill,colFrameGlow,colFillGlow,                  \
      (gslc_tsImgRef){NULL,NULL,GSLC_IMGREF_NONE,NULL},           \
      (gslc_tsImgRef){NULL,NULL,GSLC_IMGREF_NONE,NULL},           \
      NULL,                                                       \
      (char*)strTxt,                                              \
      strLength,                                                          \
      (gslc_teTxtFlags)(GSLC_TXT_MEM_RAM | GSLC_TXT_ALLOC_EXT),   \
      colTxt,                                                     \
      colTxt,                                                     \
      nAlignTxt,                                                  \
      0,                                                          \
      0,                                                          \
      pFont,                                                      \
      (void*)extraData,                                           \
      NULL,                                                       \
      NULL,                                                       \
      callFunc,                                                   \
      NULL,                                                       \
  };                                                              \
  gslc_ElemAdd(pGui,nPage,(gslc_tsElem*)&sElem##nElemId,          \
    (gslc_teElemRefFlags)(GSLC_ELEMREF_SRC_PROG | GSLC_ELEMREF_VISIBLE | GSLC_ELEMREF_REDRAW_FULL));



#else

#define gslc_ElemCreateTxt_P(pGui,nElemId,nPage,nX,nY,nW,nH,strTxt,pFont,colTxt,colFrame,colFill,nAlignTxt,bFrameEn,bFillEn) \
  static const char str##nElemId[] = strTxt;                      \
  static const uint16_t nFeatures##nElemId = GSLC_ELEM_FEA_VALID | \
    (bFrameEn?GSLC_ELEM_FEA_FRAME_EN:0) | (bFillEn?GSLC_ELEM_FEA_FILL_EN:0); \
  static const gslc_tsElem sElem##nElemId = {                     \
      nElemId,                                                    \
      nFeatures##nElemId,                                         \
      GSLC_TYPE_TXT,                                              \
      (gslc_tsRect){nX,nY,nW,nH},                                 \
      GSLC_GROUP_ID_NONE,                                         \
      colFrame,colFill,GSLC_COL_BLACK,GSLC_COL_BLACK,             \
      (gslc_tsImgRef){NULL,NULL,GSLC_IMGREF_NONE,NULL},           \
      (gslc_tsImgRef){NULL,NULL,GSLC_IMGREF_NONE,NULL},           \
      NULL,                                                       \
      (char*)str##nElemId,                                        \
      0,                                                          \
      (gslc_teTxtFlags)(GSLC_TXT_MEM_RAM | GSLC_TXT_ALLOC_EXT),  \
      colTxt,                                                     \
      colTxt,                                                     \
      nAlignTxt,                                                  \
      0,                                                          \
      0,                                                          \
      pFont,                                                      \
      NULL,                                                       \
      NULL,                                                       \
      NULL,                                                       \
      NULL,                                                       \
      NULL,                                                       \
  };                                                              \
  gslc_ElemAdd(pGui,nPage,(gslc_tsElem*)&sElem##nElemId,          \
    (gslc_teElemRefFlags)(GSLC_ELEMREF_SRC_CONST | GSLC_ELEMREF_VISIBLE | GSLC_ELEMREF_REDRAW_FULL));


#define gslc_ElemCreateTxt_P_R(pGui,nElemId,nPage,nX,nY,nW,nH,strTxt,strLength,pFont,colTxt,colFrame,colFill,nAlignTxt,bFrameEn,bFillEn) \
  static const uint16_t nFeatures##nElemId = GSLC_ELEM_FEA_VALID | \
    (bFrameEn?GSLC_ELEM_FEA_FRAME_EN:0) | (bFillEn?GSLC_ELEM_FEA_FILL_EN:0); \
  static const gslc_tsElem sElem##nElemId = {                     \
      nElemId,                                                    \
      nFeatures##nElemId,                                         \
      GSLC_TYPE_TXT,                                              \
      (gslc_tsRect){nX,nY,nW,nH},                                 \
      GSLC_GROUP_ID_NONE,                                         \
      colFrame,colFill,GSLC_COL_BLACK,GSLC_COL_BLACK,             \
      (gslc_tsImgRef){NULL,NULL,GSLC_IMGREF_NONE,NULL},           \
      (gslc_tsImgRef){NULL,NULL,GSLC_IMGREF_NONE,NULL},           \
      NULL,                                                       \
      (char*)strTxt,                                              \
      strLength,                                                  \
      (gslc_teTxtFlags)(GSLC_TXT_MEM_RAM | GSLC_TXT_ALLOC_EXT),   \
      colTxt,                                                     \
      colTxt,                                                     \
      nAlignTxt,                                                  \
      0,                                                          \
      0,                                                          \
      pFont,                                                      \
      NULL,                                                       \
      NULL,                                                       \
      NULL,                                                       \
      NULL,                                                       \
      NULL,                                                       \
  };                                                              \
  gslc_ElemAdd(pGui,nPage,(gslc_tsElem*)&sElem##nElemId,          \
    (gslc_teElemRefFlags)(GSLC_ELEMREF_SRC_CONST | GSLC_ELEMREF_VISIBLE | GSLC_ELEMREF_REDRAW_FULL));

#define gslc_ElemCreateTxt_P_R_ext(pGui,nElemId,nPage,nX,nY,nW,nH,strTxt,strLength,pFont,colTxt,colTxtGlow,colFrame,colFill,nAlignTxt,nMarginX,nMarginY,bFrameEn,bFillEn,bClickEn,bGlowEn,pfuncXEvent,pfuncXDraw,pfuncXTouch,pfuncXTick) \
  static const uint16_t nFeatures##nElemId = GSLC_ELEM_FEA_VALID | \
    (bFrameEn?GSLC_ELEM_FEA_FRAME_EN:0) | (bFillEn?GSLC_ELEM_FEA_FILL_EN:0) | (bClickEn?GSLC_ELEM_FEA_CLICK_EN:0) | (bGlowEn?GSLC_ELEM_FEA_GLOW_EN:0); \
  static const gslc_tsElem sElem##nElemId = {                     \
      nElemId,                                                    \
      nFeatures##nElemId,                                         \
      GSLC_TYPE_TXT,                                              \
      (gslc_tsRect){nX,nY,nW,nH},                                 \
      GSLC_GROUP_ID_NONE,                                         \
      colFrame,colFill,GSLC_COL_BLACK,GSLC_COL_BLACK,             \
      (gslc_tsImgRef){NULL,NULL,GSLC_IMGREF_NONE,NULL},           \
      (gslc_tsImgRef){NULL,NULL,GSLC_IMGREF_NONE,NULL},           \
      NULL,                                                       \
      (char*)strTxt,                                              \
      strLength,                                                  \
      (gslc_teTxtFlags)(GSLC_TXT_MEM_RAM | GSLC_TXT_ALLOC_EXT),   \
      colTxt,                                                     \
      colTxtGlow,                                                 \
      nAlignTxt,                                                  \
      nMarginX,                                                   \
      nMarginY,                                                   \
      pFont,                                                      \
      NULL,                                                       \
      pfuncXEvent,                                                \
      pfuncXDraw,                                                 \
      pfuncXTouch,                                                \
      pfuncXTick,                                                 \
  };                                                              \
  gslc_ElemAdd(pGui,nPage,(gslc_tsElem*)&sElem##nElemId,          \
    (gslc_teElemRefFlags)(GSLC_ELEMREF_SRC_CONST | GSLC_ELEMREF_VISIBLE | GSLC_ELEMREF_REDRAW_FULL));


#define gslc_ElemCreateBox_P(pGui,nElemId,nPage,nX,nY,nW,nH,colFrame,colFill,bFrameEn,bFillEn,pfuncXDraw,pfuncXTick) \
  static const uint16_t nFeatures##nElemId = GSLC_ELEM_FEA_VALID | \
    (bFrameEn?GSLC_ELEM_FEA_FRAME_EN:0) | (bFillEn?GSLC_ELEM_FEA_FILL_EN:0); \
  static const gslc_tsElem sElem##nElemId = {                     \
      nElemId,                                                    \
      nFeatures##nElemId,                                         \
      GSLC_TYPE_BOX,                                              \
      (gslc_tsRect){nX,nY,nW,nH},                                 \
      GSLC_GROUP_ID_NONE,                                         \
      colFrame,colFill,GSLC_COL_BLACK,GSLC_COL_BLACK,             \
      (gslc_tsImgRef){NULL,NULL,GSLC_IMGREF_NONE,NULL},           \
      (gslc_tsImgRef){NULL,NULL,GSLC_IMGREF_NONE,NULL},           \
      NULL,                                                       \
      NULL,                                                       \
      0,                                                          \
      GSLC_TXT_DEFAULT,                                           \
      GSLC_COL_WHITE,                                             \
      GSLC_COL_WHITE,                                             \
      GSLC_ALIGN_MID_MID,                                         \
      0,                                                          \
      0,                                                          \
      NULL,                                                       \
      NULL,                                                       \
      NULL,                                                       \
      pfuncXDraw,                                                 \
      NULL,                                                       \
      pfuncXTick,                                                 \
  };                                                              \
  gslc_ElemAdd(pGui,nPage,(gslc_tsElem*)&sElem##nElemId,          \
    (gslc_teElemRefFlags)(GSLC_ELEMREF_SRC_CONST | GSLC_ELEMREF_VISIBLE | GSLC_ELEMREF_REDRAW_FULL));

#define gslc_ElemCreateLine_P(pGui,nElemId,nPage,nX0,nY0,nX1,nY1,colFill) \
  static const uint16_t nFeatures##nElemId = GSLC_ELEM_FEA_VALID;  \
  static const gslc_tsElem sElem##nElemId = {                     \
      nElemId,                                                    \
      nFeatures##nElemId,                                         \
      GSLC_TYPE_LINE,                                             \
      (gslc_tsRect){nX0,nY0,nX1-nX0+1,nY1-nY0+1},                 \
      GSLC_GROUP_ID_NONE,                                         \
      colFill,colFill,GSLC_COL_BLACK,GSLC_COL_BLACK,              \
      (gslc_tsImgRef){NULL,NULL,GSLC_IMGREF_NONE,NULL},           \
      (gslc_tsImgRef){NULL,NULL,GSLC_IMGREF_NONE,NULL},           \
      NULL,                                                       \
      NULL,                                                       \
      0,                                                          \
      GSLC_TXT_DEFAULT,                                           \
      GSLC_COL_WHITE,                                             \
      GSLC_COL_WHITE,                                             \
      GSLC_ALIGN_MID_MID,                                         \
      0,                                                          \
      0,                                                          \
      NULL,                                                       \
      NULL,                                                       \
      NULL,                                                       \
      NULL,                                                       \
      NULL,                                                       \
      NULL,                                                       \
  };                                                              \
  gslc_ElemAdd(pGui,nPage,(gslc_tsElem*)&sElem##nElemId,          \
    (gslc_teElemRefFlags)(GSLC_ELEMREF_SRC_CONST | GSLC_ELEMREF_VISIBLE | GSLC_ELEMREF_REDRAW_FULL));

#define gslc_ElemCreateBtnTxt_P(pGui,nElemId,nPage,nX,nY,nW,nH,strTxt,pFont,colTxt,colFrame,colFill,colFrameGlow,colFillGlow,nAlignTxt,bFrameEn,bFillEn,callFunc,extraData) \
  static const char str##nElemId[] = strTxt;                      \
  static const uint16_t nFeatures##nElemId = GSLC_ELEM_FEA_VALID | \
    GSLC_ELEM_FEA_CLICK_EN | GSLC_ELEM_FEA_GLOW_EN |              \
    (bFrameEn?GSLC_ELEM_FEA_FRAME_EN:0) | (bFillEn?GSLC_ELEM_FEA_FILL_EN:0); \
  static const gslc_tsElem sElem##nElemId = {                     \
      nElemId,                                                    \
      nFeatures##nElemId,                                         \
      GSLC_TYPE_BTN,                                              \
      (gslc_tsRect){nX,nY,nW,nH},                                 \
      GSLC_GROUP_ID_NONE,                                         \
      colFrame,colFill,colFrameGlow,colFillGlow,                  \
      (gslc_tsImgRef){NULL,NULL,GSLC_IMGREF_NONE,NULL},           \
      (gslc_tsImgRef){NULL,NULL,GSLC_IMGREF_NONE,NULL},           \
      NULL,                                                       \
      (char*)str##nElemId,                                        \
      0,                                                          \
      (gslc_teTxtFlags)(GSLC_TXT_MEM_RAM | GSLC_TXT_ALLOC_EXT),  \
      colTxt,                                                     \
      colTxt,                                                     \
      nAlignTxt,                                                  \
      0,                                                          \
      0,                                                          \
      pFont,                                                      \
      (void*)extraData,                                           \
      NULL,                                                       \
      NULL,                                                       \
      callFunc,                                                   \
      NULL,                                                       \
  };                                                              \
  gslc_ElemAdd(pGui,nPage,(gslc_tsElem*)&sElem##nElemId,          \
    (gslc_teElemRefFlags)(GSLC_ELEMREF_SRC_CONST | GSLC_ELEMREF_VISIBLE | GSLC_ELEMREF_REDRAW_FULL));


#endif // GSLC_USE_PROGMEM



// ========================================================================
/// @}
/// \defgroup _Int_ Internal Functions
/// These functions are internal to the GUIslice implementation and are not
/// intended to be called by user code and subject to change even in minor
/// releases.
/// - The following functions are generally not required for
///   typical users of GUIslice. However, for advanced usage
///   more direct access may be required.
/// @{
// ========================================================================

// ------------------------------------------------------------------------
/// \defgroup _IntMisc_ Internal: Misc Functions
/// @{
// ------------------------------------------------------------------------

///
/// Create a blank image reference structure
///
/// \return Image reference struct
///
gslc_tsImgRef gslc_ResetImage();


// ------------------------------------------------------------------------
/// @}
/// \defgroup _IntElem_ Internal: Element Functions
/// @{
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
/// \param[in]  pStrBuf:      String to copy into element
/// \param[in]  nStrBufMax:   Maximum length of string buffer (pStrBuf). Only applicable
///                           if GSLC_LOCAL_STR=0. Ignored if GSLC_LOCAL_STR=1.)
/// \param[in]  nFontId:      Font ID for textual elements
///
/// \return Initialized structure
///
gslc_tsElem gslc_ElemCreate(gslc_tsGui* pGui,int16_t nElemId,int16_t nPageId,int16_t nType,
  gslc_tsRect rElem,char* pStrBuf,uint8_t nStrBufMax,int16_t nFontId);


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
/// \param[in]  eFlags:      Flags describing the element (eg. whether the
///                          element should be stored in internal RAM array
///                          or is located in Flash/PROGMEM).
///
/// \return Pointer to Element reference or NULL if fail
///
gslc_tsElemRef* gslc_ElemAdd(gslc_tsGui* pGui,int16_t nPageId,gslc_tsElem* pElem,gslc_teElemRefFlags eFlags);


///
/// Get the flags associated with an element reference
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Element reference pointer
/// \param[in]  nFlagMask:   Flags to read
///
/// \return Values associated with the element reference flags (subject to the flag mask)
///
uint8_t gslc_GetElemRefFlag(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,uint8_t nFlagMask);

///
/// Set the flags associated with an element reference
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Element reference pointer
/// \param[in]  nFlagMask:   Flags to read
/// \param[in]  nFlagVal:    Values to assign to masked flags
///
/// \return none
///
void gslc_SetElemRefFlag(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,uint8_t nFlagMask,uint8_t nFlagVal);


/// Returns a pointer to an element from an element reference, copying
/// from FLASH to RAM if element is stored in PROGMEM. This function
/// enables all APIs to work with Elements irrespective of whether they
/// were created in RAM or Flash.
///
/// \param[in]  pGui:         Pointer to GUI
/// \param[in]  pElemRef:     Pointer to Element Reference
///
/// \return Pointer to Element after ensuring that it is accessible from RAM
///
// TODO: Mark this as public API
gslc_tsElem* gslc_GetElemFromRef(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef);


/// Returns a pointer to an element from an element reference.
/// This is a wrapper for GetElemFromRef() including debug checking
/// for invalid pointers.
///
/// \param[in]  pGui:         Pointer to GUI
/// \param[in]  pElemRef:     Pointer to Element Reference
/// \param[in]  nLineNum:     Line number from calling function (ie. __LINE__)
///
/// \return Pointer to Element after ensuring that it is accessible from RAM
///
gslc_tsElem* gslc_GetElemFromRefD(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, int16_t nLineNum);

/// Returns a pointer to the data structure associated with an extended element.
/// - Example usage:
///   gslc_tsXListbox* pListbox = (gslc_tsXListbox*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_LISTBOX, __LINE__);
///
/// \param[in]  pGui:         Pointer to GUI
/// \param[in]  pElemRef:     Pointer to Element Reference
/// \param[in]  nType:        Expected type indicator (ie. GSLC_TYPEX_*)
/// \param[in]  nLineNum:     Line number from calling function (ie. __LINE__)
///
/// \return Void pointer to extended data (pXData), or NULL if error. Needs to be typecasted accordingly.
///
void* gslc_GetXDataFromRef(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, int16_t nType, int16_t nLineNum);


///
/// Set an element to use a bitmap image
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Pointer to Element reference to update
/// \param[in]  sImgRef:     Image reference (normal state)
/// \param[in]  sImgRefSel:  Image reference (glowing state)
///
/// \return none
///
void gslc_ElemSetImage(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,gslc_tsImgRef sImgRef,
  gslc_tsImgRef sImgRefSel);



/// Draw an element to the active display
/// - Element is referenced by an element pointer
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElemRef:    Ptr to Element reference to draw
/// \param[in]  eRedraw:     Redraw mode
///
/// \return true if success, false otherwise
///
bool gslc_ElemDrawByRef(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,gslc_teRedrawType eRedraw);


///
/// Draw an element to the active display
/// - Element is referenced by a page ID and element ID
/// - Provides similar functionality as ElemDrawByRef() but accepts page and element IDs
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nPageId:     ID of page containing element
/// \param[in]  nElemId:     ID of element
///
/// \return none
///
void gslc_ElemDraw(gslc_tsGui* pGui,int16_t nPageId,int16_t nElemId);


///
/// Draw text with full text justification
/// - This function is usually only required by internal GUIslice rendering
///   operations but is made available for custom element usage as well
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pStrBuf:     Pointer to text string buffer
/// \param[in]  rTxt:        Rectangle region to contain the text
/// \param[in]  pTxtFont:    Pointer to the font
/// \param[in]  eTxtFlags:   Text string attributes
/// \param[in]  eTxtAlign:   Text alignment / justification mode
/// \param[in]  colTxt:      Text foreground color
/// \param[in]  colBg:       Text background color
/// \param[in]  nMarginW:    Horizontal margin within rect region to keep text away
/// \param[in]  nMarginH:    Vertical margin within rect region to keep text away
///
/// \return none
///
void gslc_DrawTxtBase(gslc_tsGui* pGui, char* pStrBuf, gslc_tsRect rTxt, gslc_tsFont* pTxtFont, gslc_teTxtFlags eTxtFlags,
  int8_t eTxtAlign, gslc_tsColor colTxt, gslc_tsColor colBg, int16_t nMarginW, int16_t nMarginH);


///
/// Set the global rounded radius
/// - Used for rounded rectangles
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nRadius:     Radius for rounded elements
///
/// \return none
///
void gslc_SetRoundRadius(gslc_tsGui* pGui, uint8_t nRadius);

///
/// Set the global focus color choices
/// - These colors will be used when depicting focus frames
///   around elements
///
/// \param[in]  pGui:         Pointer to GUI
/// \param[in]  colFocusNone: The color to use when the element is not in focus.
///                           This is typically set to match the background color.
/// \param[in]  colFocus:     The color to use when the element is in focus.
/// \param[in]  colFocusEdit: The color to use when the element is in edit mode.
///
/// \return none
///
void gslc_SetFocusCol(gslc_tsGui* pGui,gslc_tsColor colFocusNone,gslc_tsColor colFocus,gslc_tsColor colFocusEdit);

// ------------------------------------------------------------------------
/// @}
/// \defgroup _IntPage_ Internal: Page Functions
/// @{
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

/* UNUSED
///
/// Assign the event callback function for a page
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pPage:       Pointer to page
/// \param[in]  funcCb:      Function pointer to event routine (or NULL for default))
///
/// \return none
///
void gslc_PageSetEventFunc(gslc_tsGui* pGui,gslc_tsPage* pPage,GSLC_CB_EVENT funcCb);
*/

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

/// Find a page in the GUI by its ID
///
/// \param[in]  pGui:         Pointer to GUI
/// \param[in]  nPageId:      Page ID to search
///
/// \return Ptr to a page or NULL if none found
///
gslc_tsPage* gslc_PageFindById(gslc_tsGui* pGui,int16_t nPageId);

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
/// \internal
void gslc_PageRedrawCalc(gslc_tsGui* pGui);


///
/// Create an event structure
///
/// \param[in]  pGui:       Pointer to GUI
/// \param[in]  eType:      Event type (draw, touch, tick, etc.)
/// \param[in]  nSubType:   Refinement of event type (or 0 if unused)
/// \param[in]  pvScope:    Void ptr to object receiving event so that
///                         the event handler will have the context
/// \param[in]  pvData:     Void ptr to additional data associated with
///                         the event (eg. coordinates for touch events)
///
/// \return None
///
gslc_tsEvent gslc_EventCreate(gslc_tsGui* pGui,gslc_teEventType eType,uint8_t nSubType,void* pvScope,void* pvData);


///
/// Common event handler function for an element
///
/// \param[in]  pvGui:       Void pointer to GUI
/// \param[in]  sEvent:      Event data structure
///
/// \return true if success, false if fail
///
bool gslc_ElemEvent(void* pvGui,gslc_tsEvent sEvent);


#if !defined(DRV_TOUCH_NONE)
///
/// Trigger an element's touch event. This is an optional
/// behavior useful in some extended element types.
///
/// \param[in]  pGui:             Pointer to GUI
/// \param[in]  pElemRefTracked:  Pointer to tracked Element reference (or NULL for none))
/// \param[in]  eTouch:           Touch event type
/// \param[in]  nX:               X coordinate of event (absolute coordinate)
/// \param[in]  nY:               Y coordinate of event (absolute coordinate)
///
/// \return true if success, false if error
///
bool gslc_ElemSendEventTouch(gslc_tsGui* pGui,gslc_tsElemRef* pElemRefTracked,
        gslc_teTouch eTouch,int16_t nX,int16_t nY);
#endif // !DRV_TOUCH_NONE


// ------------------------------------------------------------------------
// Internal: Element Collection Functions
// ------------------------------------------------------------------------
/// @}
/// \defgroup _IntCollect_ Internal: Element Collection Functions
/// @{


/// Reset the members of an element collection
///
/// \param[in]  pCollect:     Pointer to the collection
/// \param[in]  asElem:       Internal element array storage to associate with the
///                           collection
/// \param[in]  nElemMax:     Maximum number of elements that can be added to the
///                           internal element array (ie. RAM))
/// \param[in]  asElemRef:    Internal element reference array storage to
///                           associate with the collection. All elements, whether they
///                           are located in the internal element array or in
///                           external Flash (PROGMEM) storage, require an entry
///                           in the element reference array.
/// \param[in]  nElemRefMax:  Maximum number of elements in the reference array.
///                           This is effectively the maximum number of elements
///                           that can appear in the collection, irrespective of whether
///                           it is stored in RAM or Flash (PROGMEM).
///
/// \return none
///
void gslc_CollectReset(gslc_tsCollect* pCollect,gslc_tsElem* asElem,uint16_t nElemMax,
        gslc_tsElemRef* asElemRef,uint16_t nElemRefMax);


/// Add an element to a collection
/// - Note that the contents of pElem are copied to the collection's
///   element array so the pElem pointer can be discarded are the
///   call is complete.
///
/// \param[in]  pGui:         Pointer to GUI
/// \param[in]  pCollect:     Pointer to the collection
/// \param[in]  pElem:        Ptr to the element to add
/// \param[in]  eFlags:       Flags describing the element (eg. whether the
///                           element should be stored in internal RAM array
///                           or is located in Flash/PROGMEM).
///
/// \return Pointer to the element reference in the collection that has been added
///         or NULL if there was an error
///
gslc_tsElemRef* gslc_CollectElemAdd(gslc_tsGui* pGui,gslc_tsCollect* pCollect,const gslc_tsElem* pElem,gslc_teElemRefFlags eFlags);


///
/// Determine if any elements in a collection need redraw
///
/// \param[in]  pGui:         Pointer to GUI
/// \param[in]  pCollect:     Pointer to Element collection
///
/// \return True if redraw required, false otherwise
///
bool gslc_CollectGetRedraw(gslc_tsGui* pGui,gslc_tsCollect* pCollect);


/// Find an element in a collection by its Element ID
///
/// \param[in]  pGui:         Pointer to GUI
/// \param[in]  pCollect:     Pointer to the collection
/// \param[in]  nElemId:      Element ID to search for
///
/// \return Pointer to the element reference in the collection that was found
///         or NULL if no matches found
///
gslc_tsElemRef* gslc_CollectFindElemById(gslc_tsGui* pGui,gslc_tsCollect* pCollect,int16_t nElemId);


/// Find an element in a collection by a coordinate coordinate
/// - A match is found if the element is "clickable" (bClickEn=true)
///   and the coordinate falls within the element's bounds (rElem).
///
/// \param[in]  pGui:         Pointer to GUI
/// \param[in]  pCollect:     Pointer to the collection
/// \param[in]  nX:           Absolute X coordinate to use for search
/// \param[in]  nY:           Absolute Y coordinate to use for search
/// \param[out] pnElemInd:    Pointer to element index found
///
/// \return Pointer to the element reference in the collection that was found
///         or NULL if no matches found
///
gslc_tsElemRef* gslc_CollectFindElemFromCoord(gslc_tsGui* pGui,gslc_tsCollect* pCollect,int16_t nX, int16_t nY, int16_t* pnElemInd);


/// Allocate the next available Element ID in a collection
///
/// \param[in]  pGui:         Pointer to GUI
/// \param[in]  pCollect:     Pointer to the collection
///
/// \return Element ID that is reserved for use
///
int gslc_CollectGetNextId(gslc_tsGui* pGui,gslc_tsCollect* pCollect);


/// Get the element within a collection that is currently being tracked
///
/// \param[in]  pGui:         Pointer to GUI
/// \param[in]  pCollect:     Pointer to the collection
///
/// \return Pointer to the element reference in the collection that is
///         currently being tracked or NULL if no elements
///         are being tracked
///
gslc_tsElemRef* gslc_CollectGetElemRefTracked(gslc_tsGui* pGui,gslc_tsCollect* pCollect);


/// Set the element within a collection that is currently being tracked
///
/// \param[in]  pGui:         Pointer to GUI
/// \param[in]  pCollect:     Pointer to the collection
/// \param[in]  pElemRef:     Ptr to element reference to mark as being tracked
/// \param[in]  nElemInd:     Element index to mark as being tracked
///
/// \return none
///
void gslc_CollectSetElemTracked(gslc_tsGui* pGui,gslc_tsCollect* pCollect,gslc_tsElemRef* pElemRef,int16_t nElemInd);


/// Assign the parent element reference to all elements within a collection
/// - This is generally used in the case of compound elements where updates to
///   a sub-element should cause the parent (compound element) to be redrawn
///   as well.)
///
/// \param[in]  pGui:           Pointer to GUI
/// \param[in]  pCollect:       Pointer to the collection
/// \param[in]  pElemRefParent: Ptr to element reference that is the parent
///
/// \return none
///
void gslc_CollectSetParent(gslc_tsGui* pGui,gslc_tsCollect* pCollect,gslc_tsElemRef* pElemRefParent);


// ------------------------------------------------------------------------
// Internal: Collect Event Handlers
// ------------------------------------------------------------------------
/// @}
/// \defgroup _IntCollectEvt_ Internal: Element Collection Event Functions
/// @{

/* UNUSED
///
/// Assign the event callback function for an element collection
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pCollect:    Pointer to collection
/// \param[in]  funcCb:      Function pointer to event routine (or NULL for default))
///
/// \return none
///
void gslc_CollectSetEventFunc(gslc_tsGui* pGui,gslc_tsCollect* pCollect,GSLC_CB_EVENT funcCb);
*/

///
/// Common event handler function for an element collection
///
/// \param[in]  pvGui:       Void pointer to GUI
/// \param[in]  sEvent:      Event data structure
///
/// \return true if success, false if fail
///
bool gslc_CollectEvent(void* pvGui,gslc_tsEvent sEvent);


#if !defined(DRV_TOUCH_NONE)

/// Handle touch events within the element collection
///
/// \param[in]  pGui:         Pointer to the GUI
/// \param[in]  pCollect:     Ptr to the element collection
/// \param[in]  pEventTouch:  Ptr to the touch event structure
///
/// \return none
///
void gslc_CollectTouch(gslc_tsGui* pGui,gslc_tsCollect* pCollect,gslc_tsEventTouch* pEventTouch);

/// Handle dispatch of touch (up,down,move) events to compound elements sub elements
///
/// \param[in]  pvGui:       Void ptr to GUI (typecast to gslc_tsGui*)
/// \param[in]  pvElemRef:   Void ptr to Element Reference(typecast to gslc_tsElemRef*)
/// \param[in]  eTouch:      Touch event type
/// \param[in]  nRelX:       Touch X coord relative to element
/// \param[in]  nRelY:       Touch Y coord relative to element
/// \param[in]  pCollect:    Collection containing sub elements
///
/// \return true if success, false otherwise
///
bool gslc_CollectTouchCompound(void* pvGui, void* pvElemRef, gslc_teTouch eTouch, int16_t nRelX, int16_t nRelY, gslc_tsCollect* pCollect);


/// Handle direct input events within the element collection
///
/// \param[in]  pGui:         Pointer to the GUI
/// \param[in]  pCollect:     Ptr to the element collection
/// \param[in]  pEventTouch:  Ptr to the touch event structure
///
/// \return none
///
void gslc_CollectInput(gslc_tsGui* pGui,gslc_tsCollect* pCollect,gslc_tsEventTouch* pEventTouch);



// ------------------------------------------------------------------------
/// @}
/// \defgroup _IntTrack_ Internal: Tracking Functions
/// @{
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
void gslc_TrackTouch(gslc_tsGui* pGui,gslc_tsPage* pPage,int16_t nX,int16_t nY,uint16_t nPress);

///
/// Handles a direct input event and performs the necessary
/// tracking, glowing and selection actions depending
/// on the state.
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  eInputEvent: Indication of event type
/// \param[in]  nInputVal:   Additional data for event type
///
/// \return none
///
void gslc_TrackInput(gslc_tsGui* pGui,gslc_teInputRawEvent eInputEvent,int16_t nInputVal);

/// \todo Doc. This API is experimental and subject to change
///
/// Convert an external input event into a GUI action
/// - Use the InputMap table to determine the action to
///   take as a result of the external input event.
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  eInputEvent: Indication of event type
/// \param[in]  nInputVal:   Additional data for event type
/// \param[out] peAction:    The GUI action to take
/// \param[out] pnActionVal: Additional parameter for the GUI action
///
/// \return true if a matching event was found, false if none
///
bool gslc_InputMapLookup(gslc_tsGui* pGui,gslc_teInputRawEvent eInputEvent,int16_t nInputVal,gslc_teAction* peAction,int16_t* pnActionVal);

#endif // !DRV_TOUCH_NONE


// ------------------------------------------------------------------------
/// @}
/// \defgroup _IntCleanup_ Internal: Cleanup Functions
/// @{
// ------------------------------------------------------------------------



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
/// \param[in]  pGui:         Pointer to GUI
/// \param[in]  pPage:        Pointer to Page
///
/// \return none
///
void gslc_PageDestruct(gslc_tsGui* pGui,gslc_tsPage* pPage);

///
/// Free up any members associated with an element collection
///
/// \param[in]  pGui:         Pointer to GUI
/// \param[in]  pCollect:     Pointer to collection
///
/// \return none
///
void gslc_CollectDestruct(gslc_tsGui* pGui,gslc_tsCollect* pCollect);

///
/// Free up any members associated with an element
///
/// \param[in]  pElem:        Pointer to element
///
/// \return none
///
void gslc_ElemDestruct(gslc_tsElem* pElem);

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


/// @}
/// End of Internal Functions


#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _GUISLICE_H_
