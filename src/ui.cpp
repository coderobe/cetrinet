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

string ui_channel_current = "#lobby";

void ui_update_chats(){
  tgui::ChatBox::Ptr chat_server = static_pointer_cast<tgui::ChatBox>(gui.get("panel_server_chat", true));
  tgui::ChatBox::Ptr chat_channel = static_pointer_cast<tgui::ChatBox>(gui.get("panel_channel_chat", true));

  chat_server->removeAllLines();
  chat_channel->removeAllLines();
  for(auto msg : server_messages){
    if(msg->to == "server"){
      chat_server->addLine(msg->from+": "+msg->content, {msg->rgb[0], msg->rgb[1], msg->rgb[2]});
    }
    if(msg->to == ui_channel_current || msg->to == "server"){
      chat_channel->addLine(msg->from+": "+msg->content, {msg->rgb[0], msg->rgb[1], msg->rgb[2]});
    }
  }
}

void ui_update_channels(){
  tgui::Tab::Ptr tabs = static_pointer_cast<tgui::Tab>(gui.get("tab_channels", true));

  tabs->removeAll();
  tabs->add("Server");
  for(auto chan : channels){
    if(chan->joined){
      tabs->add(chan->name);
    }
  }
}

void onTabSelected(tgui::Gui& gui, string tab){
  ui_channel_current = tab;
  if(tab == "Server"){
    gui.get("panel_server")->show();
    gui.get("panel_channel")->hide();
  }else{
    gui.get("panel_server")->hide();
    gui.get("panel_channel")->show();
  }
}

void onChatSubmit(tgui::EditBox::Ptr input, string text){
  input->setText("");
  string nt = text;
  nt.erase(remove_if(nt.begin(), nt.end(), ::isspace), nt.end());
  if(nt.length() > 0){
    util::send_message(ui_channel_current, text);
  }
}

void onMenuSelected(tgui::Gui& gui, string menu){
  cout << menu << endl;
  if(menu == "Connect"){
    auto msgbox = tgui::MessageBox::create();
    gui.add(msgbox);
    msgbox->setSize({300, 220});
    msgbox->setPosition(bindWidth(gui)/2-300/2, bindHeight(gui)/2-220/2);

    auto content = tgui::Panel::create();
    msgbox->add(content);
    content->setSize(bindWidth(msgbox), bindHeight(msgbox));
    content->setBackgroundColor(color_white);

    auto label_header = tgui::Label::create();
    content->add(label_header);
    label_header->setPosition(8, 8);
    label_header->setTextSize(18);
    label_header->setText("Connect to Server");

    auto label_server = tgui::Label::create();
    content->add(label_server);
    label_server->setPosition(8, bindBottom(label_header));
    label_server->setTextSize(14);
    label_server->setText("Server");

    auto edit_server = tgui::EditBox::create();
    content->add(edit_server);
    edit_server->setPosition(8, bindBottom(label_server));
    edit_server->setTextSize(14);
    edit_server->setSize(bindWidth(content)-8*2, bindHeight(label_server));
    edit_server->setDefaultText("rrerrware.download");

    auto label_port = tgui::Label::create();
    content->add(label_port);
    label_port->setPosition(8, bindBottom(edit_server)+8);
    label_port->setTextSize(14);
    label_port->setText("Port");

    auto edit_port = tgui::EditBox::create();
    content->add(edit_port);
    edit_port->setPosition(8, bindBottom(label_port));
    edit_port->setTextSize(14);
    edit_port->setSize(bindWidth(content)-8*2, bindHeight(label_port));
    edit_port->setDefaultText("28420");

    auto label_username = tgui::Label::create();
    content->add(label_username);
    label_username->setPosition(8, bindBottom(edit_port)+8);
    label_username->setTextSize(14);
    label_username->setText("Username");

    auto edit_username = tgui::EditBox::create();
    content->add(edit_username);
    edit_username->setPosition(8, bindBottom(label_username));
    edit_username->setTextSize(14);
    edit_username->setSize(bindWidth(content)-8*2, bindHeight(label_username));
    edit_username->setDefaultText("cetrinet user");

    auto button_connect = tgui::Button::create();
    content->add(button_connect);
    button_connect->setPosition(8, bindBottom(edit_username)+8*2);
    button_connect->setTextSize(14);
    button_connect->setSize(bindWidth(content)-8*2, bindHeight(label_header));
    button_connect->setText("Connect");
    button_connect->connect("pressed", [=](){
      msgbox->getParent()->remove(msgbox);
      server = edit_server->getText();
      port = edit_port->getText();
      username = edit_username->getText();
      util::thread_start_net();
    });
  }else if(menu == "Disconnect"){
    net_disconnect();
  }else if(menu == "About"){
    auto msgbox = tgui::MessageBox::create();
    msgbox->setText("Hello, world!");
    gui.add(msgbox);
  }
  window.setActive(true);
}

