//
// GUIslice Library Examples
// - Calvin Hass
// - http://www.impulseadventure.com/elec/microsdl-sdl-gui.html
// - Example 06 (Arduino):
//     Example of clipping plus draw and tick callback function
//
// ARDUINO NOTES:
// - GUIslice_config.h must be edited to match the pinout connections
//   between the Arduino CPU and the display controller (see ADAGFX_PIN_*).
//   - IMPORTANT: This sketch can demonstrate use of a logo located on an external SD card
//     accessed via a SPI interface. To enable, the GUIslice_config.h needs to set
//     #define ADAGFX_SD_EN 1
// - To support a reasonable number of GUI elements, it is recommended to
//   use an Arduino CPU that provides more than 2KB of SRAM.
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
      E_ELEM_DATAX,E_ELEM_DATAY,E_ELEM_DATAZ,E_ELEM_SCAN};
enum {E_FONT_BTN,E_FONT_TXT};

bool      m_bQuit = false;

// Free-running counter for display
unsigned  m_nCount = 0;

float     m_fCoordX = 0;
float     m_fCoordY = 0;
float     m_fCoordZ = 0;

// Instantiate the GUI
#define MAX_FONT            5
gslc_tsGui                  m_gui;
gslc_tsDriver               m_drv;
gslc_tsFont                 m_asFont[MAX_FONT];
gslc_tsXGauge               m_sXGauge;

#define MAX_PAGE            1
#define MAX_ELEM_PG_MAIN    18
gslc_tsPage                 m_asPage[MAX_PAGE];
gslc_tsElem                 m_asPageElem[MAX_ELEM_PG_MAIN];

#define MAX_STR             10

// Define an origin for the scanner
int16_t   m_nOriginX = 0;
int16_t   m_nOriginY = 0;


// Scanner drawing callback function
// - This is called when E_ELEM_SCAN is being rendered
// - The scanner implements a custom element that replaces
//   the Box element type with a custom rendering function.
// - Clipping region is updated temporarily during this draw
// - NOTE: For Arduino displays, we don't have enough RAM to
//   support double-buffering, so some flicker will be inevitable
//   if we allow a faster frame update rate.
bool CbDrawScanner(void* pvGui,void* pvElem)
{
  int nInd;

  // Typecast the parameters to match the GUI and element types
  gslc_tsGui*   pGui  = (gslc_tsGui*)(pvGui);
  gslc_tsElem*  pElem = (gslc_tsElem*)(pvElem);
  
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

  gslc_DrawFrameRect(pGui,(gslc_tsRect){nX-30,nY-20,60,40},GSLC_COL_BLUE_DK2);
  for (nInd=-5;nInd<=5;nInd++) {
    gslc_DrawLine(pGui,nX,nY,nX+nInd*20,nY+100,GSLC_COL_PURPLE);
  }

  gslc_DrawFillRect(pGui,(gslc_tsRect){nX+1,nY+1,10,10},GSLC_COL_RED_DK2);
  gslc_DrawFillRect(pGui,(gslc_tsRect){nX+1,nY-10,10,10},GSLC_COL_GREEN_DK2);
  gslc_DrawFillRect(pGui,(gslc_tsRect){nX-10,nY+1,10,10},GSLC_COL_BLUE_DK2);
  gslc_DrawFillRect(pGui,(gslc_tsRect){nX-10,nY-10,10,10},GSLC_COL_YELLOW);

  // Disable clipping region
  gslc_SetClipRect(pGui,NULL);
  
  // Draw the frame
  gslc_DrawFrameRect(pGui,pElem->rElem,pElem->colElemFrame);
  
  // Clear the redraw flag
  gslc_ElemSetRedraw(pElem,false);
  
  return true;
}

// Demonstrate tick callback for scanner window
// - This is called whenever gslc_Update() is called
// - In this example, it simply updates the relative
//   origin of the view which will shift the display
bool CbTickScanner(void* pvGui,void* pvScope)
{
  //gslc_tsGui*  pGui  = (gslc_tsGui*)(pvGui);
  gslc_tsElem* pElem = (gslc_tsElem*)(pvScope);  
  
  m_fCoordX = 50+25.0*(sin(m_nCount/2.5));
  m_fCoordY = 50+15.0*(cos(m_nCount/1.75));
  m_fCoordZ = 13.02;

  // Adjust the scanner's origin for fun
  m_nOriginX = (int16_t)m_fCoordX;
  m_nOriginY = (int16_t)m_fCoordY;
  
  // Manually mark the scanner element as needing redraw
  // since we have shifted its relative coordinates (via origin)
  gslc_ElemSetRedraw(pElem,true);  
  
  return true;
}

// Button callbacks
bool CbBtnQuit(void* pvGui,void *pvElem,gslc_teTouch eTouch,int16_t nX,int16_t nY)
{
  if (eTouch == GSLC_TOUCH_UP_IN) {
    m_bQuit = true;
  }
  return true;
}

