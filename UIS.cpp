/*
 * UIS.cpp
 *
 *  Created on: May 30, 2016
 *      Author: Prospect
 */
#include "UIS.h"

//------------------------------------------MENU_BUTTON_CLASS------------------------------------------
class MenuButton {
public:
	MenuButton(byte pin);
	int update(unsigned long elapsedMicros);

private:
	byte inputPin;
	boolean last;
	long level;
};

// ------------------------------------------MenuButtonConstructor------------------------------------------
MenuButton::MenuButton(byte pin) {
	inputPin = pin;
	pinMode(inputPin, INPUT_PULLUP);
	level = 0;
	last = false;
} // end MenuButton

/*-------------------------------------------Menu::update------------------------------------------
 This function is called from UISystem::loop()
 1. This fucntion is a debounce routine.
 2. Returns 1 for button press, -1 for button release, otherwise 0
 // Updates button state; returns +1 if button pressed,
 // returns -1 if button released, otherwise returns zero.
 *
 */
int MenuButton::update(unsigned long elapsedMicros) {
	if (digitalRead(inputPin) == LOW) {
		// button is pressed
		level += elapsedMicros;
		if (level > MB_DEBOUNCE_MICROS) {
			// button has stayed pressed for a while
			level = MB_DEBOUNCE_MICROS;
			if (last == false) {
				// signal that button has been pressed
				last = true;
				return 1;
			}
		} // end if (level > MB_DEBOUNCE_MICROS)
	} else {
		// button not pressed
		level -= elapsedMicros;
		if (level < 0) {
			// button has stayed unpressed for a while
			level = 0;
			if (last == true) {
				// signal that button has been released
				last = false;
				return -1;
			}
		} // end iff (level < 0)
	} // end else

	return 0;  // no change to debounced state
} // end update

//------------------------------------------ UISystem_Constructor------------------------------------------
UISystem::UISystem(LiquidCrystal &lc) :
		bMode(*new MenuButton(iButtonPinMode)), bUp(
				*new MenuButton(iButtonPinUp)), bDown(
				*new MenuButton(iButtonPinDown)), bSet(
				*new MenuButton(iButtonPinSet)), lcd(lc) {
	// user-modifiable var info:           col, row, width, val, min, max //////////
	/* Changes made May 21 by Trevor and Zach
	 Previously, mins for UIVM_POWER_THRESHOLD,UIVM_STRENGTH_THRESHOLD, UIVM_STRENGTH_DIFFICULTY == 0
	 Changed these to be == 1, just as a safety feature
	 */
	setModVarInfo(UIVM_POWER_THRESHOLD, 15, 1, 2, 10, 3, 10);

	setModVarInfo(UIVM_STRENGTH_THRESHOLD, 15, 1, 2, 10, 3, 10);
	setModVarInfo(UIVM_STRENGTH_DIFFICULTY, 15, 2, 2, 2, 1, 10);
	setModVarInfo(UIVM_STRENGTH_DURATION, 15, 3, 2, 5, 1, 20);

	// non-modifiable var info: col, row, width //////////

	setNonModVarInfo(UIVN_TOWING_RESPRESS, 7, 2, 4);

	lcd.clear();
	lcd.noDisplay();
}

/* ------------------------------------------UISystem::setNonModVarInfo------------------------------------------
 This function is called from UISystem(LiquidCrystal &lc)
 1. This sets the only stuff that the user can't change.
 2. this is only used in the towing mode for the resevoir pressure on display
 */
void UISystem::setNonModVarInfo(byte vn, byte col, byte row, byte width) {
	cols[vn] = col;
	rows[vn] = row;
	wide[vn] = width;
	 //vals[vn] = 0; // this is the problem **** change made from 0 to the read
	vals[vn] = analogRead(aiReservoirPin);
} // end setNonModVarInfo

/* ------------------------------------------setModVarInfo------------------------------------------------------------------------------------
 This function is called from UISystem(LiquidCrystal &lc)
 1. This function is is for the variables that the user can change
 */
