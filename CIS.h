/*
 * CIS.h
 *
 *  Created on: May 30, 2016
 *      Author: Prospect
 */
#ifndef CompressorInverterSystem_h
#define CompressorInverterSystem_h

//-----------------------------------#INCLUDES-----------------------------------
#include "Arduino.h"
#include "Debug.h"

//-----------------------------------#DEFINES-----------------------------------
#define CSS_OFF       0 	//compressor system is currently off
#define CSS_STARTING  1  	// waiting for inverter on and unloader valve timeout
#define CSS_ON        2 	//compressor system is on
#define CSS_STOPPING  3  	// waiting for unloader valve timeout


//-----------------------------------COMPRESSOR_CLASS-----------------------------------
class CompressorSystem {
  public:
    CompressorSystem();
    void heartbeat(void);


  private:
    byte state;
    void enterState(byte newState);

    // reservoir pressure setpoints
    int  pvTankPressMin;
    int  pvTankPressMax;

    byte pvUnloaderTimeout;
};

//-----------------------------------INVERTER_CLASS-----------------------------------
class InverterSystem {
  public:
    InverterSystem();
    void heartbeat();
    void neededByCompressor(boolean en);  // compressor requests inverter on/off
    void neededByDumpValve (boolean en);  // compressor requests inverter on/off

  private:
    boolean nbCompressor, nbDumpValve;
};

#endif
