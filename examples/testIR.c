//======================================================================
//
// Test program to test the infrared sensors (and motors) of the
// 4tronix initio robot car. One can run this program within an
// ssh session.
//
// author: Raimund Kirner, University of Hertfordshire
//         initial version: Oct.2016
//
// license: GNU LESSER GENERAL PUBLIC LICENSE
//          Version 2.1, February 1999
//          (for details see LICENSE file)
//
// Compilation: 
// gcc -o testIR -Wall -Werror -lcurses -lwiringPi -lpthread -linitio testIR.c
//
//======================================================================

#include <stdlib.h>
#include <initio.h>
#include <curses.h>

//======================================================================
// testIR():
// Simple program to test infrared obstacle sensors:
// Drive forward and stop whenever an obstacle is detected by either
// the left or right infrared (IR) sensor.
//======================================================================
void testIR(int argc, char *argv[]) 
{
  int ch = 0;

  while (ch != 'q') {
    mvprintw(1, 1,"%s: Press 'q' to end program", argv[0]);
    if (initio_IrLeft()==0 && initio_IrRight()==0) {
      initio_DriveForward (100); // Max speed
    }
    else {
      initio_DriveForward (0); // Stop
    } // if
    ch = getch();
    if (ch != ERR)
      mvprintw(2, 1,"Key code: '%c' (%d)", ch, ch);
    refresh();
    delay (100); // pause 100ms
  } // while

  return;
}


//======================================================================
// main(): initialisation of libraries, etc
//======================================================================
int main (int argc, char *argv[])
{
  WINDOW *mainwin = initscr();  // curses: init screen
  noecho ();                    // curses: prevent the key being echoed
  cbreak();                     // curses: disable line buffering 
  nodelay(mainwin, TRUE);       // curses: set getch() as non-blocking 
  keypad (mainwin, TRUE);       // curses: enable detection of cursor and other keys

  initio_Init (); // initio: init the library

  testIR(argc, argv);

  initio_Cleanup ();  // initio: cleanup the library (reset robot car)
  endwin();           // curses: cleanup the library
  return EXIT_SUCCESS;
}

