//<File !Start!>
// FILE: [ex27_bld_alpha.ino]
// Created by GUIslice Builder version: [0.13.0]
//
// GUIslice Builder Generated File
//
// For the latest guides, updates and support view:
// https://github.com/ImpulseAdventure/GUIslice
//
//<File !End!>
//
// - Example 27 (Arduino):
//   - Keypad input as popup for text entry
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

// ------------------------------------------------
// Headers to include
// ------------------------------------------------
#include "GUIslice.h"
#include "GUIslice_drv.h"

// Include any extended elements
//<Includes !Start!>
// Include extended elements
#include "elem/XKeyPad_Alpha.h"

// Ensure optional features are enabled in the configuration
#if !(GSLC_FEATURE_COMPOUND)
  #error "Config: GSLC_FEATURE_COMPOUND required for this program but not enabled. Please update GUIslice/config."
#endif
//<Includes !End!>

// ------------------------------------------------
// Headers and Defines for fonts
// Note that font files are located within the Adafruit-GFX library folder:
// ------------------------------------------------
//<Fonts !Start!>
#include <Adafruit_GFX.h>
// Note that these files are located within the Adafruit-GFX library folder:
#include "Fonts/FreeMono12pt7b.h"
//<Fonts !End!>

// ------------------------------------------------
// Defines for resources
// ------------------------------------------------
//<Resources !Start!>
//<Resources !End!>

// ------------------------------------------------
// Enumerations for pages, elements, fonts, images
// ------------------------------------------------
//<Enum !Start!>
enum {E_PG_MAIN,E_POP_KEYPAD};
enum {E_LBL_NAME,E_LBL_TITLE,E_TXT_VAL1,E_ELEM_KEYPAD};
// Must use separate enum for fonts with MAX_FONT at end to use gslc_FontSet.
enum {E_FONT_MONO12,E_FONT_TXT5,MAX_FONT};
//<Enum !End!>

// ------------------------------------------------
// Instantiate the GUI
// ------------------------------------------------

// ------------------------------------------------
// Define the maximum number of elements and pages
// ------------------------------------------------
//<ElementDefines !Start!>
#define MAX_PAGE                2

#define MAX_ELEM_PG_MAIN 3                                          // # Elems total on page
#define MAX_ELEM_PG_MAIN_RAM MAX_ELEM_PG_MAIN // # Elems in RAM
//<ElementDefines !End!>

// ------------------------------------------------
// Create element storage
// ------------------------------------------------
gslc_tsGui                      m_gui;
gslc_tsDriver                   m_drv;
gslc_tsFont                     m_asFont[MAX_FONT];
gslc_tsPage                     m_asPage[MAX_PAGE];

//<GUI_Extra_Elements !Start!>
gslc_tsElem                     m_asPage1Elem[MAX_ELEM_PG_MAIN_RAM];
gslc_tsElemRef                  m_asPage1ElemRef[MAX_ELEM_PG_MAIN];
gslc_tsElem                     m_asKeypadElem[1];
gslc_tsElemRef                  m_asKeypadElemRef[1];
gslc_tsXKeyPad_Alpha            m_sKeyPad;

#define MAX_STR                 100

//<GUI_Extra_Elements !End!>

// ------------------------------------------------
// Program Globals
// ------------------------------------------------

// Save some element references for direct access
//<Save_References !Start!>
gslc_tsElemRef*  m_pElemVal1       = NULL;
gslc_tsElemRef*  m_pElemKeyPad     = NULL;
//<Save_References !End!>

// Define debug message function
static int16_t DebugOut(char ch) { if (ch == (char)'\n') Serial.println(""); else Serial.write(ch); return 0; }

