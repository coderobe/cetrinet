#include "util.h"

using namespace std;

namespace util {
  atomic<bool> stdout_silenced = false;

  void stdout_silence(){
    if(!util::stdout_silenced){
      stdout_silenced = true;

      #ifdef WINDOWS
        cerr << "util::stdout_silence() not implemented on this platform" << endl;
      #else
        freopen("/dev/null", "w", stdout);
      #endif
    }
  }

  bool is_not_digit(char c){
    return !isdigit(c);
  }

  void stdout_unsilence(){
    if(util::stdout_silenced){
      stdout_silenced = false;

      #ifdef WINDOWS
        cerr << "util::stdout_unsilence() not implemented on this platform" << endl;
      #else
        freopen("/dev/tty", "w", stdout);
      #endif
    }
  }

  void thread_start_net(){
    cout << "Starting net worker" << endl;
    threads.push_back(new thread(net_worker));
  }

  void thread_start_ui(){
    cout << "Starting UI worker" << endl;
    threads.push_back(new thread(ui_worker));
  }
}