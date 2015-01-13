#include<allegro.h>
#include<alpng.h>
#include<time.h>
#include<windows.h>
#include<shellapi.h>
#include<fstream>
#include<sstream>
#include<string>
#include<cstring>


#define MENU 0
#define JOYSTICK 1

using namespace std;

// Images
BITMAP *buffer;
BITMAP *cursor;
BITMAP *overlay, *overlay_text;

BITMAP* icon[20];

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
FONT* f1;
FONT* f2;
FONT* f3;
FONT* f4;
FONT* f5;
FONT* segoe;

// Close button handler
bool close_button_pressed;

// FPS System
volatile int ticks = 0;
const int updates_per_second = 60;
volatile int game_time = 0;

// Variables
int GAME_STATE=MENU;

int fps;
int frames_done;
int old_time;
int game_focus = 1;
int step;

int old_mouse_x;
int old_mouse_y;
bool hide_mouse;

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
struct game{
  int x;
  int y;
  char* path;
  char* name;
  string icon;
}game[100];

// FPS System
void ticker(){
  ticks++;
}
END_OF_FUNCTION(ticker)

void game_time_ticker(){
  game_time++;
}
END_OF_FUNCTION(ticker)

// Close button handler
void close_button_handler(void){
  close_button_pressed = TRUE;
}
END_OF_FUNCTION(close_button_handler)


// Convert string to int
int convertStringToInt(string newString){
  int result;
  stringstream(newString) >> result;
  return result;
}

// Area clicked
bool location_clicked(int min_x,int max_x,int min_y,int max_y){
  if(mouse_x > min_x && mouse_x < max_x && mouse_y > min_y && mouse_y < max_y && (mouse_b & 1 || joy[0].button[1].b))
    return true;
  else return false;
}

// Random number generator. Use int random(highest,lowest);
int random(int newLowest, int newHighest)
{
  int lowest = newLowest, highest = newHighest;
  int range = (highest - lowest) + 1;
  int randomNumber = lowest+int(range*rand()/(RAND_MAX + 1.0));
  return randomNumber;
}

// Streamline error reporting in file loading
void abort_on_error(const char *message){
	 set_window_title("Error!");
	 if (screen != NULL){
	    set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
	 }
	 allegro_message("%s.\n %s\n", message, allegro_error);
	 exit(-1);
}
bool joy_buttonpressed(){
    bool buttonpressed=false;
    for(int i=0; i<joy[0].num_buttons; i++)
        if(joy[0].button[i].b)buttonpressed=true;
    if(joy[0].stick[0].axis[1].d1 || joy[0].stick[0].axis[1].d2 || joy[0].stick[0].axis[0].d1 || joy[0].stick[0].axis[0].d2)buttonpressed=true;
    return buttonpressed;

}

// Write to settings file
void write_settings(){
  settings[3] = game[1].x;

  ofstream settings_file;
  settings_file.open("games/game_1.txt");

   settings_file<<game[1].path<<" ";
   settings_file<<game[1].icon<<" ";

   settings_file.close();
}

