#include "color_background.h"

color_background::color_background(char *overlay, int preset_name, double speed)
{
  BITMAP* overlay_temp;

  if(!(overlay_temp = load_bitmap( overlay, NULL))){
    overlay_temp = create_bitmap( SCREEN_W, SCREEN_H);
    rectfill( overlay_temp, 0, 0, SCREEN_W, SCREEN_H, makecol(255, 0, 0));
  }

  overlay_image = create_bitmap( SCREEN_W, SCREEN_H);
  stretch_sprite( overlay_image, overlay_temp, 0, 0, SCREEN_W, SCREEN_H);

  destroy_bitmap( overlay_temp);

  background_r = 0;
  background_g = 0;
  background_b = 0;
  background_r_up = false;
  background_g_up = false;
  background_b_up = false;


  setup_colours( preset_name, speed);
}

color_background::~color_background()
{
  //dtor
}

// Setup colours for background
// For more vibrant colours, space out numbers farther e.g. 1, 125, 254
// For more pastel colours, keep numbers close together e.g. 1, 10, 20
void color_background::setup_colours( int preset_name, double newSpeed){
  if( preset_name == BG_VIBRANT){
    background_r = 1.0;
    background_g = 166.0;
    background_b = 77.0;
    background_r_up = background_g_up = true;
    background_b_up = false;
  }
  else if( preset_name == BG_GRAYSCALE){
    background_r = 1.0;
    background_g = 1.0;
    background_b = 1.0;
    background_r_up = background_g_up = background_b_up = true;
  }
  else if( preset_name == BG_PASTEL){
    background_r = 1.0;
    background_g = 40.0;
    background_b = 80.0;
    background_r_up = background_g_up = background_b_up = true;
  }
  else if( preset_name == BG_BALANCED){
    background_r = 1.0;
    background_g = 100.0;
    background_b = 200.0;
    background_r_up = background_g_up = background_b_up = true;
  }
  background_speed = newSpeed;
}

// Change colours
void color_background::change_colours(){
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

void color_background::draw( BITMAP *tempBuffer){
  // Background
  rectfill( tempBuffer, 0, 0, SCREEN_W, SCREEN_H, makecol( background_r, background_g, background_b));

  // Overlay
  draw_trans_sprite( tempBuffer, overlay_image, 0, 0);
}
