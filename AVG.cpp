///*
// * AVG.cpp
// *
// *  Created on: May 30, 2016
// *      Author: Prospect
// */
//#include "AVG.h"
//#include "TimerOne.h"
//#include "pinDefs.h"
//
////-----------------------------------AVREAG_CONSTRUCTOR-----------------------------------
//Averager::Averager() {
//  reset();//initialize everything to 0
//}
//
///* -----------------------------------Averager::update-----------------------------------
// *  Function called from UIS.cpp loop check for MODE/SET button pushes, Accustat::loop() for update on pressure readings for display
// *  #define AVG_NUM_READINGS  40
// *  readingIndex = 0;
// *  1.  This function takes the sum of multiple readings, and stores them in an arrary
//*/
//void Averager::update(int r) {
//
//  ave -= readings[readingIndex]/AVG_NUM_READINGS;
//  //readings[readingIndex] = r;
//  ave += analogRead(aiAchievedPin)/AVG_NUM_READINGS;
//
////  if (readingCount < AVG_NUM_READINGS)
////    readingCount++;
////reset reading index if greater or equal to 40
//  if (++readingIndex == AVG_NUM_READINGS)
//    readingIndex = 0;
//}// end Averager::update()
//
//
///* -----------------------------------Averager::reset()-----------------------------------
// *  This function is called during start up, and in Accustat::enterState
// *  1. resets the variables
//*/
//void Averager::reset() {
////  readingCount = 0;
//  readingIndex = 0;
////  for (byte i = 0; i < AVG_NUM_READINGS; i++)
////    readings[i] = 0;
//  ave = 0;
//} //end Averager::reset
//
//
///* -----------------------------------Averager::getAverage()-----------------------------------
// *  This function is called from Accustat::loop(), is used for the LED DISPLAY
// *  1. returns the sum of the reading divided by the number of reading taken
//*/
//int Averager::getAverage() {
////	Serial.print(" ave: "); Serial.println(ave);
//	//Serial.print(" readingcount "); Serial.println(readingCount);
//  return ave;
//}// end Averager::getAverage()
//
//
//
//
//
