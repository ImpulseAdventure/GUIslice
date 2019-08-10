//
// GUIslice Library Examples
// - Calvin Hass
// - https://www.impulseadventure.com/elec/guislice-gui.html
// - https://github.com/ImpulseAdventure/GUIslice
// - Example 31 (Arduino): Listbox + Scrollbar
//   - Demonstrates Listbox control (single-column) with scrollbar
//   - Shows callback notifications for Listbox
//   - Provide example of additional Adafruit-GFX fonts
//     (see USE_EXTRA_FONTS)
//   - NOTE: This is the simple version of the example without
//     optimizing for memory consumption. Therefore, it may not
//     run on Arduino devices with limited memory. A "minimal"
//     version is located in the "arduino_min" folder which includes
//     FLASH memory optimization for reduced memory devices.
//
// ARDUINO NOTES:
// - GUIslice_config.h must be edited to match the pinout connections
//   between the Arduino CPU and the display controller (see ADAGFX_PIN_*).
//

#include "GUIslice.h"
#include "GUIslice_drv.h"

// Include any extended elements
#include "elem/XListbox.h"
#include "elem/XSlider.h"



// ------------------------------------------------
// Load specific fonts
// ------------------------------------------------

// To demonstrate additional fonts, uncomment the following line:
//#define USE_EXTRA_FONTS

// Different display drivers provide different fonts, so a few examples
// have been provided and selected here. Font files are usually
// located within the display library folder or fonts subfolder.
#ifdef USE_EXTRA_FONTS
  #if defined(DRV_DISP_TFT_ESPI) // TFT_eSPI
    #include <TFT_eSPI.h>
    #define FONT_NAME1 &FreeSansBold12pt7b
    #define FONT_NAME2 &FreeMono9pt7b
    #define FONT_NAME3 &FreeSans9pt7b
  #elif defined(DRV_DISP_ADAGFX_ILI9341_T3) // Teensy
    #include <font_Arial.h>
    #define FONT_NAME1 &Arial_12
    #define FONT_NAME2 &Arial_8
    #define FONT_NAME3 &Arial_10
    #define SET_FONT_MODE1 // Enable Teensy extra fonts
  #else // Arduino, etc.
    #include <Adafruit_GFX.h>
    #include <gfxfont.h>
    #include "Fonts/FreeSansBold12pt7b.h"
    #include "Fonts/FreeSans9pt7b.h"
    #include "Fonts/FreeMono9pt7b.h"
    #define FONT_NAME1 &FreeSansBold12pt7b
    #define FONT_NAME2 &FreeMono9pt7b
    #define FONT_NAME3 &FreeSans9pt7b
  #endif
#else
  // Use the default font
  #define FONT_NAME1 NULL
  #define FONT_NAME2 NULL
  #define FONT_NAME3 NULL
#endif
// ------------------------------------------------

// ------------------------------------------------
// Defines for resources
// ------------------------------------------------

// ------------------------------------------------
// Enumerations for pages, elements, fonts, images
// ------------------------------------------------
enum { E_PG_MAIN };
enum { E_BTN_QUIT, E_SCROLL, E_LISTBOX };
enum { E_FONT_TITLE, E_FONT_LISTBOX, E_FONT_TXT, MAX_FONT }; // Use separate enum for fonts, MAX_FONT at end


// ------------------------------------------------
// Instantiate the GUI
// ------------------------------------------------

// Define the maximum number of elements per page
#define MAX_PAGE                1
#define MAX_ELEM_PG_MAIN        10
#define MAX_ELEM_PG_MAIN_RAM MAX_ELEM_PG_MAIN

// GUI Elements
gslc_tsGui                      m_gui;
gslc_tsDriver                   m_drv;
gslc_tsFont                     m_asFont[MAX_FONT];
gslc_tsPage                     m_asPage[MAX_PAGE];

gslc_tsElem                 m_asPageElem[MAX_ELEM_PG_MAIN_RAM];
gslc_tsElemRef              m_asPageElemRef[MAX_ELEM_PG_MAIN];
gslc_tsXListbox             m_sListbox;
gslc_tsXSlider              m_sXSlider[1];

