//<File !Start!>
// FILE: [ex11_bld_graph.ino]
// Created by GUIslice Builder version: [0.13.0]
//
// GUIslice Builder Generated File
//
// For the latest guides, updates and support view:
// https://github.com/ImpulseAdventure/GUIslice
//
//<File !End!>
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
#include "elem/XGraph.h"
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
enum {E_ELEM_BOX2,E_ELEM_BTN_QUIT,E_ELEM_GRAPH1,E_ELEM_TXT_COUNT
      ,E_LBL_TITLE,E_SCROLLBAR,E_SLIDER,E_WRAP_GRAPH};
// Must use separate enum for fonts with MAX_FONT at end to use gslc_FontSet.
enum {E_FONT_SANS12,E_FONT_SANS9,E_FONT_TXT5,MAX_FONT};
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
gslc_tsXGraph                   m_sGraph1;
int16_t                         m_anGraphBuf1[200]; // NRows=200
gslc_tsXSlider                  m_sXSlider2;

#define MAX_STR                 100

//<GUI_Extra_Elements !End!>

// ------------------------------------------------
// Program Globals
// ------------------------------------------------
bool      m_bQuit = false;

// Free-running counter for display
unsigned  m_nCount = 0;

#define GRAPH_ROWS          200
int16_t                     m_anGraphBuf[GRAPH_ROWS];

// Save some element references for direct access
//<Save_References !Start!>
gslc_tsElemRef*  m_pElemCnt        = NULL;
gslc_tsElemRef*  m_pElemGraph1     = NULL;
gslc_tsElemRef*  m_pElemQuit       = NULL;
gslc_tsElemRef*  m_pElemSlider1    = NULL;
gslc_tsElemRef*  m_pElemSlider2    = NULL;
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
  char            acTxt[20];

  // From the element's ID we can determine which slider was updated.
  switch (pElem->nId) {
//<Slider Enums !Start!>

    case E_SLIDER:
      // Fetch the slider position
      nVal = gslc_ElemXSliderGetPos(pGui,m_pElemSlider1);
      // Link slider to the numerical display
      snprintf(acTxt,6,"%u",nVal);
      gslc_ElemSetTxtStr(pGui,m_pElemCnt,acTxt);

      // Link slider to insertion of values into graph
      gslc_ElemXGraphAdd(pGui,m_pElemGraph1,nVal);
      break;
    case E_SCROLLBAR:
      // Fetch the slider position
      nVal = gslc_ElemXSliderGetPos(pGui,m_pElemSlider2);
      // Update the graph scroll position
      gslc_ElemXGraphScrollSet(pGui,m_pElemGraph1,nVal,100);
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
  
   
  // Create E_ELEM_BOX2 box
  pElemRef = gslc_ElemCreateBox(&m_gui,E_ELEM_BOX2,E_PG_MAIN,(gslc_tsRect){10,50,300,180});
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_WHITE,GSLC_COL_BLACK,GSLC_COL_BLACK);
  
  // Create E_LBL_TITLE text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_LBL_TITLE,E_PG_MAIN,(gslc_tsRect){127,5,65,29},
    (char*)"Graph",0,E_FONT_SANS12);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,(gslc_tsColor){128,128,240});

  // Create slider E_SLIDER 
  pElemRef = gslc_ElemXSliderCreate(&m_gui,E_SLIDER,E_PG_MAIN,&m_sXSlider1,
          (gslc_tsRect){20,60,140,20},0,100,50,5,false);
  gslc_ElemXSliderSetStyle(&m_gui,pElemRef,true,GSLC_COL_GREEN_DK4,10,5,GSLC_COL_GRAY_DK2);
  gslc_ElemXSliderSetPosFunc(&m_gui,pElemRef,&CbSlidePos);
  m_pElemSlider1 = pElemRef;
  
  // Create E_ELEM_TXT_COUNT runtime modifiable text
  static char m_sDisplayText2[6] = "";
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TXT_COUNT,E_PG_MAIN,(gslc_tsRect){180,60,65,23},
    (char*)m_sDisplayText2,6,E_FONT_SANS9);
  m_pElemCnt = pElemRef;
   
  // Create E_WRAP_GRAPH box
  pElemRef = gslc_ElemCreateBox(&m_gui,E_WRAP_GRAPH,E_PG_MAIN,(gslc_tsRect){18,83,183,143});
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_BLUE_DK4,GSLC_COL_BLACK,GSLC_COL_BLACK);

  // Create graph E_ELEM_GRAPH1
  pElemRef = gslc_ElemXGraphCreate(&m_gui,E_ELEM_GRAPH1,E_PG_MAIN,
    &m_sGraph1,(gslc_tsRect){20,85,180,120},E_FONT_TXT5,(int16_t*)&m_anGraphBuf1,
        200,(gslc_tsColor){255,200,0});
  gslc_ElemXGraphSetStyle(&m_gui,pElemRef, GSLCX_GRAPH_STYLE_DOT, 5);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_BLUE_LT2,GSLC_COL_BLACK,GSLC_COL_GRAY_DK3);
  m_pElemGraph1 = pElemRef;

  // Create slider E_SCROLLBAR 
  pElemRef = gslc_ElemXSliderCreate(&m_gui,E_SCROLLBAR,E_PG_MAIN,&m_sXSlider2,
          (gslc_tsRect){20,205,180,20},0,100,100,5,false);
  gslc_ElemXSliderSetStyle(&m_gui,pElemRef,false,GSLC_COL_BLUE,10,5,GSLC_COL_BLUE);
  gslc_ElemXSliderSetPosFunc(&m_gui,pElemRef,&CbSlidePos);
  m_pElemSlider2 = pElemRef;
  
  // Create E_ELEM_BTN_QUIT button with modifiable text label
  static char m_strbtn1[7] = "Quit";
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_QUIT,E_PG_MAIN,
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
    if (!gslc_FontSet(&m_gui,E_FONT_SANS12,GSLC_FONTREF_PTR,&FreeSans12pt7b,1)) { return; }
    if (!gslc_FontSet(&m_gui,E_FONT_SANS9,GSLC_FONTREF_PTR,&FreeSans9pt7b,1)) { return; }
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
  
  m_nCount++;
  if ((m_nCount % 500) == 0) {
    gslc_ElemXGraphAdd(&m_gui,m_pElemGraph1,(m_nCount/250)%100);
  }
  
  // ------------------------------------------------
  // Periodically call GUIslice update function
  // ------------------------------------------------
  gslc_Update(&m_gui);
    
  // Slow down updates
  delay(2);

  // In a real program, we would detect the button press and take an action.
  // For this Arduino demo, we will pretend to exit by emulating it with an
  // infinite loop. Note that interrupts are not disabled so that any debug
  // messages via Serial have an opportunity to be transmitted.
  if (m_bQuit) {
    gslc_Quit(&m_gui);
    while (1) { }
  }
    
}
