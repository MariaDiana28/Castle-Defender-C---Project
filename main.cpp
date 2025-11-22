#include <FL/Fl.H>
#include "GameWindow.h"
#include <ctime>

int main() {
    srand(time(nullptr)); // random seed + get time
    GameWindow window(1000, 1000, "Castle Defender");
    window.show();
    return Fl::run();
}
