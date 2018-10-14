//======================================================================
//
// Test program to set the value of Pin 37 in OUTPUT mode of the
// 4tronix initio robot car. One can run this program within an
// ssh session.
//
// author: Raimund Kirner, University of Hertfordshire
//         initial version: Sep.2018
//
// license: GNU LESSER GENERAL PUBLIC LICENSE
//          Version 2.1, February 1999
//          (for details see LICENSE file)
//
// Compilation: 
// gcc -o testIO -Wall -Werror -lcurses -lwiringPi -lpthread -linitio testIO.c
//
//======================================================================

#include <stdlib.h>
#include <initio.h>
#include <curses.h>

//======================================================================
// testIO():
// Simple program to set the output value of Pin 37.
//======================================================================
void testIO(int argc, char *argv[]) 
{
  int ch = 0;
  int statusP37 = 0;   // status flag for Pin 37

  pinMode (37, OUTPUT) ; // set Pin 37 to output mode

  while (ch != 'q') {
    mvprintw(1, 1,"%s:  Simple IO pin test program", argv[0]);
    mvprintw(2, 1,"  'q'.. exit, 'a'.. toggle PIN37 On/Off");
    mvprintw(3, 1,"status PIN 37: %s ", (statusP37 == 0) ? "off" : "on");

    ch = getch();
    switch ( ch ) {
    case 'a':
      statusP37 = ! statusP37;
      break;
    default:
      if (ch != ERR)
        mvprintw(4, 1, "Key code: '%c' (%d) \"%s\"", ch, ch, keyname(ch) );
    }
    refresh();

    digitalWrite (37, statusP37);  // set Pin 37 to current value
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

  testIO(argc, argv);

  initio_Cleanup ();  // initio: cleanup the library (reset robot car)
  endwin();           // curses: cleanup the library
  return EXIT_SUCCESS;
}

