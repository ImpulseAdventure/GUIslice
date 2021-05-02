#ifndef _GUISLICE_EX_XKEYPAD_NUM_SETUP_H_
#define _GUISLICE_EX_XKEYPAD_NUM_SETUP_H_

#include "elem/XKeyPad.h"
#include "elem/XKeyPad_Num.h"

// ============================================================================
// This is the setup file associated with the XKeyPad_Num
// ============================================================================

#define XKEYPAD_EXTEND_CHAR       0   // 0=Single-char, 1=Multi-char
#define XKEYPAD_LABEL_MAX         6   // Max buffer for a key label

#define XKEYPAD_DISP_MAX         12   // Maximum display length (without NULL)

#define XKEYPAD_KEY_W            25   // Default key width
#define XKEYPAD_KEY_H            25   // Default key height

#define XKEYPAD_SPACING_X         0   // Default key spacing in X direction
#define XKEYPAD_SPACING_Y         0   // Default key spacing in Y direction

// Other custom settings / values
static const char* KEYPAD_LABEL_NEGATIVE    = "-";
static const char* KEYPAD_LABEL_DECIMAL_PT  = ".";

// Define the set of available KeyPad layouts
typedef enum {
  // List of keypad layouts
  E_XKEYPAD_SET_NUM,
  // Terminator
  E_XKEYPAD_SET__MAX,
} gslc_teXKeyPadSel;

// Define the startup layout
static const int8_t XKEYPAD_LAYOUT_DEFAULT = E_XKEYPAD_SET_NUM;

// Define local list of variant-specific button ID types
enum {
  // --- Variant-specific special buttons
  //     These are reserved the global ID range 16..31
  KEYPAD_IDV_DECIMAL = 16,
  KEYPAD_IDV_MINUS,
};

// Define the text strings for each special button
// - And associate with each Key ID
static gslc_tsLabelSpecial KEYPAD_SPECIAL_LABEL[] = {
  { KEYPAD_ID_BACKSPACE,    "BS"},
  { KEYPAD_IDV_DECIMAL,     "."},
  { KEYPAD_IDV_MINUS,       "-"},
  { KEYPAD_ID_ESC,          "ESC"},
  { KEYPAD_ID_ENTER,        "ENT"},
  { KEYPAD_ID_SCROLL_LEFT,  "<"},
  { KEYPAD_ID_SCROLL_RIGHT, ">"},
  // Terminate list
  { KEYPAD_ID__END,""},
};

// Text for the basic buttons
// - For each of the available keypad layouts, provide a
//   single character per button.
// - Each keypad layout should have the same number of characters,
//   so use a dummy character (eg. space) if a key button
//   is unused.

// The definition of keypad labels depends on whether
// support for extended character sets is required.
#if (XKEYPAD_EXTEND_CHAR)
  // Support extended character set
  static const char* KEYPAD_SET_LABEL[E_XKEYPAD_SET__MAX][10] = {
    { // Numeric
      "0","1","2","3","4","5","6","7","8","9",
    },
  };
#else
  // Support ASCII character set
  static const char* KEYPAD_SET_LABEL[] = {
    "0123456789", // Num
};
#endif // XKEYPAD_EXTEND_CHAR


