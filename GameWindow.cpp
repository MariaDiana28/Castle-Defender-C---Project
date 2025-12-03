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
    int infoH = 150;

    infoBuffer = new Fl_Text_Buffer(); // text in the display
    infoDisplay = new Fl_Text_Display(infoX, infoY, infoW, infoH, "Game Info"); // actual display widget
    infoDisplay->buffer(infoBuffer);
    infoDisplay->textfont(FL_HELVETICA);
    infoDisplay->textsize(20);
    infoDisplay->wrap_mode(Fl_Text_Display::WRAP_AT_BOUNDS, 0);

    infoBuffer->text("Castle HP: 100\nScore: 0\nEnemies Left: -\nStarted Adaptive Spawning? \n");
    doneButton = new Fl_Button(infoX, infoY + infoH + 20, 100, 40, "Done"); // for controlling the nummber of towers placed before the game starts

    this->end();
}

// draw window and grid
void GameWindow::draw() {
    Fl_Window::draw();
    // first screen, with the actual game
    if (!gameOver) {
        grid.draw();
    } else {
        // Draw summary screen when gameOver=true
        fl_color(FL_WHITE);
        fl_rectf(0, 0, w(), h()); // clear window background

        fl_color(FL_BLACK);
        fl_font(FL_HELVETICA_BOLD, 36);
        fl_draw("GAME OVER", w() / 2 - 150, h() / 2 - 100);

        fl_font(FL_HELVETICA, 22);
        char summary[256];
        sprintf(summary,
                "Player Score: %d\n"
                "Enemies Destroyed: %d\n"
                "Castle Health: %d\n"
                "Winner: %s",
                grid.getScore(),
                grid.getScore() / 10,
                grid.getCastleHP(),
                (grid.getCastleHP() > 0 ? "Player" : "AI"));

        // Draw multiline text
        int y = h() / 2 - 40;
        char* line = strtok(summary, "\n");
        while (line) {
            fl_draw(line, w() / 2 - 150, y);
            y += 40;
            line = strtok(nullptr, "\n");
        }

        fl_font(FL_HELVETICA, 18);
        fl_draw("Press ESC to exit", w() / 2 - 100, y + 40);
    }
}


// detect when a cell is clicked and add towers (before the game starts). The game starts automatically after the 5th tower is placed, or the player can press the "Done" button to start the game with fewer towers
int GameWindow::handle(int event) {
    switch (event) {
        case FL_PUSH: {
            int mx = Fl::event_x(); // get FLTK coordinated in the window
            int my = Fl::event_y();

            // Check if "Done" button was clicked
            if (!running && doneButton->active()) {
                // if the player clicked within the area of the button
                if (mx >= doneButton->x() && mx <= doneButton->x() + doneButton->w() &&
                    my >= doneButton->y() && my <= doneButton->y() + doneButton->h()) {
                    // Only allow if at least one tower is placed
                    if (grid.getTowerCount() >= 1) {
                        running = true;
                        doneButton->deactivate(); // not clickable
                        gameDevelopment();  // start game
                        redraw();
                        return 1;
                    }
                }
            }

            // Handle grid clicks for placing towers
            if (!running) {
                grid.addTower(mx, my);
                // start game automatically after the 5th tower is placed
                if (grid.getTowerCount() == 5) {
                    doneButton->deactivate();
                    running = true;
                    gameDevelopment(); // start game
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
    if (!gameOverPending && (grid.getCastleHP() <= 0 || grid.wavesDone())) {
        gameOverPending = true;
        updateInfoDisplay();  // show final scoreboard
        redraw();
        Fl::repeat_timeout(0.1, Timer_CB, this);  // one more redraw cycle
        return;
    }
    if (gameOverPending) {
        running = false;
        gameOver = true;
        redraw();
        return;
    }
    updateInfoDisplay();
    redraw();
    Fl::repeat_timeout(tick_interval, Timer_CB, this); // if game hasn't ended, start the timer again and have another timer callback -> another tick
}

// update the info display
void GameWindow::updateInfoDisplay() {
    char buffer[128];
    sprintf(buffer, "Castle HP: %d\nScore: %d\nEnemies Left: %d\nStarted Adaptive Spawning? %d\n", grid.getCastleHP(), grid.getScore(), grid.getEnemiesLeft(), grid.startedAdaptiveSpawning());
    infoBuffer->text(buffer);
}