#define MAX_STR                 100

#define COUNTRY_CNT  12
#define COUNTRY_LEN  15

// ------------------------------------------------
// Save some element references for update loop access
// ------------------------------------------------
gslc_tsElemRef*  m_pElemSel = NULL;
gslc_tsElemRef*  m_pElemListbox = NULL;

// ------------------------------------------------
// Program Globals
// ------------------------------------------------
bool      m_bQuit = false;

char m_astrCountryCodes[COUNTRY_CNT][3] = {
  "US",
  "BR",
  "CA",
  "DK",
  "DE",
  "FR",
  "IN",
  "JP",
  "MX",
  "PE",
  "UK",
  "VN"
};

char m_astrCountries[COUNTRY_CNT][COUNTRY_LEN + 1] = {
  "USA",
  "Brazil",
  "Canada",
  "Denmark",
  "Germany",
  "France",
  "India",
  "Japan",
  "Mexico",
  "Peru",
  "England",
  "Vietnam"
};

// Define debug message function
static int16_t DebugOut(char ch) { if (ch == (char)'\n') Serial.println(""); else Serial.write(ch); return 0; }

// ------------------------------------------------
// Utility Methods
// ------------------------------------------------
void ListboxLoad(gslc_tsElemRef* pElemRef)
{
  for (int i = 0; i < COUNTRY_CNT; i++) {
    gslc_ElemXListboxAddItem(&m_gui, m_pElemListbox, m_astrCountries[i]);
  }
}

// ------------------------------------------------
// Callback Methods
// ------------------------------------------------
// Common Button callback
bool CbBtnCommon(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY)
{
  gslc_tsElemRef* pElemRef = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem* pElem = pElemRef->pElem;

  if (eTouch == GSLC_TOUCH_UP_IN) {
    // From the element's ID we can determine which button was pressed.
    switch (pElem->nId) {
    case E_BTN_QUIT:
      m_bQuit = true;
      break;
    default:
      break;
    }
  }
  return true;
}

bool CbListBox(void* pvGui, void* pvElemRef, int16_t nSelId)
{
  gslc_tsElemRef* pElemRef = (gslc_tsElemRef*)(pvElemRef);
  char acTxt[5];

  if (pElemRef == NULL) {
    return false;
  }

  // Update the status message with the selection
  if (nSelId == XLISTBOX_SEL_NONE) {
    strncpy(acTxt, "NONE", 5);
  }
  else {
    strncpy(acTxt, m_astrCountryCodes[nSelId], 5);
  }
  gslc_ElemSetTxtStr(&m_gui, m_pElemSel, acTxt);

  return true;
}

// Callback function for when a slider's position has been updated
bool CbSlidePos(void* pvGui, void* pvElemRef, int16_t nPos)
{
  gslc_tsGui*     pGui = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem*    pElem = pElemRef->pElem;
  int16_t         nVal;
  gslc_tsElemRef* pElemRefTmp = NULL;

  // From the element's ID we can determine which slider was updated.
  switch (pElem->nId) {
  case E_SCROLL:
    // Fetch the slider position
    nVal = gslc_ElemXSliderGetPos(pGui, pElemRef);
    // Update the textbox scroll position
    pElemRefTmp = gslc_PageFindElemById(pGui, E_PG_MAIN, E_LISTBOX);
    gslc_ElemXListboxSetScrollPos(pGui, pElemRefTmp, nVal);
    break;
  default:
    break;
  }

  return true;
}


