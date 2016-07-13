#include <allegro.h>
#include <alpng.h>
#include <time.h>/*
#include <windows.h>
#include <shellapi.h>*/
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <vector>
#include "tools.h"

#include "rapidxml.hpp"
#include "rapidxml_print.hpp"

#define MENU 0
#define JOYSTICK 1

using namespace rapidxml;
using namespace std;

//Amount of games this bugger can handle
#define MAX_NUMBER_GAMES 100

// Images
BITMAP *buffer;
BITMAP *cursor;
BITMAP *overlay, *overlay_text;
BITMAP *overlay_temp, *overlay_text_temp;

BITMAP *joystick_right;
BITMAP *joystick_right_up;
BITMAP *joystick_right_down;
BITMAP *joystick_up;
BITMAP *joystick_left;
BITMAP *joystick_left_up;
BITMAP *joystick_left_down;
BITMAP *joystick_down;

BITMAP *joystick_background;
BITMAP *joystick_button_1;
BITMAP *joystick_button_2;
BITMAP *joystick_button_3;
BITMAP *joystick_button_4;
BITMAP *joystick_button_5;
BITMAP *joystick_button_6;
BITMAP *joystick_button_7;
BITMAP *joystick_button_8;


// Minigame
BITMAP *ship;
BITMAP *joystick_background2;
int ship_x, ship_y;

// Fonts
FONT* segoe;

int game_focus = 0;
int step;

int old_mouse_x;
int old_mouse_y;
bool hide_mouse;

int existing_games = 0;
int settings[5];

// Scale of icon in focus
const double icon_scale = 200;

// Transition of icons
double icon_transition;
const double icon_transition_speed = 20;

char* read_data;

// Background variables
double background_r, background_g, background_b = 0;
bool background_r_up, background_g_up, background_b_up = true;
double background_speed;

enum background_colours{
  VIBRANT,
  GREYSCALE,
  PASTEL,
  BALANCED
};

// Game icons
typedef struct{
  bool exists;
  int x;
  int y;
  string path;
  string name;
  string icon_path;

  BITMAP *icon;
} game;

game games[MAX_NUMBER_GAMES];

// Area clicked
bool location_clicked( int min_x, int max_x, int min_y, int max_y){
  if(mouse_x >= min_x && mouse_x <= max_x && mouse_y >= min_y && mouse_y <= max_y && (mouse_b & 1 || joy[0].button[1].b))
    return true;
  else return false;
}

void load_games( char *file){
    //Setup xml datatypes
    xml_document<> doc;
    xml_node<> * root_node;

    ifstream theFile (file);
    vector<char> xml_buffer((istreambuf_iterator<char>(theFile)), istreambuf_iterator<char>());
    xml_buffer.push_back('\0');

    // Parse the buffer using the xml file parsing library into doc
    doc.parse<0>(&xml_buffer[0]);

    // Find our root node
    root_node = doc.first_node("data");

    //Iterates through the whole document.
    //If it finds a game with the same number as
    //the value in the argument, it reads the data.
    int i = 0;
    for (xml_node<> * game_node = root_node->first_node("game"); game_node; game_node = game_node->next_sibling()){
      //Reads the data in the game node, assigning it to the games[] structure
      games[i].name = game_node->first_attribute("id")->value();
      games[i].path = game_node->first_attribute("path")->value();
      games[i].icon_path = "images/icons/" + string(game_node->first_attribute("icon")->value());
      games[i].icon = load_bitmap_ex( games[i].icon_path.c_str());
      games[i].exists = true;

      i++;
    }
    existing_games = i;
}

// Setup colours for background
// For more vibrant colours, space out numbers farther e.g. 1, 125, 254
// For more pastel colours, keep numbers close together e.g. 1, 10, 20
void setup_colours( int preset_name, double newSpeed){
  if( preset_name == VIBRANT){
    background_r = 1.0;
    background_g = 166.0;
    background_b = 77.0;
    background_r_up = background_g_up = true;
    background_b_up = false;
  }
  else if( preset_name == GREYSCALE){
    background_r = 1.0;
    background_g = 1.0;
    background_b = 1.0;
    background_r_up = background_g_up = background_b_up = true;
  }
  else if( preset_name == PASTEL){
    background_r = 1.0;
    background_g = 40.0;
    background_b = 80.0;
    background_r_up = background_g_up = background_b_up = true;
  }
  else if( preset_name == BALANCED){
    background_r = 1.0;
    background_g = 100.0;
    background_b = 200.0;
    background_r_up = background_g_up = background_b_up = true;
  }
  background_speed = newSpeed;
}

