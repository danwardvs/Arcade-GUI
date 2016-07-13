#include "JoystickMenu.h"

JoystickMenu::JoystickMenu()
{
  // Create buffer
  buffer = create_bitmap( SCREEN_W, SCREEN_H);

  // Allow transparency
  set_alpha_blender();

  // Setup colours, options are BG_VIBRANT, BG_GRAYSCALE, BG_PASTEL, BG_BALANCED and a speed (1 is default)
  main_bg = new color_background( "images/overlay.png", BG_BALANCED, 0.2);

  // Minigame
  ship_x = SCREEN_W/2;
  ship_y = SCREEN_H - 630;

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

  // Gui images
  cursor = load_bitmap_ex( "images/cursor.png");
  overlay_text_temp = load_bitmap_ex( "images/overlay_text.png");

  overlay_text = create_bitmap( SCREEN_W, overlay_text_temp -> h);
  stretch_sprite( overlay_text, overlay_text_temp, 0, 0, SCREEN_W, overlay_text_temp -> h);
}

void JoystickMenu::update()
{
  // Ask joystick for keys
  poll_joystick();

  // Mini Game
  if( joy[0].stick[0].axis[0].d1 && (ship_x > (SCREEN_W/2 - 298)))
    ship_x -= 2;
  else if( joy[0].stick[0].axis[0].d2 && (ship_x < (SCREEN_W/2 + 298 - ship -> w)))
    ship_x += 2;

  // Change background colour
  main_bg -> change_colours();

  //Hide mouse
  if( joy_buttonpressed(0))
    hide_mouse=true;

  if( mouse_x != old_mouse_x || mouse_y != old_mouse_y)
    hide_mouse = false;

  old_mouse_x = mouse_x;
  old_mouse_y = mouse_y;
}

void JoystickMenu::draw()
{
  // Draw background
  main_bg -> draw( buffer);

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

  if(!hide_mouse)
    draw_trans_sprite( buffer, cursor, mouse_x, mouse_y);

  draw_sprite(screen,buffer,0,0);
}

JoystickMenu::~JoystickMenu()
{

}
