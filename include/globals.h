#ifndef GLOBALS_H
#define GLOBALS_H

//All libaries
#include <allegro.h>

//Game states
enum GameStates
{
    STATE_NULL,
    STATE_INIT,
    STATE_MENU,
    STATE_JOYSTICK,
    STATE_EXIT,
};

// Frames
extern int frames_done;

//State variables
extern int stateID;
extern int nextState;

// Font
extern FONT *segoe;

#endif
