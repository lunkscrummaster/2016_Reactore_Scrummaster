/*
 * AS.cpp
 *
 *  Created on: May 30, 2016
 *      Author: Prospect
 */
#include "AS.h"
//--------------------------Variables------------------------------------------------------------

byte beeperToDo = 0;
byte beeperCountdown = 0;

/* --------------------------beeperSetup()------------------------------------------------------------
 *  Called from: Accustat::Accustat()
 *  1. Setups up oBeeper (pin 37) as output
 *  2. Writes the pin to low
 */
void beeperSetup() {
	pinMode(oBeeper, OUTPUT);
	digitalWrite(oBeeper, LOW);
} //end beeperSetup()

/*--------------------------beeperHeartbeat------------------------------------------------------------
 * CALLED FROM Accustat::heartbeat()
 * byte    beeperToDo      = 0;
 * byte    beeperCountdown = 0;
 * #define BEEPER_PERIODS  1    // beeps & pauses are this many heartbeats long
 * 1. starts or stops beeper, by writing to the oBeeper pin
 * 2. Write oBeeper to low just in case
 */
void beeperHeartbeat() {

	switch (accustat.returnmode()) {
		case ASM_INDIVIDUAL: {
			if (beeperToDo > 0) {
				if (beeperCountdown-- <= 1) {
					// start or stop beeper
					digitalWrite(oBeeper, (--beeperToDo & 1) ? HIGH : LOW); // write the beep on or off

					beeperCountdown = BEEPER_PERIODS;
				} // end if (beeperCountdown-- <= 1)
			} else
				digitalWrite(oBeeper, LOW);  // just to make sure
			break;
		}
		case ASM_POWER:
		case ASM_STRENGTH: {
				if (accustat.earlyPush == true) {
					int dispIndex = accustat.pbAvg.getAverage() - accustat.getNaturalPreCharge();
					if(accustat.pressureArray[dispIndex] < ui.getVar(UIVM_POWER_THRESHOLD) * 100) {
						digitalWrite(oBeeper, LOW);
						accustat.earlyPush = false;
						Serial.println("  &$%#$%^ early push beep off ");
					}
				}
			break;
		} // end strength
	} // end switch

	//Serial.println("beeper Heartbeat");

} //end beeperHeartbeat()

/* ------------------------beep(byte)-----------------------------------------------------------------------
 This is called from enterState below, and heartbeat below
 Count can be any of the below
 // number of times beeper sounds for new peaks
 //#define BEEP_COUNT_NEW_PEAK         1  // new peak for this cycle
 #define BEEP_NEW_SESS_PEAK          2  // new peak since last powerup/reset
 */
void beep(byte count) {
	beeperToDo = count * 2;  // odd values are beeps, even values are pauses
	beeperCountdown = BEEPER_PERIODS;
}  // end beep(byte)

const byte ledDigits[] = { 0xfc, 0x60, 0xda, 0xf2, 0x66, 0xb6, 0xbe, 0xe0, 0xfe,
		0xf6, 0x02  // dash
		};

/*------------------------------------------display_num()------------------------------------------
 *   it is called from Accustat::displayHeartbeat() , Accustat::isEnabled()
 *  Displays on the large LED 4 DIGIT DISPLAY
 * 1. Just gets the values based on the number passed to it
 * 2. The values passed to it are the session peaks
 * Max possible values are just above ^^
 */
void display_num(int number) {

	if (number < LED_MIN)
		number = LED_MIN;
	if (number > LED_MAX)
		number = LED_MAX;

	byte thousands;
	if (number < 0) {
		number = -number;  // if number is negative, reset it to be positive
		thousands = LED_DASH_CODE; // writes a dash because of negative number??????
	} else {
		thousands = (number % 10000) / 1000; //this gets the first digit of the display from the left
	} //end else

	byte hundreds = (number % 1000) / 100; //gets the second didgit from the left
	byte tens = (number % 100) / 10; // get the third digit from the left
	byte ones = (number % 10); // gets the final single digits
	digitalWrite(oLEDClear, HIGH); // set the bit so we can write to the screen
	shiftOut(oLEDData, oLEDClock, LSBFIRST, ledDigits[ones]);
	shiftOut(oLEDData, oLEDClock, LSBFIRST, ledDigits[tens]);
	shiftOut(oLEDData, oLEDClock, LSBFIRST, ledDigits[hundreds]);
	shiftOut(oLEDData, oLEDClock, LSBFIRST, ledDigits[thousands]);
	digitalWrite(oLEDClear, LOW); //clear the bit so we CANT write to the screen

	delay(4);
} // end display_num

