//
// GUIslice Library Examples
// - Calvin Hass
// - https://www.impulseadventure.com/elec/guislice-gui.html
// - https://github.com/ImpulseAdventure/GUIslice
// - TFT Diagnostic (Arduino): Test Touch Pin Detection
//   - This sketch is intended to detect and report the set of pins
//     currently wired to a 4-wire resistive touch display
//   - This sketch is only intended to run in DRV_TOUCH_ADA_SIMPLE mode
//   - The pin detection logic is not supported on ARM or ESP32 devices
//   - Please refer to the touch configuration guide for details:
//     https://github.com/ImpulseAdventure/GUIslice/wiki/Configure-Touch-Support
//
// ACKNOWLEDGEMENT:
// - The pin detection logic was adapted from David Prentice's mcufriend_kbv library:
//   https://github.com/prenticedavid/MCUFRIEND_kbv/blob/master/examples/diagnose_Touchpins/diagnose_Touchpins.ino
//

#include "GUIslice.h"
#include "GUIslice_drv.h"

// Ensure debug messaging is enabled
#if (DEBUG_ERR != 1)
#undef  DEBUG_ERR
#define DEBUG_ERR 1
#endif

#if defined(DRV_TOUCH_NONE)
  #error "Touch pin detection requires a touch driver but DRV_TOUCH_NONE was set"
#elif !defined(DRV_TOUCH_ADA_SIMPLE)
  #error "Only DRV_TOUCH_ADA_SIMPLE supports pin detection"
#endif

#if defined(__arm__) || defined(ESP32) || defined(ESP8266)
#error "Pin detection not supported on ARM or ESP platforms"
#endif


#if defined(DRV_DISP_ADAGFX_MCUFRIEND)
  // Import MCUFRIEND to support ID reporting
  #include <MCUFRIEND_kbv.h>
  extern MCUFRIEND_kbv m_disp;
  int m_nMcuFriendId = -1;
#endif

#define MAX_STR             30

// Defines for resources
enum { E_FONT_TXT };

// Instantiate the GUI
#define MAX_FONT                1

gslc_tsGui                  m_gui;
gslc_tsDriver               m_drv;
gslc_tsFont                 m_asFont[MAX_FONT];


char m_acTxt[MAX_STR];
gslc_tsRect m_rStatus = (gslc_tsRect) { 40, 60, 50, 10 };
gslc_tsRect m_rReport = (gslc_tsRect) { 40, 80, 140, 10 };
gslc_tsRect m_rError = (gslc_tsRect) { 40, 120, 140, 10 };
gslc_tsFont* m_pFont = NULL;

#define MAX_FONT                1

// Define debug message function
static int16_t DebugOut(char ch) { Serial.write(ch); return 0; }

void DrawBackground()
{
  // Background
  gslc_tsRect rBack = (gslc_tsRect) { 0, 0, 0, 0 };
  rBack.w = m_gui.nDispW;
  rBack.h = m_gui.nDispH;
  gslc_DrawFillRect(&m_gui, rBack, GSLC_COL_BLACK);

  // Status
  gslc_DrvDrawTxt(&m_gui, m_rStatus.x, m_rStatus.y, m_pFont, "Detecting Pins", GSLC_TXT_DEFAULT, GSLC_COL_YELLOW, GSLC_COL_BLACK);
  gslc_DrvDrawTxt(&m_gui, m_rReport.x, m_rReport.y, m_pFont, "See Serial Monitor output...", GSLC_TXT_DEFAULT, GSLC_COL_GREEN, GSLC_COL_BLACK);
}

void DetectId()
{
  #if defined(DRV_DISP_ADAGFX_MCUFRIEND)
  // For MCUFRIEND displays, detect the ID
  m_nMcuFriendId = m_disp.readID();
  #endif // DRV_DISP_ADAGFX_MCUFRIEND
}