// ------------------------------------------------
// Create page elements
// ------------------------------------------------
bool InitGUI()
{
  gslc_tsElemRef* pElemRef = NULL;

  //<InitGUI !Start!>
  gslc_PageAdd(&m_gui, E_PG_MAIN, m_asPageElem, MAX_ELEM_PG_MAIN_RAM, m_asPageElemRef, MAX_ELEM_PG_MAIN);

  // Background flat color
  gslc_SetBkgndColor(&m_gui, GSLC_COL_GRAY_DK3);

  // -----------------------------------
  // PAGE: E_PG_MAIN

  // Create wrapping box for textbox E_LISTBOX and scrollbar
  gslc_tsRect rList = (gslc_tsRect) { 17, 75, 200, 100 };
  pElemRef = gslc_ElemCreateBox(&m_gui, GSLC_ID_AUTO, E_PG_MAIN, rList);
  gslc_ElemSetCol(&m_gui, pElemRef, GSLC_COL_GRAY, GSLC_COL_BLACK, GSLC_COL_BLACK);

  // Create listbox
  // - Establish storage for listbox
  // - Note that XLISTBOX_BUF_OH_R is extra required per item to
  //   accommodate GUI metadata
  static uint8_t m_pXListboxBuf[COUNTRY_CNT*(COUNTRY_LEN + XLISTBOX_BUF_OH_R)];
  pElemRef = gslc_ElemXListboxCreate(&m_gui, E_LISTBOX, E_PG_MAIN, &m_sListbox,
    (gslc_tsRect) { rList.x + 2, rList.y + 4, rList.w - 23, rList.h - 7 }, E_FONT_LISTBOX,
    m_pXListboxBuf, sizeof(m_pXListboxBuf), 0);
  gslc_ElemXListboxItemsSetSize(&m_gui, pElemRef, XLISTBOX_SIZE_AUTO, XLISTBOX_SIZE_AUTO);
  gslc_ElemXListboxSetSize(&m_gui, pElemRef, 5, 1); // 5 rows, 1 column
  gslc_ElemXListboxSetSelFunc(&m_gui, pElemRef, &CbListBox);
  gslc_ElemSetFrameEn(&m_gui, pElemRef, true);
  gslc_ElemSetCol(&m_gui, pElemRef, GSLC_COL_GRAY, GSLC_COL_BLACK, GSLC_COL_BLUE_DK3);
  gslc_ElemSetTxtCol(&m_gui, pElemRef, GSLC_COL_BLUE_LT3);
  gslc_ElemSetGlowCol(&m_gui, pElemRef, GSLC_COL_GRAY, GSLC_COL_BLUE_DK3, GSLC_COL_WHITE);
  gslc_ElemSetTxtMarginXY(&m_gui, pElemRef, 5, 0); // Provide additional margin from left side
  m_pElemListbox = pElemRef;

  // Create vertical scrollbar for textbox
  pElemRef = gslc_ElemXSliderCreate(&m_gui, E_SCROLL, E_PG_MAIN, &m_sXSlider[0],
    (gslc_tsRect) { rList.x + (int16_t)rList.w - 21, rList.y + 4, 20, rList.h - 8 }, 0, COUNTRY_CNT - 1, 0, 5, true);
  gslc_ElemSetCol(&m_gui, pElemRef, GSLC_COL_BLUE_LT1, GSLC_COL_BLACK, GSLC_COL_BLACK);
  gslc_ElemXSliderSetStyle(&m_gui, pElemRef, true, GSLC_COL_BLUE_DK1, 0, 0, GSLC_COL_BLACK);
  gslc_ElemXSliderSetPosFunc(&m_gui, pElemRef, &CbSlidePos);

  // Create GSLC_ID_AUTO text label
  pElemRef = gslc_ElemCreateTxt(&m_gui, GSLC_ID_AUTO, E_PG_MAIN, (gslc_tsRect) { 15, 45, 100, 25 },
    (char*)"Country Code:", 0, E_FONT_TXT);
  gslc_ElemSetTxtAlign(&m_gui, pElemRef, GSLC_ALIGN_MID_MID);
  gslc_ElemSetTxtCol(&m_gui, pElemRef, GSLC_COL_YELLOW);
  gslc_ElemSetCol(&m_gui, pElemRef, GSLC_COL_BLACK, GSLC_COL_GRAY_DK3, GSLC_COL_BLACK);

  static char mstr2[20] = "US"; // FIXME
  pElemRef = gslc_ElemCreateTxt(&m_gui, GSLC_ID_AUTO, E_PG_MAIN, (gslc_tsRect) { 125, 45, 55, 25 },
    mstr2, sizeof(mstr2), E_FONT_TXT);
  gslc_ElemSetTxtAlign(&m_gui, pElemRef, GSLC_ALIGN_MID_MID);
  gslc_ElemSetTxtCol(&m_gui, pElemRef, GSLC_COL_ORANGE);
  gslc_ElemSetCol(&m_gui, pElemRef, GSLC_COL_BLACK, GSLC_COL_GRAY_DK3, GSLC_COL_BLACK);
  m_pElemSel = pElemRef; // Save for quick access

  // Create GSLC_ID_AUTO text label
  pElemRef = gslc_ElemCreateTxt(&m_gui, GSLC_ID_AUTO, E_PG_MAIN, (gslc_tsRect) { 10, 20, 200, 25 },
    (char*)"Country Chooser", 0, E_FONT_TITLE);
  gslc_ElemSetTxtAlign(&m_gui, pElemRef, GSLC_ALIGN_MID_MID);
  gslc_ElemSetTxtCol(&m_gui, pElemRef, GSLC_COL_YELLOW);
  gslc_ElemSetCol(&m_gui, pElemRef, GSLC_COL_BLACK, GSLC_COL_GRAY_DK3, GSLC_COL_BLACK);

  // Create E_BTN_QUIT button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui, E_BTN_QUIT, E_PG_MAIN,
    (gslc_tsRect) { 217 - 60, 190, 60, 30 }, (char*)"OK", 0, E_FONT_TXT, &CbBtnCommon);
  gslc_ElemSetCol(&m_gui, pElemRef, GSLC_COL_GREEN_DK2, GSLC_COL_GREEN_DK4, GSLC_COL_GREEN_DK1);
  gslc_ElemSetTxtCol(&m_gui, pElemRef, GSLC_COL_WHITE);

  return true;
}