// Create the default elements on each page
bool InitOverlays()
{
  gslc_tsElem*  pElem = NULL;

  gslc_PageAdd(&m_gui,E_PG_MAIN,m_asPageElem,MAX_ELEM_PG_MAIN);
  
  // Background flat color
  gslc_SetBkgndColor(&m_gui,GSLC_COL_BLACK);

  // Graphic logo from SD card
  static const char m_strLogo[] = IMG_LOGO;
  pElem = gslc_ElemCreateImg(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){160-100,5,200,40},
    gslc_GetImageFromSD(m_strLogo,GSLC_IMGREF_FMT_BMP24));


  // Create background box
  pElem = gslc_ElemCreateBox(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){10,50,300,150});
  gslc_ElemSetCol(pElem,GSLC_COL_WHITE,GSLC_COL_BLACK,GSLC_COL_BLACK);

  // Create Quit button with text label
  static const char mstr1[] PROGMEM = "Quit";
  pElem = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_QUIT,E_PG_MAIN,
    (gslc_tsRect){40,210,50,20},(char*)mstr1,strlen_P(mstr1),E_FONT_BTN,&CbBtnQuit); 
  gslc_ElemSetTxtMem(pElem,GSLC_TXT_MEM_PROG);    

  // Create counter
  static const char mstr2[] PROGMEM = "Searches:";
  pElem = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){20,60,50,10},
    (char*)mstr2,strlen_P(mstr2),E_FONT_TXT);
  gslc_ElemSetTxtMem(pElem,GSLC_TXT_MEM_PROG);     
  static char mstr3[8] = "";
  pElem = gslc_ElemCreateTxt(&m_gui,E_ELEM_TXT_COUNT,E_PG_MAIN,(gslc_tsRect){80,60,50,10},
    mstr3,8,E_FONT_TXT);
  gslc_ElemSetTxtCol(pElem,GSLC_COL_GRAY_LT2);

  // Create progress bar
  static const char mstr4[] PROGMEM = "Progress:";
  pElem = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){20,80,50,10},
    (char*)mstr4,strlen_P(mstr4),E_FONT_TXT);
  gslc_ElemSetTxtMem(pElem,GSLC_TXT_MEM_PROG);    
  pElem = gslc_ElemXGaugeCreate(&m_gui,E_ELEM_PROGRESS,E_PG_MAIN,&m_sXGauge,(gslc_tsRect){80,80,50,10},
    0,100,0,GSLC_COL_GREEN,false);

  
  // Create other labels
  static const char mstr5[] PROGMEM = "Coord X:";
  pElem = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){40,100,50,10},
    (char*)mstr5,strlen_P(mstr5),E_FONT_TXT);
  gslc_ElemSetTxtMem(pElem,GSLC_TXT_MEM_PROG);      
  gslc_ElemSetTxtCol(pElem,GSLC_COL_WHITE);
  static char mstr6[8] = "";
  pElem = gslc_ElemCreateTxt(&m_gui,E_ELEM_DATAX,E_PG_MAIN,(gslc_tsRect){100,100,50,10},
    mstr6,8,E_FONT_TXT);
  gslc_ElemSetTxtCol(pElem,GSLC_COL_GRAY_LT2);

  static const char mstr7[] PROGMEM = "Coord Y:";
  pElem = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){40,120,50,10},
    (char*)mstr7,strlen_P(mstr7),E_FONT_TXT);
  gslc_ElemSetTxtMem(pElem,GSLC_TXT_MEM_PROG);      
  gslc_ElemSetTxtCol(pElem,GSLC_COL_WHITE);
  static char mstr8[8] = "";
  pElem = gslc_ElemCreateTxt(&m_gui,E_ELEM_DATAY,E_PG_MAIN,(gslc_tsRect){100,120,50,10},
    mstr8,8,E_FONT_TXT);
  gslc_ElemSetTxtCol(pElem,GSLC_COL_GRAY_LT2);

  static const char mstr9[] PROGMEM = "Coord Z:";
  pElem = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){40,140,50,10},
    (char*)mstr9,strlen_P(mstr9),E_FONT_TXT);
  gslc_ElemSetTxtMem(pElem,GSLC_TXT_MEM_PROG);      
  gslc_ElemSetTxtCol(pElem,GSLC_COL_WHITE);
  static char mstr10[8] = "";
  pElem = gslc_ElemCreateTxt(&m_gui,E_ELEM_DATAZ,E_PG_MAIN,(gslc_tsRect){100,140,50,10},
    mstr10,8,E_FONT_TXT);
  gslc_ElemSetTxtCol(pElem,GSLC_COL_GRAY_LT2);

  static const char mstr11[] PROGMEM = "Example of GUIslice C library";
  pElem = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){120,210,170,20},
    (char*)mstr11,strlen_P(mstr11),E_FONT_BTN);
  gslc_ElemSetTxtMem(pElem,GSLC_TXT_MEM_PROG);      
  gslc_ElemSetTxtAlign(pElem,GSLC_ALIGN_MID_LEFT);
  gslc_ElemSetTxtCol(pElem,GSLC_COL_RED_LT2);

  // --------------------------------------------------------------------------
  // Create scanner
  pElem = gslc_ElemCreateBox(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){190-1-2,75-1-12,100+2+4,100+2+10+4});
  gslc_ElemSetCol(pElem,GSLC_COL_BLUE_LT2,GSLC_COL_BLACK,GSLC_COL_BLACK);

  static const char mstr12[] PROGMEM = "SCANNER";
  pElem = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){190,75-11,100,10},
    (char*)mstr12,strlen_P(mstr12),E_FONT_TXT);
  gslc_ElemSetTxtMem(pElem,GSLC_TXT_MEM_PROG);      
  gslc_ElemSetTxtCol(pElem,GSLC_COL_BLUE_DK2);
  gslc_ElemSetTxtAlign(pElem,GSLC_ALIGN_MID_MID);
  
  pElem = gslc_ElemCreateBox(&m_gui,E_ELEM_SCAN,E_PG_MAIN,(gslc_tsRect){190-1,75-1,100+2,100+2});
  gslc_ElemSetCol(pElem,GSLC_COL_BLUE_LT2,GSLC_COL_BLACK,GSLC_COL_BLACK);
  // Set the callback function to handle all drawing for the element
  gslc_ElemSetDrawFunc(pElem,&CbDrawScanner);
  // Set the callback function to update content automatically
  gslc_ElemSetTickFunc(pElem,&CbTickScanner);
  
  // --------------------------------------------------------------------------

  return true;
}

