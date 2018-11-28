//
// GUIslice Library Examples
// - Calvin Hass
// - https://www.impulseadventure.com/elec/guislice-gui.html
// - https://github.com/ImpulseAdventure/GUIslice
// - Example 04 (Arduino): Dynamic content [minimum RAM version]
//   - Demonstrates push buttons, checkboxes and slider controls
//   - Demonstrates the use of ElemCreate*_P() functions
//     These RAM-reduced examples take advantage of the internal
//     Flash storage (via PROGMEM).
//   - NOTE: This sketch requires moderate program storage in Flash.
//     As a result, it may not run on basic Arduino devices (eg. ATmega328)
//
// ARDUINO NOTES:
// - GUIslice_config.h must be edited to match the pinout connections
//   between the Arduino CPU and the display controller (see ADAGFX_PIN_*).
//

#include "GUIslice.h"
#include "GUIslice_ex.h"
#include "GUIslice_drv.h"


// Defines for resources


// Enumerations for pages, elements, fonts, images
enum {E_PG_MAIN};
enum {E_ELEM_BOX,E_ELEM_BTN_QUIT,E_ELEM_TXT_COUNT,E_ELEM_PROGRESS,E_ELEM_PROGRESS1,
      E_ELEM_CHECK1,E_ELEM_RADIO1,E_ELEM_RADIO2,E_ELEM_SLIDER,E_ELEM_TXT_SLIDER};
enum {E_FONT_BTN,E_FONT_TXT};
enum {E_GROUP1};

bool        m_bQuit = false;

// Free-running counter for display
unsigned    m_nCount = 0;


// Instantiate the GUI
#define MAX_PAGE                1
#define MAX_FONT                2

// Define the maximum number of elements per page
// - To enable the same code to run on devices that support storing
//   data into Flash (PROGMEM) and those that don't, we can make the
//   number of elements in Flash dependent upon GSLC_USE_PROGMEM
// - This should allow both Arduino and ARM Cortex to use the same code
#define MAX_ELEM_PG_MAIN          16                                        // # Elems total
#if (GSLC_USE_PROGMEM)
  #define MAX_ELEM_PG_MAIN_PROG   16                                        // # Elems in Flash
#else
  #define MAX_ELEM_PG_MAIN_PROG   0                                         // # Elems in Flash
#endif
#define MAX_ELEM_PG_MAIN_RAM      MAX_ELEM_PG_MAIN - MAX_ELEM_PG_MAIN_PROG  // # Elems in RAM

gslc_tsGui                  m_gui;
gslc_tsDriver               m_drv;
gslc_tsFont                 m_asFont[MAX_FONT];
gslc_tsPage                 m_asPage[MAX_PAGE];
gslc_tsElem                 m_asPageElem[MAX_ELEM_PG_MAIN_RAM];
gslc_tsElemRef              m_asPageElemRef[MAX_ELEM_PG_MAIN];


#define MAX_STR             15

  // Save some element references for quick access
  gslc_tsElemRef*  m_pElemCnt        = NULL;
  gslc_tsElemRef*  m_pElemProgress   = NULL;
  gslc_tsElemRef*  m_pElemProgress1  = NULL;
  gslc_tsElemRef*  m_pElemSlider     = NULL;
  gslc_tsElemRef*  m_pElemSliderTxt  = NULL;

// Define debug message function
static int16_t DebugOut(char ch) { Serial.write(ch); return 0; }

// Button callbacks
bool CbBtnQuit(void* pvGui,void *pvElem,gslc_teTouch eTouch,int16_t nX,int16_t nY)
{
  if (eTouch == GSLC_TOUCH_UP_IN) {
    m_bQuit = true;
  }
  return true;
}


