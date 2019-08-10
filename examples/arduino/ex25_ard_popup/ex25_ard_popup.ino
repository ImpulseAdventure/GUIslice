//
// GUIslice Library Example
// - Paul Conti
// - https://www.impulseadventure.com/elec/guislice-gui.html
// - https://github.com/ImpulseAdventure/GUIslice
// - Example 25 (Arduino):
//   - Popup dialog to confirm QUIT
//   - Use of EXTRA Fonts 
//   - Accept touch input, text button
//   - Expected behavior: Clicking on button terminates program
//   - NOTE: This is the simple version of the example without
//     optimizing for memory consumption. A "minimal"
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
#include "elem/XProgress.h"


// ------------------------------------------------
// Load specific fonts
// ------------------------------------------------

//<Fonts !Start!>
// To demonstrate additional fonts, uncomment the following line:
//#define USE_EXTRA_FONTS

// Different display drivers provide different fonts, so a few examples
// have been provided and selected here. Font files are usually
// located within the display library folder or fonts subfolder.
#ifdef USE_EXTRA_FONTS
  #if defined(DRV_DISP_TFT_ESPI) // TFT_eSPI
    #include <TFT_eSPI.h>
    #define FONT_NAME1 &FreeSans9pt7b
  #elif defined(DRV_DISP_ADAGFX_ILI9341_T3) // Teensy
    #include <font_Arial.h>
    #define FONT_NAME1 &Arial_8
    #define SET_FONT_MODE1 // Enable Teensy extra fonts
  #else // Arduino, etc.
    #include <Adafruit_GFX.h>
    #include <gfxfont.h>
    #include "Fonts/FreeSans9pt7b.h"
    #define FONT_NAME1 &FreeSans9pt7b
  #endif
#else
  // Use the default font
  #define FONT_NAME1 NULL
#endif
//<Fonts !End!>
// ------------------------------------------------


// ------------------------------------------------
// Defines for resources
// ------------------------------------------------
//<Resources !Start!>
//<Resources !End!>

// ------------------------------------------------
// Enumerations for pages, elements, fonts, images
// ------------------------------------------------
//<Enum !Start!>
enum { E_PG_MAIN, E_PG_POPUP };
enum { E_BTN_CANCEL, E_BTN_OK, E_BTN_QUIT, E_PROGRESS, E_TXT_STATUS };
enum { E_FONT_SANS1, E_FONT_TXT2, MAX_FONT }; // Use separate enum for fonts, MAX_FONT at end
//<Enum !End!>

// ------------------------------------------------
// Instantiate the GUI
// ------------------------------------------------

// Define the maximum number of elements per page
//<ElementDefines !Start!>
#define MAX_PAGE                2
#define MAX_ELEM_PG_MAIN        3
#define MAX_ELEM_PG_MAIN_RAM MAX_ELEM_PG_MAIN
#define MAX_ELEM_PG_POPUP       5
#define MAX_ELEM_PG_POPUP_RAM MAX_ELEM_PG_POPUP
//<ElementDefines !End!>

// GUI Elements
gslc_tsGui                      m_gui;
gslc_tsDriver                   m_drv;
gslc_tsFont                     m_asFont[MAX_FONT];
gslc_tsPage                     m_asPage[MAX_PAGE];

//<GUI_Extra_Elements !Start!>
gslc_tsElem                     m_asPage1Elem[MAX_ELEM_PG_MAIN_RAM];
gslc_tsElemRef                  m_asPage1ElemRef[MAX_ELEM_PG_MAIN];
gslc_tsElem                     m_asPage2Elem[MAX_ELEM_PG_POPUP_RAM];
gslc_tsElemRef                  m_asPage2ElemRef[MAX_ELEM_PG_POPUP];
gslc_tsXProgress                m_sXGauge[1];

#define MAX_STR                 100

//<GUI_Extra_Elements !End!>

// ------------------------------------------------
// Save some element references for update loop access
// ------------------------------------------------
//<Save_References !Start!>
//<Save_References !End!>
gslc_tsElemRef*  m_pElemProgress = NULL;
gslc_tsElemRef*  m_pTxtStatus = NULL;

// ------------------------------------------------
// Program Globals
// ------------------------------------------------

