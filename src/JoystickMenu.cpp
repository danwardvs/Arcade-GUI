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

  // Buttons
  for( int i = 0; i < 8; i++){
    std::string file_name = std::string("images/joystick/joystick_button_") + itos(i + 1) + std::string(".png");
    img_joystick_button[i]  = load_bitmap_ex( file_name.c_str());
  }

  // Positions
  for( int i = 0; i < 8; i++){
    std::string file_name = std::string("images/joystick/joystick_pos_") + itos(i + 1) + std::string(".png");
    img_joystick_positions[i]  = load_bitmap_ex( file_name.c_str());
  }

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

  // Move ship in minigame
  if( (joy[0].stick[0].axis[0].d1 || key[KEY_LEFT]) && (ship_x > (SCREEN_W/2 - 298)))
    ship_x -= 2;
  else if( (joy[0].stick[0].axis[0].d2 || key[KEY_RIGHT]) && (ship_x < (SCREEN_W/2 + 298 - ship -> w)))
    ship_x += 2;

  // Change background colour
  main_bg -> change_colours();

  //Hide mouse if joystick used
  if( joy_buttonpressed(0))
    hide_mouse = true;

  // Detect mouse motion to unhide
  if( mouse_x != old_mouse_x || mouse_y != old_mouse_y){
    hide_mouse = false;
  }
  else{
    old_mouse_x = mouse_x;
    old_mouse_y = mouse_y;
  }
}

void JoystickMenu::draw()
{
  // Draw background
  main_bg -> draw( buffer);

  // Background
  draw_trans_sprite( buffer, joystick_background2, SCREEN_W/2 - (joystick_background2 -> w/2), SCREEN_H - (joystick_background -> h + joystick_background2 -> h));
  draw_trans_sprite( buffer, joystick_background , SCREEN_W/2 - (joystick_background  -> w/2), SCREEN_H - (joystick_background -> h));

  // Buttons
  if( joy[0].button[0].b || key[KEY_1])
    draw_sprite( buffer, img_joystick_button[0], SCREEN_W/2 - 78, SCREEN_H - 424);
  if( joy[0].button[1].b || key[KEY_2])
    draw_sprite( buffer, img_joystick_button[1], SCREEN_W/2 + 30, SCREEN_H - 440);
  if( joy[0].button[2].b || key[KEY_3])
    draw_sprite( buffer, img_joystick_button[2], SCREEN_W/2 + 135, SCREEN_H - 427);
  if( joy[0].button[3].b || key[KEY_4])
    draw_sprite( buffer, img_joystick_button[3], SCREEN_W/2 - 81, SCREEN_H - 491);
  if( joy[0].button[4].b || key[KEY_5])
    draw_sprite( buffer, img_joystick_button[4], SCREEN_W/2 + 20, SCREEN_H - 513);
  if( joy[0].button[5].b || key[KEY_6])
    draw_sprite( buffer, img_joystick_button[5], SCREEN_W/2 + 122, SCREEN_H - 492);
  if( joy[0].button[10].b || key[KEY_7])
    draw_sprite( buffer, img_joystick_button[6], SCREEN_W/2 + 181, SCREEN_H - 551);
  if( joy[0].button[11].b || key[KEY_8])
    draw_sprite( buffer, img_joystick_button[7], SCREEN_W/2 + 251, SCREEN_H - 546);

  // Stick
  if( (joy[0].stick[0].axis[0].d1 && joy[0].stick[0].axis[1].d1) || (key[KEY_LEFT] && key[KEY_UP]))
    draw_sprite( buffer, img_joystick_positions[0], SCREEN_W/2 - 306, SCREEN_H - 553);
  else if( (joy[0].stick[0].axis[0].d2 && joy[0].stick[0].axis[1].d1) || (key[KEY_RIGHT] && key[KEY_UP]))
    draw_sprite( buffer, img_joystick_positions[2], SCREEN_W/2 - 306, SCREEN_H - 553);
  else if( (joy[0].stick[0].axis[0].d2 && joy[0].stick[0].axis[1].d2) || (key[KEY_RIGHT] && key[KEY_DOWN]))
    draw_sprite( buffer, img_joystick_positions[4], SCREEN_W/2 - 306, SCREEN_H - 553);
  else if( (joy[0].stick[0].axis[0].d1 && joy[0].stick[0].axis[1].d2) || (key[KEY_LEFT] && key[KEY_DOWN]))
    draw_sprite( buffer, img_joystick_positions[6], SCREEN_W/2 - 306, SCREEN_H - 553);
  else if( (joy[0].stick[0].axis[1].d1) || key[KEY_UP])
    draw_sprite( buffer, img_joystick_positions[1], SCREEN_W/2 - 306, SCREEN_H - 553);
  else if( (joy[0].stick[0].axis[0].d2) || key[KEY_RIGHT])
    draw_sprite( buffer, img_joystick_positions[3], SCREEN_W/2 - 306, SCREEN_H - 553);
  else if( (joy[0].stick[0].axis[1].d2) || key[KEY_DOWN])
    draw_sprite( buffer, img_joystick_positions[5], SCREEN_W/2 - 306, SCREEN_H - 553);
  else if( (joy[0].stick[0].axis[0].d1) || key[KEY_LEFT])
    draw_sprite( buffer, img_joystick_positions[7], SCREEN_W/2 - 306, SCREEN_H - 553);


  // Minigame
  draw_trans_sprite( buffer, ship, ship_x, ship_y);

  // Draw mouse if joystick not in use
  if(!hide_mouse)
    draw_trans_sprite( buffer, cursor, mouse_x, mouse_y);

  // Buffer to screen
  draw_sprite(screen,buffer,0,0);
}

JoystickMenu::~JoystickMenu()
{

}
