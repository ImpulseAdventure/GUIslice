//
// GUIslice Library Examples
// - Calvin Hass
// - http://www.impulseadventure.com/elec/guislice-gui.html
// - Example 07 (Arduino):
//   - Sliders with dynamic color control and position callback
//   - Also demonstrates the use of PROGMEM for strings and
//     read-only elements (using ElemCreate*_P functions).
//
// ARDUINO NOTES:
// - GUIslice_config.h must be edited to match the pinout connections
//   between the Arduino CPU and the display controller (see ADAGFX_PIN_*).
// - To support a larger number of GUI elements, it is recommended to
//   use an Arduino CPU that provides more than 2KB of SRAM.
//

#include "GUIslice.h"
#include "GUIslice_ex.h"
#include "GUIslice_drv.h"

// Defines for resources

// Enumerations for pages, elements, fonts, images
enum {E_PG_MAIN};
enum {E_ELEM_BOX,E_ELEM_BTN_QUIT,E_ELEM_COLOR,
      E_SLIDER_R,E_SLIDER_G,E_SLIDER_B};
enum {E_FONT_TXT,E_FONT_TITLE};

bool      m_bQuit = false;

// Free-running counter for display
unsigned  m_nCount = 0;

// Instantiate the GUI
#define MAX_PAGE                1
#define MAX_FONT                5
#define MAX_ELEM_PG_MAIN        17                                        // # Elems total
#define MAX_ELEM_PG_MAIN_PROG   11                                        // # Elems in Flash
#define MAX_ELEM_PG_MAIN_RAM    MAX_ELEM_PG_MAIN - MAX_ELEM_PG_MAIN_PROG  // # Elems in RAM

gslc_tsGui                  m_gui;
gslc_tsDriver               m_drv;
gslc_tsFont                 m_asFont[MAX_FONT];
gslc_tsPage                 m_asPage[MAX_PAGE];
gslc_tsElem                 m_asPageElem[MAX_ELEM_PG_MAIN_RAM];   // Storage for all elements in RAM
gslc_tsElemRef              m_asPageElemRef[MAX_ELEM_PG_MAIN];    // References for all elements in GUI

gslc_tsXSlider              m_sXSlider_R,m_sXSlider_G,m_sXSlider_B;

// Current RGB value for color box
// - Globals defined here for convenience so that callback
//   can update R,G,B components independently
uint8_t   m_nPosR = 255;
uint8_t   m_nPosG = 128;
uint8_t   m_nPosB = 0;


// Quit button callback
bool CbBtnQuit(void* pvGui,void *pvElem,gslc_teTouch eTouch,int16_t nX,int16_t nY)
{
  if (eTouch == GSLC_TOUCH_UP_IN) {
    m_bQuit = true;
  }
  return true;
}

// Callback function for when a slider's position has been updated
// - After a slider position has been changed, update the color box
// - Note that all three sliders use the same callback for
//   convenience. From the element's ID we can determine which
//   slider was updated.
bool CbSlidePos(void* pvGui,void* pvElem,int16_t nPos)
{
  gslc_tsGui*     pGui    = (gslc_tsGui*)(pvGui);
  gslc_tsElem*    pElem   = (gslc_tsElem*)(pvElem);  
  //gslc_tsXSlider* pSlider = (gslc_tsXSlider*)(pElem->pXData);  
  
  // Fetch the new RGB component from the slider
  switch (pElem->nId) {
    case E_SLIDER_R:
      m_nPosR = gslc_ElemXSliderGetPos(pElem);
      break;
    case E_SLIDER_G:
      m_nPosG = gslc_ElemXSliderGetPos(pElem);      
      break;
    case E_SLIDER_B:
      m_nPosB = gslc_ElemXSliderGetPos(pElem);      
      break;
    default:
      break;
  }
  
  // Calculate the new RGB value
  gslc_tsColor colRGB = (gslc_tsColor){m_nPosR,m_nPosG,m_nPosB};
  
  // Update the color box
  gslc_tsElem* pElemColor = gslc_PageFindElemById(pGui,E_PG_MAIN,E_ELEM_COLOR);
  gslc_ElemSetCol(pElemColor,GSLC_COL_WHITE,colRGB,GSLC_COL_WHITE); 
  
  return true;
}


