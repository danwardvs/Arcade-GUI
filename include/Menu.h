#ifndef MENU_H
#define MENU_H

#include "GameState.h"

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

#include "rapidxml.hpp"
#include "rapidxml_print.hpp"

#include "globals.h"
#include "tools.h"
#include "color_background.h"

//Amount of games this bugger can handle
#define MAX_NUMBER_GAMES 100

class Menu : public GameState
{
  public:
    //Main loop functions
    Menu();
    void update();
    void draw();
    ~Menu();
  protected:
  private:
    // Images
    BITMAP *buffer;
    BITMAP *cursor;
    BITMAP *overlay_text, *overlay_text_temp;

    int game_focus;
    int step;

    int old_mouse_x;
    int old_mouse_y;
    bool hide_mouse;

    int existing_games;

    // Scale of icon in focus
    static const double icon_scale = 200;

    // Transition of icons
    double icon_transition;
    static const double icon_transition_speed = 20;

    bool location_clicked( int min_x, int max_x, int min_y, int max_y);
    void load_games( char *file);

    // Background
    color_background *main_bg;
};

#endif // MENU_H