/* --------------------------displayHeartbeat------------------------------------------------------------
 *  Called from: Accustat::heartbeat()
 1. calls display_num, which updates the LED screen with either the sessionPeak or currentPeak
 */
void Accustat::displayHeartbeat() {
	// Serial.println("Accustat displayHeartbeat start");
	// if displayAlternateIndex == 0 then display sessionPeak else display currentPeak;
	// this cycles so that it displays sessionPeak, currentPeak, currentPeak, currentPeak, ...
	//Serial.print("dispAltInd: ");Serial.print(displayAlternateIndex); Serial.print("  sessionPeak: "); Serial.print(sessionPeak);Serial.print("  currentPeak: "); Serial.println(currentPeak);
	display_num(displayAlternateIndex == 0 ? sessionPeak : currentPeak);

	if (displayAlternateCountdown-- <= 1) {
		displayAlternateCountdown = DISPLAY_PERIOD;
		switch (displayAlternateIndex) {
		case 0:  // (fall into next)
		case 1:  // (fall into next)
		case 2:
			displayAlternateIndex++;
			break;
		case 3:
			displayAlternateIndex = 0;
			break;
		}  // end switch
	}
} // end Accustat::displayHeartbeat()

//--------------------------ACCUSTAT_Constructor -----------------------------------------------------
Accustat::Accustat():
pressureArray  						{0,    10,   20,   29,   39,   49,   59,   69,   78,   88,
									 98,  108,  118,  127,  137,  147,  157,  160,  163,  166,
									169,  172,  175,  178,  181,  184,  187,  190,  193,  196,
									211,  226,  241,  256,  271,  286,  302,  317,  332,  347,
									362,  377,  392,  409,  425,  442,  458,  474,  490,  507,
									523,  539,  555,  571,  588,  611,  632,  655,  676,  698,
									720,  741,  763,  785,  805,  827,  849,  871,  893,  915,
									937,  959,  981,  1021, 1059, 1099, 1138, 1177, 1205, 1233,
									1261, 1289, 1317, 1345, 1373, 1398, 1422, 1447, 1471, 1496,
									1521, 1523, 1539, 1555, 1571, 1587, 1603, 1619, 1635, 1651,
									1667, 1683, 1699, 1715, 1766, 1806, 1845, 1884, 1923, 1962,
									1990, 2018, 2046, 2074, 2102, 2130, 2158, 2183, 2207, 2232,
									2256, 2281, 2305, 2330, 2354, 2373, 2393, 2413, 2432, 2452,
									2471, 2491, 2510, 2530, 2550, 2661, 2671, 2682, 2691, 2702,
									2712, 2722, 2732, 2746, 2765, 2780, 2795, 2810, 2825, 2840,
									2856, 2871, 2886, 2901, 2916, 2931, 2943, 2960, 2973, 2987,
									3000, 3013, 3026, 3039, 3052, 3065, 3078, 3091, 3104, 3117,
									3131, 3139, 3164, 3186, 3208, 3230, 3252, 3274, 3295, 3317,
									3335, 3345, 3361, 3378, 3394, 3410, 3426, 3443, 3459, 3475,
									3492, 3508, 3531, 3541, 3558, 3574, 3590, 3607, 3623, 3639,
									3656, 3672, 3689, 3705, 3728, 3747, 3763, 3778, 3793, 3808,
									3823, 3838, 3853, 3868, 3883, 3898, 3913, 3924}
{
	beeperSetup();
	cooldownCounter = 0;
	currentPeak = 0;
	hiddenPeak = 0;
	sessionPeak = 0;
	lastReading = 0;
	displayAlternateCountdown = 0;
	displayAlternateIndex = 0;
	hasSeenBall = false;
	mode = ASM_INDIVIDUAL;
	state = AS_QUIET;
	precharge = 0;
	naturalPreCharge = 0;
	aveTimerStart = false;
	aveTimer = 0;
	earlyPush = false;

}
/* --------------------------Accustat::loop()------------------------------------------------------------
 * #define AS_QUIET      0    #define AS_PREHIT     1   #define AS_HITTING    2 #define AS_POSTHIT    3
 * Called from: main loop
 * 1. Checks for pad hit to wake up machine
 */
