<BACKGROUND>
  
  // Background flat color
  gslc_SetBkgndColor(&m_gui,$BACKGROUND_COLOR$);
<STOP>
<BOX>
   
  // Create $WIDGET_ENUM$ box
  gslc_ElemCreateBox_P(&m_gui,$WIDGET_ENUM$,$PAGE_ENUM$,$X$,$Y$,$WIDTH$,$HEIGHT$,
    $FRAME_COLOR$,$FILL_COLOR$,true,true,$DRAWFUNC$,$TICKFUNC$);
<STOP>
<BUTTON_CB>
// Common Button callback
bool CbBtnCommon(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY)
{
  gslc_tsElemRef* pElemRef = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem* pElem = pElemRef->pElem;
  int16_t nElemId = pElem->nId;

  if ( eTouch == GSLC_TOUCH_UP_IN ) {
    // From the element's ID we can determine which button was pressed.
    switch (pElem->nId) {
//<Button Enums !Start!>
//<Button Enums !End!>
      default:
        break;
    }
  }
  return true;
}
<STOP>
<BUTTON_CB_LOOP>
      case $WIDGET_ENUM$:
        //TODO- Replace with button handling code
        break;
<STOP>
<BUTTON_CB_CHGPAGE>
      case $WIDGET_ENUM$:
        gslc_SetPageCur(&m_gui,$PAGE_ENUM$);
        break;
<STOP>
<CHECKBOX>
   
  // create checkbox $WIDGET_ENUM$
  gslc_ElemXCheckboxCreate_P(&m_gui,$WIDGET_ENUM$,$PAGE_ENUM$,$X$,$Y$,$WIDTH$,$HEIGHT$,
    GSLC_GROUP_ID_NONE,false,GSLCX_CHECKBOX_STYLE_X,$MARK_COLOR$,$CHECKED$);
<STOP>
<COLOR>
  gslc_ElemSetCol(&m_gui,pElemRef,$FRAME_COLOR$,$FILL_COLOR$,$GLOW_COLOR$);
<STOP>
<DRAWBOX_CB>
 
// Scanner drawing callback function
// - This is called when E_ELEM_SCAN is being rendered
bool CbDrawScanner(void* pvGui,void* pvElemRef,gslc_teRedrawType eRedraw)
{
  int nInd;

  // Typecast the parameters to match the GUI and element types
  gslc_tsGui*     pGui      = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef  = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem*    pElem     = pElemRef->pElem;

  // Create shorthand variables for the origin
  int16_t  nX = pElem->rElem.x;
  int16_t  nY = pElem->rElem.y;

  // Draw the background
  gslc_tsRect rInside = pElem->rElem;
  rInside = gslc_ExpandRect(rInside,-1,-1);
  gslc_DrawFillRect(pGui,rInside,pElem->colElemFill);

  // Enable localized clipping
  gslc_SetClipRect(pGui,&rInside);

  //TODO - Add your drawing graphic primitives

  // Disable clipping region
  gslc_SetClipRect(pGui,NULL);

  // Draw the frame
  gslc_DrawFrameRect(pGui,pElem->rElem,pElem->colElemFrame);

  // Clear the redraw flag
  gslc_ElemSetRedraw(&m_gui,pElemRef,GSLC_REDRAW_NONE);

  return true;
}
<STOP>
<GRAPH>

  // Create graph $WIDGET_ENUM$
  pElemRef = gslc_ElemXGraphCreate(&m_gui,$WIDGET_ENUM$,$PAGE_ENUM$,
    &m_sGraph$ID$,(gslc_tsRect){$X$,$Y$,$WIDTH$,$HEIGHT$},$FONT_ID$,(int16_t*)&m_anGraphBuf$ID$,
        $ROWS$,$GRAPH_COLOR$);
  gslc_ElemXGraphSetStyle(&m_gui,pElemRef, $STYLE$, 5);
