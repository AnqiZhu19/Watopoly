#include "game.h"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    bool testing = false;
    std::string loadFile;
    unsigned seed = std::random_device{}();

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-testing") {
            testing = true;
        } else if (arg == "-load" && i + 1 < argc) {
            loadFile = argv[++i];
        } else if (arg == "-seed" && i + 1 < argc) {
            seed = std::stoul(argv[++i]);
        }
    }

    Game game(testing, seed);

    if (!loadFile.empty()) {
        game.loadGame(loadFile);
    } else {
        game.startGame();
    }

    return 0;
}
