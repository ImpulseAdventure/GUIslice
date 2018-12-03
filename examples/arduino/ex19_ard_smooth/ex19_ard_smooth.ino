//
// GUIslice Library Examples
// - Calvin Hass
// - https://www.impulseadventure.com/elec/guislice-gui.html
// - https://github.com/ImpulseAdventure/GUIslice
// - Example 19 (ESP8266/ESP32):
//   - Accept touch input, text button, text box
//   - Antialiased Fonts
//   - NOTE: This is Example demonstrates text antialiasing with the TFT_eSPI library.
//     The font which can be found in the TFT_eSPI source in the libraries
//     examples/Smooth Fonts/Print_Smooth_Font/data folder has to be uploaded to SPIFFS first.
//     Instructions from Bodmer/TFT_eSPI:
//       ---
//       The fonts used are in the sketch data folder, press Ctrl+K to view.
//       Upload the fonts and icons to SPIFFS (must set at least 1M for SPIFFS) using the
//       "Tools"  "ESP8266 (or ESP32) Sketch Data Upload" menu option in the IDE.
//       To add this option follow instructions here for the ESP8266:
//       https://github.com/esp8266/arduino-esp8266fs-plugin
//       or for the ESP32:
//       https://github.com/me-no-dev/arduino-esp32fs-plugin
//       Close the IDE and open again to see the new menu option.
//       ---
//

// Font file is stored in SPIFFS

#if !defined(ESP8266) && !defined(ESP32)
  #error "This example only works on ESP8266 / ESP32"
#endif


// Define FS_NO_GLOBALS ahead of FS include to enable
// both SPIFFS and SD access.
#define FS_NO_GLOBALS
#include <FS.h>

#if defined(ESP32)
  #include <SPIFFS.h>
#endif

#include <stdint.h>
#include "GUIslice.h"
#include "GUIslice_ex.h"
#include "GUIslice_drv.h"

// Defines for resources

// Enumerations for pages, elements, fonts, images
enum {E_PG_MAIN};
enum {E_ELEM_BOX, E_ELEM_BTN_QUIT1, E_ELEM_BTN_QUIT2, E_ELEM_TEXT};
enum {E_FONT_BTN, E_FONT_AATEXT};

bool    m_bQuit = false;

// Instantiate the GUI
#define MAX_PAGE            1
#define MAX_FONT            2
#define MAX_ELEM_PG_MAIN    4

gslc_tsGui                  m_gui;
gslc_tsDriver               m_drv;
gslc_tsFont                 m_asFont[MAX_FONT];
gslc_tsPage                 m_asPage[MAX_PAGE];
gslc_tsElem                 m_asPageElem[MAX_ELEM_PG_MAIN];
gslc_tsElemRef              m_asPageElemRef[MAX_ELEM_PG_MAIN];

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

void setup()
{

  bool            bOk = true;
  gslc_tsElemRef* pElemRef = NULL;

  // Initialize debug output
  Serial.begin(9600);
  gslc_InitDebug(&DebugOut);
  //delay(1000);  // NOTE: Some devices require a delay after Serial.begin() before serial port can be used

  // Init SPIFFS for Font Loading
  if (!SPIFFS.begin()) {
    Serial.println("SPIFFS initialisation failed!");
    while (1) yield(); // Stay here twiddling thumbs waiting
  }

  // Initialize
  if (!gslc_Init(&m_gui,&m_drv,m_asPage,MAX_PAGE,m_asFont,MAX_FONT)) { return; }

  // Load Fonts
  if (!gslc_FontAdd(&m_gui,E_FONT_BTN,GSLC_FONTREF_PTR,NULL,3)) { return; }
  if (!gslc_FontAdd(&m_gui,E_FONT_AATEXT,GSLC_FONTREF_FNAME,"Final-Frontier-28",28)) { return; }

  // -----------------------------------
  // Create page elements
  gslc_PageAdd(&m_gui,E_PG_MAIN,m_asPageElem,MAX_ELEM_PG_MAIN,m_asPageElemRef,MAX_ELEM_PG_MAIN);

  // Background flat color
  gslc_SetBkgndColor(&m_gui,GSLC_COL_GRAY_DK2);

  // Create background box
  pElemRef = gslc_ElemCreateBox(&m_gui,E_ELEM_BOX,E_PG_MAIN,(gslc_tsRect){10,20,300,200});
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_WHITE,GSLC_COL_BLACK,GSLC_COL_BLACK);

  // Create Quit button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_QUIT1,E_PG_MAIN,
    (gslc_tsRect){120,40,80,40},(char*)"Quit",0,E_FONT_BTN,&CbBtnQuit);

  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_QUIT2,E_PG_MAIN,
    (gslc_tsRect){120,100,80,40},(char*)"Quit",0,E_FONT_AATEXT,&CbBtnQuit);

  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TEXT,E_PG_MAIN,
    (gslc_tsRect){30,160,260,40},(char*)"Antialias Demo Text",0,E_FONT_AATEXT);



  // -----------------------------------
  // Start up display on main page
  gslc_SetPageCur(&m_gui,E_PG_MAIN);

  m_bQuit = false;
}

void loop()
{
  // Periodically call GUIslice update function
  gslc_Update(&m_gui);

  // In a real program, we would detect the button press and take an action.
  // For this Arduino demo, we will pretend to exit by emulating it with an
  // infinite loop. Note that interrupts are not disabled so that any debug
  // messages via Serial have an opportunity to be transmitted.
  if (m_bQuit) {
    gslc_Quit(&m_gui);
    while (1) { }
  }
}