// ------------------------------------------------
// Callback Methods
// ------------------------------------------------
// Common Button callback
bool CbBtnCommon(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY)
{
  gslc_tsElemRef* pElemRef = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem* pElem = pElemRef->pElem;

  if ( eTouch == GSLC_TOUCH_UP_IN ) {
    // From the element's ID we can determine which button was pressed.
    switch (pElem->nId) {
//<Button Enums !Start!>

      case E_TXT_VAL1:
        //TODO- Check the code to see what else you may need to add
        // Clicked on edit field, so show popup box and associate with this text field
        gslc_ElemXKeyPadTargetIdSet(&m_gui, m_pElemKeyPad, E_TXT_VAL1);
        gslc_PopupShow(&m_gui, E_POP_KEYPAD, true);
        // Preload current value
        gslc_ElemXKeyPadValSet(&m_gui, m_pElemKeyPad, gslc_ElemGetTxtStr(&m_gui, m_pElemVal1));
        break;
//<Button Enums !End!>
      default:
        break;
    }
  }
  return true;
}
//<Checkbox Callback !Start!>
//<Checkbox Callback !End!>
// KeyPad Input Ready callback
bool CbKeypad(void* pvGui, void *pvElemRef, int16_t nState, void* pvData)
{
  gslc_tsElemRef* pElemRef = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem* pElem = pElemRef->pElem;
  gslc_tsGui* pGui = (gslc_tsGui*)pvGui;

  // From the element's ID we can determine which element is ready.
  if (pElem->nId == E_ELEM_KEYPAD) {
    int16_t nTargetElemId = gslc_ElemXKeyPadDataTargetIdGet(pGui, pvData);
    if (nState == XKEYPAD_CB_STATE_DONE) {
      // User clicked on Enter to leave popup
      // - If we have a popup active, pass the return value directly to
      //   the corresponding value field
      switch (nTargetElemId) {
//<Keypad Enums !Start!>
        case E_TXT_VAL1:
          //TODO- Update input handling code
          // using gslc_ElemXKeyPadDataValGet(pGui, pvData)
          gslc_ElemSetTxtStr(pGui, m_pElemVal1, 
          gslc_ElemXKeyPadDataValGet(pGui, pvData));
          gslc_PopupHide(&m_gui);
        break;

//<Keypad Enums !End!>
        default:
          break;
      }
    } else if (nState == XKEYPAD_CB_STATE_CANCEL) {
      // User escaped from popup, so don't update values
      gslc_PopupHide(&m_gui);
    }
  }
}
//<Spinner Callback !Start!>
//<Spinner Callback !End!>
//<Listbox Callback !Start!>
//<Listbox Callback !End!>
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
  gslc_PageAdd(&m_gui,E_PG_MAIN,m_asPage1Elem,MAX_ELEM_PG_MAIN_RAM,m_asPage1ElemRef,MAX_ELEM_PG_MAIN);
  gslc_PageAdd(&m_gui,E_POP_KEYPAD,m_asKeypadElem,1,m_asKeypadElemRef,1);  // KeyPad

  // NOTE: The current page defaults to the first page added. Here we explicitly
  //       ensure that the main page is the correct page no matter the add order.
  gslc_SetPageCur(&m_gui,E_PG_MAIN);
  
  // Set Background to a flat color
  gslc_SetBkgndColor(&m_gui,GSLC_COL_BLACK);

  // -----------------------------------
  // PAGE: E_PG_MAIN
  
  
  // Create E_LBL_TITLE text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_LBL_TITLE,E_PG_MAIN,(gslc_tsRect){73,10,173,33},
    (char*)"Alpha KeyPad",0,E_FONT_MONO12);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_BLUE_LT4);
  
  // Create E_LBL_NAME text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_LBL_NAME,E_PG_MAIN,(gslc_tsRect){20,65,32,12},
    (char*)"Name:",0,E_FONT_TXT5);
  
  // Create E_TXT_VAL1 text input field
  static char m_sInputText1[11] = "";
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_TXT_VAL1,E_PG_MAIN,(gslc_tsRect){90,65,65,12},
    (char*)m_sInputText1,11,E_FONT_TXT5);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_WHITE);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_BLUE_DK1,GSLC_COL_BLACK,GSLC_COL_BLUE_DK4);
  gslc_ElemSetFrameEn(&m_gui,pElemRef,true);
  gslc_ElemSetClickEn(&m_gui, pElemRef, true);
  gslc_ElemSetTouchFunc(&m_gui, pElemRef, &CbBtnCommon);
  m_pElemVal1 = pElemRef;

  // -----------------------------------
  // PAGE: E_POP_KEYPAD
  
  gslc_tsXKeyPadCfg sCfgTx = gslc_ElemXKeyPadCfgInit_Alpha();
  gslc_ElemXKeyPadCfgSetButtonSz(&sCfgTx, 20, 20);
  gslc_ElemXKeyPadCfgSetRoundEn(&sCfgTx, false);
  m_pElemKeyPad = gslc_ElemXKeyPadCreate_Alpha(&m_gui, E_ELEM_KEYPAD, E_POP_KEYPAD,
    &m_sKeyPad, 65, 80, E_FONT_TXT5, &sCfgTx);
  gslc_ElemXKeyPadValSetCb(&m_gui, m_pElemKeyPad, &CbKeypad);
  
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

  if (!gslc_Init(&m_gui,&m_drv,m_asPage,MAX_PAGE,m_asFont,MAX_FONT)) { return; }

  // ------------------------------------------------
  // Load Fonts
  // ------------------------------------------------
//<Load_Fonts !Start!>
    if (!gslc_FontSet(&m_gui,E_FONT_MONO12,GSLC_FONTREF_PTR,&FreeMono12pt7b,1)) { return; }
    if (!gslc_FontSet(&m_gui,E_FONT_TXT5,GSLC_FONTREF_PTR,NULL,1)) { return; }
//<Load_Fonts !End!>

  // ------------------------------------------------
  // Create graphic elements
  // ------------------------------------------------
  InitGUI();

//<Startup !Start!>
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
  
  //TODO - Add update code for any text, gauges, or sliders
  
  // ------------------------------------------------
  // Periodically call GUIslice update function
  // ------------------------------------------------
  gslc_Update(&m_gui);
    
}
