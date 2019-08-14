//
// GUIslice Library Example
// - Calvin Hass
// - https://www.impulseadventure.com/elec/guislice-gui.html
// - https://github.com/ImpulseAdventure/GUIslice
// - Example 26 (Arduino):
//   - Keypad input as popup for simple calculator
//   - Use of EXTRA Fonts 
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

// Ensure optional compound element feature is enabled in the configuration
// - Required by XKeyPad component
#if !(GSLC_FEATURE_COMPOUND)
  #error "Config: GSLC_FEATURE_COMPOUND required for this example but not enabled. Please update GUIslice_config."
#endif

#include "elem/XKeyPad_Num.h"

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
  #elif defined(DRV_DISP_ADAGFX_ILI9341_T3) // Teensy
    #include <font_Arial.h>
    #define FONT_NAME1 &Arial_12
    #define SET_FONT_MODE1 // Enable Teensy extra fonts
  #else // Arduino, etc.
    #include <Adafruit_GFX.h>
    #include <gfxfont.h>
    #include "Fonts/FreeSansBold12pt7b.h"
    #define FONT_NAME1 &FreeSansBold12pt7b
  #endif
#else
  // Use the default font
  #define FONT_NAME1 NULL
#endif
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
enum { E_PG_MAIN, E_POP_KEYPAD };
enum {
  E_BOX1, E_BTN_ADD, E_BTN_MULT, E_BTN_SUB
  , E_TXT_RESULT, E_TXT_VAL1, E_TXT_VAL2, E_ELEM_KEYPAD
};
enum { E_FONT_SANS2, E_FONT_TXT1, MAX_FONT }; // Use separate enum for fonts, MAX_FONT at end
//<Enum !End!>

// ------------------------------------------------
// Instantiate the GUI
// ------------------------------------------------

// Define the maximum number of elements per page
//<ElementDefines !Start!>
#define MAX_PAGE                2
#define MAX_ELEM_PG_MAIN 11
#define MAX_ELEM_PG_MAIN_RAM MAX_ELEM_PG_MAIN
#define MAX_ELEM_POP_KEYPAD 1
#define MAX_ELEM_POP_KEYPAD_RAM MAX_ELEM_POP_KEYPAD
//<ElementDefines !End!>

// GUI Elements
gslc_tsGui                      m_gui;
gslc_tsDriver                   m_drv;
gslc_tsFont                     m_asFont[MAX_FONT];
gslc_tsPage                     m_asPage[MAX_PAGE];

//<GUI_Extra_Elements !Start!>
gslc_tsElem                 m_asPage1Elem[MAX_ELEM_PG_MAIN_RAM];
gslc_tsElemRef              m_asPage1ElemRef[MAX_ELEM_PG_MAIN];
gslc_tsElem                 m_asPopKeypadElem[MAX_ELEM_POP_KEYPAD_RAM];
gslc_tsElemRef              m_asPopKeypadElemRef[MAX_ELEM_POP_KEYPAD];

gslc_tsXKeyPad_Num          m_sKeyPadNum; // Keypad 

#define MAX_STR                 100

//<GUI_Extra_Elements !End!>

// ------------------------------------------------
// Save some element references for update loop access
// ------------------------------------------------
//<Save_References !Start!>
gslc_tsElemRef*  m_pElemVal1 = NULL;
gslc_tsElemRef*  m_pElemVal2 = NULL;
gslc_tsElemRef*  m_pElemResult = NULL;
gslc_tsElemRef*  m_pElemKeyPad = NULL;
//<Save_References !End!>


