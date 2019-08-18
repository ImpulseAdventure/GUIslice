//
// GUIslice Library Examples
// - Calvin Hass
// - https://www.impulseadventure.com/elec/guislice-gui.html
// - https://github.com/ImpulseAdventure/GUIslice
// - Example 04 (Arduino): Dynamic content
//   - Demonstrates push buttons, checkboxes and slider controls
//   - Shows callback notifications for checkboxes and radio buttons
//   - Provide example of additional Adafruit-GFX fonts
//     (see USE_EXTRA_FONTS)
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
#include "GUIslice_drv.h"

// Include any extended elements
#include "elem/XCheckbox.h"
#include "elem/XSlider.h"
#include "elem/XProgress.h"

// ------------------------------------------------
// Load specific fonts
// ------------------------------------------------

// To demonstrate additional fonts, uncomment the following line:
//#define USE_EXTRA_FONTS

// Different display drivers provide different fonts, so a few examples
// have been provided and selected here. Font files are usually
// located within the display library folder or fonts subfolder.
#ifdef USE_EXTRA_FONTS
  #if defined(DRV_DISP_TFT_ESPI) // TFT_eSPI
    #include <TFT_eSPI.h>
    #define FONT_NAME1 &FreeSansBold12pt7b
  #elif defined(DRV_DISP_ADAGFX_ILI9341_T3) // Teensy
    #include <font_Arial.h>
    #define FONT_NAME1 &Arial_12
    #define SET_FONT_MODE1 // Enable Teensy extra fonts
  #else // Arduino, etc.
    #include <Adafruit_GFX.h>
    #include <gfxfont.h>
    #include "Fonts/FreeSansBold12pt7b.h"
    #define FONT_NAME1 &FreeSansBold12pt7b
  #endif
#else
  // Use the default font
  #define FONT_NAME1 NULL
#endif
// ------------------------------------------------

// Defines for resources


// Enumerations for pages, elements, fonts, images
enum {E_PG_MAIN};
enum {E_ELEM_BOX,E_ELEM_BTN_QUIT,E_ELEM_TXT_COUNT,E_ELEM_PROGRESS,E_ELEM_PROGRESS1,
      E_ELEM_CHECK1,E_ELEM_RADIO1,E_ELEM_RADIO2,E_ELEM_SLIDER,E_ELEM_TXT_SLIDER};
enum {E_FONT_BTN,E_FONT_TXT,MAX_FONT}; // Use separate enum for fonts, MAX_FONT at end
enum {E_GROUP1};

bool        m_bQuit = false;

// Free-running counter for display
unsigned    m_nCount = 0;


// Instantiate the GUI
#define MAX_PAGE                1

// Define the maximum number of elements per page
#define MAX_ELEM_PG_MAIN          16                                        // # Elems total
#define MAX_ELEM_PG_MAIN_RAM      MAX_ELEM_PG_MAIN                          // # Elems in RAM

gslc_tsGui                  m_gui;
gslc_tsDriver               m_drv;
gslc_tsFont                 m_asFont[MAX_FONT];
gslc_tsPage                 m_asPage[MAX_PAGE];
gslc_tsElem                 m_asPageElem[MAX_ELEM_PG_MAIN_RAM];
gslc_tsElemRef              m_asPageElemRef[MAX_ELEM_PG_MAIN];

gslc_tsXProgress            m_sXProgress,m_sXProgress1;
gslc_tsXCheckbox            m_asXCheck[3];
gslc_tsXSlider              m_sXSlider;


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
// - Detect a button press
// - In this particular example, we are looking for the Quit button press
//   which is used to terminate the program.
bool CbBtnQuit(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY)
{
  //gslc_tsGui*     pGui      = (gslc_tsGui*)(pvGui);
  //gslc_tsElemRef* pElemRef  = (gslc_tsElemRef*)(pvElemRef);

  if (eTouch == GSLC_TOUCH_UP_IN) {
    m_bQuit = true;
    GSLC_DEBUG_PRINT("Callback: Quit button pressed\n", "");
  }
  return true;
}

