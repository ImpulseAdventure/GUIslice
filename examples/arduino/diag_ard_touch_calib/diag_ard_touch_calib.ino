//
// GUIslice Library Examples
// - Calvin Hass
// - https://www.impulseadventure.com/elec/guislice-gui.html
// - https://github.com/ImpulseAdventure/GUIslice
// - TFT Diagnostic (Arduino): Touch Calibration
//   - This sketch is intended to run on resistive displays only
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


// ------------------------------------------------------------
// CALIB: Check configuration settings
// ------------------------------------------------------------
#if   defined(DRV_TOUCH_ADA_STMPE610)
#elif defined(DRV_TOUCH_ADA_SIMPLE)
#elif defined(DRV_TOUCH_ADA_RA8875)
#elif defined(DRV_TOUCH_XPT2046_STM)
#elif defined(DRV_TOUCH_XPT2046_PS)
#elif defined(DRV_TOUCH_TFT_ESPI)
#else
  #error "Calibration only supported for resistive touch displays"
#endif // DRV_TOUCH_*
#define DO_CALIB
// ------------------------------------------------------------

// Import optional libraries
#if defined(DRV_DISP_ADAGFX_MCUFRIEND)
  // Import MCUFRIEND to support ID reporting
  #include <MCUFRIEND_kbv.h>
  extern MCUFRIEND_kbv m_disp;
  int16_t m_nMcuFriendId = -1;
#endif

// ------------------------------------------------------------
// CALIB: Additional configuration:
// ------------------------------------------------------------

// Uncomment the following line to display additional debug info
//#define DEBUG_CALIB

// Select method used for averaging
// - Uncomment one of the following lines
#define CALC_BASIC_AVG  // Simple average
//#define CALC_EXP_AVG  // Exponential moving average [not yet supported]

// ------------------------------------------------------------


// Defines for resources
enum { E_FONT_TXT };

#define MAX_STR 60

bool  m_bQuit = false; // Quit flag (unused)

// State machine settings
typedef enum {
  STATE_CAL_UNDEF = 0,
  // CALIB:
  STATE_CAL_START_MSG,
  STATE_CAPT_TL_MSG,
  STATE_CAPT_TL_DO,
  STATE_CAPT_TR_MSG,
  STATE_CAPT_TR_DO,
  STATE_CAPT_BL_MSG,
  STATE_CAPT_BL_DO,
  STATE_CAPT_BR_MSG,
  STATE_CAPT_BR_DO,
  STATE_CAPT_ROT,
  STATE_CAPT_ROT_TL_MSG,
  STATE_CAPT_ROT_TL_DO,
  STATE_CAL_REPORT,
  STATE_CAL_DONE_MSG,
  STATE_CAL_DONE_END,
  // TEST:
  STATE_CAL_SET,
  STATE_TEST_MSG,
  STATE_TEST_DO,
  STATE_CAL_REDO_MSG,
  STATE_ROTATE_MSG,
  STATE_ROTATE_DO
} teState;
teState m_eState = STATE_CAL_START_MSG;

typedef enum {
  DEBOUNCE_UNDEF = 0,
  DEBOUNCE_UP,
  DEBOUNCE_UP2DOWN,
  DEBOUNCE_DOWN,
  DEBOUNCE_DOWN2UP,
} teDebounce;
teDebounce m_eDebounce = DEBOUNCE_UP;

#define DEBOUNCE_DELAY 50 // Delay (in ms) for debouncing touch events

// ------------------------------------------------------------
// CALIB:
// ------------------------------------------------------------
// Definitions for calibration points
typedef enum {
  POINT_TL = 0,
  POINT_TR,
  POINT_BL,
  POINT_BR,
  POINT_TL_ROT,
  POINT_TEST,
  POINT__NUM,
} teCalibPt;
teCalibPt   m_eCalibPt = POINT_TL;

typedef enum {
  E_SEG_LL = 0,
  E_SEG_ML,
  E_SEG_HL,
  E_SEG_LM,
  E_SEG_MM,
  E_SEG_HM,
  E_SEG_LH,
  E_SEG_MH,
  E_SEG_HH
} teCalibSeg;

// ------------------------------------------------------------

typedef enum {
  E_ROT_NORMAL = 0,
  E_ROT_REVERSE,
  E_ROT_SWAP,
  E_ROT_NONE,
  E_ROT_UNDEF
} teRotSpecial;
teRotSpecial m_eRotSpecial = E_ROT_UNDEF;


// ----------------------------------------------------------------------------
// CALIB:
// ----------------------------------------------------------------------------
typedef struct {
  bool          bXLeft;
  bool          bYTop;
  const char*   acStr;
  teState       eStateDo;
  teState       eStateNxt;
} tsCalibStage;

// Define calibration sequence
tsCalibStage m_asCalibStage[5] = {
  {true, true, "Capture TL",STATE_CAPT_TL_DO,STATE_CAPT_TR_MSG},
  {false,true, "Capture TR",STATE_CAPT_TR_DO,STATE_CAPT_BL_MSG},
  {true, false,"Capture BL",STATE_CAPT_BL_DO,STATE_CAPT_BR_MSG},
  {false,false,"Capture BR",STATE_CAPT_BR_DO,STATE_CAPT_ROT},
  {true, true, "Capture TL (Rotated)",STATE_CAPT_ROT_TL_DO,STATE_CAL_REPORT},
};

// Define calibration target display
#define TARGET_MARGIN 10
#define TARGET_SIZE   10

// Storage for touch averaging
int32_t m_nPointBufX = 0;
int32_t m_nPointBufY = 0;
uint16_t m_nPointBufCnt = 0;