// Create page elements
bool InitOverlays()
{
  gslc_tsElem*  pElem = NULL;

  gslc_PageAdd(&m_gui,E_PG_MAIN,m_asPageElem,MAX_ELEM_PG_MAIN_RAM,m_asPageElemRef,MAX_ELEM_PG_MAIN);
  
  // Background flat color
  gslc_SetBkgndColor(&m_gui,GSLC_COL_GRAY_DK2);

  // Create Title with offset shadow
  #define TMP_COL1 (gslc_tsColor){ 32, 32, 60}
  #define TMP_COL2 (gslc_tsColor){128,128,240}
  // Note: must use title Font ID
  gslc_ElemCreateTxt_P(m_gui,98,E_PG_MAIN,2,2,320,50,"Home Automation",1,
          TMP_COL1,GSLC_COL_BLACK,GSLC_COL_BLACK,GSLC_ALIGN_MID_MID,false,false); 
  gslc_ElemCreateTxt_P(m_gui,99,E_PG_MAIN,0,0,320,50,"Home Automation",1,
          TMP_COL2,GSLC_COL_BLACK,GSLC_COL_BLACK,GSLC_ALIGN_MID_MID,false,false);

 
  // Create background box
  gslc_ElemCreateBox_P(m_gui,200,E_PG_MAIN,10,50,300,180,GSLC_COL_WHITE,GSLC_COL_BLACK,true,true);
  
  // Create dividers
  gslc_ElemCreateBox_P(m_gui,201,E_PG_MAIN,20,100,280,1,GSLC_COL_GRAY_DK3,GSLC_COL_BLACK,true,true);
  gslc_ElemCreateBox_P(m_gui,202,E_PG_MAIN,235,60,1,35,GSLC_COL_GRAY_DK3,GSLC_COL_BLACK,true,true);
  
  
  // Create color box
  pElem = gslc_ElemCreateBox(&m_gui,E_ELEM_COLOR,E_PG_MAIN,(gslc_tsRect){20,90+30,130,100});
  gslc_tsColor colRGB = (gslc_tsColor){m_nPosR,m_nPosG,m_nPosB};
  gslc_ElemSetCol(pElem,GSLC_COL_WHITE,colRGB,GSLC_COL_WHITE); 
  
  // Create Quit button with text label
  static const char mstr2[] PROGMEM = "SAVE";  
  pElem = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_QUIT,E_PG_MAIN,
    (gslc_tsRect){250,60,50,30},(char*)mstr2,strlen_P(mstr2),E_FONT_TXT,&CbBtnQuit);
  gslc_ElemSetTxtMem(pElem,GSLC_TXT_MEM_PROG);    
  gslc_ElemSetCol(pElem,GSLC_COL_BLUE_DK2,GSLC_COL_BLUE_DK4,GSLC_COL_BLUE_DK1);    
  gslc_ElemSetTxtCol(pElem,GSLC_COL_WHITE);
  
  // Create dummy selector
  gslc_ElemCreateTxt_P(m_gui,100,E_PG_MAIN,20,65,100,20,"Selected Room:",0,
          GSLC_COL_GRAY_LT2,GSLC_COL_BLACK,GSLC_COL_BLACK,GSLC_ALIGN_MID_LEFT,false,true);

  static const char mstr4[] PROGMEM = "Kitchen...";  
  pElem = gslc_ElemCreateBtnTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,
    (gslc_tsRect){140,65,80,20},(char*)mstr4,strlen_P(mstr4),E_FONT_TXT,NULL);
  gslc_ElemSetTxtMem(pElem,GSLC_TXT_MEM_PROG);        
  gslc_ElemSetCol(pElem,GSLC_COL_GRAY_DK2,GSLC_COL_GRAY_DK3,GSLC_COL_BLUE_DK1);    
  gslc_ElemSetTxtCol(pElem,GSLC_COL_WHITE);
  
  // Create sliders
  // - Define element arrangement
  int16_t   nCtrlY    = 115;
  int16_t   nCtrlGap  = 30;
  uint16_t  nSlideW   = 80;
  uint16_t  nSlideH   = 20;
  int16_t   nLabelX   = 160;
  uint16_t  nLabelW   = 30;
  uint16_t  nLabelH   = 20;
  int16_t   nSlideX   = nLabelX + nLabelW + 20;

  gslc_ElemCreateTxt_P(m_gui,105,E_PG_MAIN,160,115,120,20,"Set LED RGB:",0,
          GSLC_COL_WHITE,GSLC_COL_BLACK,GSLC_COL_BLACK,GSLC_ALIGN_MID_LEFT,false,true);
  nCtrlY += 25;
  
  // Create three sliders (R,G,B) and assign callback function
  // that is invoked upon change. The common callback will update
  // the color box.

  // Static text label
  gslc_ElemCreateTxt_P(m_gui,106,E_PG_MAIN,160,140,30,20,"Red:",0,
          GSLC_COL_GRAY_LT3,GSLC_COL_BLACK,GSLC_COL_BLACK,GSLC_ALIGN_MID_LEFT,false,true);
  // Slider
  pElem = gslc_ElemXSliderCreate(&m_gui,E_SLIDER_R,E_PG_MAIN,&m_sXSlider_R,
          (gslc_tsRect){nSlideX,nCtrlY,nSlideW,nSlideH},0,255,m_nPosR,5,false);
  gslc_ElemSetCol(pElem,GSLC_COL_RED,GSLC_COL_BLACK,GSLC_COL_BLACK);          
  gslc_ElemXSliderSetStyle(pElem,true,GSLC_COL_RED_DK4,10,5,GSLC_COL_GRAY_DK2);
  gslc_ElemXSliderSetPosFunc(pElem,&CbSlidePos);  
  nCtrlY += nCtrlGap;

  // Static text label
  gslc_ElemCreateTxt_P(m_gui,107,E_PG_MAIN,160,170,30,20,"Green:",0,
          GSLC_COL_GRAY_LT3,GSLC_COL_BLACK,GSLC_COL_BLACK,GSLC_ALIGN_MID_LEFT,false,true);
  // Slider
  pElem = gslc_ElemXSliderCreate(&m_gui,E_SLIDER_G,E_PG_MAIN,&m_sXSlider_G,
          (gslc_tsRect){nSlideX,nCtrlY,nSlideW,nSlideH},0,255,m_nPosG,5,false);
  gslc_ElemSetCol(pElem,GSLC_COL_GREEN,GSLC_COL_BLACK,GSLC_COL_BLACK);
  gslc_ElemXSliderSetStyle(pElem,true,GSLC_COL_GREEN_DK4,10,5,GSLC_COL_GRAY_DK2);
  gslc_ElemXSliderSetPosFunc(pElem,&CbSlidePos);    
  nCtrlY += nCtrlGap;
  
  // Static text label
  gslc_ElemCreateTxt_P(m_gui,108,E_PG_MAIN,160,200,30,20,"Blue:",0,
          GSLC_COL_GRAY_LT3,GSLC_COL_BLACK,GSLC_COL_BLACK,GSLC_ALIGN_MID_LEFT,false,true);
  // Slider
  pElem = gslc_ElemXSliderCreate(&m_gui,E_SLIDER_B,E_PG_MAIN,&m_sXSlider_B,
          (gslc_tsRect){nSlideX,nCtrlY,nSlideW,nSlideH},0,255,m_nPosB,5,false);
  gslc_ElemSetCol(pElem,GSLC_COL_BLUE,GSLC_COL_BLACK,GSLC_COL_BLACK);          
  gslc_ElemXSliderSetStyle(pElem,true,GSLC_COL_BLUE_DK4,10,5,GSLC_COL_GRAY_DK2);
  gslc_ElemXSliderSetPosFunc(pElem,&CbSlidePos);    
  nCtrlY += nCtrlGap;

  gslc_ElemCreateTxt_P(m_gui,109,E_PG_MAIN,250,230,60,10,"GUIslice Example",0,
          GSLC_COL_BLACK,GSLC_COL_BLACK,GSLC_COL_BLACK,GSLC_ALIGN_MID_RIGHT,false,false);

  return true;
}


