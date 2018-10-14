//
// GUIslice Library Examples
// - Calvin Hass
// - https://www.impulseadventure.com/elec/guislice-gui.html
// - https://github.com/ImpulseAdventure/GUIslice
// - Example 06 (Arduino):
//   - Example of clipping plus draw and tick callback function
//   - NOTE: This is the simple version of the example without
//     optimizing for memory consumption. Therefore, it may not
//     run on Arduino devices with limited memory. A "minimal"
//     version is located in the "arduino_min" folder which includes
//     FLASH memory optimization for reduced memory devices.
//
// ARDUINO NOTES:
// - GUIslice_config.h must be edited to match the pinout connections
//   between the Arduino CPU and the display controller (see ADAGFX_PIN_*).
//   - IMPORTANT: This sketch can demonstrate use of a logo located on an external SD card
//     accessed via a SPI interface. To enable, the GUIslice_config.h needs to set
//     #define GSLC_SD_EN 1
// - This sketch is expected to exhibit some flickering on unbuffered displays
//

#include "GUIslice.h"
#include "GUIslice_ex.h"
#include "GUIslice_drv.h"

#include <math.h>


// Defines for resources
#define IMG_LOGO        "logo1_24.bmp"

// Enumerations for pages, elements, fonts, images
enum {E_PG_MAIN};
enum {E_ELEM_BTN_QUIT,E_ELEM_TXT_COUNT,E_ELEM_PROGRESS,
      E_ELEM_DATAX,E_ELEM_DATAY,E_ELEM_DATAZ,E_ELEM_SCAN,
      E_ELEM_CHECK1};
enum {E_FONT_BTN,E_FONT_TXT};

bool      m_bQuit = false;

// Free-running counter for display
unsigned  m_nCount = 0;

float     m_fCoordX = 0;
float     m_fCoordY = 0;
float     m_fCoordZ = 0;

// Instantiate the GUI
#define MAX_PAGE                1
#define MAX_FONT                2

// Define the maximum number of elements per page
#define MAX_ELEM_PG_MAIN          21                // # Elems total
#define MAX_ELEM_PG_MAIN_RAM      MAX_ELEM_PG_MAIN  // # Elems in RAM

gslc_tsGui                  m_gui;
gslc_tsDriver               m_drv;
gslc_tsFont                 m_asFont[MAX_FONT];
gslc_tsPage                 m_asPage[MAX_PAGE];
gslc_tsElem                 m_asPageElem[MAX_ELEM_PG_MAIN_RAM];   // Storage for all elements in RAM
gslc_tsElemRef              m_asPageElemRef[MAX_ELEM_PG_MAIN];    // References for all elements in GUI

gslc_tsXGauge               m_sXGauge;
gslc_tsXCheckbox            m_asXCheck[1];


#define MAX_STR             10

// Define an origin for the scanner
int16_t   m_nOriginX = 0;
int16_t   m_nOriginY = 0;

  // Save some element pointers for quick access
  gslc_tsElemRef*  m_pElemCount      = NULL;
  gslc_tsElemRef*  m_pElemDataX      = NULL;
  gslc_tsElemRef*  m_pElemDataY      = NULL;
  gslc_tsElemRef*  m_pElemDataZ      = NULL;
  gslc_tsElemRef*  m_pElemProgress   = NULL;

// Define debug message function
static int16_t DebugOut(char ch) { Serial.write(ch); return 0; }


