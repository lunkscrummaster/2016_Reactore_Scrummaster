/*
 * ORS.cpp
 *
 *  Created on: May 30, 2016
 *      Author: Prospect
 */
#include "ORS.h"

//--------------------------------------OUTRIGGER_CONSTRUCTOR--------------------------------------
OutriggerSystem::OutriggerSystem() {
	inBalanceMode = false;
	outriggersFirstPumpDone = true;
}

/*--------------------------------------OUTRIGGER_LOOP--------------------------------------
 * OutriggerSystem checks if the machine is inBalanceMode, and raises the loose up or down
   ORS_BALANCE_TRIP == 40 WHICH IS ABOVE ^^^
*/
void OutriggerSystem::loop() {
  //  Serial.println("Outrigger Loop started");
  if (outriggersFirstPumpDone == false && pushback.getState() == PBS_QUIET) {
    digitalWrite(oOutriggerLooseUp, HIGH);
    digitalWrite(oOutriggerTightUp, HIGH);
    delay(50);
    digitalWrite(oOutriggerLooseUp, LOW);
    digitalWrite(oOutriggerTightUp, LOW);
    outriggersFirstPumpDone = true;
  } // end if(outriggersFirstPumpDone == false)

 // Serial.print("The balance mode is : "); Serial.println(outriggers.inBalanceMode);

  if (inBalanceMode && pushback.getState() == PBS_QUIET) {
    volatile int ld = master.getOutriggerLooseAve(); //was 2
    volatile int td = master.getOutriggerTightAve();
    //    Serial.print(" OutRiggers Loose: ");  Serial.print(ld); Serial.print(" OutRiggers Tight: "); Serial.print(td);
    //    Serial.print(" Dif: "); Serial.println(ld - td);
    if (ld > 0 && td > 0) { // if both are reading good values
      if (ld - td < - ORS_BALANCE_TRIP) {
        digitalWrite(oOutriggerLooseUp, HIGH);
        delay(50);
        digitalWrite(oOutriggerLooseUp, LOW);
        delay(150);
      } else if (ld - td > ORS_BALANCE_TRIP) {
        digitalWrite(oOutriggerLooseUp, LOW);
      } else { // end 1st else
//    	  Serial.println(" inBalanceFalse ");
    	  accustat.resume();
    	  delay(500);
//    	  accustat.setNaturalPreCharge();
    	  accustat.setPreChargeFlag = true;
    	  setBalanceMode(false);
    	  master.balancingDone = true;
    	  //delay(700);
      }//ends last else
/*
      if (digitalRead(oOutriggerLooseUp)) {
        if (ld - td > ORS_BALANCE_TRIP)
          digitalWrite(oOutriggerLooseUp, LOW);
      } else if (ld - td < - ORS_BALANCE_TRIP) {
        digitalWrite(oOutriggerLooseUp, HIGH);
        delay(100);
        digitalWrite(oOutriggerLooseUp, LOW);
      } else { // end 1st else
        setBalanceMode(false);
      }//ends last else
*/
    } // end  if (ld > 0 && td > 0)

  } // end if(inBalanceMode)
}// end of outrigger system

/* --------------------------------------OutriggerSystem::heartbeat(void)--------------------------------------
    Called from main loop
    1. Updates the outriggers averages
    2. Can be printed for debug
*/
void OutriggerSystem::heartbeat(void) {

//  Serial.println("Outrigger System Heartbeat Started"); // ???? Comment these reading values when code is complete to save time
//  int ol = master.getOutriggerLooseAve();
//  int ot = master.getOutriggerTightAve();
//    Serial.print("  Loose Sonar Value "); Serial.print(ol);
//    Serial.print("  Tight Sonar Value "); Serial.println(ot);
} // end OutriggerSystem::heartbeat(void)


/* --------------------------------------setBalance Mode--------------------------------------
    This function is called from the PushbackSystem::enterState
    1. This function takes a true/false, and sets the inBalance mode
    2. if it is inBalanceMode is true, and false has been passed, change inBalanceMode to false
    3. if inBalanceMode is false, and true has been passed, change inBalanceMode to true
*/
void OutriggerSystem::setBalanceMode(boolean en) {

  //  Serial.print("Outrigger balance mode: "); Serial.println(inBalanceMode);

  if (inBalanceMode) {
    if (! en) {
      // disabling balance mode, and end timer
      inBalanceMode = false;
      //      balanceTimer.stop();
      digitalWrite(oOutriggerLooseDown, LOW);
      digitalWrite(oOutriggerLooseUp,   LOW);
      digitalWrite(oOutriggerTightDown, LOW);
      digitalWrite(oOutriggerTightUp,   LOW);
    } // end if(! en)
  } else {
    if (en) {
      // enabling balance mode, and start timmer
      inBalanceMode = true;
      master.fillOutriggerArray();
      outriggersFirstPumpDone = false;
      //      balanceTimer.restart();
    } // end if (en)
  } // end else
} // end setBalanceMode


//--------------------------------------GET_BALANCE_MODE--------------------------------------
bool OutriggerSystem::getBalanceMode(){
	return inBalanceMode;
}

