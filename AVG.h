/*
 * AVG.h
 *
 *  Created on: May 30, 2016
 *      Author: Prospect
 */
#ifndef AVG_h
#define AVG_h

//-----------------------------------#INCLUDES--------------------------------------
#include "Arduino.h"
#include "Debug.h"
#include "TimerOne.h"
#include "Timer.h"
#include "MAS.h"


//-----------------------------------#DEFINES--------------------------------------


//-----------------------------------AVERAGE_CLASS--------------------------------------
class Averager {
  public:
    Averager();

    void update(int r);

    void reset(void);

     int getAverage(void);



  private:
//    volatile byte  readingCount;
//    volatile byte  readingIndex;
//    volatile int   readings[AVG_NUM_READINGS];
//    volatile long  ave;
};

#endif
