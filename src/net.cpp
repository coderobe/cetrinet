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
      json payload = json::from_msgpack(msg.data());
      cout << "got message: version " << payload["v"] << ", type " << payload["t"] << endl;

      if(payload["t"] == "motd"){
        proto::motd event = proto::motd();
        event.load_json(payload);

        cout << "motd: " << event.message << endl;
        ui_chat_message_add_raw("MOTD: "+event.message, "dark");
      }else if(payload["t"] == "error"){
        proto::error event = proto::error();
        event.load_json(payload);

        cout << "server reports error " << event.code << ": " << event.message << endl;
        ui_chat_message_add_raw("Error: "+string(event.message), "danger");
      }else if(payload["t"] == "join"){
        proto::join event = proto::join();
        event.load_json(payload);

        for(proto::channel* chan : channels){
          if(chan->name == event.target){
            if(event.user == username){
              chan->joined = true;
            }else{
              proto::user* nu = new proto::user;
              nu->name = event.user;
              chan->userdata.push_back(nu);
            }
          }
        }
        cout << "user '" << event.user << "' joined channel " << event.target << endl;
        ui_chat_message_add_raw(string(event.user)+" joined the channel", "light");
        ui_update_channel_state();
        ui_update_users_state();
      }else if(payload["t"] == "part"){
        proto::part event = proto::part();
        event.load_json(payload);

        cout << "user '" << event.user << "' parted from " << event.target << endl;
        ui_chat_message_add_raw(string(event.user)+" left the channel", "light");
      }else if(payload["t"] == "smsg"){
        proto::smsg event = proto::smsg();
        event.load_json(payload);

        cout << "smsg: " << event.message << endl;
        ui_chat_message_add_raw("Server: "+string(event.message), "danger");
      }else if(payload["t"] == "cmsg"){
        proto::cmsg event = proto::cmsg();
        event.load_json(payload);

        cout << "cmsg from '" << event.source << "': " << event.message << endl;
        ui_chat_message_add_raw(string(event.source)+": "+string(event.message), "light");
      }else if(payload["t"] == "channellist"){
        proto::channellist event = proto::channellist();
        event.load_json(payload);

        for(proto::channel* chan : event.channels){
          cout << "found channel " << chan->name << " with " << chan->users << " users" << endl;

          proto::channel* channel = new proto::channel;
          channel->name = chan->name;
          channel->users = chan->users;
          channel->joined = chan->joined;
          channels.push_back(channel);
        }
        ui_update_channel_state();
      }else if(payload["t"] == "userlist"){
        proto::userlist event = proto::userlist();
        event.load_json(payload);

        cout << "userlist for channel " << event.channel << " reports " << event.users.size() << " users" << endl;
        for(proto::channel* chan : channels){
          if(chan->name == event.channel){
            vector<proto::user*> new_users;
            for(proto::user* eu : event.users){
              proto::user* nu = new proto::user;
              nu->name = eu->name;
              new_users.push_back(nu);
            }
            //TODO: clean up new_users to prevent leaking memory?
            chan->userdata.swap(new_users);
          }
        }
        ui_update_users_state();
      }
    };
    net_client->on_open = [](shared_ptr<WsClient::Connection> connection){
      cout << "connection opened" << endl;
      net_connection = connection;
      username = username.substr(0, username.find("#"));
      ui_show_game(true);
      ui_chat_message_add_raw("Connected to server", "success");
      proto::auth auth = proto::auth();
      auth.name = username;
      net_send(json::to_msgpack(auth.encode()));
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
          if(!ui_running()){
            cout << "Socket force-closed" << endl;
          }else{
            cout << "Action canceled" << endl;
          }
          break;
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
    net_worker();
  }
}