// Create page elements
bool InitOverlays()
{
  gslc_tsElemRef* pElemRef;

  gslc_PageAdd(&m_gui,E_PG_MAIN,m_asPageElem,MAX_ELEM_PG_MAIN_RAM,m_asPageElemRef,MAX_ELEM_PG_MAIN);

  // Background flat color
  gslc_SetBkgndColor(&m_gui,GSLC_COL_GRAY_DK2);

  // Create background box
  gslc_ElemCreateBox_P(&m_gui,100,E_PG_MAIN,10,50,300,150,GSLC_COL_WHITE,GSLC_COL_BLACK,true,true,NULL,NULL);

  // Create Quit button with text label
  gslc_ElemCreateBtnTxt_P(&m_gui,E_ELEM_BTN_QUIT,E_PG_MAIN,160,80,80,40,"Quit",&m_asFont[0],
    GSLC_COL_WHITE,GSLC_COL_BLUE_DK2,GSLC_COL_BLUE_DK4,GSLC_COL_BLUE_DK2,GSLC_COL_BLUE_DK1,GSLC_ALIGN_MID_MID,true,true,&CbBtnQuit,NULL);


  // Create counter
  gslc_ElemCreateTxt_P(&m_gui,101,E_PG_MAIN,20,60,50,10,"Count:",&m_asFont[1], // E_FONT_TXT
          GSLC_COL_YELLOW,GSLC_COL_BLACK,GSLC_COL_BLACK,GSLC_ALIGN_MID_LEFT,false,true);
  static char mstr3[8] = ""; // Provide space for large counter value
  gslc_ElemCreateTxt_P_R(&m_gui,E_ELEM_TXT_COUNT,E_PG_MAIN,80,60,50,10,mstr3,8,&m_asFont[1], // E_FONT_TXT
          GSLC_COL_GRAY_LT2,GSLC_COL_BLACK,GSLC_COL_BLACK,GSLC_ALIGN_MID_LEFT,false,true);

  // Create progress bar (horizontal)
  gslc_ElemCreateTxt_P(&m_gui,102,E_PG_MAIN,20,80,50,10,"Progress:",&m_asFont[1], // E_FONT_TXT
          GSLC_COL_YELLOW,GSLC_COL_BLACK,GSLC_COL_BLACK,GSLC_ALIGN_MID_LEFT,false,true);
  gslc_ElemXGaugeCreate_P(&m_gui,E_ELEM_PROGRESS,E_PG_MAIN,80,80,50,10,
    0,100,0,GSLC_COL_GRAY,GSLC_COL_BLACK,GSLC_COL_GREEN,false);
  m_pElemProgress = gslc_PageFindElemById(&m_gui,E_PG_MAIN,E_ELEM_PROGRESS);


  // Second progress bar (vertical)
  // - Demonstration of vertical bar with offset zero-pt showing both positive and negative range
  gslc_ElemXGaugeCreate_P(&m_gui,E_ELEM_PROGRESS1,E_PG_MAIN,280,80,10,100,
    -25,75,-15,GSLC_COL_BLUE_DK3,GSLC_COL_BLACK,GSLC_COL_RED,true);
  m_pElemProgress1 = gslc_PageFindElemById(&m_gui,E_PG_MAIN,E_ELEM_PROGRESS1);

  // Create checkbox 1
  gslc_ElemCreateTxt_P(&m_gui,103,E_PG_MAIN,20,100,20,20,"Check1:",&m_asFont[1], // E_FONT_TXT
          GSLC_COL_YELLOW,GSLC_COL_BLACK,GSLC_COL_BLACK,GSLC_ALIGN_MID_LEFT,false,true);
  gslc_ElemXCheckboxCreate_P(&m_gui,E_ELEM_CHECK1,E_PG_MAIN,80,100,20,20,GSLC_COL_BLACK,true,
          GSLC_GROUP_ID_NONE,false,GSLCX_CHECKBOX_STYLE_X,GSLC_COL_BLUE_LT2,false);

  // Create radio 1
  gslc_ElemCreateTxt_P(&m_gui,104,E_PG_MAIN,20,135,20,20,"Radio1:",&m_asFont[1], // E_FONT_TXT
          GSLC_COL_YELLOW,GSLC_COL_BLACK,GSLC_COL_BLACK,GSLC_ALIGN_MID_LEFT,false,true);
  gslc_ElemXCheckboxCreate_P(&m_gui,E_ELEM_RADIO1,E_PG_MAIN,80,135,20,20,GSLC_COL_BLACK,true,
          E_GROUP1,true,GSLCX_CHECKBOX_STYLE_ROUND,GSLC_COL_ORANGE,false);

  // Create radio 2
  gslc_ElemCreateTxt_P(&m_gui,105,E_PG_MAIN,20,160,20,20,"Radio2:",&m_asFont[1], // E_FONT_TXT
          GSLC_COL_YELLOW,GSLC_COL_BLACK,GSLC_COL_BLACK,GSLC_ALIGN_MID_LEFT,false,true);
  gslc_ElemXCheckboxCreate_P(&m_gui,E_ELEM_RADIO2,E_PG_MAIN,80,160,20,20,GSLC_COL_BLACK,true,
          E_GROUP1,true,GSLCX_CHECKBOX_STYLE_ROUND,GSLC_COL_ORANGE,false);

  // Create slider
  gslc_ElemXSliderCreate_P(&m_gui,E_ELEM_SLIDER,E_PG_MAIN,160,140,100,20,
    0,100,60,5,false,GSLC_COL_GRAY,GSLC_COL_BLACK);
  m_pElemSlider = gslc_PageFindElemById(&m_gui,E_PG_MAIN,E_ELEM_SLIDER); // Save for quick access
  gslc_ElemXSliderSetStyle(&m_gui,m_pElemSlider,true,(gslc_tsColor){0,0,128},10,5,(gslc_tsColor){64,64,64});


  gslc_ElemCreateTxt_P(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,160,160,60,20,"Slider:",&m_asFont[1], // E_FONT_TXT
          GSLC_COL_YELLOW,GSLC_COL_BLACK,GSLC_COL_BLACK,GSLC_ALIGN_MID_LEFT,false,true);

  static char mstr8[6] = "???"; // Provide space for counter value
  gslc_ElemCreateTxt_P_R(&m_gui,E_ELEM_TXT_SLIDER,E_PG_MAIN,220,160,40,20,mstr8,6,&m_asFont[1], // E_FONT_TXT
          GSLC_COL_ORANGE,GSLC_COL_BLACK,GSLC_COL_BLACK,GSLC_ALIGN_MID_LEFT,false,true);
  m_pElemSliderTxt = gslc_PageFindElemById(&m_gui,E_PG_MAIN,E_ELEM_TXT_SLIDER); // Save for quick access

  return true;
}