void UISystem::setModVarInfo(byte vn, byte col, byte row, byte width, int val,
		int min, int max) {
	cols[vn] = col;
	rows[vn] = row;
	wide[vn] = width;
	vals[vn] = val;
	mins[vn] = min;
	maxs[vn] = max;
} // end setModVarInfo

/* ----------------------------------------UISystem::loop()--------------------------------------------------------------
 This function is called from the main file loop
 1. First, finds the elapsedMicros since last called
 2. then it checks for set/mode button presses, and up down button presses
 3. Wakes up if there was a button press
 4. If set was pressed, reset the accustat values
 5. Then based on the current state, and the button presses, enter a new state
 */
void UISystem::loop() {
//	noInterrupts();
//	Serial.print("looseBall: "); Serial.print(lb);	Serial.print(" TightBall: "); Serial.println(tb);
//	interrupts();
	lcd.setCursor(0, 3);
	lcd.print("                    ");
	lcd.setCursor(0,3);
	lcd.print(accustat.getNaturalPreCharge());
	lcd.print(" ");
	lcd.print(accustat.sessionPeak);
	lcd.print(" ");
	lcd.print(accustat.currentPeak);
	lcd.print(" ");
	lcd.print(accustat.hiddenPeak);
	lcd.print(" ");
	lcd.print(accustat.lastReading);
	lcd.setCursor(0,2);
	lcd.print("                    ");
	lcd.setCursor(0,2);
	lcd.print(analogRead(aiAchievedPin));
	lcd.print(" ");
	lcd.print(lb);
	lcd.print(" ");
	lcd.print(tb);


	//lcd.setCursor(0,2);
//	lcd.print("                ");
//	lcd.setCursor(0, 2);
//	lcd.print(mainTime);
//	lcd.print("  ");
//	lcd.print(heartTime);
//	if(accustat.aveTimerStart == false){
//		lcd.print("  ");
//		lcd.print(accustat.aveTimer);
//	}

	long m = micros();
	long elapsedMicros = m - lastMicros;
	if (elapsedMicros < 0) //
			{
		elapsedMicros += MICROS_MAX;
	} else {
		lastMicros = m;
	} //don't rollover

	boolean modeWasPressed = bMode.update(elapsedMicros) > 0;
	boolean setWasPressed = bSet.update(elapsedMicros) > 0;

	// if Up/Down buttons pressed, update 'adjust' value
	int adjust = 0;
	if (bUp.update(elapsedMicros) > 0)
		adjust++;
	if (bDown.update(elapsedMicros) > 0)
		adjust--;

	if (modeWasPressed) //Serial.println("MODE BUTTON WAS PRESSED");
		if (setWasPressed) //Serial.println("SET WAS PRESSED");
			if (adjust != 0)   //Serial.println("UP OR DOWN WAS PRESSED");
				// wakeup sleep system if a button was pressed

				/* **** Possible changes made to the LCD below. May 20, 2016 by Trevor Zach and Kevin1
				 The screen sometimes makes a fuzzy display due to possible EMI
				 1. By enterState(state); we will reset the screen every time a button is pressed.
				 2. This is just a possible implementation.
				 */
				/* **** CHANGED By Trevor Zach and Kevin
				 We want the machine to be SSS_AWAKE while the truck is plugged in.
				 This can be done by looking at the truck power connection as another way to wake it up.
				 Original if (modeWasPressed || setWasPressed || adjust != 0){
				 */
				if (modeWasPressed || setWasPressed
						|| adjust != 0|| digitalRead(iTrailerPowerPin) == LOW) {
					// TRUCK original: digitalRead(iTrailerPowerPin) == LOW
					sleep.wakeup();
//    Serial.println(" button press check is calling wakeup ");
//    Serial.print(" modeWasPressed: "); Serial.print(modeWasPressed);
//    Serial.print(" setWasPressed: "); Serial.print(setWasPressed);
//    Serial.print("  adjust: "); Serial.print(adjust);
//    Serial.print("  iTrailerPowerPin: "); Serial.println(digitalRead(iTrailerPowerPin));
					// ****  enterState(state); //reset the screen
				} // end if button was pressed

	// ???? investigate the sonar pushback safety stystem of when it should shut off because
	// it is going to fast TRUCK

	// reset the Accustat when the Set button is pressed
	if (setWasPressed)
		accustat.reset();
	/*
	 LIST OF POSSIBLE STATES BELLOW
	 // UISystem states
	 #define UIS_TOWING                   0      #define UIS_SCRUM_POWER              1
	 #define UIS_SCRUM_INDIVIDUAL         2      #define UIS_SCRUM_STRENGTH           3
	 #define UIS_SCRUM_STRENGTH_CHARGE    4      #define UIS_SCRUM_STRENGTH_POSTHIT   5
	 */
	//Serial.print("UI System State:  "); Serial.println(state);
	switch (state) {
	case UIS_SCRUM_POWER:
		if (modeWasPressed) {
			enterState(UIS_SCRUM_STRENGTH);
			Serial.println("UIS_SCRUM_POWER called ui.enterState");
			accustat.reset();
		} else if (setWasPressed) {
			switch (cur_var) {
			case UIVM_POWER_THRESHOLD:
				changeVar(BAD_VARNUM);
				break;
			default:
				changeVar(UIVM_POWER_THRESHOLD);
				break;
			}
		} // end else if
		break;

	case UIS_SCRUM_INDIVIDUAL:
		if (modeWasPressed) {
			enterState(UIS_SCRUM_POWER);
			Serial.println("UIS_SCRUM_IND called ui.enterState");

			accustat.reset();
		} // Set, Adjust ignored
		break;

	default:  // a Strength mode
		if (modeWasPressed) {
			enterState(UIS_SCRUM_INDIVIDUAL);
			Serial.println("default ui loop case called enterState");
			accustat.reset();
		} else if (state == UIS_SCRUM_STRENGTH && setWasPressed) {
			switch (cur_var) {
			case UIVM_STRENGTH_THRESHOLD:
				changeVar(UIVM_STRENGTH_DIFFICULTY);
				break;
			case UIVM_STRENGTH_DIFFICULTY:
				changeVar(UIVM_STRENGTH_DURATION);
				break;
			case UIVM_STRENGTH_DURATION:
				changeVar(BAD_VARNUM);
				break;
			default:
				changeVar(UIVM_STRENGTH_THRESHOLD);
				break;
			} // end switch (cur_var)
		} // end else if
		break;
	} // end switch (state)

	// adjust current variable (if one is selected)
	if (adjust != 0 && cur_var != BAD_VARNUM)
		setVar(cur_var, getVar(cur_var) + adjust);
} // end loop

