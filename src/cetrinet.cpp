#include <cstdio>
#include <iostream>
#include <unistd.h>
#include <thread>
#include <chrono>
#include <string>

#include <simple-websocket-server/client_ws.hpp>
#include <nlohmann/json.hpp>

#include <LCUI_Build.h>
#include <LCUI/LCUI.h>
#include <LCUI/display.h>
#include <LCUI/gui/widget.h>
#include <LCUI/gui/builder.h>
#include <LCUI/gui/widget/textedit.h>
extern "C" {
  #include <LCUIEx.h>
}

using namespace std;
using WsClient = SimpleWeb::SocketClient<SimpleWeb::WS>;
using json = nlohmann::json;

vector<thread*> threads;
fpos_t stdout_pos;
size_t stdout_fd;
atomic<bool> stdout_silenced = false;

void stdout_silence(){
  if(!stdout_silenced){
    stdout_silenced = true;
    fflush(stdout);
    fgetpos(stdout, &stdout_pos);
    stdout_fd = dup(fileno(stdout));
    dup2(0, fileno(stdout));
  }
}
void stdout_unsilence(){
  if(stdout_silenced){
    stdout_silenced = false;
    fflush(stdout);
    dup2(stdout_fd, fileno(stdout));
    close(stdout_fd);
    fsetpos(stdout, &stdout_pos);
  }
}

wchar_t server[256];
wchar_t username[256];
WsClient* net_client = nullptr;

void net_worker(wchar_t* server, wchar_t* username){
  if(net_client == nullptr){
    wcout << L"connecting to '" << server << L"' as '" << username << "'" << endl;

    char serverpath[256];
    wcstombs(serverpath, server, 256);

    char buffer[256];
    //snprintf(buffer, 256, "%s/", serverpath);
    snprintf(buffer, 256, "localhost:8080/"); // TODO: remove me
    cout << buffer << endl;

    net_client = new WsClient(buffer);
    net_client->on_message = [](shared_ptr<WsClient::Connection> connection, shared_ptr<WsClient::Message> message){
      string msg = string(message->string());
      json payload = json::from_msgpack(msg.data());
      cout << "got message: version " << payload["v"] << ", type " << payload["t"] << endl;
      if(payload["t"] == "motd"){
        cout << "motd: " << payload["d"]["m"] << endl;
      }
    };
    net_client->on_open = [](shared_ptr<WsClient::Connection> connection){
      cout << "connection opened" << endl;
    };
    net_client->on_close = [](shared_ptr<WsClient::Connection> connection, int status, const string& reason){
      cout << "connection closed" << endl;
    };
    // See http://www.boost.org/doc/libs/1_55_0/doc/html/boost_asio/reference.html, Error Codes for error code meanings
    net_client->on_error = [](shared_ptr<WsClient::Connection> connection, const SimpleWeb::error_code &ec) {
      cerr << "info " << ec << ", message: " << ec.message() << endl;
      switch(ec.value()){
        case 111: // connection refused
          cout << "ECONNREFUSED" << endl;
          break;
        case 125: // socket closed mid-operation, this is fine
          cout << "Action canceled" << endl;
        default:
          cout << "Unhandled error" << endl;
      }
    };

    net_client->start();
    delete net_client;
    net_client = nullptr;
  } else {
    net_client->stop();
    while(net_client != nullptr){
      this_thread::sleep_for(chrono::milliseconds(10));
    }
    net_worker(server, username);
  }
}

void onConnectButton(LCUI_Widget self, LCUI_WidgetEvent event, void* arg){
  LCUI_Widget server_widget = LCUIWidget_GetById("input-data-server");
  LCUI_Widget username_widget = LCUIWidget_GetById("input-data-username");

  TextEdit_GetTextW(server_widget, 0, 255, server);
  TextEdit_GetTextW(username_widget, 0, 255, username);

  cout << "Starting net worker" << endl;
  threads.push_back(new thread(net_worker, server, username));
}

void ui_add_tile(LCUI_Widget parent){
  LCUI_Widget tile = LCUIWidget_New("textview");
  Widget_AddClass(tile, "playtile");
  Widget_Append(parent, tile);
}

void ui_populate_fields(){
  // playfields
  for(size_t playernum = 1; playernum < 10; playernum++){
    char buffer[100];
    snprintf(buffer, 100, "playfield-%d", playernum);
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
    snprintf(buffer, 100, "previewfield-%d", previewnum);
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

int ui_worker(){
  LCUI_Widget root, pack;

  stdout_silence();
  LCUI_Init();
  LCUIEx_Init();
  stdout_unsilence();

  root = LCUIWidget_GetRoot();
  pack = LCUIBuilder_LoadFile("main.xml");
  if(!pack)
    return -1;

  LCUIDisplay_SetSize(859, 690);
  Widget_SetTitleW(root, L"cetrinet");
  Widget_Append(root, pack);
  Widget_Unwrap(pack);

  ui_populate_fields();

  LCUI_Widget connectButton = LCUIWidget_GetById("input-connect");
  Widget_BindEvent(connectButton, "click", onConnectButton, NULL, NULL);

  return LCUI_Main();  
}

int main(){
  cout << "Hello, world!" << endl;

  cout << "Starting UI worker" << endl;
  threads.push_back(new thread(ui_worker));

  while(!threads.empty()){
    cout << "Waiting for " << static_cast<size_t>(threads.size()) << " threads" << endl;
    thread* t = threads.front();
    threads.erase(threads.begin());
    threads.shrink_to_fit();
    if(t->joinable()){
      if(net_client != nullptr)
        net_client->stop();
      t->join();
    }
    delete t;
  }

  cout << "Done" << endl;
  return 0;
}