// Read from settings file
void read_settings(){
  ifstream read("games/game_1.dat");

  read>>read_data;

  // game[1].path=string(read_data);
  read.close();
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
  if(GAME_STATE==MENU){
    // Joystick APP
    if((location_clicked( 442, 837, 322, 717) || joy[0].button[2].b) && step > 9 && icon_transition == 0){
      ShellExecute(NULL, "open", game[game_focus].path, NULL, NULL, SW_SHOWDEFAULT);
      step = 0;
      if(game_focus == 7)
        GAME_STATE=JOYSTICK;
    }
    // Scroll Left
    if((location_clicked( 0, 400, 0, SCREEN_H)|| joy[0].stick[0].axis[0].d1) && step > 9 && icon_transition == 0 && game_focus > 1){
      game_focus--;
      icon_transition = -300;
      step = 0;
    }
    // Scroll Right
    if((location_clicked( SCREEN_W - 400,SCREEN_W, 0, SCREEN_H)|| joy[0].stick[0].axis[0].d2) && step>9 && icon_transition == 0 && game_focus < 7){
      game_focus++;
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

    // Read and write settings
    if(key[KEY_P])
      write_settings();
    if(key[KEY_O])
      read_settings();

    // Step counter
    step++;
  }
  if(GAME_STATE == JOYSTICK){
    // Minigame
    if(joy[0].stick[0].axis[0].d1 && ship_x > 337)
      ship_x--;
    else if(joy[0].stick[0].axis[0].d2 && ship_x < 899)
      ship_x++;

    // Change background colour, with speed
    change_colours();
  }
  //Hide mouse
  if(joy_buttonpressed()){
    hide_mouse=true;
  }
  if(mouse_x!=old_mouse_x || mouse_y!=old_mouse_y)hide_mouse=false;
  old_mouse_x=mouse_x;
  old_mouse_y=mouse_y;

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
    textout_centre_ex( buffer, segoe, game[game_focus].name, SCREEN_W/2, 80, makecol(0,0,0), -1);

    // Draw icon (stretched if needed)
    for (int i = 1; i <= 7; i++){
      // Initial scale is original
      int new_scale = 0;
      // If its the current icon, enlarge it
      if( i == game_focus)
        new_scale = icon_scale;
      // Temporary icon, allows manipulation. Makes it the size of the original icon with scaling if needed
      BITMAP* newIcon = create_bitmap( icon[i] -> w + new_scale, icon[i] -> h + new_scale);
      // Stretches icon it if its in focus
      stretch_sprite( newIcon, icon[i], 0, 0, newIcon -> w, newIcon -> h);
      // Draw it with transparency
      draw_trans_sprite(buffer, newIcon, game[i].x - ((game_focus * 300) - icon_transition) - new_scale/2, game[i].y - new_scale/2);
      // Delete temporary bitmap to free memory
      destroy_bitmap(newIcon);
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
    draw_trans_sprite( buffer, joystick_background2, 0, 0);
    draw_trans_sprite( buffer, joystick_background, 0, 0);

    // Buttons
    if(joy[0].button[0].b)
      draw_sprite(buffer,joystick_button_1,563,600);
    if(joy[0].button[1].b)
      draw_sprite(buffer,joystick_button_2,671,584);
    if(joy[0].button[2].b)
      draw_sprite(buffer,joystick_button_3,776,597);
    if(joy[0].button[3].b)
      draw_sprite(buffer,joystick_button_4,560,533);
    if(joy[0].button[4].b)
      draw_sprite(buffer,joystick_button_5,661,511);
    if(joy[0].button[5].b)
      draw_sprite(buffer,joystick_button_6,763,532);
    if(joy[0].button[10].b)
      draw_sprite(buffer,joystick_button_7,822,473);
    if(joy[0].button[11].b)
      draw_sprite(buffer,joystick_button_8,892,478);

    // Stick
    if(joy[0].stick[0].axis[0].d2 && joy[0].stick[0].axis[1].d1)
      draw_sprite(buffer,joystick_right_up,335,471);
    else if(joy[0].stick[0].axis[0].d2 && joy[0].stick[0].axis[1].d2)
      draw_sprite(buffer,joystick_right_down,335,471);
    else if(joy[0].stick[0].axis[0].d1 && joy[0].stick[0].axis[1].d1)
      draw_sprite(buffer,joystick_left_up,335,471);
    else if(joy[0].stick[0].axis[0].d1 && joy[0].stick[0].axis[1].d2)
      draw_sprite(buffer,joystick_left_down,335,471);
    else if(joy[0].stick[0].axis[1].d1)
      draw_sprite(buffer,joystick_up,335,471);
    else if(joy[0].stick[0].axis[1].d2)
      draw_sprite(buffer,joystick_down,335,471);
    else if(joy[0].stick[0].axis[0].d1)
      draw_sprite(buffer,joystick_left,335,471);
    else if(joy[0].stick[0].axis[0].d2)
      draw_sprite(buffer,joystick_right,335,471);

    // Minigame
    draw_trans_sprite( buffer, ship, ship_x, ship_y);
  }
  if(!hide_mouse)draw_trans_sprite(buffer,cursor,mouse_x,mouse_y);
  draw_sprite(screen,buffer,0,0);
}

// Setup game
void setup(){
  // Create buffer
  buffer = create_bitmap( 1280, 1024);

  // Allow transparency
  set_alpha_blender();

  // Init random number generator
  srand(time(NULL));

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

  // Load images
  // Game icons
  game[1].icon="icon_steam";
  game[2].icon="icon_csgo";
  game[3].icon="icon_garrysmod";
  game[4].icon="icon_dayofdefeat";
  game[5].icon="icon_mame";
  game[6].icon="icon_lol";
  game[7].icon="icon_joystick";

  // Minigame
  ship_x = 620;
  ship_y = 419;

  for (int i = 1; i < 8; i++){
    if (!( icon[i] = load_bitmap((string("images/icons/") + game[i].icon.c_str() + string(".png")).c_str(), NULL)))
      abort_on_error((string("Cannot find image images/icons/") + game[i].icon.c_str() + string(".png\nCheck your custom icons folder")).c_str());
  }


  if (!(cursor = load_bitmap("images/cursor.png", NULL)))
    abort_on_error("Cannot find image images/cursor.png\nPlease check your files and try again");
  if (!(overlay = load_bitmap("images/overlay.png", NULL)))
    abort_on_error("Cannot find image images/overlay.png\nPlease check your files and try again");
  if (!(overlay_text = load_bitmap("images/overlay_text.png", NULL)))
    abort_on_error("Cannot find image images/overlay_text.png\nPlease check your files and try again");


  // Joystick
  if (!(joystick_background= load_bitmap("images/joystick/joystick_background.png", NULL)))
    abort_on_error("Cannot find image images/joystick/joystick_background.png\nPlease check your files and try again");
  if (!(joystick_button_1 = load_bitmap("images/joystick/joystick_button_1.png", NULL)))
    abort_on_error("Cannot find image images/joystick/joystick_button_1.png\nPlease check your files and try again");
  if (!(joystick_button_2 = load_bitmap("images/joystick/joystick_button_2.png", NULL)))
    abort_on_error("Cannot find image images/joystick/joystick_button_2.png\nPlease check your files and try again");
  if (!(joystick_button_3 = load_bitmap("images/joystick/joystick_button_3.png", NULL)))
    abort_on_error("Cannot find image images/joystick/joystick_button_3.png\nPlease check your files and try again");
  if (!(joystick_button_4 = load_bitmap("images/joystick/joystick_button_4.png", NULL)))
    abort_on_error("Cannot find image images/joystick/joystick_button_4.png\nPlease check your files and try again");
  if (!(joystick_button_5 = load_bitmap("images/joystick/joystick_button_5.png", NULL)))
    abort_on_error("Cannot find image images/joystick/joystick_button_5.png\nPlease check your files and try again");
  if (!(joystick_button_6 = load_bitmap("images/joystick/joystick_button_6.png", NULL)))
    abort_on_error("Cannot find image images/joystick/joystick_button_6.png\nPlease check your files and try again");
  if (!(joystick_button_7 = load_bitmap("images/joystick/joystick_button_7.png", NULL)))
    abort_on_error("Cannot find image images/joystick/joystick_button_7.png\nPlease check your files and try again");
  if (!(joystick_button_8 = load_bitmap("images/joystick/joystick_button_8.png", NULL)))
    abort_on_error("Cannot find image images/joystick/joystick_button_8.png\nPlease check your files and try again");

  if (!(joystick_up = load_bitmap("images/joystick/joystick_up.png", NULL)))
    abort_on_error("Cannot find image images/joystick/joystick_up.png\nPlease check your files and try again");
  if (!(joystick_down = load_bitmap("images/joystick/joystick_down.png", NULL)))
    abort_on_error("Cannot find image images/joystick/joystick_down.png\nPlease check your files and try again");
  if (!(joystick_right = load_bitmap("images/joystick/joystick_right.png", NULL)))
    abort_on_error("Cannot find image images/joystick/joystick_right.png\nPlease check your files and try again");
  if (!(joystick_left = load_bitmap("images/joystick/joystick_left.png", NULL)))
    abort_on_error("Cannot find image images/joystick/joystick_left.png\nPlease check your files and try again");
  if (!(joystick_left_up = load_bitmap("images/joystick/joystick_left_up.png", NULL)))
    abort_on_error("Cannot find image images/joystick/joystick_left_up.png\nPlease check your files and try again");
  if (!(joystick_left_down = load_bitmap("images/joystick/joystick_left_down.png", NULL)))
    abort_on_error("Cannot find image images/joystick/joystick_left_down.png\nPlease check your files and try again");
  if (!(joystick_right_up = load_bitmap("images/joystick/joystick_right_up.png", NULL)))
    abort_on_error("Cannot find image images/joystick/joystick_right_up.png\nPlease check your files and try again");
  if (!(joystick_right_down = load_bitmap("images/joystick/joystick_right_down.png", NULL)))
    abort_on_error("Cannot find image images/joystick/joystick_right_down.png\nPlease check your files and try again");

  if (!(joystick_background2 = load_bitmap("images/joystick/background.png", NULL)))
    abort_on_error("Cannot find image images/joystick/background.png\nPlease check your files and try again");
  if (!(ship = load_bitmap("images/joystick/ship.png", NULL)))
    abort_on_error("Cannot find image images/joystick/ship.png\nPlease check your files and try again");

  // Load fonts
  f1 = load_font("segoe_ui_semibold_bold_62pt_antia.pcx", NULL, NULL);
  f2 = extract_font_range(f1, ' ', 'A'-1);
  f3 = extract_font_range(f1, 'A', 'Z');
  f4 = extract_font_range(f1, 'Z'+1, 'z');
  segoe = merge_fonts(f4, f5 = merge_fonts(f2, f3));

  // Destroy temporary fonts
  destroy_font(f1);
  destroy_font(f2);
  destroy_font(f3);
  destroy_font(f4);
  destroy_font(f5);

  game[1].path="C:\\Program Files (x86)\\Steam\\steam.exe";
  game[1].name="Steam Client";
  game[1].x=840;
  game[1].y=420;

  game[2].path="steam://rungameid/730";
  game[2].name="Counter-strike: Global Offensive";
  game[2].x=1140;
  game[2].y=420;

  game[3].path="steam://rungameid/4000";
  game[3].name="Garry's Mod";
  game[3].x=1440;
  game[3].y=420;

  game[4].path="steam://rungameid/30";
  game[4].name="Day of Defeat: Classic";
  game[4].x=1740;
  game[4].y=420;

  game[5].path="poop";
  game[5].name="MAME";
  game[5].x=2040;
  game[5].y=420;

  game[6].path="C:\\Riot Games\\League of Legends\\lol.launcher.exe";
  game[6].name="League of Legends";
  game[6].x=2340;
  game[6].y=420;

  game[7].path="crap";
  game[7].name="Joystick";
  game[7].x=2640;
  game[7].y=420;
}

// MAIN loop
int main(){

  read_settings();
  // Setup allegro
  allegro_init();
  alpng_init();
  install_timer();
  install_keyboard();
  install_mouse();
  set_color_depth(32);
  install_joystick(JOY_TYPE_AUTODETECT);

  // Init screen
  set_gfx_mode(GFX_AUTODETECT, 1280, 1024, 0, 0);
  install_sound(DIGI_AUTODETECT,MIDI_AUTODETECT,".");

  // Window Title
  set_window_title("Arcade GUI");

  // Setup game
  setup();

  // FPS System, Loops till exit
  while(!key[KEY_ESC] && !close_button_pressed){
    while(ticks == 0){
      rest(1);
    }
    while(ticks > 0){
      int old_ticks = ticks;
      update();
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
    draw();
  }
  return 0;
}
END_OF_MAIN()