/*------------------------------------------UISystem::heartbeat()------------------------------------------
 Called from the main page heartbeat function
 // output-only (non-user-modifiable) display vars
 #define UIVN_TOWING_RESPRESS        4  // display reservoir pressure while towing
 #define UIVN__NUM                   5,
 // UISystem states
 #define UIS_TOWING                   0    #define UIS_SCRUM_POWER              1
 #define UIS_SCRUM_INDIVIDUAL         2    #define UIS_SCRUM_STRENGTH           3
 #define UIS_SCRUM_STRENGTH_CHARGE    4    #define UIS_SCRUM_STRENGTH_POSTHIT   5
 1. Depending on the state, enter showvar(state) which is below
 The variables that are shown on the screen are the threshold, difficulty, duration etc.
 */
void UISystem::heartbeat() {
	// show visible variable(s)
	//Serial.print("Begin UISystem heartbeat state: "); Serial.println(ui.state);
	switch (state) {
	case UIS_TOWING:
		showVar(UIVN_TOWING_RESPRESS);
		break;

	case UIS_SCRUM_POWER:
		showVar(UIVM_POWER_THRESHOLD);
		break;

	case UIS_SCRUM_INDIVIDUAL:
		// (no vars to show)
		break;

	case UIS_SCRUM_STRENGTH:
		showVar(UIVM_STRENGTH_THRESHOLD);
		showVar(UIVM_STRENGTH_DIFFICULTY);
		showVar(UIVM_STRENGTH_DURATION);
		break;
	} // end switch (state)
} // end UISystem::heartbeat()

/* ------------------------------------------setVar------------------------------------------
 This function is called from MasterSystem::heartbeat() , UISystem::loop()
 1. This takes the value that was passed, stores it in the arrary
 2. then returns true of false depending on if the variable changed
 */