// For exponential averaging, define the time constant
// and precision used in the calculation.
#define EXP_AVG_NUMER 6
#define EXP_AVG_DENOM 10
#define EXP_AVG_FIXPT 8

// Define the tolerance in detecting the rotated display
// - Setting a value of 10 means that it will attempt to
//   locate a calibration point within 10% of the rotated click
#define DET_ROT_TOLER_PCT 10

// Define the minimum threshold in Min-Max raw range below
// which we report that the calibration has failed. In most
// displays, a typical range might be 200...3700, implying
// a full-scale range of 3500. One would expect to obtain
// calibration ranges in excess of 10% of the full-scale range.
#define DET_RAW_RANGE_MIN 350

// Define the maximum variance between readings that are
// expected to be on the same side of the display (eg. X:TL/BL)
// Exceeding this value will cause the sketch to report
// that the calibration has failed, likely due to a pin
// wiring issue.
#define DET_RAW_VAR_MAX 350

int16_t m_nPointFinalX = -1;
int16_t m_nPointFinalY = -1;

// Corner readings before adjusting for offsets
uint16_t m_nTouchCalXMinRaw;
uint16_t m_nTouchCalXMaxRaw;
uint16_t m_nTouchCalYMinRaw;
uint16_t m_nTouchCalYMaxRaw;
// Preserve reading ranges
int16_t m_nTouchCalXRngRaw;
int16_t m_nTouchCalYRngRaw;

bool m_bTouchCalFail = false;

int m_anTouchXPeak[POINT__NUM];
int m_anTouchYPeak[POINT__NUM];

// ----------------------------------------------------------------------------

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

// ----------------------------------------------------------------------------
// CALIB:
// ----------------------------------------------------------------------------
void ResetCalib()
{
// Reset the calibration ranges to enable min/max determination
m_nTouchCalXMin = 9999;
m_nTouchCalXMax = 0;
m_nTouchCalYMin = 9999;
m_nTouchCalYMax = 0;

for (int nInd = 0; nInd < POINT__NUM; nInd++) {
  m_anTouchXPeak[nInd] = 0;
  m_anTouchYPeak[nInd] = 0;
}

m_eRotSpecial = E_ROT_UNDEF;
}

void DrawBackgroundStart()
{
  int16_t nPosX = m_rReport.x;
  int16_t nPosY = m_rReport.y;
  gslc_DrvDrawTxt(&m_gui, nPosX, nPosY + 00, m_pFont, "In the following pages, use", GSLC_TXT_DEFAULT, GSLC_COL_WHITE, GSLC_COL_BLACK);
  gslc_DrvDrawTxt(&m_gui, nPosX, nPosY + 10, m_pFont, "a point/stylus to touch the", GSLC_TXT_DEFAULT, GSLC_COL_WHITE, GSLC_COL_BLACK);
  gslc_DrvDrawTxt(&m_gui, nPosX, nPosY + 20, m_pFont, "crosshairs in each corner", GSLC_TXT_DEFAULT, GSLC_COL_WHITE, GSLC_COL_BLACK);
  gslc_DrvDrawTxt(&m_gui, nPosX, nPosY + 30, m_pFont, "as indicated for >3 secs.", GSLC_TXT_DEFAULT, GSLC_COL_WHITE, GSLC_COL_BLACK);
  gslc_DrvDrawTxt(&m_gui, m_rMessage.x, m_rMessage.y, m_pFont, "Click to start", GSLC_TXT_DEFAULT, GSLC_COL_BLUE, GSLC_COL_BLACK);
}

void ResetDatapoints()
{
  m_nPointBufX = 0;
  m_nPointBufY = 0;
  m_nPointBufCnt = 0;
}


void AddDatapoint(int16_t nTouchX, int16_t nTouchY)
{
  #if defined(CALC_EXP_AVG)
  // Exponential moving average
  m_nPointBufX = ((EXP_AVG_NUMER)* m_nPointBufX / EXP_AVG_DENOM) + (EXP_AVG_FIXPT * (EXP_AVG_DENOM - EXP_AVG_NUMER) * nTouchX / EXP_AVG_DENOM);
  m_nPointBufY = ((EXP_AVG_NUMER)* m_nPointBufY / EXP_AVG_DENOM) + (EXP_AVG_FIXPT * (EXP_AVG_DENOM - EXP_AVG_NUMER) * nTouchY / EXP_AVG_DENOM);
  m_nPointBufCnt++;
  #if defined(DEBUG_CALIB)
  snprintf(m_acTxt, MAX_STR, "AddDatapoint(EXP): (%d,%d) => (%ld,%ld)", nTouchX, nTouchY, m_nPointBufX, m_nPointBufY);
  GSLC_DEBUG_PRINT("%s\n", m_acTxt);
  #endif // DEBUG_CALIB
  #elif defined(CALC_BASIC_AVG)
  // Simple averaging
  // - Avoid integer overflow
  if (m_nPointBufCnt < 10000) {
    m_nPointBufX += nTouchX;
    m_nPointBufY += nTouchY;
    m_nPointBufCnt++;
    #if defined(DEBUG_CALIB)
    snprintf(m_acTxt, MAX_STR, "AddDatapoint(BASIC): (%d,%d) => (%ld,%ld,%d)", nTouchX, nTouchY, m_nPointBufX, m_nPointBufY, m_nPointBufCnt);
    GSLC_DEBUG_PRINT("%s\n", m_acTxt);
    #endif // DEBUG_CALIB
  }
  #endif
}

