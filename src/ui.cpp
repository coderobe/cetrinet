#include "ui.h"

using namespace std;
using json = nlohmann::json;

atomic<bool> ui_active = false;

LCUI_Widget w_tabs;
LCUI_Widget w_greeter;
LCUI_Widget w_game;

void onConnectButton(LCUI_Widget self, LCUI_WidgetEvent event, void* arg){
  LCUI_Widget server_widget = LCUIWidget_GetById("input-data-server");
  LCUI_Widget port_widget = LCUIWidget_GetById("input-data-port");
  LCUI_Widget username_widget = LCUIWidget_GetById("input-data-username");

  // TODO: use actual size
  size_t bufsize = 256;
  wchar_t t_server[bufsize];
  TextEdit_GetTextW(server_widget, 0, bufsize-1, t_server);
  wchar_t t_port[bufsize];
  TextEdit_GetTextW(port_widget, 0, bufsize-1, t_port);
  wchar_t t_username[256];
  TextEdit_GetTextW(username_widget, 0, bufsize-1, t_username);

  server = util::wctos(t_server);
  port = util::wctos(t_port);
  username = util::wctos(t_username);
  util::thread_start_net();
}

void onConnectPortInput(LCUI_Widget self, LCUI_WidgetEvent event, void* arg){
  /*
  LCUI_Widget inputw = LCUIWidget_GetById("input-data-port");
  size_t bufsize = TextEdit_GetTextLength(inputw);
  wchar_t msgbuffer[bufsize];
  TextEdit_GetTextW(inputw, 0, bufsize, msgbuffer);
  char cmsgbuffer[bufsize];
  wcstombs(cmsgbuffer, msgbuffer, bufsize);
  string smsgbuffer = cmsgbuffer;
  size_t msglen = smsgbuffer.length();
  smsgbuffer.erase(remove_if(smsgbuffer.begin(), smsgbuffer.end(), &util::is_not_digit), smsgbuffer.end());
  */
  
  //TODO: hint when port not numerical or out of range
}

void onChatTextInput(LCUI_Widget self, LCUI_WidgetEvent event, void* arg){
  char in;
  util::get_char_from_textinput_event(event, &in);

  if(in == '\n'){
    wchar_t msgbuffer[1024];
    LCUI_Widget inputw = LCUIWidget_GetById("chatarea-input");
    TextEdit_GetTextW(inputw, 0, 1024, msgbuffer);
    TextEdit_ClearText(inputw);

    char cmsgbuffer[1024];
    wcstombs(cmsgbuffer, msgbuffer, 1024);
    proto::cmsg cmsg = proto::cmsg();
    cmsg.target = "#lobby";
    cmsg.message = cmsgbuffer;

    net_send(json::to_msgpack(cmsg.encode()));
  }
}

void onUIExit(LCUI_SysEvent event, void* args){
  ui_active = false;

  // give other threads time to finish their cycles before cleaning up
  // TODO: mutex!
  this_thread::sleep_for(chrono::milliseconds(100));
}

void ui_chat_message_add_raw(string message, string type){
  if(ui_active){
    LCUI_Widget chat = LCUIWidget_GetById("chatarea-output");
    LCUI_Widget message_widget = LCUIWidget_New("alert");
    Widget_AddClass(message_widget, "alert");
    char buffer[100];
    snprintf(buffer, 100, "alert-%s", type.c_str());
    Widget_AddClass(message_widget, buffer);
    wstring message_wide = wstring(message.begin(), message.end());
    TextView_SetTextW(message_widget, message_wide.c_str());
    Widget_Append(chat, message_widget);
  }
}

