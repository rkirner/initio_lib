#ifndef _4TRONIX_INITIO_H_
#define _4TRONIX_INITIO_H_
//======================================================================
//
// C library to externalise all Initio/PiRoCon specific hardware
// author: Raimund Kirner, University of Hertfordshire
//         initial version: Jun.2016 (support only for PiRoCon 2.0 board)
//         updated version: Aug.2016 (added support for RoboHAT 1.0 board)
//
// license: GNU LESSER GENERAL PUBLIC LICENSE
//          Version 2.1, February 1999
//          (for details see LICENSE file)
//
//======================================================================

#include <wiringPi.h>
#include <softPwm.h>
#include <stdint.h> // Needed for int8_t

// When compiling you must include the followinglibraries: pthread, wiringPi:
// cc -o myprog myprog.c -lwiringPi -lpthread

// All pin numbers refer to the physical pin numbers on the P1 connector

// Define a boolean data type
#ifndef BOOL
#define BOOL int8_t
#endif 
#ifndef TRUE
#define TRUE (1==1)
#endif 
#ifndef FALSE
#define FALSE (0==1)
#endif 

// Type of connected robot control board
#define UNKNOWN_HAT 0 // unknown HAT board connected
#define PIROCON2    1 // assumed PiRoCon board 2.0 (no HAT connected)
#define ROBOHAT     2 // RoboHAT board connected (uses different pins for: motors, ultrasonic)

// Define pins for platform motors (left motor: pin 19+21; right motor: pin 24+26)
#define L1_PiRoCon 19
#define L2_PiRoCon 21
#define R1_PiRoCon 24
#define R2_PiRoCon 26
#define L1_RoboHAT 36
#define L2_RoboHAT 35
#define R1_RoboHAT 33
#define R2_RoboHAT 32

#define wheelLeft  15 // Left  Wheel sensor 1 (Optional)
#define wheelRight 16 // Right Wheel sensor 1 (Optional)

// Define pins for obstacle sensors and line sensors
#define irFL 7
#define irFR 11
#define lineLeft_PiRoCon 12
#define lineLeft_RoboHat 29
#define lineRight 13

// Define Sonar Pin (same pin for both Ping and Echo)
// Note, that this can be either 8 or 23 on PiRoCon
#define sonar_PiRoCon  8 // pin on PiRoCon board
#define sonar_RoboHAT 38 // pin on RoboHAT board

// Define logical Servo pins for Pan/Tilt
// (they are mapped to Port 1 pins 18 and 22)
#define servoPan 0
#define servoTilt 1
// Define Servo pins for Pan/Tilt
#define servoPanPin 18     
#define servoTiltPin 22


//======================================================================
// General Functions

// initio_identifyControlBoard():
// Returns the type of the connected robot control board used for sensors/actuators
int initio_identifyControlBoard() ;

// initio_Init():
// Initialises GPIO pins, set physical pin numbering, switches motors off, etc
void initio_Init() ;

// initio_Cleanup():
// Sets all motors off and sets GPIO pins to standard values
void initio_Cleanup() ;

// initio_Version():
// Returns current version (decimal value).
float initio_Version() ;

// General Functions
//======================================================================



//======================================================================
// Motor Functions

// initio_Stop ():
// Stops both motors
void initio_Stop () ;

// initio_DriveForward (speed):
// Sets both motors to move forward at speed. 0 <= speed <= 100
void initio_DriveForward (int8_t speed) ;

// initio_DriveReverse (speed):
// Sets both motors to reverse at speed. 0 <= speed <= 100
void initio_DriveReverse (int8_t speed) ;

// initio_SpinLeft (speed):
// Sets motors to turn opposite directions at speed. 0 <= speed <= 100
void initio_SpinLeft (int8_t speed) ;

// initio_SpinRight(speed):
// Sets motors to turn opposite directions at speed. 0 <= speed <= 100
void initio_SpinRight(int8_t speed) ;

// initio_TurnForward (leftSpeed, rightSpeed):
// Moves forwards in an arc by setting different speeds. 0 <= leftSpeed,rightSpeed <= 100
void initio_TurnForward (int8_t leftSpeed, int8_t rightSpeed) ;

// initio_TurnReverse (leftSpeed, rightSpeed):
// Moves backwards in an arc by setting different speeds. 0 <= leftSpeed,rightSpeed <= 100
void initio_TurnReverse (int8_t leftSpeed, int8_t rightSpeed) ;

// End of Motor Functions
//======================================================================



//======================================================================
// Wheel Sensor Functions
// Note that the wheel sensor functions only indicate movement of the wheel but
// bot exact position indication. This is because the implementation assumes 
// only one of the two phase-shifted signals per wheel to be connected.

// initio_wheelSensorLeft ():
// Returns the status of the left wheel position sensor connected to pin(wheelLeft).
BOOL initio_wheelSensorLeft (void) ;

// initio_wheelSensorRight ():
// Returns the status of the right wheel position sensor connected to pin(wheelRight).
BOOL initio_wheelSensorRight (void) ;

// End of Wheel Sensor Functions
//======================================================================



//======================================================================
// IR Sensor Functions

// initio_IrLeft():
// Returns whether Left IR Obstacle sensor is triggered
BOOL initio_IrLeft (void) ;

// initio_IrRight():
// Returns whether Right IR Obstacle sensor is triggered
BOOL initio_IrRight (void) ;

// initio_IrAll():
// Returns TRUE if at least one of the Obstacle sensors is triggered
BOOL initio_IrAll (void) ;

// initio_IrLineLeft():
// Returns whether Left IR Line sensor is triggered
BOOL initio_IrLineLeft (void) ;

// initio_IrLineRight():
// Returns whether Right IR Line sensor is triggered
BOOL initio_IrLineRight (void) ;

// End of IR Sensor Functions
//======================================================================



//======================================================================
// UltraSonic Functions

// initio_UsGetDistance():
// Returns the distance in cm to the nearest reflecting object. 0 == no object
unsigned int initio_UsGetDistance (void) ;

// End of UltraSonic Functions
//======================================================================



//======================================================================
// Servo Functions

// initio_StartServos ():
// Initialises the servo background process
void initio_StartServos (void) ;

// initio_StopServos ():
// Terminates the servo background process
void initio_StopServos (void) ;

// initio_SetServo (servo, degrees):
// Sets the servo to position in degrees -90 to +90
void initio_SetServo (int8_t servo, int8_t degrees) ;

// End of Servo Functions
//======================================================================



#endif /* _4TRONIX_INITIO_H_ */