// ------------------------------------------------
// Program Globals
// ------------------------------------------------

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
  gslc_tsGui* pGui = (gslc_tsGui*)pvGui;

  char acTxtNum[11];
  int32_t nVal1, nVal2, nResult;

  if (eTouch == GSLC_TOUCH_UP_IN) {
    // From the element's ID we can determine which button was pressed.
    switch (pElem->nId) {
      //<Button Enums !Start!>
    case E_TXT_VAL1:
      // Clicked on edit field, so show popup box and associate with this text field
      gslc_ElemXKeyPadTargetIdSet(&m_gui, m_pElemKeyPad, E_TXT_VAL1);
      gslc_PopupShow(&m_gui, E_POP_KEYPAD, true);
      // Preload current value
      gslc_ElemXKeyPadValSet(&m_gui, m_pElemKeyPad, gslc_ElemGetTxtStr(&m_gui, m_pElemVal1));
      break;
    case E_TXT_VAL2:
      // Clicked on edit field, so show popup box and associate with this text field
      gslc_ElemXKeyPadTargetIdSet(&m_gui, m_pElemKeyPad, E_TXT_VAL2);
      gslc_PopupShow(&m_gui, E_POP_KEYPAD, true);
      // Preload current value
      gslc_ElemXKeyPadValSet(&m_gui, m_pElemKeyPad, gslc_ElemGetTxtStr(&m_gui, m_pElemVal2));
      break;
    case E_BTN_ADD:
      // Compute the sum and update the result
      nVal1 = atol(gslc_ElemGetTxtStr(&m_gui, m_pElemVal1));
      nVal2 = atol(gslc_ElemGetTxtStr(&m_gui, m_pElemVal2));
      nResult = nVal1 + nVal2;
      ltoa(nResult, acTxtNum, 10);
      gslc_ElemSetTxtStr(&m_gui, m_pElemResult, acTxtNum);
      break;
    case E_BTN_SUB:
      // Compute the subtraction and update the result
      nVal1 = atol(gslc_ElemGetTxtStr(&m_gui, m_pElemVal1));
      nVal2 = atol(gslc_ElemGetTxtStr(&m_gui, m_pElemVal2));
      nResult = nVal1 - nVal2;
      ltoa(nResult, acTxtNum, 10);
      gslc_ElemSetTxtStr(&m_gui, m_pElemResult, acTxtNum);
      break;
    case E_BTN_MULT:
      // Compute the multiplication and update the result
      nVal1 = atol(gslc_ElemGetTxtStr(&m_gui, m_pElemVal1));
      nVal2 = atol(gslc_ElemGetTxtStr(&m_gui, m_pElemVal2));
      nResult = nVal1 * nVal2;
      ltoa(nResult, acTxtNum, 10);
      gslc_ElemSetTxtStr(&m_gui, m_pElemResult, acTxtNum);
      break;
      //<Button Enums !End!>
    default:
      break;
    }
  }
  return true;
}

