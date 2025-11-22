//
// Created by maria on 11/3/2025.
//

#include "GameWindow.h"
#include "Grid.h"

// create window + add game info in a box display
GameWindow::GameWindow(int w, int h, const char* title)
    : Fl_Window(w, h, title), grid(50, 50, 35)
{
    int gridRightX = 50 + 20 * 35; // grid starts at 50, has 20 columns of 35px
    int infoX = gridRightX + 20;
    int infoY = 50;
    int infoW = 180;
    int infoH = 100;

    infoBuffer = new Fl_Text_Buffer(); // text in the display
    infoDisplay = new Fl_Text_Display(infoX, infoY, infoW, infoH, "Game Info"); // actual display widget
    infoDisplay->buffer(infoBuffer);
    infoDisplay->textfont(FL_HELVETICA);
    infoDisplay->textsize(20);
    infoDisplay->wrap_mode(Fl_Text_Display::WRAP_AT_BOUNDS, 0);

    infoBuffer->text("Castle HP: 100\nScore: 0\nEnemies Left: -\n");

    this->end();
}

// draw window and grid
void GameWindow::draw() {
    Fl_Window::draw();
    grid.draw();
}

// detect when a cell is clicked and add 5 towers (before the game starts)
int GameWindow::handle(int event) { // event = cell clicked
    switch (event) {
        case FL_PUSH: {
            if (!running) {
                int mx = Fl::event_x();
                int my = Fl::event_y();
                grid.addTower(mx, my);
                // auto-start when 5th tower is placed
                if (!running && grid.getTowerCount() == 5) {
                    running = true;
                    gameDevelopment(); // start timer loop
                }
                redraw();
            }
            return 1;
        }
        default:
            return Fl_Window::handle(event);
    }
}

// starts the game
void GameWindow::gameDevelopment() {
    // fire first tick immediately so enemies spawn right away
    Fl::add_timeout(0.0, Timer_CB, this);
}

// callback of a timer. when the game starts, the timer fires and this function is called. Timer_CB triggers a tick in the game (step)
void GameWindow::Timer_CB(void* self) {
    static_cast<GameWindow*>(self)->tick();
}

// what happens in each turn
void GameWindow::tick() {
    grid.step();
    // stop if game ended
    if (grid.getCastleHP() <= 0 || grid.wavesDone()) {
        redraw();
        return;
    }
    updateInfoDisplay();
    redraw();
    Fl::repeat_timeout(tick_interval, Timer_CB, this); // if game hasn't ended, start the timer again and have another timer callback -> another tick
}

void GameWindow::updateInfoDisplay() {
    char buffer[128];
    sprintf(buffer, "Castle HP: %d\nScore: %d\nEnemies Left: %d\n", grid.getCastleHP(), grid.getScore(), grid.getEnemiesLeft());
    infoBuffer->text(buffer);
}