void ui_chat_scroller_worker(){
  while(!ui_active){ // wait for UI
    this_thread::sleep_for(chrono::milliseconds(30));
  }

  LCUI_Widget scrollbar = LCUIWidget_GetById("chatarea-scrollbar");
  size_t pos = ScrollBar_GetPosition(scrollbar);
  while(ui_active){
    ScrollBar_SetPosition(scrollbar, pos+1);
    if(pos == ScrollBar_GetPosition(scrollbar)){
      this_thread::sleep_for(chrono::milliseconds(50));
    }else{
      this_thread::sleep_for(chrono::milliseconds(1));
    }
    pos = ScrollBar_GetPosition(scrollbar);
  }
}

void ui_populate_fields(){
  if(ui_active){
    // playfields
    for(size_t playernum = 1; playernum < 10; playernum++){
      char buffer[100];
      snprintf(buffer, 100, "playfield-%ld", playernum);
      LCUI_Widget field = LCUIWidget_GetById(buffer);
      for(size_t row_n = 0; row_n < 20; row_n++){
        LCUI_Widget row = LCUIWidget_New(NULL);
        Widget_AddClass(row, "row");
        for(size_t col_n = 0; col_n < 12; col_n++){
          ui_add_tile(row);
        }
        Widget_Append(field, row);
      }
    }
    // previews
    for(size_t previewnum = 1; previewnum < 7; previewnum++){
      char buffer[100];
      snprintf(buffer, 100, "previewfield-%ld", previewnum);
      LCUI_Widget field = LCUIWidget_GetById(buffer);
      for(size_t row_n = 0; row_n < 6; row_n++){
        LCUI_Widget row = LCUIWidget_New(NULL);
        Widget_AddClass(row, "row");
        for(size_t col_n = 0; col_n < 6; col_n++){
          ui_add_tile(row);
        }
        Widget_Append(field, row);
      }
    }
  }
}

void ui_add_tile(LCUI_Widget parent){
  LCUI_Widget tile = LCUIWidget_New("textview");
  Widget_AddClass(tile, "playtile");
  Widget_Append(parent, tile);
}

void ui_show_game(bool show){
  if(ui_active){
    if(show){
      Widget_Hide(w_greeter);
      Widget_Show(w_tabs);
      Widget_Show(w_game);
    }else{
      Widget_Hide(w_game);
      Widget_Hide(w_tabs);
      Widget_Show(w_greeter);    
    }
  }
}

void ui_worker(){
  LCUI_Widget root, pack;

  util::stdout_silence();
  LCUI_Init();
  LCUIEx_Init();
  util::stdout_unsilence();

  const char* raw_xml_main =
    #include "../assets/main.xml"
  ;

  root = LCUIWidget_GetRoot();
  pack = LCUIBuilder_LoadString(raw_xml_main, string(raw_xml_main).size());

  if(!pack)
    terminate();

  LCUIDisplay_SetSize(860, 720);
  Widget_SetTitleW(root, L"cetrinet");
  Widget_Append(root, pack);
  Widget_Unwrap(pack);

  const char* raw_css_lcui =
    #include "lcui.css"
  ;
  const char* raw_css_main =
    #include "main.css"
  ;

  LCUI_LoadCSSString(raw_css_lcui, NULL);
  LCUI_LoadCSSString(raw_css_main, NULL);

  ui_active = true;

  ui_populate_fields();
  ui_chat_message_add_raw("Welcome to cetrinet", "success");

  LCUI_Widget connectButton = LCUIWidget_GetById("input-connect");
  Widget_BindEvent(connectButton, "click", onConnectButton, NULL, NULL);
  Widget_BindEvent(LCUIWidget_GetById("chatarea-input"), "textinput", onChatTextInput, NULL, NULL);
  Widget_BindEvent(LCUIWidget_GetById("input-data-port"), "textinput", onConnectPortInput, NULL, NULL);

  LCUI_BindEvent(LCUI_QUIT, onUIExit, NULL, NULL);

  w_tabs = LCUIWidget_GetById("tabs");
  w_greeter = LCUIWidget_GetById("greeter");
  w_game = LCUIWidget_GetById("game");

  ui_show_game(false);

  LCUI_Main();
}