#include "Menu.h"

// Game icons
typedef struct{
  bool exists;
  int x;
  int y;
  string path;
  string name;
  string icon_path;

  BITMAP *icon;
} game;

game games[MAX_NUMBER_GAMES];

// Area clicked
bool Menu::location_clicked( int min_x, int max_x, int min_y, int max_y){
  if(mouse_x >= min_x && mouse_x <= max_x && mouse_y >= min_y && mouse_y <= max_y && (mouse_b & 1 || joy[0].button[1].b))
    return true;
  else return false;
}

void Menu::load_games( char *file){
    //Setup xml datatypes
    xml_document<> doc;
    xml_node<> * root_node;

    ifstream theFile (file);
    vector<char> xml_buffer((istreambuf_iterator<char>(theFile)), istreambuf_iterator<char>());
    xml_buffer.push_back('\0');

    // Parse the buffer using the xml file parsing library into doc
    doc.parse<0>(&xml_buffer[0]);

    // Find our root node
    root_node = doc.first_node("data");

    //Iterates through the whole document.
    //If it finds a game with the same number as
    //the value in the argument, it reads the data.
    int i = 0;
    for (xml_node<> * game_node = root_node->first_node("game"); game_node; game_node = game_node->next_sibling()){
      //Reads the data in the game node, assigning it to the games[] structure
      games[i].name = game_node->first_attribute("id")->value();
      games[i].path = game_node->first_attribute("path")->value();
      games[i].icon_path = "images/icons/" + string(game_node->first_attribute("icon")->value());
      games[i].icon = load_bitmap_ex( games[i].icon_path.c_str());
      games[i].exists = true;

      i++;
    }
    existing_games = i;
}

Menu::Menu()
{
  game_focus = 0;
  existing_games = 0;

  // Create buffer
  buffer = create_bitmap( SCREEN_W, SCREEN_H);

  // Setup colours, options are VIBRANT, GREYSCALE, PASTEL, BALANCED and a speed (1 is default)
  main_bg = new color_background( "images/overlay.png", BG_BALANCED, 0.2);

  // Allow transparency
  set_alpha_blender();

  // Init random number generator
  srand(time(NULL));

  // Load games
  load_games("games/games.xml");

  // Gui images
  cursor = load_bitmap_ex( "images/cursor.png");
  overlay_text_temp = load_bitmap_ex( "images/overlay_text.png");

  overlay_text = create_bitmap( SCREEN_W, overlay_text_temp -> h);
  stretch_sprite( overlay_text, overlay_text_temp, 0, 0, SCREEN_W, overlay_text_temp -> h);

  // X and Y Positions
  int i, j;
  for( j = 0, i = 0; i <= existing_games; i++){
    if(games[i].exists){
      j++;
      games[i].x = (SCREEN_W/2 - 100) + (300 * j);
      games[i].y = (SCREEN_H/2 - 100) + SCREEN_H/10;
    }
  }
}

void Menu::update()
{
  // Ask joystick for keys
  poll_joystick();

  // Joystick APP
  if((location_clicked( 442, 837, 322, 717) || joy[0].button[2].b) && step > 9 && icon_transition == 0){
    if(games[game_focus].path == "local.joystick"){
        set_next_state( STATE_JOYSTICK);
    }
    else{
      //ShellExecute(NULL, "open", games[game_focus].path.c_str(), NULL, NULL, SW_SHOWDEFAULT);
      step = 0;
    }
  }

  // Scroll Left
  if((location_clicked( 0, 400, 0, SCREEN_H)|| joy[0].stick[0].axis[0].d1) && step > 9 && icon_transition == 0 && game_focus > 0){
    game_focus--;
    icon_transition = -300;
    step = 0;
  }
  // Scroll Right
  if((location_clicked( SCREEN_W - 400,SCREEN_W, 0, SCREEN_H)|| joy[0].stick[0].axis[0].d2) && step>9 && icon_transition == 0 && game_focus < existing_games - 1){
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

  // Step counter
  step++;

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

void Menu::draw()
{
  // Draw background
  main_bg -> draw( buffer);

  // Title
  draw_trans_sprite( buffer, overlay_text, 0, 50);
  textout_centre_ex( buffer, segoe, games[game_focus].name.c_str(), SCREEN_W/2, 80, makecol(0,0,0), -1);

  // Draw icon (stretched if needed)
  for( int i = 0; i < existing_games; i++){
    // Initial scale is original
    int new_scale = 0;

    // If its the current icon, enlarge it
    if( i == game_focus)
      new_scale = icon_scale;

    // Temporary icon, allows manipulation. Makes it a 200x200 icon with scaling if needed
    BITMAP* newIcon = create_bitmap( 200 + new_scale, 200 + new_scale);

    // Stretches icon it if its in focus
    stretch_sprite( newIcon, games[i].icon, 0, 0, newIcon -> w, newIcon -> h);

    // Draw it with transparency
    draw_trans_sprite( buffer, newIcon, games[i].x - (((game_focus + 1) * 300) - icon_transition) - new_scale/2, games[i].y - new_scale/2);

    // Delete temporary bitmap to free memory
    destroy_bitmap( newIcon);
  }

  // When using joystick, dont show mouse
  if(!hide_mouse)
    draw_trans_sprite( buffer, cursor, mouse_x, mouse_y);

  draw_sprite(screen,buffer,0,0);
}

Menu::~Menu()
{

}