// Change colours
void change_colours(){
  // If colour value increase, increase by 1, else decrease by 1
  // When increasing,  coolbackground_r_up = true or 1, *2 -1 still equals 1
  // When increasing, background_r_up = false or 0, *2 -1 still equals -1
  background_r += (background_r_up *2 -1) * background_speed;
  background_g += (background_g_up *2 -1) * background_speed;
  background_b += (background_b_up *2 -1) * background_speed;

  // Start decreasing once it reaches the end
  if(background_r >= 255 || background_r <= 0)
    background_r_up = !background_r_up;
  if(background_g >= 255 || background_g <= 0)
    background_g_up = !background_g_up;
  if(background_b >= 255 || background_b <= 0)
    background_b_up = !background_b_up;
}

// Update
void update(){
  // Ask joystick for keys
  poll_joystick();
  // Menu
  if( GAME_STATE == MENU){
    // Joystick APP
    if((location_clicked( 442, 837, 322, 717) || joy[0].button[2].b) && step > 9 && icon_transition == 0){
      if(games[game_focus].path=="local.joystick"){
          GAME_STATE=JOYSTICK;
      }
      else{
        //ShellExecute(NULL, "open", games[game_focus].path.c_str(), NULL, NULL, SW_SHOWDEFAULT);
        step = 0;
      }
    }

    // Scroll Left
    if((location_clicked( 0, 400, 0, SCREEN_H)|| joy[0].stick[0].axis[0].d1) && step > 9 && icon_transition == 0 && game_focus > 0){
      int i=game_focus-1;
      while(!games[i].exists)
        i--;
      game_focus=i;
      icon_transition = -300;
      step = 0;
    }
    // Scroll Right
    if((location_clicked( SCREEN_W - 400,SCREEN_W, 0, SCREEN_H)|| joy[0].stick[0].axis[0].d2) && step>9 && icon_transition == 0 && game_focus < existing_games - 1){

      //Allan, take your bracket standards elsewhere. They are not wanted.
      int i=game_focus+1;
      while(!games[i].exists)
        i++;
      game_focus=i;
      icon_transition = 300;
      step = 0;
    }

    // Scroll
    if( icon_transition > icon_transition_speed)
      icon_transition -= icon_transition_speed;
    else if( icon_transition > 0)
      icon_transition = 0;
    else if( icon_transition < 0)
      icon_transition += icon_transition_speed;
    else if( icon_transition < icon_transition_speed)
      icon_transition = 0;

    // Change background colour, with speed
    change_colours();

    // Step counter
    step++;
  }
  if( GAME_STATE == JOYSTICK){
    // Mini Game
    if( joy[0].stick[0].axis[0].d1 && (ship_x > (SCREEN_W/2 - 298)))
      ship_x -= 2;
    else if( joy[0].stick[0].axis[0].d2 && (ship_x < (SCREEN_W/2 + 298 - ship -> w)))
      ship_x += 2;

    // Change background colour, with spesed
    change_colours();
  }
  //Hide mouse
  if( joy_buttonpressed(0))
    hide_mouse=true;

  if( mouse_x != old_mouse_x || mouse_y != old_mouse_y)
    hide_mouse = false;

  old_mouse_x = mouse_x;
  old_mouse_y = mouse_y;
}