void setup()
{
  bool bOk = true;

  // Initialize
  if (!gslc_Init(&m_gui,&m_drv,m_asPage,MAX_PAGE,m_asFont,MAX_FONT)) { exit(1); }  

  // Load Fonts
  // - NOTE: If we are using the ElemCreate*_P() macros then it is important to note
  //   the order in which fonts are added as this same font index is provided to certain
  //   ElemCreate*_P() functions (eg. ElemCreateTxt_P).
  bOk = gslc_FontAdd(&m_gui,E_FONT_TXT,"",1); // m_asFont[0]
  if (!bOk) { fprintf(stderr,"ERROR: FontAdd failed\n"); exit(1); }
  bOk = gslc_FontAdd(&m_gui,E_FONT_TITLE,"",3); // m_asFont[1]
  if (!bOk) { fprintf(stderr,"ERROR: FontAdd failed\n"); exit(1); }

  // Create pages display
  InitOverlays();

  // Start up display on main page
  gslc_SetPageCur(&m_gui,E_PG_MAIN);
 
  m_bQuit = false;
}

void loop()
{
  // General counter
  m_nCount++;

  // Periodically call GUIslice update function
  gslc_Update(&m_gui);

  // Upon Quit, close down GUI and terminate loop
  if (m_bQuit) {
    gslc_Quit(&m_gui);
    exit(1);  // In Arduino, this essentially starts infinite loop
  }
}

