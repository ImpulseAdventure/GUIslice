//<File !Start!>
// FILE: [ex44_bld_spiffs.ino]
// Created by GUIslice Builder version: [0.16.0]
//
// GUIslice Builder Generated GUI Framework File
//
// For the latest guides, updates and support view:
// https://github.com/ImpulseAdventure/GUIslice
//
//<File !End!>
// GUIslice Builder Examples
// - Paul Conti
// - Example 44 SPIFFS (ESP8266/ESP32):
//   - Access Jpeg Images and Icons from SPIFFS file system.
//     A Popup page with a text, text button, and a text box is 
//     also demonstrated when the QUIT button is touched.
//
//   - NOTE: This Example demonstrates access with the TFT_eSPI, TFT_eFEX,
//     and JPEGDecoder libraries.
//
//     Instructions using Arduino IDE:
//       ---
//       You must start by uploading the images inside the folder 
//       GUIslice\examples\builder\ex44_bld_spiffs\data to your
//       SPIFFS data partition.  They take up only about 82.2kb.
//       ---
//       You use the "Tools"  "ESP32" (or ESP8266) "Sketch Data Upload"  
//       menu option in the IDE.
//       ---
//       To add this option follow instructions here for the ESP8266:
//       https://github.com/esp8266/arduino-esp8266fs-plugin
//       or for the ESP32:
//       https://github.com/me-no-dev/arduino-esp32fs-plugin
//       Close the IDE and open again to see the new menu option.
//       ---
//       Now you may upload the images and icons to SPIFFS using the
//       "Sketch Data Upload" menu option in the IDE.
//

// ------------------------------------------------
// Headers to include
// ------------------------------------------------
#include "GUIslice.h"
#include "GUIslice_drv.h"

// Include any extended elements
//<Includes !Start!>
//<Includes !End!>

#if !defined(ESP8266) && !defined(ESP32)
  #error "This example only works on ESP8266 / ESP32"
#endif

// ------------------------------------------------
// Headers and Defines for fonts
// Note that font files are located within the Adafruit-GFX library folder:
// ------------------------------------------------

//<Fonts !Start!>
#if !defined(DRV_DISP_TFT_ESPI)
  #error Project tab->Target Platform should be arduino
#endif
#include <TFT_eSPI.h>
//<Fonts !End!>

// ------------------------------------------------
// Defines for resources
// ------------------------------------------------
//<Resources !Start!>
#define IMG_EXIT_G24_SEL          "/exit_g24.jpg"
#define IMG_EXIT_N24              "/exit_n24.jpg"
#define IMG_LOGO                  "/logo.jpg"
//<Resources !End!>

// ------------------------------------------------
// Enumerations for pages, elements, fonts, images
// ------------------------------------------------
//<Enum !Start!>
enum {E_PG_MAIN,E_PG_POPUP_MSG_QUIT};
enum {E_ELEM_BOX1,E_ELEM_BTN1,E_ELEM_BTN_QUIT,E_ELEM_IMAGE_LOGO
      ,E_ELEM_TEXT_PRESSED,E_ELEM_TEXT_TITLE};
// Must use separate enum for fonts with MAX_FONT at end to use gslc_FontSet.
enum {E_FREESANS12,E_FREESANS9,MAX_FONT};
//<Enum !End!>

// ------------------------------------------------
// Instantiate the GUI
// ------------------------------------------------

// ------------------------------------------------
// Define the maximum number of elements and pages
// ------------------------------------------------
//<ElementDefines !Start!>
#define MAX_PAGE                2

#define MAX_ELEM_PG_MAIN 3 // # Elems total on page
#define MAX_ELEM_PG_MAIN_RAM MAX_ELEM_PG_MAIN // # Elems in RAM

#define MAX_ELEM_PG_POPUP_MSG_QUIT 3 // # Elems total on page
#define MAX_ELEM_PG_POPUP_MSG_QUIT_RAM MAX_ELEM_PG_POPUP_MSG_QUIT // # Elems in RAM
//<ElementDefines !End!>

// ------------------------------------------------
// Create element storage
// ------------------------------------------------
gslc_tsGui                      m_gui;
gslc_tsDriver                   m_drv;
gslc_tsFont                     m_asFont[MAX_FONT];
gslc_tsPage                     m_asPage[MAX_PAGE];

//<GUI_Extra_Elements !Start!>
gslc_tsElem                     m_asPage1Elem[MAX_ELEM_PG_MAIN_RAM];
gslc_tsElemRef                  m_asPage1ElemRef[MAX_ELEM_PG_MAIN];
gslc_tsElem                     m_asPopup1Elem[MAX_ELEM_PG_POPUP_MSG_QUIT_RAM];
gslc_tsElemRef                  m_asPopup1ElemRef[MAX_ELEM_PG_POPUP_MSG_QUIT];

