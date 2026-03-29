#pragma once
#include "player.h"
#include <vector>
#include <memory>

class OwnableSquare;

class Auction {
public:
    // Runs the auction for the given property among active players
    // Returns the winning player (or nullptr if everyone withdraws - property stays unowned)
    void run(OwnableSquare* property, std::vector<std::unique_ptr<Player>>& players);
};
