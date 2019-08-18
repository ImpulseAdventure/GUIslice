//<File !Start!>
// FILE: [ex06_bld_callback.ino]
// Created by GUIslice Builder version: [0.13.0]
//
// GUIslice Builder Generated File
//
// For the latest guides, updates and support view:
// https://github.com/ImpulseAdventure/GUIslice
//
//<File !End!>
//
// - Example 06 (Arduino):
//   - Example of clipping plus draw and tick callback function
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

// Ensure optional features are enabled in the configuration
#if !(GSLC_SD_EN)
  #error "Config: GSLC_SD_EN required for this program but not enabled. Please update GUIslice/config/"
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
#define IMG_LOGO1_24              "/logo1_24.bmp"
//<Resources !End!>

// ------------------------------------------------
// Enumerations for pages, elements, fonts, images
// ------------------------------------------------
//<Enum !Start!>
enum {E_PG_MAIN};
enum {E_ELEM_BOX1,E_ELEM_BOX2,E_ELEM_BTN_QUIT,E_ELEM_CHECK1
      ,E_ELEM_DATAX,E_ELEM_DATAY,E_ELEM_DATAZ,E_ELEM_LOGO
      ,E_ELEM_PROGRESS1,E_ELEM_TXT_COUNT,E_LBL_CONTROL,E_LBL_COORDX
      ,E_LBL_COORDY,E_LBL_COORDZ,E_LBL_ENABLE,E_LBL_EXAMPLE
      ,E_LBL_PROGRESS,E_LBL_SCANNER,E_LBL_SEARCHES,E_SCAN3};
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
#define MAX_PAGE                1

#define MAX_ELEM_PG_MAIN 20                                         // # Elems total on page
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
gslc_tsXProgress                m_sXBarGauge1;
gslc_tsXCheckbox                m_asXCheck1;

#define MAX_STR                 100

//<GUI_Extra_Elements !End!>

// ------------------------------------------------
// Program Globals
// ------------------------------------------------
bool      m_bQuit = false;

// Free-running counter for display
unsigned  m_nCount = 0;

// Define an origin for the scanner
int16_t   m_nOriginX = 0;
int16_t   m_nOriginY = 0;

float     m_fCoordX = 0;
float     m_fCoordY = 0;
float     m_fCoordZ = 0;

// Save some element references for direct access
//<Save_References !Start!>
gslc_tsElemRef*  m_pElemCount      = NULL;
gslc_tsElemRef*  m_pElemDataX      = NULL;
gslc_tsElemRef*  m_pElemDataY      = NULL;
gslc_tsElemRef*  m_pElemDataZ      = NULL;
gslc_tsElemRef*  m_pElemProgress1  = NULL;
gslc_tsElemRef*  m_pElemQuit       = NULL;
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
 
// Scanner drawing callback function
// - This is called when E_ELEM_SCAN is being rendered
bool CbDrawScanner(void* pvGui,void* pvElemRef,gslc_teRedrawType eRedraw)
{
  int nInd;

  // Typecast the parameters to match the GUI and element types
  gslc_tsGui*     pGui      = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef  = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem*    pElem     = pElemRef->pElem;

  // Create shorthand variables for the origin
  int16_t  nX = pElem->rElem.x;
  int16_t  nY = pElem->rElem.y;

  // Draw the background
  gslc_tsRect rInside = pElem->rElem;
  rInside = gslc_ExpandRect(rInside,-1,-1);
  gslc_DrawFillRect(pGui,rInside,pElem->colElemFill);

  // Enable localized clipping
  gslc_SetClipRect(pGui,&rInside);

  // Perform the drawing of example graphic primitives
  gslc_DrawLine(pGui,nX,nY-200,nX,nY+200,GSLC_COL_GRAY_DK2);
  gslc_DrawLine(pGui,nX-200,nY,nX+200,nY,GSLC_COL_GRAY_DK2);

  gslc_DrawFrameRect(pGui,(gslc_tsRect){(int16_t)(nX-30),(int16_t)(nY-20),60,40},GSLC_COL_BLUE_DK2);
  for (nInd=-5;nInd<=5;nInd++) {
    gslc_DrawLine(pGui,nX,nY,nX+nInd*20,nY+100,GSLC_COL_PURPLE);
  }

  gslc_DrawFillRect(pGui,(gslc_tsRect){(int16_t)(nX+1),(int16_t)(nY+1),10,10},GSLC_COL_RED_DK2);
  gslc_DrawFillRect(pGui,(gslc_tsRect){(int16_t)(nX+1),(int16_t)(nY-10),10,10},GSLC_COL_GREEN_DK2);
  gslc_DrawFillRect(pGui,(gslc_tsRect){(int16_t)(nX-10),(int16_t)(nY+1),10,10},GSLC_COL_BLUE_DK2);
  gslc_DrawFillRect(pGui,(gslc_tsRect){(int16_t)(nX-10),(int16_t)(nY-10),10,10},GSLC_COL_YELLOW);

  // Disable clipping region
  gslc_SetClipRect(pGui,NULL);

  // Draw the frame
  gslc_DrawFrameRect(pGui,pElem->rElem,pElem->colElemFrame);

  // Clear the redraw flag
  gslc_ElemSetRedraw(&m_gui,pElemRef,GSLC_REDRAW_NONE);

  return true;
}
//<Slider Callback !Start!>
//<Slider Callback !End!>

