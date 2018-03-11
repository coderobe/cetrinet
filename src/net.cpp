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

void net_disconnect(){
  if(net_client != nullptr){
    net_client->stop();
  }
  while(net_client != nullptr){
    this_thread::sleep_for(chrono::milliseconds(10));
  }
}

void net_worker(){
  if(net_client == nullptr){
    clean_up();

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

      if(username.substr(0, username.find("#")).length() < 1){
        return;
      }

      util::add_message("server", "cetrinet", "connected to server", (unsigned char[3]){0, 100, 0});

      proto::auth auth = proto::auth();
      auth.name = username;
      net_send(json::to_msgpack(auth.encode()));

      username = username.substr(0, username.find("#"));
    };
    net_client->on_close = [](shared_ptr<WsClient::Connection> connection, int status, const string& reason){
      cout << "connection closed" << endl;
      util::add_message("server", "cetrinet", "disconnected from server", (unsigned char[3]){100, 0, 0});
    };
    // See http://www.boost.org/doc/libs/1_55_0/doc/html/boost_asio/reference.html, Error Codes for error code meanings
    net_client->on_error = [](shared_ptr<WsClient::Connection> connection, const SimpleWeb::error_code &ec) {
      cerr << "info " << ec << ", message: " << ec.message() << endl;
      switch(ec.value()){
        case 111: // connection refused
          cout << "ECONNREFUSED" << endl;
          util::add_message("server", "cetrinet", "connection refused", (unsigned char[3]){100, 0, 0});
          break;
        case 125: // socket closed mid-operation, this is fine
        case 995:
          if(!ui_running()){
            cout << "Socket force-closed" << endl;
          }else{
            cout << "Action canceled" << endl;
          }
          break;
        default:
          cout << "Unhandled error" << endl;
          util::add_message("server", "cetrinet", "Unhandled error " + to_string(ec.value()) + ": "+ec.message(), (unsigned char[3]){100, 0, 0});
      }
    };

    net_client->start();
    if(net_client != nullptr){
      delete net_client;
      net_client = nullptr;
    }
    ui_handle_disconnect();
  } else {
    net_client->stop();
    while(net_client != nullptr){
      this_thread::sleep_for(chrono::milliseconds(10));
    }
    net_worker();
  }
}
