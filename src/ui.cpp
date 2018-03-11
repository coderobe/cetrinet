#include "ui.h"

using namespace std;
using json = nlohmann::json;

string ui_get_font_path(const char* name){
  FcConfig* config = FcInitLoadConfigAndFonts();
  FcPattern* pattern = FcNameParse((const FcChar8*)name);
  FcConfigSubstitute(config, pattern, FcMatchPattern);
  FcDefaultSubstitute(pattern);
  FcResult result;
  FcPattern* font = FcFontMatch(config, pattern, &result);
  if(font){
    FcChar8* font_path = NULL;
    if(FcPatternGetString(font, FC_FILE, 0, &font_path) == FcResultMatch){
      return string((const char*)font_path);
    }
  }
  FcPatternDestroy(font);
  FcPatternDestroy(pattern);
  FcConfigDestroy(config);
  return string("");
}

bool ui_running(){
  return window.isOpen();
}

template <typename T>
void ui_gui_set_font(T gui, string font){
  #ifndef WINDOWS
    tgui::Font font_data(ui_get_font_path((const char*)font.c_str()).c_str());
    gui->setFont(font_data);
  #endif
}

void onTabSelected(tgui::Gui& gui, string tab){
  if(tab == "Server"){
    gui.get("panel_server")->show();
    gui.get("panel_channel")->hide();
  }else{
    gui.get("panel_server")->hide();
    gui.get("panel_channel")->show();
  }
}

