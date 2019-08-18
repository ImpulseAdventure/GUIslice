//<File !Start!>
// FILE: [ex25_bld_popup.ino]
// Created by GUIslice Builder version: [0.13.0]
//
// GUIslice Builder Generated File
//
// For the latest guides, updates and support view:
// https://github.com/ImpulseAdventure/GUIslice
//
//<File !End!>
//
// - Example 25 (Arduino):
//   - Popup dialog to confirm QUIT
//   - Use of EXTRA Fonts 
//   - Accept touch input, text button
//   - Expected behavior: Clicking on button terminates program
//   - NOTE: This is the simple version of the example without
//     optimizing for memory consumption. 
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
#include "elem/XProgress.h"
//<Includes !End!>

// ------------------------------------------------
// Headers and Defines for fonts
// Note that font files are located within the Adafruit-GFX library folder:
// ------------------------------------------------
//<Fonts !Start!>
#include <Adafruit_GFX.h>
// Note that these files are located within the Adafruit-GFX library folder:
#include "Fonts/FreeSans9pt7b.h"
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
enum {E_PG_MAIN,E_PG_POPUP};
enum {E_BTN_CANCEL,E_BTN_OK,E_BTN_QUIT,E_ELEM_BOX1,E_ELEM_TEXT2
      ,E_ELEM_TEXT3,E_PROGRESS,E_TXT_STATUS};
// Must use separate enum for fonts with MAX_FONT at end to use gslc_FontSet.
enum {E_FONT_SANS9,E_FONT_TXT10,MAX_FONT};
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

#define MAX_ELEM_PG_POPUP 5                                          // # Elems total on page
#define MAX_ELEM_PG_POPUP_RAM MAX_ELEM_PG_POPUP // # Elems in RAM
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
gslc_tsElem                     m_asPopup1Elem[MAX_ELEM_PG_POPUP_RAM];
gslc_tsElemRef                  m_asPopup1ElemRef[MAX_ELEM_PG_POPUP];
gslc_tsXProgress                m_sXBarGauge1;

#define MAX_STR                 100

//<GUI_Extra_Elements !End!>

// ------------------------------------------------
// Program Globals
// ------------------------------------------------
// Flag to stop program
bool m_bQuit = false;

// Free-running counter for display
unsigned    m_nCount = 0;

