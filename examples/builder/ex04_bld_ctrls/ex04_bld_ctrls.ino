//<File !Start!>
// FILE: [ex04_bld_ctrls.ino]
// Created by GUIslice Builder version: [0.13.0]
//
// GUIslice Builder Generated File
//
// For the latest guides, updates and support view:
// https://github.com/ImpulseAdventure/GUIslice
//
//<File !End!>
//
// - Example 04 (Arduino): Dynamic content
//   - Demonstrates push buttons, checkboxes and slider controls
//   - Shows callback notifications for checkboxes and radio buttons
//   - Provide example of additional Adafruit-GFX fonts.
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
#include "elem/XSlider.h"
//<Includes !End!>

// ------------------------------------------------
// Headers and Defines for fonts
// Note that font files are located within the Adafruit-GFX library folder:
// ------------------------------------------------
//<Fonts !Start!>
#include <Adafruit_GFX.h>
// Note that these files are located within the Adafruit-GFX library folder:
#include "Fonts/FreeSans12pt7b.h"
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
enum {E_PAGE_MAIN};
enum {E_ELEM_BOX1,E_ELEM_BTN_QUIT,E_ELEM_CHECK1,E_ELEM_PROGRESS
      ,E_ELEM_PROGRESS2,E_ELEM_RADIO1,E_ELEM_RADIO2,E_ELEM_SLIDER1
      ,E_ELEM_TXT_COUNT,E_ELEM_TXT_SLIDER,E_LBL_CHECK1,E_LBL_COUNT
      ,E_LBL_PROGRESS,E_LBL_RADIO1,E_LBL_RADIO2,E_LBL_SLIDER};
enum {E_GROUP1};
// Must use separate enum for fonts with MAX_FONT at end to use gslc_FontSet.
enum {E_FONT_SANS12,E_FONT_TXT5,MAX_FONT};
//<Enum !End!>

// ------------------------------------------------
// Instantiate the GUI
// ------------------------------------------------

// ------------------------------------------------
// Define the maximum number of elements and pages
// ------------------------------------------------
//<ElementDefines !Start!>
#define MAX_PAGE                1

#define MAX_ELEM_PAGE_MAIN 16                                         // # Elems total on page
#define MAX_ELEM_PAGE_MAIN_RAM MAX_ELEM_PAGE_MAIN // # Elems in RAM
//<ElementDefines !End!>

// ------------------------------------------------
// Create element storage
// ------------------------------------------------
gslc_tsGui                      m_gui;
gslc_tsDriver                   m_drv;
gslc_tsFont                     m_asFont[MAX_FONT];
gslc_tsPage                     m_asPage[MAX_PAGE];

//<GUI_Extra_Elements !Start!>
gslc_tsElem                     m_asPage1Elem[MAX_ELEM_PAGE_MAIN_RAM];
gslc_tsElemRef                  m_asPage1ElemRef[MAX_ELEM_PAGE_MAIN];
gslc_tsXProgress                m_sXBarGauge1;
gslc_tsXProgress                m_sXBarGauge2;
gslc_tsXCheckbox                m_asXCheck1;
gslc_tsXCheckbox                m_asXRadio1;
gslc_tsXCheckbox                m_asXRadio2;
gslc_tsXSlider                  m_sXSlider1;

#define MAX_STR                 100

//<GUI_Extra_Elements !End!>

// ------------------------------------------------
// Program Globals
// ------------------------------------------------
bool        m_bQuit = false;

// Free-running counter for display
unsigned    m_nCount = 0;


// Save some element references for direct access
//<Save_References !Start!>
gslc_tsElemRef*  m_pElemCnt        = NULL;
gslc_tsElemRef*  m_pElemProgress   = NULL;
gslc_tsElemRef*  m_pElemProgress2  = NULL;
gslc_tsElemRef*  m_pElemQuit       = NULL;
gslc_tsElemRef*  m_pElemSlider1    = NULL;
gslc_tsElemRef*  m_pElemSliderTxt  = NULL;
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

