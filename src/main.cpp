#include <allegro.h>
#include <alpng.h>

#include "globals.h"
#include "tools.h"

#include "GameState.h"

#include "Init.h"
#include "Menu.h"
#include "JoystickMenu.h"

// Close button
volatile int close_button_pressed = FALSE;
bool closeGame;

// FPS System
volatile int ticks = 0;
const int updates_per_second = 60;
volatile int game_time = 0;

int fps;
int old_time;

void ticker(){
  ticks++;
}
END_OF_FUNCTION(ticker)

void game_time_ticker(){
  game_time++;
}
END_OF_FUNCTION(ticker)

void close_button_handler(void){
  close_button_pressed = TRUE;
}
END_OF_FUNCTION(close_button_handler)

//Game state object
GameState *currentState = NULL;

// Change game screen
void change_state()
{
  //If the state needs to be changed
  if( nextState != STATE_NULL ){
    //Delete the current state
    if( nextState != STATE_EXIT ){
        delete currentState;
    }

    //Change the state
    switch( nextState )
    {
      case STATE_INIT:
        currentState = new Init();
        break;

      case STATE_MENU:
        currentState = new Menu();
        break;

      case STATE_JOYSTICK:
        currentState = new JoystickMenu();
        break;
    }


    //Change the current state ID
    stateID = nextState;

    //NULL the next state ID
    nextState = STATE_NULL;
  }
}

// Setup game
void setup(){
  // Load allegro library
  allegro_init();
  alpng_init();
  install_timer();
  install_keyboard();
  install_mouse();
  install_joystick(JOY_TYPE_AUTODETECT);
  install_sound(DIGI_AUTODETECT,MIDI_AUTODETECT,".");
  set_color_depth(32);

  // Setup for FPS system
  LOCK_VARIABLE(ticks);
  LOCK_FUNCTION(ticker);
  install_int_ex(ticker, BPS_TO_TIMER(updates_per_second));

  LOCK_VARIABLE(game_time);
  LOCK_FUNCTION(game_time_ticker);
  install_int_ex(game_time_ticker, BPS_TO_TIMER(10));

  // Close button
  LOCK_FUNCTION(close_button_handler);
  set_close_button_callback(close_button_handler);

  // Game state
  stateID = STATE_NULL;
  nextState = STATE_NULL;

  // Variables
  closeGame = false;
}

void clean_up()
{
    //Delete game state and free state resources
    delete currentState;
}


//Main function*/
int main(){
  // Setup basic functionality
  setup();

  //Set the current state ID
  stateID = STATE_INIT;

  //Set the current game state object
  currentState = new Init();

  while( !key[KEY_ESC] && !close_button_pressed && stateID != STATE_EXIT){
    while(ticks == 0){
      rest(1);
    }
    while(ticks > 0){
      int old_ticks = ticks;

      //Do state logic
      currentState -> update();

      //Change state if needed
      change_state();

      // Counter for FPS
      frames_done++;

      ticks--;
      if(old_ticks <= ticks){
        break;
      }
    }
    if(game_time - old_time >= 10){
      fps = frames_done;
      frames_done = 0;
      old_time = game_time;
    }
    //Do state rendering
    currentState -> draw();
  }

  //Clean up
  clean_up();

  return 0;
}
END_OF_MAIN();
