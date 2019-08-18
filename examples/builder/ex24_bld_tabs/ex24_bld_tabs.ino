//<File !Start!>
// FILE: [ex24_bld_tabs.ino]
// Created by GUIslice Builder version: [0.13.0]
//
// GUIslice Builder Generated File
//
// For the latest guides, updates and support view:
// https://github.com/ImpulseAdventure/GUIslice
//
//<File !End!>
//
// - Example 24 (Arduino):
//   - Multiple page handling, tab dialog, global (base layer) elements
//     and popup dialog
//   - NOTE: This is the simple version of the example without
//     optimizing for memory consumption. Therefore, it may not
//     run on Arduino devices with limited memory. 
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
#include "elem/XCheckbox.h"
#include "elem/XProgress.h"
//<Includes !End!>

// ------------------------------------------------
// Headers and Defines for fonts
// Note that font files are located within the Adafruit-GFX library folder:
// ------------------------------------------------
//<Fonts !Start!>
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
enum {E_PG_MAIN,E_PG_BASE,E_PG_CONFIG,E_PG_ALERT};
enum {E_DRAW_LINE1,E_ELEM_ALERT_CANCEL,E_ELEM_ALERT_OK,E_ELEM_BOX1
      ,E_ELEM_BOX2,E_ELEM_BTN_ALERT,E_ELEM_BTN_QUIT,E_ELEM_CHECK1
      ,E_ELEM_CHECK2,E_ELEM_CHECK3,E_ELEM_PROGRESS1,E_ELEM_TAB_CONFIG
      ,E_ELEM_TAB_MAIN,E_ELEM_TEXT9,E_ELEM_TXT_COUNT,E_LBL_COUNT
      ,E_LBL_DATA1,E_LBL_DATA2,E_LBL_PROGRESS,E_LBL_TITLE};
// Must use separate enum for fonts with MAX_FONT at end to use gslc_FontSet.
enum {E_FONT_TXT5,MAX_FONT};
//<Enum !End!>

// ------------------------------------------------
// Instantiate the GUI
// ------------------------------------------------

// ------------------------------------------------
// Define the maximum number of elements and pages
// ------------------------------------------------
//<ElementDefines !Start!>
#define MAX_PAGE                4

#define MAX_ELEM_PG_MAIN 3                                          // # Elems total on page
#define MAX_ELEM_PG_MAIN_RAM MAX_ELEM_PG_MAIN // # Elems in RAM

#define MAX_ELEM_PG_BASE 8                                          // # Elems total on page
#define MAX_ELEM_PG_BASE_RAM MAX_ELEM_PG_BASE // # Elems in RAM

#define MAX_ELEM_PG_CONFIG 5                                          // # Elems total on page
#define MAX_ELEM_PG_CONFIG_RAM MAX_ELEM_PG_CONFIG // # Elems in RAM

#define MAX_ELEM_PG_ALERT 4                                          // # Elems total on page
#define MAX_ELEM_PG_ALERT_RAM MAX_ELEM_PG_ALERT // # Elems in RAM
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
gslc_tsElem                     m_asBasePage1Elem[MAX_ELEM_PG_BASE_RAM];
gslc_tsElemRef                  m_asBasePage1ElemRef[MAX_ELEM_PG_BASE];
gslc_tsElem                     m_asPage2Elem[MAX_ELEM_PG_CONFIG_RAM];
gslc_tsElemRef                  m_asPage2ElemRef[MAX_ELEM_PG_CONFIG];
gslc_tsElem                     m_asPopup1Elem[MAX_ELEM_PG_ALERT_RAM];
gslc_tsElemRef                  m_asPopup1ElemRef[MAX_ELEM_PG_ALERT];
gslc_tsXProgress                m_sXBarGauge1;
gslc_tsXCheckbox                m_asXCheck1;
gslc_tsXCheckbox                m_asXCheck2;
gslc_tsXCheckbox                m_asXCheck3;

#define MAX_STR                 100

//<GUI_Extra_Elements !End!>

// ------------------------------------------------
// Program Globals
// ------------------------------------------------
bool      m_bQuit = false;

