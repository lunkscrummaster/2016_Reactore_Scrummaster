/*
 * SS.cpp
 *
 *  Created on: May 30, 2016
 *      Author: Prospect
 */
#include "SS.h"

//-----------------------------------------SLEEP_CONSTRUCTOR-----------------------------------------
SleepSystem::SleepSystem() {
  wakeup(); //initilizes the system to be away during the setup
}//end SleepSystem::SleepSystem()

/*----------------------------------------- SleepSystem::heartbeat()-----------------------------------------
    This function is called from the main heartbeat function
    timeoutHeartbeats = SLEEP_TIMEOUT_SECONDS * HEARTBEATS_PER_SECOND;
    SLEEP_TIMEOUT_SECONDS  (15 * 60)
    HEARTBEATS_PER_SECOND  4
    timeoutHeartbeats = 3600
    1. If we are in SSS_AWAKE state, decrement the counters
    2. Else, enter the SSS_ASLEEP state
    #define SSS_ASLEEP  0
    #define SSS_AWAKE   1
    enterState(SSS_ASLEEP) IS below
 *  **** A Changes Made May 19th, 2016 By: Trevor Richardson
    1. The batteries should be linked whenever the system is now in SSS_ASLEEP
    2. When the system is asleep, the batteries should not be connected
*/
void SleepSystem::heartbeat() {
  //Serial.print("Sleep system state: "); Serial.println(sleep.state);
  // Serial.print(" timeoutHeartbeats: "); Serial.println(timeoutHeartbeats);
  //  Serial.print("oBattLink:  "); Serial.print(digitalRead(oBatteryLink));



  switch (state) {
    case SSS_AWAKE:
      if (timeoutHeartbeats > 0)  {
        timeoutHeartbeats--; //decrement the timer
        //       Serial.print("  timeoutHeartbeats = "); Serial.println(timeoutHeartbeats);
      }
      else if (true /* canSleep */) {
        //       Serial.println("gone to sleep@@@@@@@@@@@@@@");
        enterState(SSS_ASLEEP);//go to sleep
        /* **** Changed May 20,th 2016 by trevor zach and kevin
            digitalWrite(oBatteryLink, LOW); // **** B Disconnect the batteries when ASLEEP
          // You can't disconnect the batteries here, because while the machine is asleep,
          it may need to connect the batteries for the inverter to add air to the resevoir
          find a new place to do this
        */
        //digitalWrite(oBatteryLink, LOW); // moved to enterState
      } // end else if
      break;
  }// end switch
}// end SleepSystem::heartbeat()

/* -----------------------------------------SleepSystem::wakeup()-----------------------------------------
 *  This is called from UIS.cpp when a button is pushed in the sleep mode
 *  1. This function just calls another function to enter into the awake state, which is below
*/
void SleepSystem::wakeup() {
  // Serial.println(" THE SYSTEM ENTERING AWAKE ");
  enterState(SSS_AWAKE);
} //end SleepSystem::wakeup

/* -----------------------------------------SleepSystem::enterState-----------------------------------------
 *  This is called from SleepSystem::wakeup() , SleepSystem::heartbeat(),
 *  1. If in the sleep state, stay in the sleep state
 *  2. If told to wake up, from SleepSystem::wakeup(), reset the timeoutHeartbeats
*/
void SleepSystem::enterState(byte newState) {

  switch (state = newState) {
    case SSS_ASLEEP:
      digitalWrite(oDisplayPowerPin, LOW);
      /* **** Changes made May 24 by trevor and Zach
          We had the inverter on while sleep from strength mode.
          Fix, if we enter sleep, with inverter on, this will turn it off.
      */
      if (digitalRead(iInverterOnPin)) {
        inverter.neededByDumpValve (false); // turn off inverter flag for dump valve
      }
      digitalWrite(oBatteryLink, LOW); //unlink batteries
      break;

    case SSS_AWAKE:
      if (ui.getState() > 2) {
        inverter.neededByDumpValve (true); //turn on inverter
      }
      digitalWrite(oBatteryLink, HIGH); //link batteries
      timeoutHeartbeats = SLEEP_TIMEOUT_SECONDS * HEARTBEATS_PER_SECOND;
      // Serial.println("The SSS_awake enterstate fucntion reset timeoutheartbeats");
      // added analogRead(aiScrumPin) == LOW to see if that would help the LED screen from flickering
      if (digitalRead(iTrailerPowerPin) == HIGH && analogRead(aiScrumPin) == HIGH) { //truck not plugged in
        // TRUCK original: digitalRead(iTrailerPowerPin) == HIGH
        digitalWrite(oDisplayPowerPin, HIGH);      //turn on LED
      }
      break;
  }//end switch
}// end SleepSystem::enterState

/* -----------------------------------------SleepSystem::getState()-----------------------------------------
    This function is called in MAS.cpp
    1. Returns the SleepSystem state
*/
byte SleepSystem::getState() {
  return state;
} // end SleepSystem::getState()





