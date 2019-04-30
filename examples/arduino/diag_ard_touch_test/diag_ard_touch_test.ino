//
// GUIslice Library Examples
// - Calvin Hass
// - https://www.impulseadventure.com/elec/guislice-gui.html
// - https://github.com/ImpulseAdventure/GUIslice
// - TFT Diagnostic (Arduino): Touch Test
//   - Please refer to the touch configuration guide for details:
//     https://github.com/ImpulseAdventure/GUIslice/wiki/Configure-Touch-Support
//
// ARDUINO NOTES:
// - GUIslice_config must be edited to match the pinout connections
//   between the Arduino CPU and the display controller (eg. ADAGFX_PIN_*).
//

#include "GUIslice.h"
#include "GUIslice_drv.h"

// Ensure touch support has been enabled
#if defined(DRV_TOUCH_NONE)
  #error "Calibration testing requires a touch driver but DRV_TOUCH_NONE was set"
#elif defined(DRV_TOUCH_INPUT)
  #error "Calibration testing requires a touch driver but DRV_TOUCH_INPUT was set"
#elif defined(DRV_TOUCH_M5STACK)
  #error "Calibration testing requires a touch driver but DRV_TOUCH_M5STACK was set"
#endif


// Import optional libraries
#if defined(DRV_DISP_ADAGFX_MCUFRIEND)
  // Import MCUFRIEND to support ID reporting
  #include <MCUFRIEND_kbv.h>
  extern MCUFRIEND_kbv m_disp;
  int16_t m_nMcuFriendId = -1;
#endif



// Defines for resources
enum { E_FONT_TXT };

#define MAX_STR 60

bool  m_bQuit = false; // Quit flag (unused)

// State machine settings
typedef enum {
  STATE_CAL_UNDEF = 0,
  // TEST:
  STATE_CAL_SET,
  STATE_TEST_MSG,
  STATE_TEST_DO,
  STATE_CAL_REDO_MSG,
  STATE_ROTATE_MSG,
  STATE_ROTATE_DO
} teState;
teState m_eState = STATE_CAL_SET;

typedef enum {
  DEBOUNCE_UNDEF = 0,
  DEBOUNCE_UP,
  DEBOUNCE_UP2DOWN,
  DEBOUNCE_DOWN,
  DEBOUNCE_DOWN2UP,
} teDebounce;
teDebounce m_eDebounce = DEBOUNCE_UP;

#define DEBOUNCE_DELAY 50 // Delay (in ms) for debouncing touch events

typedef enum {
  E_ROT_NORMAL = 0,
  E_ROT_REVERSE,
  E_ROT_SWAP,
  E_ROT_NONE,
  E_ROT_UNDEF
} teRotSpecial;
teRotSpecial m_eRotSpecial = E_ROT_UNDEF;



teState     m_eStateOld = STATE_CAL_UNDEF;
bool        m_bStateBegin = true;


// Calibration data
int16_t   m_nTouchXMin;
int16_t   m_nTouchXMax;
int16_t   m_nTouchYMin;
int16_t   m_nTouchYMax;
int16_t   m_nTouchZMin;
int16_t   m_nTouchZMax;

uint16_t  m_nTouchCalXMin;
uint16_t  m_nTouchCalXMax;
uint16_t  m_nTouchCalYMin;
uint16_t  m_nTouchCalYMax;

bool m_bRemapYX = false;

bool m_bTouchCoordValid = false;

char m_acTxt[MAX_STR];

// Define GUI display elements
gslc_tsRect m_rLblX = (gslc_tsRect) { 40, 60, 50, 10 };
gslc_tsRect m_rCurX = (gslc_tsRect) { 100, 60, 30, 10 };
gslc_tsRect m_rRngX = (gslc_tsRect) { 140, 60, 65, 10 };

gslc_tsRect m_rLblY = (gslc_tsRect) { 40, 80, 50, 10 };
gslc_tsRect m_rCurY = (gslc_tsRect) { 100, 80, 30, 10 };
gslc_tsRect m_rRngY = (gslc_tsRect) { 140, 80, 65, 10 };

