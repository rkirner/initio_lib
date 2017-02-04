//======================================================================
//
// Test program to test the motor & servo control and sensor readings
// of the 4tronix initio robot car. One can run this program within an
// ssh session and control the car via the remote computer's keyboard.
//
// author: Raimund Kirner, University of Hertfordshire
//         initial version: Jun.2016
//
// license: GNU LESSER GENERAL PUBLIC LICENSE
//          Version 2.1, February 1999
//          (for details see LICENSE file)
//
//======================================================================

#include <stdio.h>
#include <stdlib.h>
#include <sys/param.h>
#include <curses.h>

/* uncomment the following headers, if needed for extensions of this program */
//#include <wiringPi.h>
//#include <softPwm.h>
#include "initio.h"

#define KEY_ESCAPE 27
#define KEY_SDOWN 336
#define KEY_SUP 337

#define POSXS  10
#define POSYS  10

int main (int argc, char **argv)
{
    int ch = 0;
    int speed = 30, posTilt = 0, posPan = 0;
    unsigned int distance;
    WINDOW *mainwin;
    BOOL bIrLeft=FALSE, bIrRight=FALSE, bLineLeft=FALSE, bLineRight=FALSE;
    BOOL bBrake=FALSE;
    BOOL bWheelLeft, bWheelRight;

    initio_Init() ;
    mainwin = initscr () ;
    noecho () ; // prevent the key being echoed
    keypad (mainwin, TRUE) ; // enable the cursor and other keys to be detected
    void (*pMotionFunc)(int8_t) = initio_DriveForward;

    mvprintw(1, 1, "motorControl: q..quit, cursor..steer, a/z..speed, space..stop, shift-cursor..servo, b..centre servo, s..measure");
    while (ch != KEY_ESCAPE && ch !='q') {
        bIrLeft =     initio_IrLeft () ;
        bIrRight =    initio_IrRight () ;
        bLineLeft =   initio_IrLineLeft () ;
        bLineRight =  initio_IrLineRight () ;
        distance =    initio_UsGetDistance () ;
        bWheelLeft =  initio_wheelSensorLeft () ;
        bWheelRight = initio_wheelSensorRight () ;
        mvprintw(5,10, "IrLeft:   %c (%d)",  bIrLeft    ? 'T' : '_', bIrLeft    ) ;
        mvprintw(5,30, "IrRight:   %c (%d)", bIrRight   ? 'T' : '_', bIrRight   ) ;
        mvprintw(6,10, "LineLeft: %c (%d)",  bLineLeft  ? 'T' : '_', bLineLeft  ) ;
        mvprintw(6,30, "LineRight: %c (%d)", bLineRight ? 'T' : '_', bLineRight ) ;
        mvprintw(7,10, "Distance: %d cm  ",  distance    ) ;
        mvprintw(8,10, "WheelLeft: %d",      bWheelLeft  ) ;
        mvprintw(8,30, "WheelRight: %d",     bWheelRight ) ;
        mvprintw(POSYS, POSXS, "");
        ch = getch() ;
        deleteln();
        switch ( ch ) {
        // Motor control keys
        case 'a':
            bBrake = FALSE ;
            speed = MIN(100,speed+10);
            initio_DriveForward (speed);
	    mvprintw(POSYS, POSXS, "Speed+ %d", speed);
            break;
        case 'z':
            bBrake = FALSE ;
            speed = MAX(0,speed-10);
            initio_DriveForward (speed);
	    mvprintw(POSYS, POSXS, "Speed- %d", speed);
            break;
        case KEY_LEFT:
            bBrake = FALSE ;
            initio_SpinLeft (speed);
	    mvprintw(POSYS, POSXS, "Spin Left %d", speed);
            break;
        case KEY_RIGHT:
            bBrake = FALSE ;
            initio_SpinRight (speed);
	    mvprintw(POSYS, POSXS, "Spin Right %d", speed);
            break;
        case KEY_UP:
            bBrake = FALSE ;
            initio_DriveForward (speed);
	    mvprintw(POSYS, POSXS, "Forward %d", speed);
            break;
        case KEY_DOWN:
            bBrake = FALSE ;
            initio_DriveReverse (speed);
	    mvprintw(POSYS, POSXS, "Reverse %d", speed);
            break;
        case ' ':
            bBrake = ! bBrake ;
            if (bBrake == TRUE) {
                initio_DriveForward (0) ;
	        mvprintw(POSYS, POSXS, "Brake On %d", 0);
            }
            else {
                (*pMotionFunc) (speed) ;
	        mvprintw(POSYS, POSXS, "Brake Released %d", speed);
            }
            break;
        // Servo control keys
        case 'b':
            posTilt = 0;
            posPan = 0;
            initio_SetServo (servoTilt, posTilt) ;
            initio_SetServo (servoPan, posPan) ;
	    mvprintw(POSYS, POSXS, "Servo Reset %d,%d", posTilt, posPan);
            break;
        case KEY_SLEFT:
            posTilt = MAX(-80, posTilt-10) ;
            initio_SetServo (servoTilt, posTilt) ;
	    mvprintw(POSYS, POSXS, "Servo Left %d", posTilt);
            break;
        case KEY_SRIGHT:
            posTilt = MIN(80, posTilt+10) ;
            initio_SetServo (servoTilt, posTilt) ;
	    mvprintw(POSYS, POSXS, "Servo Right %d", posTilt);
            break;
        case KEY_SUP:
            posPan = MIN(80, posPan+10) ; // limit vertical movement to 90 to avoid non-resting motors
            initio_SetServo (servoPan, posPan) ;
	    mvprintw(POSYS, POSXS, "Servo Up %d", posPan);
            break;
        case KEY_SDOWN:
            posPan = MAX(-40, posPan-10) ; // limit vertical movement to -40 due to size of ultrasonic sensor
            initio_SetServo (servoPan, posPan) ;
	    mvprintw(POSYS, POSXS, "Servo Down %d", posPan);
            break;
        // Sensor updates
        case 's':
            break;
        default:
	    mvprintw(POSYS, POSXS, "You pressed: 0x%x (%d)", ch, ch);
        }
	refresh(); 
    } // while

    delwin(mainwin);
    endwin();
    refresh();
    initio_Cleanup() ;
    return (EXIT_SUCCESS) ;
}

