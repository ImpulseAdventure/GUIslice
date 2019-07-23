//
// GUIslice Library Examples
// - Calvin Hass
// - https://www.impulseadventure.com/elec/guislice-gui.html
// - https://github.com/ImpulseAdventure/GUIslice
// - Example 40 (Arduino): Small controls (low resolution display)
//   - Demonstrates basic elements on a small display (eg. 128x128)
//     suitable for displays such as ST7735
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
// Enumerations for pages, elements, fonts, images
// ------------------------------------------------
enum { E_PG_MAIN };
enum {
  E_BOX1, E_BTN_OK, E_CHECK1, E_PROGRESS1,
  E_PROGRESS2, E_SLIDER1, E_TXT_CNT
};
enum { E_FONT_TXT1, MAX_FONT }; // Use separate enum for fonts, MAX_FONT at end

// ------------------------------------------------
// Instantiate the GUI
// ------------------------------------------------

// Define the maximum number of elements per page
#define MAX_PAGE                1
#define MAX_ELEM_PG_MAIN 9
#define MAX_ELEM_PG_MAIN_RAM MAX_ELEM_PG_MAIN

// GUI Elements
gslc_tsGui                      m_gui;
gslc_tsDriver                   m_drv;
gslc_tsFont                     m_asFont[MAX_FONT];
gslc_tsPage                     m_asPage[MAX_PAGE];

gslc_tsElem                     m_asPage1Elem[MAX_ELEM_PG_MAIN_RAM];
gslc_tsElemRef                  m_asPage1ElemRef[MAX_ELEM_PG_MAIN];
gslc_tsXCheckbox                m_asXCheck[1];
gslc_tsXProgress                m_sXGauge[2];
gslc_tsXSlider                  m_sXSlider[1];

#define MAX_STR                 100

// ------------------------------------------------
// Save some element references for update loop access
// ------------------------------------------------
gslc_tsElemRef*  m_pElemProgress1 = NULL;
gslc_tsElemRef*  m_pElemProgress2 = NULL;
gslc_tsElemRef*  m_pElemSlider1 = NULL;
gslc_tsElemRef*  m_pTxtCnt = NULL;

// ------------------------------------------------
// Program Globals
// ------------------------------------------------

bool m_bQuit = false;
char m_acTxt[10];  // General purpose text string
int32_t m_nCount = 0; // Free-running counter


// Define debug message function
static int16_t DebugOut(char ch) { if (ch == (char)'\n') Serial.println(""); else Serial.write(ch); return 0; }

// ------------------------------------------------
// Callback Methods
// ------------------------------------------------
// Common Button callback
bool CbBtnCommon(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY)
{
  gslc_tsElemRef* pElemRef = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem* pElem = pElemRef->pElem;

  if (eTouch == GSLC_TOUCH_UP_IN) {
    // From the element's ID we can determine which button was pressed.
    switch (pElem->nId) {
    case E_BTN_OK:
		  m_bQuit = true;
      break;
    default:
      break;
    }
  }
  return true;
}

// Callback function for when a slider's position has been updated
bool CbSlidePos(void* pvGui, void* pvElemRef, int16_t nPos)
{
  gslc_tsGui*     pGui = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem*    pElem = pElemRef->pElem;
  int16_t         nVal;

  // From the element's ID we can determine which slider was updated.
  switch (pElem->nId) {
  case E_SLIDER1:
    // Fetch the slider position (0..100)
    nVal = gslc_ElemXSliderGetPos(pGui, pElemRef);
    // Update the right progress bar
    gslc_ElemXProgressSetVal(&m_gui, m_pElemProgress2, nVal);
    break;
  default:
    break;
  }

  return true;
}