void Accustat::loop() {
	/* Added code below by trevor and zach
	 So if the machine is in sleep mode, the machine should wake up when someone hits the pads
	 check to see if in sleep, then wakeup if pressure on pads is there.
	 TRUCK, the analogRead value needs to be tested
	 */
	if (sleep.getState() == 0) {
		if (analogRead(aiAchievedPin) > 275) {
			sleep.wakeup();
			Serial.print("  analogRead(aiAchievedPin) is wakeing it up");
			Serial.println(analogRead(aiAchievedPin));
		}
	} // end if (sleep.getState() == 0)

	if (isEnabled() && state != AS_QUIET) {
		// update running sum
		int p = analogRead(aiAchievedPin); //read current value and assign to p
		pbAvg.update(p); //update the average, using the last read value

		int avg = pbAvg.getAverage(); //updated average
//    Serial.print("p: "); Serial.print(p); Serial.print(" avg: "); Serial.println(avg);
//    Serial.print(" diff: "); Serial.println(p-avg);
		switch (state) {
		case AS_PREHIT: {
//          int diff = p - avg; //difference between the current and average reading
			int diff = avg - naturalPreCharge;
			if (diff > HIT_TRIP && outriggers.getBalanceMode() == false) { //#define HIT_TRIP     6 ???? confirm this operation wrt #define (HIT_TRIP) based on reading during operation
//           Serial.print(" avg: "); Serial.print(avg); Serial.print(" naturalprecharge: "); Serial.println(naturalPreCharge);
				if (mode == ASM_INDIVIDUAL) {
					lastReading = avg;
//              Serial.print(" hit trip success");
				} else {
					lastReading = avg;
				}
				enterState(AS_HITTING);	//enter AS_HITTING since the pads are being hit
			} else
				lastReading = avg;		//assign the last reading to the average
		}			//end case AS_PREHIT
			break;
			/*
			 // Accustat modes POSSIBLE MODES
			 #define ASM_INDIVIDUAL  0 //individual just measure how strong someone is pushing
			 #define ASM_POWER       1 ..?
			 #define ASM_STRENGTH    2 ..?
			 */
		case AS_HITTING:
			switch (mode) {
			case ASM_INDIVIDUAL:
				if (lastReading < avg) {
					lastReading = avg;
				}
//            if (lastReading < p)
//              // update peak reading
//              lastReading = p;
				break;
			case ASM_POWER: //since no break, will execute ASM_STRENGTH even though its ASM_POWER
			case ASM_STRENGTH:
				lastReading = avg;
				break;
			} // end switch (mode)
			break;
		} // end switch (state)
	} // end first  if (isEabled() && state != AS_QUIET) {
} // end of Accustat loop

/* ---------------------------------------Accustat::isEnabled()---------------------------------------
 Function is called from Accustat::enable (below) , and Accustat::loop()
 1. The function returns true/false depending on oDisplayPowerPin (pin 27)
 2. this oDisplayPowerPin controls large LED DISPLAY. display is off while sleep, else on
 */
boolean Accustat::isEnabled() {
	return digitalRead(oDisplayPowerPin);
} // end Accustat::isEnabled()

/* ---------------------------------------Accustat::enable---------------------------------------
 Function is called from nowhere, this is not implemented yet
 This function appears to impliment a sleep state for the LED DISPLAY
 */
