#include "Reactore_2016.h"

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);

//--------------------------DEBUG------------------------------------------------------------
#ifdef DEBUG  // DEBUGGING

void debugSetup() {
  Serial.begin(9600);
  Serial.println("==================================================");
}

void debugPrintS(const char* s)  {
  Serial.print(s);
}

void debugPrintI(int i) {
  Serial.print(i);
}

int debugRead() {
  return Serial.read();
}

extern void beep(byte count);  // AS.cpp

#endif


//--------------------------CLASS_INSTANCIATION------------------------------------------------------------
MasterSystem         master;

Accustat             accustat;
CompressorSystem     comp;
InitialChargeSystem  initcharge;
InverterSystem       inverter;
OutriggerSystem      outriggers;    // handles both outriggers
PushbackSystem       pushback;
SleepSystem          sleep;
UISystem             ui(lcd);


//--------------------------TIMERS------------------------------------------------------------
Timer heartbeatTimer;
Timer chargeAlternateTimer;

//--------------------------CHARGE_ALTERNATE------------------------------------------------------------

void chargeAlternateCallback() {
  digitalWrite(oChargeAlternatePin, ! digitalRead(oChargeAlternatePin));
  //  Serial.print(" Charge alternate pin is: "); Serial.println(digitalRead(oChargeAlternatePin));
}

//--------------------------MAIN_HEARTBEAT----------------------------------------------------------------
void heartbeat() {
int heartStart = millis();
  accustat.heartbeat();     //AS.cpp
  comp.heartbeat();         //CIS.cpp comp = compressor system
  initcharge.heartbeat();   //ICS.cpp initial charge system
  inverter.heartbeat();     //CIS.cpp inverter = inverter system
  master.heartbeat();       // MAS.cpp
  outriggers.heartbeat();   // ORS.cpp
  pushback.heartbeat();     // PBS.cpp
  sleep.heartbeat();        // SS.cpp
  ui.heartbeat();           // UIS.cpp
ui.heartTime = millis()-heartStart;
}

//--------------------------SETUP----------------------------------------------------------------
void setup() {
#ifdef DEBUG
  debugSetup();
#endif

  halSetup(); //sets up the input and output pins used the board

  Serial.begin(9600); //sets up serial communication

  lcd.begin(20, 4); //sets up screen

  heartbeatTimer.every(100L / HEARTBEATS_PER_SECOND, heartbeat); //calls heartbeat function when timer goes off.

  chargeAlternateTimer.every(CHARGE_ALTERNATE_MINUTES * 60L * 1000, chargeAlternateCallback);

  initPushbackAve();

  sonarTimer.initialize(134000);

  sonarTimer.attachInterrupt(sonarISR);

  interrupts();

  DEBUG_PRINT_S("Setup done\n");

  // Serial.println("Setup Complete");
}

//--------------------------LOOP----------------------------------------------------------------
void loop() {

	int startLoop = millis();

  ui.loop();          // call debouncer frequently

  accustat.loop();    // to average pushback-arm pressure readings

 if(outriggers.getBalanceMode() == true)
	 outriggers.loop();  // fast update of outrigger balancing system

  /* Code added May 21 by Trevor and Zach
      The code below is used for the successtimer which is what is used to determine how long the
      sled can be pushed after a success. Code handles rollovers after approx 50days
  */
  if (master.successStartTime > 0) {
	unsigned long currentTime = millis();
	unsigned long currentMillis = 0;
	if(currentTime > master.successStartTime)
		currentMillis = currentTime - master.successStartTime;
	else
		currentMillis = currentTime - master.successStartTime + MILLIS_MAX; // rollover
    //Serial.print(" currentMillis; "); Serial.println(currentMillis);
    if (currentMillis > TRAVEL_TIME) {
      digitalWrite(oSuccess, LOW);
      master.successStartTime = 0;
      master.successOverFlag_AS = true;
      master.successOverFlag_UI = true;

      Serial.println("____________________SUCCESS_OVER______");

      Serial.print(" AS over  flag: "); Serial.print(master.successOverFlag_AS);
      Serial.print(" UI over flag: "); Serial.print(master.successOverFlag_UI);
      Serial.print(" dumpValveFlag: "); Serial.print(accustat.dumpValveFlag);
      Serial.print(" beeperFlag: "); Serial.print(accustat.beeperFlag);
      Serial.print(" restartStrengthFlag: "); Serial.println(ui.restartStrengthFlag);

      Serial.print(" AS: "); Serial.print(accustat.returnState());
      Serial.print(" PB: "); Serial.print(pushback.getState());
      Serial.print(" SS: "); Serial.print(sleep.getState());
      Serial.print(" UIS: "); Serial.println(ui.getState());
    }

  }
  master.loop();      // during Strength Charge phase
  ui.mainTime = millis()-startLoop;

  heartbeatTimer.update();

  chargeAlternateTimer.update();


//  Serial.print("  ************************ time for main loop:    "); Serial.println(millis() - startLoop);
  //Serial.print("  The truck pin is:  "); Serial.println(digitalRead(iTrailerPowerPin));

} // end loop