//<Button Enums !End!>
        default:
        break;
    }
  }
  return true;
}
// Checkbox / radio callbacks
// - Creating a callback function is optional, but doing so enables you to
//   detect changes in the state of the elements.
bool CbCheckbox(void* pvGui, void* pvElemRef, int16_t nSelId, bool bState)
{
  gslc_tsGui*     pGui      = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef  = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem*    pElem     = gslc_GetElemFromRef(pGui,pElemRef);
  if (pElemRef == NULL) {
    return false;
  }
  
  boolean bChecked = gslc_ElemXCheckboxGetState(pGui,pElemRef);

  // Determine which element issued the callback
  switch (pElem->nId) {
//<Checkbox Enums !Start!>
    case E_ELEM_CHECK1:
      GSLC_DEBUG_PRINT("Callback: Check[ID=%d] state=%u\n", pElem->nId,bChecked);
      break;
    case E_ELEM_RADIO1:
    case E_ELEM_RADIO2:
      // For the radio buttons, determine which ID is currently selected (nSelId)
      // - Note that this may not always be the same as the element that
      //   issued the callback (pElem->nId)
      // - A return value of GSLC_ID_NONE indicates that no radio buttons
      //   in the group are currently selected
      if (nSelId == GSLC_ID_NONE) {
        GSLC_DEBUG_PRINT("Callback: Radio[ID=NONE] selected\n", "");
      } else {
        GSLC_DEBUG_PRINT("Callback: Radio[ID=%d] selected\n", nSelId);
      }
      break;

//<Checkbox Enums !End!>
    default:
      break;
  } // switch
  return true;
}
//<Keypad Callback !Start!>
//<Keypad Callback !End!>
//<Spinner Callback !Start!>
//<Spinner Callback !End!>
//<Listbox Callback !Start!>
//<Listbox Callback !End!>
//<Draw Callback !Start!>
//<Draw Callback !End!>

// Callback function for when a slider's position has been updated
bool CbSlidePos(void* pvGui,void* pvElemRef,int16_t nPos)
{
  gslc_tsGui*     pGui      = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef  = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem*    pElem     = pElemRef->pElem;
  int16_t         nVal;

  char                acTxt[20];

  // From the element's ID we can determine which slider was updated.
  switch (pElem->nId) {
//<Slider Enums !Start!>
    case E_ELEM_SLIDER1:
      // Fetch the slider position
      nVal = gslc_ElemXSliderGetPos(pGui,m_pElemSlider1);
      snprintf(acTxt,MAX_STR,"%u",nVal);
      gslc_ElemSetTxtStr(&m_gui,m_pElemSliderTxt,acTxt);
      gslc_ElemXProgressSetVal(&m_gui,m_pElemProgress2,(nVal*80.0/100.0)-15);
      break;

//<Slider Enums !End!>
    default:
      break;
  }

  return true;
}
//<Tick Callback !Start!>
//<Tick Callback !End!>

