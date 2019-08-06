//
// GUIslice Library Examples
// - Calvin Hass
// - https://www.impulseadventure.com/elec/guislice-gui.html
// - https://github.com/ImpulseAdventure/GUIslice
// - Example 09 (Arduino):
//     Demonstrate radial and ramp controls
//     NOTE: The ramp control is intended only as a demonstration of
//           a custom control and not intended of use as-is
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
#include "elem/XRadial.h"
#include "elem/XRamp.h"
#include "elem/XSlider.h"


// Defines for resources

// Enumerations for pages, elements, fonts, images
enum { E_PG_MAIN };
enum {
  E_ELEM_BOX, E_ELEM_BTN_QUIT, E_ELEM_COLOR,
  E_RADIAL, E_RAMP, E_SLIDER, E_ELEM_TXT_COUNT
};
enum { E_FONT_BTN, E_FONT_TXT, E_FONT_TITLE, MAX_FONT }; // Use separate enum for fonts, MAX_FONT at end

bool      m_bQuit = false;

// Free-running counter for display
unsigned  m_nCount = 0;

// Instantiate the GUI
#define MAX_PAGE                1

// Define the maximum number of elements per page
#define MAX_ELEM_PG_MAIN          8                 // # Elems total
#define MAX_ELEM_PG_MAIN_RAM      MAX_ELEM_PG_MAIN  // # Elems in RAM

gslc_tsGui                  m_gui;
gslc_tsDriver               m_drv;
gslc_tsFont                 m_asFont[MAX_FONT];
gslc_tsPage                 m_asPage[MAX_PAGE];
gslc_tsElem                 m_asPageElem[MAX_ELEM_PG_MAIN_RAM];   // Storage for all elements in RAM
gslc_tsElemRef              m_asPageElemRef[MAX_ELEM_PG_MAIN];    // References for all elements in GUI

gslc_tsXRadial              m_sXRadial;
gslc_tsXRamp                m_sXRamp;
gslc_tsXSlider              m_sXSlider;

// Current RGB value for color box
// - Globals defined here for convenience so that callback
//   can update R,G,B components independently
uint8_t   m_nPosR = 255;
uint8_t   m_nPosG = 128;
uint8_t   m_nPosB = 0;

// Define debug message function
static int16_t DebugOut(char ch) { Serial.write(ch); return 0; }

// Quit button callback
bool CbBtnQuit(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY)
{
  if (eTouch == GSLC_TOUCH_UP_IN) {
    m_bQuit = true;
  }
  return true;
}

bool CbSlideRadial(void* pvGui, void* pvElemRef, int16_t nPos)
{
  gslc_tsGui*     pGui = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem*    pElem = gslc_GetElemFromRef(pGui, pElemRef);

  char    acTxt[8];
  int16_t nVal;

  // Fetch the new RGB component from the slider
  gslc_tsElemRef* pElemRefTmp = NULL;
  switch (pElem->nId) {
  case E_SLIDER:
    nVal = gslc_ElemXSliderGetPos(pGui, pElemRef);

    // Link slider to the radial control
    pElemRefTmp = gslc_PageFindElemById(pGui, E_PG_MAIN, E_RADIAL);
    gslc_ElemXRadialSetVal(pGui, pElemRefTmp, nVal);

    // Link slider to the ramp control
    pElemRefTmp = gslc_PageFindElemById(pGui, E_PG_MAIN, E_RAMP);
    gslc_ElemXRampSetVal(pGui, pElemRefTmp, nVal);

    // Link slider to the numerical display
    snprintf(acTxt, 8, "%u", nVal);
    pElemRefTmp = gslc_PageFindElemById(pGui, E_PG_MAIN, E_ELEM_TXT_COUNT);
    gslc_ElemSetTxtStr(pGui, pElemRefTmp, acTxt);
    break;

  default:
    break;
  }
  return true;
}


