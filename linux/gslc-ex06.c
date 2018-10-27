//
// GUIslice Library Examples
// - Calvin Hass
// - https://www.impulseadventure.com/elec/guislice-gui.html
// - https://github.com/ImpulseAdventure/GUIslice
// - Example 06 (LINUX):
//     Example of clipping plus draw and tick callback function
//

#include "GUIslice.h"
#include "GUIslice_ex.h"
#include "GUIslice_drv.h"

#include <math.h>
#include <libgen.h>       // For path parsing


// Defines for resources
#define MAX_PATH  255
#define FONT1 "/usr/share/fonts/truetype/noto/NotoSans-Regular.ttf"
#define IMG_LOGO        "/res/logo1-200x40.bmp"
char m_strImgLogo[MAX_PATH];

// Enumerations for pages, elements, fonts, images
enum {E_PG_MAIN};
enum {E_ELEM_BTN_QUIT,E_ELEM_TXT_COUNT,E_ELEM_PROGRESS,
      E_ELEM_DATAX,E_ELEM_DATAY,E_ELEM_DATAZ,E_ELEM_SCAN,
      E_ELEM_CHECK1};
enum {E_FONT_BTN,E_FONT_TXT};

bool      m_bQuit = false;

// Free-running counter for display
unsigned  m_nCount = 0;

float     m_fCoordX = 0;
float     m_fCoordY = 0;
float     m_fCoordZ = 0;

// Instantiate the GUI
#define MAX_PAGE            1
#define MAX_FONT            5
#define MAX_ELEM_PG_MAIN    20

gslc_tsGui                  m_gui;
gslc_tsDriver               m_drv;
gslc_tsFont                 m_asFont[MAX_FONT];
gslc_tsPage                 m_asPage[MAX_PAGE];
gslc_tsElem                 m_asPageElem[MAX_ELEM_PG_MAIN];
gslc_tsElemRef              m_asPageElemRef[MAX_ELEM_PG_MAIN];

gslc_tsXGauge               m_sXGauge;
gslc_tsXCheckbox            m_asXCheck[1];

#define MAX_STR             100

// Define an origin for the scanner
int16_t   m_nOriginX = 0;
int16_t   m_nOriginY = 0;

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

// Scanner drawing callback function
// - This is called when E_ELEM_SCAN is being rendered
// - The scanner implements a custom element that replaces
//   the Box element type with a custom rendering function.
// - Clipping region is updated temporarily during this draw
bool CbDrawScanner(void* pvGui,void* pvElemRef,gslc_teRedrawType eRedraw)
{
  int nInd;

  // Typecast the parameters to match the GUI and element types
  gslc_tsGui*     pGui      = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef  = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem*    pElem     = gslc_GetElemFromRef(pGui,pElemRef);

  // Create shorthand variables for the origin
  uint16_t  nX = pElem->rElem.x + m_nOriginX;
  uint16_t  nY = pElem->rElem.y + m_nOriginY;

  // Draw the background
  gslc_tsRect rInside = pElem->rElem;
  rInside = gslc_ExpandRect(rInside,-1,-1);
  gslc_DrawFillRect(pGui,rInside,pElem->colElemFill);

  // Enable localized clipping
  gslc_SetClipRect(pGui,&rInside);

  // Perform the drawing of example graphic primitives
  gslc_DrawLine(pGui,nX,nY-200,nX,nY+200,GSLC_COL_GRAY_DK2);
  gslc_DrawLine(pGui,nX-200,nY,nX+200,nY,GSLC_COL_GRAY_DK2);

  gslc_DrawFrameRect(pGui,(gslc_tsRect){nX-30,nY-20,60,40},GSLC_COL_BLUE_DK2);
  for (nInd=-5;nInd<=5;nInd++) {
    gslc_DrawLine(pGui,nX,nY,nX+nInd*20,nY+100,GSLC_COL_PURPLE);
  }

  gslc_DrawFillRect(pGui,(gslc_tsRect){nX+1,nY+1,10,10},GSLC_COL_RED_DK2);
  gslc_DrawFillRect(pGui,(gslc_tsRect){nX+1,nY-10,10,10},GSLC_COL_GREEN_DK2);
  gslc_DrawFillRect(pGui,(gslc_tsRect){nX-10,nY+1,10,10},GSLC_COL_BLUE_DK2);
  gslc_DrawFillRect(pGui,(gslc_tsRect){nX-10,nY-10,10,10},GSLC_COL_YELLOW);

  // Disable clipping region
  gslc_SetClipRect(pGui,NULL);

  // Draw the frame
  gslc_DrawFrameRect(pGui,pElem->rElem,pElem->colElemFrame);

  // Clear the redraw flag
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_NONE);

  return true;
}

