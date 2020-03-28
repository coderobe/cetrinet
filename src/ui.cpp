#include "ui.h"

using namespace std;
using json = nlohmann::json;

atomic<bool> ui_needs_redraw = true;

string ui_get_font_path(const char* name){
  string font_path_str;
  FcConfig* config = FcInitLoadConfigAndFonts();
  FcPattern* pattern = FcNameParse((const FcChar8*)name);
  FcConfigSubstitute(config, pattern, FcMatchPattern);
  FcDefaultSubstitute(pattern);
  FcResult result;
  FcPattern* font = FcFontMatch(config, pattern, &result);
  if(font){
    FcChar8* font_path = NULL;
    if(FcPatternGetString(font, FC_FILE, 0, &font_path) == FcResultMatch){
      font_path_str = (const char*)font_path;
    }
  }
  FcPatternDestroy(font);
  FcPatternDestroy(pattern);
  FcConfigDestroy(config);
  return font_path_str;
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

string ui_channel_current = "Server";
mutex ui_update_chats_mutex;

void ui_update_chats(){
  lock_guard<mutex> lock(ui_update_chats_mutex);
  tgui::ChatBox::Ptr chat_server = static_pointer_cast<tgui::ChatBox>(gui.get("panel_server_chat"));
  tgui::ChatBox::Ptr chat_channel = static_pointer_cast<tgui::ChatBox>(gui.get("panel_channel_chat"));

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
  ui_needs_redraw = true;
}

void ui_update_channels(){
  tgui::Tabs::Ptr tabs = static_pointer_cast<tgui::Tabs>(gui.get("tab_channels"));

  tabs->removeAll();
  tabs->add("Server");
  for(auto chan : channels){
    if(chan->joined){
      tabs->add(chan->name);
    }
  }
  ui_needs_redraw = true;
}

void ui_update_users(){
  tgui::ListBox::Ptr list = static_pointer_cast<tgui::ListBox>(gui.get("panel_channel_users"));

  for(auto chan : channels){
    if(chan->name == ui_channel_current){
      list->removeAllItems();
      for(auto user : chan->userdata){
        list->addItem(((user->ready) ? "[R] " : "") + user->name, user->name);
      }
    }
  }
  ui_needs_redraw = true;
}

void onTabSelected(tgui::Gui& gui, string tab){
  if(ui_channel_current != tab){
    ui_channel_current = tab;
    if(tab == "Server"){
      gui.get("panel_server")->setVisible(true);
      gui.get("panel_channel")->setVisible(false);
    }else{
      gui.get("panel_server")->setVisible(false);
      gui.get("panel_channel")->setVisible(true);
      ui_update_users();
      ui_update_chats();
    }
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
  if(menu == "Connect"){
    auto msgbox = tgui::MessageBox::create();
    gui.add(msgbox);
    msgbox->setTitle("Connect");
    msgbox->setTitleButtons(tgui::ChildWindow::TitleButton::Close);
    msgbox->setSize({300, 220});
    msgbox->setPosition(bindWidth(gui)/2-300/2, bindHeight(gui)/2-220/2);

    auto content = tgui::Panel::create();
    msgbox->add(content);
    content->setSize(bindWidth(msgbox), bindHeight(msgbox));
    content->getRenderer()->setBackgroundColor(color_white);

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
  }else if(menu == "Join Channel"){
    auto msgbox = tgui::MessageBox::create();
    gui.add(msgbox);
    msgbox->setTitle("Join Channel");
    msgbox->setTitleButtons(tgui::ChildWindow::TitleButton::Close);
    msgbox->setSize({200, 90});
    msgbox->setPosition(bindWidth(gui)/2-200/2, bindHeight(gui)/2-90/2);

    auto content = tgui::Panel::create();
    msgbox->add(content);
    content->setSize(bindWidth(msgbox), bindHeight(msgbox));
    content->getRenderer()->setBackgroundColor(color_white);

    auto label_channame = tgui::Label::create();
    label_channame->setPosition(8, 8);
    label_channame->setTextSize(14);
    label_channame->setText("Channel Name");
    content->add(label_channame);

    auto edit_channel = tgui::EditBox::create();
    edit_channel->setPosition(bindLeft(label_channame), bindBottom(label_channame));
    edit_channel->setTextSize(14);
    edit_channel->setSize(bindWidth(content)-8*2, bindHeight(label_channame));
    edit_channel->setDefaultText("#lobby");
    content->add(edit_channel);

    auto button_join = tgui::Button::create();
    content->add(button_join);
    button_join->setPosition(8, bindBottom(edit_channel)+8*2);
    button_join->setTextSize(14);
    button_join->setSize(bindWidth(content)-8*2, bindHeight(edit_channel));
    button_join->setText("Join");
    button_join->connect("pressed", [=](){
      msgbox->getParent()->remove(msgbox);
      if(edit_channel->getText().getSize() > 0)
        util::join_channel(edit_channel->getText());
      else
        util::add_error_message("No channel name specified");
    });
  }else if(menu == "Leave Channel"){
    if(ui_channel_current != "Server")
      util::leave_channel(ui_channel_current);
  }else if(menu == "About"){
    auto msgbox = tgui::MessageBox::create();
    gui.add(msgbox);
    msgbox->setTitle("About");
    msgbox->setTitleButtons(tgui::ChildWindow::TitleButton::Close);
    msgbox->setPosition(bindWidth(gui)/2-300/2, bindHeight(gui)/2-220/2);
    msgbox->setText("cetrinet by coderobe\n2018-2020\nhttps://coderobe.net");
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
    menubar->addMenu("Channels");
    menubar->addMenuItem("Join Channel");
    menubar->addMenuItem("Leave Channel");
    menubar->addMenu("Help");
    menubar->addMenuItem("About");
    menubar->connect("MenuItemClicked", onMenuSelected, std::ref(gui));
    gui.add(menubar);

    auto tabs = tgui::Tabs::create();
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
    panel_server_chat->setSize(panel_server->getSize().x, panel_server->getSize().y);
    panel_server_chat->setPosition(0, 0);
    panel_server_chat->setTextSize(font_size);
    ui_gui_set_font(panel_server_chat->getRenderer(), "monospace");
    panel_server->add(panel_server_chat, "panel_server_chat");

    panel_server_chat->addLine("Welcome to cetrinet");

    auto panel_channel = tgui::Panel::create();
    panel_channel->getRenderer()->setBackgroundColor(color_white);
    panel_channel->setSize(window.getSize().x-(padding*2), window.getSize().y-panel_offset-padding);
    panel_channel->setPosition(padding, panel_offset);
    panel_channel->setVisible(false);
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

    game_field_main_hold->getRenderer()->setBackgroundColor(color_black);
    game_field_main_hold->setSize(game_secondary_tile_size*game_field_main_box_tiles+border_weight*(game_field_main_box_tiles+3), bindHeight(game_field_main));
    game_field_main_hold->setPosition(0, 0);
    panel_channel->add(game_field_main_hold, "game_field_main_hold");

    auto game_field_main_hold_label_box = tgui::Panel::create();
    game_field_main_hold_label_box->getRenderer()->setBackgroundColor(color_white);
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
      hold_box->getRenderer()->setBackgroundColor(color_grey);
      hold_box->setSize(
        game_secondary_tile_size*game_field_main_box_tiles+border_weight*game_field_main_box_tiles,
        game_secondary_tile_size*game_field_main_box_tiles+border_weight*game_field_main_box_tiles
      );
      tgui::Layout l_height = border_weight+bindHeight(game_field_main_label_box);
      tgui::Layout l_offset = ((bindHeight(game_field_main_hold)-l_height)/6.0f)-bindHeight(hold_box)-(padding/7.0f);
      if(i > 0){
        size_t l_i = i;
        while(l_i > 0){
          l_height = l_height.getValue()+bindHeight(game_field_main_hold->get("hold_box_"+to_string(l_i-1)))+l_offset;
          l_i--;
        }
      }
      if(i < 6){
        l_height = l_height.getValue()+border_weight;
      }
      hold_box->setPosition(border_weight, l_height);

      for(size_t l_x = 0; l_x < game_field_main_box_tiles; l_x++){
        for(size_t l_y = 0; l_y < game_field_main_box_tiles; l_y++){
          auto game_field_hold_tile = tgui::Panel::create();
          game_field_hold_tile->getRenderer()->setBackgroundColor(color_white);
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

    game_field_main->getRenderer()->setBackgroundColor(color_black);
    game_field_main->setSize(game_row_w*game_tile_size+border_weight*(game_row_w+1), (1+game_row_h)*game_tile_size+border_weight*(game_row_h+1));
    game_field_main->setPosition(bindRight(game_field_main_hold)+padding, 0);
    panel_channel->add(game_field_main, "game_field_main");

    game_field_main_label_box->getRenderer()->setBackgroundColor(color_white);
    game_field_main_label_box->setSize(game_row_w*game_tile_size+(border_weight*(game_row_w-1)), game_tile_size-border_weight);
    game_field_main_label_box->setPosition(border_weight, border_weight);
    game_field_main->add(game_field_main_label_box);

    auto game_field_main_label = tgui::Label::create();
    game_field_main_label->setText("Your Field");
    game_field_main_label->setPosition(border_weight, border_weight);
    game_field_main_label->setTextSize(font_size);
    game_field_main_label_box->add(game_field_main_label);

    auto game_field_main_box = tgui::Panel::create();
    game_field_main_box->getRenderer()->setBackgroundColor(color_grey);
    game_field_main_box->setSize(
      bindWidth(game_field_main)-border_weight*2,
      bindHeight(game_field_main)-bindHeight(game_field_main_label_box)-border_weight*2
    );
    game_field_main_box->setPosition(border_weight, border_weight+bindHeight(game_field_main_label_box));
    game_field_main->add(game_field_main_box);

    for(size_t x = 0; x < game_row_w; x++){
      for(size_t y = 0; y < game_row_h; y++){
        auto game_field_main_tile = tgui::Panel::create();
        game_field_main_tile->getRenderer()->setBackgroundColor(color_white);
        game_field_main_tile->setSize(game_tile_size, game_tile_size);
        game_field_main_tile->setPosition(border_weight+(border_weight+game_tile_size)*x, border_weight+(border_weight+game_tile_size)*y);
        game_field_main_box->add(game_field_main_tile);
      }
    }

    auto game_field_main_preview = tgui::Panel::create();
    game_field_main_preview->getRenderer()->setBackgroundColor(color_black);
    game_field_main_preview->setSize(game_secondary_tile_size*game_field_main_box_tiles+border_weight*(game_field_main_box_tiles+2), bindHeight(game_field_main));
    game_field_main_preview->setPosition(bindRight(game_field_main)+padding, 0);
    panel_channel->add(game_field_main_preview, "game_field_main_preview");

    auto game_field_main_preview_label_box = tgui::Panel::create();
    game_field_main_preview_label_box->getRenderer()->setBackgroundColor(color_white);
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
      hold_box->getRenderer()->setBackgroundColor(color_grey);
      hold_box->setSize(
        game_secondary_tile_size*game_field_main_box_tiles+border_weight*game_field_main_box_tiles,
        game_secondary_tile_size*game_field_main_box_tiles+border_weight*game_field_main_box_tiles
      );
      tgui::Layout l_height = border_weight+bindHeight(game_field_main_label_box);
      tgui::Layout l_offset = ((bindHeight(game_field_main_preview)-l_height)/6.0f)-bindHeight(hold_box)-(padding/7.0f);
      if(i > 0){
        size_t l_i = i;
        while(l_i > 0){
          l_height = l_height.getValue()+bindHeight(game_field_main_preview->get("preview_box_"+to_string(l_i-1)))+l_offset;
          l_i--;
        }
      }
      if(i < 6){
        l_height = l_height.getValue()+border_weight;
      }
      hold_box->setPosition(border_weight, l_height);

      for(size_t l_x = 0; l_x < game_field_main_box_tiles; l_x++){
        for(size_t l_y = 0; l_y < game_field_main_box_tiles; l_y++){
          auto game_field_hold_tile = tgui::Panel::create();
          game_field_hold_tile->getRenderer()->setBackgroundColor(color_white);
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

    auto game_items = tgui::Panel::create();
    game_items->getRenderer()->setBackgroundColor(color_grey);
    game_items->setSize(
      bindWidth(game_field_main)+
      bindWidth(game_field_main_preview)+
      bindWidth(game_field_main_hold)+
      padding*2,
      25+padding*2);
    game_items->setPosition(0, bindBottom(game_field_main)+border_weight+padding);
    panel_channel->add(game_items);
    // TODO: Add item boxes

    auto game_controls = tgui::Panel::create();
    game_controls->getRenderer()->setBackgroundColor(color_white);
    game_controls->setSize(bindWidth(panel_channel)-(bindWidth(game_items)+padding), 25+padding*2);
    game_controls->setPosition(bindRight(game_items)+padding, bindTop(game_items));
    panel_channel->add(game_controls);

    auto game_control_ready = tgui::Button::create();
    game_control_ready->setPosition(border_weight*2, border_weight*2);
    game_control_ready->setTextSize(14);
    game_control_ready->setSize(95, bindHeight(game_controls)-border_weight*4);
    game_control_ready->setText("Toggle Ready");
    game_control_ready->connect("pressed", [=](){
      util::toggle_ready(ui_channel_current);
    });
    game_controls->add(game_control_ready);

    auto game_control_start = tgui::Button::create();
    game_control_start->setPosition(bindRight(game_control_ready)+border_weight*2, border_weight*2);
    game_control_start->setTextSize(14);
    game_control_start->setSize(95, bindHeight(game_controls)-border_weight*4);
    game_control_start->setText("Start Game");
    game_control_start->connect("pressed", [=](){
      util::send_gstart(ui_channel_current);
    });
    game_controls->add(game_control_start);

    auto game_control_stop = tgui::Button::create();
    game_control_stop->setPosition(bindRight(game_control_start)+border_weight*2, border_weight*2);
    game_control_stop->setTextSize(14);
    game_control_stop->setSize(95, bindHeight(game_controls)-border_weight*4);
    game_control_stop->setText("Stop Game");
    game_control_stop->connect("pressed", [=](){
      util::send_gstop(ui_channel_current);
    });
    game_controls->add(game_control_stop);

    auto game_field_secondary = tgui::Panel::create();
    game_field_secondary->getRenderer()->setBackgroundColor(color_black);
    game_field_secondary->setSize((game_row_w*game_secondary_tile_size+border_weight*(game_row_w+2))*4+border_weight, (game_row_h*game_secondary_tile_size+border_weight*(game_row_h+1))*2+1);
    game_field_secondary->setPosition(bindRight(game_field_main_preview)+padding, bindTop(game_field_main_preview));
    panel_channel->add(game_field_secondary, "game_field_secondary");

    for(size_t player_id = 2; player_id <= 9; player_id++){
      auto game_field_player_secondary = tgui::Panel::create();
      game_field_player_secondary->getRenderer()->setBackgroundColor(color_grey);

      tgui::Panel::Ptr previous_panel = static_pointer_cast<tgui::Panel>(gui.get("game_field_player_"+to_string(player_id-1)));

      tgui::Layout posh, posv;
      if(player_id == 2){
        posh = border_weight;
        posv = border_weight;
      }else if(player_id % 2){
        posh = bindLeft(previous_panel);
        posv = bindBottom(previous_panel)+border_weight;
      } else {
        posh = bindRight(previous_panel)+border_weight;
        posv = border_weight;
      }

      game_field_player_secondary->setPosition(posh, posv);
      game_field_player_secondary->setSize(game_row_w*game_secondary_tile_size+border_weight*(game_row_w+1), game_row_h*game_secondary_tile_size+border_weight*game_row_h);

      for(size_t x = 0; x < game_row_w; x++){
        for(size_t y = 0; y < game_row_h; y++){
          auto game_field_secondary_tile = tgui::Panel::create();
          game_field_secondary_tile->getRenderer()->setBackgroundColor(color_white);
          game_field_secondary_tile->setSize(game_secondary_tile_size, game_secondary_tile_size);
          game_field_secondary_tile->setPosition(border_weight+(border_weight+game_secondary_tile_size)*x, border_weight+(border_weight+game_secondary_tile_size)*y);
          game_field_player_secondary->add(game_field_secondary_tile);
        }
      }

      auto game_field_secondary_label = tgui::Label::create();
      game_field_secondary_label->setText("No Player");
      game_field_secondary_label->setPosition((bindWidth(game_field_player_secondary)/2)-(bindWidth(game_field_secondary_label)/2),
        (bindHeight(game_field_player_secondary)/2)-(bindHeight(game_field_secondary_label)/2));
      game_field_secondary_label->setTextSize(font_size*1.8);
      game_field_secondary_label->getRenderer()->setOpacity(0.8);
      game_field_player_secondary->add(game_field_secondary_label, "game_field_player_label_"+to_string(player_id));

      game_field_secondary->add(game_field_player_secondary, "game_field_player_"+to_string(player_id));
    }

    auto panel_channel_chat_box = tgui::Panel::create();
    panel_channel_chat_box->setSize(bindWidth(panel_channel), bindHeight(panel_channel)-bindHeight(game_field_main)-bindHeight(game_items)-padding*2.5);
    panel_channel_chat_box->setPosition(0, bindBottom(game_controls)+border_weight+padding);
    panel_channel_chat_box->getRenderer()->setBackgroundColor(color_black);
    panel_channel->add(panel_channel_chat_box);
    
    auto panel_channel_chat = tgui::ChatBox::create();
    panel_channel_chat->setSize(bindWidth(panel_channel_chat_box)*0.8, bindHeight(panel_channel_chat_box)*0.85);
    panel_channel_chat->setPosition(border_weight, border_weight);
    panel_channel_chat->setTextSize(font_size);
    ui_gui_set_font(panel_channel_chat->getRenderer(), "monospace");
    panel_channel_chat_box->add(panel_channel_chat, "panel_channel_chat");
   
    auto panel_channel_chat_input = tgui::EditBox::create();
    panel_channel_chat_box->add(panel_channel_chat_input);
    panel_channel_chat_input->connect("ReturnKeyPressed", onChatSubmit, panel_channel_chat_input);
    panel_channel_chat_input->setSize(bindWidth(panel_channel_chat), bindHeight(panel_channel_chat_box)*0.15);
    panel_channel_chat_input->setPosition(bindLeft(panel_channel_chat), bindBottom(panel_channel_chat)-border_weight);
    panel_channel_chat_input->setTextSize(font_size);
    panel_channel_chat_input->setDefaultText("Enter message...");
    panel_channel_chat_input->setSize(panel_channel_chat_input->getSize().x, panel_channel_chat_input->getSize().y-border_weight);
    ui_gui_set_font(panel_channel_chat_input->getRenderer(), "monospace");

    auto panel_channel_users = tgui::ListBox::create();
    panel_channel_users->setSize(bindWidth(panel_channel_chat_box)*0.2-border_weight*2, bindHeight(panel_channel_chat_box)-border_weight*2);
    panel_channel_users->setPosition(bindRight(panel_channel_chat), bindTop(panel_channel_chat));
    panel_channel_chat_box->add(panel_channel_users, "panel_channel_users");

    menubar->moveToFront();

  }catch(const tgui::Exception& e){
    cerr << "TGUI Exception: " << e.what() << endl;
  }

  onMenuSelected(gui, "Connect");


  const sf::Time TimePerFrame = sf::seconds(1.f/60.f);
  ui_needs_redraw = true;

  while(ui_running()){
    sf::Event event;
    bool ui_has_drawn = false;
    while(window.pollEvent(event)){
      ui_needs_redraw = true;
      if(event.type == sf::Event::Closed){
        window.close();
      }/*else if (event.type == sf::Event::Resized){
        window.setView(sf::View(sf::FloatRect(0.f, 0.f, static_cast<float>(event.size.width), static_cast<float>(event.size.height))));
        gui.setView(window.getView());
      }*/ // Window resizing instead of scaling
      gui.handleEvent(event);
    }
    if(ui_needs_redraw){
      ui_has_drawn = true;
      window.clear(color_white);
      gui.draw();
      window.display();
      ui_needs_redraw = false;
    }
    if(!ui_has_drawn){
      sf::sleep(TimePerFrame);
    }
  }
}