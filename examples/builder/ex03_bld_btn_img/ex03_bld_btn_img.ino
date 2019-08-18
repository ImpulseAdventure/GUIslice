//<File !Start!>
// FILE: [ex03_bld_btn_img.ino]
// Created by GUIslice Builder version: [0.13.0]
//
// GUIslice Builder Generated File
//
// For the latest guides, updates and support view:
// https://github.com/ImpulseAdventure/GUIslice
//
//<File !End!>
//
// - Example 03 (Arduino):
//   - Accept touch input, graphic button
//   - Expected behavior: Clicking on button outputs a message
//     to the serial monitor
//   - NOTE: This is the simple version of the example without
//     optimizing for memory consumption.
//
// - GUIslice_config.h must be edited to match the pinout connections
//   between the Arduino CPU and the display controller (see ADAGFX_PIN_*).
//   - IMPORTANT: This sketch uses graphics located on an external SD card
//     accessed via a SPI interface. The GUIslice_config.h needs to set
//     #define GSLC_SD_EN 1
// - This example assumes that two image files have been created in the
//   root of the SD card directory: "exit_n24.bmp" and "exit_g24.bmp"
//   representing the quit button normal and glowing states. The file
//   format is 24-bit BMP. These files have been included for reference in
//   /builder/res/

// ------------------------------------------------
// Headers to include
// ------------------------------------------------
#include "GUIslice.h"
#include "GUIslice_drv.h"

// Include any extended elements
//<Includes !Start!>

// Ensure optional features are enabled in the configuration
#if !(GSLC_SD_EN)
  #error "Config: GSLC_SD_EN required for this program but not enabled. Please update GUIslice/config/"
#endif
//<Includes !End!>

// ------------------------------------------------
// Headers and Defines for fonts
// Note that font files are located within the Adafruit-GFX library folder:
// ------------------------------------------------
//<Fonts !Start!>
//<Fonts !End!>

// ------------------------------------------------
// Defines for resources
// ------------------------------------------------
//<Resources !Start!>
#define IMG_EXIT_G24_SEL          "/exit_g24.bmp"
#define IMG_EXIT_N24              "/exit_n24.bmp"
//<Resources !End!>

// ------------------------------------------------
// Enumerations for pages, elements, fonts, images
// ------------------------------------------------
//<Enum !Start!>
enum {E_PAGE_MAIN};
enum {E_ELEM_BOX1,E_ELEM_BTN_QUIT};
// Must use separate enum for fonts with MAX_FONT at end to use gslc_FontSet.
enum {MAX_FONT};
//<Enum !End!>

// ------------------------------------------------
// Instantiate the GUI
// ------------------------------------------------

// ------------------------------------------------
// Define the maximum number of elements and pages
// ------------------------------------------------
//<ElementDefines !Start!>
#define MAX_PAGE                1

#define MAX_ELEM_PAGE_MAIN 2                                          // # Elems total on page
#define MAX_ELEM_PAGE_MAIN_RAM MAX_ELEM_PAGE_MAIN // # Elems in RAM
//<ElementDefines !End!>

// ------------------------------------------------
// Create element storage
// ------------------------------------------------
gslc_tsGui                      m_gui;
gslc_tsDriver                   m_drv;
gslc_tsFont                     m_asFont[MAX_FONT];
gslc_tsPage                     m_asPage[MAX_PAGE];

//<GUI_Extra_Elements !Start!>
gslc_tsElem                     m_asPage1Elem[MAX_ELEM_PAGE_MAIN_RAM];
gslc_tsElemRef                  m_asPage1ElemRef[MAX_ELEM_PAGE_MAIN];

#define MAX_STR                 100

//<GUI_Extra_Elements !End!>

// ------------------------------------------------
// Program Globals
// ------------------------------------------------
bool        m_bQuit = false;

// Save some element references for direct access
//<Save_References !Start!>
//<Save_References !End!>

// Define debug message function
static int16_t DebugOut(char ch) { if (ch == (char)'\n') Serial.println(""); else Serial.write(ch); return 0; }