// Draw to screen
void draw(){
  // Menu
  if(GAME_STATE == MENU){
    // Background
    rectfill( buffer, 0, 0, SCREEN_W, SCREEN_H, makecol( background_r, background_g, background_b));
    draw_trans_sprite( buffer, overlay, 0, 0);

    // Title
    draw_trans_sprite( buffer, overlay_text, 0, 50);
    textout_centre_ex( buffer, segoe, games[game_focus].name.c_str(), SCREEN_W/2, 80, makecol(0,0,0), -1);

    // Draw icon (stretched if needed)
    for( int i = 0; i < existing_games; i++){
      // Initial scale is original
      int new_scale = 0;

      // If its the current icon, enlarge it
      if( i == game_focus)
        new_scale = icon_scale;

      // Temporary icon, allows manipulation. Makes it the size of the original icon with scaling if needed
      BITMAP* newIcon = create_bitmap( games[i].icon -> w + new_scale, games[i].icon -> h + new_scale);

      // Stretches icon it if its in focus
      stretch_sprite( newIcon, games[i].icon, 0, 0, newIcon -> w, newIcon -> h);

      // Draw it with transparency
      draw_trans_sprite( buffer, newIcon, games[i].x - (((game_focus + 1) * 300) - icon_transition) - new_scale/2, games[i].y - new_scale/2);

      // Delete temporary bitmap to free memory
      destroy_bitmap( newIcon);
    }
  }
  // Joystick APP
  if(GAME_STATE == JOYSTICK){
    // Background
    rectfill( buffer, 0, 0, SCREEN_W, SCREEN_H, makecol( background_r, background_g, background_b));

    // 2 Overlays for added effect
    draw_trans_sprite( buffer, overlay, 0, 0);
    draw_trans_sprite( buffer, overlay, 0, 0);

    // Background
    draw_trans_sprite( buffer, joystick_background2, SCREEN_W/2 - (joystick_background2 -> w/2), SCREEN_H - (joystick_background -> h + joystick_background2 -> h));
    draw_trans_sprite( buffer, joystick_background , SCREEN_W/2 - (joystick_background  -> w/2), SCREEN_H - (joystick_background -> h));

    // Buttons
    if( joy[0].button[0].b)
      draw_sprite( buffer, joystick_button_1, SCREEN_W/2 - 78, SCREEN_H - 425);
    if( joy[0].button[1].b)
      draw_sprite( buffer, joystick_button_2, SCREEN_W/2 + 30, SCREEN_H - 440);
    if( joy[0].button[2].b)
      draw_sprite( buffer, joystick_button_3, SCREEN_W/2 + 135, SCREEN_H - 428);
    if( joy[0].button[3].b)
      draw_sprite( buffer, joystick_button_4, SCREEN_W/2 - 82, SCREEN_H - 491);
    if( joy[0].button[4].b)
      draw_sprite( buffer, joystick_button_5, SCREEN_W/2 + 20, SCREEN_H - 511);
    if( joy[0].button[5].b)
      draw_sprite( buffer, joystick_button_6, SCREEN_W/2 + 122, SCREEN_H - 491);
    if( joy[0].button[10].b)
      draw_sprite( buffer, joystick_button_7, SCREEN_W/2 + 179, SCREEN_H - 552);
    if( joy[0].button[11].b)
      draw_sprite( buffer, joystick_button_8, SCREEN_W/2 + 251, SCREEN_H - 546);

    // Stick
    if( joy[0].stick[0].axis[0].d2 && joy[0].stick[0].axis[1].d1)
      draw_sprite( buffer, joystick_right_up, SCREEN_W/2 - 306, SCREEN_H - 553);
    else if( joy[0].stick[0].axis[0].d2 && joy[0].stick[0].axis[1].d2)
      draw_sprite( buffer,joystick_right_down, SCREEN_W/2 - 306, SCREEN_H - 553);
    else if( joy[0].stick[0].axis[0].d1 && joy[0].stick[0].axis[1].d1)
      draw_sprite( buffer, joystick_left_up, SCREEN_W/2 - 306, SCREEN_H - 553);
    else if( joy[0].stick[0].axis[0].d1 && joy[0].stick[0].axis[1].d2)
      draw_sprite( buffer, joystick_left_down, SCREEN_W/2 - 306, SCREEN_H - 553);
    else if( joy[0].stick[0].axis[1].d1)
      draw_sprite( buffer, joystick_up, SCREEN_W/2 - 306, SCREEN_H - 553);
    else if( joy[0].stick[0].axis[1].d2)
      draw_sprite( buffer, joystick_down, SCREEN_W/2 - 306, SCREEN_H - 553);
    else if( joy[0].stick[0].axis[0].d1)
      draw_sprite(buffer, joystick_left, SCREEN_W/2 - 306, SCREEN_H - 553);
    else if( joy[0].stick[0].axis[0].d2)
      draw_sprite( buffer, joystick_right, SCREEN_W/2 - 306, SCREEN_H - 553);

    // Minigame
    draw_trans_sprite( buffer, ship, ship_x, ship_y);
  }

  //Trying to get rid of errors in my bread (debug)
  if(2>1){
    textprintf_ex(buffer,font,20,20,makecol(255,255,255),-1,"game_focus: %i",game_focus);
  }

  if(!hide_mouse)
    draw_trans_sprite( buffer, cursor, mouse_x, mouse_y);

  draw_sprite(screen,buffer,0,0);
}

