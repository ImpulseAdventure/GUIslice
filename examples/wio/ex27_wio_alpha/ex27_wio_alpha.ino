//
// GUIslice Library Example
// - Calvin Hass
// - https://www.impulseadventure.com/elec/guislice-gui.html
// - https://github.com/ImpulseAdventure/GUIslice
// - Example 27 (WIO Terminal):
//   - Keypad input as popup for text entry
//   - NOTE: This is the simple version of the example without
//     optimizing for memory consumption. A "minimal"
//     version is located in the "arduino_min" folder which includes
//     FLASH memory optimization for reduced memory devices.
//
// MODS FROM ex27_ard_alpha:
// - Removed EXTRA FONTS
// - Add EasyButton for external pin input
// - Add InputMap for external pin input
// - Add CbPinPoll() and GPIO pullups
//
// ARDUINO NOTES:
// - GUIslice_config.h must be edited to match the pinout connections
//   between the Arduino CPU and the display controller (see ADAGFX_PIN_*).
//

#include "GUIslice.h"
#include "GUIslice_drv.h"


#include "elem/XKeyPad_Alpha.h"

// Button Control
// - Define the pin connections for the external buttons.
// - On the WIO Terminal, there are 8 buttons that can be mapped:
//   - WIO_KEY_A
//   - WIO_KEY_B
//   - WIO_KEY_C
//   - WIO_SS_UP
//   - WIO_SS_DOWN
//   - WIO_SS_LEFT
//   - WIO_SS_RIGHT
//   - WIO_SS_PRESS
#include "EasyButton.h"
#define PIN_PREV  WIO_KEY_A
#define PIN_SEL   WIO_KEY_B
#define PIN_NEXT  WIO_KEY_C
EasyButton btn_prev(PIN_PREV, 35, true, true);
EasyButton btn_sel( PIN_SEL,  35, true, true);
EasyButton btn_next(PIN_NEXT, 35, true, true);

// Ensure config settings are correct for the sketch
#if !(GSLC_FEATURE_INPUT)
  #warning "This sketch requires config: #define GSLC_FEATURE_INPUT 1"
#endif

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
  E_BOX1,
  E_TXT_VAL1, E_ELEM_KEYPAD
};
enum { E_FONT_SANS2, E_FONT_TXT1, MAX_FONT }; // Use separate enum for fonts, MAX_FONT at end
//<Enum !End!>

// ------------------------------------------------
// Instantiate the GUI
// ------------------------------------------------

// Define the maximum number of elements per page
//<ElementDefines !Start!>
#define MAX_PAGE                2
#define MAX_ELEM_PG_MAIN 3
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

gslc_tsXKeyPad              m_sKeyPadAlpha; // Keypad 

//<GUI_Extra_Elements !End!>

// Pin mapping table
#define MAX_INPUT_MAP       4
gslc_tsInputMap             m_asInputMap[MAX_INPUT_MAP];

// ------------------------------------------------
// Save some element references for update loop access
// ------------------------------------------------
//<Save_References !Start!>
gslc_tsElemRef*  m_pElemVal1 = NULL;
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
  gslc_tsGui* pGui = (gslc_tsGui*)pvGui;
  gslc_tsElem* pElem = gslc_GetElemFromRef(pGui,pElemRef);


  if (eTouch == GSLC_TOUCH_UP_IN) {
    // From the element's ID we can determine which button was pressed.
    switch (pElem->nId) {
      //<Button Enums !Start!>
    case E_TXT_VAL1:
      // Clicked on edit field, so show popup box and associate with this text field
      gslc_ElemXKeyPadInputAsk(pGui, m_pElemKeyPad, E_POP_KEYPAD, m_pElemVal1);
      break;
      //<Button Enums !End!>
    default:
      break;
    }
  }
  return true;
}

// Pin Input polling callback function
bool CbPinPoll(void* pvGui, int16_t* pnPinInd, int16_t* pnPinVal)
{
  // Sample all pin inputs
  btn_prev.read();
  btn_sel.read();
  btn_next.read();

  // Determine if any pin edge events occur
  // - If multiple pin events occur, they will be handled in consecutive CbPinPoll() calls
  if      (btn_prev.wasPressed())  { *pnPinInd = PIN_PREV; *pnPinVal = 1; }
  else if (btn_prev.wasReleased()) { *pnPinInd = PIN_PREV; *pnPinVal = 0; }
  else if (btn_sel.wasPressed())   { *pnPinInd = PIN_SEL;  *pnPinVal = 1; }
  else if (btn_sel.wasReleased())  { *pnPinInd = PIN_SEL;  *pnPinVal = 0; }
  else if (btn_next.wasPressed())  { *pnPinInd = PIN_NEXT; *pnPinVal = 1; }
  else if (btn_next.wasReleased()) { *pnPinInd = PIN_NEXT; *pnPinVal = 0; }
  else return false; // No pin event detected

  // If we reach here, then a pin event was detected
  return true;
}