// Free-running counter for display
unsigned  m_nCount = 0;

// Save some element references for direct access
//<Save_References !Start!>
gslc_tsElemRef*  m_pElemAlertMsg   = NULL;
gslc_tsElemRef*  m_pElemCnt        = NULL;
gslc_tsElemRef*  m_pElemProgress1  = NULL;
gslc_tsElemRef*  m_pElemQuit       = NULL;
gslc_tsElemRef*  m_pElemTabConfig  = NULL;
gslc_tsElemRef*  m_pElemTabMain    = NULL;
//<Save_References !End!>

// Define debug message function
static int16_t DebugOut(char ch) { if (ch == (char)'\n') Serial.println(""); else Serial.write(ch); return 0; }

// Update the tab dialog button highlights
void SetTabHighlight(int16_t nTabSel)
{
  gslc_ElemSetCol(&m_gui, m_pElemTabMain, (nTabSel == E_ELEM_TAB_MAIN) ? GSLC_COL_WHITE : GSLC_COL_BLUE_DK2,
    GSLC_COL_BLUE_DK4, GSLC_COL_BLUE_DK1);
  gslc_ElemSetCol(&m_gui, m_pElemTabConfig, (nTabSel == E_ELEM_TAB_CONFIG) ? GSLC_COL_WHITE : GSLC_COL_BLUE_DK2,
    GSLC_COL_BLUE_DK4, GSLC_COL_BLUE_DK1);
}

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
      case E_ELEM_BTN_QUIT:
        m_bQuit = true;
        gslc_ElemSetTxtStr(&m_gui,m_pElemQuit,"DONE");
        gslc_ElemSetCol(&m_gui,m_pElemQuit,GSLC_COL_RED,GSLC_COL_BLACK,GSLC_COL_BLACK);
        break;
      case E_ELEM_TAB_MAIN:
        gslc_SetPageCur(&m_gui,E_PG_MAIN);
        SetTabHighlight(E_ELEM_TAB_MAIN);
        break;
      case E_ELEM_TAB_CONFIG:
        gslc_SetPageCur(&m_gui,E_PG_CONFIG);
        SetTabHighlight(E_ELEM_TAB_CONFIG);
        break;
      case E_ELEM_BTN_ALERT:
        gslc_ElemSetTxtStr(&m_gui, m_pElemAlertMsg, "Alert Message!");
        gslc_PopupShow(&m_gui, E_PG_ALERT, true);
        break;
      case E_ELEM_ALERT_OK:
        GSLC_DEBUG_PRINT("INFO: Alert popup selected OK\n", "");
        // Dispose of alert
        gslc_PopupHide(&m_gui);
        break;
      case E_ELEM_ALERT_CANCEL:
        GSLC_DEBUG_PRINT("INFO: Alert popup selected Cancel\n", "");
        // Dispose of alert
        gslc_PopupHide(&m_gui);
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
  gslc_PageAdd(&m_gui,E_PG_BASE,m_asBasePage1Elem,MAX_ELEM_PG_BASE_RAM,m_asBasePage1ElemRef,MAX_ELEM_PG_BASE);
  gslc_PageAdd(&m_gui,E_PG_MAIN,m_asPage1Elem,MAX_ELEM_PG_MAIN_RAM,m_asPage1ElemRef,MAX_ELEM_PG_MAIN);
  gslc_PageAdd(&m_gui,E_PG_CONFIG,m_asPage2Elem,MAX_ELEM_PG_CONFIG_RAM,m_asPage2ElemRef,MAX_ELEM_PG_CONFIG);
  gslc_PageAdd(&m_gui,E_PG_ALERT,m_asPopup1Elem,MAX_ELEM_PG_ALERT_RAM,m_asPopup1ElemRef,MAX_ELEM_PG_ALERT);

  // Now mark $<COM-000> as a "base" page which means that it's elements
  // are always visible. This is useful for common page elements.
  gslc_SetPageBase(&m_gui, E_PG_BASE);


  // NOTE: The current page defaults to the first page added. Here we explicitly
  //       ensure that the main page is the correct page no matter the add order.
  gslc_SetPageCur(&m_gui,E_PG_MAIN);
  
  // Set Background to a flat color
  gslc_SetBkgndColor(&m_gui,GSLC_COL_BLACK);

  // -----------------------------------
  // PAGE: E_PG_MAIN
  
  
  // Create E_LBL_PROGRESS text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_LBL_PROGRESS,E_PG_MAIN,(gslc_tsRect){40,120,56,12},
    (char*)"Progress:",0,E_FONT_TXT5);

  // Create progress bar E_ELEM_PROGRESS1 
  pElemRef = gslc_ElemXProgressCreate(&m_gui,E_ELEM_PROGRESS1,E_PG_MAIN,&m_sXBarGauge1,
    (gslc_tsRect){100,120,50,12},0,100,0,GSLC_COL_GREEN,false);
  m_pElemProgress1 = pElemRef;
   
  // create checkbox E_ELEM_CHECK1
  pElemRef = gslc_ElemXCheckboxCreate(&m_gui,E_ELEM_CHECK1,E_PG_MAIN,&m_asXCheck1,
    (gslc_tsRect){100,160,30,30},false,GSLCX_CHECKBOX_STYLE_X,GSLC_COL_BLUE_LT2,false);

  // -----------------------------------
  // PAGE: E_PG_BASE
  
  
  // Create E_LBL_TITLE text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_LBL_TITLE,E_PG_BASE,(gslc_tsRect){10,10,80,12},
    (char*)"GUIslice Demo",0,E_FONT_TXT5);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_WHITE);
  
  // Create E_ELEM_BTN_QUIT button with modifiable text label
  static char m_strbtn1[7] = "Quit";
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_QUIT,E_PG_BASE,
    (gslc_tsRect){240,5,50,25},
    (char*)m_strbtn1,7,E_FONT_TXT5,&CbBtnCommon);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_WHITE);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_RED_DK2,GSLC_COL_RED_DK4,GSLC_COL_RED_DK1);
  m_pElemQuit = pElemRef;
  
  // Create E_LBL_COUNT text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_LBL_COUNT,E_PG_BASE,(gslc_tsRect){110,10,38,12},
    (char*)"Count:",0,E_FONT_TXT5);
  
  // Create E_ELEM_TXT_COUNT runtime modifiable text
  static char m_sDisplayText5[8] = "";
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TXT_COUNT,E_PG_BASE,(gslc_tsRect){170,10,44,12},
    (char*)m_sDisplayText5,8,E_FONT_TXT5);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_PURPLE);
  m_pElemCnt = pElemRef;
  
  // create E_ELEM_TAB_MAIN button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_TAB_MAIN,E_PG_BASE,
    (gslc_tsRect){30,50,50,20},(char*)"Main",0,E_FONT_TXT5,&CbBtnCommon);
  m_pElemTabMain = pElemRef;
  
  // create E_ELEM_TAB_CONFIG button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_TAB_CONFIG,E_PG_BASE,
    (gslc_tsRect){90,50,50,20},(char*)"Extra",0,E_FONT_TXT5,&CbBtnCommon);
  m_pElemTabConfig = pElemRef;
   
  // Create E_ELEM_BOX1 box
  pElemRef = gslc_ElemCreateBox(&m_gui,E_ELEM_BOX1,E_PG_BASE,(gslc_tsRect){20,70,200,150});
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_WHITE,GSLC_COL_BLACK,GSLC_COL_BLACK);

  // Create E_DRAW_LINE1 line 
  pElemRef = gslc_ElemCreateLine(&m_gui,E_DRAW_LINE1,E_PG_BASE,0,35,320,35);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_BLACK,GSLC_COL_GRAY_LT2,GSLC_COL_GRAY_LT2);

  // -----------------------------------
  // PAGE: E_PG_CONFIG
  
  
  // create E_ELEM_BTN_ALERT button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_ALERT,E_PG_CONFIG,
    (gslc_tsRect){60,170,50,20},(char*)"Alert",0,E_FONT_TXT5,&CbBtnCommon);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_WHITE);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_GREEN_DK2,GSLC_COL_GREEN_DK4,GSLC_COL_GREEN_DK1);
   
  // create checkbox E_ELEM_CHECK2
  pElemRef = gslc_ElemXCheckboxCreate(&m_gui,E_ELEM_CHECK2,E_PG_CONFIG,&m_asXCheck2,
    (gslc_tsRect){60,80,20,20},false,GSLCX_CHECKBOX_STYLE_X,GSLC_COL_RED_LT2,false);
  
  // Create E_LBL_DATA1 text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_LBL_DATA1,E_PG_CONFIG,(gslc_tsRect){100,80,38,12},
    (char*)"Data 1",0,E_FONT_TXT5);
   
  // create checkbox E_ELEM_CHECK3
  pElemRef = gslc_ElemXCheckboxCreate(&m_gui,E_ELEM_CHECK3,E_PG_CONFIG,&m_asXCheck3,
    (gslc_tsRect){60,110,20,20},false,GSLCX_CHECKBOX_STYLE_X,GSLC_COL_RED_LT2,false);
  
  // Create E_LBL_DATA2 text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_LBL_DATA2,E_PG_CONFIG,(gslc_tsRect){100,110,38,12},
    (char*)"Data 2",0,E_FONT_TXT5);

  // -----------------------------------
  // PAGE: E_PG_ALERT
  
   
  // Create E_ELEM_BOX2 box
  pElemRef = gslc_ElemCreateBox(&m_gui,E_ELEM_BOX2,E_PG_ALERT,(gslc_tsRect){70,90,180,90});
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_BLUE_LT2,GSLC_COL_BLACK,GSLC_COL_GRAY_DK3);
  
  // Create E_ELEM_TEXT9 runtime modifiable text
  static char m_sDisplayText9[20] = "Alert";
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TEXT9,E_PG_ALERT,(gslc_tsRect){85,95,146,12},
    (char*)m_sDisplayText9,20,E_FONT_TXT5);
  gslc_ElemSetTxtAlign(&m_gui,pElemRef,GSLC_ALIGN_MID_MID);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_RED_LT1);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_GRAY,GSLC_COL_GRAY_DK2,GSLC_COL_BLACK);
  m_pElemAlertMsg = pElemRef;
  
  // create E_ELEM_ALERT_OK button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_ALERT_OK,E_PG_ALERT,
    (gslc_tsRect){90,150,60,20},(char*)"OK",0,E_FONT_TXT5,&CbBtnCommon);
  
  // create E_ELEM_ALERT_CANCEL button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_ALERT_CANCEL,E_PG_ALERT,
    (gslc_tsRect){170,150,60,20},(char*)"CANCEL",0,E_FONT_TXT5,&CbBtnCommon);