bool DetectPins()
{
  int nPinXm = -1;
  int nPinXp = -1;
  int nPinYp = -1;
  int nPinYm = -1;

  int i, j, value, Apins[2], Dpins[2], Values[2], found = 0;
  bool bRet = false;

  GSLC_DEBUG_PRINT("Making all control and bus pins INPUT_PULLUP\n", "");
  GSLC_DEBUG_PRINT("Typical 30k Analog pullup with corresponding pin\n", "");
  GSLC_DEBUG_PRINT("would read low when digital is written LOW\n", "");
  GSLC_DEBUG_PRINT("e.g. reads ~25 for 300R X direction\n", "");
  GSLC_DEBUG_PRINT("e.g. reads ~30 for 500R Y direction\n", "");
  GSLC_DEBUG_PRINT("\n", "");
  GSLC_DEBUG_PRINT("Starting detection:\n", "");

  for (i = A0; i < A5; i++) pinMode(i, INPUT_PULLUP);
  for (i = 2; i < 10; i++) pinMode(i, INPUT_PULLUP);
  for (i = A0; i < A4; i++) {
    for (j = 5; j < 10; j++) {
      pinMode(j, OUTPUT);
      digitalWrite(j, LOW);
      value = analogRead(i);   // ignore first reading
      value = analogRead(i);
      if (value < 100 && value > 0) {
        GSLC_DEBUG_PRINT(" Detected: (A%u, D%u) = %u\n", (i - A0), j, value);
        if (found < 2) {
          Apins[found] = i;
          Dpins[found] = j;
          Values[found] = value;
        }
        found++;
      }
      pinMode(j, INPUT_PULLUP);
    }
    pinMode(i, INPUT_PULLUP);
  }

  if (found == 2) {
    if (Values[0] < Values[1]) {
      nPinXm = Apins[0];
      nPinXp = Dpins[0];
    }
    else {
      nPinYp = Apins[0];
      nPinYm = Dpins[0];
    }
    if (Values[1] < Values[0]) {
      nPinXm = Apins[1];
      nPinXp = Dpins[1];
    }
    else {
      nPinYp = Apins[1];
      nPinYm = Dpins[1];
    }
    GSLC_DEBUG_PRINT("Final detection: YP=A%u XM=A%u YM=%u XP=%u\n", (nPinYp - A0), (nPinXm - A0), nPinYm, nPinXp);
    GSLC_DEBUG_PRINT("\n\n", "");


    GSLC_DEBUG_PRINT("-----------------------------------------------\n", "");
    GSLC_DEBUG_PRINT("Recommended config settings:\n", "");
    GSLC_DEBUG_PRINT("- Please copy these values over any existing ADATOUCH_PIN_* settings in your config (SECTION 4A).\n\n", "");

    GSLC_DEBUG_PRINT("  // Pinout for DRV_TOUCH_SIMPLE 4-wire resistive touchscreen\n", "");
    #if defined(DRV_DISP_ADAGFX_MCUFRIEND)
      // For MCUFRIEND displays, report the ID
      snprintf(m_acTxt, MAX_STR, "(MCUFRIEND ID=0x%04X)", m_nMcuFriendId);
      GSLC_DEBUG_PRINT("  // %s\n", m_acTxt);
    #endif
    GSLC_DEBUG_PRINT("  #define ADATOUCH_PIN_YP   A%d    // \"Y+\": Must be an analog pin\n", (nPinYp - A0));
    GSLC_DEBUG_PRINT("  #define ADATOUCH_PIN_XM   A%d    // \"X-\": Must be an analog pin\n", (nPinXm - A0));
    GSLC_DEBUG_PRINT("  #define ADATOUCH_PIN_YM   %d     // \"Y-\": Can be a digital pin\n", nPinYm);
    GSLC_DEBUG_PRINT("  #define ADATOUCH_PIN_XP   %d     // \"X+\": Can be a digital pin\n", nPinXp);

    GSLC_DEBUG_PRINT("-----------------------------------------------\n", "");
    GSLC_DEBUG_PRINT("Current config settings:\n", "");
    GSLC_DEBUG_PRINT("- YP=A%u XM=A%u YM=%u XP=%u\n",
      (ADATOUCH_PIN_YP - A0), (ADATOUCH_PIN_XM - A0), ADATOUCH_PIN_YM, ADATOUCH_PIN_XP);
    GSLC_DEBUG_PRINT("-----------------------------------------------\n", "");
    GSLC_DEBUG_PRINT("Current config match recommendation? ", "");
    // Check pins for a match
    if ((nPinXm != ADATOUCH_PIN_XM) || (nPinXp != ADATOUCH_PIN_XP) || \
      (nPinYp != ADATOUCH_PIN_YP) || (nPinYm != ADATOUCH_PIN_YM))
    {
      GSLC_DEBUG_PRINT("MISMATCH - Please update config\n", "");
      bRet = true;
    }
    else {
      GSLC_DEBUG_PRINT("OK - No need to update config\n", "");
      bRet = true;
    }
  }
  else {
    GSLC_DEBUG_PRINT("\nERROR - Touchscreen is probably broken (or wrong DRV_TOUCH_* touch driver\n", "");
    bRet = false;
  }

  GSLC_DEBUG_PRINT("-----------------------------------------------\n\n", "");
  return bRet;
}


void setup()
{
  bool bDetectPinsOk = false;

  // Initialize debug output
  Serial.begin(9600);
  gslc_InitDebug(&DebugOut);
  //delay(1000);  // NOTE: Some devices require a delay after Serial.begin() before serial port can be used

  GSLC_DEBUG_PRINT("\n=== Touch Pin Detection ===\n\n", "");

  // Detect pins before we initialize the display driver
  DetectId();
  bDetectPinsOk = DetectPins();

  // mcufriend displays need re-init after detect
  #if defined(DRV_DISP_ADAGFX_MCUFRIEND)
    m_disp.begin(m_nMcuFriendId);
  #endif // DRV_DISP_ADAGFX_MCUFRIEND

  // Initialize (with no support for GUI elements)
  if (!gslc_Init(&m_gui, &m_drv, NULL, 0, m_asFont, MAX_FONT)) { return; }

  // Use default font
  if (!gslc_FontAdd(&m_gui, E_FONT_TXT, GSLC_FONTREF_PTR, NULL, 1)) { return; }

  m_pFont = gslc_FontGet(&m_gui, E_FONT_TXT);

  DrawBackground();

  // Alert the user visually if we failed to detect the pins
  if (!bDetectPinsOk) {
    gslc_DrvDrawTxt(&m_gui, m_rError.x, m_rError.y, m_pFont, "*** Broken Touchscreen ***", GSLC_TXT_DEFAULT, GSLC_COL_RED, GSLC_COL_BLACK);
  }
}

void loop()
{

}