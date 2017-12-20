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

#define POSXS  10 // column and line of extra status print
#define POSYS  11 

#define ADD_MOTOR_SPEED(var, val) (MAX(0 , MIN( 100, (var)+(val) )))  // limit virtual motor speed to 0/100
#define ADD_SERVO_TILT(var, val) (MAX(-80 , MIN( 80, (var)+(val) )))  // limit horizontal movement: -80/80 to avoid non-resting motors
#define ADD_SERVO_PAN(var, val)  (MAX(-40 , MIN( 80, (var)+(val) )))  // limit vertical movement: -40 to fit size of ultrasoncic sensor, 80 to avoid non-resting motors

int main (int argc, char **argv)
{
    enum driveMode { stop=0, forward, reverse, spinleft, spinright } driveMode = stop;
    const char *driveModeNames[5] = {"Stop", "Forward", "Reverse", "SpinLeft", "SpinRight" };
    int ch = 0, pos;
    int speed = 30, posTilt = 0, posPan = 0;
    const unsigned int delayMS = 20; // delay in ms between updates on certain controls, e.g., acceleration
    const unsigned int delayGestureUS = 500000; // delay in us for yes/no gestures
    unsigned int timeCurrent, timeNext = 0;
    unsigned int distance;
    BOOL bIrLeft=FALSE, bIrRight=FALSE, bLineLeft=FALSE, bLineRight=FALSE;
    BOOL bWheelLeft, bWheelRight;

    WINDOW *mainwin = initscr ();   // curses: init screen
    noecho ();                      // curses: prevent the key being echoed
    cbreak ();                      // curses: disable line buffering
    nodelay (mainwin, TRUE);        // curses: set getch() as non-blocking
    keypad (mainwin, TRUE);         // curses: enable the cursor and other keys to be detected

    initio_Init(); // initio: init the library

    void (*pMotionFunc)(int8_t) = initio_DriveForward;

    mvprintw(1, 1, "motorControl: q..quit, cursor..steer, b..reverse, space/cursor-down..stop, shift-cursor/aswd..servo, r..centre servo, y/n..say yes/no");
    while (ch != KEY_ESCAPE && ch !='q') {
        bIrLeft =  initio_IrLeft () ;
        bIrRight = initio_IrRight () ;
        bLineLeft =  initio_IrLineLeft () ;
        bLineRight = initio_IrLineRight () ;
        distance = initio_UsGetDistance () ;
        bWheelLeft =  initio_wheelSensorLeft () ;
        bWheelRight = initio_wheelSensorRight () ;
        mvprintw(5,10, "IrLeft:    %c (%d)",  bIrLeft  ? 'T' : '_', bIrLeft) ;
        mvprintw(5,35, "IrRight:    %c (%d)", bIrRight ? 'T' : '_', bIrRight) ;
        mvprintw(6,10, "LineLeft:  %c (%d)",  bLineLeft  ? 'T' : '_', bLineLeft) ;
        mvprintw(6,35, "LineRight : %c (%d)", bLineRight ? 'T' : '_', bLineRight) ;
        mvprintw(7,10, "Distance:  %d cm  ", distance) ;
        mvprintw(8,10, "WheelLeft: %d",  bWheelLeft) ;
        mvprintw(8,35, "WheelRight: %d", bWheelRight) ;
        mvprintw(9,10, "DriveMode: %s    ", driveModeNames[driveMode]) ;
        mvprintw(9,35, "PWM:        %d  ", speed);
        mvprintw(POSYS, POSXS, "");
        timeCurrent = millis () ;
        ch = getch() ;
        deleteln();
        switch ( ch ) {
        // Motor control keys
        case KEY_LEFT:
            if (driveMode == stop ||  driveMode == spinleft) {
                speed = ADD_MOTOR_SPEED (speed, 10) ;
            } else if (driveMode == spinright) {
                speed = 0 ;
            } // endif
            driveMode = spinleft;
            pMotionFunc = initio_SpinLeft ;
            (*pMotionFunc) (speed) ;
	    mvprintw(POSYS, POSXS, "%s %d", driveModeNames[driveMode], speed) ;
            break;
        case KEY_RIGHT:
            if (driveMode == spinright || driveMode == stop) {
                speed = ADD_MOTOR_SPEED (speed, 10) ;
            } else if (driveMode == spinleft) {
                speed = 0 ;
            } ; // endif
            driveMode = spinright;
            pMotionFunc = initio_SpinRight ;
            (*pMotionFunc) (speed) ;
	    mvprintw(POSYS, POSXS, "%s %d", driveModeNames[driveMode], speed) ;
            break;
        case KEY_UP:
            switch (driveMode) {
            case forward:
                 if (timeCurrent < timeNext) break; // throttle acceleration
                 timeNext = timeCurrent + delayMS; 
            case stop:
                 driveMode = forward ;
                 pMotionFunc = initio_DriveForward ;
                 speed = ADD_MOTOR_SPEED (speed, 10) ;
                 break;
            case reverse:
                 speed = ADD_MOTOR_SPEED (speed, -10) ;
                 if (speed == 0) {
                     driveMode = stop ;
                 } // endif
                 break ;
            default:
                 driveMode = forward ;
                 pMotionFunc = initio_DriveForward ;
            } // switch
            (*pMotionFunc) (speed) ;
	    mvprintw(POSYS, POSXS, "%s %d", driveModeNames[driveMode], speed) ;
            break;
        case KEY_DOWN:
            speed = 0 ;
            (*pMotionFunc) (speed) ;
	    mvprintw(POSYS, POSXS, "%s %d", driveModeNames[driveMode], speed) ;
            break;
        case 'b':
            switch (driveMode) {
            case reverse:
                 //if (timeCurrent < timeNext) break; // throttle acceleration
                 timeNext = timeCurrent + delayMS; 
            case stop:
                 driveMode = reverse ;
                 pMotionFunc = initio_DriveReverse ;
                 speed = ADD_MOTOR_SPEED (speed, 10) ;
                 break;
            case forward:
                 speed = ADD_MOTOR_SPEED (speed, -10) ;
                 if (speed == 0) {
                     driveMode = stop ;
                 } // endif
                 break ;
            default:
                 driveMode = reverse ;
                 pMotionFunc = initio_DriveReverse ;
            } // switch
            (*pMotionFunc) (speed) ;
	    mvprintw(POSYS, POSXS, "%s %d", driveModeNames[driveMode], speed) ;
            break;
        case ' ':
            driveMode = stop ;
            speed = 0;
            initio_Stop () ;
	    mvprintw(POSYS, POSXS, "%s %d", driveModeNames[driveMode], speed) ;
            break;
        // Servo control keys
        case 'r':
            posTilt = 0;
            posPan = 0;
            initio_SetServo (servoTilt, posTilt) ;
            initio_SetServo (servoPan, posPan) ;
	    mvprintw(POSYS, POSXS, "Servo Reset %d,%d", posTilt, posPan);
            break;
        case KEY_SLEFT:
        case 'a':
            posTilt = ADD_SERVO_TILT (posTilt, -10);
            initio_SetServo (servoTilt, posTilt) ;
	    mvprintw(POSYS, POSXS, "Servo Left %d", posTilt);
            break;
        case KEY_SRIGHT:
        case 'd':
            posTilt = ADD_SERVO_TILT (posTilt, 10);
            initio_SetServo (servoTilt, posTilt) ;
	    mvprintw(POSYS, POSXS, "Servo Right %d", posTilt);
            break;
        case KEY_SUP:
        case 'w':
            posPan = ADD_SERVO_PAN (posPan, 10);
            initio_SetServo (servoPan, posPan) ;
	    mvprintw(POSYS, POSXS, "Servo Up %d", posPan);
            break;
        case KEY_SDOWN:
        case 's':
            posPan = ADD_SERVO_PAN (posPan, -10);
            initio_SetServo (servoPan, posPan) ;
	    mvprintw(POSYS, POSXS, "Servo Down %d", posPan);
            break;
        case 'y': // fun: display 'head nodding' movement
            pos = posPan ;
            posPan = ADD_SERVO_PAN(pos, 40) ; initio_SetServo (servoPan, posPan) ;
            initio_SetServo (servoPan, posPan) ;
            delayMicroseconds (delayGestureUS) ;
            posPan = ADD_SERVO_PAN(pos, -40) ; initio_SetServo (servoPan, posPan) ;
            delayMicroseconds (delayGestureUS) ;
            posPan = pos ; initio_SetServo (servoPan, posPan) ;
            break;
        case 'n': // fun: display 'head shake' movement
            pos = posTilt ;
            posTilt = ADD_SERVO_TILT(pos, 40) ; initio_SetServo (servoTilt, posTilt) ;
            delayMicroseconds (delayGestureUS) ;
            posTilt = ADD_SERVO_TILT(pos, -40) ; initio_SetServo (servoTilt, posTilt) ;
            delayMicroseconds (delayGestureUS) ;
            posTilt = pos; initio_SetServo (servoTilt, posTilt) ;
            break;
        default:
            if (ch != ERR)
                mvprintw(POSYS, POSXS, "Key code: '%c' (%d) \"%s\"", ch, ch, keyname(ch) );
        } // switch
	refresh(); // update screen
    } // endwhile
    // resource cleanup for ncurses
    delwin(mainwin) ; 
    endwin() ;
    refresh() ;
    initio_Cleanup() ;
    return (EXIT_SUCCESS) ;
}

