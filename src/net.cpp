#include "net.h"

using namespace std;
using json = nlohmann::json;

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
        ui_chat_message_add_raw("MOTD: "+payload["d"].value("m", "<error>"), "light");
      }
    };
    net_client->on_open = [](shared_ptr<WsClient::Connection> connection){
      cout << "connection opened" << endl;
      ui_chat_message_add_raw("Connected to server", "success");
    };
    net_client->on_close = [](shared_ptr<WsClient::Connection> connection, int status, const string& reason){
      cout << "connection closed" << endl;
      ui_chat_message_add_raw("Disconnected from server", "danger");
    };
    // See http://www.boost.org/doc/libs/1_55_0/doc/html/boost_asio/reference.html, Error Codes for error code meanings
    net_client->on_error = [](shared_ptr<WsClient::Connection> connection, const SimpleWeb::error_code &ec) {
      cerr << "info " << ec << ", message: " << ec.message() << endl;
      switch(ec.value()){
        case 111: // connection refused
          cout << "ECONNREFUSED" << endl;
          ui_chat_message_add_raw("Connection refused", "danger");
          break;
        case 125: // socket closed mid-operation, this is fine
          cout << "Action canceled" << endl;
        default:
          cout << "Unhandled error" << endl;
          ui_chat_message_add_raw("Unhandled error: "+ec.message(), "danger");
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