// Scanner drawing callback function
// - This is called when E_ELEM_SCAN is being rendered
// - The scanner implements a custom element that replaces
//   the Box element type with a custom rendering function.
// - Clipping region is updated temporarily during this draw
// - NOTE: For Arduino displays, we don't have enough RAM to
//   support double-buffering, so some flicker will be inevitable
//   if we allow a faster frame update rate.
bool CbDrawScanner(void* pvGui,void* pvElemRef,gslc_teRedrawType eRedraw)
{
  int nInd;

  // Typecast the parameters to match the GUI and element types
  gslc_tsGui*     pGui      = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef  = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem*    pElem     = pElemRef->pElem;

  // Create shorthand variables for the origin
  int16_t  nX = pElem->rElem.x + m_nOriginX;
  int16_t  nY = pElem->rElem.y + m_nOriginY;

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

// Demonstrate tick callback for scanner window
// - This is called whenever gslc_Update() is called
// - In this example, it simply updates the relative
//   origin of the view which will shift the display
bool CbTickScanner(void* pvGui,void* pvScope)
{

  //gslc_tsGui*   pGui      = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef  = (gslc_tsElemRef*)(pvScope);
  //gslc_tsElem*    pElem     = pElemRef->pElem;

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

// Button callbacks
bool CbBtnQuit(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY)
{
  if (eTouch == GSLC_TOUCH_UP_IN) {
    m_bQuit = true;
  }
  return true;
}

// Create the default elements on each page
bool InitOverlays()
{
  gslc_tsElemRef* pElemRef = NULL;

  gslc_PageAdd(&m_gui,E_PG_MAIN,m_asPageElem,MAX_ELEM_PG_MAIN_RAM,m_asPageElemRef,MAX_ELEM_PG_MAIN);

  // Background flat color
  gslc_SetBkgndColor(&m_gui,GSLC_COL_GRAY_DK3);

  // Graphic logo from SD card
  #if 0
  static const char m_strLogo[] = IMG_LOGO;
  pElemRef = gslc_ElemCreateImg(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){160-100,5,200,40},
    gslc_GetImageFromSD(m_strLogo,GSLC_IMGREF_FMT_BMP24));
  #endif


  // Create background box
  pElemRef = gslc_ElemCreateBox(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){10,50,300,150});
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_WHITE,GSLC_COL_BLACK,GSLC_COL_BLACK);

  // Create Quit button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_QUIT,E_PG_MAIN,
    (gslc_tsRect){40,210,50,20},(char*)"QUIT",0,E_FONT_BTN,&CbBtnQuit);

  // Create counter
  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){20,60,50,10},
    (char*)"Searches:",0,E_FONT_TXT);
  static char mstr1[8] = "";
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TXT_COUNT,E_PG_MAIN,(gslc_tsRect){80,60,50,10},
    mstr1,sizeof(mstr1),E_FONT_TXT);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_GRAY_LT2);
  m_pElemCount = pElemRef; // Save for quick access

  // Create progress bar
  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){20,80,50,10},
    (char*)"Progress:",0,E_FONT_TXT);
  pElemRef = gslc_ElemXGaugeCreate(&m_gui,E_ELEM_PROGRESS,E_PG_MAIN,&m_sXGauge,(gslc_tsRect){80,80,50,10},
    0,100,0,GSLC_COL_GREEN,false);
  m_pElemProgress = pElemRef; // Save for quick access


  // Create other labels

  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){40,100,50,10},
    (char*)"Coord X:",0,E_FONT_TXT);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_WHITE);
  static char mstr2[8] = "";
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_DATAX,E_PG_MAIN,(gslc_tsRect){100,100,50,10},
    mstr2,sizeof(mstr2),E_FONT_TXT);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_GRAY_LT2);
  m_pElemDataX = pElemRef; // Save for quick access

  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){40,120,50,10},
    (char*)"Coord Y:",0,E_FONT_TXT);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_WHITE);
  static char mstr3[8] = "";
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_DATAY,E_PG_MAIN,(gslc_tsRect){100,120,50,10},
    mstr3,sizeof(mstr3),E_FONT_TXT);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_GRAY_LT2);
  m_pElemDataY = pElemRef; // Save for quick access

  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){40,140,50,10},
    (char*)"Coord Z:",0,E_FONT_TXT);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_WHITE);
  static char mstr4[8] = "";
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_DATAZ,E_PG_MAIN,(gslc_tsRect){100,140,50,10},
    mstr4,sizeof(mstr4),E_FONT_TXT);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_GRAY_LT2);
  m_pElemDataZ = pElemRef; // Save for quick access

  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){20,170,50,10},
    (char*)"Control:",0,E_FONT_TXT);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_ORANGE);

  pElemRef = gslc_ElemXCheckboxCreate(&m_gui,E_ELEM_CHECK1,E_PG_MAIN,&m_asXCheck[0],
    (gslc_tsRect){80,170,20,20},false,GSLCX_CHECKBOX_STYLE_X,GSLC_COL_BLUE_LT2,false);

  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){110,170,50,10},
    (char*)"Enable",0,E_FONT_TXT);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_GRAY_LT1);

  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){120,210,170,20},
    (char*)"Example of GUIslice C library",0,E_FONT_BTN);
  gslc_ElemSetTxtAlign(&m_gui,pElemRef,GSLC_ALIGN_MID_LEFT);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_RED_LT2);
  gslc_ElemSetFillEn(&m_gui,pElemRef,false);

  // --------------------------------------------------------------------------
  // Create scanner
  pElemRef = gslc_ElemCreateBox(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){190-1-2,75-1-12,100+2+4,100+2+10+4});
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_BLUE_LT2,GSLC_COL_BLACK,GSLC_COL_BLACK);

  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){190,75-11,100,10},
    (char*)"SCANNER",0,E_FONT_TXT);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_BLUE_DK2);
  gslc_ElemSetTxtAlign(&m_gui,pElemRef,GSLC_ALIGN_MID_MID);

  // We create a basic box and then provide a custom draw callback function for it
  pElemRef = gslc_ElemCreateBox(&m_gui,E_ELEM_SCAN,E_PG_MAIN,(gslc_tsRect){190-1,75-1,100+2,100+2});
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_BLUE_LT2,GSLC_COL_BLACK,GSLC_COL_BLACK);

  // Set the callback function to handle all drawing for the element
  gslc_ElemSetDrawFunc(&m_gui,pElemRef,&CbDrawScanner);
  // Set the callback function to update content automatically
  gslc_ElemSetTickFunc(&m_gui,pElemRef,&CbTickScanner);

  // --------------------------------------------------------------------------

  return true;
}

