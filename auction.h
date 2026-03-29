#pragma once
#include <vector>

class OwnableSquare;
class Player;

class Auction {
public:
    // Runs the auction for the given property among active players
    // Returns the winning player (or nullptr if everyone withdraws - property stays unowned)
    void run(OwnableSquare* property, std::vector<Player*>& players);
};
