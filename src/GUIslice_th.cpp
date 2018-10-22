// Abstract touch handler (TH) class
// This is the abstract base class for creating specific touch handlers 
// The touch handler performs the adaption in between the GUIslice framework and any touch driver
// The touch handler used is specified in the main program by calling gslc_InitTouchHandler(&touchHandler);


#include "GUIslice_th.h"


///////////////////////////////////////////////////
// Point x,y,z Class for usage in the touch handler

THPoint::THPoint(void) {
  x = y = z = 0;
}

THPoint::THPoint(int16_t x0, int16_t y0, int16_t z0) {
  x = x0;
  y = y0;
  z = z0;
}

bool THPoint::operator==(THPoint p1) {
  return  ((p1.x == x) && (p1.y == y) && (p1.z == z));
}

bool THPoint::operator!=(THPoint p1) {
  return  ((p1.x != x) || (p1.y != y) || (p1.z != z));
}


///////////////////////////////////////////////////
// Abstract TouchHandler - you have to inherit this 

void TouchHandler::setSize(uint16_t _disp_xSize, uint16_t _disp_ySize)
{
    disp_xSize = _disp_xSize;
    disp_ySize = _disp_ySize;
}    

void TouchHandler::setCalibration(uint16_t _ts_xMin, uint16_t _ts_xMax, uint16_t _ts_yMin, uint16_t _ts_yMax)
{
    ts_xMin = _ts_xMin;
    ts_xMax = _ts_xMax;
    ts_yMin = _ts_yMin;
    ts_yMax = _ts_yMax;
}    

void TouchHandler::setSwapFlip(bool _swapXY,bool _flipX,bool _flipY)
{
    swapXY = _swapXY;
    flipX = _flipX;
    flipY = _flipY;
}

THPoint TouchHandler::scale(THPoint pIn)
{
    THPoint pOut;
    
    pOut.x = map(pIn.x, ts_xMin,ts_xMax, 0,disp_xSize);
    pOut.y = map(pIn.y, ts_yMin,ts_yMax, 0,disp_ySize);
    pOut.z = pIn.z;

    pOut.x = constrain(pOut.x,0,disp_xSize-1);
    pOut.y = constrain(pOut.y,0,disp_ySize-1);
    pOut.z = constrain(pOut.z,0,4095);

    if (swapXY)
    {
        uint16_t x = pOut.x;
        pOut.x = pOut.y;
        pOut.y = x;
    }

    if (flipX)
        pOut.x = ( (!swapXY) ? disp_xSize-1 : disp_ySize-1 ) - pOut.x;
        
    if (flipY)
        pOut.y = ( (!swapXY) ? disp_ySize-1 : disp_xSize-1 ) - pOut.y;
        
    //Serial.print("disp_xSize= ");Serial.println(disp_xSize);
        
    return pOut;
}


// overwrite this with your code to add initialisation of the touch driver used
void TouchHandler::begin(void) {
   return;
}


// overwrite this with your code to return the scaled touch coordinates
THPoint TouchHandler::getPoint(void) {
   return THPoint();
}


/////////////////////////////////
// init and set the touch handler 

// Pointer to touch handler used by GUIslice 
TouchHandler *pTouchHandler = NULL;  // NULL => no handler is available

// Init and set the touch hander
void gslc_InitTouchHandler(TouchHandler *pTH) {
    //begin
    pTH->begin();
    //set the touch handler to be used by GUIslice_drv_...
    pTouchHandler = pTH;
}

// Get the touch handler
TouchHandler* gslc_getTouchHandler(void)
{
    return pTouchHandler;
}