void CalcDatapoint(teCalibPt ePoint)
{
  #if defined(CALC_EXP_AVG)
  m_nPointFinalX = m_nPointBufX / EXP_AVG_FIXPT;
  m_nPointFinalY = m_nPointBufY / EXP_AVG_FIXPT;
  #elif defined(CALC_BASIC_AVG)
  if (m_nPointBufCnt > 0) {
    m_nPointFinalX = m_nPointBufX / m_nPointBufCnt;
    m_nPointFinalY = m_nPointBufY / m_nPointBufCnt;
  }
  #endif  

  // Record the final value in the corresponding touch point
  m_anTouchXPeak[ePoint] = m_nPointFinalX;
  m_anTouchYPeak[ePoint] = m_nPointFinalY;

}

#define SEG_MIN_RANGE 200
bool CalcSegments()
{
  // Algorithm:
  // - 1) Calculate min & max of X & Y coordinates
  // - 2) Segment X & Y ranges into thirds (Low,Mid,High), making
  //      a total of 9 segments when using both X & Y together
  // - 3) Classify each touch press into one of the 9 segments
  // - 4) Determine if all corner segments (LL, HL, LH, HH) have
  //      been detected in the 4 points. If not, error.
  // - 5) Identify the mapping type based on the segment assignments.

  int16_t nXMin = 9999;
  int16_t nXMax = 0;
  int16_t nYMin = 9999;
  int16_t nYMax = 0;

  int16_t nXRng = 1;
  int16_t nYRng = 1;
  int16_t nXEdgeLM = 0;
  int16_t nXEdgeMH = 0;
  int16_t nYEdgeLM = 0;
  int16_t nYEdgeMH = 0;
  teCalibSeg aePointSeg[4];

  // Step 1:
  for (unsigned nInd = POINT_TL; nInd <= POINT_BR; nInd++) {
    nXMin = (m_anTouchXPeak[nInd] < nXMin) ? m_anTouchXPeak[nInd] : nXMin;
    nXMax = (m_anTouchXPeak[nInd] > nXMax) ? m_anTouchXPeak[nInd] : nXMax;
    nYMin = (m_anTouchYPeak[nInd] < nYMin) ? m_anTouchYPeak[nInd] : nYMin;
    nYMax = (m_anTouchYPeak[nInd] > nYMax) ? m_anTouchYPeak[nInd] : nYMax;
  }

  // Step 2:
  nXRng = nXMax - nXMin;
  nYRng = nYMax - nYMin;
  nXEdgeLM = 1*(nXRng / 3);
  nXEdgeMH = 2*(nXRng / 3);
  nYEdgeLM = 1*(nYRng / 3);
  nYEdgeMH = 2*(nYRng / 3);
  // Check for very small ranges
  if ((nXRng < SEG_MIN_RANGE) || (nYRng < SEG_MIN_RANGE)) {
    GSLC_DEBUG_PRINT("ERROR: Small range detected (XRng=%d YRng=%d)\n", nXRng, nYRng);
    return false;
  }

  // Step 3:
  int16_t nX, nY, nSeg;
  for (unsigned nInd = POINT_TL; nInd <= POINT_BR; nInd++) {
    nX = m_anTouchXPeak[nInd];
    nY = m_anTouchYPeak[nInd];
    nSeg = 0;
    // Shortcut math to identify all 9 segs:
    if (nY < nYEdgeLM) {
      nSeg = 0;
    } else if (nY < nYEdgeMH) {
      nSeg = 3;
    } else {
      nSeg = 6;
    }
    if (nX < nXEdgeLM) {
      nSeg += 0;
    } else if (nX < nXEdgeMH) {
      nSeg += 1;
    } else {
      nSeg += 2;
    }
    aePointSeg[nInd] = static_cast<teCalibSeg>(nSeg);
  }

  // Step 4:
  uint8_t nSegsFound = 0;
  for (unsigned nInd = POINT_TL; nInd <= POINT_BR; nInd++) {
    if (aePointSeg[nInd] == E_SEG_LL) {
      nSegsFound |= 1;
    } else if (aePointSeg[nInd] == E_SEG_HL) {
      nSegsFound |= 2;
    } else if (aePointSeg[nInd] == E_SEG_LH) {
      nSegsFound |= 4;
    } else if (aePointSeg[nInd] == E_SEG_HH) {
      nSegsFound |= 8;
    }
  }
  if (nSegsFound != 15) {
    GSLC_DEBUG_PRINT("ERROR: Missing corner segment (status=%u)\n", nSegsFound);
    return false;
  }

  // Step 5:
  teCalibSeg segTL, segTR, segBL, segBR;
  m_bRemapYX = false;
  segTL = aePointSeg[POINT_TL];
  segTR = aePointSeg[POINT_TR];
  segBL = aePointSeg[POINT_BL];
  segBR = aePointSeg[POINT_BR];
  if ((segTL == E_SEG_LL) && (segTR == E_SEG_HL) && (segBL == E_SEG_LH) && (segBR == E_SEG_HH)) {
    GSLC_DEBUG_PRINT("SegDetect: Normal display\n", "");
  } else if ((segTL == E_SEG_HL) && (segTR == E_SEG_LL) && (segBL == E_SEG_HH) && (segBR == E_SEG_LH)) {
    GSLC_DEBUG_PRINT("SegDetect: Normal display (FlipX)\n", "");
  } else if ((segTL == E_SEG_LH) && (segTR == E_SEG_HH) && (segBL == E_SEG_LL) && (segBR == E_SEG_HL)) {
    GSLC_DEBUG_PRINT("SegDetect: Normal display (FlipY)\n", "");
  } else if ((segTL == E_SEG_HH) && (segTR == E_SEG_LH) && (segBL == E_SEG_HL) && (segBR == E_SEG_LL)) {
    GSLC_DEBUG_PRINT("SegDetect: Normal display (FlipX,FlipY)\n", "");
  } else if ((segTL == E_SEG_LL) && (segTR == E_SEG_LH) && (segBL == E_SEG_HL) && (segBR == E_SEG_HH)) {
    GSLC_DEBUG_PRINT("SegDetect: RemapYX display\n", "");
    m_bRemapYX = true;
  } else if ((segTL == E_SEG_LH) && (segTR == E_SEG_LL) && (segBL == E_SEG_HH) && (segBR == E_SEG_HL)) {
    GSLC_DEBUG_PRINT("SegDetect: RemapYX display (FlipX)\n", "");
    m_bRemapYX = true;
  } else if ((segTL == E_SEG_HL) && (segTR == E_SEG_HH) && (segBL == E_SEG_LL) && (segBR == E_SEG_LH)) {
    GSLC_DEBUG_PRINT("SegDetect: RemapYX display (FlipY)\n", "");
    m_bRemapYX = true;
  } else if ((segTL == E_SEG_HH) && (segTR == E_SEG_HL) && (segBL == E_SEG_LH) && (segBR == E_SEG_LL)) {
    GSLC_DEBUG_PRINT("SegDetect: RemapYX display (FlipX,FlipY)\n", "");
    m_bRemapYX = true;
  } else {
    GSLC_DEBUG_PRINT("SegDetect: Unknown display orientation (%d,%d,%d,%d)\n",
      segTL, segTR, segBL, segBR);
    return false;
  }

  // If m_bRemapYX is set, then we will request the following line be added
  // to the user configuration. This is displayed in the final report.
  //   #define ADATOUCH_REMAP_YX 1

  // If we determined that the X & Y were swapped (even in the native orientation)
  // then we must update the calibration points we have collected so far to adjust
  // for this so that all further calculations account for this remapping.
  if (m_bRemapYX) {
    for (unsigned nInd = POINT_TL; nInd <= POINT_BR; nInd++) {
      nX = m_anTouchXPeak[nInd];
      nY = m_anTouchYPeak[nInd];
      m_anTouchXPeak[nInd] = nY;
      m_anTouchYPeak[nInd] = nX;
    }
  }

  // From this point onwards, we want to use the XY coordinates taking
  // into account any swapping detected above. This way, when we
  // capture coordinates from additional calibration points (eg. rotated view),
  // the coordinates will be remapped as needed.
  gslc_SetTouchRemapYX(&m_gui, m_bRemapYX);

  return true;
}

