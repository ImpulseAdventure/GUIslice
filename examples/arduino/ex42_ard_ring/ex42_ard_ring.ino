//
// GUIslice Library Examples
// - Calvin Hass
// - https://www.impulseadventure.com/elec/guislice-gui.html
// - https://github.com/ImpulseAdventure/GUIslice
// - Example 42 (Arduino):
//   - Demonstrate ring gauge, controlled by slider
//   - Expected behavior: Clicking on button terminates program
//   - NOTE: This is the simple version of the example without
//     optimizing for memory consumption. A "minimal"
//     version is located in the "arduino_min" folder which includes
//     FLASH memory optimization for reduced memory devices.
//

// ARDUINO NOTES:
// - GUIslice_config.h must be edited to match the pinout connections
//   between the Arduino CPU and the display controller (see ADAGFX_PIN_*).

#include "GUIslice.h"
#include "GUIslice_drv.h"

// Include any extended elements
#include "elem/XSlider.h"
#include "elem/XRingGauge.h"

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

// To limit noisy touchscreen input, add optional filtering
#define FILTER_UPDATES // Comment out to disable filtering
#define FILTER_MODE 3
#define UPDATE_PERIOD 200
#define UPDATE_FLOAT 5


// Defines for resources

// Enumerations for pages, elements, fonts, images
enum { E_PG_MAIN };
enum { E_ELEM_BOX, E_ELEM_BTN_QUIT, E_ELEM_XRING, E_ELEM_SLIDER };
enum { E_FONT_BTN, E_FONT_DIAL, MAX_FONT };

bool    m_bQuit = false;

// Instantiate the GUI
#define MAX_PAGE            1
#define MAX_ELEM_PG_MAIN    4

gslc_tsGui                  m_gui;
gslc_tsDriver               m_drv;
gslc_tsFont                 m_asFont[MAX_FONT];
gslc_tsPage                 m_asPage[MAX_PAGE];
gslc_tsElem                 m_asPageElem[MAX_ELEM_PG_MAIN];
gslc_tsElemRef              m_asPageElemRef[MAX_ELEM_PG_MAIN];

gslc_tsXSlider              m_sXSlider;
gslc_tsXRingGauge           m_sXRingGauge;

// Save some element references for quick access
gslc_tsElemRef*  m_pElemSlider = NULL;
gslc_tsElemRef*  m_pElemXRingGauge = NULL;

int16_t m_nSliderPos = 0;

uint16_t m_nLoops = 0;
int16_t m_nCount = 0;
bool m_bCountUp = true;

// Define debug message function
static int16_t DebugOut(char ch) { Serial.write(ch); return 0; }

uint32_t m_nTimeLast = 0;

// Button callbacks
bool CbBtnQuit(void* pvGui, void *pvElem, gslc_teTouch eTouch, int16_t nX, int16_t nY)
{
  if (eTouch == GSLC_TOUCH_UP_IN) {
    m_bQuit = true;
  }
  return true;
}


