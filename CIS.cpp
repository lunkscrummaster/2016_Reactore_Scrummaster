/*
 * CIS.cpp
 *
 *  Created on: May 30, 2016
 *      Author: Prospect
 */
#include "CIS.h"
#include "SS.h"
#include "HAL.h"
#include "Reactore_2016.h"
#include "pinDefs.h"  // inverterPin, inverterOnPin

//----------------------------------- CompressorSystem_CONSTRUCTOR-----------------------------------
CompressorSystem::CompressorSystem() {
	  state = 0;
	  pvTankPressMin = 700;
	  pvTankPressMax = 940;
	  pvUnloaderTimeout = 0;
}


/*----------------------------------- CompressorSystem::heartbeat-----------------------------------
 *  This is called from the heartbeat() in the main page
 *  1. Reads the reservoir pressure
 *  2. Fills tank when it is too low, enters starting state
 *  3. Waits for one loop through, then enters the on state
 *  4. Once tank is filled to correct pressure, enter stopping state
 *  5. Waits for on loop through, then enters the off state
*/
void CompressorSystem::heartbeat(void){
  //Serial.print("Compressor System Heartbeat Started, State: "); Serial.println(comp.state);

  int reservoirPressure = analogRead(aiReservoirPin);
  if(reservoirPressure < pvTankPressMin-100)
	  reservoirPressure = pvTankPressMin;

  switch(state){
    case CSS_OFF:
      if (reservoirPressure < pvTankPressMin) { //if the compressor is off, and the tank is to low, turn it on
        inverter.neededByCompressor(true);
//        Serial.print(" reservoirPressure: "); Serial.println(reservoirPressure);
        digitalWrite(oUnloaderPin, HIGH); //dump pressure for 2 heart beats before compressor is truned on
        pvUnloaderTimeout = 2;    // number of heartbeat intervals to wait for unloader
        enterState(CSS_STARTING); //compressor system is now starting
      } else
        digitalWrite(oCompressorPin, LOW); //turn off compressor
      break;

    case CSS_STARTING:
      if (pvUnloaderTimeout > 1) {
        pvUnloaderTimeout--;  // keep waiting  This works allongs as
      } else if (digitalRead(iInverterOnPin)) {
        digitalWrite(oUnloaderPin, LOW);        // unloader has timed out so close it
        digitalWrite(oCompressorPin, HIGH); //start compressor
        //Serial.print("oCompressorPin: "); Serial.print(oCompressorPin);
        enterState(CSS_ON); //the compressor is now on
      }  // end else if
      break;

    case CSS_ON:
      if (reservoirPressure > pvTankPressMax) {
        digitalWrite(oCompressorPin, LOW); //turn off if tank is pressurized
        inverter.neededByCompressor(false); //compressor is no longer needed by inverter
        pvUnloaderTimeout = 2;    // number of heartbeat intervals to wait for unloader
        enterState(CSS_STOPPING); //enter the stopping state
      } else
        digitalWrite(oCompressorPin, HIGH); //if tank pressure is not achieved, keep  compressor on
      break;

    case CSS_STOPPING:
      if (pvUnloaderTimeout > 1) {
        pvUnloaderTimeout--;  // keep waiting
      } else {
        // unloader has timed out so close it
        digitalWrite(oUnloaderPin, LOW);
        enterState(CSS_OFF); //compressor is now off
      }
      break;
  } //end switch (state)
} //end CompressorSystem::heartbeat


/* -----------------------------------CompressorSystem::enterState(byte)-----------------------------------
 *  This function is called in CompressorSystem::heartbeat above ^^
 *  1. This just sets state to the new state,
 *  	heartbeat handles the rest of switching compressor states
*/
void CompressorSystem::enterState(byte newState) {
  state = newState;
//  Serial.print("Compressor System enterState: "); Serial.println(newState);
} // end CompressorSystem::enterState


//-----------------------------------InverterSystem_CONSTRUCTOR-----------------------------------
InverterSystem::InverterSystem(){
  pinMode(oInverterPin, OUTPUT);
  digitalWrite(oInverterPin, LOW);
  pinMode(iInverterOnPin, INPUT);
  nbCompressor  = false;
  nbDumpValve = false;
}

/* -----------------------------------InverterSystem::heartbeat()-----------------------------------
 *  This function is called from heartbeat() in the main page
 *  1. The heartbeat turns the inverter on or off when it is needed
 *  I think the inverter works like a button that turns a light on and off.
 *  Push the button and it turns the light(inverter) on.
 *  Push the button again, and it turns the light(inverter) OFF
*/
void InverterSystem::heartbeat() {
  //Serial.println("Inverter System heartbeat start ");

  boolean enabled = nbCompressor || nbDumpValve; //either these need inverter?

  byte ac = digitalRead(iInverterOnPin);
  // iInverterOnPin: High when inverter on
  // iInverterOnPin: Low when inverter off

  byte st = 0 ;
  if (!enabled && !ac)  st = 0;
  if ( enabled && !ac)  st = 1;
  if ( enabled && ac)  st = 2;
  if (!enabled && ac)  st = 3;

  switch (st) {
    case 1:  // starting
      digitalWrite(oBatteryLink, HIGH);
      //Serial.println("Inverter starting");    // link the batteries while inverter is running
    case 3:  // stopping
      if (! digitalRead(oInverterPin)){ // Arduino digitalRead lets you read from pins declared as output
        digitalWrite(oInverterPin, HIGH);} //Serial.println("Inverter stopping");  // begins pressing button on inverter
//      Serial.println("**************************WR ITTEN HIGH**************");}
      break;
// ???? whenever the system is awake, the batteries should be linked
    case 0:  // stopped
  //  Serial.println("inverter stopped");
     // ****  digitalWrite(oBatteryLink, LOW); Serial.println("Inverter stopped");   // unlink the batteries while inverter not running
    case 2:  // running
      if (digitalRead(oInverterPin)){
        digitalWrite(oInverterPin, LOW);  // stops pressing button when inverter has started/stopped
 //     Serial.println("^^^^^^WRITTEN LOW^^^^^^^^");
      }
      break;
  } //end switch
} // end InverterSystem::heartbeat()


/* -----------------------------------InverterSystem::neededByCompressor-----------------------------------
 *  This function is called in CompressorSystem::heartbeat
 *  1. nbCompressor is a boolean, and just sets that the flag to true or false
*/
void InverterSystem::neededByCompressor(boolean en) {
  nbCompressor = en;
//  Serial.println(" neededByCompressor called sleep.wakeup() ");
//  Serial.println("  InverterSystem::neededByCompressor is wakeing up system  ");
  if(sleep.getState() == SSS_ASLEEP)
	  sleep.wakeup(); //wake up system ****
  /* **** Changes made May 20, 2016 By t,z,k
   *  Whenever the compressor is needed while asleep, the system will wake up
   *  This will allow for the batteries to be linked
  */
} //end InverterSystem::neededByCompressor

/*----------------------------------- InverterSystem::neededByDumpValve-----------------------------------
 *  This is called by UISystem::enterState
 *  1. nbDumpValve is a boolean, and just sets the flag to true or false
*/
void InverterSystem::neededByDumpValve(boolean en) {
  nbDumpValve = en;
} //end InverterSystem::neededByDumpValve







