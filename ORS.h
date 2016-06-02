/*
 * ORS.h
 *
 *  Created on: May 30, 2016
 *      Author: Prospect
 */
#ifndef OutriggerSystem_H
#define OutriggerSystem_H

//--------------------------------------#INCLUDES--------------------------------------
#include "Arduino.h"
#include "Debug.h"
#include "Timer.h"
#include "HAL.h"
#include "PBS.h"
#include "MAS.h"
#include "pinDefs.h"

//--------------------------------------#DEFINES--------------------------------------
#define ORS_BALANCE_TRIP  15   		// raises outrigger when sonars differ by more than this
#define BALANCE_PERIOD    100 		//in microseconds

//--------------------------------------OUTRIGGERS_CLASS--------------------------------------
class OutriggerSystem {
  public:
    OutriggerSystem();
    void loop(void);
    void heartbeat(void);

    void setBalanceMode(boolean en);

    bool getBalanceMode(void);

    boolean outriggersFirstPumpDone;

  private:

    boolean inBalanceMode;
};

#endif