bool CbTickScanner(void* pvGui,void* pvScope)
{
  gslc_tsGui*   pGui      = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef  = (gslc_tsElemRef*)(pvScope);
  gslc_tsElem*    pElem     = pElemRef->pElem;

  m_fCoordX = 50+25.0*(sin(m_nCount/2.5));
  m_fCoordY = 50+15.0*(cos(m_nCount/1.75));
  m_fCoordZ = 13.02;

  // Adjust the scanner's origin for fun
  m_nOriginX = (int16_t)m_fCoordX;
  m_nOriginY = (int16_t)m_fCoordY;

  // Manually mark the scanner element as needing redraw
  // since we have shifted its relative coordinates (via origin)
  gslc_ElemSetRedraw(&m_gui,pElemRef,GSLC_REDRAW_FULL);

  return true;
}

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
  gslc_SetBkgndColor(&m_gui,GSLC_COL_BLACK);

  // -----------------------------------
  // PAGE: E_PG_MAIN
  
 
  // Create E_ELEM_LOGO using Image 
  pElemRef = gslc_ElemCreateImg(&m_gui,E_ELEM_LOGO,E_PG_MAIN,(gslc_tsRect){60,5,200,40},
    gslc_GetImageFromSD((const char*)IMG_LOGO1_24,GSLC_IMGREF_FMT_BMP24));
  gslc_ElemSetFillEn(&m_gui,pElemRef,true);
   
  // Create E_ELEM_BOX1 box
  pElemRef = gslc_ElemCreateBox(&m_gui,E_ELEM_BOX1,E_PG_MAIN,(gslc_tsRect){10,50,300,150});
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_WHITE,GSLC_COL_BLACK,GSLC_COL_BLACK);
  
  // Create E_ELEM_BTN_QUIT button with modifiable text label
  static char m_strbtn1[7] = "QUIT";
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_QUIT,E_PG_MAIN,
    (gslc_tsRect){40,210,50,20},
    (char*)m_strbtn1,7,E_FONT_TXT5,&CbBtnCommon);
  m_pElemQuit = pElemRef;
  
  // Create E_LBL_SEARCHES text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_LBL_SEARCHES,E_PG_MAIN,(gslc_tsRect){20,60,56,12},
    (char*)"Searches:",0,E_FONT_TXT5);
  
  // Create E_ELEM_TXT_COUNT runtime modifiable text
  static char m_sDisplayText2[8] = "";
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TXT_COUNT,E_PG_MAIN,(gslc_tsRect){80,60,44,12},
    (char*)m_sDisplayText2,8,E_FONT_TXT5);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_GRAY_LT2);
  m_pElemCount = pElemRef;
  
  // Create E_LBL_PROGRESS text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_LBL_PROGRESS,E_PG_MAIN,(gslc_tsRect){20,80,56,12},
    (char*)"Progress:",0,E_FONT_TXT5);

  // Create progress bar E_ELEM_PROGRESS1 
  pElemRef = gslc_ElemXProgressCreate(&m_gui,E_ELEM_PROGRESS1,E_PG_MAIN,&m_sXBarGauge1,
    (gslc_tsRect){80,80,50,12},0,100,0,GSLC_COL_GREEN,false);
  m_pElemProgress1 = pElemRef;
  
  // Create E_LBL_COORDX text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_LBL_COORDX,E_PG_MAIN,(gslc_tsRect){40,100,50,12},
    (char*)"Coord X:",0,E_FONT_TXT5);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_WHITE);
  
  // Create E_ELEM_DATAX runtime modifiable text
  static char m_sDisplayText5[8] = "";
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_DATAX,E_PG_MAIN,(gslc_tsRect){100,100,44,12},
    (char*)m_sDisplayText5,8,E_FONT_TXT5);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_GRAY_LT2);
  m_pElemDataX = pElemRef;
  
  // Create E_LBL_COORDY text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_LBL_COORDY,E_PG_MAIN,(gslc_tsRect){40,120,50,12},
    (char*)"Coord Y:",0,E_FONT_TXT5);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_WHITE);
  
  // Create E_ELEM_DATAY runtime modifiable text
  static char m_sDisplayText7[8] = "";
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_DATAY,E_PG_MAIN,(gslc_tsRect){100,120,44,12},
    (char*)m_sDisplayText7,8,E_FONT_TXT5);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_GRAY_LT2);
  m_pElemDataY = pElemRef;
  
  // Create E_LBL_COORDZ text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_LBL_COORDZ,E_PG_MAIN,(gslc_tsRect){40,140,50,12},
    (char*)"Coord Z:",0,E_FONT_TXT5);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_WHITE);
  
  // Create E_ELEM_DATAZ runtime modifiable text
  static char m_sDisplayText9[8] = "";
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_DATAZ,E_PG_MAIN,(gslc_tsRect){100,140,44,12},
    (char*)m_sDisplayText9,8,E_FONT_TXT5);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_GRAY_LT2);
  m_pElemDataZ = pElemRef;
  
  // Create E_LBL_CONTROL text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_LBL_CONTROL,E_PG_MAIN,(gslc_tsRect){20,170,50,12},
    (char*)"Control:",0,E_FONT_TXT5);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_ORANGE);
   
  // create checkbox E_ELEM_CHECK1
  pElemRef = gslc_ElemXCheckboxCreate(&m_gui,E_ELEM_CHECK1,E_PG_MAIN,&m_asXCheck1,
    (gslc_tsRect){80,170,20,20},false,GSLCX_CHECKBOX_STYLE_X,GSLC_COL_BLUE_LT2,false);
  
  // Create E_LBL_ENABLE text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_LBL_ENABLE,E_PG_MAIN,(gslc_tsRect){110,170,38,12},
    (char*)"Enable",0,E_FONT_TXT5);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_GRAY_LT1);
  
  // Create E_LBL_EXAMPLE text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_LBL_EXAMPLE,E_PG_MAIN,(gslc_tsRect){110,210,176,12},
    (char*)"Example of GUIslice C library",0,E_FONT_TXT5);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_RED_LT2);
   
  // Create E_ELEM_BOX2 box
  pElemRef = gslc_ElemCreateBox(&m_gui,E_ELEM_BOX2,E_PG_MAIN,(gslc_tsRect){188,59,104,118});
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_BLUE_LT2,GSLC_COL_BLACK,GSLC_COL_BLACK);
  
  // Create E_LBL_SCANNER text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_LBL_SCANNER,E_PG_MAIN,(gslc_tsRect){218,60,44,12},
    (char*)"SCANNER",0,E_FONT_TXT5);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_BLUE_LT2);
   
  // Create E_SCAN3 box
  pElemRef = gslc_ElemCreateBox(&m_gui,E_SCAN3,E_PG_MAIN,(gslc_tsRect){189,74,102,102});
  // Set the callback function to handle all drawing for the element
  gslc_ElemSetDrawFunc(&m_gui,pElemRef,&CbDrawScanner);
  // Set the callback function to update content automatically
  gslc_ElemSetTickFunc(&m_gui,pElemRef,&CbTickScanner);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_BLUE_LT2,GSLC_COL_BLACK,GSLC_COL_BLACK);
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

  // Update the data display values
  m_nCount++;

  // ------------------------------------------------
  // Update GUI Elements
  // ------------------------------------------------
  
  // By default, Arduino sprintf() doesn't include floating point
  // support, so we're just going to display integers
  snprintf(acTxt,MAX_STR,"%4d",(int16_t)(m_fCoordX-50));
  gslc_ElemSetTxtStr(&m_gui,m_pElemDataX,acTxt);
  snprintf(acTxt,MAX_STR,"%4d",(int16_t)(m_fCoordY-50));
  gslc_ElemSetTxtStr(&m_gui,m_pElemDataY,acTxt);
  snprintf(acTxt,MAX_STR,"%4d",(int16_t)(m_fCoordZ));
  gslc_ElemSetTxtStr(&m_gui,m_pElemDataZ,acTxt);
  gslc_ElemSetTxtCol(&m_gui,m_pElemDataZ,(m_fCoordY>50)?GSLC_COL_GREEN_LT2:GSLC_COL_RED_DK2);

  gslc_ElemXProgressSetVal(&m_gui,m_pElemProgress1,50+50*sin(m_nCount/5.0));

  // ------------------------------------------------
  // Periodically call GUIslice update function
  // ------------------------------------------------
  gslc_Update(&m_gui);
    
  // Slow down display
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