void ui_handle_disconnect(){
  util::add_message("server", "cetrinet", "Disconnected from server", (unsigned char[3]){100, 0, 0});  
}

void ui_worker(){
  try{
    ui_gui_set_font(&gui, "sans");

    size_t font_size = 14;
    size_t padding = 4;

    auto menubar = tgui::MenuBar::create();
    menubar->setSize(window.getSize().x, 20);
    menubar->addMenu("Connection");
    menubar->addMenuItem("Connect");
    menubar->addMenuItem("Disconnect");
    menubar->addMenu("Help");
    menubar->addMenuItem("About");
    menubar->connect("MenuItemClicked", onMenuSelected, std::ref(gui));
    gui.add(menubar);

    auto tabs = tgui::Tab::create();
    gui.add(tabs, "tab_channels");
    tabs->setTabHeight(20);
    tabs->setPosition(padding+2, 25);
    tabs->setTextSize(font_size);
    ui_update_channels();
    tabs->connect("TabSelected", onTabSelected, std::ref(gui));

    size_t panel_offset = 50;

    auto panel_server = tgui::Panel::create();
    panel_server->setSize(window.getSize().x-(padding*2), window.getSize().y-panel_offset-padding);
    panel_server->setPosition(padding, panel_offset);
    gui.add(panel_server, "panel_server");

    auto panel_server_chat = tgui::ChatBox::create();
    panel_server_chat->setSize(panel_server->getSize().x, panel_server->getSize().y*0.96);
    panel_server_chat->setPosition(0, 0);
    panel_server_chat->setTextSize(font_size);
    ui_gui_set_font(panel_server_chat, "monospace");
    panel_server->add(panel_server_chat, "panel_server_chat");
    auto panel_server_chat_input = tgui::EditBox::create();
    panel_server->add(panel_server_chat_input);
    panel_server_chat_input->connect("ReturnKeyPressed", onChatSubmit, panel_server_chat_input);
    panel_server_chat_input->setSize(panel_server->getSize().x, panel_server->getSize().y*0.04);
    panel_server_chat_input->setPosition(0, panel_server_chat->getSize().y);
    panel_server_chat_input->setTextSize(font_size);
    panel_server_chat_input->setDefaultText("Enter message...");
    ui_gui_set_font(panel_server_chat_input, "monospace");

    //TODO: remove
    panel_server_chat->addLine("Welcome to cetrinet");

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
    game_field_secondary->setBackgroundColor(color_grey);
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
    
    auto panel_channel_chat = tgui::ChatBox::create();
    panel_channel_chat->setSize(bindWidth(panel_channel_chat_box), bindHeight(panel_channel_chat_box)*0.85);
    panel_channel_chat->setPosition(0, 0);
    panel_channel_chat->setTextSize(font_size);
    ui_gui_set_font(panel_channel_chat, "monospace");
    panel_channel_chat_box->add(panel_channel_chat, "panel_channel_chat");
   
    auto panel_channel_chat_input = tgui::EditBox::create();
    panel_channel_chat_box->add(panel_channel_chat_input);
    panel_channel_chat_input->connect("ReturnKeyPressed", onChatSubmit, panel_channel_chat_input);
    panel_channel_chat_input->setSize(bindWidth(panel_channel_chat_box), bindHeight(panel_channel_chat_box)*0.15);
    panel_channel_chat_input->setPosition(0, bindBottom(panel_channel_chat));
    panel_channel_chat_input->setTextSize(font_size);
    panel_channel_chat_input->setDefaultText("Enter message...");
    ui_gui_set_font(panel_channel_chat_input, "monospace");

    menubar->moveToFront();

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