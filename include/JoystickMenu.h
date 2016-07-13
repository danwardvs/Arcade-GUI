#ifndef JOYSTICKMENU_H
#define JOYSTICKMENU_H

#include "GameState.h"

#include <allegro.h>
#include <alpng.h>
#include <string>
#include <vector>

#include "globals.h"
#include "tools.h"
#include "color_background.h"

class JoystickMenu : public GameState
{
  public:
    //Main loop functions
    JoystickMenu();
    void update();
    void draw();
    ~JoystickMenu();
  protected:
  private:
    BITMAP *buffer;
    BITMAP *cursor;
    BITMAP *overlay_text;
    BITMAP *overlay_text_temp;

    BITMAP *joystick_background;

    BITMAP *img_joystick_button[8];
    BITMAP *img_joystick_positions[8];

    // Minigame
    BITMAP *ship;
    BITMAP *joystick_background2;
    int ship_x, ship_y;

    bool hide_mouse;
    int old_mouse_x;
    int old_mouse_y;

    color_background *main_bg;
};

#endif // JOYSTICKMENU_H
