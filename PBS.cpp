/*
 * PBS.cpp
 *
 *  Created on: May 30, 2016
 *      Author: Prospect
 */
#include "PBS.h"

//--------------------------------------PUSHBACK_CONSTRUCTOR--------------------------------------
PushbackSystem::PushbackSystem() {
	state = PBS_QUIET;
	readyRaiseTo = 0;
	readySinkTo = 0;
	settlingTimeout = 0;
	enabled = false;
}

/* --------------------------------------PushbackSystem::heartbeat()--------------------------------------
    This is called from the heartbeat function in the main program page
*/
void PushbackSystem::heartbeat() {
	noInterrupts();
	volatile int son = master.pushbackSonarAve;//find value of pushback sonar
	interrupts();

  switch (state) {
    case PBS_READY1_SINKING:
      /* Changes made May 20, 2016 by Trevor, Zach and Kevin
          readySinkTo is the lower limit. The machine will sink to this value, or below then start raising.
          the machine will raise until readyRaiseTo is less than or equal to the sonar value
          once this raise is complete, the machine enters a settling state for apprx 1sec, then is quiet
      */
      if (readySinkTo >= son) {
        // start raising
        digitalWrite(oAirSpringLockout, HIGH);  //written high to allow air into the springs
        Serial.println(" PBS heartbeat changed PBS State ");
        enterState(PBS_READY2_RAISING);
      }
      break;

    case PBS_READY2_RAISING:
      // Serial.print("  readyRaiseTo= ");Serial.print(readyRaiseTo);
      // Serial.print("  son = "); Serial.println(son);
      if (readyRaiseTo <= son) {
        // start settling
        enterState(PBS_READY3_SETTLING);
      }
      break;

    case PBS_READY3_SETTLING:
      if (settlingTimeout-- <= 0) {
        // finished settling
        enterState(PBS_QUIET);
        master.pushbackIsReady();
      }
      break;
  } // end switch (state)
} //end pushback heartbeat

/* --------------------------------------PushbackSystem::enable--------------------------------------
    This function is called from MasterSystem::heartbeat()
    1. Enables, or disables the "enabled" variable that belongs to the pushback system class
    2. This is set to true or false

*/
void PushbackSystem::enable(boolean en) {
  if (en) {
    if (!enabled) {
      // enabling
      enabled = true;
    } // end if
  } else {
    if (enabled) {
      // disabling
      enabled = false;
      enterState(PBS_QUIET);
    } // end if (enabled)
  } // end else
} // end PushbackSystem::enable

/* --------------------------------------PushbackSystem::enterState--------------------------------------
 *  this takes a new state, or current state
    Function is called from MAS.cpp
    1. depending on the state, depends on what it does
    2. View indiviudual states for further comments
*/
void PushbackSystem::enterState(byte newState) {
  state = newState;
  //  Serial.print ("pushback system new state: "); Serial.println(state);
  switch (state) {
    case PBS_QUIET:
      halSetPushbackUpDown(0);
      //outriggers.setBalanceMode(false);
      break;

    case PBS_READY1_SINKING:
      digitalWrite(oAirSpringLockout, LOW); // when low, suspension is availble to go down
      accustat.pause();
      halSetPushbackUpDown(-1);
      break;

    case PBS_READY2_RAISING:
//      Serial.println("PBS_READY2_RAISE called setBalanceMode");
      halSetPushbackUpDown(1);
      outriggers.setBalanceMode(true); //beging to balance the machine
      break;

    case PBS_READY3_SETTLING:
      halSetPushbackUpDown(0);
      //outriggers.setBalanceMode(false);
      settlingTimeout = SETTLING_COUNT;
      break;
  } // end switch (state)
}//end enterState

/* --------------------------------------goReady--------------------------------------
 * , is called from MAS.cpp
    1. changes the accustate state to whatever state was passed in
    2. enters new state of PBS_READY1_SINKING which is ^^^
*/
void PushbackSystem::goReady(byte asMode, int sinkTo, int raiseTo) {
  readySinkTo  = sinkTo;
  readyRaiseTo = raiseTo;
  accustat.setMode(asMode);
//  Serial.println(" goReady changed PBS STATE");
  enterState(PBS_READY1_SINKING);// this is above^^
}//end goReady

/* --------------------------------------PushbackSystem::getState()--------------------------------------
 *  Called from sonarISR() , OutriggerSystem::loop()
 *  1. Returns the current state from the PushbackSystem
*/
byte PushbackSystem::getState() {
  return state;
} // end PushbackSystem::getState()

//******************** WHY IS THIS COMMENTED OUT?????????????
//void PushbackSystem::reReady() {
//  accustat.pause();
//  enterState(PBS_READY2_RAISING);
//}



