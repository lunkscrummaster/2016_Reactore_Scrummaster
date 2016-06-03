/*
 * MAS.h
 *
 *  Created on: May 30, 2016
 *      Author: Prospect
 */
#ifndef MasterSystem_h
#define MasterSystem_h

//---------------------------------------#INCLUDES---------------------------------------
#include "Arduino.h"
#include "Debug.h"
#include "ORS.h"
#include "AVG.h"
#include "SS.h"
#include "AS.h"
#include "HAL.h"
#include "ICS.h"
#include "PBS.h"
#include "UIS.h"
#include "pinDefs.h"
#include "TimerOne.h"

//---------------------------------------#DEFINES---------------------------------------
#define MILLIS_MAX 					4294967295

#define INDIVIDUAL_SINK_RAISE      	325, 380   	//Minimum pushback distance is approx. 33cm, max is approx 40cm
#define POWER_SINK_RAISE           	325, 380  	// here are the pushback arm settings
#define STRENGTH_SINK_RAISE        	325, 330
//#define RESET 						300, 325

#define CHARGE_PRESSURE_TRIP  		400   		// if pressure over this, shutdown (400 means approx. 30 lbs) TRUCK
#define CHARGE_DISTANCE_TRIP  		390   		// if sonar over this, shutdown

#define STRENGTH_POSTHIT_HEARTBEATS   (2 * HEARTBEATS_PER_SECOND)	// how long to wait after Strength Charge completes (Success or Shutdown)

#define MAS_LTS_UNKNOWN  			0	// states of 'lastTowingSwitch'
#define MAS_LTS_OFF      			1
#define MAS_LTS_ON       			2

#define AVE_ARRAY_SIZE    			5


//---------------------------------------MasterSystem_CLASS---------------------------------------
class MasterSystem {
  public:
    MasterSystem();
    void heartbeat();
    void loop();      // used during Strength Charge phase

    void pushbackIsReady();          // called by Pushback when it's Ready
    void accustatEnteringPosthit();  // called by Accustat when hit is over

    // called by UISystem when its mode changes, or one of its vars change
    void UIModeChanged(byte uis);
    void UIVarChanged (byte uivn, int val);

    byte getLastUIState();

    void updateOutriggerLooseArray(void);
    void updateOutriggerTightArray(void);
    void fillOutriggerArray(void);
    int getOutriggerLooseAve(void);
    int getOutriggerTightAve(void);
//    int getPushbackSonarAve(void);

    volatile long successStartTime;

    volatile int outriggerLooseIndex;
    volatile int outriggerTightIndex;

    volatile int outriggerLooseSonar [AVE_ARRAY_SIZE];
    volatile int outriggerLooseSonarAve;
    volatile int outriggerTightSonar [AVE_ARRAY_SIZE];
    volatile int outriggerTightSonarAve;

    volatile int pushbackSonar [AVE_ARRAY_SIZE];
    volatile int pushbackSonarAve;
    volatile int pushbackIndex;



  private:
    byte lastUIState;
    byte lastReadyState;
    byte lastTowSwitch;

    // used during loop() in STRENGTH CHARGE phase
    long lastMillis;
    long strengthChargeTimeoutMillis;
    byte strengthPosthitTimeoutHeartbeats;

};

#endif
