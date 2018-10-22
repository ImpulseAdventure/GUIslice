// Abstract touch handler (TH) class
// This is the abstract base class for creating specific touch handlers 
// The touch handler performs the adaption in between the GUIslice framework and any touch driver
// The touch handler used is specified in the main program by calling gslc_InitTouchHandler(&touchHandler);

#ifndef _GUISLICE_TH_H_
#define _GUISLICE_TH_H_

#include <Arduino.h>

///////////////////////////////////////////////////
// Point x,y,z Class for usage in the touch handler

class THPoint {
 public:
  THPoint(void);
  THPoint(int16_t x, int16_t y, int16_t z);
  
  bool operator==(THPoint);
  bool operator!=(THPoint);

  int16_t x, y, z;
};


///////////////////////////////////////////////////
// Abstract TouchHandler - you have to inherit this 

class TouchHandler {
 public:
  //in order to create a specific touch handler you have to write your own constructor
  TouchHandler() {};

  void setSize(uint16_t _disp_xSize, uint16_t _disp_ySize);
  void setCalibration(uint16_t ts_xMin, uint16_t ts_xMax, uint16_t ts_yMin, uint16_t ts_yMax);
  //order of operations: map, swap, constraint, flip 
  void setSwapFlip(bool _swapXY,bool _flipX,bool _flipY);

  THPoint scale(THPoint pIn);
  
  //in order to create a specific touch handler you have to overwrite this methods
  virtual void begin(void);
  virtual THPoint getPoint(void);

private:
    //landscape perspective: x: width, y: heigth
    uint16_t disp_xSize = 320;
    uint16_t disp_ySize = 240;

    uint16_t ts_xMin = 0;
    uint16_t ts_xMax = 4095;
    uint16_t ts_yMin = 0;
    uint16_t ts_yMax = 4095;
    
    bool swapXY = false;    
    bool flipX = false;    
    bool flipY = false;    
};



/////////////////////////////////
// init and set the touch handler 

void gslc_InitTouchHandler(TouchHandler *pTHO);     
TouchHandler* gslc_getTouchHandler(void);     


#endif
