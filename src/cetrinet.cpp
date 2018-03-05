#include <cstdio>

#include <LCUI_Build.h>
#include <LCUI/LCUI.h>
#include <LCUI/display.h>
#include <LCUI/gui/widget.h>
#include <LCUI/gui/builder.h>

extern "C" {
  #include <LCUIEx.h>
}

int main(){
  LCUI_Widget root, pack;

  LCUI_Init();
  LCUIEx_Init();

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