gslc_tsRect m_rLblZ = (gslc_tsRect) { 40, 100, 50, 10 };
gslc_tsRect m_rCurZ = (gslc_tsRect) { 100, 100, 30, 10 };
gslc_tsRect m_rRngZ = (gslc_tsRect) { 140, 100, 65, 10 };

gslc_tsRect m_rLblStatus = (gslc_tsRect) { 30, 20, 110, 10 };
gslc_tsRect m_rCurStatus = (gslc_tsRect) { 100, 20, 120, 10 };

gslc_tsRect m_rReport = (gslc_tsRect) { 40, 60, 140, 10 };
gslc_tsRect m_rMessage = (gslc_tsRect) { 20, 160, 140, 10 };


// Instantiate the GUI
#define MAX_FONT                1

gslc_tsGui                  m_gui;
gslc_tsDriver               m_drv;
gslc_tsFont                 m_asFont[MAX_FONT];

gslc_tsFont*                m_pFont = NULL;

// Define debug message function
static int16_t DebugOut(char ch) { Serial.write(ch); return 0; }


void ResetMax()
{
  m_nTouchXMin = 9999;
  m_nTouchXMax = 0;
  m_nTouchYMin = 9999;
  m_nTouchYMax = 0;
  m_nTouchZMin = 9999;
  m_nTouchZMax = 0;

  m_bTouchCoordValid = false;
}

void DrawBackground()
{
  gslc_tsRect rBack = (gslc_tsRect) { 0, 0, 0, 0 };
  rBack.w = m_gui.nDispW;
  rBack.h = m_gui.nDispH;
  gslc_DrawFillRect(&m_gui, rBack, GSLC_COL_BLACK);
}


void DrawPageCoords()
{
  gslc_DrvDrawTxt(&m_gui, m_rLblX.x, m_rLblX.y, m_pFont, "X:", GSLC_TXT_DEFAULT, GSLC_COL_YELLOW, GSLC_COL_BLACK);
  gslc_DrvDrawTxt(&m_gui, m_rLblY.x, m_rLblY.y, m_pFont, "Y:", GSLC_TXT_DEFAULT, GSLC_COL_YELLOW, GSLC_COL_BLACK);
  gslc_DrvDrawTxt(&m_gui, m_rLblZ.x, m_rLblZ.y, m_pFont, "Z:", GSLC_TXT_DEFAULT, GSLC_COL_YELLOW, GSLC_COL_BLACK);
}

void DrawPageConfirm()
{
  gslc_DrvDrawTxt(&m_gui, m_rMessage.x, m_rMessage.y, m_pFont, "Confirm red dots match touches", GSLC_TXT_DEFAULT, GSLC_COL_ORANGE, GSLC_COL_BLACK);
}

void RedrawStatus(const char* pStatus)
{
  gslc_DrvDrawTxt(&m_gui, m_rLblStatus.x, m_rLblStatus.y, m_pFont, "Status:", GSLC_TXT_DEFAULT, GSLC_COL_WHITE, GSLC_COL_BLACK);

  gslc_DrawFillRect(&m_gui, m_rCurStatus, GSLC_COL_BLACK);
  gslc_DrvDrawTxt(&m_gui, m_rCurStatus.x, m_rCurStatus.y, m_pFont, pStatus, GSLC_TXT_DEFAULT, GSLC_COL_PURPLE, GSLC_COL_BLACK);
}

void DrawTarget(int16_t nX, int16_t nY, uint16_t nSize, gslc_tsColor nColOut, gslc_tsColor nColIn)
{
  gslc_tsRect rRect;
  rRect.x = nX - nSize;
  rRect.y = nY - nSize;
  rRect.w = nSize * 2;
  rRect.h = nSize * 2;
  gslc_DrawFrameRect(&m_gui, rRect, nColOut);
  gslc_DrawLine(&m_gui, nX - (nSize / 2), nY, nX + (nSize / 2) - 1, nY, nColIn);
  gslc_DrawLine(&m_gui, nX, nY - (nSize / 2), nX, nY + (nSize / 2) - 1, nColIn);
}

