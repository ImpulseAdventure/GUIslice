//
// GUIslice Library Examples
// - Calvin Hass
// - https://www.impulseadventure.com/elec/guislice-gui.html
// - https://github.com/ImpulseAdventure/GUIslice
// - Example 06 (Arduino):
//   - Example of clipping plus draw and tick callback function
//   - Demonstrates the use of ElemCreate*_P() functions
//     These RAM-reduced examples take advantage of the internal
//     Flash storage (via PROGMEM).
//   - NOTE: This sketch requires moderate program storage in Flash.
//     As a result, it may not run on basic Arduino devices (eg. ATmega328)
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
// - To enable the same code to run on devices that support storing
//   data into Flash (PROGMEM) and those that don't, we can make the
//   number of elements in Flash dependent upon GSLC_USE_PROGMEM
// - This should allow both Arduino and ARM Cortex to use the same code
#define MAX_ELEM_PG_MAIN          19                                        // # Elems total
#if (GSLC_USE_PROGMEM)
  #define MAX_ELEM_PG_MAIN_PROG   19                                        // # Elems in Flash
#else
  #define MAX_ELEM_PG_MAIN_PROG   0                                         // # Elems in Flash
#endif
#define MAX_ELEM_PG_MAIN_RAM      MAX_ELEM_PG_MAIN - MAX_ELEM_PG_MAIN_PROG  // # Elems in RAM

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
  gslc_tsElem*    pElem     = gslc_GetElemFromRef(pGui,pElemRef);

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
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_NONE);

  return true;
}

