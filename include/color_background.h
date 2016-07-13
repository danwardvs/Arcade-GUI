#ifndef COLOR_BACKGROUND_H
#define COLOR_BACKGROUND_H

#include <allegro.h>

#define BG_VIBRANT 0
#define BG_GRAYSCALE 1
#define BG_PASTEL 2
#define BG_BALANCED 3

class color_background
{
  public:
    color_background(char *overlay, int preset_name, double speed);
    virtual ~color_background();

    void setup_colours( int preset_name, double newSpeed);
    void change_colours();

    void draw( BITMAP *tempBuffer);
  protected:
  private:
    // Background variables
    double background_r, background_g, background_b;
    bool background_r_up, background_g_up, background_b_up;
    double background_speed;

    BITMAP* overlay_image;
};

#endif // COLOR_BACKGROUND_H
