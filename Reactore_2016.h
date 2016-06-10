/*
 * blink.h
 *
 *  Created on: Nov 27, 2015
 *      Author: jan
 */

#ifndef REACTORE_2016_H_
#define REACTORE_2016_H_
//--------------------------#INCLUDE------------------------------------------------------------------
#include <LiquidCrystal.h>
#include "Arduino.h"
#include "Timer.h"  // https://github.com/JChristensen/Timer
#include "TimerOne.h"
#include "Debug.h"
#include "AS.h"
#include "CIS.h"
#include "ICS.h"
#include "MAS.h"
#include "ORS.h"
#include "PBS.h"
#include "SS.h"
#include "UIS.h"
#include "HAL.h"
#include "pinDefs.h"  // debugging


//--------------------------#DEFINE------------------------------------------------------------------
#define INTERVAL 					500 //The blinking interval
#define CHARGE_ALTERNATE_MINUTES   	60 // **** changed from 60, doesn't matter if it is awake or sleep// battery charge circuit alternates every 'CHARGE_ALTERNATE_MINUTES'
#define MILLIS_MAX 					4294967295
#define MICROS_MAX 					4294967295
#define sonarTimer        			Timer1

/* Hi Kevin. Hope you are doing well. If you want to change the TRAVEL_TIME, which is the length of time
    the sled will be pushed after there was a successful push.
    1000 = 1 second
    2000 = 2 seconds etc...
*/
#define TRAVEL_TIME   				5000




//--------------------------FUNCTION_LIST------------------------------------------------------------
extern void initPushbackAve(void);
extern void sonarISR(void);

#endif /* REACTORE_2016_H_ */
