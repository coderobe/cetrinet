#include "util.h"

using namespace std;

namespace util {
  atomic<bool> stdout_silenced = false;

  void stdout_silence(){
    if(!util::stdout_silenced){
      stdout_silenced = true;
      freopen("/dev/null", "w", stdout);
    }
  }

  void stdout_unsilence(){
    if(util::stdout_silenced){
      stdout_silenced = false;
      freopen("/dev/tty", "w", stdout);
    }
  }

  void thread_start_net(wchar_t* server, wchar_t* port, wchar_t* username){
    cout << "Starting net worker" << endl;
    threads.push_back(new thread(net_worker, server, port, username));
  }

  void thread_start_ui(){
    cout << "Starting UI worker" << endl;
    threads.push_back(new thread(ui_worker));
    threads.push_back(new thread(ui_chat_scroller_worker));
  }
}