// KeyPad Input Ready callback
bool CbInputCommon(void* pvGui, void *pvElemRef, int16_t nState, void* pvData)
{
  gslc_tsElemRef* pElemRef = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem* pElem = pElemRef->pElem;
  gslc_tsGui* pGui = (gslc_tsGui*)pvGui;

  char acTxtNum[11];
  // From the element's ID we can determine which element is ready.
  if (pElem->nId == E_ELEM_KEYPAD) {
    int16_t nTargetElemId = gslc_ElemXKeyPadDataTargetIdGet(pGui, pvData);
    switch (nState) {
    case XKEYPAD_CB_STATE_DONE:
      // User clicked on Enter to leave popup
      // - If we have a popup active, pass the return value directly to
      //   the corresponding value field
      if (nTargetElemId == E_TXT_VAL1) {
        gslc_ElemSetTxtStr(pGui, m_pElemVal1, gslc_ElemXKeyPadDataValGet(pGui, pvData));
        gslc_PopupHide(&m_gui);
      }
      else if (nTargetElemId == E_TXT_VAL2) {
        gslc_ElemSetTxtStr(pGui, m_pElemVal2, gslc_ElemXKeyPadDataValGet(pGui, pvData));
        gslc_PopupHide(&m_gui);
      }
      else {
        // ERROR
      }
      break;
    case XKEYPAD_CB_STATE_CANCEL:
      // User escaped from popup, so don't update values
      gslc_PopupHide(&m_gui);
      break;

    case XKEYPAD_CB_STATE_UPDATE:
      // KeyPad was updated, so could optionally take action here
      break;

    default:
      break;
    }
  }
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
  gslc_PageAdd(&m_gui, E_POP_KEYPAD, m_asPopKeypadElem, MAX_ELEM_POP_KEYPAD_RAM, m_asPopKeypadElemRef, MAX_ELEM_POP_KEYPAD);

  // Background flat color
  gslc_SetBkgndColor(&m_gui, GSLC_COL_BLACK);

  // -----------------------------------
  // PAGE: E_PG_MAIN

  // Create GSLC_ID_AUTO text label
  pElemRef = gslc_ElemCreateTxt(&m_gui, GSLC_ID_AUTO, E_PG_MAIN, (gslc_tsRect) { 90, 10, 134, 32 },
    (char*)"Simple Calc", 0, E_FONT_SANS2);
  gslc_ElemSetTxtCol(&m_gui, pElemRef, GSLC_COL_BLUE_LT4);

  // Create GSLC_ID_AUTO text label
  pElemRef = gslc_ElemCreateTxt(&m_gui, GSLC_ID_AUTO, E_PG_MAIN, (gslc_tsRect) { 20, 65, 62, 17 },
    (char*)"Value 1:", 0, E_FONT_TXT1);

  // Create GSLC_ID_AUTO text label
  pElemRef = gslc_ElemCreateTxt(&m_gui, GSLC_ID_AUTO, E_PG_MAIN, (gslc_tsRect) { 20, 90, 62, 17 },
    (char*)"Value 2:", 0, E_FONT_TXT1);

  // Create GSLC_ID_AUTO text label
  pElemRef = gslc_ElemCreateTxt(&m_gui, GSLC_ID_AUTO, E_PG_MAIN, (gslc_tsRect) { 80, 200, 62, 12 },
    (char*)"Result:", 0, E_FONT_TXT1);

  // Create E_BOX1 box
  pElemRef = gslc_ElemCreateBox(&m_gui, E_BOX1, E_PG_MAIN, (gslc_tsRect) { 10, 120, 300, 60 });

  // Create E_TXT_VAL1 modifiable text label
  static char m_strtxt5[11] = "";
  pElemRef = gslc_ElemCreateTxt(&m_gui, E_TXT_VAL1, E_PG_MAIN, (gslc_tsRect) { 90, 65, 62, 17 },
    (char*)m_strtxt5, 11, E_FONT_TXT1);
  gslc_ElemSetCol(&m_gui, pElemRef, GSLC_COL_BLUE_DK1, GSLC_COL_BLACK, GSLC_COL_BLUE_DK4);
  gslc_ElemSetTxtCol(&m_gui, pElemRef, GSLC_COL_WHITE);
  gslc_ElemSetFrameEn(&m_gui, pElemRef, true);
  gslc_ElemSetTxtMargin(&m_gui, pElemRef, 5);
  gslc_ElemSetClickEn(&m_gui, pElemRef, true);
  gslc_ElemSetTouchFunc(&m_gui, pElemRef, &CbBtnCommon);
  m_pElemVal1 = pElemRef; // Save for later

  // Create E_TXT_VAL2 modifiable text label
  static char m_strtxt6[11] = "";
  pElemRef = gslc_ElemCreateTxt(&m_gui, E_TXT_VAL2, E_PG_MAIN, (gslc_tsRect) { 90, 90, 62, 17 },
    (char*)m_strtxt6, 11, E_FONT_TXT1);
  gslc_ElemSetCol(&m_gui, pElemRef, GSLC_COL_BLUE_DK1, GSLC_COL_BLACK, GSLC_COL_BLUE_DK4);
  gslc_ElemSetTxtCol(&m_gui, pElemRef, GSLC_COL_WHITE);
  gslc_ElemSetFrameEn(&m_gui, pElemRef, true);
  gslc_ElemSetTxtMargin(&m_gui, pElemRef, 5);
  gslc_ElemSetClickEn(&m_gui, pElemRef, true);
  gslc_ElemSetTouchFunc(&m_gui, pElemRef, &CbBtnCommon);
  m_pElemVal2 = pElemRef; // Save for later

  // Create E_TXT_RESULT modifiable text label
  static char m_strResult[11] = "";
  pElemRef = gslc_ElemCreateTxt(&m_gui, E_TXT_RESULT, E_PG_MAIN, (gslc_tsRect) { 150, 200, 62, 12 },
    (char*)m_strResult, 11, E_FONT_TXT1);
  gslc_ElemSetTxtCol(&m_gui, pElemRef, GSLC_COL_GREEN);
  m_pElemResult = pElemRef; // Save for later

  // create E_BTN_ADD button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui, E_BTN_ADD, E_PG_MAIN,
    (gslc_tsRect) { 30, 140, 60, 20 }, (char*)"Add", 0, E_FONT_TXT1, &CbBtnCommon);
  gslc_ElemSetFrameEn(&m_gui, pElemRef, true);

  // create E_BTN_SUB button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui, E_BTN_SUB, E_PG_MAIN,
    (gslc_tsRect) { 130, 140, 60, 20 }, (char*)"Subtract", 0, E_FONT_TXT1, &CbBtnCommon);
  gslc_ElemSetFrameEn(&m_gui, pElemRef, true);

  // create E_BTN_MULT button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui, E_BTN_MULT, E_PG_MAIN,
    (gslc_tsRect) { 220, 140, 60, 20 }, (char*)"Multiply", 0, E_FONT_TXT1, &CbBtnCommon);
  gslc_ElemSetFrameEn(&m_gui, pElemRef, true);

  // -----------------------------------
  // PAGE: E_POP_KEYPAD
  gslc_tsXKeyPadCfg sCfg = gslc_ElemXKeyPadCfgInit_Num();
  gslc_ElemXKeyPadCfgSetFloatEn(&sCfg, false);
  gslc_ElemXKeyPadCfgSetSignEn(&sCfg, true);
  gslc_ElemXKeyPadCfgSetButtonSz(&sCfg, 25, 25);
  m_pElemKeyPad = gslc_ElemXKeyPadCreate_Num(&m_gui, E_ELEM_KEYPAD, E_POP_KEYPAD,
    &m_sKeyPadNum, 65, 80, E_FONT_TXT1, &sCfg);
  gslc_ElemXKeyPadValSetCb(&m_gui, m_pElemKeyPad, &CbInputCommon);

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

  // ------------------------------------------------
  // Load Fonts
  // ------------------------------------------------
  //<Load_Fonts !Start!>
  if (!gslc_FontSet(&m_gui, E_FONT_SANS2, GSLC_FONTREF_PTR, FONT_NAME1, 1)) { return; }
  if (!gslc_FontSet(&m_gui, E_FONT_TXT1, GSLC_FONTREF_PTR, NULL, 1)) { return; }
  // Some display drivers need to set a mode to use the extra fonts
  #if defined(SET_FONT_MODE1)
    gslc_FontSetMode(&m_gui, E_FONT_SANS2, GSLC_FONTREF_MODE_1);
  #endif
  //<Load_Fonts !End!>

  // ------------------------------------------------
  // Create graphic elements
  // ------------------------------------------------
  InitGUI();

  // ------------------------------------------------
  // Save some element references for quick access
  // ------------------------------------------------
  //<Quick_Access !Start!>
  //<Quick_Access !End!>

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


  // ------------------------------------------------
  // Periodically call GUIslice update function
  // ------------------------------------------------
  gslc_Update(&m_gui);

}

