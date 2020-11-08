// FILE: [ex47_ard_seekbar.ino]
//
// For the latest guides, updates and support view:
// https://github.com/ImpulseAdventure/GUIslice
//
//<File !End!>
//
// GUIslice Library Examples
// - Paul Conti
// - https://www.impulseadventure.com/elec/guislice-gui.html
// - https://github.com/ImpulseAdventure/GUIslice
// - Example 47 (Arduino): Seekbar 
//   - Demonstrate seekbar controls 
//     a modern slider control with a Android Look and Feel
//   - Shows callback notifications for Seekbars
//   - NOTE: This is the simple version of the example without
//     optimizing for memory consumption. Therefore, it may not
//     run on Arduino devices with limited memory. A "minimal"
//     version is located in the "arduino_min" folder which includes
//     FLASH memory optimization for reduced memory devices.
//
// For the latest guides, updates and support view:
// https://github.com/ImpulseAdventure/GUIslice
//

// ------------------------------------------------
// Headers to include
// ------------------------------------------------
#include "GUIslice.h"
#include "GUIslice_drv.h"

// Include any extended elements
#include "elem/XSeekbar.h"

// ------------------------------------------------
// Headers and Defines for fonts
// Note that font files are located within the Adafruit-GFX library folder:
// ------------------------------------------------

// ------------------------------------------------
// Defines for resources
// ------------------------------------------------

// ------------------------------------------------
// Enumerations for pages, elements, fonts, images
// ------------------------------------------------
enum {E_PG_MAIN};
enum {E_ELEM_BOX1,E_ELEM_SEEKBAR,E_ELEM_SEEKBAR3,E_ELEM_SEEKBAR4
      ,E_ELEM_SEEKBAR2,E_ELEM_TXT_SEEKBAR,E_LBL_SEEKBAR};
// Must use separate enum for fonts with MAX_FONT at end to use gslc_FontSet.
enum {E_BUILTIN_TXT10,MAX_FONT};

// ------------------------------------------------
// Instantiate the GUI
// ------------------------------------------------

// ------------------------------------------------
// Define the maximum number of elements and pages
// ------------------------------------------------
#define MAX_PAGE                1

#define MAX_ELEM_PG_MAIN 7 // # Elems total on page
#define MAX_ELEM_PG_MAIN_RAM MAX_ELEM_PG_MAIN // # Elems in RAM

// ------------------------------------------------
// Create element storage
// ------------------------------------------------
gslc_tsGui                      m_gui;
gslc_tsDriver                   m_drv;
gslc_tsFont                     m_asFont[MAX_FONT];
gslc_tsPage                     m_asPage[MAX_PAGE];

gslc_tsElem                     m_asPage1Elem[MAX_ELEM_PG_MAIN_RAM];
gslc_tsElemRef                  m_asPage1ElemRef[MAX_ELEM_PG_MAIN];
gslc_tsXSeekbar                 m_sXSeekbar1;
gslc_tsXSeekbar                 m_sXSeekbar2;
gslc_tsXSeekbar                 m_sXSeekbar3;
gslc_tsXSeekbar                 m_sXSeekbar4;

#define MAX_STR                 20


// ------------------------------------------------
// Program Globals
// ------------------------------------------------

// Save some element references for direct access
gslc_tsElemRef* m_pElemSeekbar    = NULL;
gslc_tsElemRef* m_pElemSeekbar2   = NULL;
gslc_tsElemRef* m_pElemSeekbar3   = NULL;
gslc_tsElemRef* m_pElemSeekbar4   = NULL;
gslc_tsElemRef* m_pElemSeekbarTxt = NULL;

// Define debug message function
static int16_t DebugOut(char ch) { if (ch == (char)'\n') Serial.println(""); else Serial.write(ch); return 0; }

// ------------------------------------------------
// Callback Methods
// ------------------------------------------------

// Callback function for when a slider's position has been updated
bool CbSlidePos(void* pvGui,void* pvElemRef,int16_t nPos)
{
  gslc_tsGui*     pGui     = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem*    pElem    = gslc_GetElemFromRef(pGui,pElemRef);
  int16_t         nVal;
  char            acTxt[MAX_STR];

  // From the element's ID we can determine which slider was updated.
  switch (pElem->nId) {
    case E_ELEM_SEEKBAR:
      // Fetch the slider position
      nVal = gslc_ElemXSeekbarGetPos(pGui,m_pElemSeekbar);
      snprintf(acTxt,MAX_STR,"%u",nVal); 
      gslc_ElemSetTxtStr(pGui,m_pElemSeekbarTxt,acTxt);
      break;
    case E_ELEM_SEEKBAR2:
      // Fetch the slider position
      nVal = gslc_ElemXSeekbarGetPos(pGui,m_pElemSeekbar2);
      break;
    case E_ELEM_SEEKBAR3:
      // Fetch the slider position
      nVal = gslc_ElemXSeekbarGetPos(pGui,m_pElemSeekbar3);
      break;
    case E_ELEM_SEEKBAR4:
      // Fetch the slider position
      nVal = gslc_ElemXSeekbarGetPos(pGui,m_pElemSeekbar4);
      break;

    default:
      break;
  }

  return true;
}