// This function updates the coordinate text
// - Background redraw is used to limit the update area
void RedrawCoords(int16_t nTouchX, int16_t nTouchY, uint16_t nTouchZ)
{
  // X
  snprintf(m_acTxt, MAX_STR, (m_bTouchCoordValid) ? "%u" : "-", nTouchX);
  gslc_DrawFillRect(&m_gui, m_rCurX, GSLC_COL_BLACK);
  gslc_DrvDrawTxt(&m_gui, m_rCurX.x, m_rCurX.y, m_pFont, m_acTxt, GSLC_TXT_DEFAULT, GSLC_COL_YELLOW, GSLC_COL_BLACK);
  snprintf(m_acTxt, MAX_STR, (m_bTouchCoordValid) ? "%u..%u" : "-", m_nTouchXMin, m_nTouchXMax);
  gslc_DrawFillRect(&m_gui, m_rRngX, GSLC_COL_BLACK);
  gslc_DrvDrawTxt(&m_gui, m_rRngX.x, m_rRngX.y, m_pFont, m_acTxt, GSLC_TXT_DEFAULT, GSLC_COL_BLUE, GSLC_COL_BLACK);

  // Y
  snprintf(m_acTxt, MAX_STR, (m_bTouchCoordValid) ? "%u" : "-", nTouchY);
  gslc_DrawFillRect(&m_gui, m_rCurY, GSLC_COL_BLACK);
  gslc_DrvDrawTxt(&m_gui, m_rCurY.x, m_rCurY.y, m_pFont, m_acTxt, GSLC_TXT_DEFAULT, GSLC_COL_YELLOW, GSLC_COL_BLACK);
  snprintf(m_acTxt, MAX_STR, (m_bTouchCoordValid) ? "%u..%u" : "-", m_nTouchYMin, m_nTouchYMax);
  gslc_DrawFillRect(&m_gui, m_rRngY, GSLC_COL_BLACK);
  gslc_DrvDrawTxt(&m_gui, m_rRngY.x, m_rRngY.y, m_pFont, m_acTxt, GSLC_TXT_DEFAULT, GSLC_COL_BLUE, GSLC_COL_BLACK);

  // Z
  snprintf(m_acTxt, MAX_STR, (m_bTouchCoordValid) ? "%u" : "-", nTouchZ);
  gslc_DrawFillRect(&m_gui, m_rCurZ, GSLC_COL_BLACK);
  gslc_DrvDrawTxt(&m_gui, m_rCurZ.x, m_rCurZ.y, m_pFont, m_acTxt, GSLC_TXT_DEFAULT, GSLC_COL_YELLOW, GSLC_COL_BLACK);
  snprintf(m_acTxt, MAX_STR, (m_bTouchCoordValid) ? "%u..%u" : "-", m_nTouchZMin, m_nTouchZMax);
  gslc_DrawFillRect(&m_gui, m_rRngZ, GSLC_COL_BLACK);
  gslc_DrvDrawTxt(&m_gui, m_rRngZ.x, m_rRngZ.y, m_pFont, m_acTxt, GSLC_TXT_DEFAULT, GSLC_COL_BLUE, GSLC_COL_BLACK);
}


void DrawTracer(int16_t nTouchX, int16_t nTouchY)
{
  // Render marker
  gslc_DrawSetPixel(&m_gui, nTouchX, nTouchY, GSLC_COL_RED);
}


void CalcMaxCoords(int16_t nTouchX, int16_t nTouchY, uint16_t nTouchZ)
{
  // Capture maximums (for current step)
  m_nTouchXMin = (nTouchX < m_nTouchXMin) ? nTouchX : m_nTouchXMin;
  m_nTouchXMax = (nTouchX > m_nTouchXMax) ? nTouchX : m_nTouchXMax;
  m_nTouchYMin = (nTouchY < m_nTouchYMin) ? nTouchY : m_nTouchYMin;
  m_nTouchYMax = (nTouchY > m_nTouchYMax) ? nTouchY : m_nTouchYMax;
  m_nTouchZMin = ((int16_t)nTouchZ < m_nTouchZMin) ? (int16_t)nTouchZ : m_nTouchZMin;
  m_nTouchZMax = ((int16_t)nTouchZ > m_nTouchZMax) ? (int16_t)nTouchZ : m_nTouchZMax;
}

