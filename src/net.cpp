#include "net.h"

using namespace std;
using json = nlohmann::json;

shared_ptr<WsClient::Connection> net_connection;

void net_send(vector<unsigned char> data){
  if(net_client != nullptr){
    shared_ptr<WsClient::SendStream> stream = make_shared<WsClient::SendStream>();
    for(unsigned char piece : data){
      *stream << piece;
    }
    net_connection->send(stream, nullptr, 130);
  }
}

void net_worker(){
  if(net_client == nullptr){
    if(server.size() < 1){
      server = "localhost";
    }
    if(port.size() < 1){
      port = "28420";
    }
    if(username.size() < 1){
      username = "TestUser#123"; //TODO: change me
    }

    cout << "connecting to '" << server << "' (port " << port << ") as '" << username << "'" << endl;

    net_client = new WsClient((server+":"+port+"/").c_str());
    net_client->on_message = [](shared_ptr<WsClient::Connection> connection, shared_ptr<WsClient::Message> message){
      string msg = string(message->string());

      json payload = json::from_msgpack(msg);
      cout << "got message: version " << payload["v"] << ", type " << payload["t"] << endl;

      state_update(payload);
    };
    net_client->on_open = [](shared_ptr<WsClient::Connection> connection){
      cout << "connection opened" << endl;
      net_connection = connection;
      username = username.substr(0, username.find("#"));
      //TODO: ui_show_game(true);
      //TODO: ui_chat_message_add_raw("Connected to server", "success");
      proto::auth auth = proto::auth();
      auth.name = username;
      net_send(json::to_msgpack(auth.encode()));
    };
    net_client->on_close = [](shared_ptr<WsClient::Connection> connection, int status, const string& reason){
      cout << "connection closed" << endl;
      //TODO: ui_chat_message_add_raw("Disconnected from server", "danger");
    };
    // See http://www.boost.org/doc/libs/1_55_0/doc/html/boost_asio/reference.html, Error Codes for error code meanings
    net_client->on_error = [](shared_ptr<WsClient::Connection> connection, const SimpleWeb::error_code &ec) {
      cerr << "info " << ec << ", message: " << ec.message() << endl;
      switch(ec.value()){
        case 111: // connection refused
          cout << "ECONNREFUSED" << endl;
          //TODO: ui_chat_message_add_raw("Connection refused", "danger");
          break;
        case 125: // socket closed mid-operation, this is fine
          if(!ui_running()){
            cout << "Socket force-closed" << endl;
          }else{
            cout << "Action canceled" << endl;
          }
          break;
        default:
          cout << "Unhandled error" << endl;
          //TODO: ui_chat_message_add_raw("Unhandled error: "+ec.message(), "danger");
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
    net_worker();
  }
}