// Checkbox / radio callbacks
// - Creating a callback function is optional, but doing so enables you to
//   detect changes in the state of the elements.
bool CbCheckbox(void* pvGui, void* pvElemRef, int16_t nSelId, bool bChecked)
{
  gslc_tsGui*     pGui      = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef  = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem*    pElem     = gslc_GetElemFromRef(pGui,pElemRef);
  if (pElemRef == NULL) {
    return false;
  }

  // Determine which element issued the callback
  switch (pElem->nId) {
    case E_ELEM_CHECK1:
      GSLC_DEBUG_PRINT("Callback: Check[ID=%d] state=%u\n", pElem->nId,bChecked);
      break;
    case E_ELEM_RADIO1:
    case E_ELEM_RADIO2:
      // For the radio buttons, determine which ID is currently selected (nSelId)
      // - Note that this may not always be the same as the element that
      //   issued the callback (pElem->nId)
      // - A return value of GSLC_ID_NONE indicates that no radio buttons
      //   in the group are currently selected
      if (nSelId == GSLC_ID_NONE) {
        GSLC_DEBUG_PRINT("Callback: Radio[ID=NONE] selected\n", "");
      } else {
        GSLC_DEBUG_PRINT("Callback: Radio[ID=%d] selected\n", nSelId);
      }
      break;
    default:
      break;
  } // switch
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

  // Create Quit button with modifiable text label
  static char mstr_quit[8] = "Quit";
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_QUIT,E_PG_MAIN,
    (gslc_tsRect){160,80,80,40},mstr_quit,sizeof(mstr_quit),E_FONT_BTN,&CbBtnQuit);
  gslc_ElemSetRoundEn(&m_gui,pElemRef,true);

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
  pElemRef = gslc_ElemXProgressCreate(&m_gui,E_ELEM_PROGRESS,E_PG_MAIN,&m_sXProgress,
    (gslc_tsRect){80,80,50,10},0,100,0,GSLC_COL_GREEN,false);
  m_pElemProgress = pElemRef; // Save for quick access

  // Second progress bar (vertical)
  // - Demonstration of vertical bar with offset zero-pt showing both positive and negative range
  pElemRef = gslc_ElemXProgressCreate(&m_gui,E_ELEM_PROGRESS1,E_PG_MAIN,&m_sXProgress1,
    (gslc_tsRect){280,80,10,100},-25,75,-15,GSLC_COL_RED,true);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_BLUE_DK3,GSLC_COL_BLACK,GSLC_COL_BLACK);
  m_pElemProgress1 = pElemRef; // Save for quick access


  // Create checkbox 1
  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){20,100,20,20},
    (char*)"Check1:",0,E_FONT_TXT);
  pElemRef = gslc_ElemXCheckboxCreate(&m_gui,E_ELEM_CHECK1,E_PG_MAIN,&m_asXCheck[0],
    (gslc_tsRect){80,100,20,20},false,GSLCX_CHECKBOX_STYLE_X,GSLC_COL_BLUE_LT2,false);
  gslc_ElemXCheckboxSetStateFunc(&m_gui, pElemRef, &CbCheckbox);

  // Create radio 1
  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){20,135,20,20},
    (char*)"Radio1:",0,E_FONT_TXT);
  pElemRef = gslc_ElemXCheckboxCreate(&m_gui,E_ELEM_RADIO1,E_PG_MAIN,&m_asXCheck[1],
    (gslc_tsRect){80,135,20,20},true,GSLCX_CHECKBOX_STYLE_ROUND,GSLC_COL_ORANGE,false);
  gslc_ElemSetGroup(&m_gui,pElemRef,E_GROUP1);
  gslc_ElemXCheckboxSetStateFunc(&m_gui, pElemRef, &CbCheckbox);

  // Create radio 2
  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){20,160,20,20},
    (char*)"Radio2:",0,E_FONT_TXT);
  pElemRef = gslc_ElemXCheckboxCreate(&m_gui,E_ELEM_RADIO2,E_PG_MAIN,&m_asXCheck[2],
    (gslc_tsRect){80,160,20,20},true,GSLCX_CHECKBOX_STYLE_ROUND,GSLC_COL_ORANGE,false);
  gslc_ElemSetGroup(&m_gui,pElemRef,E_GROUP1);
  gslc_ElemXCheckboxSetStateFunc(&m_gui, pElemRef, &CbCheckbox);

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

  // Load Fonts
  if (!gslc_FontSet(&m_gui,E_FONT_BTN,GSLC_FONTREF_PTR,FONT_NAME1,1)) { return; }
  if (!gslc_FontSet(&m_gui,E_FONT_TXT,GSLC_FONTREF_PTR,NULL,1)) { return; }
  // Some display drivers need to set a mode to use the extra fonts
  #if defined(SET_FONT_MODE1)
    gslc_FontSetMode(&m_gui, E_FONT_BTN, GSLC_FONTREF_MODE_1);
  #endif


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

  gslc_ElemXProgressSetVal(&m_gui,m_pElemProgress,((m_nCount/1)%100));

  // NOTE: A more efficient method is to move the following
  //       code into the slider position callback function.
  //       Please see example 07.
  int nPos = gslc_ElemXSliderGetPos(&m_gui,m_pElemSlider);
  snprintf(acTxt,MAX_STR,"%u",nPos);
  gslc_ElemSetTxtStr(&m_gui,m_pElemSliderTxt,acTxt);

  gslc_ElemXProgressSetVal(&m_gui,m_pElemProgress1,(nPos*80.0/100.0)-15);


  // Periodically call GUIslice update function
  gslc_Update(&m_gui);

  // Slow down updates
  delay(10);

  // In an Arduino sketch, one doesn't normally "Quit" the program.
  // For demonstration purposes, we are going to demonstrate
  // changing the Quit button appearance and then stopping the program
  // in an infinite loop.
  if (m_bQuit) {
    // Fetch a reference to the Quit button
    // - This demonstrates how to get an element from its ID (enumeration)
    // - In most cases, it is easier to save an element reference variable
    //   when we call ElemCreate*() on any element that we want to modify later.
    gslc_tsElemRef* pElemRef = gslc_PageFindElemById(&m_gui, E_PG_MAIN, E_ELEM_BTN_QUIT);

    // Change the button text
    gslc_ElemSetTxtStr(&m_gui, pElemRef, (char*)"STOP");
    // Change the button color
    gslc_ElemSetCol(&m_gui, pElemRef, GSLC_COL_RED_LT4, GSLC_COL_RED, GSLC_COL_RED_LT2);

    // Let's stop the program here by calling a final update, Quit and
    // then stall in an infinite loop.
    gslc_Update(&m_gui);
    gslc_Quit(&m_gui);
    while (1) { }
  }
}
