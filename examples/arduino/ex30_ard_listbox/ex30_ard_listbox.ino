//
// GUIslice Library Examples
// - Calvin Hass
// - https://www.impulseadventure.com/elec/guislice-gui.html
// - https://github.com/ImpulseAdventure/GUIslice
// - Example 30 (Arduino): Listbox
//   - Demonstrates Listbox control (multi-column)
//   - Shows callback notifications for Listbox
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
#include "elem/XListbox.h"

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
    #define FONT_NAME1 &FreeSans9pt7b
  #elif defined(DRV_DISP_ADAGFX_ILI9341_T3) // Teensy
    #include <font_Arial.h>
    #define FONT_NAME1 &Arial_9
    #define SET_FONT_MODE1 // Enable Teensy extra fonts
  #else // Arduino, etc.
    #include <Adafruit_GFX.h>
    #include <gfxfont.h>
    #include "Fonts/FreeSans9pt7b.h"
    #define FONT_NAME1 &FreeSans9pt7b
  #endif
#else
  // Use the default font
  #define FONT_NAME1 NULL
#endif
// ------------------------------------------------

// To demonstrate additional fonts, uncomment the following line:
//#define USE_EXTRA_FONTS

#ifdef USE_EXTRA_FONTS
  // Note that these files are located within the Adafruit-GFX library folder:
  #include <Adafruit_GFX.h>
  #include "Fonts/FreeSansBold9pt7b.h"
#endif

// Defines for resources


// Enumerations for pages, elements, fonts, images
enum { E_PG_MAIN };
enum { E_ELEM_BTN_QUIT, E_ELEM_LISTBOX };
enum { E_FONT_EXTRA, E_FONT_TXT, MAX_FONT }; // Use separate enum for fonts, MAX_FONT at end

bool        m_bQuit = false;

// Free-running counter for display
unsigned    m_nCount = 0;


// Instantiate the GUI
#define MAX_PAGE                1

// Define the maximum number of elements per page
#define MAX_ELEM_PG_MAIN          5                                         // # Elems total
#define MAX_ELEM_PG_MAIN_RAM      MAX_ELEM_PG_MAIN                          // # Elems in RAM

gslc_tsGui                  m_gui;
gslc_tsDriver               m_drv;
gslc_tsFont                 m_asFont[MAX_FONT];
gslc_tsPage                 m_asPage[MAX_PAGE];
gslc_tsElem                 m_asPageElem[MAX_ELEM_PG_MAIN_RAM];
gslc_tsElemRef              m_asPageElemRef[MAX_ELEM_PG_MAIN];

gslc_tsXListbox             m_sXListbox;


#define MAX_STR     15
char                acTxt[MAX_STR + 1];


// Save some element references for quick access
gslc_tsElemRef*  m_pElemSel = NULL;
gslc_tsElemRef*  m_pElemListbox = NULL;

// Define debug message function
static int16_t DebugOut(char ch) { Serial.write(ch); return 0; }

// Button callbacks
// - Detect a button press
// - In this particular example, we are looking for the Quit button press
//   which is used to terminate the program.
bool CbBtnQuit(void* pvGui, void *pvElem, gslc_teTouch eTouch, int16_t nX, int16_t nY)
{
  if (eTouch == GSLC_TOUCH_UP_IN) {
    m_bQuit = true;
  }
  return true;
}

bool CbListbox(void* pvGui, void* pvElemRef, int16_t nSelId)
{
  gslc_tsGui*     pGui = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef = (gslc_tsElemRef*)(pvElemRef);
  //gslc_tsElem*    pElem = gslc_GetElemFromRef(pGui, pElemRef);
  if (pElemRef == NULL) {
    return false;
  }

  // Update the status message with the selection
  if (nSelId == XLISTBOX_SEL_NONE) {
    snprintf(acTxt, MAX_STR, "NONE");
  } else {
    gslc_ElemXListboxGetItem(&m_gui, pElemRef, nSelId, acTxt, MAX_STR);
  }
  gslc_ElemSetTxtStr(&m_gui, m_pElemSel, acTxt);

  return true;
}