// Flag to stop program
bool m_bQuit = false;

// Free-running counter for display
unsigned    m_nCount = 0;

// Define debug message function
static int16_t DebugOut(char ch) { if (ch == (char)'\n') Serial.println(""); else Serial.write(ch); return 0; }

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
      //<Button Enums !Start!>
    case E_BTN_QUIT:
      gslc_PopupShow(&m_gui, E_PG_POPUP, true);
      break;
    case E_BTN_OK:
      gslc_PopupHide(&m_gui);
      gslc_ElemSetTxtStr(&m_gui, m_pTxtStatus, "Stopped!");
      m_bQuit = true;
      break;
    case E_BTN_CANCEL:
      gslc_PopupHide(&m_gui);
      gslc_ElemSetTxtStr(&m_gui, m_pTxtStatus, "Keep Running!");
      break;
      //<Button Enums !End!>
    default:
      break;
    }
  }
  return true;
}
//<Draw Callback !Start!>
//<Draw Callback !End!>
//<Slider Callback !Start!>
//<Slider Callback !End!>
//<Tick Callback !Start!>
//<Tick Callback !End!>

// ------------------------------------------------
// Create page elements
// ------------------------------------------------
bool InitGUI()
{
  gslc_tsElemRef* pElemRef = NULL;

  //<InitGUI !Start!>
  gslc_PageAdd(&m_gui, E_PG_MAIN, m_asPage1Elem, MAX_ELEM_PG_MAIN_RAM, m_asPage1ElemRef, MAX_ELEM_PG_MAIN);
  gslc_PageAdd(&m_gui, E_PG_POPUP, m_asPage2Elem, MAX_ELEM_PG_POPUP_RAM, m_asPage2ElemRef, MAX_ELEM_PG_POPUP);

  // Background flat color
  gslc_SetBkgndColor(&m_gui, GSLC_COL_BLACK);

  // -----------------------------------
  // PAGE: E_PG_MAIN

  // create E_BTN_QUIT button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui, E_BTN_QUIT, E_PG_MAIN,
    (gslc_tsRect) { 80, 130, 80, 40 }, (char*)"QUIT", 0, E_FONT_SANS1, &CbBtnCommon);
  gslc_ElemSetTxtCol(&m_gui, pElemRef, GSLC_COL_WHITE);
  gslc_ElemSetCol(&m_gui, pElemRef, GSLC_COL_RED, GSLC_COL_BLUE_LT2, GSLC_COL_BLUE_LT4);
  gslc_ElemSetFrameEn(&m_gui, pElemRef, true);

  // Create E_TXT_STATUS modifiable text label
  static char m_strtxt1[18] = "Program Running";
  pElemRef = gslc_ElemCreateTxt(&m_gui, E_TXT_STATUS, E_PG_MAIN,
    (gslc_tsRect) { 27, 21, 182, 20 },
    (char*)m_strtxt1, 18, E_FONT_TXT2);
  gslc_ElemSetTxtAlign(&m_gui, pElemRef, GSLC_ALIGN_MID_MID);
  m_pTxtStatus = pElemRef;

  // Create progress bar E_PROGRESS 
  pElemRef = gslc_ElemXProgressCreate(&m_gui, E_PROGRESS, E_PG_MAIN, &m_sXGauge[0],
    (gslc_tsRect) { 70, 68, 100, 20 }, 0, 100, 0, GSLC_COL_GREEN, false);
  m_pElemProgress = pElemRef;

  // -----------------------------------
  // PAGE: E_PG_POPUP

  // Create E_BOX2 box
  pElemRef = gslc_ElemCreateBox(&m_gui, GSLC_ID_AUTO, E_PG_POPUP, (gslc_tsRect) { 20, 80, 200, 150 });
  gslc_ElemSetCol(&m_gui, pElemRef, GSLC_COL_RED, GSLC_COL_GRAY_LT2, GSLC_COL_BLACK);

  // create E_BTN_OK button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui, E_BTN_OK, E_PG_POPUP, (gslc_tsRect) { 130, 170, 80, 40 }, (char*)"YES", 0, E_FONT_SANS1, &CbBtnCommon);
  gslc_ElemSetTxtCol(&m_gui, pElemRef, GSLC_COL_WHITE);
  gslc_ElemSetCol(&m_gui, pElemRef, GSLC_COL_RED, GSLC_COL_BLUE_LT2, GSLC_COL_BLUE_LT4);
  gslc_ElemSetFrameEn(&m_gui, pElemRef, true);

  // create E_BTN_CANCEL button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui, E_BTN_CANCEL, E_PG_POPUP, (gslc_tsRect) { 30, 170, 80, 40 }, (char*)"NO", 0, E_FONT_SANS1, &CbBtnCommon);
  gslc_ElemSetTxtCol(&m_gui, pElemRef, GSLC_COL_WHITE);
  gslc_ElemSetCol(&m_gui, pElemRef, GSLC_COL_RED, GSLC_COL_BLUE_LT2, GSLC_COL_BLUE_LT4);
  gslc_ElemSetFrameEn(&m_gui, pElemRef, true);

  // Create E_TXT1 text label
  pElemRef = gslc_ElemCreateTxt(&m_gui, GSLC_ID_AUTO, E_PG_POPUP, (gslc_tsRect) { 43, 100, 153, 23 },
    (char*)"Do you really want", 0, E_FONT_SANS1);
  gslc_ElemSetTxtCol(&m_gui, pElemRef, GSLC_COL_BLACK);
  gslc_ElemSetCol(&m_gui, pElemRef, GSLC_COL_GRAY_LT2, GSLC_COL_GRAY_LT2, GSLC_COL_GRAY_LT2);

  // Create E_TXT2 text label
  pElemRef = gslc_ElemCreateTxt(&m_gui, GSLC_ID_AUTO, E_PG_POPUP, (gslc_tsRect) { 86, 133, 68, 23 },
    (char*)"to Quit?", 0, E_FONT_SANS1);
  gslc_ElemSetTxtCol(&m_gui, pElemRef, GSLC_COL_BLACK);
  gslc_ElemSetCol(&m_gui, pElemRef, GSLC_COL_GRAY_LT2, GSLC_COL_GRAY_LT2, GSLC_COL_GRAY_LT2);
  //<InitGUI !End!>

  return true;
}

