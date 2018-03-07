#include "ui.h"

using namespace std;
using json = nlohmann::json;

atomic<bool> ui_active = false;

void onConnectButton(LCUI_Widget self, LCUI_WidgetEvent event, void* arg){
  LCUI_Widget server_widget = LCUIWidget_GetById("input-data-server");
  LCUI_Widget port_widget = LCUIWidget_GetById("input-data-port");
  LCUI_Widget username_widget = LCUIWidget_GetById("input-data-username");

  TextEdit_GetTextW(server_widget, 0, 255, server);
  TextEdit_GetTextW(port_widget, 0, 255, port);
  TextEdit_GetTextW(username_widget, 0, 255, username);

  util::thread_start_net(server, port, username);
}

void onChatTextInput(LCUI_Widget self, LCUI_WidgetEvent event, void* arg){
  cout << "onChatTextInput" << endl;
  string tinput = "";
  char buffer = '\0';
  size_t buffer_index = 0;
  while(true){
    wcstombs(&buffer, &event->text.text[buffer_index], 1);
    if(buffer == '\0'){
      break;
    }
    buffer_index++;
    tinput += buffer;
  }

  char in = tinput[tinput.size()-1];
  cout << tinput.size() << endl;
  if(in == '\n'){
    wchar_t msgbuffer[1024];
    auto inputw = LCUIWidget_GetById("chatarea-input");
    TextEdit_GetTextW(inputw, 0, 1024, msgbuffer);
    TextEdit_ClearText(inputw);

    char cmsgbuffer[1024];
    wcstombs(cmsgbuffer, msgbuffer, 1024);
    proto::cmsg cmsg = proto::cmsg();
    cmsg.target = "#lobby";
    cmsg.message = cmsgbuffer;

    net_send(json::to_msgpack(cmsg.encode()));

    wcout << msgbuffer << endl;
  }
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
    this_thread::sleep_for(chrono::milliseconds(10));
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

void ui_worker(){
  LCUI_Widget root, pack;

  util::stdout_silence();
  LCUI_Init();
  LCUIEx_Init();
  ui_active = true;
  util::stdout_unsilence();

  const char* raw_xml_main =
    #include "../assets/main.xml"
  ;

  root = LCUIWidget_GetRoot();
  pack = LCUIBuilder_LoadString(raw_xml_main, string(raw_xml_main).size());

  if(!pack)
    terminate();

  LCUIDisplay_SetSize(859, 690);
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

  ui_populate_fields();

  ui_chat_message_add_raw("Welcome to cetrinet", "success");

  LCUI_Widget connectButton = LCUIWidget_GetById("input-connect");
  Widget_BindEvent(connectButton, "click", onConnectButton, NULL, NULL);
  Widget_BindEvent(LCUIWidget_GetById("chatarea-input"), "textinput", onChatTextInput, NULL, NULL);

  LCUI_Main();
  ui_active = false;
}