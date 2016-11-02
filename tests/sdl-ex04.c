//
// microSDL GUI Library Examples
// - Calvin Hass
// - http://www.impulseadventure.com/elec/microsdl-sdl-gui.html
// - Example 04: Dynamic content
//   Demonstrates two types of Checkbox elements
//

#include "microsdl.h"
#include "microsdl_ex.h"

// Defines for resources
#define FONT_DROID_SANS "/usr/share/fonts/truetype/droid/DroidSans.ttf"

// Enumerations for pages, elements, fonts, images
enum {E_PG_MAIN};
enum {E_ELEM_BOX,E_ELEM_BTN_QUIT,E_ELEM_TXT_COUNT,E_ELEM_PROGRESS,
      E_ELEM_CHECK1,E_ELEM_CHECK2,E_ELEM_SLIDER,E_ELEM_TXT_SLIDER};
enum {E_FONT_BTN,E_FONT_TXT};

// Free-running counter for display
unsigned m_nCount = 0;

// Instantiate the GUI
#define MAX_ELEM      30
#define MAX_FONT      10
microSDL_tsGui        m_gui;
microSDL_tsElem       m_asElem[MAX_ELEM];
microSDL_tsFont       m_asFont[MAX_FONT];
microSDL_tsXGauge     m_sXGauge;
microSDL_tsXCheckbox  m_asXCheck[2];
microSDL_tsXSlider    m_sXSlider;

// Create page elements
bool InitOverlays()
{
  int nElemId;

  // Background flat color
  microSDL_SetBkgndColor(&m_gui,MSDL_COL_GRAY_DK);

  // Create background box
  nElemId = microSDL_ElemCreateBox(&m_gui,E_ELEM_BOX,E_PG_MAIN,(SDL_Rect){10,50,300,150});
  microSDL_ElemSetCol(&m_gui,nElemId,MSDL_COL_WHITE,MSDL_COL_BLACK,MSDL_COL_BLACK);

  // Create Quit button with text label
  nElemId = microSDL_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_QUIT,E_PG_MAIN,
    (SDL_Rect){160,80,80,40},"Quit",E_FONT_BTN);

  // Create counter
  nElemId = microSDL_ElemCreateTxt(&m_gui,MSDL_ID_AUTO,E_PG_MAIN,(SDL_Rect){20,60,50,10},
    "Count:",E_FONT_TXT);
  nElemId = microSDL_ElemCreateTxt(&m_gui,E_ELEM_TXT_COUNT,E_PG_MAIN,(SDL_Rect){80,60,50,10},
    "",E_FONT_TXT);
  microSDL_ElemSetTxtCol(&m_gui,nElemId,MSDL_COL_YELLOW);

  // Create progress bar
  nElemId = microSDL_ElemCreateTxt(&m_gui,MSDL_ID_AUTO,E_PG_MAIN,(SDL_Rect){20,80,50,10},
    "Progress:",E_FONT_TXT);
  nElemId = microSDL_ElemXGaugeCreate(&m_gui,E_ELEM_PROGRESS,E_PG_MAIN,&m_sXGauge,(SDL_Rect){80,80,50,10},
    0,100,0,MSDL_COL_GREEN_DK,false);
  
  // Create checkbox 1
  nElemId = microSDL_ElemCreateTxt(&m_gui,MSDL_ID_AUTO,E_PG_MAIN,(SDL_Rect){20,110,20,20},
    "Check1:",E_FONT_TXT);
  nElemId = microSDL_ElemXCheckboxCreate(&m_gui,E_ELEM_CHECK1,E_PG_MAIN,&m_asXCheck[0],(SDL_Rect){80,110,20,20},
    MSDL_COL_ORANGE,false);

  // Create checkbox 2
  nElemId = microSDL_ElemCreateTxt(&m_gui,MSDL_ID_AUTO,E_PG_MAIN,(SDL_Rect){20,150,20,20},
    "Check2:",E_FONT_TXT);
  nElemId = microSDL_ElemXCheckboxCreate(&m_gui,E_ELEM_CHECK2,E_PG_MAIN,&m_asXCheck[1],(SDL_Rect){80,150,20,20},
    MSDL_COL_ORANGE,false);
  
  // Create slider
  nElemId = microSDL_ElemXSliderCreate(&m_gui,E_ELEM_SLIDER,E_PG_MAIN,&m_sXSlider,(SDL_Rect){160,140,100,20},
    0,100,60,5,false);
  microSDL_ElemXSliderSetStyle(&m_gui,E_ELEM_SLIDER,true,(SDL_Color){0,0,128},10,
          5,(SDL_Color){64,64,64});
  nElemId = microSDL_ElemCreateTxt(&m_gui,E_ELEM_TXT_SLIDER,E_PG_MAIN,(SDL_Rect){160,160,60,20},
    "Slider: ???",E_FONT_TXT);
  
  return true;
}