void setup()
{
  bool              bOk = true;
  char              acTxt[MAX_STR];

  // -----------------------------------
  // Initialize
  if (!gslc_Init(&m_gui,&m_drv,m_asPage,MAX_PAGE,m_asFont,MAX_FONT)) { exit(1); }

  // Load Fonts
  bOk = gslc_FontAdd(&m_gui,E_FONT_BTN,"",1);
  if (!bOk) { fprintf(stderr,"ERROR: FontAdd failed\n"); exit(1); }
  bOk = gslc_FontAdd(&m_gui,E_FONT_TXT,"",1);
  if (!bOk) { fprintf(stderr,"ERROR: FontAdd failed\n"); exit(1); }


  // -----------------------------------
  // Create the graphic elements
  InitOverlays();

  // Start up display on main page
  gslc_SetPageCur(&m_gui,E_PG_MAIN);

  // Save some element references for quick access
  gslc_tsElem*  pElemCount      = gslc_PageFindElemById(&m_gui,E_PG_MAIN,E_ELEM_TXT_COUNT);
  gslc_tsElem*  pElemDataX      = gslc_PageFindElemById(&m_gui,E_PG_MAIN,E_ELEM_DATAX);
  gslc_tsElem*  pElemDataY      = gslc_PageFindElemById(&m_gui,E_PG_MAIN,E_ELEM_DATAY);
  gslc_tsElem*  pElemDataZ      = gslc_PageFindElemById(&m_gui,E_PG_MAIN,E_ELEM_DATAZ);
  gslc_tsElem*  pElemProgress   = gslc_PageFindElemById(&m_gui,E_PG_MAIN,E_ELEM_PROGRESS);
  
  // -----------------------------------
  // Main event loop

  m_bQuit = false;
  while (!m_bQuit) {
    
    // Update the data display values
    m_nCount++;

    // -----------------------------------------------

    // Perform any immediate updates on active page
    snprintf(acTxt,MAX_STR,"%u",m_nCount);
    gslc_ElemSetTxtStr(pElemCount,acTxt);

    // By default, Arduino sprintf() doesn't include floating point
    // support, so we're just going to display integers
    snprintf(acTxt,MAX_STR,"%4d",(int16_t)(m_fCoordX-50));
    gslc_ElemSetTxtStr(pElemDataX,acTxt);
    snprintf(acTxt,MAX_STR,"%4d",(int16_t)(m_fCoordY-50));
    gslc_ElemSetTxtStr(pElemDataY,acTxt);
    snprintf(acTxt,MAX_STR,"%4d",(int16_t)(m_fCoordZ));
    gslc_ElemSetTxtStr(pElemDataZ,acTxt);
    gslc_ElemSetTxtCol(pElemDataZ,(m_fCoordY>50)?GSLC_COL_GREEN_LT2:GSLC_COL_RED_DK2);

    gslc_ElemXGaugeUpdate(pElemProgress,50+50*sin(m_nCount/5.0));

    // -----------------------------------------------
    
    // Periodically call GUIslice update function   
    gslc_Update(&m_gui);

    // Slow down display
    delay(10);

  } // bQuit


  // -----------------------------------
  // Close down display

  gslc_Quit(&m_gui);
}

void loop() { }