void setup()
{
  bool bOk = true;

  // Initialize debug output
  Serial.begin(9600);
  gslc_InitDebug(&DebugOut);
  //delay(1000);  // NOTE: Some devices require a delay after Serial.begin() before serial port can be used

  // -----------------------------------
  // Initialize
  if (!gslc_Init(&m_gui,&m_drv,m_asPage,MAX_PAGE,m_asFont,MAX_FONT)) { return; }

  // Load Fonts
  bOk = gslc_FontAdd(&m_gui,E_FONT_BTN,GSLC_FONTREF_PTR,NULL,1);
  if (!bOk) { return; }
  bOk = gslc_FontAdd(&m_gui,E_FONT_TXT,GSLC_FONTREF_PTR,NULL,1);
  if (!bOk) { return; }


  // -----------------------------------
  // Create the graphic elements
  InitOverlays();

  // Start up display on main page
  gslc_SetPageCur(&m_gui,E_PG_MAIN);

  m_bQuit = false;
}

void loop()
{
  char              acTxt[MAX_STR];

  // Update the data display values
  m_nCount++;

  // Perform any immediate updates on active page
  snprintf(acTxt,MAX_STR,"%u",m_nCount);
  gslc_ElemSetTxtStr(&m_gui,m_pElemCount,acTxt);

  // By default, Arduino sprintf() doesn't include floating point
  // support, so we're just going to display integers
  snprintf(acTxt,MAX_STR,"%4d",(int16_t)(m_fCoordX-50));
  gslc_ElemSetTxtStr(&m_gui,m_pElemDataX,acTxt);
  snprintf(acTxt,MAX_STR,"%4d",(int16_t)(m_fCoordY-50));
  gslc_ElemSetTxtStr(&m_gui,m_pElemDataY,acTxt);
  snprintf(acTxt,MAX_STR,"%4d",(int16_t)(m_fCoordZ));
  gslc_ElemSetTxtStr(&m_gui,m_pElemDataZ,acTxt);
  gslc_ElemSetTxtCol(&m_gui,m_pElemDataZ,(m_fCoordY>50)?GSLC_COL_GREEN_LT2:GSLC_COL_RED_DK2);

  gslc_ElemXGaugeUpdate(&m_gui,m_pElemProgress,50+50*sin(m_nCount/5.0));

  // -----------------------------------------------

  // Periodically call GUIslice update function
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