// Define a layout of the KeyPad
// - { ID, ROW, COL, ROWSPAN< COLSPAN, TYPE }
static gslc_tsKey KEYPAD_LAYOUT[] = {
  // Text field
  { KEYPAD_ID_TXT,          0, 1, 1, 4, E_XKEYPAD_TYPE_TXT }, 
  // Special & Basic buttons
  { KEYPAD_ID_SCROLL_LEFT,    0, 0, 1, 1, E_XKEYPAD_TYPE_SPECIAL },   
  { KEYPAD_ID_SCROLL_RIGHT,   0, 5, 1, 1, E_XKEYPAD_TYPE_SPECIAL },   
  { KEYPAD_ID_ESC,            0, 6, 1, 2, E_XKEYPAD_TYPE_SPECIAL }, 
  { KEYPAD_ID_BASIC_START+0 , 1, 0, 1, 1, E_XKEYPAD_TYPE_BASIC },  // "0"
  { KEYPAD_ID_BASIC_START+1 , 1, 1, 1, 1, E_XKEYPAD_TYPE_BASIC },  // "1", 
  { KEYPAD_ID_BASIC_START+2 , 1, 2, 1, 1, E_XKEYPAD_TYPE_BASIC },  // "2", 
  { KEYPAD_ID_BASIC_START+3 , 1, 3, 1, 1, E_XKEYPAD_TYPE_BASIC },  // "3", 
  { KEYPAD_ID_BASIC_START+4 , 1, 4, 1, 1, E_XKEYPAD_TYPE_BASIC },  // "4", 
  { KEYPAD_IDV_MINUS,         1, 5, 1, 1, E_XKEYPAD_TYPE_SPECIAL }, 
  { KEYPAD_ID_BACKSPACE,      1, 6, 1, 2, E_XKEYPAD_TYPE_SPECIAL },   
  { KEYPAD_ID_BASIC_START+5 , 2, 0, 1, 1, E_XKEYPAD_TYPE_BASIC },  // "5", 
  { KEYPAD_ID_BASIC_START+6 , 2, 1, 1, 1, E_XKEYPAD_TYPE_BASIC },  // "6", 
  { KEYPAD_ID_BASIC_START+7 , 2, 2, 1, 1, E_XKEYPAD_TYPE_BASIC },  // "7" ,
  { KEYPAD_ID_BASIC_START+8 , 2, 3, 1, 1, E_XKEYPAD_TYPE_BASIC },  // "8", 
  { KEYPAD_ID_BASIC_START+9 , 2, 4, 1, 1, E_XKEYPAD_TYPE_BASIC },  // "9",
  { KEYPAD_IDV_DECIMAL,       2, 5, 1, 1, E_XKEYPAD_TYPE_SPECIAL }, 
  { KEYPAD_ID_ENTER,          2, 6, 1, 2, E_XKEYPAD_TYPE_SPECIAL }, 
  // End of list
  { KEYPAD_ID__END,0,0,0,0,E_XKEYPAD_TYPE_END },
};

// Define a layout to use for each KeyPad page
static gslc_tsKey* KEYPAD_LAYOUTS[E_XKEYPAD_SET__MAX] = {
  KEYPAD_LAYOUT,  // Num
};

// Define colors
// - Default colors
#define XKEYPAD_COL_DISABLE_TXT GSLC_COL_GRAY_LT1
#define XKEYPAD_COL_DISABLE_FILL GSLC_COL_BLACK
#define XKEYPAD_COL_DEF_TXT GSLC_COL_WHITE
#define XKEYPAD_COL_DEF_FRAME GSLC_COL_GRAY
#define XKEYPAD_COL_DEF_FILL GSLC_COL_GRAY_DK1
#define XKEYPAD_COL_DEF_GLOW GSLC_COL_GRAY_LT3
// - Color overrides for common keys
#define XKEYPAD_COL_BASIC_FILL GSLC_COL_BLUE_DK1
#define XKEYPAD_COL_BASIC_GLOW GSLC_COL_BLUE_LT4
#define XKEYPAD_COL_TEXT_TXT GSLC_COL_YELLOW
#define XKEYPAD_COL_TEXT_FILL GSLC_COL_BLACK
#define XKEYPAD_COL_TEXT_GLOW GSLC_COL_GRAY_DK2
#define XKEYPAD_COL_SPACE_FILL GSLC_COL_BLUE_DK1
#define XKEYPAD_COL_SPACE_GLOW GSLC_COL_BLUE_LT4
#define XKEYPAD_COL_ESC_FILL GSLC_COL_RED_DK1
#define XKEYPAD_COL_ESC_GLOW GSLC_COL_RED_LT4
#define XKEYPAD_COL_ENTER_FILL GSLC_COL_GREEN_DK1
#define XKEYPAD_COL_ENTER_GLOW GSLC_COL_GREEN_LT4
#define XKEYPAD_COL_SCROLL_L_FILL GSLC_COL_BLUE_DK1
#define XKEYPAD_COL_SCROLL_L_GLOW GSLC_COL_BLUE_LT4
#define XKEYPAD_COL_SCROLL_R_FILL GSLC_COL_BLUE_DK1
#define XKEYPAD_COL_SCROLL_R_GLOW GSLC_COL_BLUE_LT4
// - Color overrides for variant-specific keys
#define XKEYPAD_COL_DECIMAL_FILL GSLC_COL_GRAY_DK1
#define XKEYPAD_COL_DECIMAL_GLOW GSLC_COL_GRAY_LT4
#define XKEYPAD_COL_MINUS_FILL GSLC_COL_GRAY_DK1
#define XKEYPAD_COL_MINUS_GLOW GSLC_COL_GRAY_LT4

#endif // _GUISLICE_EX_XKEYPAD_NUM_SETUP_H_