void CalcCalib()
{
  float   fSx, fSy;
  int16_t nXMin, nXMax, nYMin, nYMax;
  int16_t nXd, nYd;
  int16_t nPixXd, nPixYd;

  // Calculate calibration ranges based on averaging of raw target values
  // - Assume calibration was done in native orientation
  // - Assume pins XP/XM/YP/YM have been assigned correctly
  m_nTouchCalXMinRaw = (m_anTouchXPeak[POINT_TL] + m_anTouchXPeak[POINT_BL]) / 2;
  m_nTouchCalXMaxRaw = (m_anTouchXPeak[POINT_TR] + m_anTouchXPeak[POINT_BR]) / 2;
  m_nTouchCalYMinRaw = (m_anTouchYPeak[POINT_TL] + m_anTouchYPeak[POINT_TR]) / 2;
  m_nTouchCalYMaxRaw = (m_anTouchYPeak[POINT_BL] + m_anTouchYPeak[POINT_BR]) / 2;

  m_nTouchCalXRngRaw = m_nTouchCalXMaxRaw - m_nTouchCalXMinRaw;
  m_nTouchCalYRngRaw = m_nTouchCalYMaxRaw - m_nTouchCalYMinRaw;
  m_nTouchCalXRngRaw = abs(m_nTouchCalXRngRaw);
  m_nTouchCalYRngRaw = abs(m_nTouchCalYRngRaw);

  // If the pin configuration was incorrect, then the Left-Right or Top-Bottom
  // ranges could be close to zero. Detect this condition and report it
  if ((m_nTouchCalXRngRaw < DET_RAW_RANGE_MIN) || (m_nTouchCalYRngRaw < DET_RAW_RANGE_MIN)) {
    m_bTouchCalFail = true;
  }
  // Similarly, if there was a large variance between the points being averaged
  // (X:TL/BL, X:TR/BR, Y:TL/TR, Y:BL/BR), then it suggests we might also have
  // a bad pin configuration (eg. perhaps pins were swapped).
  int16_t nVarXMinRaw = (m_anTouchXPeak[POINT_TL] - m_anTouchXPeak[POINT_BL]);
  int16_t nVarXMaxRaw = (m_anTouchXPeak[POINT_TR] - m_anTouchXPeak[POINT_BR]);
  int16_t nVarYMinRaw = (m_anTouchYPeak[POINT_TL] - m_anTouchYPeak[POINT_TR]);
  int16_t nVarYMaxRaw = (m_anTouchYPeak[POINT_BL] - m_anTouchYPeak[POINT_BR]);
  nVarXMinRaw = abs(nVarXMinRaw);
  nVarXMaxRaw = abs(nVarXMaxRaw);
  nVarYMinRaw = abs(nVarYMinRaw);
  nVarYMaxRaw = abs(nVarYMaxRaw);
  if ((nVarXMinRaw > DET_RAW_VAR_MAX) || (nVarXMaxRaw > DET_RAW_VAR_MAX) || \
      (nVarYMinRaw > DET_RAW_VAR_MAX) || (nVarYMaxRaw > DET_RAW_VAR_MAX)) {
    m_bTouchCalFail = true;
  }

  if (m_bTouchCalFail) {
    GSLC_DEBUG_PRINT("\nERROR: Calibration failed. Please check touch pin config ADATOUCH_PIN_*\n\n", "");
  }

  // Apply correction factor for targets not at corners

  #if defined(DEBUG_CALIB)
    // Report raw calibration range prior to adjustment
    GSLC_DEBUG_PRINT("CALIB: Result(raw) XMin=%d XMax=%d Ymin=%d YMax=%d\n",
      m_nTouchCalXMinRaw, m_nTouchCalXMaxRaw, m_nTouchCalYMinRaw, m_nTouchCalYMaxRaw);
    GSLC_DEBUG_PRINT("CALIB: Range X=%d Y=%d. Expect > %d\n",
      m_nTouchCalXRngRaw, m_nTouchCalYRngRaw, DET_RAW_RANGE_MIN);
    GSLC_DEBUG_PRINT("CALIB: Var XMin=%d XMax=%d YMin=%d YMax=%d. Expect < %d\n",
      nVarXMinRaw, nVarXMaxRaw, nVarYMinRaw, nVarYMaxRaw, DET_RAW_VAR_MAX);
  #endif // DEBUG_CALIB


  // Calculate the range in touch readings & range in pixel coordinates
  nXd = (m_nTouchCalXMaxRaw - m_nTouchCalXMinRaw);
  nYd = (m_nTouchCalYMaxRaw - m_nTouchCalYMinRaw);
  nPixXd = (m_gui.nDisp0W - 2 * TARGET_MARGIN);
  nPixYd = (m_gui.nDisp0H - 2 * TARGET_MARGIN);

  // Determine mapping slope
  fSx = (float)nXd / (float)nPixXd;
  fSy = (float)nYd / (float)nPixYd;

  // Extrapolate range in touch readings (at corners)
  nXMin = m_nTouchCalXMinRaw - (fSx * TARGET_MARGIN);
  nYMin = m_nTouchCalYMinRaw - (fSy * TARGET_MARGIN);
  nXMax = m_nTouchCalXMaxRaw + (fSx * TARGET_MARGIN);
  nYMax = m_nTouchCalYMaxRaw + (fSy * TARGET_MARGIN);

  // Assign the adjusted calibration ranges
  m_nTouchCalXMin = nXMin;
  m_nTouchCalYMin = nYMin;
  m_nTouchCalXMax = nXMax;
  m_nTouchCalYMax = nYMax;

  GSLC_DEBUG_PRINT("CALIB: Result(adj) XMin=%d XMax=%d YMin=%d YMax=%d\n",
    m_nTouchCalXMin, m_nTouchCalXMax, m_nTouchCalYMin, m_nTouchCalYMax);

}