// Create page elements
bool InitOverlays()
{
  gslc_tsElemRef* pElemRef;

  gslc_PageAdd(&m_gui, E_PG_MAIN, m_asPageElem, MAX_ELEM_PG_MAIN_RAM, m_asPageElemRef, MAX_ELEM_PG_MAIN);


  // Background flat color
  gslc_SetBkgndColor(&m_gui, GSLC_COL_GRAY_DK2);

  // Create background box
  pElemRef = gslc_ElemCreateBox(&m_gui, GSLC_ID_AUTO, E_PG_MAIN, (gslc_tsRect) { 10, 10, 300, 220 });
  gslc_ElemSetCol(&m_gui, pElemRef, GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);

  // Create Quit button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui, E_ELEM_BTN_QUIT, E_PG_MAIN,
    (gslc_tsRect) { 240, 20, 60, 30 }, (char*)"Quit", 0, E_FONT_TXT, &CbBtnQuit);

  // Create selection status message
  pElemRef = gslc_ElemCreateTxt(&m_gui, GSLC_ID_AUTO, E_PG_MAIN, (gslc_tsRect) { 20, 20, 50, 10 },
    (char*)"Selected:", 0, E_FONT_TXT);
  static char mstr2[20] = "";
  pElemRef = gslc_ElemCreateTxt(&m_gui, GSLC_ID_AUTO, E_PG_MAIN, (gslc_tsRect) { 80, 20, 50, 10 },
    mstr2, sizeof(mstr2), E_FONT_TXT);
  gslc_ElemSetTxtCol(&m_gui, pElemRef, GSLC_COL_ORANGE);
  m_pElemSel = pElemRef; // Save for quick access

  // Create the XListbox
  // - Other APIs that can be called:
  //   gslc_ElemXListboxItemsSetGap(&m_gui, pElemRef, 2);
  //   gslc_ElemXListboxItemsSetTxtMargin(&m_gui, pElemRef, 5, 8);
  static uint8_t m_pXListboxBuf[50];
  pElemRef = gslc_ElemXListboxCreate(&m_gui, E_ELEM_LISTBOX, E_PG_MAIN, &m_sXListbox,
    (gslc_tsRect) { 60, 50, 160, 160 }, E_FONT_EXTRA, m_pXListboxBuf, 50, XLISTBOX_SEL_NONE);
  gslc_ElemXListboxSetSize(&m_gui, pElemRef, 4, 2);
  gslc_ElemXListboxItemsSetSize(&m_gui, pElemRef, XLISTBOX_SIZE_AUTO, XLISTBOX_SIZE_AUTO);
  gslc_ElemXListboxAddItem(&m_gui, pElemRef, "Red");
  gslc_ElemXListboxAddItem(&m_gui, pElemRef, "Orange");
  gslc_ElemXListboxAddItem(&m_gui, pElemRef, "Yellow");
  gslc_ElemXListboxAddItem(&m_gui, pElemRef, "Green");
  gslc_ElemXListboxAddItem(&m_gui, pElemRef, "Blue");
  gslc_ElemXListboxAddItem(&m_gui, pElemRef, "Indigo");
  gslc_ElemXListboxAddItem(&m_gui, pElemRef, "Violet");
  gslc_ElemXListboxSetSelFunc(&m_gui, pElemRef, &CbListbox);
  gslc_ElemSetCol(&m_gui, pElemRef, GSLC_COL_BLUE_DK3, GSLC_COL_GRAY_DK3, GSLC_COL_GREEN_DK1);
  gslc_ElemSetFrameEn(&m_gui, pElemRef, true);
  gslc_ElemSetTxtCol(&m_gui, pElemRef, GSLC_COL_WHITE);
  gslc_ElemSetTxtMarginXY(&m_gui, pElemRef, 5, 0); // Provide additional margin from left side
  m_pElemListbox = pElemRef; // Save for quick access


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
  if (!gslc_FontSet(&m_gui, E_FONT_EXTRA, GSLC_FONTREF_PTR, FONT_NAME1, 1)) { return; }
  if (!gslc_FontSet(&m_gui, E_FONT_TXT, GSLC_FONTREF_PTR, NULL, 1)) { return; }
  // Some display drivers need to set a mode to use the extra fonts
  #if defined(SET_FONT_MODE1)
    gslc_FontSetMode(&m_gui, E_FONT_EXTRA, GSLC_FONTREF_MODE_1);
  #endif

  // Create graphic elements
  InitOverlays();

  // Start up display on main page
  gslc_SetPageCur(&m_gui, E_PG_MAIN);

  m_bQuit = false;
}

void loop()
{
  // General counter
  m_nCount++;

  // Update elements on active page

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
    while (1) {}
  }
}