// ------------------------------------------------
// Create page elements
// ------------------------------------------------
bool InitGUI()
{
  gslc_tsElemRef* pElemRef = NULL;

  gslc_PageAdd(&m_gui, E_PG_MAIN, m_asPage1Elem, MAX_ELEM_PG_MAIN_RAM, m_asPage1ElemRef, MAX_ELEM_PG_MAIN);

  // Background flat color
  gslc_SetBkgndColor(&m_gui, GSLC_COL_BLUE_DK4);

  // -----------------------------------
  // PAGE: E_PG_MAIN

  pElemRef = gslc_ElemCreateTxt(&m_gui, GSLC_ID_AUTO, E_PG_MAIN, (gslc_tsRect) { 15, 5, 98, 12 },
    (char*)"GUIslice example", 0, E_FONT_TXT1);
  gslc_ElemSetCol(&m_gui, pElemRef, GSLC_COL_GRAY, GSLC_COL_BLUE_DK4, GSLC_COL_BLACK);

  pElemRef = gslc_ElemCreateBox(&m_gui, E_BOX1, E_PG_MAIN, (gslc_tsRect) { 4, 24, 120, 100 });

  // Create slider E_SLIDER1 
  m_pElemSlider1 = gslc_ElemXSliderCreate(&m_gui, E_SLIDER1, E_PG_MAIN, &m_sXSlider[0],
    (gslc_tsRect) { 20, 30, 80, 20 }, 0, 100, 50, 5, false);
  gslc_ElemXSliderSetStyle(&m_gui, m_pElemSlider1, false, GSLC_COL_BLUE, 10, 5, GSLC_COL_BLUE);
  gslc_ElemXSliderSetPosFunc(&m_gui, m_pElemSlider1, &CbSlidePos);

  // Create progress bar E_PROGRESS1 
  m_pElemProgress1 = gslc_ElemXProgressCreate(&m_gui, E_PROGRESS1, E_PG_MAIN, &m_sXGauge[0],
    (gslc_tsRect) { 10, 60, 10, 50 }, 0, 100, 0, GSLC_COL_GREEN, true);

  // Create progress bar E_PROGRESS2 
  m_pElemProgress2 = gslc_ElemXProgressCreate(&m_gui, E_PROGRESS2, E_PG_MAIN, &m_sXGauge[1],
    (gslc_tsRect) { 30, 60, 10, 50 }, 0, 100, 0, GSLC_COL_RED, true);

  pElemRef = gslc_ElemCreateTxt(&m_gui, GSLC_ID_AUTO, E_PG_MAIN, (gslc_tsRect) { 50, 60, 15, 12 },
    (char*)"Cnt:", 0, E_FONT_TXT1);
  gslc_ElemSetTxtCol(&m_gui, pElemRef, GSLC_COL_WHITE);

  // Create E_TXT_CNT modifiable text label
  static char m_strtxt3[7] = "###";
  m_pTxtCnt = gslc_ElemCreateTxt(&m_gui, E_TXT_CNT, E_PG_MAIN, (gslc_tsRect) { 75, 60, 30, 12 },
    (char*)m_strtxt3, 7, E_FONT_TXT1);
  gslc_ElemSetTxtCol(&m_gui, m_pTxtCnt, GSLC_COL_BLUE_LT4);

  // create E_BTN_OK button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui, E_BTN_OK, E_PG_MAIN,
    (gslc_tsRect) { 50, 90, 40, 20 }, (char*)"OK", 0, E_FONT_TXT1, &CbBtnCommon);
  gslc_ElemSetTxtCol(&m_gui, pElemRef, GSLC_COL_WHITE);
  gslc_ElemSetCol(&m_gui, pElemRef, GSLC_COL_BLUE_DK2, GSLC_COL_BLUE_DK4, GSLC_COL_BLUE_DK1);
  gslc_ElemSetFrameEn(&m_gui, pElemRef, true);

  // create checkbox E_CHECK1
  pElemRef = gslc_ElemXCheckboxCreate(&m_gui, E_CHECK1, E_PG_MAIN, &m_asXCheck[0],
    (gslc_tsRect) { 100, 95, 15, 15 }, false, GSLCX_CHECKBOX_STYLE_X, (gslc_tsColor) { 255, 200, 0 }, true);

  return true;
}

void setup()
{
  // ------------------------------------------------
  // Initialize
  // ------------------------------------------------
  Serial.begin(9600);
  // Wait for USB Serial 
  //delay(1000);  // NOTE: Some devices require a delay after Serial.begin() before serial port can be used

  gslc_InitDebug(&DebugOut);

  if (!gslc_Init(&m_gui, &m_drv, m_asPage, MAX_PAGE, m_asFont, MAX_FONT)) { return; }

  // ------------------------------------------------
  // Load Fonts
  // ------------------------------------------------
  if (!gslc_FontSet(&m_gui, E_FONT_TXT1, GSLC_FONTREF_PTR, NULL, 1)) { return; }

  // ------------------------------------------------
  // Create graphic elements
  // ------------------------------------------------
  InitGUI();

  // ------------------------------------------------
  // Start up display on first page
  // ------------------------------------------------
  gslc_SetPageCur(&m_gui, E_PG_MAIN);

}

// -----------------------------------
// Main event loop
// -----------------------------------
void loop()
{

  // ------------------------------------------------
  // Update GUI Elements
  // ------------------------------------------------

  // Increment counter
  m_nCount++;

  // Update left progress bar
  gslc_ElemXProgressSetVal(&m_gui, m_pElemProgress1, m_nCount % 100);

  // Update counter text
  snprintf(m_acTxt, 10, "%d", (m_nCount / 10) % 10000);
  gslc_ElemSetTxtStr(&m_gui, m_pTxtCnt, m_acTxt);

  // ------------------------------------------------
  // Periodically call GUIslice update function
  // ------------------------------------------------
  gslc_Update(&m_gui);

  // Slow down updates
  delay(10);

  // In a real program, we would detect the button press and take an action.
  // For this Arduino demo, we will pretend to exit by emulating it with an
  // infinite loop. Note that interrupts are not disabled so that any debug
  // messages via Serial have an opportunity to be transmitted.
  if (m_bQuit) {
	  gslc_Quit(&m_gui);
	  while (1) {}
  }

}