// Create page elements
bool InitOverlays()
{
  gslc_tsElemRef*  pElemRef = NULL;

  gslc_PageAdd(&m_gui, E_PG_MAIN, m_asPageElem, MAX_ELEM_PG_MAIN_RAM, m_asPageElemRef, MAX_ELEM_PG_MAIN);

  // Background flat color
  gslc_SetBkgndColor(&m_gui, GSLC_COL_GRAY_DK2);

  // Create Title with offset shadow
  pElemRef = gslc_ElemCreateTxt(&m_gui, GSLC_ID_AUTO, E_PG_MAIN, (gslc_tsRect) { 2, 2, 320, 50 },
    (char*)"Directional", 0, E_FONT_TITLE);
  gslc_ElemSetTxtCol(&m_gui, pElemRef, (gslc_tsColor) { 32, 32, 60 });
  gslc_ElemSetTxtAlign(&m_gui, pElemRef, GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(&m_gui, pElemRef, false);
  pElemRef = gslc_ElemCreateTxt(&m_gui, GSLC_ID_AUTO, E_PG_MAIN, (gslc_tsRect) { 0, 0, 320, 50 },
    (char*)"Directional", 0, E_FONT_TITLE);
  gslc_ElemSetTxtCol(&m_gui, pElemRef, (gslc_tsColor) { 128, 128, 240 });
  gslc_ElemSetTxtAlign(&m_gui, pElemRef, GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(&m_gui, pElemRef, false);

  // Create background box
  pElemRef = gslc_ElemCreateBox(&m_gui, E_ELEM_BOX, E_PG_MAIN, (gslc_tsRect) { 10, 50, 300, 180 });
  gslc_ElemSetCol(&m_gui, pElemRef, GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);

  pElemRef = gslc_ElemXRadialCreate(&m_gui, E_RADIAL, E_PG_MAIN, &m_sXRadial,
    (gslc_tsRect) { 210, 140, 80, 80 }, 0, 100, 0, GSLC_COL_YELLOW);
  gslc_ElemSetCol(&m_gui, pElemRef, GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);
  gslc_ElemXRadialSetIndicator(&m_gui, pElemRef, GSLC_COL_YELLOW, 30, 3, true);
  gslc_ElemXRadialSetTicks(&m_gui, pElemRef, GSLC_COL_GRAY_LT1, 8, 5);

  pElemRef = gslc_ElemXRampCreate(&m_gui, E_RAMP, E_PG_MAIN, &m_sXRamp,
    (gslc_tsRect) { 80, 140, 100, 80 }, 0, 100, 50, GSLC_COL_YELLOW, false);
  gslc_ElemSetCol(&m_gui, pElemRef, GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);

  pElemRef = gslc_ElemXSliderCreate(&m_gui, E_SLIDER, E_PG_MAIN, &m_sXSlider,
    (gslc_tsRect) { 20, 60, 140, 20 }, 0, 100, 50, 5, false);
  gslc_ElemSetCol(&m_gui, pElemRef, GSLC_COL_GREEN, GSLC_COL_BLACK, GSLC_COL_BLACK);
  gslc_ElemXSliderSetStyle(&m_gui, pElemRef, true, GSLC_COL_GREEN_DK4, 10, 5, GSLC_COL_GRAY_DK2);
  gslc_ElemXSliderSetPosFunc(&m_gui, pElemRef, &CbSlideRadial);

  pElemRef = gslc_ElemCreateTxt(&m_gui, E_ELEM_TXT_COUNT, E_PG_MAIN, (gslc_tsRect) { 180, 60, 40, 20 },
    (char*)"", 0, E_FONT_TXT);

  pElemRef = gslc_ElemCreateBtnTxt(&m_gui, E_ELEM_BTN_QUIT, E_PG_MAIN,
    (gslc_tsRect) { 250, 60, 50, 30 }, (char*)"QUIT", 0, E_FONT_BTN, &CbBtnQuit);
  gslc_ElemSetCol(&m_gui, pElemRef, GSLC_COL_BLUE_DK2, GSLC_COL_BLUE_DK4, GSLC_COL_BLUE_DK1);
  gslc_ElemSetTxtCol(&m_gui, pElemRef, GSLC_COL_WHITE);

  return true;
}


void setup()
{
  // Initialize debug output
  Serial.begin(9600);
  gslc_InitDebug(&DebugOut);
  //delay(1000);  // NOTE: Some devices require a delay after Serial.begin() before serial port can be used

  // Initialize
  if (!gslc_Init(&m_gui, &m_drv, m_asPage, MAX_PAGE, m_asFont, MAX_FONT)) { return; }

  // Load Fonts
  if (!gslc_FontSet(&m_gui, E_FONT_BTN, GSLC_FONTREF_PTR, NULL, 1)) { return; }
  if (!gslc_FontSet(&m_gui, E_FONT_TXT, GSLC_FONTREF_PTR, NULL, 1)) { return; }
  if (!gslc_FontSet(&m_gui, E_FONT_TITLE, GSLC_FONTREF_PTR, NULL, 3)) { return; }

  // Create pages display
  InitOverlays();

  // Start up display on main page
  gslc_SetPageCur(&m_gui, E_PG_MAIN);

  m_bQuit = false;
  return;
}

void loop()
{
  // General counter
  m_nCount++;

  // Periodically call GUIslice update function
  gslc_Update(&m_gui);

  // In a real program, we would detect the button press and take an action.
  // For this Arduino demo, we will pretend to exit by emulating it with an
  // infinite loop. Note that interrupts are not disabled so that any debug
  // messages via Serial have an opportunity to be transmitted.
  if (m_bQuit) {
    gslc_Quit(&m_gui);
    while (1) {}
  }
}