void Accustat::enable(boolean en) {
	if (isEnabled()) { //if enabled,
		if (!en) {
			// disabling
			digitalWrite(oDisplayPowerPin, LOW); //turn off display
			enterState(AS_QUIET); //enter quiet mode
		}
	} else {
		if (en) {
			// enabling
			digitalWrite(oDisplayPowerPin, HIGH);
			display_num(0);
			enterState(AS_QUIET);
		}
	} // end else
} // end Accustat::enable(boolean)

/* ---------------------------------------Accustat::reset()------------------------------------------------------------
 This function is called from UISystem::loop()
 1. The function resets the statistics when the set button is pressed
 2. turns off the beeper
 */
void Accustat::reset() {
	sessionPeak = currentPeak = 0;
	Serial.println("ACCUSTAT RESET THE SESSION PEAK AND CURRENT PEAK");
	digitalWrite(oBeeper, LOW);
} // end Accustat::reset()

/* ---------------------------------------Accustat::pause()------------------------------------------------------------
 This function is called from PushbackSystem::enterState(byte newState)
 Also possibly called from PushbackSystem::reReady(), but this is currently not implemented
 1. This function sets the accustat state to AS_QUIET
 */
void Accustat::pause() {
	enterState(AS_QUIET);
} // end Accustat::pause()

/* ---------------------------------------Accustat::resume()---------------------------------------
 This function is called from MasterSystem::pushbackIsReady()
 1. This function changes the accustat state too AS_PREHIT
 */
void Accustat::resume() {
	enterState(AS_PREHIT);
} // end Accustat::resume()

/* ---------------------------------------Accustat::setMode (byte)---------------------------------------
 This function is called from PushbackSystem::goReady(
 1. This function assigns the Accustat mode to whatever was passed in
 // Accustat modes
 #define ASM_INDIVIDUAL  0
 #define ASM_POWER       1
 #define ASM_STRENGTH    2
 */
void Accustat::setMode(byte m) {
	mode = m;
} //end Accustat::setMode

/* ---------------------------------------Accustat::saveHiddenPeak()---------------------------------------
 This function is called from MasterSystem::loop()
 1. set currentPeak = hiddenPeak
 */
void Accustat::saveHiddenPeak() {
	currentPeak = hiddenPeak;
} // end Accustat::saveHiddenPeak

/* ---------------------------------------Accustat::enterState(byte)---------------------------------------
 This function is called from Accustat::resume() , Accustat::pause() , Accustat::isEnabled() , Accustat::loop() , Accustat::heartbeat()
 1. sets the accustat state to whatever was passed to it
 2.depending on the state, perform diffent duties
 */
void Accustat::enterState(byte newState) {
	state = newState;
//  Serial.print("Accustat enterState: "); Serial.println(newState);
	switch (state) {
	case AS_QUIET:
		displayAlternateIndex = DISPLAYMODE_ALTERNATE; // this code indicates "alternate between current peak and session peak"
		break;

	case AS_PREHIT:
		// reset the running average
		pbAvg.reset();
		break;

	case AS_HITTING:
		DEBUG_PRINT_S("\n AS->HITTING\n");
		hasSeenBall = false;
		cooldownCounter = COOLDOWN_PERIODS; // COOLDOWN_PERIODS  8 minimum hitting phase is this # of heartbeats long
		precharge = lastReading;
		currentPeak = hiddenPeak = 0; //reset the peaks
		displayAlternateIndex = DISPLAYMODE_CURRPEAK; // this code indicates "display current peak"
		break;

	case AS_POSTHIT:
		DEBUG_PRINT_S(" AS->POSTHIT\n\n");
		digitalWrite(oBeeper, LOW); //turn off any noise
		if (mode == ASM_POWER || mode == ASM_STRENGTH) {
			currentPeak = hiddenPeak;
		}
		if (sessionPeak < currentPeak) {
			sessionPeak = currentPeak; //reset new session peak
			beep(BEEP_NEW_SESS_PEAK); //#define BEEP_NEW_SESS_PEAK 2  // new peak since last powerup/reset
		}
		displayAlternateIndex = DISPLAYMODE_ALTERNATE; // this code indicates "alternate between current peak and session peak"
		master.accustatEnteringPosthit(); //located in MAS.cpp
		break;
	} //end switch (state)
} // end Accustat::enterState

