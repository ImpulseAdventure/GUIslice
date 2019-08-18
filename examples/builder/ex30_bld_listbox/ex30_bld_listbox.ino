//<File !Start!>
// FILE: [ex30_bld_listbox.ino]
// Created by GUIslice Builder version: [0.13.0]
//
// GUIslice Builder Generated File
//
// For the latest guides, updates and support view:
// https://github.com/ImpulseAdventure/GUIslice
//
//<File !End!>
//
// - Example 30 (Arduino): Listbox
//   - Demonstrates Listbox control (multi-column)
//   - Shows callback notifications for Listbox
//   - Provide example of additional Adafruit-GFX fonts
//     (see USE_EXTRA_FONTS)
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
#include "elem/XListbox.h"
//<Includes !End!>

// ------------------------------------------------
// Headers and Defines for fonts
// Note that font files are located within the Adafruit-GFX library folder:
// ------------------------------------------------
//<Fonts !Start!>
#include <Adafruit_GFX.h>
// Note that these files are located within the Adafruit-GFX library folder:
#include "Fonts/FreeMono9pt7b.h"
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
enum {E_ELEM_BOX1,E_ELEM_LISTBOX1,E_ELEM_QUIT,E_ELEM_SELECTED
      ,E_LBLSELECTED};
// Must use separate enum for fonts with MAX_FONT at end to use gslc_FontSet.
enum {E_FONT_MONO9,E_FONT_TXT5,MAX_FONT};
//<Enum !End!>

// ------------------------------------------------
// Instantiate the GUI
// ------------------------------------------------

// ------------------------------------------------
// Define the maximum number of elements and pages
// ------------------------------------------------
//<ElementDefines !Start!>
#define MAX_PAGE                1

#define MAX_ELEM_PAGE_MAIN 5                                          // # Elems total on page
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
gslc_tsXListbox                 m_sListbox1;
// - Note that XLISTBOX_BUF_OH_R is extra required per item
char                            m_acListboxBuf1[43 + XLISTBOX_BUF_OH_R];

#define MAX_STR                 100

//<GUI_Extra_Elements !End!>

// ------------------------------------------------
// Program Globals
// ------------------------------------------------
bool        m_bQuit = false;

// Save some element references for direct access
//<Save_References !Start!>
gslc_tsElemRef*  m_pElemListbox    = NULL;
gslc_tsElemRef*  m_pElemQuit       = NULL;
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
      case E_ELEM_QUIT:
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
    case E_ELEM_LISTBOX1:
      if (nSelId != XLISTBOX_SEL_NONE) {
        gslc_ElemXListboxGetItem(&m_gui, pElemRef, nSelId, acTxt, MAX_STR);
      } else {
        strncpy(acTxt,"None",MAX_STR);
      }
      gslc_ElemSetTxtStr(&m_gui, m_pElemSel, acTxt);
      break;

//<Listbox Enums !End!>
    default:
      break;
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
  gslc_PageAdd(&m_gui,E_PAGE_MAIN,m_asPage1Elem,MAX_ELEM_PAGE_MAIN_RAM,m_asPage1ElemRef,MAX_ELEM_PAGE_MAIN);

  // NOTE: The current page defaults to the first page added. Here we explicitly
  //       ensure that the main page is the correct page no matter the add order.
  gslc_SetPageCur(&m_gui,E_PAGE_MAIN);
  
  // Set Background to a flat color
  gslc_SetBkgndColor(&m_gui,GSLC_COL_GRAY_DK2);

  // -----------------------------------
  // PAGE: E_PAGE_MAIN
  
   
  // Create E_ELEM_BOX1 box
  pElemRef = gslc_ElemCreateBox(&m_gui,E_ELEM_BOX1,E_PAGE_MAIN,(gslc_tsRect){10,10,300,220});
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_WHITE,GSLC_COL_BLACK,GSLC_COL_BLACK);
  
  // Create E_ELEM_QUIT button with modifiable text label
  static char m_strbtn1[7] = "QUIT";
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_QUIT,E_PAGE_MAIN,
    (gslc_tsRect){240,20,60,30},
    (char*)m_strbtn1,7,E_FONT_TXT5,&CbBtnCommon);
  m_pElemQuit = pElemRef;
  
  // Create E_LBLSELECTED text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_LBLSELECTED,E_PAGE_MAIN,(gslc_tsRect){20,20,56,12},
    (char*)"Selected:",0,E_FONT_TXT5);
  
  // Create E_ELEM_SELECTED runtime modifiable text
  static char m_sDisplayText2[11] = "Red";
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_SELECTED,E_PAGE_MAIN,(gslc_tsRect){80,20,62,12},
    (char*)m_sDisplayText2,11,E_FONT_TXT5);
  gslc_ElemSetFrameEn(&m_gui,pElemRef,true);
  m_pElemSel = pElemRef;
   
  // Create listbox
  pElemRef = gslc_ElemXListboxCreate(&m_gui,E_ELEM_LISTBOX1,E_PAGE_MAIN,&m_sListbox1,
    (gslc_tsRect){60,50,160,160},E_FONT_MONO9,
    (char*)&m_acListboxBuf1,sizeof(m_acListboxBuf1),0);
  gslc_ElemXListboxSetSize(&m_gui, pElemRef, 4, 2); // 4 rows, 2 columns
  gslc_ElemXListboxItemsSetSize(&m_gui, pElemRef, XLISTBOX_SIZE_AUTO, XLISTBOX_SIZE_AUTO);
  gslc_ElemSetTxtMarginXY(&m_gui, pElemRef, 5, 0);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_WHITE);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_BLUE_DK3,GSLC_COL_GRAY_DK3,GSLC_COL_GRAY_DK2);
  gslc_ElemXListboxSetSelFunc(&m_gui, pElemRef, &CbListbox);
  gslc_ElemXListboxAddItem(&m_gui, pElemRef, "Red");
  gslc_ElemXListboxAddItem(&m_gui, pElemRef, "Orange");
  gslc_ElemXListboxAddItem(&m_gui, pElemRef, "Yellow");
  gslc_ElemXListboxAddItem(&m_gui, pElemRef, "Green");
  gslc_ElemXListboxAddItem(&m_gui, pElemRef, "Blue");
  gslc_ElemXListboxAddItem(&m_gui, pElemRef, "Indigo");
  gslc_ElemXListboxAddItem(&m_gui, pElemRef, "Violet");
  gslc_ElemSetFrameEn(&m_gui,pElemRef,true);
  m_pElemListbox = pElemRef;
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
