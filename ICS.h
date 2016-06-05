/*
 * ICS.h
 *
 *  Created on: May 30, 2016
 *      Author: Prospect
 */
#ifndef InitialChargeSystem_h
#define InitialChargeSystem_h

//-----------------------------------#INCLUDES-----------------------------------
#include "Arduino.h"
#include "Debug.h"
#include "HAL.h"
#include "pinDefs.h"

//-----------------------------------#DEFINES-----------------------------------
#define ICS_QUIET     0   //ICS is not needed, or pressure doesn't need to be changed
#define ICS_RAISING   1   //ICS needs more pressure
#define ICS_LOWERING  2   //ICS needs less pressure

#define PERCENT_TO_PRESSURE_OFFSET  160		// converting percent to raw pressure (from analogRead)
#define PERCENT_TO_PRESSURE_FACTOR  2.8    // 2.8 corresponds to max 35 lbs

//-----------------------------------INITIAL_CHARGE_CLASS-----------------------------------
class InitialChargeSystem {
  public:
    InitialChargeSystem();
    void heartbeat();

    void enable(boolean en);  // disable when towing

    int  getCurrentPercent();    // gets current pressure as a percent
    void setTargetPercent(int p);

  private:
    byte state;
    void enterState(byte newState);

    boolean enabled;
    int  targetPressure;
};

#endif