// ------------------------------------------------
// Create page elements
// ------------------------------------------------
bool InitGUI()
{
  gslc_tsElemRef* pElemRef = NULL;

//<InitGUI !Start!>
  gslc_PageAdd(&m_gui,E_PAGE_MAIN,m_asPage1Elem,MAX_ELEM_PAGE_MAIN_RAM,m_asPage1ElemRef,MAX_ELEM_PAGE_MAIN);

  // NOTE: The current page defaults to the first page added. Here we explicitly
  //       ensure that the main page is the correct page no matter the add order.
  gslc_SetPageCur(&m_gui,E_PAGE_MAIN);
  
  // Set Background to a flat color
  gslc_SetBkgndColor(&m_gui,GSLC_COL_GRAY_DK2);

  // -----------------------------------
  // PAGE: E_PAGE_MAIN
  
   
  // Create E_ELEM_BOX1 box
  pElemRef = gslc_ElemCreateBox(&m_gui,E_ELEM_BOX1,E_PAGE_MAIN,(gslc_tsRect){10,50,300,150});
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_WHITE,GSLC_COL_BLACK,GSLC_COL_BLACK);
  
  // Create E_ELEM_BTN_QUIT button with modifiable text label
  static char m_strbtn1[7] = "Quit";
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_QUIT,E_PAGE_MAIN,
    (gslc_tsRect){160,80,80,40},
    (char*)m_strbtn1,7,E_FONT_SANS12,&CbBtnCommon);
  m_pElemQuit = pElemRef;
  
  // Create E_LBL_COUNT text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_LBL_COUNT,E_PAGE_MAIN,(gslc_tsRect){20,60,38,12},
    (char*)"Count:",0,E_FONT_TXT5);
  
  // Create E_ELEM_TXT_COUNT runtime modifiable text
  static char m_sDisplayText2[8] = "";
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TXT_COUNT,E_PAGE_MAIN,(gslc_tsRect){80,60,44,12},
    (char*)m_sDisplayText2,8,E_FONT_TXT5);
  m_pElemCnt = pElemRef;
  
  // Create E_LBL_PROGRESS text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_LBL_PROGRESS,E_PAGE_MAIN,(gslc_tsRect){20,80,50,12},
    (char*)"Progress:",0,E_FONT_TXT5);

  // Create progress bar E_ELEM_PROGRESS 
  pElemRef = gslc_ElemXProgressCreate(&m_gui,E_ELEM_PROGRESS,E_PAGE_MAIN,&m_sXBarGauge1,
    (gslc_tsRect){80,80,50,12},0,100,0,GSLC_COL_GREEN,false);
  m_pElemProgress = pElemRef;

  // Create progress bar E_ELEM_PROGRESS2 
  pElemRef = gslc_ElemXProgressCreate(&m_gui,E_ELEM_PROGRESS2,E_PAGE_MAIN,&m_sXBarGauge2,
    (gslc_tsRect){280,80,10,100},-25,75,-15,GSLC_COL_RED,true);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_BLUE_DK3,GSLC_COL_BLACK,GSLC_COL_BLACK);
  m_pElemProgress2 = pElemRef;
  
  // Create E_LBL_CHECK1 text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_LBL_CHECK1,E_PAGE_MAIN,(gslc_tsRect){20,100,44,12},
    (char*)"Check1:",0,E_FONT_TXT5);
   
  // create checkbox E_ELEM_CHECK1
  pElemRef = gslc_ElemXCheckboxCreate(&m_gui,E_ELEM_CHECK1,E_PAGE_MAIN,&m_asXCheck1,
    (gslc_tsRect){80,100,20,20},false,GSLCX_CHECKBOX_STYLE_X,GSLC_COL_BLUE_LT2,false);
  gslc_ElemXCheckboxSetStateFunc(&m_gui, pElemRef, &CbCheckbox);
  
  // Create E_LBL_RADIO1 text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_LBL_RADIO1,E_PAGE_MAIN,(gslc_tsRect){20,135,44,12},
    (char*)"Radio1:",0,E_FONT_TXT5);
  
  // Create radio button E_ELEM_RADIO1
  pElemRef = gslc_ElemXCheckboxCreate(&m_gui,E_ELEM_RADIO1,E_PAGE_MAIN,&m_asXRadio1,
    (gslc_tsRect){80,135,20,20},true,GSLCX_CHECKBOX_STYLE_ROUND,(gslc_tsColor){255,200,0},false);
  gslc_ElemSetGroup(&m_gui,pElemRef,E_GROUP1);
  gslc_ElemXCheckboxSetStateFunc(&m_gui, pElemRef, &CbCheckbox);
  
  // Create E_LBL_RADIO2 text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_LBL_RADIO2,E_PAGE_MAIN,(gslc_tsRect){20,160,44,12},
    (char*)"Radio2:",0,E_FONT_TXT5);
  
  // Create radio button E_ELEM_RADIO2
  pElemRef = gslc_ElemXCheckboxCreate(&m_gui,E_ELEM_RADIO2,E_PAGE_MAIN,&m_asXRadio2,
    (gslc_tsRect){80,160,20,20},true,GSLCX_CHECKBOX_STYLE_ROUND,(gslc_tsColor){255,200,0},false);
  gslc_ElemSetGroup(&m_gui,pElemRef,E_GROUP1);
  gslc_ElemXCheckboxSetStateFunc(&m_gui, pElemRef, &CbCheckbox);

  // Create slider E_ELEM_SLIDER1 
  pElemRef = gslc_ElemXSliderCreate(&m_gui,E_ELEM_SLIDER1,E_PAGE_MAIN,&m_sXSlider1,
          (gslc_tsRect){160,140,80,20},0,100,60,5,false);
  gslc_ElemXSliderSetStyle(&m_gui,pElemRef,true,GSLC_COL_BLUE_DK4,10,5,GSLC_COL_GRAY_DK2);
  gslc_ElemXSliderSetPosFunc(&m_gui,pElemRef,&CbSlidePos);
  m_pElemSlider1 = pElemRef;
  
  // Create E_LBL_SLIDER text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_LBL_SLIDER,E_PAGE_MAIN,(gslc_tsRect){160,160,44,12},
    (char*)"Slider:",0,E_FONT_TXT5);
  
  // Create E_ELEM_TXT_SLIDER runtime modifiable text
  static char m_sDisplayText11[6] = "";
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TXT_SLIDER,E_PAGE_MAIN,(gslc_tsRect){220,160,32,12},
    (char*)m_sDisplayText11,6,E_FONT_TXT5);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_ORANGE);
  m_pElemSliderTxt = pElemRef;
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
    if (!gslc_FontSet(&m_gui,E_FONT_SANS12,GSLC_FONTREF_PTR,&FreeSans12pt7b,1)) { return; }
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
  char                acTxt[MAX_STR];

  // General counter
  m_nCount++;

  // Update elements on active page

  snprintf(acTxt,MAX_STR,"%u",m_nCount/5);
  gslc_ElemSetTxtStr(&m_gui,m_pElemCnt,acTxt);

  gslc_ElemXProgressSetVal(&m_gui,m_pElemProgress,((m_nCount/1)%100));

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
    while (1) { }
  }
    
}