teCalibPt FindClosestPt(int16_t nX, int16_t nY)
{
  teCalibPt ePoint = POINT_TEST;
  int16_t nDeltaMinX, nDeltaMaxX, nDeltaMinY, nDeltaMaxY;
  /*
    nDeltaMinX = abs(m_nTouchCalXMin - nX);
    nDeltaMaxX = abs(m_nTouchCalXMax - nX);
    nDeltaMinY = abs(m_nTouchCalYMin - nY);
    nDeltaMaxY = abs(m_nTouchCalYMax - nY);
  */
  nDeltaMinX = m_nTouchCalXMinRaw - nX;
  nDeltaMaxX = m_nTouchCalXMaxRaw - nX;
  nDeltaMinY = m_nTouchCalYMinRaw - nY;
  nDeltaMaxY = m_nTouchCalYMaxRaw - nY;
  nDeltaMinX = abs(nDeltaMinX);
  nDeltaMaxX = abs(nDeltaMaxX);
  nDeltaMinY = abs(nDeltaMinY);
  nDeltaMaxY = abs(nDeltaMaxY);

  // Calculate tolerance for corner detection
  int16_t nTolerX;
  int16_t nTolerY;
  nTolerX = m_nTouchCalXRngRaw * DET_ROT_TOLER_PCT / 100;
  nTolerY = m_nTouchCalYRngRaw * DET_ROT_TOLER_PCT / 100;

  #ifdef DEBUG_CALIB
    GSLC_DEBUG_PRINT("ROT_FIND: calib_raw MinX=%d MaxX=%d MinY=%d MaxY=%d\n",
      m_nTouchCalXMinRaw, m_nTouchCalXMaxRaw, m_nTouchCalYMinRaw, m_nTouchCalYMaxRaw);
    GSLC_DEBUG_PRINT("ROT_FIND: rotate    X=%d Y=%d\n", nX, nY);
    GSLC_DEBUG_PRINT("ROT_FIND: tolerance X=%d Y=%d\n", nTolerX, nTolerY);
    GSLC_DEBUG_PRINT("ROT_FIND: delta     MinX= %d MaxX= %d MinY= %d MaxY= %d\n",
      nDeltaMinX, nDeltaMaxX, nDeltaMinY, nDeltaMaxY);
  #endif

  // Locate nearest calibration point that is within the tolerance zone
  if ((nDeltaMinX < nDeltaMaxX) && (nDeltaMinX < nTolerX)) {
    if ((nDeltaMinY < nDeltaMaxY) && (nDeltaMinY < nTolerY)) {
      ePoint = POINT_TL;
    }
    else if ((nDeltaMaxY < nDeltaMinY) && (nDeltaMaxY < nTolerY)) {
      ePoint = POINT_BL;
    }
    else {
      #ifdef DEBUG_CALIB
        GSLC_DEBUG_PRINT("ROT_FIND: Closest not found: X close, Y far\n", "");
      #endif // DEBUG_CALIB
    }
  }
  else if ((nDeltaMaxX < nDeltaMinX) && (nDeltaMaxX < nTolerX)) {
    if ((nDeltaMinY < nDeltaMaxY) && (nDeltaMinY < nTolerY)) {
      ePoint = POINT_TR;
    }
    else if ((nDeltaMaxY < nDeltaMinY) && (nDeltaMaxY < nTolerY)) {
      ePoint = POINT_BR;
    }
    else {
      #ifdef DEBUG_CALIB
        GSLC_DEBUG_PRINT("ROT_FIND: Closest not found: X far, Y close\n", "");
      #endif // DEBUG_CALIB
    }
  }
  else {
    #ifdef DEBUG_CALIB
      GSLC_DEBUG_PRINT("ROT_FIND: Closest not found: X far, Y far\n", "");
    #endif // DEBUG_CALIB
  }

  #ifdef DEBUG_CALIB
    if (ePoint != POINT_TEST) {
      GSLC_DEBUG_PRINT("ROT_FIND: Closest found: [%d]\n", ePoint);
    }
  #endif // DEBUG_CALIB

  return ePoint;
}

