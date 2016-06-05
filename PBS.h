/*
 * PBS.h
 *
 *  Created on: May 30, 2016
 *      Author: Prospect
 */
#ifndef PushbackSystem_H
#define PushbackSystem_H
//--------------------------------------#INCLUDES--------------------------------------
#include "Arduino.h"
#include "Debug.h"
#include "AS.h"
#include "HAL.h"
#include "MAS.h"
#include "ORS.h"
#include "pinDefs.h"

//--------------------------------------#DEFINES--------------------------------------
#define SETTLING_COUNT   		2   // number of heartbeat periods to wait

#define PBS_QUIET            	0	// PushbackSystem states
#define PBS_READY1_SINKING   	1
#define PBS_READY2_RAISING   	2
#define PBS_READY3_SETTLING  	3

//--------------------------------------PUSHBACK_CLASS--------------------------------------
class PushbackSystem {
  public:
    PushbackSystem();
    void heartbeat(void);

    void enable(boolean en);

    // go to Ready position from Towing mode
    void goReady(byte asMode, int sinkTo, int raiseTo);

    byte getState(void);

    int  readySinkTo, readyRaiseTo;   //moved from private to public to implement timer interrupt

    void enterState(byte newState);

  private:

    byte state;

 //   void enterState(byte newState);

    boolean enabled;
    byte    settlingTimeout;
};

#endif