// --------------------------------sonarISR()-------------------------------------------------------------------------------
/*  Function is called from a timer interrupt
 *  1. First shifts the array
 *  2. Add latest reading to last part of the array
 *  3. Enter PBS_READY3_SETTLING if awake, if PBS is raising, if arm has gone far enough
 *  4. TRUCK ADD POSSIBLE BALL CHECKS FOR TIGHT/LOOSE SONARS
 *  5. TRUCK ADD POSSIBLE CHECK FOR PUSHBACK MOVING TO FAST ie. settings to high
*/
void sonarISR() {                 //****added to constanty read pushback sonar. can add master shutdown control here later.

	if(!master.noInterrupts){
		master.pushbackSonarAve -= master.pushbackSonar[master.pushbackIndex]/AVE_ARRAY_SIZE;
		int temp =  analogRead(aiPushbackSonar);

		if(temp > 80 || temp < 58){
			if(master.pushbackIndex == 0){
				master.pushbackSonar[master.pushbackIndex] = master.pushbackSonar[AVE_ARRAY_SIZE-1];
			}else{
				master.pushbackSonar[master.pushbackIndex] = master.pushbackSonar[master.pushbackIndex-1];
			}
		}else{
			master.pushbackSonar[master.pushbackIndex] =  temp;
		}
		master.pushbackSonarAve += master.pushbackSonar[master.pushbackIndex]/AVE_ARRAY_SIZE;

		/*    CODE PLANNING TO CHECK FOR BALL IN, AND EMERGENCY SHUTDOWN
		 *   if (accustat.returnmode() == AS_HITTING)  // if we are hitting and need
		 *   {
		 *   //check for ball
		 *   }
			if (moving to fast)
			shutdown
		*/
		if(ui.getState() == UIS_SCRUM_STRENGTH_CHARGE){
			if(accustat.returnmode() == ASM_STRENGTH && accustat.returnState() == AS_HITTING){
				if(master.pushbackSonar[master.pushbackIndex] > 78) {
					pushback.enterState(PBS_READY1_SINKING);
					ui.goStrengthPosthit(UISPH_TOO_HIGH, 0);
					ui.pbTooFar = true;

				}
			}
		}
		//TRUCK, NO POINT TO CHECK FOR BALLS WHEN INDIVIDUAL
		/* original was if (accustat.returnState() == AS_HITTING)
		 * no point checking for a ball if we are in indiviual mode
		*/
		if(accustat.lookForBall == true){
			if(analogRead(aiLoose_ball_sonar) < 70 || analogRead(aiTight_ball_sonar) < 70){
	//			ui.lb = analogRead(aiLoose_ball_sonar);
	//			ui.tb = analogRead(aiTight_ball_sonar);
				accustat.setHasSeenBall(true);
				accustat.lookForBall = false;
				accustat.printBall = true;
				if(accustat.returnmode() == ASM_STRENGTH){
					master.UIModeChanged(UIS_SCRUM_STRENGTH_CHARGE);
					ui.enterState(UIS_SCRUM_STRENGTH_CHARGE);
				}
			}
		}
		master.pushbackIndex++;
		if(master.pushbackIndex == AVE_ARRAY_SIZE)
			master.pushbackIndex = 0;
	}
} // end sonarISR()

// --------------------------------initPushbackAve()-------------------------------------------------------------------------------
void initPushbackAve(){
	  for (int i = 0; i < AVE_ARRAY_SIZE; i++) {
	    master.pushbackSonar[i] = analogRead(aiPushbackSonar); //fill pushback sonar array
	    master.pushbackSonarAve += master.pushbackSonar[i]/AVE_ARRAY_SIZE;
	    master.outriggerTightSonar[i] = analogRead(aiOutriggerTightSonar);
	    master.outriggerLooseSonar[i] = analogRead(aiOutriggerLooseSonar);
	  }
}