// Demonstrate tick callback for scanner window
// - This is called whenever gslc_Update() is called
// - In this example, it simply updates the relative
//   origin of the view which will shift the display
bool CbTickScanner(void* pvGui,void* pvScope)
{
  gslc_tsGui*     pGui      = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef  = (gslc_tsElemRef*)(pvScope);
  //gslc_tsElem*    pElem     = gslc_GetElemFromRef(pGui,pElemRef);

  m_fCoordX = 50+25.0*(sin(m_nCount/250.0));
  m_fCoordY = 50+15.0*(cos(m_nCount/175.0));
  m_fCoordZ = 13.02;

  // Adjust the scanner's origin for fun
  m_nOriginX = (int16_t)m_fCoordX;
  m_nOriginY = (int16_t)m_fCoordY;

  // Manually mark the scanner element as needing redraw
  // since we have shifted its relative coordinates (via origin)
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_FULL);

  return true;
}

// Button callbacks
bool CbBtnQuit(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY)
{
  if (eTouch == GSLC_TOUCH_UP_IN) {
    m_bQuit = true;
  }
  return true;
}

// Create the default elements on each page
// - strPath: Path to executable passed in to locate resource files
bool InitOverlays(char *strPath)
{
  gslc_tsElemRef*  pElemRef = NULL;

  gslc_PageAdd(&m_gui,E_PG_MAIN,m_asPageElem,MAX_ELEM_PG_MAIN,m_asPageElemRef,MAX_ELEM_PG_MAIN);

  // Background flat color
  gslc_SetBkgndColor(&m_gui,GSLC_COL_GRAY_DK3);

  // Graphic logo
  // - Extra code to demonstrate path generation based on location of executable
  strncpy(m_strImgLogo,strPath,MAX_PATH);
  strncat(m_strImgLogo,IMG_LOGO,MAX_PATH);
  pElemRef = gslc_ElemCreateImg(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){160-100,5,200,40},
    gslc_GetImageFromFile(m_strImgLogo,GSLC_IMGREF_FMT_BMP16));


  // Create background box
  pElemRef = gslc_ElemCreateBox(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){10,50,300,150});
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_WHITE,GSLC_COL_BLACK,GSLC_COL_BLACK);

  // Create Quit button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_QUIT,E_PG_MAIN,
    (gslc_tsRect){40,210,50,20},"QUIT",0,E_FONT_BTN,&CbBtnQuit);

  // Create counter
  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){20,60,50,10},
    "Searches:",0,E_FONT_TXT);
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TXT_COUNT,E_PG_MAIN,(gslc_tsRect){80,60,50,10},
    "",0,E_FONT_TXT);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_GRAY_LT2);

  // Create progress bar
  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){20,80,50,10},
    "Progress:",0,E_FONT_TXT);
  pElemRef = gslc_ElemXGaugeCreate(&m_gui,E_ELEM_PROGRESS,E_PG_MAIN,&m_sXGauge,(gslc_tsRect){80,80,50,10},
    0,100,0,GSLC_COL_GREEN,false);


  // Create other labels
  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){40,100,50,10},
    "Coord X:",0,E_FONT_TXT);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_WHITE);
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_DATAX,E_PG_MAIN,(gslc_tsRect){100,100,50,10},
    "",0,E_FONT_TXT);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_GRAY_LT2);

  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){40,120,50,10},
    "Coord Y:",0,E_FONT_TXT);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_WHITE);
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_DATAY,E_PG_MAIN,(gslc_tsRect){100,120,50,10},
    "",0,E_FONT_TXT);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_GRAY_LT2);

  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){40,140,50,10},
    "Coord Z:",0,E_FONT_TXT);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_WHITE);
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_DATAZ,E_PG_MAIN,(gslc_tsRect){100,140,50,10},
    "",0,E_FONT_TXT);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_GRAY_LT2);

  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){20,170,50,10},
    "Control:",0,E_FONT_TXT);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_ORANGE);

  pElemRef = gslc_ElemXCheckboxCreate(&m_gui,E_ELEM_CHECK1,E_PG_MAIN,&m_asXCheck[0],
    (gslc_tsRect){80,170,20,20},false,GSLCX_CHECKBOX_STYLE_X,GSLC_COL_BLUE_LT2,false);

  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){110,170,50,10},
    "Enable",0,E_FONT_TXT);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_GRAY_LT1);

  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){120,210,170,20},
    "Example of GUIslice C library",0,E_FONT_BTN);
  gslc_ElemSetTxtAlign(&m_gui,pElemRef,GSLC_ALIGN_MID_LEFT);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_RED_LT2);
  gslc_ElemSetFillEn(&m_gui,pElemRef,false);

  // --------------------------------------------------------------------------
  // Create scanner
  pElemRef = gslc_ElemCreateBox(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){190-1-2,75-1-12,100+2+4,100+2+10+4});
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_BLUE_LT2,GSLC_COL_BLACK,GSLC_COL_BLACK);

  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){190,75-11,100,10},
    "SCANNER",0,E_FONT_TXT);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_BLUE_DK2);
  gslc_ElemSetTxtAlign(&m_gui,pElemRef,GSLC_ALIGN_MID_MID);

  pElemRef = gslc_ElemCreateBox(&m_gui,E_ELEM_SCAN,E_PG_MAIN,(gslc_tsRect){190-1,75-1,100+2,100+2});
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_BLUE_LT2,GSLC_COL_BLACK,GSLC_COL_BLACK);
  // Set the callback function to handle all drawing for the element
  gslc_ElemSetDrawFunc(&m_gui,pElemRef,&CbDrawScanner);
  // Set the callback function to update content automatically
  gslc_ElemSetTickFunc(&m_gui,pElemRef,&CbTickScanner);

  // --------------------------------------------------------------------------

  return true;
}