void setup()
{
  // Initialize debug output
  Serial.begin(9600);
  gslc_InitDebug(&DebugOut);
  //delay(1000);  // NOTE: Some devices require a delay after Serial.begin() before serial port can be used

  // Initialize
  if (!gslc_Init(&m_gui,&m_drv,m_asPage,MAX_PAGE,m_asFont,MAX_FONT)) { return; }

  // Load Fonts
  // - NOTE: If we are using the ElemCreate*_P() macros then it is important to note
  //   the font pointer (array index) as it will be provided to certain
  //   ElemCreate*_P() functions (eg. ElemCreateTxt_P).
  if (!gslc_FontAdd(&m_gui,E_FONT_BTN,GSLC_FONTREF_PTR,NULL,1)) { return; }    // m_asFont[0]
  if (!gslc_FontAdd(&m_gui,E_FONT_TXT,GSLC_FONTREF_PTR,NULL,1)) { return; }    // m_asFont[1]

  // Create graphic elements
  InitOverlays();

  // Start up display on main page
  gslc_SetPageCur(&m_gui,E_PG_MAIN);

  m_bQuit = false;
}

void loop()
{
  char                acTxt[MAX_STR];
  gslc_tsElemRef*     pElemRef = NULL;

  // General counter
  m_nCount++;

  // Update elements on active page
  snprintf(acTxt,MAX_STR,"%u",m_nCount/5);
  pElemRef = gslc_PageFindElemById(&m_gui,E_PG_MAIN,E_ELEM_TXT_COUNT);
  gslc_ElemSetTxtStr(&m_gui,pElemRef,acTxt);

  gslc_ElemXGaugeUpdate(&m_gui,m_pElemProgress,((m_nCount/1)%100));


  // NOTE: A more efficient method is to move the following
  //       code into the slider position callback function.
  //       Please see example 07.
  int nPos = gslc_ElemXSliderGetPos(&m_gui,m_pElemSlider);
  snprintf(acTxt,MAX_STR,"%u",nPos);
  gslc_ElemSetTxtStr(&m_gui,m_pElemSliderTxt,acTxt);

  gslc_ElemXGaugeUpdate(&m_gui,m_pElemProgress1,(nPos*80.0/100.0)-15);

  // Periodically call GUIslice update function
  gslc_Update(&m_gui);

  // Slow down updates
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


