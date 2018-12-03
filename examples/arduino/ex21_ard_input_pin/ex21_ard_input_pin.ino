//
// GUIslice Library Examples
// - Calvin Hass
// - https://www.impulseadventure.com/elec/guislice-gui.html
// - https://github.com/ImpulseAdventure/GUIslice
// - Example 21 (Arduino): Dynamic content with GPIO control
//   - Same as Example 04, except adds GPIO pin input control
//     IMPORTANT: See notes under Button Control
//   - Demonstrates push buttons, checkboxes and slider controls
//   - NOTE: This is the simple version of the example without
//     optimizing for memory consumption. Therefore, it may not
//     run on Arduino devices with limited memory. A "minimal"
//     version is located in the "arduino_min" folder which includes
//     FLASH memory optimization for reduced memory devices.
//
// ARDUINO NOTES:
// - GUIslice_config.h must be edited to match the pinout connections
//   between the Arduino CPU and the display controller (see ADAGFX_PIN_*).
//

#include "GUIslice.h"
#include "GUIslice_ex.h"
#include "GUIslice_drv.h"

#include <Adafruit_GFX.h>

// Button Control
// - Define the pin connections for the external buttons.
// - NOTE: The defaults provided assume an ATmega 2560, and will need to be
//         changed according to your board / wiring.
// - NOTE: The EasyButton constructor defaults to pull-up enabled with inverted
//         polarity, meaning that the switches will connect the pin to ground
//         when closed, and pull-up (via ATmega internal pullup resistors) when
//         open. If your device does not have internal pullup resistors, then
//         you must provide them externally.
//           EasyButton(uint8_t pin, uint32_t dbTime = 35, bool puEnable = true, bool invert = true);
#include "EasyButton.h"
#define PIN_PREV  38
#define PIN_SEL   40
#define PIN_NEXT  42
EasyButton btn_prev(PIN_PREV, 35, true, true);
EasyButton btn_sel( PIN_SEL,  35, true, true);
EasyButton btn_next(PIN_NEXT, 35, true, true);


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
#define MAX_ELEM_PG_MAIN          16                                        // # Elems total
#define MAX_ELEM_PG_MAIN_RAM      MAX_ELEM_PG_MAIN                          // # Elems in RAM

gslc_tsGui                  m_gui;
gslc_tsDriver               m_drv;
gslc_tsFont                 m_asFont[MAX_FONT];
gslc_tsPage                 m_asPage[MAX_PAGE];
gslc_tsElem                 m_asPageElem[MAX_ELEM_PG_MAIN_RAM];
gslc_tsElemRef              m_asPageElemRef[MAX_ELEM_PG_MAIN];

gslc_tsXGauge               m_sXGauge,m_sXGauge1;
gslc_tsXCheckbox            m_asXCheck[3];
gslc_tsXSlider              m_sXSlider;

#define MAX_INPUT_MAP       3
gslc_tsInputMap             m_asInputMap[MAX_INPUT_MAP];

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

