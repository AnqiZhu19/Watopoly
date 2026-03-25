#ifndef SAVEMANAGER_H
#define SAVEMANAGER_H

#include <string>
#include <vector>

class Board;
class Player;

class SaveManager {
public:
    static void save(const std::string& file, const std::vector<Player*>& players, const Board& board);
    static bool load(const std::string& file, std::vector<Player*>& players, Board& board);
};

#endif
