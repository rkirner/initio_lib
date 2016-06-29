//======================================================================
//
// C library to externalise all Initio/PiRoCon specific hardware
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
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <time.h>

#include <wiringPi.h>
#include <softPwm.h>
#include "initio.h"

// When compiling you must include the libraries pthread, wiringPi:
// cc -o myprog myprog.c -lwiringPi -lpthread


//======================================================================
// Helper Functions
//
// MergeStrings(num_args, str1, ...). Helper function to merge strings
static char* MergeStrings(int num_args, char* str1, ...);
//======================================================================


// File pointer to Servo Demon interface (ServoBlaster)
static FILE *fpServoBlaster = NULL;


//======================================================================
// General Functions

// initio_init():
// Initialises GPIO pins, switches motors off, etc
void initio_Init()
{
    // Set GPIO bit numbering to use the physical pin numbers on the P1 connector only
    wiringPiSetupPhys () ;

    // set up digital wheel sensors as inputs
    pinMode (wheelLeft,  INPUT) ; // Left wheel sensor 1
    pinMode (wheelRight, INPUT) ; // Right wheel sensor 1

    // set up digital line detectors as inputs
    pinMode (lineRight, INPUT) ; // Right line sensor
    pinMode (lineLeft,  INPUT) ; // Left line sensor

    // Set up IR obstacle sensors as inputs
    pinMode (irFL, INPUT) ; // Left obstacle sensor
    pinMode (irFR, INPUT) ; // Right obstacle sensor

    // use pwm on inputs so motors don't go too fast
    softPwmCreate (L1, 0, 100) ;
    softPwmCreate (L2, 0, 100) ;
    softPwmCreate (R1, 0, 100) ;
    softPwmCreate (R2, 0, 100) ;

    // Initialise the servo background process
    initio_StartServos() ;
}

// initio_Cleanup():
// Sets all motors off, stops the PWM threads and servos demon,
// and sets GPIO to standard values.
void initio_Cleanup()
{
    int usedPins[13] = { L1, L2, R1, R2, wheelLeft, wheelRight, 
                         irFL, irFR, lineLeft, lineRight,
                         sonar, servoPan, servoTilt  };
    int pin;

    // Stop all motors
    initio_Stop () ;

    // Stop the PWM threads
    softPwmStop (L1) ;
    softPwmStop (L2) ;
    softPwmStop (R1) ;
    softPwmStop (R2) ;

    // Stop the sevos demon
    initio_StopServos () ;

    // Set GPIO to standard values (Input, no Pull-Up/Down)
    for (pin = sizeof(usedPins)/sizeof(int)-1; pin >= 0; pin--)
    {
        pullUpDnControl (usedPins[pin], PUD_OFF);
        pinMode (usedPins[pin], INPUT) ;
    }
}

// initio_Version():
// Returns current version (float).
float initio_Version()
{
    return 0.1;
}
//======================================================================
// General Functions



//======================================================================
// Motor Functions

/*** Python PWM: p=L1, q=L2, a=R1, b=R2 ***/

// initio_Stop ():
// Stops both motors
void initio_Stop ()
{
    softPwmWrite (L1, 0) ;
    softPwmWrite (L2, 0) ;
    softPwmWrite (R1, 0) ;
    softPwmWrite (R2, 0) ;
}

// initio_DriveForward (speed):
// Sets both motors to move forward at speed. 0 <= speed <= 100
void initio_DriveForward (int8_t speed)
{
    softPwmWrite (L1, speed) ;
    softPwmWrite (L2, 0) ;
    softPwmWrite (R1, speed) ;
    softPwmWrite (R2, 0) ;
}

// initio_DriveReverse (speed):
// Sets both motors to reverse at speed. 0 <= speed <= 100
void initio_DriveReverse (int8_t speed)
{
    softPwmWrite (L1, 0) ;
    softPwmWrite (L2, speed) ;
    softPwmWrite (R1, 0) ;
    softPwmWrite (R2, speed) ;
}

// initio_SpinLeft (speed):
// Sets motors to turn opposite directions at speed. 0 <= speed <= 100
void initio_SpinLeft (int8_t speed)
{
    softPwmWrite (L1, 0) ;
    softPwmWrite (L2, speed) ;
    softPwmWrite (R1, speed) ;
    softPwmWrite (R2, 0) ;
}

