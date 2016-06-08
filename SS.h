/*
 * SS.h
 *
 *  Created on: May 30, 2016
 *      Author: Prospect
 */
#ifndef SleepSystem_h
#define SleepSystem_h

//-----------------------------------------#INCLUDES-----------------------------------------
#include "Arduino.h"
#include "Debug.h"
#include "CIS.h"
#include "HAL.h"
#include "UIS.h"
#include "pinDefs.h"

//-----------------------------------------#DEFINES-----------------------------------------
// sleeps after this long (must be <= 136 minutes or 'timeoutHeartbeats' will overflow)
#define SLEEP_TIMEOUT_SECONDS	(15 * 60 * 5) 		//original SLEEP_TIMEOUT_SECONDS  (15 * 60) added * 10 to make change in heartbeat time.

#define SSS_ASLEEP  			0 				//System is ASLEEP
#define SSS_AWAKE   			1 				//System is AWAKE

//-----------------------------------------SLEEP_CLASS-----------------------------------------
class SleepSystem {
  public:
    SleepSystem();

    void heartbeat();

    void wakeup();

    byte getState();

  private:
    byte state;
    void enterState(byte newState);

    int timeoutHeartbeats;
};

#endif
