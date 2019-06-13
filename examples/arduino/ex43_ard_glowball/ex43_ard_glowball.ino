//
// GUIslice Library Examples
// - Calvin Hass
// - https://www.impulseadventure.com/elec/guislice-gui.html
// - https://github.com/ImpulseAdventure/GUIslice
// - Example 43 (Arduino):
//   - Demonstrate XGlowball gauge, controlled by slider
//

// ARDUINO NOTES:
// - GUIslice_config.h must be edited to match the pinout connections
//   between the Arduino CPU and the display controller (see ADAGFX_PIN_*).

#include "GUIslice.h"
#include "GUIslice_drv.h"

// Include any extended elements
#include "elem/XSlider.h"
#include "elem/XGlowball.h"

// Defines for resources

// Enumerations for pages, elements, fonts, images
enum { E_PG_MAIN };
enum { E_ELEM_BOX, E_ELEM_BTN_QUIT, E_ELEM_XGLOW, E_ELEM_SLIDER };
enum { E_FONT_BTN, MAX_FONT };

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
gslc_tsXGlowball            m_sXGlowball;

// Save some element references for quick access
gslc_tsElemRef* m_pElemSlider = NULL;
gslc_tsElemRef* m_pElemXGlowball = NULL;


// Define debug message function
static int16_t DebugOut(char ch) { Serial.write(ch); return 0; }

// Program global variables
int16_t m_nSliderPos = 0;



// Define the XGlowball appearance
// The tsXGlowballRing array defines each ring in the Glowball:
// - Inner radius
// - Outer radius
// - Color of ring
// Note that rings should be defined from inner to outer.
// It is valid to leave gaps between the rings if desired.
// If the maximum radius is large, then the quality setting
// may need to be increased to ensure the ring has a smooth appearance.
#define NUM_RINGS 9
gslc_tsXGlowballRing asRings[NUM_RINGS] = {
  {0,12,(gslc_tsColor) { 138, 0, 255 }},
  {12,18,(gslc_tsColor) { 0, 12, 255 }},
  {18,24,(gslc_tsColor) { 0, 96, 255 }},
  {24,30,(gslc_tsColor) { 0, 198, 255 }},
  {30,36,(gslc_tsColor) { 0, 255, 150 }},
  {36,42,(gslc_tsColor) { 33, 217, 0 }},
  {42,48,(gslc_tsColor) { 255, 234, 0 }},
  {48,54,(gslc_tsColor) { 255, 152, 0 }},
  {54,60,(gslc_tsColor) { 255, 0, 0 }}
};

// Button callbacks
bool CbBtnQuit(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY)
{
  if (eTouch == GSLC_TOUCH_UP_IN) {
    m_bQuit = true;
  }
  return true;
}


// Callback function for when a slider's position has been updated
bool CbSlidePos(void* pvGui, void* pvElemRef, int16_t nPos)
{
  gslc_tsGui*     pGui     = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem*    pElem    = gslc_GetElemFromRef(pGui,pElemRef);
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

  // Create a Glowball
  pElemRef = gslc_ElemXGlowballCreate(&m_gui, E_ELEM_XGLOW, E_PG_MAIN, &m_sXGlowball,
    120, 120, asRings, NUM_RINGS);
  gslc_ElemXGlowballSetColorBack(&m_gui, pElemRef, GSLC_COL_BLACK);
  gslc_ElemXGlowballSetAngles(&m_gui, pElemRef, 0, 360); // Full-circle (default)
  //gslc_ElemXGlowballSetAngles(&m_gui, pElemRef, 270, 360 + 90); // Semi-circle
  //gslc_ElemXGlowballSetAngles(&m_gui, pElemRef, 0, 90); // Quarter-circle
  m_pElemXGlowball = pElemRef; // Save for quick access


   // Create slider
  pElemRef = gslc_ElemXSliderCreate(&m_gui, E_ELEM_SLIDER, E_PG_MAIN, &m_sXSlider,
    (gslc_tsRect) { 200, 80, 100, 20 }, 0, 9, 0, 5, false);
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
}


void loop()
{

  // Update the XGlowball value with the slider position
  gslc_ElemXGlowballSetVal(&m_gui, m_pElemXGlowball, m_nSliderPos);

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