boolean UISystem::setVar(byte vn, int val) {

	if(getState() == UIS_TOWING){
		if (val < mins[vn])
			mins[vn] = val; //changed May 23; from val = min[vn]
		if (val > maxs[vn])
			maxs[vn] = val;
	}else{
		if (val < mins[vn])
			val = mins[vn]; //changed May 23; from val = min[vn]
		if (val > maxs[vn])
			val = maxs[vn];
	}

	boolean hasChanged = vals[vn] != val;
	vals[vn] = val;

	if (hasChanged)
		master.UIVarChanged(vn, val);
	return hasChanged;
} // end setVar

/* ------------------------------------------getVar------------------------------------------
 This fucntion is called from Accustat::heartbeat() , MasterSystem::UIModeChanged( , UISystem::loop()
 1.Returns that value depending on what state was passed in
 */
int UISystem::getVar(byte vn) {
	return vals[vn];
} //getVar

/*------------------------------------------UISystem::showVar(byte)------------------------------------------
 This is called from UISystem::heartbeat() which is above^^
 1. Depending on the state, it displays the
 */
void UISystem::showVar(byte vn) {
	lcd.setCursor(cols[vn], rows[vn]);

	int val = vals[vn];

	// print leading spaces
	int w = wide[vn] - (val > 9) - (val > 99) - (val > 999);
	while (--w > 0)
		lcd.print(' '); //print spaces before the name

	lcd.print(val); //prints the value

	// print suffix
	switch (vn) {
	case UIVM_POWER_THRESHOLD:      // (fall into next)
	case UIVM_STRENGTH_THRESHOLD:
		lcd.print("00");
		break;
	case UIVM_STRENGTH_DIFFICULTY:
		lcd.print("0%");
		break;
	case UIVM_STRENGTH_DURATION:
		lcd.print("s");
		break;
	}
}      //end UISystem::showVar

/* ------------------------------------------getState()------------------------------------------
 This function is called from UISystem::loop() , MasterSystem::heartbeat()
 1. Returns that UI state
 */
byte UISystem::getState() {
	return state;
} // end getState

/*------------------------------------------UISystem::enterState------------------------------------------
 This function is called from UISystem::goStrengthPosthit , Accustat::heartbeat() , MasterSystem::accustatEnteringPosthit()
 called in UISystem::loop()
 1. Function is passed a new state and updates the screen to display new state
 2.
 */
