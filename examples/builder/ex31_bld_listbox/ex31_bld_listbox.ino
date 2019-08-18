//<File !Start!>
// FILE: [ex31_bld_listbox.ino]
// Created by GUIslice Builder version: [0.13.0]
//
// GUIslice Builder Generated File
//
// For the latest guides, updates and support view:
// https://github.com/ImpulseAdventure/GUIslice
//
//<File !End!>
//
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

// ------------------------------------------------
// Headers to include
// ------------------------------------------------
#include "GUIslice.h"
#include "GUIslice_drv.h"

// Include any extended elements
//<Includes !Start!>
// Include extended elements
#include "elem/XListbox.h"
#include "elem/XSlider.h"
//<Includes !End!>

// ------------------------------------------------
// Headers and Defines for fonts
// Note that font files are located within the Adafruit-GFX library folder:
// ------------------------------------------------
//<Fonts !Start!>
#include <Adafruit_GFX.h>
// Note that these files are located within the Adafruit-GFX library folder:
#include "Fonts/FreeMono9pt7b.h"
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
enum {E_PG_MAIN};
enum {E_BTN_OK,E_LBL_COUNTRYCD,E_LBL_TITLE,E_LISTBOX,E_LISTSCROLL1
      ,E_TXT_COUNTRY_CODE};
// Must use separate enum for fonts with MAX_FONT at end to use gslc_FontSet.
enum {E_FONT_MONO9,E_FONT_SANS9,MAX_FONT};
//<Enum !End!>

// ------------------------------------------------
// Instantiate the GUI
// ------------------------------------------------

// ------------------------------------------------
// Define the maximum number of elements and pages
// ------------------------------------------------
//<ElementDefines !Start!>
#define MAX_PAGE                1

#define MAX_ELEM_PG_MAIN 7                                          // # Elems total on page
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
gslc_tsXListbox                 m_sListbox1;
// - Note that XLISTBOX_BUF_OH_R is extra required per item
char                            m_acListboxBuf1[81 + XLISTBOX_BUF_OH_R];
gslc_tsXSlider                  m_sListScroll1;

#define MAX_STR                 100

//<GUI_Extra_Elements !End!>

// ------------------------------------------------
// Program Globals
// ------------------------------------------------
bool      m_bQuit = false;

#define COUNTRY_CNT  12
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


// Save some element references for direct access
//<Save_References !Start!>
gslc_tsElemRef*  m_pElemListbox    = NULL;
gslc_tsElemRef*  m_pElemOK         = NULL;
gslc_tsElemRef*  m_pElemSel        = NULL;
gslc_tsElemRef*  m_pListSlider1    = NULL;
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
        m_bQuit = true;
        gslc_ElemSetTxtStr(&m_gui,m_pElemOK,"DONE");
        gslc_ElemSetCol(&m_gui,m_pElemOK,GSLC_COL_RED,GSLC_COL_BLACK,GSLC_COL_BLACK);
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
bool CbListbox(void* pvGui, void* pvElemRef, int16_t nSelId)
{
  gslc_tsGui*     pGui = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem*    pElem = gslc_GetElemFromRef(pGui, pElemRef);
  char            acTxt[MAX_STR + 1];
  
  if (pElemRef == NULL) {
    return false;
  }

  // From the element's ID we can determine which listbox was active.
  switch (pElem->nId) {
//<Listbox Enums !Start!>
    case E_LISTBOX:
      // Update the status message with the selection
      if (nSelId != XLISTBOX_SEL_NONE) {
        strncpy(acTxt, m_astrCountryCodes[nSelId], 5);
        gslc_ElemSetTxtStr(&m_gui, m_pElemSel, acTxt);
      }
      break;

//<Listbox Enums !End!>
    default:
      break;
  }
  return true;
}
//<Draw Callback !Start!>
//<Draw Callback !End!>

