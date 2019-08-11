//
// GUIslice Library Examples
// - Calvin Hass
// - https://www.impulseadventure.com/elec/guislice-gui.html
// - https://github.com/ImpulseAdventure/GUIslice
// - Example 02 (Arduino): [minimum RAM version]
//   - Accept touch input, text button
//   - Expected behavior: Clicking on button outputs a message
//     to the serial monitor
//   - Demonstrates the use of ElemCreate*_P() functions.
//     These RAM-reduced examples take advantage of the internal
//     Flash storage (via PROGMEM).
//

// ARDUINO NOTES:
// - GUIslice_config.h must be edited to match the pinout connections
//   between the Arduino CPU and the display controller (see ADAGFX_PIN_*).

#include "GUIslice.h"
#include "GUIslice_drv.h"

// Defines for resources

// Enumerations for pages, elements, fonts, images
enum {E_PG_MAIN};
enum {E_ELEM_BOX,E_ELEM_BTN_QUIT};
enum {E_FONT_BTN,MAX_FONT}; // Use separate enum for fonts, MAX_FONT at end

bool    m_bQuit = false;

// Instantiate the GUI
#define MAX_PAGE            1
#define MAX_ELEM_PG_MAIN    2

// Define the maximum number of elements per page
// - To enable the same code to run on devices that support storing
//   data into Flash (PROGMEM) and those that don't, we can make the
//   number of elements in Flash dependent upon GSLC_USE_PROGMEM
// - This should allow both Arduino and ARM Cortex to use the same code
#define MAX_ELEM_PG_MAIN          2                                         // # Elems total
#if (GSLC_USE_PROGMEM)
  #define MAX_ELEM_PG_MAIN_PROG   2                                         // # Elems in Flash
#else
  #define MAX_ELEM_PG_MAIN_PROG   0                                         // # Elems in Flash
#endif
#define MAX_ELEM_PG_MAIN_RAM      MAX_ELEM_PG_MAIN - MAX_ELEM_PG_MAIN_PROG  // # Elems in RAM

gslc_tsGui                  m_gui;
gslc_tsDriver               m_drv;
gslc_tsFont                 m_asFont[MAX_FONT];
gslc_tsPage                 m_asPage[MAX_PAGE];
gslc_tsElem                 m_asPageElem[MAX_ELEM_PG_MAIN_RAM];   // Storage for all elements in RAM
gslc_tsElemRef              m_asPageElemRef[MAX_ELEM_PG_MAIN];    // References for all elements in GUI

// Define debug message function
static int16_t DebugOut(char ch) { Serial.write(ch); return 0; }

// Button callbacks
// - This function gets called when the button is pressed
bool CbBtnQuit(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY)
{
  // Determine what type of event occurred on the button
  // - In this case we're just looking for the user releasing
  //   a touch over the button.
  if (eTouch == GSLC_TOUCH_UP_IN) {
    // Output a message when the button is pressed
    Serial.println("Quit button pressed");
    // Set a variable flag that we can use elsewhere
    m_bQuit = true;
  }
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
  if (!gslc_FontSet(&m_gui,E_FONT_BTN,GSLC_FONTREF_PTR,NULL,1)) { return; }

  // -----------------------------------
  // Create page elements
  gslc_PageAdd(&m_gui,E_PG_MAIN,m_asPageElem,MAX_ELEM_PG_MAIN_RAM,m_asPageElemRef,MAX_ELEM_PG_MAIN);

  // Background flat color
  gslc_SetBkgndColor(&m_gui,GSLC_COL_GRAY_DK2);

  // Create background box
  gslc_ElemCreateBox_P(&m_gui,E_ELEM_BOX,E_PG_MAIN,10,50,300,150,GSLC_COL_WHITE,GSLC_COL_BLACK,true,true,NULL,NULL);

  // Create Quit button with text label
  gslc_ElemCreateBtnTxt_P(&m_gui,E_ELEM_BTN_QUIT,E_PG_MAIN,120,100,80,40,"Quit",&m_asFont[E_FONT_BTN],
    GSLC_COL_WHITE,GSLC_COL_BLUE_DK2,GSLC_COL_BLUE_DK4,GSLC_COL_BLUE_DK2,GSLC_COL_BLUE_DK1,GSLC_ALIGN_MID_MID,true,true,&CbBtnQuit,NULL);


  // -----------------------------------
  // Start up display on main page
  gslc_SetPageCur(&m_gui,E_PG_MAIN);

  m_bQuit = false;
}

void loop()
{
  // Periodically call GUIslice update function
  gslc_Update(&m_gui);

  // In most programs, we would detect the button press and take an
  // action. In this simplest of examples, the "Quit" button callback
  // just outputs a message to the serial monitor when pressed and
  // sets the variable m_bQuit to true but the main loop continues to run.
}