void UISystem::enterState(byte newState) {
	cur_var = BAD_VARNUM;
	sleep.wakeup();
	/* if sleep.wakeup(); was moved into each state, we could possibly add a uisleepstate
	 This would allow for the screen to display that it is sleep, with the resevoir pressure
	 and we could probably turn off the inverter
	 */
	//  Serial.println("UISystem::enterState is forcing wakeup ");
	lcd.display();
	lcd.clear();
	//Serial.print("UI enterState: "); Serial.println(newState);
	//if(sleep.getState() == SSS_AWAKE){
	switch (state = newState) {
	case UIS_TOWING:
		inverter.neededByDumpValve(false);
		lcd.print("=== TOWING MODE ===");
		lcd.setCursor(0, 1);
		lcd.print("Reservoir Pressure:");
		break;

	case UIS_SCRUM_POWER:
		inverter.neededByDumpValve(false);
		lcd.print("** POWER DRIVING **");
		lcd.setCursor(3, 1);
		lcd.print("Threshold:");
		break;

	case UIS_SCRUM_INDIVIDUAL:
		inverter.neededByDumpValve(false);
		lcd.print("++++ INDIVIDUAL ++++");
		lcd.setCursor(0, 1);
		lcd.print("++++  TRAINING  ++++");
		break;

	case UIS_SCRUM_STRENGTH:
		halSetPushbackDumpValve(LOW);
		inverter.neededByDumpValve(true); // inverter will be needed soon to power dump valve
		lcd.print(" STRENGTH TRAINING ");
		lcd.setCursor(2, 1);
		lcd.print("Threshold:");
		lcd.setCursor(2, 2);
		lcd.print("Difficulty:"); // TRUCK possible "y" on lcd?
		lcd.setCursor(2, 3);
		lcd.print("Duration:");
		break;

		// Below is when the machine is pushing back
	case UIS_SCRUM_STRENGTH_CHARGE:
		initcharge.enable(false);  // lock in Init Charge pressure
		halSetPushbackDumpValve(HIGH);
		lcd.print(" STRENGTH TRAINING ");
		lcd.setCursor(6, 2);
		lcd.print("CHARGING");
		break;

	case UIS_SCRUM_STRENGTH_POSTHIT:
		halSetPushbackDumpValve(LOW);
		inverter.neededByDumpValve(false);
		lcd.print(" STRENGTH TRAINING ");
		if (strengthPosthitCode == UISPH_SUCCESS) {
			lcd.setCursor(6, 2);
			lcd.print("SUCCESS");
		} else {
			// shutdown
			lcd.setCursor(6, 1);
			lcd.print("SHUTDOWN");
			lcd.setCursor(6, 2);
			switch (strengthPosthitCode) {
			case UISPH_TOO_HIGH:
				lcd.print("Difficulty is set to high adujust down"); //difficulty is to high
				break;
			case UISPH_TOO_FAST:
				lcd.print("Hold scrum longer");//did not complete duration
				break;
			case UISPH_TOO_MUCH:
				lcd.print("Reduce "); //may be not needed
				//TRUCK add a print to the lcd for when they are above the threshold and the buzzer is going off
				break;
			default:
				lcd.print("?? ");
				lcd.print(strengthPosthitCode);
				lcd.print(" ??");
				break;
			}
			lcd.setCursor(8, 3);
			lcd.print(strengthPosthitValue);
		}
		break;
	} // switch (state = newState)

	//  }else{// closes if(sleep.getState)
	//      lcd.print("=== SLEEPING ===");
	//      lcd.setCursor(0, 1);  lcd.print("Reservoir Pressure:");
	//      showVar(UIVN_TOWING_RESPRESS);
	//  }
   // Serial.print("UIenterState called UImode: "); Serial.print(state); Serial.println("  lastUI: ");Serial.println(master.getLastUIState());
    if (master.getLastUIState() != state) {
		master.UIModeChanged(state);
		Serial.println("ui.IF(last!=State)");
	}
} // end UISystem::enterState

/* ------------------------------------------goStrengthPosthit------------------------------------------
 Called from MasterSystem::loop()
 1. This function assigns a code, and value that will be used in other functions
 */
void UISystem::goStrengthPosthit(byte SPH_code, int val) {

	//  Serial.print("goStrengthPosthit started. SPH_code: "); Serial.println(SPH_code);
	//  Serial.print("val: "); Serial.println(val);

	strengthPosthitCode = SPH_code;
	strengthPosthitValue = val;
	switch (SPH_code) {
	case UISPH_TOO_HIGH:
		break;
	case UISPH_TOO_FAST:
		break;
	case UISPH_TOO_MUCH:
		break;
	case UISPH_SUCCESS:
		break;
	default:
		break;
	}

	enterState(UIS_SCRUM_STRENGTH_POSTHIT);
} // end goStrengthPosthit

/*------------------------------------------changeVar------------------------------------------
 Called from UISystem::loop()
 1. this deletes old variable, and prints the new variables that can be modified
 */
void UISystem::changeVar(byte newVar) {
	// erase old "current variable" chevrons
	if (cur_var != BAD_VARNUM) {
		lcd.setCursor(cols[cur_var] - CHEVRON_LEFT, rows[cur_var]);
		lcd.print(" ");
		lcd.setCursor(cols[cur_var] + CHEVRON_RIGHT, rows[cur_var]);
		lcd.print(" ");
	}

	cur_var = newVar;

	// draw new "current variable" chevrons
	if (cur_var != BAD_VARNUM) {
		lcd.setCursor(cols[cur_var] - CHEVRON_LEFT, rows[cur_var]);
		lcd.print(">");
		lcd.setCursor(cols[cur_var] + CHEVRON_RIGHT, rows[cur_var]);
		lcd.print("<");
	}
} // end changeVar