//<InitGUI !End!>

  // Highlight the Main Page Tab
  SetTabHighlight(E_ELEM_TAB_MAIN);

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
  char              acTxt[MAX_STR];

  // ------------------------------------------------
  // Update GUI Elements
  // ------------------------------------------------
  m_nCount++;

  // Perform drawing updates
  // - Note: we can make the updates conditional on the active
  //   page by checking gslc_GetPageCur() first.

  if ((m_nCount % 50) == 0) {
    snprintf(acTxt, MAX_STR, "%u", m_nCount);
    gslc_ElemSetTxtStr(&m_gui, m_pElemCnt, acTxt);
  }

  gslc_ElemXProgressSetVal(&m_gui, m_pElemProgress1, ((m_nCount / 2) % 100));

  // We can change or disable the global page as needed:
  //   gslc_SetPageGlobal(&m_gui, E_PG_BASE);    // Set to E_PG_BASE
  //   gslc_SetPageGlobal(&m_gui, GSLC_PAGE_NONE); // Disable

  
  // ------------------------------------------------
  // Periodically call GUIslice update function
  // ------------------------------------------------
  gslc_Update(&m_gui);
    
  // Slow down updates
  delay(10);

  // In a real program, we would detect the button press and take an action.
  // For this Arduino demo, we will pretend to exit by emulating it with an
  // infinite loop. Note that interrupts are not disabled so that any debug
  // messages via Serial have an opportunity to be transmitted.
  if (m_bQuit) {
    gslc_Quit(&m_gui);
    while (1) {}
  }
}
