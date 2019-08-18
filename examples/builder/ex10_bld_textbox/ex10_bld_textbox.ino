//<File !Start!>
// FILE: [ex10_bld_textbox.ino]
// Created by GUIslice Builder version: [0.13.0]
//
// GUIslice Builder Generated File
//
// For the latest guides, updates and support view:
// https://github.com/ImpulseAdventure/GUIslice
//
//<File !End!>
//
// - Example 10 (Arduino):
//   - Demonstrate textbox controls
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
#include "elem/XSlider.h"
#include "elem/XTextbox.h"
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
enum {E_PG_MAIN};
enum {E_ELEM_BOX1,E_ELEM_SLIDER1,E_ELEM_TBTN1,E_ELEM_TEXTBOX1
      ,E_ELEM_TXT_COUNT,E_LBL_TITLE,E_SCROLLBAR};
// Must use separate enum for fonts with MAX_FONT at end to use gslc_FontSet.
enum {E_FONT_TXT15,E_FONT_TXT5,MAX_FONT};
//<Enum !End!>

// ------------------------------------------------
// Instantiate the GUI
// ------------------------------------------------

// ------------------------------------------------
// Define the maximum number of elements and pages
// ------------------------------------------------
//<ElementDefines !Start!>
#define MAX_PAGE                1

#define MAX_ELEM_PG_MAIN 8                                          // # Elems total on page
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
gslc_tsXSlider                  m_sXSlider1;
gslc_tsXTextbox                 m_sTextbox1;
char                            m_acTextboxBuf1[192]; // NRows=12 NCols=16
gslc_tsXSlider                  m_sTextScroll1;

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
gslc_tsElemRef*  m_pElemCount      = NULL;
gslc_tsElemRef*  m_pElemQuit       = NULL;
gslc_tsElemRef*  m_pElemSlider     = NULL;
gslc_tsElemRef*  m_pElemSlider1    = NULL;
gslc_tsElemRef*  m_pElemTextbox    = NULL;
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
      case E_ELEM_TBTN1:
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

  char acTxt[21];
  
  // From the element's ID we can determine which slider was updated.
  switch (pElem->nId) {
//<Slider Enums !Start!>
    case E_ELEM_SLIDER1:
      // Fetch the slider position
      nVal = gslc_ElemXSliderGetPos(pGui,m_pElemSlider1);
      // Link slider to the numerical display
      snprintf(acTxt,20,(char*)"%u",nVal);
      gslc_ElemSetTxtStr(pGui,m_pElemCount,acTxt);

      // Link slider to insertion of text into textbox
      snprintf(acTxt,20,(char*)"Slider=%3u\n",nVal);
      gslc_ElemXTextboxAdd(pGui,m_pElemTextbox,acTxt);

      break;
    case E_SCROLLBAR:
      // Fetch the slider position
      nVal = gslc_ElemXSliderGetPos(pGui,m_pElemSlider);
      gslc_ElemXTextboxScrollSet(pGui,m_pElemTextbox,nVal,100);
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
  
  
  // Create E_LBL_TITLE text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_LBL_TITLE,E_PG_MAIN,(gslc_tsRect){94,0,128,28},
    (char*)"Textbox",0,E_FONT_TXT15);
   
  // Create E_ELEM_BOX1 box
  pElemRef = gslc_ElemCreateBox(&m_gui,E_ELEM_BOX1,E_PG_MAIN,(gslc_tsRect){10,50,300,180});
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_WHITE,GSLC_COL_BLACK,GSLC_COL_BLACK);

  // Create slider E_ELEM_SLIDER1 
  pElemRef = gslc_ElemXSliderCreate(&m_gui,E_ELEM_SLIDER1,E_PG_MAIN,&m_sXSlider1,
          (gslc_tsRect){20,60,140,20},0,100,50,5,false);
  gslc_ElemXSliderSetStyle(&m_gui,pElemRef,true,GSLC_COL_GREEN_DK4,10,5,GSLC_COL_GRAY_LT2);
  gslc_ElemXSliderSetPosFunc(&m_gui,pElemRef,&CbSlidePos);
  m_pElemSlider1 = pElemRef;
  
  // Create E_ELEM_TXT_COUNT runtime modifiable text
  static char m_sDisplayText2[8] = "";
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TXT_COUNT,E_PG_MAIN,(gslc_tsRect){180,60,44,12},
    (char*)m_sDisplayText2,8,E_FONT_TXT5);
  m_pElemCount = pElemRef;
   
  // Create wrapping box for textbox E_ELEM_TEXTBOX1 and scrollbar
  pElemRef = gslc_ElemCreateBox(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){18,83,203,64});
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_BLUE,GSLC_COL_BLACK,GSLC_COL_GRAY_DK3);
  
  // Create textbox
  pElemRef = gslc_ElemXTextboxCreate(&m_gui,E_ELEM_TEXTBOX1,E_PG_MAIN,&m_sTextbox1,
    (gslc_tsRect){18+2,83+4,203-23,64-7},E_FONT_TXT5,
    (char*)&m_acTextboxBuf1,12,16);
  gslc_ElemXTextboxWrapSet(&m_gui,pElemRef,true);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_YELLOW);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_BLUE,GSLC_COL_BLACK,GSLC_COL_GRAY_DK3);
  m_pElemTextbox = pElemRef;

  // Create vertical scrollbar for textbox
  pElemRef = gslc_ElemXSliderCreate(&m_gui,E_SCROLLBAR,E_PG_MAIN,&m_sTextScroll1,
          (gslc_tsRect){18+203-21,83+4,20,64-8},0,255,0,5,true);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_GRAY,GSLC_COL_BLACK,GSLC_COL_GRAY);
  gslc_ElemXSliderSetPosFunc(&m_gui,pElemRef,&CbSlidePos);
  m_pElemSlider = pElemRef;
  
  // Create E_ELEM_TBTN1 button with modifiable text label
  static char m_strbtn1[7] = "QUIT";
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_TBTN1,E_PG_MAIN,
    (gslc_tsRect){250,60,50,30},
    (char*)m_strbtn1,7,E_FONT_TXT5,&CbBtnCommon);
  m_pElemQuit = pElemRef;
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
    if (!gslc_FontSet(&m_gui,E_FONT_TXT15,GSLC_FONTREF_PTR,NULL,3)) { return; }
    if (!gslc_FontSet(&m_gui,E_FONT_TXT5,GSLC_FONTREF_PTR,NULL,1)) { return; }
//<Load_Fonts !End!>

  // ------------------------------------------------
  // Create graphic elements
  // ------------------------------------------------
  InitGUI();

//<Startup !Start!>
//<Startup !End!>

  // Insert some text
  gslc_ElemXTextboxAdd(&m_gui,m_pElemTextbox,(char*)"Welcome\n");
  gslc_ElemXTextboxAdd(&m_gui,m_pElemTextbox,(char*)"Hi ");
  gslc_ElemXTextboxAdd(&m_gui,m_pElemTextbox,(char*)"Long line here that may wrap\n");
  gslc_ElemXTextboxAdd(&m_gui,m_pElemTextbox,(char*)"End...\n");

}

// -----------------------------------
// Main event loop
// -----------------------------------
void loop()
{
  char                acTxt[MAX_STR];

  // ------------------------------------------------
  // Update GUI Elements
  // ------------------------------------------------
  
  // General counter
  m_nCount++;

  if ((m_nCount % 5000) == 0) {
    snprintf(acTxt,MAX_STR,"Step %u\n",m_nCount);
    gslc_ElemXTextboxAdd(&m_gui,m_pElemTextbox,acTxt);
  }

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
    while (1) { }
  }
}
