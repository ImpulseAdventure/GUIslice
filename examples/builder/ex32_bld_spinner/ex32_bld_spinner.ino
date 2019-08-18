//<File !Start!>
// FILE: [ex32_bld_spinner.ino]
// Created by GUIslice Builder version: [0.13.0]
//
// GUIslice Builder Generated File
//
// For the latest guides, updates and support view:
// https://github.com/ImpulseAdventure/GUIslice
//
//<File !End!>
//
// - Example 32 (Arduino):
//   - Multiple page handling
//   - Background image
//   - XSpinner compound elements
//
//   - NOTE: The XSpinner compound element requires GSLC_FEATURE_COMPOUND enabled
//
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
#include "elem/XProgress.h"
#include "elem/XSpinner.h"

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
enum {E_PG_MAIN,E_PG_EXTRA};
enum {E_ELEM_BOX1,E_ELEM_BOX2,E_ELEM_BTN_BACK,E_ELEM_BTN_EXTRA
      ,E_ELEM_BTN_QUIT,E_ELEM_COMP1,E_ELEM_COMP2,E_ELEM_PROGRESS1
      ,E_ELEM_SPINNER3,E_LBL_COUNT,E_LBL_DATA1,E_LBL_DATA2,E_LBL_DATA3
      ,E_LBL_PROGRESS,E_LBL_TITLE,E_TXT_COUNT,E_TXT_DATA1,E_TXT_DATA2
      ,E_TXT_DATA3};
// Must use separate enum for fonts with MAX_FONT at end to use gslc_FontSet.
enum {E_FONT_TXT10,E_FONT_TXT5,MAX_FONT};
//<Enum !End!>

// ------------------------------------------------
// Instantiate the GUI
// ------------------------------------------------

// ------------------------------------------------
// Define the maximum number of elements and pages
// ------------------------------------------------
//<ElementDefines !Start!>
#define MAX_PAGE                2

#define MAX_ELEM_PG_MAIN 9                                          // # Elems total on page
#define MAX_ELEM_PG_MAIN_RAM MAX_ELEM_PG_MAIN // # Elems in RAM

#define MAX_ELEM_PG_EXTRA 10                                         // # Elems total on page
#define MAX_ELEM_PG_EXTRA_RAM MAX_ELEM_PG_EXTRA // # Elems in RAM
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
gslc_tsElem                     m_asPage2Elem[MAX_ELEM_PG_EXTRA_RAM];
gslc_tsElemRef                  m_asPage2ElemRef[MAX_ELEM_PG_EXTRA];
gslc_tsXProgress                m_sXBarGauge1;
gslc_tsXSpinner                 m_sXSpinner1;
gslc_tsXSpinner                 m_sXSpinner2;
gslc_tsXSpinner                 m_sXSpinner3;

#define MAX_STR                 100

//<GUI_Extra_Elements !End!>

// ------------------------------------------------
// Program Globals
// ------------------------------------------------
bool      m_bQuit = false;

// Free-running counter for display
unsigned  m_nCount = 0;
int16_t   m_nComp1 = 0;
int16_t   m_nComp2 = 0;
int16_t   m_nComp3 = 0;


