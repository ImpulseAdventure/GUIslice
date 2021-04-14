#ifndef _GUISLICE_EX_XKEYPAD_ALPHA_SETUP_H_
#define _GUISLICE_EX_XKEYPAD_ALPHA_SETUP_H_

#include "elem/XKeyPad.h"
#include "elem/XKeyPad_Alpha.h"

// ============================================================================
// This is the setup file associated with the XKeyPad_Alpha
// ============================================================================

#define XKEYPAD_EXTEND_CHAR       0   // 0=Single-char, 1=Multi-char
#define XKEYPAD_LABEL_MAX         6   // Max buffer for a key label

#define XKEYPAD_DISP_MAX         18   // Maximum display length (without NULL)

// NOTE: XKeyPad_Alpha specifies half-width keys and then
//       doubles up the width with ColSpan=2. This allows
//       us to stagger some keys.
#define XKEYPAD_KEY_W            12   // Default key width
#define XKEYPAD_KEY_H            25   // Default key height

#define XKEYPAD_SPACING_X         0   // Default key spacing in X direction
#define XKEYPAD_SPACING_Y         0   // Default key spacing in Y direction

// Other custom settings / values
static const char* XKEYPAD_LABEL_SPACE = " ";

// Define the set of available KeyPad layouts
typedef enum {
  // List of keypads
  E_XKEYPAD_SET_UPPER,
  E_XKEYPAD_SET_LOWER,
  E_XKEYPAD_SET_NUM,
  // Terminator
  E_XKEYPAD_SET__MAX,
} gslc_teXKeyPadSel;

// Define the startup layout
static const int8_t XKEYPAD_LAYOUT_DEFAULT = E_XKEYPAD_SET_UPPER;

// Define local list of variant-specific button ID types
/*
enum {
  // --- Variant-specific special buttons
  //     These are reserved the global ID range 16..31
  //KEYPAD_IDV_... = 16,
};
*/