<STOP>
<FONT_LOAD>
    if (!gslc_FontAdd(&m_gui,$FONT_ID$,$FONT_REFTYPE$,$FONT_REF$,$FONT_SZ$)) { return; }
<STOP>
<IMAGE>
 
  // Get Image $IMAGE_DEFINE$ from SD Card
  pElemRef = gslc_ElemCreateImg(&m_gui,$WIDGET_ENUM$,$PAGE_ENUM$,(gslc_tsRect){$X$,$Y$,$WIDTH$,$HEIGHT$},
    gslc_GetImageFromSD((const char*)$IMAGE_DEFINE$,$IMAGE_FORMAT$));
<STOP>
<IMGBUTTON>
  
  // Create $WIDGET_ENUM$ button with image label
  pElemRef = gslc_ElemCreateBtnImg(&m_gui,$WIDGET_ENUM$,$PAGE_ENUM$,(gslc_tsRect){$X$,$Y$,$WIDTH$,$HEIGHT$},
          gslc_GetImageFromSD((const char*)$IMAGE_DEFINE$,$IMAGE_FORMAT$),
          gslc_GetImageFromSD((const char*)$IMAGE_SEL_DEFINE$,$IMAGE_FORMAT$),
          &CbBtnCommon);
<STOP>
<IMAGETRANSPARENT>
          gslc_ElemSetFillEn(&m_gui,pElemRef,$BOOL$);
<STOP>
<PROGMEM>
// Define the maximum number of elements per page
// - To enable the same code to run on devices that support storing
//   data into Flash (PROGMEM) and those that don't, we can make the
//   number of elements in Flash dependent upon GSLC_USE_PROGMEM
// - This should allow both Arduino and ARM Cortex to use the same code
<STOP>
<PROGRESSBAR>

  // Create progress bar $WIDGET_ENUM$ 
  gslc_ElemXGaugeCreate_P(&m_gui,$WIDGET_ENUM$,$PAGE_ENUM$,$X$,$Y$,$WIDTH$,$HEIGHT$,$MIN$,$MAX$,$VALUE$,
    $FRAME_COLOR$,$FILL_COLOR$,$MARK_COLOR$,$CHECKED$);
<STOP>
<RADIOBUTTON>
  
  // Create radio button $WIDGET_ENUM$
  gslc_ElemXCheckboxCreate_P(&m_gui,$WIDGET_ENUM$,$PAGE_ENUM$,$X$,$Y$,$WIDTH$,$HEIGHT$,
          $GROUP_ID$,true,GSLCX_CHECKBOX_STYLE_ROUND,$MARK_COLOR$,$CHECKED$);
<STOP>
<SLIDER>
  // Create slider $WIDGET_ENUM$
  gslc_ElemXSliderCreate_P(&m_gui,$WIDGET_ENUM$,$PAGE_ENUM$,$X$,$Y$,$WIDTH$,$HEIGHT$,
    $MIN$,$MAX$,$VALUE$,$THUMBSZ$,$CHECKED$,$FRAME_COLOR$,$FILL_COLOR$);
  pElemRef = gslc_PageFindElemById(&m_gui,$PAGE_ENUM$,$WIDGET_ENUM$);
  gslc_ElemXSliderSetStyle(&m_gui,pElemRef,$TRIMSTYLE$,$TRIM_COLOR$,$DIVISIONS$,$TICKSZ$,$MARK_COLOR$);
  gslc_ElemXSliderSetPosFunc(&m_gui,pElemRef,&CbSlidePos);
<STOP>
<SLIDER_CB>

// Callback function for when a slider's position has been updated
bool CbSlidePos(void* pvGui,void* pvElemRef,int16_t nPos)
{
  gslc_tsGui*     pGui      = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef  = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem*    pElem     = pElemRef->pElem;

  // From the element's ID we can determine which slider was updated.
  switch (pElem->nId) {
//<Slider Enums !Start!>
//<Slider Enums !End!>
    default:
      break;
  }

  return true;
}
<STOP>
<SLIDER_CB_LOOP>
    case $WIDGET_ENUM$:
      // Fetch the slider position
      nVal = gslc_ElemXSliderGetPos(pGui,pElemRef);
      //TODO- Replace with slider handling code
      break;