// KeyPad Input Ready callback
bool CbInputCommon(void* pvGui, void *pvElemRef, int16_t nState, void* pvData)
{
  gslc_tsElemRef* pElemRef = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsGui* pGui = (gslc_tsGui*)pvGui;
  gslc_tsElem* pElem = gslc_GetElemFromRef(pGui,pElemRef);

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
        gslc_PopupHide(pGui);
      }
      else {
        // ERROR
      }
      break;
    case XKEYPAD_CB_STATE_CANCEL:
      // User escaped from popup, so don't update values
      gslc_PopupHide(pGui);
      break;

    case XKEYPAD_CB_STATE_UPDATE:
      // KeyPad was updated, so could optionally take action here
      break;

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
  gslc_PageAdd(&m_gui, E_POP_KEYPAD, m_asPopKeypadElem, MAX_ELEM_POP_KEYPAD_RAM, m_asPopKeypadElemRef, MAX_ELEM_POP_KEYPAD);

  // Background flat color
  gslc_SetBkgndColor(&m_gui, GSLC_COL_BLACK);

  // -----------------------------------
  // PAGE: E_PG_MAIN

  // Create GSLC_ID_AUTO text label
  pElemRef = gslc_ElemCreateTxt(&m_gui, GSLC_ID_AUTO, E_PG_MAIN, (gslc_tsRect) { 90, 10, 134, 32 },
    (char*)"Alpha KeyPad", 0, E_FONT_SANS2);
  gslc_ElemSetTxtCol(&m_gui, pElemRef, GSLC_COL_BLUE_LT4);

  // Create GSLC_ID_AUTO text label
  pElemRef = gslc_ElemCreateTxt(&m_gui, GSLC_ID_AUTO, E_PG_MAIN, (gslc_tsRect) { 20, 65, 62, 17 },
    (char*)"Name:", 0, E_FONT_TXT1);

  // Create E_TXT_VAL1 modifiable text label
  static char m_strtxt5[11] = "";
  pElemRef = gslc_ElemCreateTxt(&m_gui, E_TXT_VAL1, E_PG_MAIN, (gslc_tsRect) { 90, 65, 62+10, 17 },
    (char*)m_strtxt5, 11, E_FONT_TXT1);
  gslc_ElemSetCol(&m_gui, pElemRef, GSLC_COL_BLUE_DK1, GSLC_COL_BLACK, GSLC_COL_BLUE_DK4);
  gslc_ElemSetTxtCol(&m_gui, pElemRef, GSLC_COL_WHITE);
  gslc_ElemSetFrameEn(&m_gui, pElemRef, true);
  gslc_ElemSetTxtMargin(&m_gui, pElemRef, 5);
  gslc_ElemSetClickEn(&m_gui, pElemRef, true);
  gslc_ElemSetTouchFunc(&m_gui, pElemRef, &CbBtnCommon);
  m_pElemVal1 = pElemRef; // Save for later


  // -----------------------------------
  // PAGE: E_POP_KEYPAD
  static gslc_tsXKeyPadCfg_Alpha sCfg;
  sCfg = gslc_ElemXKeyPadCfgInit_Alpha();
  //gslc_ElemXKeyPadCfgSetButtonSz((gslc_tsXKeyPadCfg*)&sCfg, 12, 25);
  m_pElemKeyPad = gslc_ElemXKeyPadCreate_Alpha(&m_gui, E_ELEM_KEYPAD, E_POP_KEYPAD,
    &m_sKeyPadAlpha, 50, 80, E_FONT_TXT1, &sCfg);
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

  // Initialize pins controls. Note that this calls pinMode()
  btn_prev.begin();
  btn_sel.begin();
  btn_next.begin();

  // Set the pin poll callback function
  gslc_SetPinPollFunc(&m_gui, CbPinPoll);
  
  // Create the GUI input mapping (pin event to GUI action)
  gslc_InitInputMap(&m_gui, m_asInputMap, MAX_INPUT_MAP);
  gslc_InputMapAdd(&m_gui, GSLC_INPUT_PIN_DEASSERT, PIN_PREV,     GSLC_ACTION_FOCUS_PREV, 0);
  gslc_InputMapAdd(&m_gui, GSLC_INPUT_PIN_ASSERT,   PIN_SEL,      GSLC_ACTION_PRESELECT, 0);
  gslc_InputMapAdd(&m_gui, GSLC_INPUT_PIN_DEASSERT, PIN_SEL,      GSLC_ACTION_SELECT, 0);
  gslc_InputMapAdd(&m_gui, GSLC_INPUT_PIN_DEASSERT, PIN_NEXT,     GSLC_ACTION_FOCUS_NEXT, 0);  

  
  // ------------------------------------------------
  // Load Fonts
  // ------------------------------------------------
  //<Load_Fonts !Start!>
  if (!gslc_FontSet(&m_gui, E_FONT_SANS2, GSLC_FONTREF_PTR, NULL, 1)) { return; }
  if (!gslc_FontSet(&m_gui,E_FONT_TXT1,GSLC_FONTREF_PTR,NULL,1)) { return; }
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