// Perform additional check to infer screen behavior as a
// result of screen rotation. The additional calibrattion
// point in ROTATE=1 mode enables us to see whether the
// display rotates as expected or whether the rotation direction
// is reversed and requires additional configuration.
void CalcCalibRotate()
{
  m_eRotSpecial = E_ROT_UNDEF;

  // Is the remapped POINT_ROT_TL in the top-right corner? (near POINT_TR?)
  // Determine which of the four raw calibration points were closest to
  // rotated point. Depending on which point was detected, we can infer
  // how the display must have been rotated. This enables us to determine
  // if we need additional configuration to handle the various rotation modes.
  teCalibPt ePtClosest = FindClosestPt(m_anTouchXPeak[POINT_TL_ROT], m_anTouchYPeak[POINT_TL_ROT]);

  switch (ePtClosest) {
  case POINT_TL:
    // No change! Display doesn't support rotation
    // NOTE: This case is expected on certain displays, but GUIslice
    //       does not currently disable rotation in this mode. For now
    //       users should not call GuiRotate() on these displays.
    m_eRotSpecial = E_ROT_NONE;
    break;
  case POINT_TR:
    // Normal expected rotation
    GSLC_DEBUG_PRINT("CALIB: Detected normal rotation\n", "");
    m_eRotSpecial = E_ROT_NORMAL;
    break;
  case POINT_BL:
    // Reversed rotation
    // NOTE: This case is not expected to be encountered and is not
    //       currently supported by GUIslice. However, if users report
    //       encountering displays with this case, support will be added.
    GSLC_DEBUG_PRINT("WARN: Detected reversed rotation. Please file GitHub issue.\n", "");
    m_eRotSpecial = E_ROT_REVERSE;
    break;
  case POINT_BR:
    // Unusual (swapped) rotation
    // NOTE: This case is not expected to be encountered and will not
    //       be handled by GUIslice
    GSLC_DEBUG_PRINT("ERROR: Detected unusual swapped rotation. Please file GitHub issue.\n", "");
    m_eRotSpecial = E_ROT_SWAP;
    break;
  case POINT_TEST:
  default:
    // No match found?
    GSLC_DEBUG_PRINT("ERROR: Could not detect rotation. Please retry calibration.\n", "");
    m_eRotSpecial = E_ROT_UNDEF;
    break;
  }

}

void ReportCalibResult()
{
  GSLC_DEBUG_PRINT("\n\n", "");
  GSLC_DEBUG_PRINT("Recommended calibration settings are shown below.\n", "");
  GSLC_DEBUG_PRINT("- Copy and paste these lines into your GUIslice config file\n", "");
  GSLC_DEBUG_PRINT("  over top of the existing ADATOUCH_X/Y_MIN/MAX settings.\n", "");
  GSLC_DEBUG_PRINT("\n", "");

  GSLC_DEBUG_PRINT("---\n", "");

  // Report the display's native dimensions
  char acDim[12] = "";
  snprintf(acDim, 12, "[%ux%u]", m_gui.nDisp0W, m_gui.nDisp0H);

  // NOTE: Only need to handle resistive touch display modes
  #if defined(DRV_TOUCH_ADA_SIMPLE)
    GSLC_DEBUG_PRINT("  // DRV_TOUCH_ADA_SIMPLE %s: ", acDim);
    #if defined(DRV_DISP_ADAGFX_MCUFRIEND)
      // For MCUFRIEND displays, report the ID
      snprintf(m_acTxt, MAX_STR, "(MCUFRIEND ID=0x%04X)", m_nMcuFriendId);
      GSLC_DEBUG_PRINT("%s ", m_acTxt);
    #endif
    // Report the pin wiring to the 4-wire resistive interface
    #if !defined(A0)
      // Support MCUs that don't use analog pin "A0" notation (eg. STM32)
      GSLC_DEBUG_PRINT("(XP=%d,XM=%d,YP=%d,YM=%d) ", ADATOUCH_PIN_XP, ADATOUCH_PIN_XM, ADATOUCH_PIN_YP, ADATOUCH_PIN_YM);
    #else
      GSLC_DEBUG_PRINT("(XP=%u,XM=A%d,YP=A%d,YM=%u) ", ADATOUCH_PIN_XP, (ADATOUCH_PIN_XM - A0), (ADATOUCH_PIN_YP - A0), ADATOUCH_PIN_YM);
    #endif

    GSLC_DEBUG_PRINT("\n", "");
  #elif defined(DRV_TOUCH_ADA_STMPE610)
    GSLC_DEBUG_PRINT("  // DRV_TOUCH_ADA_STMPE610 %s:\n", acDim);
  #elif defined(DRV_TOUCH_XPT2046_PS)
    GSLC_DEBUG_PRINT("  // DRV_TOUCH_XPT2046_PS %s:\n", acDim);
  #elif defined(DRV_TOUCH_XPT2046_STM)
    GSLC_DEBUG_PRINT("  // DRV_TOUCH_XPT2046_STM %s:\n", acDim);
  #endif // DRV_TOUCH_ADA_*

  if (m_bTouchCalFail) {
    GSLC_DEBUG_PRINT("*** ERROR: Calibration failed. Please check log.\n", "");
    return;
  }

  GSLC_DEBUG_PRINT("  #define ADATOUCH_X_MIN    %u\n", m_nTouchCalXMin);
  GSLC_DEBUG_PRINT("  #define ADATOUCH_X_MAX    %u\n", m_nTouchCalXMax);
  GSLC_DEBUG_PRINT("  #define ADATOUCH_Y_MIN    %u\n", m_nTouchCalYMin);
  GSLC_DEBUG_PRINT("  #define ADATOUCH_Y_MAX    %u\n", m_nTouchCalYMax);

  GSLC_DEBUG_PRINT("  #define ADATOUCH_REMAP_YX %u\n", m_bRemapYX);


  // Check for unexpected rotation behavior
  switch (m_eRotSpecial) {
  case E_ROT_NORMAL:
    // No need to add special config
    break;
  case E_ROT_REVERSE:
    GSLC_DEBUG_PRINT("  #define GSLC_ROTATE_SPECIAL %d // Reversed Rotation\n", m_eRotSpecial);
    break;
  case E_ROT_SWAP:
    GSLC_DEBUG_PRINT("  #define GSLC_ROTATE_SPECIAL %d // Swapped Rotation\n", m_eRotSpecial);
    break;
  case E_ROT_NONE:
    GSLC_DEBUG_PRINT("  #define GSLC_ROTATE_SPECIAL %d // No Rotation supported\n", m_eRotSpecial);
    break;
  case E_ROT_UNDEF:
  default:
    GSLC_DEBUG_PRINT("  // NOTE: Rotation type not detected properly\n", "");
    break;
  }
  GSLC_DEBUG_PRINT("---\n", "");

  if ((m_eRotSpecial == E_ROT_REVERSE) || (m_eRotSpecial == E_ROT_SWAP)) {
    GSLC_DEBUG_PRINT("*** Please file a GitHub issue to add support for this mode\n", "");
  }

  GSLC_DEBUG_PRINT("\n", "");

}