<STOP>
<TEXT>
  
  // Create $WIDGET_ENUM$ text label
  gslc_ElemCreateTxt_P(&m_gui,$WIDGET_ENUM$,$PAGE_ENUM$,$X$,$Y$,$WIDTH$,$HEIGHT$,"$TEXT$",&m_asFont[$FONT_COUNT$],
    $TEXT_COLOR$,$FRAME_COLOR$,$FILL_COLOR$,$TEXT_ALIGN$,false,true);
<STOP>
<TEXTBOX>
   
  // Create wrapping box for textbox $WIDGET_ENUM$ and scrollbar
  gslc_ElemCreateBox_P(&m_gui,$WIDGET_ENUM$,$PAGE_ENUM$,$X$,$Y$,$WIDTH$,$HEIGHT$,$FRAME_COLOR$,$FILL_COLOR$,true,true,NULL,NULL);
  
  // Create textbox $WIDGET_ENUM$
  // - NOTE: XTextbox does not have a FLASH-based version yet (ElemXTextboxCreate_P)
  pElemRef = gslc_ElemXTextboxCreate(&m_gui,$WIDGET_ENUM$,$PAGE_ENUM$,&m_sTextbox$ID$,
    (gslc_tsRect){$X$+2,$Y$+4,$WIDTH$-23,$HEIGHT$-8},$FONT_ID$,(char*)&m_acTextboxBuf$ID$,$ROWS$,$COLS$);
  gslc_ElemXTextboxWrapSet(&m_gui,pElemRef,$CHECKED$);
  gslc_ElemSetCol(&m_gui,pElemRef,$FRAME_COLOR$,$FILL_COLOR$,$FILL_COLOR$);

  // Create vertical scrollbar for textbox
  gslc_ElemXSliderCreate_P(&m_gui,E_SCROLLBAR$ID$,$PAGE_ENUM$,$X$+$WIDTH$-21,$Y$+4,20,$HEIGHT$-8,
    0,100,0,5,true,$FRAME_COLOR$,$FILL_COLOR$);
  pElemRef = gslc_PageFindElemById(&m_gui,E_PG_MAIN,E_SCROLLBAR$ID$);
  gslc_ElemXSliderSetPosFunc(&m_gui,pElemRef,&CbSlidePos);
<STOP>
<TEXT_UPDATE>
  
  // Create $WIDGET_ENUM$ text label
  static char mstr$ID$[$SIZE$] = "$TEXT$";
  gslc_ElemCreateTxt_P_R(&m_gui,$WIDGET_ENUM$,$PAGE_ENUM$,$X$,$Y$,$WIDTH$,$HEIGHT$,mstr$ID$,$SIZE$,&m_asFont[$FONT_COUNT$],
    $TEXT_COLOR$,$FILL_COLOR$,$FILL_COLOR$,$TEXT_ALIGN$,false,true);
<STOP>
<TICK_CB>

bool CbTickScanner(void* pvGui,void* pvScope)
{
  gslc_tsGui*   pGui      = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef  = (gslc_tsElemRef*)(pvScope);
  gslc_tsElem*    pElem     = pElemRef->pElem;


  return true;
}
<STOP>
<TXTBUTTON>
  
  // create $WIDGET_ENUM$ button with text label
  gslc_ElemCreateBtnTxt_P(&m_gui,$WIDGET_ENUM$,$PAGE_ENUM$,$X$,$Y$,$WIDTH$,$HEIGHT$,"$TEXT$",&m_asFont[$FONT_COUNT$],
    $TEXT_COLOR$,$FRAME_COLOR$,$FILL_COLOR$,$FRAME_COLOR$,
    $GLOW_COLOR$,GSLC_ALIGN_MID_MID,true,true,&CbBtnCommon,NULL);
<STOP>
<END>
