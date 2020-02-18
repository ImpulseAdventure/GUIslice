// touch handler (TH) for GSL1680 using https://github.com/insolace/GSL1680

// The touch handler performs the adaption in between the GUIslice framework and any touch driver
// The touch handler used is specified in the main program by calling gslc_InitTouchHandler(&touchHandler);

/// \file GUIslice_th_GSL1680.h

#ifndef _GUISLICE_TH_GSL1680_H_
#define _GUISLICE_TH_GSL1680_H_

#include <Arduino.h>
#include <GUIslice_th.h>
#include <GSL1680.h>

// Touch Pins
#define WAKE 16 // wakeup! (is this used?)
#define INTRPT 17 // touch interrupt

class TouchHandler_GSL1680: public TouchHandler {
    public:
        // parameters:

        TouchHandler_GSL1680() : touchDriver(GSL1680()) {
            //empirical calibration values, can be updated by calling setCalibration in the user program
            setCalibration(398,3877,280,3805);
            //swapping and flipping to adopt to default GUIslice orientation
            setSwapFlip(true,false,true);
        }
        //begin is called by gslc_InitTouchHandler
        void begin(void) {
            //init the touch driver
            touchDriver.begin(WAKE, INTRPT);
        }

        //this method returns the scaled point provided by the touch driver
        THPoint getPoint(void) {

            // If interrupt flag is low then return no touch
            if (digitalRead(INTRPT) == LOW) {
                return THPoint(0,0,0);
            }

            // read data from touchscreen over I2C, returns how many fingers are touching (up to 10)
            fingers = touchDriver.dataread();
            // update finger #0
            curFing = touchDriver.readFingerID(0);

            // read in X and Y values, set pressure to max as captouch has no pressure
            THPoint ps = THPoint(touchDriver.readFingerX(0), touchDriver.readFingerY(0), 255);

            //Serial.print("ps= ");Serial.print(ps.x);Serial.print(",");Serial.print(ps.y);Serial.print(",");Serial.println(ps.z);

            return ps;
        };

        long fingers, curFing;  // data for finger data and IDs
        /*long lastPress = 0;
        long touchX = 0;
        long touchY = 0;
        long newX = 0;
        long newY = 0;
        int difX = 0;
        int difY = 0;*/

      //SPIClass spi;
      GSL1680 touchDriver;

};


#endif