void DrawCalibResult()
{
  snprintf(m_acTxt, MAX_STR, "ADATOUCH_X_MIN: %u", m_nTouchCalXMin);
  gslc_DrvDrawTxt(&m_gui, m_rReport.x, m_rReport.y + 00, m_pFont, m_acTxt, GSLC_TXT_DEFAULT, GSLC_COL_YELLOW, GSLC_COL_BLACK);
  snprintf(m_acTxt, MAX_STR, "ADATOUCH_Y_MIN: %u", m_nTouchCalYMin);
  gslc_DrvDrawTxt(&m_gui, m_rReport.x, m_rReport.y + 20, m_pFont, m_acTxt, GSLC_TXT_DEFAULT, GSLC_COL_YELLOW, GSLC_COL_BLACK);
  snprintf(m_acTxt, MAX_STR, "ADATOUCH_X_MAX: %u", m_nTouchCalXMax);
  gslc_DrvDrawTxt(&m_gui, m_rReport.x, m_rReport.y + 40, m_pFont, m_acTxt, GSLC_TXT_DEFAULT, GSLC_COL_YELLOW, GSLC_COL_BLACK);
  snprintf(m_acTxt, MAX_STR, "ADATOUCH_Y_MAX: %u", m_nTouchCalYMax);
  gslc_DrvDrawTxt(&m_gui, m_rReport.x, m_rReport.y + 60, m_pFont, m_acTxt, GSLC_TXT_DEFAULT, GSLC_COL_YELLOW, GSLC_COL_BLACK);

  if (m_bTouchCalFail) {
    gslc_DrvDrawTxt(&m_gui, m_rReport.x, m_rReport.y + 100, m_pFont, "ERROR: See serial log", GSLC_TXT_DEFAULT, GSLC_COL_RED, GSLC_COL_BLACK);
  }
  else if ((m_eRotSpecial == E_ROT_REVERSE) || (m_eRotSpecial == E_ROT_SWAP)) {
    gslc_DrvDrawTxt(&m_gui, m_rReport.x, m_rReport.y + 100, m_pFont, "ERROR: See serial log", GSLC_TXT_DEFAULT, GSLC_COL_RED, GSLC_COL_BLACK);
  }
  else if (m_eRotSpecial == E_ROT_NONE) {
    gslc_DrvDrawTxt(&m_gui, m_rReport.x, m_rReport.y + 100, m_pFont, "No rotation supported", GSLC_TXT_DEFAULT, GSLC_COL_RED, GSLC_COL_BLACK);
  }
  else if (m_eRotSpecial == E_ROT_UNDEF) {
    gslc_DrvDrawTxt(&m_gui, m_rReport.x, m_rReport.y + 100, m_pFont, "ERROR: Could not detect rotation", GSLC_TXT_DEFAULT, GSLC_COL_RED, GSLC_COL_BLACK);
  }
  else if (m_eRotSpecial == E_ROT_NORMAL) {
    gslc_DrvDrawTxt(&m_gui, m_rReport.x, m_rReport.y + 100, m_pFont, "Rotation detected OK", GSLC_TXT_DEFAULT, GSLC_COL_GREEN, GSLC_COL_BLACK);
  }

  gslc_DrvDrawTxt(&m_gui, m_rReport.x, m_rReport.y + 140, m_pFont, "Click to continue", GSLC_TXT_DEFAULT, GSLC_COL_BLUE, GSLC_COL_BLACK);
}