void setup()
{
  // ------------------------------------------------
  // Initialize
  // ------------------------------------------------
  Serial.begin(9600);
  // Wait for USB Serial 
  delay(1000);  // NOTE: Some devices require a delay after Serial.begin() before serial port can be used

  gslc_InitDebug(&DebugOut);

  if (!gslc_Init(&m_gui, &m_drv, m_asPage, MAX_PAGE, m_asFont, MAX_FONT)) { return; }

  // ------------------------------------------------
  // Load Fonts
  // ------------------------------------------------
  if (!gslc_FontSet(&m_gui, E_FONT_TITLE, GSLC_FONTREF_PTR, FONT_NAME1, 1)) { return; }
  if (!gslc_FontSet(&m_gui, E_FONT_LISTBOX, GSLC_FONTREF_PTR, FONT_NAME2, 1)) { return; }
  if (!gslc_FontSet(&m_gui, E_FONT_TXT, GSLC_FONTREF_PTR, FONT_NAME3, 1)) { return; }
  // Some display drivers need to set a mode to use the extra fonts
  #if defined(SET_FONT_MODE1)
    gslc_FontSetMode(&m_gui, E_FONT_TITLE, GSLC_FONTREF_MODE_1);
    gslc_FontSetMode(&m_gui, E_FONT_LISTBOX, GSLC_FONTREF_MODE_1);
    gslc_FontSetMode(&m_gui, E_FONT_TXT, GSLC_FONTREF_MODE_1);
  #endif

  // ------------------------------------------------
  // Create graphic elements
  // ------------------------------------------------
  InitGUI();

  // ------------------------------------------------
  // Populate our Listbox with data
  // ------------------------------------------------
  ListboxLoad(m_pElemListbox);

  // ------------------------------------------------
  // Start up display on first page
  // ------------------------------------------------
  gslc_SetPageCur(&m_gui, E_PG_MAIN);
  gslc_GuiRotate(&m_gui, 0);

}

// -----------------------------------
// Main event loop
// -----------------------------------
void loop()
{

  // ------------------------------------------------
  // Update GUI Elements
  // ------------------------------------------------

  // ------------------------------------------------
  // Periodically call GUIslice update function
  // ------------------------------------------------
  gslc_Update(&m_gui);

  if (m_bQuit) {
    gslc_Quit(&m_gui);
    while (1) { }
  }

}