// Demonstrate tick callback for scanner window
// - This is called whenever gslc_Update() is called
// - In this example, it simply updates the relative
//   origin of the view which will shift the display
bool CbTickScanner(void* pvGui,void* pvScope)
{
  gslc_tsGui*     pGui      = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef  = (gslc_tsElemRef*)(pvScope);
  //gslc_tsElem*    pElem     = gslc_GetElemFromRef(pGui,pElemRef);

  m_fCoordX = 50+25.0*(sin(m_nCount/2.5));
  m_fCoordY = 50+15.0*(cos(m_nCount/1.75));
  m_fCoordZ = 13.02;

  // Adjust the scanner's origin for fun
  m_nOriginX = (int16_t)m_fCoordX;
  m_nOriginY = (int16_t)m_fCoordY;

  // Manually mark the scanner element as needing redraw
  // since we have shifted its relative coordinates (via origin)
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_FULL);

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
  gslc_tsElemRef*  pElemRef = NULL;

  gslc_PageAdd(&m_gui,E_PG_MAIN,m_asPageElem,MAX_ELEM_PG_MAIN_RAM,m_asPageElemRef,MAX_ELEM_PG_MAIN);

  // Background flat color
  gslc_SetBkgndColor(&m_gui,GSLC_COL_GRAY_DK3);

  // Graphic logo from SD card
  // - If enabling this, ensure MAX_ELEM_PG_MAIN is incremented
  #if 0
  static const char m_strLogo[] = IMG_LOGO;
  pElemRef = gslc_ElemCreateImg(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){160-100,5,200,40},
    gslc_GetImageFromSD(m_strLogo,GSLC_IMGREF_FMT_BMP24));
  #endif


  // Create background box
  gslc_ElemCreateBox_P(&m_gui,100,E_PG_MAIN,10,50,300,150,GSLC_COL_WHITE,GSLC_COL_BLACK,true,true,NULL,NULL);

  // Create Quit button with text label
  gslc_ElemCreateBtnTxt_P(&m_gui,E_ELEM_BTN_QUIT,E_PG_MAIN,40,210,50,20,"Quit",&m_asFont[0],
    GSLC_COL_WHITE,GSLC_COL_BLUE_DK2,GSLC_COL_BLUE_DK4,GSLC_COL_BLUE_DK2,GSLC_COL_BLUE_DK1,GSLC_ALIGN_MID_MID,true,true,&CbBtnQuit,NULL);


  // Create counter
  // - Static label
  gslc_ElemCreateTxt_P(&m_gui,101,E_PG_MAIN,20,60,50,10,"Searches:",&m_asFont[1],
          GSLC_COL_YELLOW,GSLC_COL_BLACK,GSLC_COL_BLACK,GSLC_ALIGN_MID_LEFT,false,true);
  // - Read-write value
  static char mstr3[8] = "";  // Placeholder for counter
  gslc_ElemCreateTxt_P_R(&m_gui,E_ELEM_TXT_COUNT,E_PG_MAIN,80,60,50,10,mstr3,8,&m_asFont[1], // E_FONT_TXT
          GSLC_COL_GRAY_LT2,GSLC_COL_BLACK,GSLC_COL_BLACK,GSLC_ALIGN_MID_LEFT,false,true);
  m_pElemCount = gslc_PageFindElemById(&m_gui,E_PG_MAIN,E_ELEM_TXT_COUNT);

  // Create progress bar
  gslc_ElemCreateTxt_P(&m_gui,102,E_PG_MAIN,20,80,50,10,"Progress:",&m_asFont[1],
          GSLC_COL_YELLOW,GSLC_COL_BLACK,GSLC_COL_BLACK,GSLC_ALIGN_MID_LEFT,false,true);
  gslc_ElemXGaugeCreate_P(&m_gui,E_ELEM_PROGRESS,E_PG_MAIN,80,80,50,10,
    0,100,0,GSLC_COL_GRAY,GSLC_COL_BLACK,GSLC_COL_GREEN,false);
  m_pElemProgress = gslc_PageFindElemById(&m_gui,E_PG_MAIN,E_ELEM_PROGRESS);


  // Create other labels

  // Static label
  gslc_ElemCreateTxt_P(&m_gui,103,E_PG_MAIN,40,100,50,10,"Coord X:",&m_asFont[1],
          GSLC_COL_WHITE,GSLC_COL_BLACK,GSLC_COL_BLACK,GSLC_ALIGN_MID_LEFT,false,true);
  // Read/write value
  static char mstr6[8] = "???";
  gslc_ElemCreateTxt_P_R(&m_gui,E_ELEM_DATAX,E_PG_MAIN,100,100,50,10,mstr6,8,&m_asFont[1], // E_FONT_TXT
          GSLC_COL_GRAY_LT2,GSLC_COL_BLACK,GSLC_COL_BLACK,GSLC_ALIGN_MID_LEFT,false,true);

  // Static label
  gslc_ElemCreateTxt_P(&m_gui,104,E_PG_MAIN,40,120,50,10,"Coord Y:",&m_asFont[1],
          GSLC_COL_WHITE,GSLC_COL_BLACK,GSLC_COL_BLACK,GSLC_ALIGN_MID_LEFT,false,true);
  // Read-write value
  static char mstr8[8] = "";
  gslc_ElemCreateTxt_P_R(&m_gui,E_ELEM_DATAY,E_PG_MAIN,100,120,50,10,mstr8,8,&m_asFont[1], // E_FONT_TXT
          GSLC_COL_GRAY_LT2,GSLC_COL_BLACK,GSLC_COL_BLACK,GSLC_ALIGN_MID_LEFT,false,true);

  // Static label
  gslc_ElemCreateTxt_P(&m_gui,105,E_PG_MAIN,40,140,50,10,"Coord Z:",&m_asFont[1],
          GSLC_COL_WHITE,GSLC_COL_BLACK,GSLC_COL_BLACK,GSLC_ALIGN_MID_LEFT,false,true);
  // Read-write value
  static char mstr10[8] = "";
  gslc_ElemCreateTxt_P_R(&m_gui,E_ELEM_DATAZ,E_PG_MAIN,100,140,50,10,mstr10,8,&m_asFont[1], // E_FONT_TXT
          GSLC_COL_GRAY_LT2,GSLC_COL_BLACK,GSLC_COL_BLACK,GSLC_ALIGN_MID_LEFT,false,true);

  gslc_ElemCreateTxt_P(&m_gui,106,E_PG_MAIN,20,170,50,10,"Control:",&m_asFont[1],
          GSLC_COL_ORANGE,GSLC_COL_BLACK,GSLC_COL_BLACK,GSLC_ALIGN_MID_LEFT,false,true);

  gslc_ElemXCheckboxCreate_P(&m_gui,E_ELEM_CHECK1,E_PG_MAIN,80,170,20,20,GSLC_COL_BLACK,true,
          GSLC_GROUP_ID_NONE,false,GSLCX_CHECKBOX_STYLE_X,GSLC_COL_BLUE_LT2,false);

  gslc_ElemCreateTxt_P(&m_gui,107,E_PG_MAIN,110,170,50,10,"Enable",&m_asFont[1],
          GSLC_COL_GRAY_LT1,GSLC_COL_BLACK,GSLC_COL_BLACK,GSLC_ALIGN_MID_LEFT,false,true);

  gslc_ElemCreateTxt_P(&m_gui,108,E_PG_MAIN,120,210,170,20,"Example of GUIslice C library",&m_asFont[1],
          GSLC_COL_RED_LT2,GSLC_COL_BLACK,GSLC_COL_BLACK,GSLC_ALIGN_MID_LEFT,false,false);

  // --------------------------------------------------------------------------
  // Create scanner
  gslc_ElemCreateBox_P(&m_gui,109,E_PG_MAIN,190-1-2,75-1-12,100+2+4,100+2+10+4,GSLC_COL_BLUE_LT2,GSLC_COL_BLACK,
          true,true,NULL,NULL);

  gslc_ElemCreateTxt_P(&m_gui,110,E_PG_MAIN,190,75-11,100,10,"SCANNER",&m_asFont[1],
          GSLC_COL_BLUE_DK2,GSLC_COL_BLACK,GSLC_COL_BLACK,GSLC_ALIGN_MID_MID,false,true);

  // We create a basic box and then provide a custom draw callback function for it
  gslc_ElemCreateBox_P(&m_gui,E_ELEM_SCAN,E_PG_MAIN,190-1,75-1,100+2,100+2,
    GSLC_COL_BLUE_LT2,GSLC_COL_BLACK,true,true,&CbDrawScanner,&CbTickScanner);

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
  bOk = gslc_FontAdd(&m_gui,E_FONT_BTN,GSLC_FONTREF_PTR,NULL,1); // m_asFont[0]
  if (!bOk) { return; }
  bOk = gslc_FontAdd(&m_gui,E_FONT_TXT,GSLC_FONTREF_PTR,NULL,1); // m_asFont[1]
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
  gslc_tsElemRef*   pElemRef = NULL;

  // Update the data display values
  m_nCount++;

  // Perform any immediate updates on active page
  snprintf(acTxt,MAX_STR,"%u",m_nCount);
  gslc_ElemSetTxtStr(&m_gui,m_pElemCount,acTxt);

  // By default, Arduino sprintf() doesn't include floating point
  // support, so we're just going to display integers
  snprintf(acTxt,MAX_STR,"%4d",(int16_t)(m_fCoordX-50));
  pElemRef = gslc_PageFindElemById(&m_gui,E_PG_MAIN,E_ELEM_DATAX);
  gslc_ElemSetTxtStr(&m_gui,pElemRef,acTxt);
  snprintf(acTxt,MAX_STR,"%4d",(int16_t)(m_fCoordY-50));
  pElemRef = gslc_PageFindElemById(&m_gui,E_PG_MAIN,E_ELEM_DATAY);
  gslc_ElemSetTxtStr(&m_gui,pElemRef,acTxt);
  snprintf(acTxt,MAX_STR,"%4d",(int16_t)(m_fCoordZ));
  pElemRef = gslc_PageFindElemById(&m_gui,E_PG_MAIN,E_ELEM_DATAZ);
  gslc_ElemSetTxtStr(&m_gui,pElemRef,acTxt);

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