// Setup game
void setup(){
  // Setup allegro
  allegro_init();
  alpng_init();
  install_timer();
  install_keyboard();
  install_mouse();
  set_color_depth(32);
  install_joystick(JOY_TYPE_AUTODETECT);

  // Init screen
  int w, h;
  get_desktop_resolution(&w, &h);
  set_gfx_mode(GFX_AUTODETECT_WINDOWED, 1024, 768, 0, 0);
  install_sound(DIGI_AUTODETECT,MIDI_AUTODETECT,".");

  // Window Title
  set_window_title("Arcade GUI");

  // Create buffer
  buffer = create_bitmap( SCREEN_W, SCREEN_H);

  // Allow transparency
  set_alpha_blender();

  // Init random number generator
  srand(time(NULL));

  // Load games
  load_games("games/games.xml");

  // Setup colours, options are VIBRANT, GREYSCALE, PASTEL, BALANCED and a speed (1 is default)
  setup_colours( BALANCED, 0.2);

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

  // Minigame
  ship_x = SCREEN_W/2;
  ship_y = SCREEN_H - 630;

  // Gui images
  cursor = load_bitmap_ex( "images/cursor.png");
  overlay_temp = load_bitmap_ex( "images/overlay.png");
  overlay_text_temp = load_bitmap_ex( "images/overlay_text.png");

  // Overlay Stretch
  overlay = create_bitmap( SCREEN_W, SCREEN_H);
  stretch_sprite( overlay, overlay_temp, 0, 0, SCREEN_W, SCREEN_H);

  overlay_text = create_bitmap( SCREEN_W, overlay_text_temp -> h);
  stretch_sprite( overlay_text, overlay_text_temp, 0, 0, SCREEN_W, overlay_text_temp -> h);

  // Joystick
  joystick_background = load_bitmap_ex( "images/joystick/joystick_background.png");
  joystick_button_1  = load_bitmap_ex( "images/joystick/joystick_button_1.png");
  joystick_button_2  = load_bitmap_ex( "images/joystick/joystick_button_2.png");
  joystick_button_3  = load_bitmap_ex( "images/joystick/joystick_button_3.png");
  joystick_button_4  = load_bitmap_ex( "images/joystick/joystick_button_4.png");
  joystick_button_5  = load_bitmap_ex( "images/joystick/joystick_button_5.png");
  joystick_button_6  = load_bitmap_ex( "images/joystick/joystick_button_6.png");
  joystick_button_7  = load_bitmap_ex( "images/joystick/joystick_button_7.png");
  joystick_button_8  = load_bitmap_ex( "images/joystick/joystick_button_8.png");

  joystick_up = load_bitmap_ex( "images/joystick/joystick_up.png");
  joystick_down = load_bitmap_ex( "images/joystick/joystick_down.png");
  joystick_right = load_bitmap_ex( "images/joystick/joystick_right.png");
  joystick_left = load_bitmap_ex( "images/joystick/joystick_left.png");
  joystick_left_up = load_bitmap_ex( "images/joystick/joystick_left_up.png");
  joystick_left_down = load_bitmap_ex( "images/joystick/joystick_left_down.png");
  joystick_right_up = load_bitmap_ex( "images/joystick/joystick_right_up.png");
  joystick_right_down = load_bitmap_ex( "images/joystick/joystick_right_down.png");
  joystick_background2 = load_bitmap_ex( "images/joystick/background.png");

  ship = load_bitmap_ex("images/joystick/ship.png");

  // Load fonts
  segoe = load_font_ex("segoe_ui_semibold_bold_62pt_antia.pcx");

  // X and Y Positions
  int i, j;
  for( j = 0, i = 0; i <= existing_games; i++){
    if(games[i].exists){
      j++;
      games[i].x = (SCREEN_W/2 - 100) + (300 * j);
      games[i].y = (SCREEN_H/2 - 100) + SCREEN_H/10;
    }
  }
}