// Define the text strings for each special button
// - And associate with each Key ID
static gslc_tsLabelSpecial KEYPAD_SPECIAL_LABEL[] = {
  { KEYPAD_ID_BACKSPACE,    "BS"},
  { KEYPAD_ID_SPACE,        " "},
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
  static const char* KEYPAD_SET_LABEL[E_XKEYPAD_SET__MAX][26] = {
    { // Alpha (upper case)
      "Q","W","E","R","T","Y","U","I","O","P","A","S","D","F","G","H","J","K","L","Z","X","C","V","B","N","M",
    },
    { // Alpha (lower case)
      "q","w","e","r","t","y","u","i","o","p","a","s","d","f","g","h","j","k","l","z","x","c","v","b","n","m",
    },
    { // Numeric / symbols
      "0","1","2","3","4","5","6","7","8","9","+","-","=","%","'","&","(",")",".","@","#","$","/","!","?",",",
    },
  };
#else
  // Support ASCII character set
  static const char* KEYPAD_SET_LABEL[] = {
    "QWERTYUIOPASDFGHJKLZXCVBNM", // Alpha (upper case)
    "qwertyuiopasdfghjklzxcvbnm", // Alpha (lower case)
    "0123456789+-=%'&().@#$/!?,", // Numeric / symbols
  };
#endif // XKEYPAD_EXTEND_CHAR


// Keypad selection button
// - This is the text used to represent the
//   keypad select / swap button.
// - Entries here should be in the same order as
//   appear in the gslc_teXKeyPadSel enumeration.
static const char* KEYPAD_SPECIAL_SELECT[] = {
  "ABC",
  "abc",
  "123",
};

// Define a layout of the KeyPad
// - { ID, ROW, COL, ROWSPAN, COLSPAN, TYPE }
static gslc_tsKey KEYPAD_LAYOUT[] = {
  // Text field
  { KEYPAD_ID_TXT,            0, 2, 1,12, E_XKEYPAD_TYPE_TXT }, 

  // Special & basic buttons
  { KEYPAD_ID_SCROLL_LEFT,    0, 0, 1, 2, E_XKEYPAD_TYPE_SPECIAL },   
  { KEYPAD_ID_SCROLL_RIGHT,   0,14, 1, 2, E_XKEYPAD_TYPE_SPECIAL },   
  { KEYPAD_ID_ESC,            0,16, 1, 4, E_XKEYPAD_TYPE_SPECIAL }, 
  { KEYPAD_ID_BASIC_START+0 , 1, 0, 1, 2, E_XKEYPAD_TYPE_BASIC },  // "Q" - "0"
  { KEYPAD_ID_BASIC_START+1 , 1, 2, 1, 2, E_XKEYPAD_TYPE_BASIC },  // "W" - "1"
  { KEYPAD_ID_BASIC_START+2 , 1, 4, 1, 2, E_XKEYPAD_TYPE_BASIC },  // "E" - "2"
  { KEYPAD_ID_BASIC_START+3 , 1, 6, 1, 2, E_XKEYPAD_TYPE_BASIC },  // "R" - "3"
  { KEYPAD_ID_BASIC_START+4 , 1, 8, 1, 2, E_XKEYPAD_TYPE_BASIC },  // "T" - "4"
  { KEYPAD_ID_BASIC_START+5 , 1,10, 1, 2, E_XKEYPAD_TYPE_BASIC },  // "Y" - "5"
  { KEYPAD_ID_BASIC_START+6 , 1,12, 1, 2, E_XKEYPAD_TYPE_BASIC },  // "U" - "6"
  { KEYPAD_ID_BASIC_START+7 , 1,14, 1, 2, E_XKEYPAD_TYPE_BASIC },  // "I" - "7"
  { KEYPAD_ID_BASIC_START+8 , 1,16, 1, 2, E_XKEYPAD_TYPE_BASIC },  // "O" - "8"
  { KEYPAD_ID_BASIC_START+9 , 1,18, 1, 2, E_XKEYPAD_TYPE_BASIC },  // "P" - "9"
  { KEYPAD_ID_BASIC_START+10, 2, 1, 1, 2, E_XKEYPAD_TYPE_BASIC },  // "A" - "+"
  { KEYPAD_ID_BASIC_START+11, 2, 3, 1, 2, E_XKEYPAD_TYPE_BASIC },  // "S" - "-"
  { KEYPAD_ID_BASIC_START+12, 2, 5, 1, 2, E_XKEYPAD_TYPE_BASIC },  // "D" - "=" 
  { KEYPAD_ID_BASIC_START+13, 2, 7, 1, 2, E_XKEYPAD_TYPE_BASIC },  // "F" - "%"
  { KEYPAD_ID_BASIC_START+14, 2, 9, 1, 2, E_XKEYPAD_TYPE_BASIC },  // "G" - "'"
  { KEYPAD_ID_BASIC_START+15, 2,11, 1, 2, E_XKEYPAD_TYPE_BASIC },  // "H" - "&'
  { KEYPAD_ID_BASIC_START+16, 2,13, 1, 2, E_XKEYPAD_TYPE_BASIC },  // "J" - "("
  { KEYPAD_ID_BASIC_START+17, 2,15, 1, 2, E_XKEYPAD_TYPE_BASIC },  // "K" - ")"
  { KEYPAD_ID_BASIC_START+18, 2,17, 1, 2, E_XKEYPAD_TYPE_BASIC },  // "L" - "."
  { KEYPAD_ID_BASIC_START+19, 3, 2, 1, 2, E_XKEYPAD_TYPE_BASIC },  // "Z" - "@"
  { KEYPAD_ID_BASIC_START+20, 3, 4, 1, 2, E_XKEYPAD_TYPE_BASIC },  // "X" - "#"
  { KEYPAD_ID_BASIC_START+21, 3, 6, 1, 2, E_XKEYPAD_TYPE_BASIC },  // "C" - "$"
  { KEYPAD_ID_BASIC_START+22, 3, 8, 1, 2, E_XKEYPAD_TYPE_BASIC },  // "V" - "/"
  { KEYPAD_ID_BASIC_START+23, 3,10, 1, 2, E_XKEYPAD_TYPE_BASIC },  // "B" - "!"
  { KEYPAD_ID_BASIC_START+24, 3,12, 1, 2, E_XKEYPAD_TYPE_BASIC },  // "N" - "?" 
  { KEYPAD_ID_BASIC_START+25, 3,14, 1, 2, E_XKEYPAD_TYPE_BASIC },  // "M" - ","
  { KEYPAD_ID_BACKSPACE,      3,16, 1, 4, E_XKEYPAD_TYPE_SPECIAL }, 
  { KEYPAD_ID_SWAP_PAD,       4, 0, 1, 4, E_XKEYPAD_TYPE_SPECIAL }, 
  { KEYPAD_ID_SPACE,          4, 4, 1,12, E_XKEYPAD_TYPE_SPECIAL }, 
  { KEYPAD_ID_ENTER,          4,16, 1, 4, E_XKEYPAD_TYPE_SPECIAL }, 

  // End of list
  { KEYPAD_ID__END,0,0,0,0,E_XKEYPAD_TYPE_END },
};

// Define a layout to use for each KeyPad page
static gslc_tsKey* KEYPAD_LAYOUTS[E_XKEYPAD_SET__MAX] = {
  KEYPAD_LAYOUT,  // Upper
  KEYPAD_LAYOUT,  // Lower
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
// ...

#endif // _GUISLICE_EX_XKEYPAD_ALPHA_SETUP_H_