// Take raw input down / up events and update with debounced versions
void DoDebounce(bool bRawDown, bool bRawUp, bool &bFiltDown, bool &bFiltUp)
{
  static uint32_t nTmStart = 0; // Timestamp of last transition event
  bool bHeld = (millis() - nTmStart) > DEBOUNCE_DELAY;

  switch (m_eDebounce) {
  case DEBOUNCE_UP:
    if (bRawDown) {
      nTmStart = millis(); // Latch timestamp for start of transition
      m_eDebounce = DEBOUNCE_UP2DOWN;
    } else if (bRawUp) {
      // Don't provide any touch events while continuing to be up
    }
    break;
  case DEBOUNCE_UP2DOWN:
    if (bRawUp) {
      m_eDebounce = DEBOUNCE_UP;
      #ifdef DBG_TOUCH
      GSLC_DEBUG_PRINT("DBG: Touch bounce (up-down-up) detected\n", "");
      #endif
    } else if (bHeld) {
      m_eDebounce = DEBOUNCE_DOWN; // Transition confirmed
      bFiltDown = true;
    }
    break;
  case DEBOUNCE_DOWN:
    if (bRawUp) {
      m_eDebounce = DEBOUNCE_DOWN2UP;
      nTmStart = millis(); // Latch timestamp for start of transition
    } else if (bRawDown) {
      // Provide continuous touch events while continuing to be down
      bFiltDown = true;
    }
    break;
  case DEBOUNCE_DOWN2UP:
    if (bRawDown) {
      m_eDebounce = DEBOUNCE_DOWN;
      #ifdef DBG_TOUCH
      GSLC_DEBUG_PRINT("DBG: Touch bounce (down-up-down) detected\n", "");
      #endif
    } else if (bHeld) {
      m_eDebounce = DEBOUNCE_UP; // Transition confirmed
      bFiltUp = true;
      nTmStart = millis(); // Latch timestamp for start of transition
    }
    break;
  default:
    break;
  }

}

// Implement State Machine
void DoFsm(bool bTouchDown, bool bTouchUp, int16_t nTouchX, int16_t nTouchY, uint16_t nTouchZ)
{
  // Save state
  m_eStateOld = m_eState;


  // Handle state machine
  switch (m_eState) {


  case STATE_CAL_SET:
    GSLC_DEBUG_PRINT("\nCALIB: Apply calibration\n", "");

    // Assign the new touch calibration
    gslc_SetTouchRemapCal(&m_gui, m_nTouchCalXMin, m_nTouchCalXMax, m_nTouchCalYMin, m_nTouchCalYMax);
    gslc_SetTouchRemapEn(&m_gui, true);
    gslc_SetTouchRemapYX(&m_gui, m_bRemapYX);

    m_eState = STATE_TEST_MSG;
    break;

  case STATE_TEST_MSG:
    // Reset the background
    DrawBackground();
    DrawPageCoords();
    DrawPageConfirm();

    snprintf(m_acTxt, MAX_STR, "Test Rotate=%u", m_gui.nRotation);
    RedrawStatus(m_acTxt);

    // Clear stats
    ResetMax();
    m_bTouchCoordValid = false;
    RedrawCoords(0, 0, 0);

    m_eState = STATE_TEST_DO;
    break;

  case STATE_TEST_DO:
    if (bTouchDown) {
      CalcMaxCoords(nTouchX, nTouchY, nTouchZ);
      RedrawCoords(nTouchX, nTouchY, nTouchZ);
      DrawTracer(nTouchX, nTouchY);
    }
    else if (bTouchUp) {
      m_eState = STATE_ROTATE_MSG;
    }
    break;

  case STATE_CAL_REDO_MSG:
    break;

  case STATE_ROTATE_MSG:
    m_eState = STATE_ROTATE_DO;
    break;

  case STATE_ROTATE_DO:
    ResetMax();

    // Increment the current rotation by one step
    gslc_GuiRotate(&m_gui, (m_gui.nRotation + 1) % 4);

    m_eState = STATE_TEST_MSG;
    break;

  default:
    break;
  } // case(m_eState)

  // Are we going to enter a new state?
  // - This is used to limit certain behavior to state entry
  m_bStateBegin = (m_eState != m_eStateOld);
}