#define MAX_STR                 100

//<GUI_Extra_Elements !End!>

// ------------------------------------------------
// Program Globals
// ------------------------------------------------

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
  gslc_tsElem* pElem = gslc_GetElemFromRef(&m_gui,pElemRef);

  if ( eTouch == GSLC_TOUCH_UP_IN ) {
    // From the element's ID we can determine which button was pressed.
    switch (pElem->nId) {
//<Button Enums !Start!>
      case E_ELEM_BTN_QUIT:
        //TODO- Check the code to see what else you may need to add
        gslc_PopupShow(&m_gui, E_PG_POPUP_MSG_QUIT, true);
        break;
      case E_ELEM_BTN1:
        //TODO- Check the code to see what else you may need to add
        gslc_PopupHide(&m_gui);
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
  gslc_PageAdd(&m_gui,E_PG_MAIN,m_asPage1Elem,MAX_ELEM_PG_MAIN_RAM,m_asPage1ElemRef,MAX_ELEM_PG_MAIN);
  gslc_PageAdd(&m_gui,E_PG_POPUP_MSG_QUIT,m_asPopup1Elem,MAX_ELEM_PG_POPUP_MSG_QUIT_RAM,m_asPopup1ElemRef,MAX_ELEM_PG_POPUP_MSG_QUIT);

  // NOTE: The current page defaults to the first page added. Here we explicitly
  //       ensure that the main page is the correct page no matter the add order.
  gslc_SetPageCur(&m_gui,E_PG_MAIN);
  
  // Set Background to a flat color
  gslc_SetBkgndColor(&m_gui,GSLC_COL_BLACK);

  // -----------------------------------
  // PAGE: E_PG_MAIN
  
 
  // Create E_ELEM_IMAGE_LOGO using Image 
  pElemRef = gslc_ElemCreateImg(&m_gui,E_ELEM_IMAGE_LOGO,E_PG_MAIN,(gslc_tsRect){50,13,219,60},
    gslc_GetImageFromFile(IMG_LOGO,GSLC_IMGREF_FMT_JPG));
  
  // Create E_ELEM_TEXT_TITLE text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TEXT_TITLE,E_PG_MAIN,(gslc_tsRect){38,80,244,29},
    (char*)"Using SPIFFS Example",0,E_FREESANS12);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_WHITE);
  
  // Create E_ELEM_BTN_QUIT button with image label
  pElemRef = gslc_ElemCreateBtnImg(&m_gui,E_ELEM_BTN_QUIT,E_PG_MAIN,(gslc_tsRect){275,200,32,32},
          gslc_GetImageFromFile(IMG_EXIT_N24,GSLC_IMGREF_FMT_JPG),
          gslc_GetImageFromFile(IMG_EXIT_G24_SEL,GSLC_IMGREF_FMT_JPG),
          &CbBtnCommon);

  // -----------------------------------
  // PAGE: E_PG_POPUP_MSG_QUIT
  
   
  // Create E_ELEM_BOX1 box
  pElemRef = gslc_ElemCreateBox(&m_gui,E_ELEM_BOX1,E_PG_POPUP_MSG_QUIT,(gslc_tsRect){85,38,155,150});
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_GRAY,GSLC_COL_GRAY_LT2,GSLC_COL_GRAY_LT2);
  
  // Create E_ELEM_TEXT_PRESSED text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TEXT_PRESSED,E_PG_POPUP_MSG_QUIT,(gslc_tsRect){90,89,141,23},
    (char*)"You Pressed Quit",0,E_FREESANS9);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_BLACK);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_GRAY,GSLC_COL_GRAY_LT2,GSLC_COL_GRAY_LT2);
  
  // create E_ELEM_BTN1 button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN1,E_PG_POPUP_MSG_QUIT,
    (gslc_tsRect){140,141,40,40},(char*)"OK",0,E_FREESANS9,&CbBtnCommon);
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
  delay(1000);  // NOTE: Some devices require a delay after Serial.begin() before serial port can be used
 
  gslc_InitDebug(&DebugOut);

  if (!gslc_Init(&m_gui,&m_drv,m_asPage,MAX_PAGE,m_asFont,MAX_FONT)) { return; }


  // ------------------------------------------------
  // Load Fonts
  // ------------------------------------------------
//<Load_Fonts !Start!>
    if (!gslc_FontSet(&m_gui,E_FREESANS12,GSLC_FONTREF_PTR,&FreeSans12pt7b,1)) { return; }
    if (!gslc_FontSet(&m_gui,E_FREESANS9,GSLC_FONTREF_PTR,&FreeSans9pt7b,1)) { return; }
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
    
}
