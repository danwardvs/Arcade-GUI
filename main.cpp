#include<allegro.h>
#include<alpng.h>
#include<time.h>
#include <windows.h>
#include<shellapi.h>

BITMAP* buffer;
BITMAP* cursor;

BITMAP* icon_steam;
BITMAP* icon_csgo;
BITMAP* icon_garrysmod;

bool close_button_pressed;

// FPS System
volatile int ticks = 0;
const int updates_per_second = 60;
volatile int game_time = 0;

int fps;
int frames_done;
int old_time;
int game_focus;
int step;

struct game{
  int x;
  int y;
  int internal_id;
  int is_steamgame;
  int steam_id;
  char* path;
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

void update(){
  if(key[KEY_C])ShellExecute(NULL, "open", "E:\\Steam Games\\Steam is a piece of shit\\steamapps\\common\\Counter-Strike Global Offensive\\csgo.exe", NULL, NULL, SW_SHOWDEFAULT);
  if(location_clicked(500,700,200,400))ShellExecute(NULL, "open", "steam://rungameid/730", NULL, NULL, SW_SHOWDEFAULT);
  if(location_clicked(800,1000,200,400))ShellExecute(NULL, "open", "steam://rungameid/4000", NULL, NULL, SW_SHOWDEFAULT);


  if(location_clicked(200,400,200,400))ShellExecute(NULL, "open", "C:\\Program Files (x86)\\Steam\\steam.exe", NULL, NULL, SW_SHOWDEFAULT);
  if(location_clicked(0,400,0,SCREEN_H) && step>9){
    step=0;
    game_focus++;
  }

  if(location_clicked(SCREEN_W-400,SCREEN_W,0,SCREEN_H) && step>9){
    step=0;
    game_focus--;
  }

  step++;

}

void draw(){
    rectfill(buffer,0,0,1024,768,makecol(255,255,255));
    draw_sprite(buffer,icon_steam,0+(game_focus*200),200);
    draw_sprite(buffer,icon_csgo,300+(game_focus*200),200);
    draw_sprite(buffer,icon_garrysmod,600+(game_focus*200),200);
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
  if (!(cursor = load_bitmap("cursor.png", NULL)))
      abort_on_error("Cannot find image cursor.png\nPlease check your files and try again");

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
