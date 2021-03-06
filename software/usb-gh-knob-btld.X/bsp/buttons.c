/********************************************************************
 Software License Agreement:

 The software supplied herewith by Microchip Technology Incorporated
 (the "Company") for its PIC(R) Microcontroller is intended and
 supplied to you, the Company's customer, for use solely and
 exclusively on Microchip PIC Microcontroller products. The
 software is owned by the Company and/or its supplier, and is
 protected under applicable copyright laws. All rights are reserved.
 Any use in violation of the foregoing restrictions may subject the
 user to criminal sanctions under applicable laws, as well as to
 civil liability for the breach of the terms and conditions of this
 license.

 THIS SOFTWARE IS PROVIDED IN AN "AS IS" CONDITION. NO WARRANTIES,
 WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *******************************************************************/

#include <xc.h>
#include <stdbool.h>
#include <buttons.h>

/*** Button Definitions *********************************************/
#define S1_PORT  PORTBbits.RB6      // SW1
#define S2_PORT  PORTBbits.RB5      // CHA
#define S3_PORT  PORTBbits.RB4      // CHB

#define BUTTON_PRESSED      0
#define BUTTON_NOT_PRESSED  1

#define PIN_INPUT           1
#define PIN_OUTPUT          0

#define PIN_DIGITAL         1
#define PIN_ANALOG          0

static char state1, timer;
static char last_knob, this_knob;
static signed char value_knob;

/*********************************************************************
* Function: bool BUTTON_IsPressed(BUTTON button);
*
* Overview: Returns the current state of the requested button
*
* PreCondition: button configured via BUTTON_SetConfiguration()
*
* Input: BUTTON button - enumeration of the buttons available in
*        this demo.  They should be meaningful names and not the names 
*        of the buttons on the silkscreen on the board (as the demo 
*        code may be ported to other boards).
*         i.e. - ButtonIsPressed(BUTTON_SEND_MESSAGE);
*
* Output: TRUE if pressed; FALSE if not pressed.
*
********************************************************************/

void BUTTON_UpdateStates (void)
{
    // run this every 10-20ms to debounce and update button states for is pressed functions
    if (timer < 9) {
        timer++;
    } else {
        timer = 0;
        
        // button 1 debounce state machine
        switch (state1) {
            case 0: 
                state1 = (S1_PORT == BUTTON_PRESSED) ? 1 : 0;
                break;
            case 1: 
                state1 = (S1_PORT == BUTTON_PRESSED) ? 2 : 0;
                break;
            case 2: 
                state1 = (S1_PORT == BUTTON_PRESSED) ? 2 : 3;
                break;
            case 3: 
                state1 = (S1_PORT == BUTTON_PRESSED) ? 2 : 0;
                break;
        }
    }
    
     // this_knob = { CHB, CHA }
    this_knob = (S3_PORT << 1) | S2_PORT;

    switch (last_knob) {
        case 0: if (this_knob == 1) { value_knob--; } else if (this_knob == 2) { value_knob++; } break;
        case 1: if (this_knob == 0) { value_knob++; } else if (this_knob == 3) { value_knob--; } break;
        case 2: if (this_knob == 0) { value_knob--; } else if (this_knob == 3) { value_knob++; } break;
        case 3: if (this_knob == 1) { value_knob++; } else if (this_knob == 2) { value_knob--; } break;
    }

    last_knob = this_knob;
}


bool BUTTON_IsPressed(BUTTON button)
{
    switch(button)
    {
        case BUTTON_S1:
            return ((state1 >= 2) ? true : false);

        case BUTTON_S2:
            if (value_knob < 0) {
                value_knob++;
                return true;
            }
            return false;

        case BUTTON_S3:
            if (value_knob > 0) {
                value_knob--;
                return true;
            }
            return false;

        case BUTTON_NONE:
            return false;
    }
    
    return false;
}

/*********************************************************************
* Function: void BUTTON_Enable(BUTTON button);
*
* Overview: Returns the current state of the requested button
*
* PreCondition: button configured via BUTTON_SetConfiguration()
*
* Input: BUTTON button - enumeration of the buttons available in
*        this demo.  They should be meaningful names and not the names
*        of the buttons on the silkscreen on the board (as the demo
*        code may be ported to other boards).
*         i.e. - ButtonIsPressed(BUTTON_SEND_MESSAGE);
*
* Output: None
*
********************************************************************/
void BUTTON_Enable(BUTTON button)
{
    switch(button)
    {
        case BUTTON_S1:
            OPTION_REGbits.nWPUEN = 0;
            TRISBbits.TRISB6 = PIN_INPUT;
            WPUBbits.WPUB6 = 1;
            state1 = 0;
            break;

        case BUTTON_S2:
            TRISBbits.TRISB5 = PIN_INPUT;
            ANSELBbits.ANSB5 = 0;
            WPUBbits.WPUB5 = 1;
            break;

        case BUTTON_S3:
            TRISBbits.TRISB4 = PIN_INPUT;
            ANSELBbits.ANSB4 = 0;
            WPUBbits.WPUB4 = 1;
            break;

        case BUTTON_NONE:
            break;
    }
    
    last_knob = (S3_PORT << 1) | S2_PORT;
    value_knob = 0;
}