void setup()
{
  gslc_tsElemRef* pElemRef = NULL;

  // Initialize debug output
  Serial.begin(9600);
  gslc_InitDebug(&DebugOut);
  //delay(1000);  // NOTE: Some devices require a delay after Serial.begin() before serial port can be used

  // Initialize
  if (!gslc_Init(&m_gui, &m_drv, m_asPage, MAX_PAGE, m_asFont, MAX_FONT)) { return; }

  // Load Fonts
  if (!gslc_FontSet(&m_gui, E_FONT_BTN, GSLC_FONTREF_PTR, NULL, 1)) { return; }
  if (!gslc_FontSet(&m_gui, E_FONT_DIAL, GSLC_FONTREF_PTR, FONT_NAME1, 1)) { return; }
  // Some display drivers need to set a mode to use the extra fonts
  #if defined(SET_FONT_MODE1)
    gslc_FontSetMode(&m_gui, E_FONT_DIAL, GSLC_FONTREF_MODE_1);
  #endif


  // -----------------------------------
  // Create page elements
  gslc_PageAdd(&m_gui, E_PG_MAIN, m_asPageElem, MAX_ELEM_PG_MAIN, m_asPageElemRef, MAX_ELEM_PG_MAIN);

  // Background flat color
  gslc_SetBkgndColor(&m_gui, GSLC_COL_GRAY_DK2);

  // Create background box
  pElemRef = gslc_ElemCreateBox(&m_gui, E_ELEM_BOX, E_PG_MAIN, (gslc_tsRect) { 10, 50, 300, 150 });
  gslc_ElemSetCol(&m_gui, pElemRef, GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);

  // Create Quit button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui, E_ELEM_BTN_QUIT, E_PG_MAIN,
    (gslc_tsRect) { 235, 5, 80, 30 }, (char*)"Quit", 0, E_FONT_BTN, &CbBtnQuit);

  // Create a RingGauge
  static char m_str10[10] = "";
  pElemRef = gslc_ElemXRingGaugeCreate(&m_gui, E_ELEM_XRING, E_PG_MAIN, &m_sXRingGauge,
    (gslc_tsRect) { 80, 80, 100, 100 }, m_str10, 10, E_FONT_DIAL);
  gslc_ElemXRingGaugeSetValRange(&m_gui, pElemRef, 0, 100);
  gslc_ElemXRingGaugeSetVal(&m_gui, pElemRef, 60); // Set initial value
  // The following are some additional config options available
  //gslc_ElemXRingGaugeSetAngleRange(&m_gui, pElemRef, 0, 360, true);    // Full circle 
  //gslc_ElemXRingGaugeSetAngleRange(&m_gui, pElemRef, -90, 180, true);  // Upper Semi-circle
  //gslc_ElemXRingGaugeSetAngleRange(&m_gui, pElemRef, 0, 90, true);     // Top-Right Quarter-circle
  //gslc_ElemXRingGaugeSetAngleRange(&m_gui, pElemRef, -135, 270, true); // Three-Quarter circle
  //gslc_ElemXRingGaugeSetThickness(&m_gui,pElemRef, 8);
  //gslc_ElemXRingGaugeSetQuality(&m_gui,pElemRef, 72);
  //gslc_ElemXRingGaugeSetColorInactive(&m_gui,pElemRef, GSLC_COL_GRAY_DK3);
  //gslc_ElemXRingGaugeSetColorActiveFlat(&m_gui,pElemRef, GSLC_COL_ORANGE);
  //gslc_ElemXRingGaugeSetColorActiveGradient(&m_gui, pElemRef, GSLC_COL_BLUE_LT4, GSLC_COL_RED);
  m_pElemXRingGauge = pElemRef; // Save for quick access

   // Create slider
  pElemRef = gslc_ElemXSliderCreate(&m_gui, E_ELEM_SLIDER, E_PG_MAIN, &m_sXSlider,
    (gslc_tsRect) { 200, 80, 100, 20 }, 0, 100, 60, 5, false);
  gslc_ElemXSliderSetStyle(&m_gui, pElemRef, true, (gslc_tsColor) { 0, 0, 128 }, 10,
    5, (gslc_tsColor) { 64, 64, 64 });
  gslc_ElemXSliderSetPosFunc(&m_gui, pElemRef, &CbSlidePos);
  m_pElemSlider = pElemRef; // Save for quick access


  // -----------------------------------
  // Start up display on main page
  gslc_SetPageCur(&m_gui, E_PG_MAIN);

  m_bQuit = false;

  // Initialize startup state
  m_nSliderPos = gslc_ElemXSliderGetPos(&m_gui, m_pElemSlider);
  m_nTimeLast = millis();
}

// Callback function for when a slider's position has been updated
bool CbSlidePos(void* pvGui, void* pvElemRef, int16_t nPos)
{
  gslc_tsGui*     pGui = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem*    pElem = pElemRef->pElem;
  //gslc_tsXSlider* pSlider = (gslc_tsXSlider*)(pElem->pXData);

  switch (pElem->nId) {
  case E_ELEM_SLIDER:
    m_nSliderPos = gslc_ElemXSliderGetPos(pGui, pElemRef);
    break;
  default:
    break;
  }
  return true;
}



void loop()
{
  char acStr[10];

  // Periodically call GUIslice update function
  gslc_Update(&m_gui);

  // Do we want to filter out some touchscreen inputs?
#if !defined(FILTER_UPDATES)
  // No filtering enabled -- update the XRingGauge immediately

  // Update the XRingGauge position with the slider position
  gslc_ElemXRingGaugeSetVal(&m_gui, m_pElemXRingGauge, m_nSliderPos);

  // Update the XRingGauge text string with a percentage
  snprintf(acStr, 10, "%d%%", m_nSliderPos);
  gslc_ElemSetTxtStr(&m_gui, m_pElemXRingGauge, acStr);

#else

  // Provide some filtering to limit the gauge updates
  // due to noisy touchscreen readings
  bool bDoUpdate = false;
  static int16_t nValLast = -999; // Initialize to invalid value

  // Perform an update if it has been some time since the last
  // update, or if the position has changed moderately
  if ((millis() - m_nTimeLast) >= UPDATE_PERIOD) {
    bDoUpdate = true;
  } else {
    if (abs(m_nSliderPos - nValLast) > UPDATE_FLOAT) {
      bDoUpdate = true;
    }
  }

  // Do we want to update the XRingGauge?
  if (bDoUpdate) {
    // Update the XRingGauge position with the slider position
    gslc_ElemXRingGaugeSetVal(&m_gui, m_pElemXRingGauge, m_nSliderPos);

    // Update the XRingGauge text string with a percentage
    snprintf(acStr, 10, "%d%%", m_nSliderPos);
    gslc_ElemSetTxtStr(&m_gui, m_pElemXRingGauge, acStr);

    // Remember the time and value of this update
    nValLast = m_nSliderPos;
    m_nTimeLast = millis();
  }

#endif // FILTER_UPDATES


  // In a real program, we would detect the button press and take an action.
  // For this Arduino demo, we will pretend to exit by emulating it with an
  // infinite loop. Note that interrupts are not disabled so that any debug
  // messages via Serial have an opportunity to be transmitted.
  if (m_bQuit) {
    gslc_Quit(&m_gui);
    while (1) {}
  }
}