int main( int argc, char* args[] )
{
  bool                bOk = true;
  bool                bQuit = false;  
  int                 nClickX,nClickY;
  unsigned            nClickPress;
  int                 nTrackElemClicked;
  char                acTxt[100];

  // -----------------------------------
  // Initialize

  microSDL_InitEnv(&m_gui);

  if (!microSDL_Init(&m_gui,m_asElem,MAX_ELEM,m_asFont,MAX_FONT,NULL,0)) { exit(1); }

  microSDL_InitTs(&m_gui,"/dev/input/touchscreen");

  // Load Fonts
  bOk = microSDL_FontAdd(&m_gui,E_FONT_BTN,FONT_DROID_SANS,12);
  if (!bOk) { fprintf(stderr,"ERROR: FontAdd failed\n"); exit(1); }
  bOk = microSDL_FontAdd(&m_gui,E_FONT_TXT,FONT_DROID_SANS,8);
  if (!bOk) { fprintf(stderr,"ERROR: FontAdd failed\n"); exit(1); }


  // -----------------------------------
  // Start display
  InitOverlays();

  // Start up display on main page
  microSDL_SetPageCur(&m_gui,E_PG_MAIN);

  // -----------------------------------
  // Main event loop

  bQuit = false;
  while (!bQuit) {

    // General counter
    m_nCount++;

    // -----------------------------------

    // Update elements on active page
    sprintf(acTxt,"%u",m_nCount);
    microSDL_ElemSetTxtStr(&m_gui,E_ELEM_TXT_COUNT,acTxt);

    microSDL_ElemXGaugeUpdate(&m_gui,E_ELEM_PROGRESS,((m_nCount/200)%100));
    
    // TODO: Replace with Slider callback
    int nPos = microSDL_ElemXSliderGetPos(&m_gui,E_ELEM_SLIDER);
    sprintf(acTxt,"Slider: %u",nPos);
    microSDL_ElemSetTxtStr(&m_gui,E_ELEM_TXT_SLIDER,acTxt);
    
    
    // Periodically redraw screen in case of any changes
    microSDL_PageRedrawGo(&m_gui);

    // -----------------------------------
  
    // Poll for touchscreen presses
    if (microSDL_GetTsClick(&m_gui,&nClickX,&nClickY,&nClickPress)) {
 
      // Track the touch event and find any associated object
      microSDL_TrackClick(&m_gui,nClickX,nClickY,nClickPress);
      nTrackElemClicked = microSDL_GetTrackElemClicked(&m_gui);
     
      // Any selectable object clicked? (MSDL_ID_NONE if no)
      if (nTrackElemClicked == E_ELEM_BTN_QUIT) {
        // Quit button pressed
        bQuit = true;
      }

      // Clear click event
      microSDL_ClearTrackElemClicked(&m_gui);
  
    } // Touchscreen press
  } // bQuit

  // Read checkbox state:
  // bool bCheck = microSDL_ElemXCheckboxGetState(&m_gui,E_ELEM_CHECK1);

  // -----------------------------------
  // Close down display

  microSDL_Quit(&m_gui);

  return 0;
}