/* ---------------------------------------Accustat::heartbeat()---------------------------------------
 Possibble states
 // accustat states
 #define AS_QUIET      0   #define AS_PREHIT     1   #define AS_HITTING    2  #define AS_POSTHIT    3
 1. go into beeperHeartbeat, which starts or stops the beep
 2. go into displayHeartbeat, which updates the LED screen
 3. Checks state, and does things depending on the state
 */
void Accustat::heartbeat() {
	//Serial.println("Accustat Heartbeat Started");
	beeperHeartbeat(); //just above
	displayHeartbeat(); //just above
	//Serial.print("lastReading: "); Serial.println(lastReading);
	/* // accustat state
	 #define AS_QUIET      0   #define AS_PREHIT     1   #define AS_HITTING    2   #define AS_POSTHIT    3
	 */
	switch (state) {
	case AS_HITTING: {
		/* **** Changed made May 21 by Trevor and Zach
		 Below, the converiosn that was made before didn't seem right.
		 We changed the equation to what we got from the data that was supplised by Kevin
		 */
		//changed this equation to fit experimentally acquired data
		int x = lastReading - naturalPreCharge;
        if (x < 0)
        	x = 0;
//        int disp = ((-6) * (10 ^ (-10)) * (x ^ 6)) + (4 * (10 ^ (-7)) * (x ^ 5)) - (1 * (10 ^ (-4)) * (x ^ 4)) + (0.0111 * (x ^ 3)) - (0.3613 * (x ^ 2)) + (11.237 * x);
		Serial.print(" diff: ");
		Serial.print(x);
//        int disp = 0;
		//   if(x < 49){
		//  	float disp1 = x * 5.04;
		///  	disp = (int)disp1;
		//  }else{
//        	float disp1 = (-0.0002 * (x * x)) + (20.086 * x) - 289.07;
//        	long double disp1 = ((-5.699)*(0.0000000001)*(x*x*x*x*x*x));
//        	disp1 = disp1 + ((3.995)*(0.0000001)*(x*x*x*x*x));
//        	disp1 = disp1 - ((0.0001)*(x*x*x*x));
//        	disp1 = disp1 + ((0.0109)*(x*x*x));
//        	disp1 = disp1 - ((0.3447)*(x*x));
//        	disp1 = disp1 + ((10.643)*(x));
//        	disp1 = disp1 + 6.7988;
////        	long float disp1 = ((-5.699)*(0.0000000001)*(x*x*x*x*x*x)) + ((3.995)*(0.0000001)*(x*x*x*x*x)) - ((0.0001)*(x*x*x*x)) + ((0.0109)*(x*x*x)) - ((0.3447)*(x*x)) + ((10.643)*(x)) + 6.7988;
//        	disp = (int)disp1;
		//  }
//        	Serial.print(" disp1:  "); Serial.print((float)disp1);
		int disp = pressureArray[x];
		Serial.print(" display: ");
		Serial.print(disp);
		Serial.print(" aianalogread: ");
		Serial.println(analogRead(aiAchievedPin));
		/*
		 // Accustat modes POSSIBLE MODES
		 #define ASM_INDIVIDUAL  0 //individual just measure how strong someone is pushing
		 #define ASM_POWER       1 ..?
		 #define ASM_STRENGTH    2 ..?
		 */
		switch (mode) {
		case ASM_INDIVIDUAL:
			if (currentPeak < disp) {
				currentPeak = disp;
				//Serial.print("  current peak set to disp  ");
				// beep for passing current peak?
			} // end if (currentPeak < disp)
			break;
			// below will call a penatly if the players are pushing to hard before the ball is in???
		case ASM_POWER: {
			if (disp > ENGAGED_MIN) {
				int threshold = ui.getVar(UIVM_POWER_THRESHOLD) * 100;
				if (disp <= threshold) {
					if (hasSeenBall) {
						if (hiddenPeak < disp)
							hiddenPeak = disp;
				    	}
					} else {
						if(!hasSeenBall){
							digitalWrite(oBeeper, HIGH);
							Serial.print(" ***ball to early beep ");
							earlyPush = true;
						}
					} // end else
				 // closes if (disp > threshold)
			} // end if (disp > ENGAGED_MIN)
		} // end case ASM_POWER
			break;

		case ASM_STRENGTH: {
			/* ADDED, by trevor and zach May 21
			 So if the machine has a reading of 100lbs of force or more, then is can possibly pushback
			 */
			if (disp > ENGAGED_MIN) { // if someone is pushing against the machine
				int threshold = ui.getVar(UIVM_STRENGTH_THRESHOLD) * 100; //this is in lbs
				if (disp > threshold) { //if (current measured pressure > minimum to push  )
					if (hasSeenBall) {
						if (hiddenPeak < disp) {
							hiddenPeak = disp;
							if (ui.getState() == UIS_SCRUM_STRENGTH)
								ui.enterState(UIS_SCRUM_STRENGTH_CHARGE); //writes new peak
						}
					} else { // ball has not been seen, give a penatly
						digitalWrite(oBeeper, HIGH);
					} // end else
				} //closes if (disp > threshold)
			} // end if (disp > someoneis there)
		} // end ASM_STRENGTH
			break;
		} // end switch (mode)

		if (sessionPeak < currentPeak) {
			sessionPeak = currentPeak;
			beep(BEEP_NEW_SESS_PEAK);
		}

		if (returnmode() == ASM_INDIVIDUAL) {
			//Serial.print("  average: "); Serial.print(pbAvg.getAverage()); Serial.print(" precharge "); Serial.print(naturalPreCharge);
			if (currentPeak > 0 && pbAvg.getAverage() - naturalPreCharge < 6) {
				enterState(AS_POSTHIT);
			}
		} else {

			if (cooldownCounter > 0)
				cooldownCounter--;
			else {
				switch (mode) {
				case ASM_INDIVIDUAL:
//                  enterState(AS_POSTHIT);
					break;
				case ASM_POWER:
				case ASM_STRENGTH:
					if (disp < 50)
						enterState(AS_POSTHIT);
					break;
				} // end switch (mode)
			} //closes else ie cooldownCounter = 0
		}

	} // end case AS_HITTING
		break;

	case AS_POSTHIT:
		enterState(AS_PREHIT);
		break;
	} // end switch (state)

	  //Serial.print(" hasSeenBall = "); Serial.println(hasSeenBall);
} //end Accustat::heartbeat()

