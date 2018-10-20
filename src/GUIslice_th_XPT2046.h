// touch handler (TH) for XPT2046 using the arduino built in driver <XPT2046_touch.h>

// The touch handler performs the adaption in between the GUIslice framework and any touch driver
// The touch handler used is specified in the main program by calling gslc_InitTouchHandler(&touchHandler);

#ifndef _GUISLICE_TH_XPT2046_H_
#define _GUISLICE_TH_XPT2046_H_

#include <Arduino.h>
#include <GUIslice_th.h>
#include <XPT2046_touch.h>

class TouchHandler_XPT2046: public TouchHandler {
    public:
        // parameters:
        //   spi object to be used
        //   chip select pin for spi
        TouchHandler_XPT2046(SPIClass &spi, uint8_t spi_cs_pin) : spi(spi), touchDriver(XPT2046_touch(spi_cs_pin, spi)) {
            //empirical calibration values, can be updated by calling setCalibration in the user program
            setCalibration(398,3877,280,3805);
            //swapping and flipping to adopt to default GUIslice orientation
            setSwapFlip(true,false,true);
        }            
   
        //begin is called by gslc_InitTouchHandler
        void begin(void) {
            //init the touch driver
            touchDriver.begin();
        }
                   
        //this method returns the scaled point provided by the touch driver 
        THPoint getPoint(void) {
            //get the coordinates from the touch driver
            TS_Point pt = touchDriver.getPoint();
            //Serial.print("pt= ");Serial.print(pt.x);Serial.print(",");Serial.print(pt.y);Serial.print(",");Serial.println(pt.z);
    
            //perform scaling (this includes swapping and flipping)
            THPoint ps = scale( THPoint(pt.x,pt.y,pt.z) );
            //Serial.print("ps= ");Serial.print(ps.x);Serial.print(",");Serial.print(ps.y);Serial.print(",");Serial.println(ps.z);
            
            return ps;
        };

      SPIClass spi; 
      XPT2046_touch touchDriver;
            
};


#endif