// Save some element references for direct access
//<Save_References !Start!>
gslc_tsElemRef*  m_pElemCnt        = NULL;
gslc_tsElemRef*  m_pElemComp1      = NULL;
gslc_tsElemRef*  m_pElemComp2      = NULL;
gslc_tsElemRef*  m_pElemComp3      = NULL;
gslc_tsElemRef*  m_pElemProgress1  = NULL;
gslc_tsElemRef*  m_pElemQuit       = NULL;
gslc_tsElemRef*  m_pElemSpinner1   = NULL;
gslc_tsElemRef*  m_pElemSpinner2   = NULL;
gslc_tsElemRef*  m_pElemSpinner3   = NULL;
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
      case E_ELEM_BTN_QUIT:
        m_bQuit = true;
        gslc_ElemSetTxtStr(&m_gui,m_pElemQuit,"DONE");
        gslc_ElemSetCol(&m_gui,m_pElemQuit,GSLC_COL_RED,GSLC_COL_BLACK,GSLC_COL_BLACK);
        break;
      case E_ELEM_BTN_EXTRA:
        gslc_SetPageCur(&m_gui,E_PG_EXTRA);
        break;
      case E_ELEM_BTN_BACK:
        gslc_SetPageCur(&m_gui,E_PG_MAIN);
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
// Spinner Input Ready callback
bool CbSpinner(void* pvGui, void *pvElemRef, int16_t nState, void* pvData)
{
  gslc_tsElemRef* pElemRef = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem* pElem = pElemRef->pElem;
  gslc_tsGui* pGui = (gslc_tsGui*)pvGui;

  char acTxtNum[4];

  // NOTE: pvData is NULL
  if (nState == XSPINNER_CB_STATE_UPDATE) {
    // From the element's ID we can determine which input field is ready.
    switch (pElem->nId) {
//<Spinner Enums !Start!>
      case E_ELEM_COMP1:
        //TODO- Add Spinner handling code
        // using gslc_ElemXSpinnerGetCounter(&m_gui, &m_sXSpinner1);
        m_nComp1 = gslc_ElemXSpinnerGetCounter(&m_gui, &m_sXSpinner1);
        snprintf(acTxtNum, 4, "%d", m_nComp1);
        gslc_ElemSetTxtStr(&m_gui,m_pElemComp1,acTxtNum);
        break;
      case E_ELEM_COMP2:
        //TODO- Add Spinner handling code
        // using gslc_ElemXSpinnerGetCounter(&m_gui, &m_sXSpinner2);
        m_nComp2 = gslc_ElemXSpinnerGetCounter(&m_gui, &m_sXSpinner2);
        snprintf(acTxtNum, 4, "%d", m_nComp2);
        gslc_ElemSetTxtStr(&m_gui,m_pElemComp2,acTxtNum);
        break;
      case E_ELEM_SPINNER3:
        //TODO- Add Spinner handling code
        // using gslc_ElemXSpinnerGetCounter(&m_gui, &m_sXSpinner3);
        m_nComp3 = gslc_ElemXSpinnerGetCounter(&m_gui, &m_sXSpinner3);
        snprintf(acTxtNum, 4, "%d", m_nComp3);
        gslc_ElemSetTxtStr(&m_gui,m_pElemComp3,acTxtNum);
        break;

//<Spinner Enums !End!>
      default:
        break;
    }
  }
}
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
  gslc_PageAdd(&m_gui,E_PG_EXTRA,m_asPage2Elem,MAX_ELEM_PG_EXTRA_RAM,m_asPage2ElemRef,MAX_ELEM_PG_EXTRA);

  // NOTE: The current page defaults to the first page added. Here we explicitly
  //       ensure that the main page is the correct page no matter the add order.
  gslc_SetPageCur(&m_gui,E_PG_MAIN);
  
  // Set Background to a flat color
  gslc_SetBkgndColor(&m_gui,GSLC_COL_GRAY_DK2);

  // -----------------------------------
  // PAGE: E_PG_MAIN
  
   
  // Create E_ELEM_BOX1 box
  pElemRef = gslc_ElemCreateBox(&m_gui,E_ELEM_BOX1,E_PG_MAIN,(gslc_tsRect){20,50,280,150});
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_WHITE,GSLC_COL_BLACK,GSLC_COL_BLACK);
  
  // Create E_LBL_TITLE text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_LBL_TITLE,E_PG_MAIN,(gslc_tsRect){88,10,144,18},
    (char*)"Demo Spinner",0,E_FONT_TXT10);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_WHITE);
  
  // Create E_ELEM_BTN_QUIT button with modifiable text label
  static char m_strbtn1[7] = "Quit";
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_QUIT,E_PG_MAIN,
    (gslc_tsRect){100,140,50,20},
    (char*)m_strbtn1,7,E_FONT_TXT5,&CbBtnCommon);
  m_pElemQuit = pElemRef;
  
  // create E_ELEM_BTN_EXTRA button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_EXTRA,E_PG_MAIN,
    (gslc_tsRect){170,140,50,20},(char*)"Extra",0,E_FONT_TXT5,&CbBtnCommon);
  
  // Create E_LBL_COUNT text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_LBL_COUNT,E_PG_MAIN,(gslc_tsRect){40,60,36,10},
    (char*)"Count:",0,E_FONT_TXT5);
  
  // Create E_TXT_COUNT runtime modifiable text
  static char m_sDisplayText3[8] = "";
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_TXT_COUNT,E_PG_MAIN,(gslc_tsRect){100,60,42,10},
    (char*)m_sDisplayText3,8,E_FONT_TXT5);
  m_pElemCnt = pElemRef;
  
  // Create E_LBL_PROGRESS text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_LBL_PROGRESS,E_PG_MAIN,(gslc_tsRect){40,80,54,10},
    (char*)"Progress:",0,E_FONT_TXT5);

  // Create progress bar E_ELEM_PROGRESS1 
  pElemRef = gslc_ElemXProgressCreate(&m_gui,E_ELEM_PROGRESS1,E_PG_MAIN,&m_sXBarGauge1,
    (gslc_tsRect){100,80,50,12},0,100,0,GSLC_COL_GREEN,false);
  m_pElemProgress1 = pElemRef;
  // Add Spinner element
  pElemRef = gslc_ElemXSpinnerCreate(&m_gui,E_ELEM_COMP1,E_PG_MAIN,&m_sXSpinner1,
    (gslc_tsRect){160,60,68,20},0,99,0,1,E_FONT_TXT5,20,&CbSpinner);

  m_pElemSpinner1 = pElemRef;

  // -----------------------------------
  // PAGE: E_PG_EXTRA
  
   
  // Create E_ELEM_BOX2 box
  pElemRef = gslc_ElemCreateBox(&m_gui,E_ELEM_BOX2,E_PG_EXTRA,(gslc_tsRect){40,40,240,160});
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_WHITE,GSLC_COL_BLACK,GSLC_COL_BLACK);
  
  // create E_ELEM_BTN_BACK button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_BACK,E_PG_EXTRA,
    (gslc_tsRect){50,170,50,20},(char*)"Back",0,E_FONT_TXT5,&CbBtnCommon);
  
  // Create E_LBL_DATA1 text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_LBL_DATA1,E_PG_EXTRA,(gslc_tsRect){60,60,42,10},
    (char*)"Data 1:",0,E_FONT_TXT5);
  
  // Create E_TXT_DATA1 runtime modifiable text
  static char m_sDisplayText6[4] = "0";
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_TXT_DATA1,E_PG_EXTRA,(gslc_tsRect){120,60,24,10},
    (char*)m_sDisplayText6,4,E_FONT_TXT5);
  gslc_ElemSetTxtAlign(&m_gui,pElemRef,GSLC_ALIGN_MID_RIGHT);
  m_pElemComp1 = pElemRef;
  
  // Create E_LBL_DATA2 text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_LBL_DATA2,E_PG_EXTRA,(gslc_tsRect){60,100,42,10},
    (char*)"Data 2:",0,E_FONT_TXT5);
  
  // Create E_TXT_DATA2 runtime modifiable text
  static char m_sDisplayText8[4] = "0";
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_TXT_DATA2,E_PG_EXTRA,(gslc_tsRect){120,100,24,10},
    (char*)m_sDisplayText8,4,E_FONT_TXT5);
  gslc_ElemSetTxtAlign(&m_gui,pElemRef,GSLC_ALIGN_MID_RIGHT);
  m_pElemComp2 = pElemRef;
  // Add Spinner element
  pElemRef = gslc_ElemXSpinnerCreate(&m_gui,E_ELEM_COMP2,E_PG_EXTRA,&m_sXSpinner2,
    (gslc_tsRect){200,100,68,20},0,99,0,1,E_FONT_TXT5,20,&CbSpinner);

  m_pElemSpinner2 = pElemRef;
  
  // Create E_LBL_DATA3 text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_LBL_DATA3,E_PG_EXTRA,(gslc_tsRect){60,140,42,10},
    (char*)"Data 3:",0,E_FONT_TXT5);
  
  // Create E_TXT_DATA3 runtime modifiable text
  static char m_sDisplayText10[4] = "0";
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_TXT_DATA3,E_PG_EXTRA,(gslc_tsRect){120,140,24,10},
    (char*)m_sDisplayText10,4,E_FONT_TXT5);
  gslc_ElemSetTxtAlign(&m_gui,pElemRef,GSLC_ALIGN_MID_RIGHT);
  m_pElemComp3 = pElemRef;
  // Add Spinner element
  pElemRef = gslc_ElemXSpinnerCreate(&m_gui,E_ELEM_SPINNER3,E_PG_EXTRA,&m_sXSpinner3,
    (gslc_tsRect){200,140,68,20},0,99,0,1,E_FONT_TXT5,20,&CbSpinner);

  m_pElemSpinner3 = pElemRef;
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
    if (!gslc_FontSet(&m_gui,E_FONT_TXT10,GSLC_FONTREF_PTR,NULL,2)) { return; }
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

  snprintf(acTxt,MAX_STR,"%u",m_nCount);
  gslc_ElemSetTxtStr(&m_gui,m_pElemCnt,acTxt);

  gslc_ElemXProgressSetVal(&m_gui,m_pElemProgress1,((m_nCount/2)%100));

  
  // ------------------------------------------------
  // Periodically call GUIslice update function
  // ------------------------------------------------
  gslc_Update(&m_gui);
    
  // Slow down updates
  delay(100);

  // In a real program, we would detect the button press and take an action.
  // For this Arduino demo, we will pretend to exit by emulating it with an
  // infinite loop. Note that interrupts are not disabled so that any debug
  // messages via Serial have an opportunity to be transmitted.
  if (m_bQuit) {
    gslc_Quit(&m_gui);
    while (1) { }
  }
}
