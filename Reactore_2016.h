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
#define TRAVEL_TIME   				1000
#define sonarTimer        			Timer1



//--------------------------FUNCTION_LIST------------------------------------------------------------
extern void initPushbackAve(void);
extern void sonarISR(void);

#endif /* REACTORE_2016_H_ */
