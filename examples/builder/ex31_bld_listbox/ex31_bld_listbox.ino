//<File !Start!>
// FILE: [ex31_bld_listbox.ino]
// Created by GUIslice Builder version: [0.16.0]
//
// GUIslice Builder Generated GUI Framework File
//
// For the latest guides, updates and support view:
// https://github.com/ImpulseAdventure/GUIslice
//
//<File !End!>
//
// - Example 31 (Arduino): Listbox + Scrollbar
//   - Demonstrates Listbox control (single-column) with scrollbar
//     Usage of InsertItemAt and DeleteItemAt for List.
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
#if defined(DRV_DISP_TFT_ESPI)
  #error Project tab->Target Platform should be tft_espi
#endif
#include <Adafruit_GFX.h>
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
enum {E_PG_MAIN,E_PG_WARNING};
enum {E_BTN_ADD,E_BTN_DEL,E_BTN_OK,E_ELEM_BOX1,E_ELEM_TEXT4
      ,E_ELEM_TEXT5,E_LBL_COUNTRYCD,E_LBL_TITLE,E_LISTBOX,E_LISTSCROLL1
      ,E_TXT_COUNTRY_CODE};
// Must use separate enum for fonts with MAX_FONT at end to use gslc_FontSet.
enum {E_BUILTIN10X16,E_BUILTIN5X8,E_FREEMONO9,E_FREESANS9,MAX_FONT};
//<Enum !End!>

// ------------------------------------------------
// Instantiate the GUI
// ------------------------------------------------

// ------------------------------------------------
// Define the maximum number of elements and pages
// ------------------------------------------------
//<ElementDefines !Start!>
#define MAX_PAGE                2

#define MAX_ELEM_PG_MAIN 8 // # Elems total on page
#define MAX_ELEM_PG_MAIN_RAM MAX_ELEM_PG_MAIN // # Elems in RAM

#define MAX_ELEM_PG_WARNING 4 // # Elems total on page
#define MAX_ELEM_PG_WARNING_RAM MAX_ELEM_PG_WARNING // # Elems in RAM
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
gslc_tsElem                     m_asPopup1Elem[MAX_ELEM_PG_WARNING_RAM];
gslc_tsElemRef                  m_asPopup1ElemRef[MAX_ELEM_PG_WARNING];
gslc_tsXListbox                 m_sListbox1;
// - Note that XLISTBOX_BUF_OH_R is extra required per item
char                            m_acListboxBuf1[100 + XLISTBOX_BUF_OH_R];
gslc_tsXSlider                  m_sListScroll1;

#define MAX_STR                 20

//<GUI_Extra_Elements !End!>

// ------------------------------------------------
// Program Globals
// ------------------------------------------------
bool      m_bQuit = false;
int16_t   m_nNumNewCountry = 0; // Counter for New Countries
char      m_acNewCode[6];

#define COUNTRY_CNT  12

char m_CountryCodes[COUNTRY_CNT][4] = {
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
  "VN",
};

char m_CountryNames[COUNTRY_CNT][10] = {
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
  "Vietnam",
};

// This is a basic example demonstrating a conversion
// from full country name to a shortened country
// code. In addition to the built-in country code
// list, it fakes out codes for "New Country" entries.
char* searchCountryCode(char* countryName)
{
  // Look through built-in list of countries
  for(int i=0; i<COUNTRY_CNT; i++) {
    if (strcmp(countryName,(char*)&m_CountryNames[i]) == 0) {
      return m_CountryCodes[i];
    }
  }

  // In this simplistic example, when dealing with "New Country##" entries,
  // we just extract the country index from the name, ie:
  //   "New Country5" -> "NEW5"
  if (strncmp(countryName,"New Country",11) == 0) {
    sprintf(m_acNewCode,"NEW%s",(char*)&countryName[11]);
    return m_acNewCode;
  }
  
  // If no matches, return empty string
  return (char*)("");
}