// initio_SpinRight(speed):
// Sets motors to turn opposite directions at speed. 0 <= speed <= 100
void initio_SpinRight(int8_t speed)
{
    softPwmWrite (L1, speed) ;
    softPwmWrite (L2, 0) ;
    softPwmWrite (R1, 0) ;
    softPwmWrite (R2, speed) ;
}

// initio_TurnForward (leftSpeed, rightSpeed):
// Moves forwards in an arc by setting different speeds. 0 <= leftSpeed,rightSpeed <= 100
void initio_TurnForward (int8_t leftSpeed, int8_t rightSpeed)
{
    softPwmWrite (L1, leftSpeed) ;
    softPwmWrite (L2, 0) ;
    softPwmWrite (R1, rightSpeed) ;
    softPwmWrite (R2, 0) ;
}

// initio_TurnReverse (leftSpeed, rightSpeed):
// Moves backwards in an arc by setting different speeds. 0 <= leftSpeed,rightSpeed <= 100
void initio_TurnReverse (int8_t leftSpeed, int8_t rightSpeed)
{
    softPwmWrite (L1, 0) ;
    softPwmWrite (L2, leftSpeed) ;
    softPwmWrite (R1, 0) ;
    softPwmWrite (R2, rightSpeed) ;
}

// End of Motor Functions
//======================================================================



//======================================================================
// Wheel Sensor Functions
// Note that the wheel sensor functions only indicate movement of the wheel but
// bot exact position indication. This is because the implementation assumes
// only one of the two phase-shifted signals per wheel to be connected.

// initio_wheelSensorLeft ():
// Returns the status of the left wheel position sensor connected to pin(wheelLeft).
BOOL initio_wheelSensorLeft (void)
{
    return (digitalRead (wheelLeft)) ;
}

// initio_wheelSensorRight ():
// Returns the status of the right wheel position sensor connected to pin(wheelRight).
BOOL initio_wheelSensorRight (void)
{
    return (digitalRead (wheelLeft)) ;
}

// End of Wheel Sensor Functions
//======================================================================



//======================================================================
// IR Sensor Functions

// initio_IrLeft():
// Returns whether Left IR Obstacle sensor is triggered
BOOL initio_IrLeft (void)
{
    return (digitalRead (irFL) == 0) ;
}

// initio_IrRight():
// Returns whether Right IR Obstacle sensor is triggered
BOOL initio_IrRight (void)
{
    return (digitalRead (irFR) == 0) ;
}

// initio_IrAll():
// Returns TRUE if at least one of the Obstacle sensors is triggered
BOOL initio_IrAll (void)
{
    return ((digitalRead (irFL) == 0) || (digitalRead (irFR) == 0)) ;
}

// initio_IrLineLeft():
// Returns whether Left IR Line sensor is triggered
BOOL initio_IrLineLeft (void)
{
    return (digitalRead (lineLeft) == 0) ;
}

// initio_IrLineRight():
// Returns whether Right IR Line sensor is triggered
BOOL initio_IrLineRight (void)
{
    return (digitalRead (lineRight) == 0) ;
}

// End of IR Sensor Functions
//======================================================================



//======================================================================
// UltraSonic Functions

// initio_UsGetDistance():
// Returns the distance in cm to the nearest reflecting object. 0 == no object
unsigned int initio_UsGetDistance (void)
{
    unsigned int start, count, stop, elapsed, distance;

    pinMode (sonar, OUTPUT) ; // set sonar as output
    // Send 10us pulse to trigger
    digitalWrite (sonar, TRUE) ;
    delayMicroseconds (10) ;
    digitalWrite (sonar, FALSE) ;
    start =  millis () ;
    count =  millis () ;
    pinMode (sonar, INPUT) ; // set sonar as input
    while ((digitalRead (sonar) == 0) && ((millis() - count) < 100))
        start = millis () ;
    stop = count;
    while ((digitalRead (sonar) == 1) && ((millis() - count) < 100))
        stop = millis () ;
    elapsed = stop - start ; // Calculate pulse length
    // Distance pulse travelled in that time is time
    // multiplied by the speed of sound (cm/s)
    distance = elapsed * 34 ;
    // That was the distance there and back so halve the value
    distance = distance / 2 ;
    return distance ;
}

