#ifndef Accustat_H
#define Accustat_H

//--------------------------#INCLUDE------------------------------------------------------------------
#include "Arduino.h"
#include "Debug.h"
#include "AVG.h"
#include "SS.h"
#include "AVG.h"
#include "HAL.h"
#include "MAS.h"
#include "PBS.h"
#include "UIS.h"
#include "pinDefs.h"

//--------------------------#DEFINE------------------------------------------------------------------

#define ASM_INDIVIDUAL  		0			// Accustat modes
#define ASM_POWER       		1
#define ASM_STRENGTH    		2

#define DISPLAY_FACTOR    		20			// raw pressure readings are multiplied by this for display
#define ENGAGED_MIN       		150

#define AS_QUIET      			0 			// accustat states // sleep/ not being used
#define AS_PREHIT     			1 			// awake, waiting for hit ??
#define AS_HITTING    			2 			// currently being hit??
#define AS_POSTHIT    			3 			// hit finished, and do something....?

#define HIT_TRIP     			6      		// hit detected if sudden rise by this amount (raw pressure)
#define COOLDOWN_PERIODS  		8  			// minimum hitting phase is this number of heartbeats long

//#define BEEP_COUNT_NEW_PEAK  	1 			// new peak for this cycle
#define BEEP_NEW_SESS_PEAK  	2  			// new peak since last powerup/reset
#define BEEPER_PERIODS  		1    		// beeps & pauses are this many heartbeats long

#define LED_DASH_CODE  			10			//DEFINES FOR LED DISPLAY
#define LED_MIN  		   		-999		// max/min values that will fit in 4 digits
#define LED_MAX  				9999
#define DISPLAY_PERIOD  		4  			// Short-display period is this many heartbeats long
#define DISPLAYMODE_ALTERNATE  	0    		// this code indicates "alternate between current peak and session peak"
#define DISPLAYMODE_CURRPEAK   	4    		// this code indicates "display current peak"


//--------------------------ACCUSTAT_CLASS------------------------------------------------------------
class Accustat {
  public:
    Accustat();
    void loop();
    void heartbeat(void);

    void enable(boolean en);  // turns off LED display when disabled

    void reset();  // resets sessionPeak, currentPeak

    // stop/start normal operation
    void pause();
    void resume();

    void setMode(byte m);    // one of ASM_*, determines Accustat sensitivity etc.

    void saveHiddenPeak();  // after Strength Charge completes, save the hidden peak as the Current Peak

    byte returnmode(); //this returns the mode at start up
    byte returnState(); //this returns the state
    void setHasSeenBall(boolean ball);
    boolean getHasSeenBall(void);
    Averager pbAvg;
    void setNaturalPreCharge(void);
    int getNaturalPreCharge(void);
    int aveTimer;
    bool aveTimerStart;

    int  sessionPeak;
    int  currentPeak;  // displayed on LED
    int  hiddenPeak;   // Power/Strength peak displayed at end of cycle, not during cycle
    int  precharge;
    int  lastReading;

  private:
    int naturalPreCharge;
    byte state; // one of AS_
    void enterState(byte newState);

    byte mode;  // one of ASM_*

    boolean hasSeenBall;  // TRUE if ball-in sensor has been triggered this cycle

    boolean isEnabled();

    //Averager pbAvg;  // to smooth out pushback pressure readings

//    int  sessionPeak;
//    int  currentPeak;  // displayed on LED
//    int  hiddenPeak;   // Power/Strength peak displayed at end of cycle, not during cycle
//    int  precharge;
//    int  lastReading;
    byte cooldownCounter;  // mainly for Individual mode (weak hits & bounces)

    byte  displayAlternateCountdown;  // counts heartbeats until next alternate
    byte  displayAlternateIndex;      // 0 = display session peak, >0 = display current peak

    void  displayHeartbeat();
};

#endif