/* ---------------------------------------Accustat::returnmode()---------------------------------------
 *  Called from: ....????
 *  1. Returns the Accustat mode
 */
byte Accustat::returnmode() {
	return mode;
} // end returnmode

/* ---------------------------------------Accustat::returnstate()---------------------------------------
 *  Called from: sonarISR in main page
 *  1. Returns the Accustat state
 */
byte Accustat::returnState() {
	return state;
} // end returnState

/* ---------------------------------------Accustat::getHasSeenBall()---------------------------------------
 *  Called from: sonarISR in main page
 *  1. Returns the hasSeenBall
 */
boolean Accustat::getHasSeenBall() {
	return hasSeenBall;
}

/* ---------------------------------------Accustat::setHasSeenBall()---------------------------------------
 *  Called from: sonarISR in main page
 *  1. Returns the hasSeenBall
 */
void Accustat::setHasSeenBall(boolean ball) {
	hasSeenBall = ball;
}

/* ---------------------------------------Accustat::setNaturalPreCharge()---------------------------------------
 *
 *
 */
void Accustat::setNaturalPreCharge(void) {
	pbAvg.reset();
	for (int i = 0; i < AVG_NUM_READINGS; i++) {
		pbAvg.update(analogRead(aiAchievedPin));
	}
	naturalPreCharge = pbAvg.getAverage();
}

/* ---------------------------------------Accustat::getNaturalPreCharge()---------------------------------------
 *
 *
 */
int Accustat::getNaturalPreCharge(void) {
	return naturalPreCharge;
}