int main( int argc, char* args[] )
{
  bool              bOk = true;
  char              acTxt[100];

  // -----------------------------------
  // Initialize
  gslc_InitDebug(&DebugOut);
  UserInitEnv();
  if (!gslc_Init(&m_gui,&m_drv,m_asPage,MAX_PAGE,m_asFont,MAX_FONT)) { exit(1); }

  // Load Fonts
  bOk = gslc_FontAdd(&m_gui,E_FONT_BTN,GSLC_FONTREF_FNAME,FONT1,12);
  if (!bOk) { fprintf(stderr,"ERROR: FontAdd failed\n"); exit(1); }
  bOk = gslc_FontAdd(&m_gui,E_FONT_TXT,GSLC_FONTREF_FNAME,FONT1,10);
  if (!bOk) { fprintf(stderr,"ERROR: FontAdd failed\n"); exit(1); }


  // -----------------------------------
  // Create the graphic elements
  InitOverlays(dirname(args[0])); // Pass executable path to find resource files

  // Start up display on main page
  gslc_SetPageCur(&m_gui,E_PG_MAIN);

  // Save some element references for quick access
  gslc_tsElemRef*  pElemCount      = gslc_PageFindElemById(&m_gui,E_PG_MAIN,E_ELEM_TXT_COUNT);
  gslc_tsElemRef*  pElemDataX      = gslc_PageFindElemById(&m_gui,E_PG_MAIN,E_ELEM_DATAX);
  gslc_tsElemRef*  pElemDataY      = gslc_PageFindElemById(&m_gui,E_PG_MAIN,E_ELEM_DATAY);
  gslc_tsElemRef*  pElemDataZ      = gslc_PageFindElemById(&m_gui,E_PG_MAIN,E_ELEM_DATAZ);
  gslc_tsElemRef*  pElemProgress   = gslc_PageFindElemById(&m_gui,E_PG_MAIN,E_ELEM_PROGRESS);

  // -----------------------------------
  // Main event loop

  m_bQuit = false;
  while (!m_bQuit) {

    // Update the data display values
    m_nCount++;

    // -----------------------------------------------

    // Perform any immediate updates on active page
    snprintf(acTxt,MAX_STR,"%u",m_nCount);
    gslc_ElemSetTxtStr(&m_gui,pElemCount,acTxt);

    snprintf(acTxt,MAX_STR,"%4.2f",m_fCoordX-50);
    gslc_ElemSetTxtStr(&m_gui,pElemDataX,acTxt);
    snprintf(acTxt,MAX_STR,"%4.2f",m_fCoordY-50);
    gslc_ElemSetTxtStr(&m_gui,pElemDataY,acTxt);
    snprintf(acTxt,MAX_STR,"%4.2f",m_fCoordZ);
    gslc_ElemSetTxtStr(&m_gui,pElemDataZ,acTxt);
    gslc_ElemSetTxtCol(&m_gui,pElemDataZ,(m_fCoordY>50)?GSLC_COL_GREEN_LT2:GSLC_COL_RED_DK2);

    gslc_ElemXGaugeUpdate(&m_gui,pElemProgress,50+50*sin(m_nCount/500.0));

    // -----------------------------------------------

    // Periodically call GUIslice update function
    gslc_Update(&m_gui);

  } // bQuit


  // -----------------------------------
  // Close down display

  gslc_Quit(&m_gui);

  return 0;
}

