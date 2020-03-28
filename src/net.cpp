#include "net.h"

using namespace std;
using json = nlohmann::json;

shared_ptr<WsClient::Connection> net_connection;
mutex net_disconnect_mutex;

void net_send(vector<unsigned char> data){
  if(net_client != nullptr){
    shared_ptr<WsClient::OutMessage> stream = make_shared<WsClient::OutMessage>();
    for(unsigned char piece : data){
      *stream << piece;
    }
    net_connection->send(stream, nullptr, 130);
  }
}

void net_disconnect(){
  lock_guard<mutex> lock(net_disconnect_mutex);
  if(net_client != nullptr){
    cout << "net disconnect" << endl;
    net_connection.reset();
    net_client->stop();
    net_client = nullptr;
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

    util::add_notify_message("Connecting to '"+server+"' (port "+port+")");

    net_client = new WsClient((server+":"+port+"/").c_str());
    net_client->on_message = [](shared_ptr<WsClient::Connection> connection, shared_ptr<WsClient::InMessage> message){
      string msg = string(message->string());

      json payload = json::from_msgpack(msg);
      cout << "got message: version " << payload["v"] << ", type " << payload["t"] << endl;

      state_update(payload);
    };
    net_client->on_open = [](shared_ptr<WsClient::Connection> connection){
      cout << "connection opened" << endl;
      net_connection = std::move(connection);

      if(username.substr(0, username.find("#")).length() < 1){
        return;
      }

      util::add_notify_message("Connected to server "+server+":"+port);

      proto::auth auth = proto::auth();
      auth.name = username;
      net_send(json::to_msgpack(auth.encode()));

      username = username.substr(0, username.find("#"));
    };
    net_client->on_close = [](shared_ptr<WsClient::Connection> connection, int status, const string& reason){
      cout << "connection closed" << endl;
      net_disconnect();
    };
    // See http://www.boost.org/doc/libs/1_55_0/doc/html/boost_asio/reference.html, Error Codes for error code meanings
    net_client->on_error = [](shared_ptr<WsClient::Connection> connection, const SimpleWeb::error_code &ec) {
      cerr << "info " << ec << ", message: " << ec.message() << endl;
      switch(ec.value()){
        case 111: // connection refused
          cout << "ECONNREFUSED" << endl;
          util::add_error_message("Connection refused");
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
          util::add_error_message("Unhandled error " + to_string(ec.value()) + ": "+ec.message());
      }
      net_disconnect();
    };

    net_client->start();
    ui_handle_disconnect();
  } else {
    cerr << "net worker is already running" << endl;
  }
}