// Save some element references for direct access
//<Save_References !Start!>
gslc_tsElemRef* m_pElemListbox    = NULL;
gslc_tsElemRef* m_pElemSel        = NULL;
gslc_tsElemRef* m_pListSlider1    = NULL;
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
  gslc_tsElem* pElem = gslc_GetElemFromRef(&m_gui,pElemRef);

  char    acTxt[MAX_STR+1];
  int16_t nSelId;
  
  if ( eTouch == GSLC_TOUCH_UP_IN ) {
    // From the element's ID we can determine which button was pressed.
    switch (pElem->nId) {
//<Button Enums !Start!>
      case E_BTN_ADD:
        sprintf(acTxt,"New Country%d",m_nNumNewCountry+1);
        if (gslc_ElemXListboxInsertItemAt(&m_gui,m_pElemListbox,0,acTxt)) {
          // successfully added new country
          m_nNumNewCountry++;
          gslc_ElemSetTxtStr(&m_gui, m_pElemSel, searchCountryCode(acTxt));
        } else {
          gslc_ElemSetTxtStr(&m_gui, m_pElemSel, "");
          gslc_PopupShow(&m_gui, E_PG_WARNING, true);
        }
        break;
      case E_BTN_DEL:
        nSelId = gslc_ElemXListboxGetSel(&m_gui, m_pElemListbox);
        if (nSelId != XLISTBOX_SEL_NONE) {
          gslc_ElemXListboxDeleteItemAt(&m_gui,m_pElemListbox,nSelId);
          gslc_ElemSetTxtStr(&m_gui, m_pElemSel, "");
        } else {
          Serial.println("No country selected for deletion");
        }
        break;
      case E_BTN_OK:
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
bool CbListbox(void* pvGui, void* pvElemRef, int16_t nSelId)
{
  gslc_tsGui*     pGui = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem*    pElem = gslc_GetElemFromRef(pGui, pElemRef);
  char            acTxt[MAX_STR+1];
  
  if (pElemRef == NULL) {
    return false;
  }

  // From the element's ID we can determine which listbox was active.
  switch (pElem->nId) {
//<Listbox Enums !Start!>
    case E_LISTBOX:
      // Update the status message with the selection
      if (nSelId != XLISTBOX_SEL_NONE) {
        gslc_ElemXListboxGetItem(&m_gui,m_pElemListbox,nSelId,acTxt,sizeof(acTxt));
        gslc_ElemSetTxtStr(&m_gui, m_pElemSel, searchCountryCode(acTxt));
      } else {
        gslc_ElemSetTxtStr(&m_gui, m_pElemSel, "");
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
  gslc_tsElem*    pElem     = gslc_GetElemFromRef(pGui,pElemRef);
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
  int8_t nInd;  
  gslc_tsElemRef* pElemRef = NULL;

//<InitGUI !Start!>
  gslc_PageAdd(&m_gui,E_PG_MAIN,m_asPage1Elem,MAX_ELEM_PG_MAIN_RAM,m_asPage1ElemRef,MAX_ELEM_PG_MAIN);
  gslc_PageAdd(&m_gui,E_PG_WARNING,m_asPopup1Elem,MAX_ELEM_PG_WARNING_RAM,m_asPopup1ElemRef,MAX_ELEM_PG_WARNING);

  // NOTE: The current page defaults to the first page added. Here we explicitly
  //       ensure that the main page is the correct page no matter the add order.
  gslc_SetPageCur(&m_gui,E_PG_MAIN);
  
  // Set Background to a flat color
  gslc_SetBkgndColor(&m_gui,GSLC_COL_BLACK);

  // -----------------------------------
  // PAGE: E_PG_MAIN
  
   
  // Create wrapping box for listbox E_LISTBOX and scrollbar
  pElemRef = gslc_ElemCreateBox(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){20,75,200,100});
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_GRAY,GSLC_COL_GRAY_DK3,GSLC_COL_GRAY_DK2);
  
  // Create listbox
  pElemRef = gslc_ElemXListboxCreate(&m_gui,E_LISTBOX,E_PG_MAIN,&m_sListbox1,
    (gslc_tsRect){20+2,75+4,200-23,100-7},E_FREEMONO9,
    (uint8_t*)&m_acListboxBuf1,sizeof(m_acListboxBuf1),0);
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
          (gslc_tsRect){20+200-21,75+4,20,100-8},0,20,0,5,true);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_BLUE_LT1,GSLC_COL_BLACK,GSLC_COL_BLUE_LT1);
  gslc_ElemXSliderSetPosFunc(&m_gui,pElemRef,&CbSlidePos);
  m_pListSlider1 = pElemRef;
  
  // Create E_TXT_COUNTRY_CODE runtime modifiable text
  static char m_sDisplayText2[6] = "US";
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_TXT_COUNTRY_CODE,E_PG_MAIN,(gslc_tsRect){130,45,90,23},
    (char*)m_sDisplayText2,6,E_FREESANS9);
  gslc_ElemSetTxtAlign(&m_gui,pElemRef,GSLC_ALIGN_MID_MID);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_GRAY,GSLC_COL_GRAY_DK2,GSLC_COL_BLACK);
  m_pElemSel = pElemRef;
  
  // Create E_LBL_COUNTRYCD text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_LBL_COUNTRYCD,E_PG_MAIN,(gslc_tsRect){10,45,121,23},
    (char*)"Country Code:",0,E_FREESANS9);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_GRAY,GSLC_COL_GRAY_DK2,GSLC_COL_BLACK);
  
  // Create E_LBL_TITLE text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_LBL_TITLE,E_PG_MAIN,(gslc_tsRect){49,20,141,23},
    (char*)"Country Chooser",0,E_FREESANS9);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_GRAY,GSLC_COL_GRAY_DK2,GSLC_COL_BLACK);
  
  // create E_BTN_ADD button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_BTN_ADD,E_PG_MAIN,
    (gslc_tsRect){140,190,80,30},(char*)"Insert New",0,E_BUILTIN5X8,&CbBtnCommon);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_GREEN_DK2,GSLC_COL_GREEN_DK4,GSLC_COL_GREEN_DK1);
  
  // create E_BTN_DEL button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_BTN_DEL,E_PG_MAIN,
    (gslc_tsRect){20,190,80,30},(char*)"Delete",0,E_BUILTIN5X8,&CbBtnCommon);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_GREEN_DK2,GSLC_COL_GREEN_DK4,GSLC_COL_GREEN_DK1);

  // -----------------------------------
  // PAGE: E_PG_WARNING
  
   
  // Create E_ELEM_BOX1 box
  pElemRef = gslc_ElemCreateBox(&m_gui,E_ELEM_BOX1,E_PG_WARNING,(gslc_tsRect){50,50,250,150});
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_GRAY,GSLC_COL_GRAY_LT3,GSLC_COL_GRAY_LT3);
  
  // Create E_ELEM_TEXT4 text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TEXT4,E_PG_WARNING,(gslc_tsRect){60,70,100,25},
    (char*)"WARNING:",0,E_BUILTIN10X16);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_RED);
  
  // Create E_ELEM_TEXT5 text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TEXT5,E_PG_WARNING,(gslc_tsRect){60,120,228,10},
    (char*)"Buffer too small to add more  entries.",0,E_BUILTIN5X8);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_BLACK);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_GRAY,GSLC_COL_GRAY_LT3,GSLC_COL_GRAY_LT3);
  
  // create E_BTN_OK button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_BTN_OK,E_PG_WARNING,
    (gslc_tsRect){250,150,40,40},(char*)"OK",0,E_BUILTIN5X8,&CbBtnCommon);
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
    if (!gslc_FontSet(&m_gui,E_BUILTIN10X16,GSLC_FONTREF_PTR,NULL,2)) { return; }
    if (!gslc_FontSet(&m_gui,E_BUILTIN5X8,GSLC_FONTREF_PTR,NULL,1)) { return; }
    if (!gslc_FontSet(&m_gui,E_FREEMONO9,GSLC_FONTREF_PTR,&FreeMono9pt7b,1)) { return; }
    if (!gslc_FontSet(&m_gui,E_FREESANS9,GSLC_FONTREF_PTR,&FreeSans9pt7b,1)) { return; }
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
