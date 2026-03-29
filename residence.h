#pragma once
#include "ownablesquare.h"

class Board;

class Residence : public OwnableSquare {
public:
    Residence(const std::string& name, int position)
        : OwnableSquare{name, position, 200} {}

    int getRentForOwned(int numOwned) const;
    int getRent(Player& p, Board& b) const override;
    void landOn(Player& p, Board& b) override;
    void getDisplayLines(std::string lines[4], const std::string& players) const override;
};