// ------------------------------------------------
// Callback Methods
// ------------------------------------------------
// Common Button callback
bool CbBtnCommon(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY)
{
  gslc_tsElemRef* pElemRef = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem* pElem = pElemRef->pElem;

  // Determine what type of event occurred on the button
  // - In this case we're just looking for the user releasing
  //   a touch over the button.
  if ( eTouch == GSLC_TOUCH_UP_IN ) {
    // From the element's ID we can determine which button was pressed.
    switch (pElem->nId) {
//<Button Enums !Start!>

      case E_ELEM_BTN_QUIT:
        // Output a message when the button is pressed
        Serial.println("Quit button pressed");
        // Set a variable flag that we can use elsewhere
        m_bQuit = true;
        break;
//<Button Enums !End!>
        default:
        break;
    }
  }
  return true;
}
//<Checkbox Callback !Start!>
//<Checkbox Callback !End!>
//<Keypad Callback !Start!>
//<Keypad Callback !End!>
//<Spinner Callback !Start!>
//<Spinner Callback !End!>
//<Listbox Callback !Start!>
//<Listbox Callback !End!>
//<Draw Callback !Start!>
//<Draw Callback !End!>
//<Slider Callback !Start!>
//<Slider Callback !End!>
//<Tick Callback !Start!>
//<Tick Callback !End!>

// ------------------------------------------------
// Create page elements
// ------------------------------------------------
bool InitGUI()
{
  gslc_tsElemRef* pElemRef = NULL;

//<InitGUI !Start!>
  gslc_PageAdd(&m_gui,E_PAGE_MAIN,m_asPage1Elem,MAX_ELEM_PAGE_MAIN_RAM,m_asPage1ElemRef,MAX_ELEM_PAGE_MAIN);

  // NOTE: The current page defaults to the first page added. Here we explicitly
  //       ensure that the main page is the correct page no matter the add order.
  gslc_SetPageCur(&m_gui,E_PAGE_MAIN);
  
  // Set Background to a flat color
  gslc_SetBkgndColor(&m_gui,GSLC_COL_GRAY_DK2);

  // -----------------------------------
  // PAGE: E_PAGE_MAIN
  
   
  // Create E_ELEM_BOX1 box
  pElemRef = gslc_ElemCreateBox(&m_gui,E_ELEM_BOX1,E_PAGE_MAIN,(gslc_tsRect){10,50,300,150});
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_WHITE,GSLC_COL_BLACK,GSLC_COL_BLACK);
  
  // Create E_ELEM_BTN_QUIT button with image label
  pElemRef = gslc_ElemCreateBtnImg(&m_gui,E_ELEM_BTN_QUIT,E_PAGE_MAIN,(gslc_tsRect){258,70,32,32},
          gslc_GetImageFromSD((const char*)IMG_EXIT_N24,GSLC_IMGREF_FMT_BMP24),
          gslc_GetImageFromSD((const char*)IMG_EXIT_G24_SEL,GSLC_IMGREF_FMT_BMP24),
          &CbBtnCommon);
  gslc_ElemSetFillEn(&m_gui,pElemRef,true);
//<InitGUI !End!>

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

  if (!gslc_Init(&m_gui,&m_drv,m_asPage,MAX_PAGE,m_asFont,MAX_FONT)) { return; }

  // ------------------------------------------------
  // Load Fonts
  // ------------------------------------------------
//<Load_Fonts !Start!>
//<Load_Fonts !End!>

  // ------------------------------------------------
  // Create graphic elements
  // ------------------------------------------------
  InitGUI();

//<Startup !Start!>
//<Startup !End!>

}

// -----------------------------------
// Main event loop
// -----------------------------------
void loop()
{

  // ------------------------------------------------
  // Update GUI Elements
  // ------------------------------------------------
  
  //TODO - Add update code for any text, gauges, or sliders
  
  // ------------------------------------------------
  // Periodically call GUIslice update function
  // ------------------------------------------------
  gslc_Update(&m_gui);
    
  // In most programs, we would detect the button press and take an
  // action. In this simplest of examples, the "Quit" button callback
  // just outputs a message to the serial monitor when pressed and
  // sets the variable m_bQuit to true but the main loop continues to run.
}

