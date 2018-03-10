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

void onTabSelected(tgui::Gui& gui, string tab){
  if(tab == "Server"){
    gui.get("panel_server")->show();
    gui.get("panel_channel_lobby")->hide();
  }else if(tab == "#lobby"){
    gui.get("panel_server")->hide();
    gui.get("panel_channel_lobby")->show();
  }
}

void ui_worker(){
  window.setFramerateLimit(60);
  static tgui::Gui gui(window);

  sf::Color color_white = sf::Color(255, 255, 255, 255);
  sf::Color color_black = sf::Color(0, 0, 0, 255);
  sf::Color color_red = sf::Color(255, 0, 0, 255);
  sf::Color color_green = sf::Color(0, 255, 0, 255);
  sf::Color color_blue = sf::Color(0, 0, 255, 255);

  tgui::Font font_sans(ui_get_font_path((const char*)"sans").c_str());
  tgui::Font font_serif(ui_get_font_path((const char*)"serif").c_str());
  tgui::Font font_mono(ui_get_font_path((const char*)"monospace").c_str());

  try{
    gui.setFont(font_sans);

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
    panel_server_chat->setFont(font_mono);
    panel_server->add(panel_server_chat);
    auto panel_server_chat_input = tgui::EditBox::create();
    panel_server_chat_input->setSize(panel_server->getSize().x, panel_server->getSize().y*0.04);
    panel_server_chat_input->setPosition(0, panel_server_chat->getSize().y);
    panel_server_chat_input->setTextSize(font_size);
    panel_server_chat_input->setFont(font_mono);
    panel_server->add(panel_server_chat_input);

    //TODO: remove
    panel_server_chat->addItem("Welcome to cetrinet");

    //TODO: dynamic creation
    auto panel_channel_lobby = tgui::Panel::create();
    panel_channel_lobby->setBackgroundColor(color_white);
    panel_channel_lobby->setSize(window.getSize().x-(padding*2), window.getSize().y-panel_offset-padding);
    panel_channel_lobby->setPosition(padding, panel_offset);
    panel_channel_lobby->hide();
    gui.add(panel_channel_lobby, "panel_channel_lobby");

    size_t game_tile_size = 20;
    size_t game_row_w = 12;
    size_t game_row_h = 20;
    size_t border_weight = 1;
    auto game_field_main = tgui::Panel::create();
    game_field_main->setBackgroundColor(color_black);
    game_field_main->setSize(game_row_w*game_tile_size+border_weight*(game_row_w+1), (1+game_row_h)*game_tile_size+border_weight*(game_row_h+1));
    game_field_main->setPosition(0, 0);
    panel_channel_lobby->add(game_field_main, "game_field_main");

    auto game_field_main_label_box = tgui::Panel::create();
    game_field_main_label_box->setBackgroundColor(color_white);
    game_field_main_label_box->setSize(game_row_w*game_tile_size+(border_weight*(game_row_w-1)), game_tile_size-border_weight);
    game_field_main_label_box->setPosition(border_weight, border_weight);
    game_field_main->add(game_field_main_label_box);

    auto game_field_main_label = tgui::Label::create();
    game_field_main_label->setText("Your Field");
    game_field_main_label->setPosition(border_weight, border_weight);
    game_field_main_label->setTextSize(font_size);
    game_field_main_label_box->add(game_field_main_label);

    for(size_t x = 0; x < game_row_w; x++){
      for(size_t y = 0; y < game_row_h; y++){
        auto game_field_main_tile = tgui::Panel::create();
        game_field_main_tile->setBackgroundColor(color_white);
        game_field_main_tile->setSize(game_tile_size, game_tile_size);
        game_field_main_tile->setPosition(border_weight+(border_weight+game_tile_size)*x, (border_weight+game_tile_size)*(y+1));
        game_field_main->add(game_field_main_tile);
      }
    }


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