// Callback function for when a slider's position has been updated
bool CbSlidePos(void* pvGui,void* pvElemRef,int16_t nPos)
{
  gslc_tsGui*     pGui      = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef  = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem*    pElem     = pElemRef->pElem;
  int16_t         nVal;

  // From the element's ID we can determine which slider was updated.
  switch (pElem->nId) {
//<Slider Enums !Start!>

    case E_LISTSCROLL1:
      // Fetch the slider position
      nVal = gslc_ElemXSliderGetPos(pGui,m_pListSlider1);
      gslc_ElemXListboxSetScrollPos(pGui, m_pElemListbox, nVal);
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
  gslc_PageAdd(&m_gui,E_PG_MAIN,m_asPage1Elem,MAX_ELEM_PG_MAIN_RAM,m_asPage1ElemRef,MAX_ELEM_PG_MAIN);

  // NOTE: The current page defaults to the first page added. Here we explicitly
  //       ensure that the main page is the correct page no matter the add order.
  gslc_SetPageCur(&m_gui,E_PG_MAIN);
  
  // Set Background to a flat color
  gslc_SetBkgndColor(&m_gui,GSLC_COL_GRAY_DK2);

  // -----------------------------------
  // PAGE: E_PG_MAIN
  
   
  // Create wrapping box for listbox E_LISTBOX and scrollbar
  pElemRef = gslc_ElemCreateBox(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){17,75,200,100});
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_GRAY,GSLC_COL_GRAY_DK3,GSLC_COL_GRAY_DK2);
  
  // Create listbox
  pElemRef = gslc_ElemXListboxCreate(&m_gui,E_LISTBOX,E_PG_MAIN,&m_sListbox1,
    (gslc_tsRect){17+2,75+4,200-23,100-7},E_FONT_MONO9,
    (char*)&m_acListboxBuf1,sizeof(m_acListboxBuf1),0);
  gslc_ElemXListboxSetSize(&m_gui, pElemRef, 5, 1); // 5 rows, 1 columns
  gslc_ElemXListboxItemsSetSize(&m_gui, pElemRef, XLISTBOX_SIZE_AUTO, XLISTBOX_SIZE_AUTO);
  gslc_ElemSetTxtMarginXY(&m_gui, pElemRef, 5, 0);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_WHITE);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_GRAY,GSLC_COL_GRAY_DK3,GSLC_COL_GRAY_DK2);
  gslc_ElemXListboxSetSelFunc(&m_gui, pElemRef, &CbListbox);
  gslc_ElemXListboxAddItem(&m_gui, pElemRef, "USA");
  gslc_ElemXListboxAddItem(&m_gui, pElemRef, "Brazil");
  gslc_ElemXListboxAddItem(&m_gui, pElemRef, "Canada");
  gslc_ElemXListboxAddItem(&m_gui, pElemRef, "Denmark");
  gslc_ElemXListboxAddItem(&m_gui, pElemRef, "Germany");
  gslc_ElemXListboxAddItem(&m_gui, pElemRef, "France");
  gslc_ElemXListboxAddItem(&m_gui, pElemRef, "India");
  gslc_ElemXListboxAddItem(&m_gui, pElemRef, "Japan");
  gslc_ElemXListboxAddItem(&m_gui, pElemRef, "Mexico");
  gslc_ElemXListboxAddItem(&m_gui, pElemRef, "Peru");
  gslc_ElemXListboxAddItem(&m_gui, pElemRef, "England");
  gslc_ElemXListboxAddItem(&m_gui, pElemRef, "Vietnam");
  gslc_ElemSetFrameEn(&m_gui,pElemRef,true);
  m_pElemListbox = pElemRef;

  // Create vertical scrollbar for listbox
  pElemRef = gslc_ElemXSliderCreate(&m_gui,E_LISTSCROLL1,E_PG_MAIN,&m_sListScroll1,
          (gslc_tsRect){17+200-21,75+4,20,100-8},0,12,0,5,true);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_BLUE_LT1,GSLC_COL_BLACK,GSLC_COL_BLUE_LT1);
  gslc_ElemXSliderSetPosFunc(&m_gui,pElemRef,&CbSlidePos);
  m_pListSlider1 = pElemRef;
  
  // Create E_TXT_COUNTRY_CODE runtime modifiable text
  static char m_sDisplayText2[6] = "US";
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_TXT_COUNTRY_CODE,E_PG_MAIN,(gslc_tsRect){125,45,105,23},
    (char*)m_sDisplayText2,6,E_FONT_SANS9);
  gslc_ElemSetTxtAlign(&m_gui,pElemRef,GSLC_ALIGN_MID_MID);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_GRAY,GSLC_COL_GRAY_DK2,GSLC_COL_BLACK);
  m_pElemSel = pElemRef;
  
  // Create E_LBL_COUNTRYCD text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_LBL_COUNTRYCD,E_PG_MAIN,(gslc_tsRect){14,45,121,23},
    (char*)"Country Code:",0,E_FONT_SANS9);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_GRAY,GSLC_COL_GRAY_DK2,GSLC_COL_BLACK);
  
  // Create E_LBL_TITLE text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_LBL_TITLE,E_PG_MAIN,(gslc_tsRect){49,20,141,23},
    (char*)"Country Chooser",0,E_FONT_SANS9);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_GRAY,GSLC_COL_GRAY_DK2,GSLC_COL_BLACK);
  
  // Create E_BTN_OK button with modifiable text label
  static char m_strbtn1[7] = "OK";
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_BTN_OK,E_PG_MAIN,
    (gslc_tsRect){157,190,60,30},
    (char*)m_strbtn1,7,E_FONT_SANS9,&CbBtnCommon);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_WHITE);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_GREEN_DK2,GSLC_COL_GREEN_DK4,GSLC_COL_GREEN_DK1);
  m_pElemOK = pElemRef;
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
    if (!gslc_FontSet(&m_gui,E_FONT_MONO9,GSLC_FONTREF_PTR,&FreeMono9pt7b,1)) { return; }
    if (!gslc_FontSet(&m_gui,E_FONT_SANS9,GSLC_FONTREF_PTR,&FreeSans9pt7b,1)) { return; }
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
  
  // ------------------------------------------------
  // Periodically call GUIslice update function
  // ------------------------------------------------
  gslc_Update(&m_gui);
    
  if (m_bQuit) {
    gslc_Quit(&m_gui);
    while (1) {}
  }
}
