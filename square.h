#ifndef SQUARE_H
#define SQUARE_H

#include <string>

class Player;
class Board;

class Square {
protected:
    std::string name;
    int position;

public:
    Square(const std::string& name, int position) : name{name}, position{position} {}
    virtual ~Square() = default;

    std::string getName() const { return name; }
    int getPosition() const { return position; }

    virtual void landOn(Player& p, Board& b) = 0;

    // Returns display lines for the board (4 content lines, each 7 chars wide)
    // line[0] = top, line[1] = improvement/second name, line[2] = name/third, line[3] = players
    virtual void getDisplayLines(std::string lines[4], const std::string& players) const = 0;
};

#endif
