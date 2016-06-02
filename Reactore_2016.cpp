#include "Reactore_2016.h"


/* Hi Kevin. Hope you are doing well. If you want to change the TRAVEL_TIME, which is the length of time
    the sled will be pushed after there was a successful push.
    1000 = 1 second
    2000 = 2 seconds etc...
*/

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

  accustat.heartbeat();     //AS.cpp
  comp.heartbeat();         //CIS.cpp comp = compressor system
  initcharge.heartbeat();   //ICS.cpp initial charge system
  inverter.heartbeat();     //CIS.cpp inverter = inverter system
  master.heartbeat();       // MAS.cpp
  outriggers.heartbeat();   // ORS.cpp
  pushback.heartbeat();     // PBS.cpp
  sleep.heartbeat();        // SS.cpp
  ui.heartbeat();           // UIS.cpp

}

//--------------------------SETUP----------------------------------------------------------------
void setup() {
#ifdef DEBUG
  debugSetup();
#endif

  halSetup(); //sets up the input and output pins used the board

  Serial.begin(9600); //sets up serial communication

  lcd.begin(20, 4); //sets up screen

  heartbeatTimer.every(1000L / HEARTBEATS_PER_SECOND, heartbeat); //calls heartbeat function when timer goes off.

  chargeAlternateTimer.every(CHARGE_ALTERNATE_MINUTES * 60L * 1000, chargeAlternateCallback);

  initPushbackAve();

  sonarTimer.initialize(140000);

  sonarTimer.attachInterrupt(sonarISR);

  interrupts();

  DEBUG_PRINT_S("Setup done\n");

  // Serial.println("Setup Complete");
}

//--------------------------LOOP----------------------------------------------------------------
void loop() {

  ui.loop();          // call debouncer frequently

  accustat.loop();    // to average pushback-arm pressure readings

  outriggers.loop();  // fast update of outrigger balancing system

  /* Code added May 21 by Trevor and Zach
      The code below is used for the successtimer which is what is used to determine how long the
      sled can be pushed after a success. Code handles rollovers after approx 50days
  */
  if (master.successStartTime > 0) {
    long currentMillis = millis() - master.successStartTime;
    if (currentMillis < 0)
      currentMillis += MILLIS_MAX; // rollover
    if (currentMillis > TRAVEL_TIME) {
      digitalWrite(oSuccess, LOW);
      master.successStartTime = 0;
    }

  }
  master.loop();      // during Strength Charge phase

  heartbeatTimer.update();

  chargeAlternateTimer.update();

  //Serial.print("  ************************ time for main loop:    "); Serial.println(millis() - StartLoop);
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

	int oldReading = master.pushbackSonar[master.pushbackIndex];
	int temp =  analogRead(aiPushbackSonar)*5;

	if(temp > 450 || temp < 290){
		if(master.pushbackIndex == 0){
			master.pushbackSonar[master.pushbackIndex] = master.pushbackSonar[AVE_ARRAY_SIZE-1];
		}else{
			master.pushbackSonar[master.pushbackIndex] = master.pushbackSonar[master.pushbackIndex-1];
		}
	}else{
		master.pushbackSonar[master.pushbackIndex] =  temp;
	}
	master.pushbackSonarAve += master.pushbackSonar[master.pushbackIndex]/AVE_ARRAY_SIZE-oldReading/AVE_ARRAY_SIZE;
	master.pushbackIndex++;

	if(master.pushbackIndex == AVE_ARRAY_SIZE)
		master.pushbackIndex = 0;

	//  Serial.print("ISR Read: "); Serial.print(master.pushbackSonar[index]);
	//  Serial.print(" ANalog Read:   "); Serial.print(analogRead(aiPushbackSonar));
	//  Serial.print(" reading: "); Serial.println(reading);


	//  if (pushback.readyRaiseTo <= master.pushbackSonar[AVE_ARRAY_SIZE - 1] &&
	//      pushback.getState()   == PBS_READY2_RAISING                       &&
	//      sleep.getState()      == SSS_AWAKE) {
	//
	//    // start settling
	//    pushback.enterState(PBS_READY3_SETTLING);
	//    //pushback.heartbeat();
	//  } // end if
	/*    CODE PLANNING TO CHECK FOR BALL IN, AND EMERGENCY SHUTDOWN
	 *   if (accustat.returnmode() == AS_HITTING)  // if we are hitting and need
	 *   {
	 *   //check for ball
	 *   }
		if (moving to fast)
		shutdown
	*/
} // end sonarISR()

// --------------------------------initPushbackAve()-------------------------------------------------------------------------------
void initPushbackAve(){
	  for (int i = 0; i < AVE_ARRAY_SIZE; i++) {
	    master.pushbackSonar[i] = analogRead(aiPushbackSonar)*5; //fill pushback sonar array
	    master.pushbackSonarAve += master.pushbackSonar[i]/AVE_ARRAY_SIZE;
	    master.outriggerTightSonar[i] = analogRead(aiOutriggerTightSonar)*5;
	    master.outriggerLooseSonar[i] = analogRead(aiOutriggerLooseSonar)*5;
	  }
}

