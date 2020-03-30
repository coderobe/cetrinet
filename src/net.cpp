#include "net.h"

using namespace std;
using json = nlohmann::json;

std::atomic<bool> net_connected = true;
shared_ptr<WsClient::Connection> net_connection;
mutex net_disconnect_mutex;

void net_send(vector<unsigned char> data){
  if(net_connection != nullptr){
    shared_ptr<WsClient::OutMessage> stream = make_shared<WsClient::OutMessage>();
    for(unsigned char piece : data){
      *stream << piece;
    }
    net_connection->send(stream, nullptr, 130);
  }
}

void net_send(json payload){
  cout << "sent message: version " << payload["v"] << ", type " << payload["t"] << endl;
  net_send(json::to_msgpack(payload));
}

void net_disconnect(){
  scoped_lock lock(net_disconnect_mutex);
  if(net_client != nullptr){
    net_connection = nullptr;
    net_client->stop();
  }
}

void net_worker(){
  if(net_connected) net_disconnect();
  net_connected = true;

  if(username.substr(0, username.find("#")).length() < 1){
    util::add_error_message("Bad username");
    return;
  }

  net_client.reset(new WsClient((server+":"+port+"/").c_str()));
  clean_up();
  util::add_message_divider();
  util::add_notify_message("Connecting to '"+server+":"+port+"'...");

  net_client->on_message = [](shared_ptr<WsClient::Connection> connection, shared_ptr<WsClient::InMessage> message){
    string msg = message->string();

    json payload = json::from_msgpack(msg);
    cout << "got message: version " << payload["v"] << ", type " << payload["t"] << endl;

    state_update(payload);
  };
  net_client->on_open = [](shared_ptr<WsClient::Connection> connection){
    net_connection = std::move(connection);

    util::add_notify_message("Connected to server "+server+":"+port);

    proto::auth auth = proto::auth();
    auth.name = username;
    username = username.substr(0, username.find("#"));
    util::add_muted_message("Authenticating as '"+username+"'...");
    net_send(json::to_msgpack(auth.encode()));
  };
  net_client->on_close = [](shared_ptr<WsClient::Connection> connection, int status, const string& reason){
    util::add_info_message("The remote host closed the connection");
    net_disconnect();
  };
  // See http://www.boost.org/doc/libs/1_55_0/doc/html/boost_asio/reference.html, Error Codes for error code meanings
  net_client->on_error = [](shared_ptr<WsClient::Connection> connection, const SimpleWeb::error_code &ec) {
    switch(ec.value()){
      case 1: // host not found (dns)
      case 111: // connection refused
        util::add_error_message(ec.message());
        break;
      case 2: // end of file
        util::add_error_message("The server unexpectedly closed the connection");
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
        util::add_error_message("Unhandled error " + to_string(ec.value()) + ": "+ec.message());
        cerr << "info " << ec << ", message: " << ec.message() << endl;
    }
    net_disconnect();
  };

  net_client->start();
  net_connected = false;
  util::add_info_message("Disconnected from server");
}
