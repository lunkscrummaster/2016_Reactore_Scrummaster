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


//-----------------------------------#DEFINES--------------------------------------
#define AVG_NUM_READINGS  5


//-----------------------------------AVERAGE_CLASS--------------------------------------
class Averager {
  public:
    Averager();

    void update(int r);

    void reset(void);

    int getAverage(void);



  private:
    byte  readingCount;
    byte  readingIndex;
    int   readings[AVG_NUM_READINGS];
    long  sum;
};

#endif