void setup()
{
  // Initialize debug output
  Serial.begin(9600);
  gslc_InitDebug(&DebugOut);
  //delay(1000);  // NOTE: Some devices require a delay after Serial.begin() before serial port can be used

  // Initialize
  // - Basic init without support for GUI elements
  if (!gslc_Init(&m_gui, &m_drv, NULL, 0, m_asFont, MAX_FONT)) { return; }

  // Use default font
  if (!gslc_FontAdd(&m_gui, E_FONT_TXT, GSLC_FONTREF_PTR, NULL, 1)) { return; }
  m_pFont = gslc_FontGet(&m_gui, E_FONT_TXT);

  #if defined(DRV_DISP_ADAGFX_MCUFRIEND)
    // For MCUFRIEND displays, detect the ID
    m_nMcuFriendId = m_disp.readID();
  #endif // DRV_DISP_ADAGFX_MCUFRIEND

  // Force orientation to native rotation for calibration purposes
  gslc_GuiRotate(&m_gui, 0);

  // Initialize the display
  DrawBackground();

  // Initialize settings and report
  #if defined(DO_CALIB)
    GSLC_DEBUG_PRINT("\n=== Touch Calibration ===\n\n", "");
  #else
    GSLC_DEBUG_PRINT("\n=== Touch Testing ===\n\n", "");
  #endif

  #if defined(DRV_TOUCH_TYPE_RES)
    // Reset to calibration defaults from configuration
    m_nTouchCalXMin = ADATOUCH_X_MIN;
    m_nTouchCalXMax = ADATOUCH_X_MAX;
    m_nTouchCalYMin = ADATOUCH_Y_MIN;
    m_nTouchCalYMax = ADATOUCH_Y_MAX;
    #if defined(ADATOUCH_REMAP_YX)
      m_bRemapYX = ADATOUCH_REMAP_YX;
    #else
      m_bRemapYX = false;
    #endif
    GSLC_DEBUG_PRINT("CALIB: Config defaults: XMin=%u XMax=%u YMin=%u YMax=%u RemapYX=%u\n",
      ADATOUCH_X_MIN, ADATOUCH_X_MAX, ADATOUCH_Y_MIN, ADATOUCH_Y_MAX,m_bRemapYX);

    #if defined(DRV_DISP_ADAGFX_MCUFRIEND)
      // For MCUFRIEND displays, report the ID
      snprintf(m_acTxt, MAX_STR, "- MCUFRIEND ID=0x%04X", m_nMcuFriendId);
      GSLC_DEBUG_PRINT("%s ", m_acTxt);
    #endif

  #endif // DRV_TOUCH_TYPE_RES
  GSLC_DEBUG_PRINT("\n", "");
}

void loop()
{
  int16_t               nTouchX = 0;
  int16_t               nTouchY = 0;
  uint16_t              nTouchZ = 0;
  bool                  bTouchEvent = false;
  gslc_teInputRawEvent  eInputEvent = GSLC_INPUT_NONE;
  int16_t               nInputVal = 0;

  // Perform touch tracking
  bTouchEvent = gslc_GetTouch(&m_gui, &nTouchX, &nTouchY, &nTouchZ, &eInputEvent, &nInputVal);

  if (eInputEvent != GSLC_INPUT_TOUCH) {
    // Just in case we received another type of event
    bTouchEvent = false;
  }
  else {
    // Set the flag indicating that we have at least one
    // valid reading. This just affects our display.
    m_bTouchCoordValid = true;
  }

  // Detect transition events
  bool bTouchDown = (bTouchEvent && (nTouchZ > 0));
  bool bTouchUp = (bTouchEvent && (nTouchZ == 0));
  bool bFiltDown = false;
  bool bFiltUp = false;

  // Perform debouncing
  DoDebounce(bTouchDown, bTouchUp, bFiltDown, bFiltUp);

  // Step through the FSM
  DoFsm(bFiltDown, bFiltUp, nTouchX, nTouchY, nTouchZ);

  // In a real program, we would detect the button press and take an action.
  // For this Arduino demo, we will pretend to exit by emulating it with an
  // infinite loop. Note that interrupts are not disabled so that any debug
  // messages via Serial have an opportunity to be transmitted.
  if (m_bQuit) {
    gslc_Quit(&m_gui);
    while (1) {}
  }
}