// Save some element references for direct access
//<Save_References !Start!>
gslc_tsElemRef*  m_pElemProgress   = NULL;
gslc_tsElemRef*  m_pElemQuit       = NULL;
gslc_tsElemRef*  m_pTxtStatus      = NULL;
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
      case E_BTN_OK:
        gslc_PopupHide(&m_gui);
        m_bQuit = true;
        gslc_ElemSetTxtStr(&m_gui, m_pTxtStatus, "Stopped!");
        gslc_ElemSetTxtStr(&m_gui,m_pElemQuit,"DONE");
        gslc_ElemSetCol(&m_gui,m_pElemQuit,GSLC_COL_RED,GSLC_COL_BLACK,GSLC_COL_BLACK);
       break;
      case E_BTN_CANCEL:
        gslc_PopupHide(&m_gui);
        gslc_ElemSetTxtStr(&m_gui, m_pTxtStatus, "Keep Running!");
        break;

      case E_BTN_QUIT:
        gslc_PopupShow(&m_gui, E_PG_POPUP, true);
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
//<Keypad Callback !Start!>
//<Keypad Callback !End!>
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
  gslc_PageAdd(&m_gui,E_PG_POPUP,m_asPopup1Elem,MAX_ELEM_PG_POPUP_RAM,m_asPopup1ElemRef,MAX_ELEM_PG_POPUP);

  // NOTE: The current page defaults to the first page added. Here we explicitly
  //       ensure that the main page is the correct page no matter the add order.
  gslc_SetPageCur(&m_gui,E_PG_MAIN);
  
  // Set Background to a flat color
  gslc_SetBkgndColor(&m_gui,GSLC_COL_GRAY_DK2);

  // -----------------------------------
  // PAGE: E_PG_MAIN
  
  
  // Create E_BTN_QUIT button with modifiable text label
  static char m_strbtn1[7] = "Quit";
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_BTN_QUIT,E_PG_MAIN,
    (gslc_tsRect){80,130,80,40},
    (char*)m_strbtn1,7,E_FONT_SANS9,&CbBtnCommon);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_WHITE);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_RED,GSLC_COL_BLUE_DK4,GSLC_COL_BLUE_DK1);
  gslc_ElemSetFrameEn(&m_gui,pElemRef,true);
  m_pElemQuit = pElemRef;
  
  // Create E_TXT_STATUS runtime modifiable text
  static char m_sDisplayText1[18] = "Program Running";
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_TXT_STATUS,E_PG_MAIN,(gslc_tsRect){27,21,190,20},
    (char*)m_sDisplayText1,18,E_FONT_TXT10);
  gslc_ElemSetTxtAlign(&m_gui,pElemRef,GSLC_ALIGN_MID_MID);
  m_pTxtStatus = pElemRef;

  // Create progress bar E_PROGRESS 
  pElemRef = gslc_ElemXProgressCreate(&m_gui,E_PROGRESS,E_PG_MAIN,&m_sXBarGauge1,
    (gslc_tsRect){70,68,100,12},0,100,0,GSLC_COL_GREEN,false);
  m_pElemProgress = pElemRef;

  // -----------------------------------
  // PAGE: E_PG_POPUP
  
   
  // Create E_ELEM_BOX1 box
  pElemRef = gslc_ElemCreateBox(&m_gui,E_ELEM_BOX1,E_PG_POPUP,(gslc_tsRect){20,80,200,150});
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_RED,GSLC_COL_GRAY_LT2,GSLC_COL_BLACK);
  
  // create E_BTN_OK button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_BTN_OK,E_PG_POPUP,
    (gslc_tsRect){130,170,80,40},(char*)"YES",0,E_FONT_SANS9,&CbBtnCommon);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_WHITE);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_RED,GSLC_COL_BLUE_LT2,GSLC_COL_BLUE_LT4);
  gslc_ElemSetFrameEn(&m_gui,pElemRef,true);
  
  // create E_BTN_CANCEL button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_BTN_CANCEL,E_PG_POPUP,
    (gslc_tsRect){30,170,80,40},(char*)"NO",0,E_FONT_SANS9,&CbBtnCommon);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_WHITE);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_RED,GSLC_COL_BLUE_LT2,GSLC_COL_BLUE_LT4);
  gslc_ElemSetFrameEn(&m_gui,pElemRef,true);
  
  // Create E_ELEM_TEXT2 text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TEXT2,E_PG_POPUP,(gslc_tsRect){43,100,153,23},
    (char*)"Do you really want",0,E_FONT_SANS9);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_BLACK);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_GRAY_LT2,GSLC_COL_GRAY_LT2,GSLC_COL_GRAY_LT2);
  
  // Create E_ELEM_TEXT3 text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TEXT3,E_PG_POPUP,(gslc_tsRect){86,123,68,23},
    (char*)"to Quit?",0,E_FONT_SANS9);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_BLACK);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_GRAY_LT2,GSLC_COL_GRAY_LT2,GSLC_COL_GRAY_LT2);
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
    if (!gslc_FontSet(&m_gui,E_FONT_SANS9,GSLC_FONTREF_PTR,&FreeSans9pt7b,1)) { return; }
    if (!gslc_FontSet(&m_gui,E_FONT_TXT10,GSLC_FONTREF_PTR,NULL,2)) { return; }
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
  
  // General counter
  m_nCount++;
  gslc_ElemXProgressSetVal(&m_gui, m_pElemProgress, ((m_nCount / 1) % 100));

  // Slow down updates
  delay(10);

  // ------------------------------------------------
  // Periodically call GUIslice update function
  // ------------------------------------------------
  gslc_Update(&m_gui);

  // In a real program, we would detect the button press and take an action.
  // For this Arduino demo, we will pretend to exit by emulating it with an
  // infinite loop. Note that interrupts are not disabled so that any debug
  // messages via Serial have an opportunity to be transmitted.
  if (m_bQuit) {
    gslc_Quit(&m_gui);
    while (1) {}
  }
  
}
