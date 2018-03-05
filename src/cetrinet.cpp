#include <cstdio>
#include <unistd.h>
#include <thread>
#include <chrono>

#include <cpprest/ws_client.h>

#include <LCUI_Build.h>
#include <LCUI/LCUI.h>
#include <LCUI/display.h>
#include <LCUI/gui/widget.h>
#include <LCUI/gui/builder.h>
extern "C" {
  #include <LCUIEx.h>
}

using namespace std;

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

  return LCUI_Main();  
}

int main(){
  printf("Hello, world!\n");

  thread ui_thread(ui_worker);

  printf("Waiting for threads\n");
  ui_thread.join();
  printf("Done\n");
  return 0;
}