void ui_worker(){
  window.setFramerateLimit(60);
  static tgui::Gui gui(window);

  sf::Color color_white = sf::Color(255, 255, 255, 255);
  sf::Color color_black = sf::Color(0, 0, 0, 255);
  sf::Color color_grey = sf::Color(200, 200, 200, 255);
  sf::Color color_red = sf::Color(255, 0, 0, 255);
  sf::Color color_green = sf::Color(0, 255, 0, 255);
  sf::Color color_blue = sf::Color(0, 0, 255, 255);

  try{
    ui_gui_set_font(&gui, "sans");

    size_t font_size = 14;
    size_t padding = 4;

    auto menubar = tgui::MenuBar::create();
    menubar->setSize(window.getSize().x, 20);
    menubar->addMenu("Connect");
    menubar->addMenu("About");
    gui.add(menubar);

    auto tabs = tgui::Tab::create();
    tabs->setTabHeight(20);
    tabs->setPosition(padding+2, 25);
    tabs->setTextSize(font_size);
    tabs->add("Server");
    tabs->add("#lobby");
    tabs->select("Server");
    tabs->connect("TabSelected", onTabSelected, std::ref(gui));
    gui.add(tabs);

    size_t panel_offset = 50;

    auto panel_server = tgui::Panel::create();
    panel_server->setSize(window.getSize().x-(padding*2), window.getSize().y-panel_offset-padding);
    panel_server->setPosition(padding, panel_offset);
    gui.add(panel_server, "panel_server");

    auto panel_server_chat = tgui::ListBox::create();
    panel_server_chat->setSize(panel_server->getSize().x, panel_server->getSize().y*0.96);
    panel_server_chat->setPosition(0, 0);
    panel_server_chat->setTextSize(font_size);
    ui_gui_set_font(panel_server_chat, "monospace");
    panel_server->add(panel_server_chat);
    auto panel_server_chat_input = tgui::EditBox::create();
    panel_server_chat_input->setSize(panel_server->getSize().x, panel_server->getSize().y*0.04);
    panel_server_chat_input->setPosition(0, panel_server_chat->getSize().y);
    panel_server_chat_input->setTextSize(font_size);
    ui_gui_set_font(panel_server_chat_input, "monospace");
    panel_server->add(panel_server_chat_input);

    //TODO: remove
    panel_server_chat->addItem("Welcome to cetrinet");

    //TODO: dynamic creation
    auto panel_channel = tgui::Panel::create();
    panel_channel->setBackgroundColor(color_white);
    panel_channel->setSize(window.getSize().x-(padding*2), window.getSize().y-panel_offset-padding);
    panel_channel->setPosition(padding, panel_offset);
    panel_channel->hide();
    gui.add(panel_channel, "panel_channel");

    size_t game_tile_size = 20;
    size_t game_row_w = 12;
    size_t game_row_h = 20;
    size_t border_weight = 1;
    size_t game_secondary_tile_size = 10;
    size_t game_field_main_box_tiles = 6;


    auto game_field_main = tgui::Panel::create();
    auto game_field_main_label_box = tgui::Panel::create();
    auto game_field_main_hold = tgui::Panel::create();

    game_field_main_hold->setBackgroundColor(color_black);
    game_field_main_hold->setSize(game_secondary_tile_size*game_field_main_box_tiles+border_weight*(game_field_main_box_tiles+3), bindHeight(game_field_main));
    game_field_main_hold->setPosition(0, 0);
    panel_channel->add(game_field_main_hold, "game_field_main_hold");

    auto game_field_main_hold_label_box = tgui::Panel::create();
    game_field_main_hold_label_box->setBackgroundColor(color_white);
    game_field_main_hold_label_box->setSize(bindWidth(game_field_main_hold)-border_weight*2, bindHeight(game_field_main_label_box));
    game_field_main_hold_label_box->setPosition(border_weight, border_weight);
    game_field_main_hold->add(game_field_main_hold_label_box);

    auto game_field_main_hold_label = tgui::Label::create();
    game_field_main_hold_label->setText("Hold");
    game_field_main_hold_label->setPosition(border_weight, border_weight);
    game_field_main_hold_label->setTextSize(font_size);
    game_field_main_hold_label_box->add(game_field_main_hold_label);

    for(size_t i = 0; i < 6; i++){
      auto hold_box = tgui::Panel::create();
      hold_box->setBackgroundColor(color_grey);
      hold_box->setSize(
        game_secondary_tile_size*game_field_main_box_tiles+border_weight*game_field_main_box_tiles,
        game_secondary_tile_size*game_field_main_box_tiles+border_weight*game_field_main_box_tiles
      );
      tgui::Layout l_height = border_weight+bindHeight(game_field_main_label_box);
      tgui::Layout l_offset = ((bindHeight(game_field_main_hold)-l_height)/6.0f)-bindHeight(hold_box)-(padding/7.0f);
      if(i > 0){
        size_t l_i = i;
        while(l_i > 0){
          l_height += bindHeight(game_field_main_hold->get("hold_box_"+to_string(l_i-1)))+l_offset;
          l_i--;
        }
      }
      if(i < 6){
        l_height += border_weight;
      }
      hold_box->setPosition(border_weight, l_height);

      for(size_t l_x = 0; l_x < game_field_main_box_tiles; l_x++){
        for(size_t l_y = 0; l_y < game_field_main_box_tiles; l_y++){
          auto game_field_hold_tile = tgui::Panel::create();
          game_field_hold_tile->setBackgroundColor(color_white);
          game_field_hold_tile->setSize(game_secondary_tile_size, game_secondary_tile_size);
          game_field_hold_tile->setPosition(
            border_weight+(border_weight+game_secondary_tile_size)*l_x,
            (border_weight+game_secondary_tile_size)*l_y
          );
          hold_box->add(game_field_hold_tile);
        }
      }
      game_field_main_hold->add(hold_box, "hold_box_"+to_string(i));
    }

    game_field_main->setBackgroundColor(color_black);
    game_field_main->setSize(game_row_w*game_tile_size+border_weight*(game_row_w+1), (1+game_row_h)*game_tile_size+border_weight*(game_row_h+1));
    game_field_main->setPosition(bindRight(game_field_main_hold)+padding, 0);
    panel_channel->add(game_field_main, "game_field_main");

    game_field_main_label_box->setBackgroundColor(color_white);
    game_field_main_label_box->setSize(game_row_w*game_tile_size+(border_weight*(game_row_w-1)), game_tile_size-border_weight);
    game_field_main_label_box->setPosition(border_weight, border_weight);
    game_field_main->add(game_field_main_label_box);

    auto game_field_main_label = tgui::Label::create();
    game_field_main_label->setText("Your Field");
    game_field_main_label->setPosition(border_weight, border_weight);
    game_field_main_label->setTextSize(font_size);
    game_field_main_label_box->add(game_field_main_label);

    auto game_field_main_box = tgui::Panel::create();
    game_field_main_box->setBackgroundColor(color_grey);
    game_field_main_box->setSize(
      bindWidth(game_field_main)-border_weight*2,
      bindHeight(game_field_main)-bindHeight(game_field_main_label_box)-border_weight*2
    );
    game_field_main_box->setPosition(border_weight, border_weight+bindHeight(game_field_main_label_box));
    game_field_main->add(game_field_main_box);

    for(size_t x = 0; x < game_row_w; x++){
      for(size_t y = 0; y < game_row_h; y++){
        auto game_field_main_tile = tgui::Panel::create();
        game_field_main_tile->setBackgroundColor(color_white);
        game_field_main_tile->setSize(game_tile_size, game_tile_size);
        game_field_main_tile->setPosition(border_weight+(border_weight+game_tile_size)*x, border_weight+(border_weight+game_tile_size)*y);
        game_field_main_box->add(game_field_main_tile);
      }
    }

    auto game_field_main_preview = tgui::Panel::create();
    game_field_main_preview->setBackgroundColor(color_black);
    game_field_main_preview->setSize(game_secondary_tile_size*game_field_main_box_tiles+border_weight*(game_field_main_box_tiles+2), bindHeight(game_field_main));
    game_field_main_preview->setPosition(bindRight(game_field_main)+padding, 0);
    panel_channel->add(game_field_main_preview, "game_field_main_preview");

    auto game_field_main_preview_label_box = tgui::Panel::create();
    game_field_main_preview_label_box->setBackgroundColor(color_white);
    game_field_main_preview_label_box->setSize(bindWidth(game_field_main_preview)-border_weight*2, bindHeight(game_field_main_label_box));
    game_field_main_preview_label_box->setPosition(border_weight, border_weight);
    game_field_main_preview->add(game_field_main_preview_label_box);

    auto game_field_main_preview_label = tgui::Label::create();
    game_field_main_preview_label->setText("Next Up");
    game_field_main_preview_label->setPosition(border_weight, border_weight);
    game_field_main_preview_label->setTextSize(font_size);
    game_field_main_preview_label_box->add(game_field_main_preview_label);

    for(size_t i = 0; i < 6; i++){
      auto hold_box = tgui::Panel::create();
      hold_box->setBackgroundColor(color_grey);
      hold_box->setSize(
        game_secondary_tile_size*game_field_main_box_tiles+border_weight*game_field_main_box_tiles,
        game_secondary_tile_size*game_field_main_box_tiles+border_weight*game_field_main_box_tiles
      );
      tgui::Layout l_height = border_weight+bindHeight(game_field_main_label_box);
      tgui::Layout l_offset = ((bindHeight(game_field_main_preview)-l_height)/6.0f)-bindHeight(hold_box)-(padding/7.0f);
      if(i > 0){
        size_t l_i = i;
        while(l_i > 0){
          l_height += bindHeight(game_field_main_preview->get("preview_box_"+to_string(l_i-1)))+l_offset;
          l_i--;
        }
      }
      if(i < 6){
        l_height += border_weight;
      }
      hold_box->setPosition(border_weight, l_height);

      for(size_t l_x = 0; l_x < game_field_main_box_tiles; l_x++){
        for(size_t l_y = 0; l_y < game_field_main_box_tiles; l_y++){
          auto game_field_hold_tile = tgui::Panel::create();
          game_field_hold_tile->setBackgroundColor(color_white);
          game_field_hold_tile->setSize(game_secondary_tile_size, game_secondary_tile_size);
          game_field_hold_tile->setPosition(
            border_weight+(border_weight+game_secondary_tile_size)*l_x,
            (border_weight+game_secondary_tile_size)*l_y
          );
          hold_box->add(game_field_hold_tile);
        }
      }
      game_field_main_preview->add(hold_box, "preview_box_"+to_string(i));
    }

    auto game_field_secondary = tgui::Panel::create();
    game_field_secondary->setBackgroundColor(color_blue);
    game_field_secondary->setSize((game_row_w*game_secondary_tile_size+border_weight*(game_row_w+1))*4, (game_row_h*game_secondary_tile_size+border_weight*game_row_h)*2+1);
    game_field_secondary->setPosition(bindRight(game_field_main_preview)+padding, 0);
    panel_channel->add(game_field_secondary, "game_field_secondary");

    for(size_t player_id = 2; player_id <= 9; player_id++){
      auto game_field_player_secondary = tgui::Panel::create();
      game_field_player_secondary->setBackgroundColor(color_red);
    }

    auto panel_channel_chat_box = tgui::Panel::create();
    panel_channel_chat_box->setSize(bindWidth(panel_channel), bindHeight(panel_channel)-bindHeight(game_field_main));
    panel_channel_chat_box->setPosition(0, bindBottom(game_field_main)+border_weight+padding);
    panel_channel->add(panel_channel_chat_box);
    
    auto panel_channel_chat = tgui::ListBox::create();
    panel_channel_chat->setSize(bindWidth(panel_channel_chat_box), bindHeight(panel_channel_chat_box)*0.85);
    panel_channel_chat->setPosition(0, 0);
    panel_channel_chat->setTextSize(font_size);
    ui_gui_set_font(panel_channel_chat, "monospace");
    panel_channel_chat_box->add(panel_channel_chat);

    //TODO: remove
    panel_channel_chat->addItem("Welcome to cetrinet");
    
    auto panel_channel_chat_input = tgui::EditBox::create();
    panel_channel_chat_input->setSize(bindWidth(panel_channel_chat_box), bindHeight(panel_channel_chat_box)*0.15);
    panel_channel_chat_input->setPosition(0, bindBottom(panel_channel_chat));
    panel_channel_chat_input->setTextSize(font_size);
    ui_gui_set_font(panel_channel_chat_input, "monospace");
    panel_channel_chat_box->add(panel_channel_chat_input);

  }catch(const tgui::Exception& e){
    cerr << "TGUI Exception: " << e.what() << endl;
  }

  while(window.isOpen()){
    sf::Event event;
    while(window.pollEvent(event)){
      if(event.type == sf::Event::Closed){
        window.close();
      }
      gui.handleEvent(event);
    }
    window.clear(color_white);
    gui.draw();
    window.display();
  }
}