// ------------------------------------------------
// Create page elements
// ------------------------------------------------
bool InitGUI()
{
  gslc_tsElemRef* pElemRef = NULL;

  gslc_PageAdd(&m_gui,E_PG_MAIN,m_asPage1Elem,MAX_ELEM_PG_MAIN_RAM,m_asPage1ElemRef,MAX_ELEM_PG_MAIN);

  // NOTE: The current page defaults to the first page added. Here we explicitly
  //       ensure that the main page is the correct page no matter the add order.
  gslc_SetPageCur(&m_gui,E_PG_MAIN);
  
  // Set Background to a flat color
  gslc_SetBkgndColor(&m_gui,GSLC_COL_GRAY);

  // -----------------------------------
  // PAGE: E_PG_MAIN
  
   
  // Create E_ELEM_BOX1 box
  pElemRef = gslc_ElemCreateBox(&m_gui,E_ELEM_BOX1,E_PG_MAIN,(gslc_tsRect){10,50,300,150});
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_WHITE,GSLC_COL_GRAY_LT3,GSLC_COL_GRAY_LT3);
  
  // Create E_LBL_SEEKBAR text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_LBL_SEEKBAR,E_PG_MAIN,(gslc_tsRect){0,0,320,30},
    (char*)"Seekbar Demo",0,E_BUILTIN_TXT10);
  gslc_ElemSetTxtAlign(&m_gui,pElemRef,GSLC_ALIGN_MID_MID);
  gslc_ElemSetFrameEn(&m_gui,pElemRef,true);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_WHITE);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_GRAY,GSLC_COL_BLUE_DK4,GSLC_COL_BLUE_DK4);
  
  // Create E_ELEM_TXT_SEEKBAR runtime modifiable text
  static char m_sDisplayText2[5] = "20";
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TXT_SEEKBAR,E_PG_MAIN,(gslc_tsRect){240,80,48,18},
    (char*)m_sDisplayText2,5,E_BUILTIN_TXT10);
  gslc_ElemSetTxtAlign(&m_gui,pElemRef,GSLC_ALIGN_MID_RIGHT);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_BLACK);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_GRAY,GSLC_COL_WHITE,GSLC_COL_WHITE);
  m_pElemSeekbarTxt = pElemRef;

  // Create slider E_ELEM_SEEKBAR with thumb trim, narrow progress bar and tick marks
  pElemRef = gslc_ElemXSeekbarCreate(&m_gui,E_ELEM_SEEKBAR,E_PG_MAIN,&m_sXSeekbar1,
          (gslc_tsRect){100,80,120,20},0,100,20,
          2,2,8,GSLC_COL_BLUE_DK3,GSLC_COL_BLUE_LT3,GSLC_COL_BLUE_DK4,false);
  gslc_ElemXSeekbarSetStyle(&m_gui,pElemRef,true,GSLC_COL_BLUE_LT3,
          false,GSLC_COL_BLACK,10,6,GSLC_COL_BLACK);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_GRAY_LT3,GSLC_COL_GRAY_LT3,GSLC_COL_GRAY_LT3);
  gslc_ElemXSeekbarSetPosFunc(&m_gui,pElemRef,&CbSlidePos);
  m_pElemSeekbar = pElemRef;

  // Create slider E_ELEM_SEEKBAR2 with solid color thumb, wider progress bar and vertical position
  pElemRef = gslc_ElemXSeekbarCreate(&m_gui,E_ELEM_SEEKBAR2,E_PG_MAIN,&m_sXSeekbar2,
          (gslc_tsRect){50,70,20,120},0,100,20,
          5,2,8,GSLC_COL_GREEN_DK3,GSLC_COL_GREEN_LT2,GSLC_COL_GREEN_DK4,true);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_GRAY_LT3,GSLC_COL_GRAY_LT3,GSLC_COL_GRAY_LT3);
  gslc_ElemXSeekbarSetPosFunc(&m_gui,pElemRef,&CbSlidePos);
  m_pElemSeekbar2 = pElemRef;

  // Create slider E_ELEM_SEEKBAR3 with thumb trim and wider progress bar
  pElemRef = gslc_ElemXSeekbarCreate(&m_gui,E_ELEM_SEEKBAR3,E_PG_MAIN,&m_sXSeekbar3,
          (gslc_tsRect){100,120,120,20},0,100,40,
          5,2,8,GSLC_COL_RED_DK3,GSLC_COL_RED_LT2,GSLC_COL_RED_DK4,false);
  gslc_ElemXSeekbarSetStyle(&m_gui,pElemRef,true,GSLC_COL_RED_LT3,
          false,GSLC_COL_BLACK,0,0,GSLC_COL_BLACK);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_GRAY_LT3,GSLC_COL_GRAY_LT3,GSLC_COL_GRAY_LT3);
  gslc_ElemXSeekbarSetPosFunc(&m_gui,pElemRef,&CbSlidePos);
  m_pElemSeekbar3 = pElemRef;

  // Create slider E_ELEM_SEEKBAR4 with thumb trim and frame and wider progress bar
  pElemRef = gslc_ElemXSeekbarCreate(&m_gui,E_ELEM_SEEKBAR4,E_PG_MAIN,&m_sXSeekbar4,
          (gslc_tsRect){100,160,120,20},0,100,20,
          5,2,8,GSLC_COL_BLUE_DK3,GSLC_COL_BLUE_LT3,GSLC_COL_BLUE_DK4,false);
  gslc_ElemXSeekbarSetStyle(&m_gui,pElemRef,true,GSLC_COL_WHITE,
          true,GSLC_COL_BLACK,0,0,GSLC_COL_BLACK);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_BLACK,GSLC_COL_GRAY_LT3,GSLC_COL_GRAY_LT3);
  gslc_ElemXSeekbarSetPosFunc(&m_gui,pElemRef,&CbSlidePos);
  m_pElemSeekbar4 = pElemRef;

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
    if (!gslc_FontSet(&m_gui,E_BUILTIN_TXT10,GSLC_FONTREF_PTR,NULL,2)) { return; }

  // ------------------------------------------------
  // Create graphic elements
  // ------------------------------------------------
  InitGUI();


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