// ----------------------------------------------------------------------------

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

  // ----------------------------------------------------------------------------
  // CALIB:
  // ----------------------------------------------------------------------------
  tsCalibStage sCalibStage;
  bool bSegsOk = false;

  // Determine which calibration point we are handling
  switch (m_eState) {
  case STATE_CAPT_TL_MSG:
  case STATE_CAPT_TL_DO:
    m_eCalibPt = POINT_TL;
    break;
  case STATE_CAPT_TR_MSG:
  case STATE_CAPT_TR_DO:
    m_eCalibPt = POINT_TR;
    break;
  case STATE_CAPT_BL_MSG:
  case STATE_CAPT_BL_DO:
    m_eCalibPt = POINT_BL;
    break;
  case STATE_CAPT_BR_MSG:
  case STATE_CAPT_BR_DO:
    m_eCalibPt = POINT_BR;
    break;
  case STATE_CAPT_ROT_TL_MSG:
  case STATE_CAPT_ROT_TL_DO:
    m_eCalibPt = POINT_TL_ROT;
    break;

  default:
    m_eCalibPt = POINT_TEST;
    break;
  } // m_eState
  // ----------------------------------------------------------------------------

  // Handle state machine
  switch (m_eState) {

  // ----------------------------------------------------------------------------
  // CALIB:
  // ----------------------------------------------------------------------------

  case STATE_CAL_START_MSG:
    if (m_bStateBegin) {
      RedrawStatus("GUIslice Calibration");
      DrawBackgroundStart();
      ResetDatapoints();

      // Initialize the calibration to arbitrary defaults
      ResetCalib();
      ResetMax();

      #if defined(CALC_EXP_AVG)
      GSLC_DEBUG_PRINT("CALIB: Averaging mode: EXP\n", "");
      #elif defined(CALC_BASIC_AVG)
      GSLC_DEBUG_PRINT("CALIB: Averaging mode: BASIC\n", "");
      #endif      
      GSLC_DEBUG_PRINT("\n", "");

      // Need to disable any touch remapping during calibration
      gslc_SetTouchRemapEn(&m_gui, false);
      gslc_SetTouchRemapYX(&m_gui, false);
    }
    if (bTouchUp) { m_eState = STATE_CAPT_TL_MSG; }
    break;

  case STATE_CAPT_TL_MSG:
  case STATE_CAPT_TR_MSG:
  case STATE_CAPT_BL_MSG:
  case STATE_CAPT_BR_MSG:
  case STATE_CAPT_ROT_TL_MSG:
    sCalibStage = m_asCalibStage[m_eCalibPt];
    if (m_bStateBegin) {
      DrawBackground();
      RedrawStatus(sCalibStage.acStr);
      ResetMax();
      ResetDatapoints();
      DrawPageCoords();
      m_bTouchCoordValid = false;
      RedrawCoords(0, 0, 0);
    }
    DrawTarget(sCalibStage.bXLeft ? TARGET_MARGIN : (m_gui.nDispW - TARGET_MARGIN),
      sCalibStage.bYTop ? TARGET_MARGIN : (m_gui.nDispH - TARGET_MARGIN),
      TARGET_SIZE, GSLC_COL_GREEN, GSLC_COL_GREEN_LT2);
    if (bTouchDown) { m_eState = sCalibStage.eStateDo; }
    break;

  case STATE_CAPT_TL_DO:
  case STATE_CAPT_TR_DO:
  case STATE_CAPT_BL_DO:
  case STATE_CAPT_BR_DO:
  case STATE_CAPT_ROT_TL_DO:
    sCalibStage = m_asCalibStage[m_eCalibPt];
    if (bTouchDown) {
      CalcMaxCoords(nTouchX, nTouchY, nTouchZ);
      RedrawCoords(nTouchX, nTouchY, nTouchZ);
      AddDatapoint(nTouchX, nTouchY);
    }
    if (bTouchUp) {
      CalcDatapoint(m_eCalibPt);
      GSLC_DEBUG_PRINT("%s: X=%d Y=%d Cnt=%u, Z=(%d..%d)\n", sCalibStage.acStr, m_nPointFinalX, m_nPointFinalY, m_nPointBufCnt,
        m_nTouchZMin, m_nTouchZMax);
      m_eState = sCalibStage.eStateNxt;
    }
    break;

  case STATE_CAPT_ROT:
    // Calculate calibration
    bSegsOk = CalcSegments();
	(void)bSegsOk; // Unused
    CalcCalib();

    GSLC_DEBUG_PRINT("\nCALIB: Rotate\n", "");

    // Reset the background
    DrawBackground();

    // Rotate the display to ROTATE=1
    //
    // This step detects how the display rotation behaves (ie. rotates clockwise or
    // counter-clockwise) by asking for an additional calibration point
    // after rotating the display between orientation 0 to orientation 1.
    // Depending on the quadrant that the user selects in this final step,
    // a determination can be made as to the rotation direction and
    // an additional rotation config setting can be reported.
    gslc_GuiRotate(&m_gui, 1);

    m_eState = STATE_CAPT_ROT_TL_MSG;
    break;

  case STATE_CAL_REPORT:
    // Redraw to cleanup markers
    DrawBackground();

    if (m_bStateBegin) {
      RedrawStatus("Calibrate Done");
    }
    m_bTouchCoordValid = false;

    // Calculate the calibration settings based on the
    // rotated orientation (ROTATE=1).
    CalcCalibRotate();

    ReportCalibResult();
    DrawCalibResult();

    m_eState = STATE_CAL_DONE_MSG;
    break;

  case STATE_CAL_DONE_MSG:
    if (bTouchDown) { m_eState = STATE_CAL_DONE_END; }
    break;
  case STATE_CAL_DONE_END:
    if (bTouchUp) { m_eState = STATE_CAL_SET; }
    break;

  // ----------------------------------------------------------------------------

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