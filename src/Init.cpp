#include "Init.h"

Init::Init()
{
  // Set window title
  set_window_title("Arcade GUI");

  // Init screen
  int w, h;
  get_desktop_resolution(&w, &h);
  set_gfx_mode(GFX_AUTODETECT_WINDOWED, 1024, 768, 0, 0);
  install_sound(DIGI_AUTODETECT,MIDI_AUTODETECT,".");

  // Load fonts
  segoe = load_font_ex("fonts/segoe_ui_semibold_bold_62pt_antia.pcx");
}

void Init::update()
{
  // Change to splash screen
  set_next_state(STATE_MENU);
}

void Init::draw()
{

}

Init::~Init()
{

}
