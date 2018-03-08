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

  LCUI_Widget get_widget(char* id){
    return LCUIWidget_GetById(id);
  }

  LCUI_Widget get_widget(string id){
    return LCUIWidget_GetById(id.c_str());
  }

  string wctos(wchar_t* w){
    string tinput = "";
    char buffer = '\0';
    size_t buffer_index = 0;
    while(true){
      wcstombs(&buffer, &w[buffer_index], 1);
      if(buffer == '\0'){
        break;
      }
      buffer_index++;
      tinput += buffer;
    }
    return tinput;
  }

  string wctos(wchar_t* w, size_t len){
    char out_c[len];
    wcstombs(out_c, w, len);
    return string(out_c);
  }

  void get_char_from_textinput_event(LCUI_WidgetEvent event, char* dest){
    /*
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
    */
    wcstombs(dest, &event->text.text[0], 1);
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
    threads.push_back(new thread(ui_chat_scroller_worker));
  }
}