// End of UltraSonic Functions
//======================================================================



//======================================================================
// Servo Functions

// initio_StartServos ():
// Initialises the servo background process
void initio_StartServos (void)
{
    char *pstrServoPrg = NULL;
    char *pstrInitCmd = NULL;

    fprintf (stdout, "Starting servod\n") ;
    // TODO: check for secure_getenv, http://www.gnu.org/software/libc/manual/html_node/Environment-Access.html
    fprintf(stdout, "Starting servod. ServosActive: %s\n", (fpServoBlaster!=NULL) ? "TRUE" : "FALSE") ;
    initio_StopServos () ; // make sure no previous instance of 'servod' is running
    pstrServoPrg = getenv("SERVOD") ; // Try to find 'servod' via environment variable
    if (pstrServoPrg == NULL) {
        pstrServoPrg = "servod";
    }
    // start ServerBlaster demon servod, options: Idle-Timeout = 20s, Port1-Pins = 18,22
    pstrInitCmd = MergeStrings (3, "sudo ", pstrServoPrg, " --pcm --idle-timeout=20000 --p1pins=\"18,22\" > /dev/null\n") ;
    system(pstrInitCmd) ;
    free (pstrInitCmd) ; // free mem allocated by MergeString
    // open interface of servo demon
    fpServoBlaster = fopen("/dev/servoblaster","w") ;
    if (fpServoBlaster == NULL) {
        fprintf(stderr,"Opening /dev/servoblaster failed \n") ;
        exit(EXIT_FAILURE) ;
    } // endif
}

// initio_stopServos ():
// Terminates the servo background process
void initio_StopServos (void)
{
    fprintf(stdout,"Stopping servo\n") ;
    system("sudo pkill -f servod") ;
    if (fpServoBlaster != NULL) {
        fclose (fpServoBlaster) ;
        fpServoBlaster = NULL;
    } // endif
}


// initio_SetServo (servo, degrees):
// Sets the servo to position in degrees -90 to +90
void initio_SetServo (int8_t servo, int8_t degrees)
{
    //fprintf (stdout, "ServosActive: %s\n", (fpServoBlaster!=NULL) ? "TRUE" : "FALSE") ;
    //fprintf(stdout,"pin = %d, Setting servo: degrees = %d\n", pin, degrees) ;
    if (fpServoBlaster == NULL)
         initio_StartServos() ;  // start servo demon if not already running
    // Write <pin> = <servo-position> to /dev/servoblaster.
    // By default <servo-position> is the pulse width in units of 10us
    fprintf (fpServoBlaster, "%d=%d\n", servo, 50 + ((90 - degrees) * 200 / 180) ) ;
    fflush (fpServoBlaster) ;
}

// End of Servo Functions
//======================================================================



//======================================================================
// Helper Functions

/*
 MergeStrings(num_args, str1, ...):
 Function that takes the count of strings to be merged and the variable
 number of strings as input, allocates memory of sufficient size and
 merges all input strings into that allocated memory.
 The return value is the pointer of the new created merged string.  If
 the first argument is just a NULL pointer, then NULL is returned.
 */
static char* MergeStrings(int num_args, char* str1, ...)
{
   va_list ap;
   char *p, *pins;
   int len = 0;
   int i;
   char *smerged = NULL;

   len = strlen(str1) ;
   va_start(ap, str1) ;
   for(i = num_args - 1; i > 0; i--)
   {
      p = va_arg(ap, char*) ;
      len = len + strlen(p) ;
   }
   va_end(ap) ;

   if (len > 0)
   {
      len = len + 1 ;  // allocate extra space for string termination
      smerged = malloc (len) ;

      if (smerged==NULL) return NULL ;  // out-of-memory?
      smerged[0] = '\0';  // init as empty string

      pins = smerged;  // points to first empty space in array
      strcpy(pins, str1) ;
      pins = pins + strlen(str1) ;
      va_start(ap, str1) ;
      for(i = num_args -1; i > 0; i--)
      {
         p = va_arg(ap, char*) ;
         strcpy(pins, p) ;
         pins = pins + strlen(p) ;
      }
      va_end(ap) ;
      *pins = '\0' ;  // terminate the merged string
   }

   return smerged ;  // return allocated memory (needs explicit free() later)
}
// End of Helper Functions
//======================================================================


