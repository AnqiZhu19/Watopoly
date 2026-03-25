#ifndef GYM_H
#define GYM_H

#include "ownablesquare.h"

class Board;

class Gym : public OwnableSquare {
public:
    Gym(const std::string& name, int position)
        : OwnableSquare{name, position, 150} {}

    int getFee(int diceSum, int numOwned) const;
    int getRent(Player& p, Board& b) const override;
    void landOn(Player& p, Board& b) override;
    void getDisplayLines(std::string lines[4], const std::string& players) const override;
};

#endif
