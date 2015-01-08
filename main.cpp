#include<allegro.h>
#include<alpng.h>
#include<time.h>
#include<windows.h>
#include<shellapi.h>
#include<fstream>
#include<sstream>

using namespace std;

BITMAP* buffer;
BITMAP* cursor;

BITMAP* icon_steam;
BITMAP* icon_csgo;
BITMAP* icon_garrysmod;
BITMAP* icon_dayofdefeat;

bool close_button_pressed;

// FPS System
volatile int ticks = 0;
const int updates_per_second = 60;
volatile int game_time = 0;

int fps;
int frames_done;
int old_time;
int game_focus=1;
int step;

int settings[5];

int background_r=255;
int background_g=200;
int background_b=200;

struct game{
  int x;
  int y;
  int internal_id;
  bool is_steamgame;
  int steam_id;
  string path;
  BITMAP* icon;
}game[100];

void ticker(){
  ticks++;
}
END_OF_FUNCTION(ticker)

void game_time_ticker(){
  game_time++;
}
END_OF_FUNCTION(ticker)

void close_button_handler(void){
  close_button_pressed = TRUE;
}
END_OF_FUNCTION(close_button_handler)


//Convert string to int
int convertStringToInt(string newString){
  int result;
  stringstream(newString) >> result;
  return result;
}

//Area clicked
bool location_clicked(int min_x,int max_x,int min_y,int max_y){
    if(mouse_x>min_x && mouse_x<max_x && mouse_y>min_y && mouse_y<max_y && mouse_b & 1 || mouse_x>min_x && mouse_x<max_x && mouse_y>min_y && mouse_y<max_y && joy[0].button[1].b)
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


//A function to streamline error reporting in file loading
void abort_on_error(const char *message){
	 set_window_title("Error!");
	 if (screen != NULL){
	    set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
	 }
	 allegro_message("%s.\n %s\n", message, allegro_error);
	 exit(-1);
}

void write_settings(){
    settings[1]=game[1].steam_id;
    settings[2]=game[1].is_steamgame;
    settings[3]=game[1].x;

    ofstream settings_file;
    settings_file.open("games/game_1.txt");

    for (int i = 0; i < 4; i++){
        settings_file<<settings[i]<<" ";
    }
    settings_file<<game[1].path;

    settings_file.close();

}

void read_settings(){
    ifstream read("games/game_1.dat");
    for (int i = 0; i < 4; i++){
        read>>settings[i];
    }
    read>>game[1].path;
    read.close();

    game[1].steam_id=settings[1];
    game[1].is_steamgame=settings[2];
    game[1].x=settings[3];

}


void update(){

  if(location_clicked(412,612,200,400) && step>9){
    if(game_focus==2)ShellExecute(NULL, "open", "steam://rungameid/730", NULL, NULL, SW_SHOWDEFAULT);
    if(game_focus==1)ShellExecute(NULL, "open", "C:\\Program Files (x86)\\Steam\\steam.exe", NULL, NULL, SW_SHOWDEFAULT);
    if(game_focus==3)ShellExecute(NULL, "open", "steam://rungameid/4000", NULL, NULL, SW_SHOWDEFAULT);
    if(game_focus==4)ShellExecute(NULL, "open", "steam://rungameid/30", NULL, NULL, SW_SHOWDEFAULT);

    step=0;
  }
  if(location_clicked(0,400,0,SCREEN_H) && step>9){
    step=0;
    game_focus--;
  }

  if(location_clicked(SCREEN_W-400,SCREEN_W,0,SCREEN_H) && step>9){
    step=0;
    game_focus++;
  }
  background_r+=random(-1,1);
  if(background_r>255)background_r=255;
  if(background_r<200)background_r=200;
  background_b+=random(-1,1);
  if(background_b>255)background_b=255;
  if(background_b<200)background_b=200;
  background_g+=random(-1,1);
  if(background_g>255)background_g=255;
  if(background_g<200)background_g=200;

  if(key[KEY_P])write_settings();
  if(key[KEY_O])read_settings();

  step++;

}

void draw(){
    rectfill(buffer,0,0,1024,768,makecol(background_r,background_g,background_b));
    rect(buffer,512,0,512,768,makecol(0,0,0));
    draw_sprite(buffer,game[1].icon,game[1].x-(game_focus*300),game[1].y);
    draw_sprite(buffer,game[2].icon,game[2].x-(game_focus*300),game[2].y);
    draw_sprite(buffer,game[3].icon,game[3].x-(game_focus*300),game[3].y);
    draw_sprite(buffer,game[4].icon,game[4].x-(game_focus*300),game[4].y);

    draw_sprite(buffer,cursor,mouse_x,mouse_y);
    draw_sprite(screen,buffer,0,0);
}







void setup(){
    buffer=create_bitmap(1024,768);





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

  if (!(icon_steam = load_bitmap("icons/icon_steam.png", NULL)))
      abort_on_error("Cannot find image icons/icon_steam.png\nPlease check your files and try again");
  if (!(icon_csgo = load_bitmap("icons/icon_csgo.png", NULL)))
      abort_on_error("Cannot find image icons/icon_csgo.png\nPlease check your files and try again");
  if (!(icon_garrysmod = load_bitmap("icons/icon_garrysmod.png", NULL)))
      abort_on_error("Cannot find image icons/icon_garrysmod.png\nPlease check your files and try again");
  if (!(icon_dayofdefeat = load_bitmap("icons/icon_dayofdefeat.png", NULL)))
      abort_on_error("Cannot find image icons/icon_dayofdefeat.png\nPlease check your files and try again");
  if (!(cursor = load_bitmap("cursor.png", NULL)))
      abort_on_error("Cannot find image cursor.png\nPlease check your files and try again");

    game[1].path="C:\\Program Files (x86)\\Steam\\steam.exe";
    game[1].is_steamgame=false;
    game[1].icon=icon_steam;
    game[1].y=200;
    game[1].x=712;

    game[2].path="C:\\Program Files (x86)\\Steam\\steam.exe";
    game[2].is_steamgame=true;
    game[2].icon=icon_csgo;
    game[2].x=1012;
    game[2].y=200;

    game[3].path="C:\\Program Files (x86)\\Steam\\steam.exe";
    game[3].is_steamgame=true;
    game[3].icon=icon_garrysmod;
    game[3].x=1312;
    game[3].y=200;

    game[4].path="C:\\Program Files (x86)\\Steam\\steam.exe";
    game[4].is_steamgame=true;
    game[4].icon=icon_dayofdefeat;
    game[4].x=1612;
    game[4].y=200;


}






int main(){

  allegro_init();
  alpng_init();
  install_timer();
  install_keyboard();
  install_mouse();
  set_color_depth(32);


  set_gfx_mode(GFX_AUTODETECT_WINDOWED,1024,768, 0, 0);
  install_sound(DIGI_AUTODETECT,MIDI_AUTODETECT,".");



  set_window_title("Arcade GUI");
  setup();


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
