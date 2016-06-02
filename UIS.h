/*
 * UIS.h
 *
 *  Created on: May 30, 2016
 *      Author: Prospect
 */
#ifndef UISystem_h
#define UISystem_h

//------------------------------------------#INCLUDES------------------------------------------
#include "Arduino.h"
#include "Debug.h"
#include <LiquidCrystal.h>
#include "AS.h"
#include "CIS.h"
#include "HAL.h"
#include "ICS.h"
#include "MAS.h"
#include "SS.h"
#include "pinDefs.h"

//------------------------------------------#DEINFES------------------------------------------
#define MICROS_MAX 					4294967295

#define CHEVRON_LEFT   				-1			// display chevrons to left and right of current variable being modified
#define CHEVRON_RIGHT				4
#define BAD_VARNUM   				0xff

#define MB_DEBOUNCE_MICROS  		100000  	// MenuButton: debounce time in microseconds

#define UIS_TOWING                  0			// UISystem states
#define UIS_SCRUM_POWER             1
#define UIS_SCRUM_INDIVIDUAL        2
#define UIS_SCRUM_STRENGTH          3
#define UIS_SCRUM_STRENGTH_CHARGE   4
#define UIS_SCRUM_STRENGTH_POSTHIT	5

#define UIVM_POWER_THRESHOLD		0 			// modifiable display variables
#define UIVM_STRENGTH_THRESHOLD     1			//reads the max value that team can push before getting a penalty
#define UIVM_STRENGTH_DIFFICULTY    2
#define UIVM_STRENGTH_DURATION      3
#define UIVM__NUM                   4

#define UIVN_TOWING_RESPRESS        4  			// output-only (non-user-modifiable) display vars
#define UIVN__NUM                   5			// display reservoir pressure while towing

#define UISPH_TOO_HIGH  			1			// codes for goStrengthPosthit()
#define UISPH_TOO_FAST  			2
#define UISPH_TOO_MUCH  			3
#define UISPH_SUCCESS   			4

//------------------------------------------UI_CLASS------------------------------------------
class UISystem {
  public:
    UISystem(LiquidCrystal &lc);

    void loop();
    void heartbeat();

    // get/set a variable's value
    int     getVar(byte vn);
    boolean setVar(byte vn, int val);  // returns false if value wasn't changed

    byte getState();
    void enterState(byte newState);

    // enter UIS_SCRUM_STRENGTH_POSTHIT and display message
    void goStrengthPosthit(byte SPH_code, int val);

    //byte returnUIState();

  private:

    byte state;
    byte cur_var;  // current display variable being modified

    // button debouncing
    unsigned long lastMicros;
    class MenuButton &bMode, &bUp, &bDown, &bSet;

    LiquidCrystal  &lcd;

    // Info for every display variable
    byte cols[UIVN__NUM], rows[UIVN__NUM];   // display position
    byte wide[UIVN__NUM];                    // width (for padding)
    int  vals[UIVN__NUM];

    void setNonModVarInfo(byte vn, byte col, byte row, byte width);

    // Info for modifiable display variables
    int  mins[UIVM__NUM], maxs[UIVM__NUM];   // min/max limits

    void setModVarInfo(byte vn, byte col, byte row, byte width, int val, int min, int max);


    void showVar(byte vn);  // show var's value on the LCD

    void changeVar(byte newVar);  // select a new "current variable" on the LCD

    byte strengthPosthitCode;
    int  strengthPosthitValue;
};

#endif
