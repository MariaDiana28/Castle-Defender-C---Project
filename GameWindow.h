//
// Created by maria on 11/3/2025.
//

#ifndef CASTLE_DEFENDER_GAMEWINDOW_H
#define CASTLE_DEFENDER_GAMEWINDOW_H
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Text_Buffer.H>
#include "Grid.h"

class GameWindow : public Fl_Window {
public:
    GameWindow(int w, int h, const char* title);
    void draw() override;
    int handle(int event) override;
    void gameDevelopment();
    void tick();
    static void Timer_CB(void* self);
    void updateInfoDisplay();
private:
    Grid grid;
    bool running = false;
    double tick_interval = 0.1;
    Fl_Text_Display* infoDisplay = nullptr;
    Fl_Text_Buffer* infoBuffer = nullptr;
};

#endif //CASTLE_DEFENDER_GAMEWINDOW_H