// Pin Input polling callback function
bool CbPinPoll(void* pvGui, int16_t* pnPinInd, int16_t* pnPinVal)
{
  // Sample all pin inputs
  btn_prev.read();
  btn_sel.read();
  btn_next.read();

  // Determine if any pin edge events occur
  // - If multiple pin events occur, they will be handled in consecutive CbPinPoll() calls
  if      (btn_prev.wasPressed()) { *pnPinInd = PIN_PREV; *pnPinVal = 1; }
  else if (btn_sel.wasPressed())  { *pnPinInd = PIN_SEL;  *pnPinVal = 1; }
  else if (btn_next.wasPressed()) { *pnPinInd = PIN_NEXT; *pnPinVal = 1; }
  else return false; // No pin event detected

  // If we reach here, then an pin event was detected
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
  pElemRef = gslc_ElemCreateBox(&m_gui,E_ELEM_BOX,E_PG_MAIN,(gslc_tsRect){10,50,300,150});
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_WHITE,GSLC_COL_BLACK,GSLC_COL_BLACK);

  // Create Quit button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_QUIT,E_PG_MAIN,
    (gslc_tsRect){160,80,80,40},(char*)"Quit",0,E_FONT_BTN,&CbBtnQuit);

  // Create counter
  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){20,60,50,10},
    (char*)"Count:",0,E_FONT_TXT);
  static char mstr1[8] = "";
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TXT_COUNT,E_PG_MAIN,(gslc_tsRect){80,60,50,10},
    mstr1,sizeof(mstr1),E_FONT_TXT);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_YELLOW);
  m_pElemCnt = pElemRef; // Save for quick access

  // Create progress bar (horizontal)
  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){20,80,50,10},
    (char*)"Progress:",0,E_FONT_TXT);
  pElemRef = gslc_ElemXGaugeCreate(&m_gui,E_ELEM_PROGRESS,E_PG_MAIN,&m_sXGauge,
    (gslc_tsRect){80,80,50,10},0,100,0,GSLC_COL_GREEN,false);
  m_pElemProgress = pElemRef; // Save for quick access

  // Second progress bar (vertical)
  // - Demonstration of vertical bar with offset zero-pt showing both positive and negative range
  pElemRef = gslc_ElemXGaugeCreate(&m_gui,E_ELEM_PROGRESS1,E_PG_MAIN,&m_sXGauge1,
    (gslc_tsRect){280,80,10,100},-25,75,-15,GSLC_COL_RED,true);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_BLUE_DK3,GSLC_COL_BLACK,GSLC_COL_BLACK);
  m_pElemProgress1 = pElemRef; // Save for quick access


  // Create checkbox 1
  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){20,100,20,20},
    (char*)"Check1:",0,E_FONT_TXT);
  pElemRef = gslc_ElemXCheckboxCreate(&m_gui,E_ELEM_CHECK1,E_PG_MAIN,&m_asXCheck[0],
    (gslc_tsRect){80,100,20,20},false,GSLCX_CHECKBOX_STYLE_X,GSLC_COL_BLUE_LT2,false);

  // Create radio 1
  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){20,135,20,20},
    (char*)"Radio1:",0,E_FONT_TXT);
  pElemRef = gslc_ElemXCheckboxCreate(&m_gui,E_ELEM_RADIO1,E_PG_MAIN,&m_asXCheck[1],
    (gslc_tsRect){80,135,20,20},true,GSLCX_CHECKBOX_STYLE_ROUND,GSLC_COL_ORANGE,false);
  gslc_ElemSetGroup(&m_gui,pElemRef,E_GROUP1);

  // Create radio 2
  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){20,160,20,20},
    (char*)"Radio2:",0,E_FONT_TXT);
  pElemRef = gslc_ElemXCheckboxCreate(&m_gui,E_ELEM_RADIO2,E_PG_MAIN,&m_asXCheck[2],
    (gslc_tsRect){80,160,20,20},true,GSLCX_CHECKBOX_STYLE_ROUND,GSLC_COL_ORANGE,false);
  gslc_ElemSetGroup(&m_gui,pElemRef,E_GROUP1);

  // Create slider
  pElemRef = gslc_ElemXSliderCreate(&m_gui,E_ELEM_SLIDER,E_PG_MAIN,&m_sXSlider,
    (gslc_tsRect){160,140,100,20},0,100,60,5,false);
  gslc_ElemXSliderSetStyle(&m_gui,pElemRef,true,(gslc_tsColor){0,0,128},10,
          5,(gslc_tsColor){64,64,64});
  m_pElemSlider = pElemRef; // Save for quick access


  static char mstr2[8] = "Slider:";
  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){160,160,60,20},
    mstr2,sizeof(mstr2),E_FONT_TXT);
  static char mstr3[6] = "???";
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TXT_SLIDER,E_PG_MAIN,(gslc_tsRect){220,160,40,20},
    mstr3,sizeof(mstr3),E_FONT_TXT);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_ORANGE);
  m_pElemSliderTxt = pElemRef; // Save for quick access

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

  // Initialize pins controls. Note that this calls pinMode()
  btn_prev.begin();
  btn_sel.begin();
  btn_next.begin();

  // Set the pin poll callback function
  gslc_SetPinPollFunc(&m_gui, CbPinPoll);

  // Create the GUI input mapping (pin event to GUI action)
  gslc_InitInputMap(&m_gui,m_asInputMap,MAX_INPUT_MAP);
  gslc_InputMapAdd(&m_gui, GSLC_INPUT_PIN_ASSERT, PIN_PREV, GSLC_ACTION_FOCUS_PREV, 0);
  gslc_InputMapAdd(&m_gui, GSLC_INPUT_PIN_ASSERT, PIN_SEL,  GSLC_ACTION_SELECT, 0);
  gslc_InputMapAdd(&m_gui, GSLC_INPUT_PIN_ASSERT, PIN_NEXT, GSLC_ACTION_FOCUS_NEXT, 0);

  // Use default font
  if (!gslc_FontAdd(&m_gui,E_FONT_BTN,GSLC_FONTREF_PTR,NULL,1)) { return; }
  if (!gslc_FontAdd(&m_gui,E_FONT_TXT,GSLC_FONTREF_PTR,NULL,1)) { return; }

  // Create graphic elements
  InitOverlays();

  // Start up display on main page
  gslc_SetPageCur(&m_gui,E_PG_MAIN);

  m_bQuit = false;
}

void loop()
{
  char                acTxt[MAX_STR];

  // General counter
  m_nCount++;

  // Update elements on active page

  snprintf(acTxt,MAX_STR,"%u",m_nCount/5);
  gslc_ElemSetTxtStr(&m_gui,m_pElemCnt,acTxt);

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
