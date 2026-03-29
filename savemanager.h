#pragma once
#include "player.h"
#include <string>
#include <vector>
#include <memory>

class Board;

class SaveManager {
public:
    static void save(const std::string& file, const std::vector<std::unique_ptr<Player>>& players, const Board& board);
    static bool load(const std::string& file, std::vector<std::unique_ptr<Player>>& players, Board& board);
};