void setup()
{
  // ------------------------------------------------
  // Initialize
  // ------------------------------------------------
  Serial.begin(9600);
  // Wait for USB Serial 
  //delay(1000);  // NOTE: Some devices require a delay after Serial.begin() before serial port can be used

  gslc_InitDebug(&DebugOut);

  if (!gslc_Init(&m_gui, &m_drv, m_asPage, MAX_PAGE, m_asFont, MAX_FONT)) { return; }

  gslc_GuiRotate(&m_gui, 0);

  // ------------------------------------------------
  // Load Fonts
  // ------------------------------------------------
  //<Load_Fonts !Start!>
  if (!gslc_FontSet(&m_gui, E_FONT_SANS1, GSLC_FONTREF_PTR, FONT_NAME1, 1)) { return; }
  if (!gslc_FontSet(&m_gui, E_FONT_TXT2, GSLC_FONTREF_PTR, NULL, 2)) { return; }
  // Some display drivers need to set a mode to use the extra fonts
  #if defined(SET_FONT_MODE1)
    gslc_FontSetMode(&m_gui, E_FONT_SANS1, GSLC_FONTREF_MODE_1);
  #endif
  //<Load_Fonts !End!>

  // ------------------------------------------------
  // Create graphic elements
  // ------------------------------------------------
  InitGUI();


  //<Startup !Start!>
  // ------------------------------------------------
  // Start up display on first page
  // ------------------------------------------------
  gslc_SetPageCur(&m_gui, E_PG_MAIN);
  //<Startup !End!>

}

// -----------------------------------
// Main event loop
// -----------------------------------
void loop()
{

  // ------------------------------------------------
  // Update GUI Elements
  // ------------------------------------------------

  // Slow down updates
  delay(10);

  // General counter
  m_nCount++;
  gslc_ElemXProgressSetVal(&m_gui, m_pElemProgress, ((m_nCount / 1) % 100));


  // ------------------------------------------------
  // Periodically call GUIslice update function
  // ------------------------------------------------
  gslc_Update(&m_gui);

}
