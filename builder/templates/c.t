//<File !Start!>
//<File !End!>
//
// GUIslice Builder Generated File
//
//

#include "GUIslice.h"
#include "GUIslice_ex.h"
#include "GUIslice_drv.h"

#include <libgen.h>       // For path parsing


// ------------------------------------------------
// Defines for resources
// ------------------------------------------------
#define MAX_PATH  255
//<PathStorage !Start!>
//<PathStorage !End!>
//<Fonts !Start!>
//<Fonts !End!>
//<Resources !Start!>
//<Resources !End!>

// ------------------------------------------------
// Enumerations for pages, elements, fonts, images
// ------------------------------------------------
//<Enum !Start!>
//<Enum !End!>

// ------------------------------------------------
// Instantiate the GUI
// ------------------------------------------------

// Define the maximum number of elements per page
//<ElementDefines !Start!>
//<ElementDefines !End!>

// GUI Elements
gslc_tsGui                      m_gui;
gslc_tsDriver                   m_drv;
gslc_tsFont                     m_asFont[MAX_FONT];
gslc_tsPage                     m_asPage[MAX_PAGE];

//<GUI_Extra_Elements !Start!>
//<GUI_Extra_Elements !End!>

// ------------------------------------------------
// Save some element references for update loop access
// ------------------------------------------------
//<Save_References !Start!>
//<Save_References !End!>

// ------------------------------------------------
// Program Globals
// ------------------------------------------------

// Configure environment variables suitable for display
// - These may need modification to match your system
//   environment and display type
// - Defaults for GSLC_DEV_FB and GSLC_DEV_TOUCH are in GUIslice_config.h
// - Note that the environment variable settings can
//   also be set directly within the shell via export
//   (or init script).
//   - eg. export TSLIB_FBDEVICE=/dev/fb1
void UserInitEnv()
{
#if defined(DRV_DISP_SDL1) || defined(DRV_DISP_SDL2)
  setenv((char*)"FRAMEBUFFER",GSLC_DEV_FB,1);
  setenv((char*)"SDL_FBDEV",GSLC_DEV_FB,1);
  setenv((char*)"SDL_VIDEODRIVER",GSLC_DEV_VID_DRV,1);
#endif

#if defined(DRV_TOUCH_TSLIB)
  setenv((char*)"TSLIB_FBDEVICE",GSLC_DEV_FB,1);
  setenv((char*)"TSLIB_TSDEVICE",GSLC_DEV_TOUCH,1);
  setenv((char*)"TSLIB_CALIBFILE",(char*)"/etc/pointercal",1);
  setenv((char*)"TSLIB_CONFFILE",(char*)"/etc/ts.conf",1);
  setenv((char*)"TSLIB_PLUGINDIR",(char*)"/usr/local/lib/ts",1);
#endif
}

// Define debug message function
static int16_t DebugOut(char ch) { fputc(ch,stderr); return 0; }

// ------------------------------------------------
// Callback Methods
// ------------------------------------------------
//<Button Callback !Start!>
//<Button Callback !End!>
//<Draw Callback !Start!>
//<Draw Callback !End!>
//<Tick Callback !Start!>
//<Tick Callback !End!>

// ------------------------------------------------
// Create page elements
// - strPath: Path to executable passed in to locate resource files
// ------------------------------------------------
bool InitGUI(char *strPath)
{
  gslc_tsElemRef* pElemRef = NULL;

//<InitGUI !Start!>
//<InitGUI !End!>

  return true;
}

int main( int argc, char* args[] )
{
  bool              bOk = true;
  char              acTxt[MAX_STR];

  // ------------------------------------------------
  // Initialize
  // ------------------------------------------------
  gslc_InitDebug(&DebugOut);
  UserInitEnv();
  
  if (!gslc_Init(&m_gui,&m_drv,m_asPage,MAX_PAGE,m_asFont,MAX_FONT)) { exit(1); }

  // ------------------------------------------------
  // Load Fonts
  // ------------------------------------------------
//<Load_Fonts !Start!>
//<Load_Fonts !End!>

  // ------------------------------------------------
  // Create graphic elements
  // ------------------------------------------------
  InitGUI(dirname(args[0])); // Pass executable path to find resource files

  // ------------------------------------------------
  // Save some element references for quick access
  // ------------------------------------------------
//<Quick_Access !Start!>
//<Quick_Access !End!>

  // ------------------------------------------------
  // Start up display on main page
  // ------------------------------------------------
//<Startup !Start!>
//<Startup !End!>

  // ------------------------------------------------
  // Main event loop
  // ------------------------------------------------

  m_bQuit = false;
  while (!m_bQuit) {

    // ----------------------------------------------
    // Update GUI Elements
    // ----------------------------------------------
    
    //TODO - Add update code for any text, gauges, or sliders
  
    // ----------------------------------------------
    // Periodically call GUIslice update function
    // ----------------------------------------------
    gslc_Update(&m_gui);

  } // bQuit

  // ------------------------------------------------
  // Close down display
  // ------------------------------------------------

  gslc_Quit(&m_gui);

  return 0;
}

