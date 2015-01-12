#include<allegro.h>
#include<alpng.h>
#include<time.h>
#include<windows.h>
#include<shellapi.h>
#include<fstream>
#include<sstream>

#define MENU 0
#define JOYSTICK 1

using namespace std;

// Images
BITMAP* buffer;
BITMAP* cursor;

BITMAP* icon_steam;
BITMAP* icon_csgo;
BITMAP* icon_garrysmod;
BITMAP* icon_dayofdefeat;
BITMAP* icon_mame;
BITMAP* icon_lol;
BITMAP* icon_joystick;

BITMAP* joystick_background;
BITMAP* joystick_button_1;
BITMAP* joystick_button_2;
BITMAP* joystick_button_3;
BITMAP* joystick_button_4;
BITMAP* joystick_button_5;
BITMAP* joystick_button_6;
BITMAP* joystick_button_7;
BITMAP* joystick_button_8;

// Fonts
FONT* f1;
FONT* f2;
FONT* f3;
FONT* f4;
FONT* f5;
FONT* arimo_22;

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
int game_focus=1;
int step;

int settings[5];

int background_r=200;
int background_g=200;
int background_b=255;

// Game icons
struct game{
  int x;
  int y;
  char* path;
  char* name;
  BITMAP* icon;
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

// Write to settings file
void write_settings(){
  settings[3]=game[1].x;

  ofstream settings_file;
  settings_file.open("games/game_1.txt");

  for (int i = 0; i < 4; i++){
    settings_file<<settings[i]<<" ";
  }
  settings_file<<game[1].path;

  settings_file.close();
}

// Read from settings file
void read_settings(){
  ifstream read("games/game_1.dat");
  for (int i = 0; i < 4; i++){
    read>>settings[i];
  }
  read>>game[1].path;
  read.close();

  game[1].x=settings[3];
}


// Update
void update(){
  // Ask joystick for keys
  poll_joystick();
  // Menu
  if(GAME_STATE==MENU){
    // Joystick APP
    if((location_clicked(412,612,200,400) || joy[0].button[2].b) && step>9){
      ShellExecute(NULL, "open", game[game_focus].path, NULL, NULL, SW_SHOWDEFAULT);
      step=0;
      if(game_focus==7)
        GAME_STATE=JOYSTICK;
    }
    // Scroll Left
    if((location_clicked(0,400,0,SCREEN_H)|| joy[0].stick[0].axis[0].d1) && step>9){
      step=0;
      if(game_focus>1)
        game_focus--;
    }
    // Scroll Right
    if((location_clicked(SCREEN_W-400,SCREEN_W,0,SCREEN_H)|| joy[0].stick[0].axis[0].d2) && step>9){
      step=0;
      if(game_focus<7)game_focus++;
    }

    // Change background colour
    background_r+=random(-1,1);
    if(background_r>255)
      background_r=255;
    if(background_r<200)
      background_r=200;
    background_b+=random(-1,1);
    if(background_b>255)
      background_b=255;
    if(background_b<200)
      background_b=200;
    background_g+=random(-1,1);
    if(background_g>255)
      background_g=255;
    if(background_g<200)
      background_g=200;

    // Read and write settings
    if(key[KEY_P])
      write_settings();
    if(key[KEY_O])
      read_settings();

    // Step counter
    step++;
  }
}

// Draw to screen
void draw(){
  // Background
  rectfill(buffer,0,0,1024,768,makecol(background_r,background_g,background_b));
  //rect(buffer,512,0,512,768,makecol(0,0,0));

  // Menu
  if(GAME_STATE==MENU){
    textout_centre_ex(buffer, arimo_22, game[game_focus].name, 512, 150, makecol(0,0,0), -1);
    for (int i = 1; i < 5; i++){
      draw_sprite(buffer,game[i].icon,game[i].x-(game_focus*300),game[i].y);
    }
    draw_trans_sprite(buffer,game[5].icon,game[5].x-(game_focus*300),game[5].y);
    draw_trans_sprite(buffer,game[6].icon,game[6].x-(game_focus*300),game[6].y);
    draw_sprite(buffer,game[7].icon,game[7].x-(game_focus*300),game[7].y);
  }
  // Joystick APP
  if(GAME_STATE==JOYSTICK){
    draw_sprite(buffer,joystick_background,0,0);
    if(joy[0].button[0].b)
      draw_sprite(buffer,joystick_button_1,435,344);
    if(joy[0].button[1].b)
      draw_sprite(buffer,joystick_button_2,543,328);
    if(joy[0].button[2].b)
      draw_sprite(buffer,joystick_button_3,648,341);
    if(joy[0].button[3].b)
      draw_sprite(buffer,joystick_button_4,432,277);
    if(joy[0].button[4].b)
      draw_sprite(buffer,joystick_button_5,533,255);
    if(joy[0].button[5].b)
      draw_sprite(buffer,joystick_button_6,635,276);
    if(joy[0].button[10].b)
      draw_sprite(buffer,joystick_button_7,694,217);
    if(joy[0].button[11].b)
      draw_sprite(buffer,joystick_button_8,764,222);
  }
  draw_sprite(buffer,cursor,mouse_x,mouse_y);
  draw_sprite(screen,buffer,0,0);
}

// Setup game
void setup(){
  // Create buffer
  buffer = create_bitmap( 1024, 768);

  // Allow transparency
  set_alpha_blender();

  // Init random number generator
  srand(time(NULL));

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
  if (!(icon_steam = load_bitmap("icons/icon_steam.png", NULL)))
    abort_on_error("Cannot find image icons/icon_steam.png\nPlease check your files and try again");
  if (!(icon_csgo = load_bitmap("icons/icon_csgo.png", NULL)))
    abort_on_error("Cannot find image icons/icon_csgo.png\nPlease check your files and try again");
  if (!(icon_garrysmod = load_bitmap("icons/icon_garrysmod.png", NULL)))
    abort_on_error("Cannot find image icons/icon_garrysmod.png\nPlease check your files and try again");
  if (!(icon_dayofdefeat = load_bitmap("icons/icon_dayofdefeat.png", NULL)))
    abort_on_error("Cannot find image icons/icon_dayofdefeat.png\nPlease check your files and try again");
  if (!(icon_mame = load_bitmap("icons/icon_mame.png", NULL)))
    abort_on_error("Cannot find image icons/icon_mame.png\nPlease check your files and try again");
  if (!(icon_lol = load_bitmap("icons/icon_lol.png", NULL)))
    abort_on_error("Cannot find image icons/icon_lol.png\nPlease check your files and try again");
  if (!(icon_joystick = load_bitmap("icons/icon_joystick.png", NULL)))
    abort_on_error("Cannot find image icons/icon_joystick.png\nPlease check your files and try again");
  if (!(cursor = load_bitmap("cursor.png", NULL)))
    abort_on_error("Cannot find image cursor.png\nPlease check your files and try again");

  // Joystick
  if (!(joystick_background= load_bitmap("joystick/joystick_background.png", NULL)))
    abort_on_error("Cannot find image joystick/joystick_background.png\nPlease check your files and try again");
  if (!(joystick_button_1 = load_bitmap("joystick/joystick_button_1.png", NULL)))
    abort_on_error("Cannot find image joystick/joystick_button_1.png\nPlease check your files and try again");
  if (!(joystick_button_2 = load_bitmap("joystick/joystick_button_2.png", NULL)))
    abort_on_error("Cannot find image joystick/joystick_button_2.png\nPlease check your files and try again");
  if (!(joystick_button_3 = load_bitmap("joystick/joystick_button_3.png", NULL)))
    abort_on_error("Cannot find image joystick/joystick_button_3.png\nPlease check your files and try again");
  if (!(joystick_button_4 = load_bitmap("joystick/joystick_button_4.png", NULL)))
    abort_on_error("Cannot find image joystick/joystick_button_4.png\nPlease check your files and try again");
  if (!(joystick_button_5 = load_bitmap("joystick/joystick_button_5.png", NULL)))
    abort_on_error("Cannot find image joystick/joystick_button_5.png\nPlease check your files and try again");
  if (!(joystick_button_6 = load_bitmap("joystick/joystick_button_6.png", NULL)))
    abort_on_error("Cannot find image joystick/joystick_button_6.png\nPlease check your files and try again");
  if (!(joystick_button_7 = load_bitmap("joystick/joystick_button_7.png", NULL)))
    abort_on_error("Cannot find image joystick/joystick_button_7.png\nPlease check your files and try again");
  if (!(joystick_button_8 = load_bitmap("joystick/joystick_button_8.png", NULL)))
    abort_on_error("Cannot find image joystick/joystick_button_8.png\nPlease check your files and try again");

  // Load fonts
  f1 = load_font("arimo_22.pcx", NULL, NULL);
  f2 = extract_font_range(f1, ' ', 'A'-1);
  f3 = extract_font_range(f1, 'A', 'Z');
  f4 = extract_font_range(f1, 'Z'+1, 'z');
  arimo_22 = merge_fonts(f4, f5 = merge_fonts(f2, f3));

  // Destroy temporary fonts
  destroy_font(f1);
  destroy_font(f2);
  destroy_font(f3);
  destroy_font(f4);
  destroy_font(f5);

  game[1].path="C:\\Program Files (x86)\\Steam\\steam.exe";
  game[1].name="Steam Client";
  game[1].icon=icon_steam;
  game[1].y=200;
  game[1].x=712;

  game[2].path="steam://rungameid/730";
  game[2].name="Counter-strike: Global Offensive";
  game[2].icon=icon_csgo;
  game[2].x=1012;
  game[2].y=200;

  game[3].path="steam://rungameid/4000";
  game[3].name="Garry's Mod";
  game[3].icon=icon_garrysmod;
  game[3].x=1312;
  game[3].y=200;

  game[4].path="steam://rungameid/30";
  game[4].name="Day of Defeat: Classic";
  game[4].icon=icon_dayofdefeat;
  game[4].x=1612;
  game[4].y=200;

  game[5].path="poop";
  game[5].name="MAME";
  game[5].icon=icon_mame;
  game[5].x=1912;
  game[5].y=200;

  game[6].path="C:\\Riot Games\\League of Legends\\lol.launcher.exe";
  game[6].name="League of Legends";
  game[6].icon=icon_lol;
  game[6].x=2212;
  game[6].y=200;

  game[7].path="crap";
  game[7].name="Joystick";
  game[7].icon=icon_joystick;
  game[7].x=2512;
  game[7].y=200;
}

// MAIN loop
int main(){
  // Setup allegro
  allegro_init();
  alpng_init();
  install_timer();
  install_keyboard();
  install_mouse();
  set_color_depth(32);
  install_joystick(JOY_TYPE_AUTODETECT);

  // Init screen
  set_gfx_mode(GFX_AUTODETECT_WINDOWED,1024,768, 0, 0);
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
