#ifndef NONOWNABLE_H
#define NONOWNABLE_H

#include "square.h"
#include <string>

// Base for squares that can't be owned
class NonOwnable : public Square {
public:
    NonOwnable(const std::string& name, int position) : Square{name, position} {}
    void getDisplayLines(std::string lines[4], const std::string& players) const override;
};

// Collect OSAP (position 0)
class CollectOSAP : public NonOwnable {
public:
    CollectOSAP() : NonOwnable{"Collect OSAP", 0} {}
    void landOn(Player& p, Board& b) override;
    void getDisplayLines(std::string lines[4], const std::string& players) const override;
};

// DC Tims Line (position 10)
class DCTimsLine : public NonOwnable {
public:
    DCTimsLine() : NonOwnable{"DC Tims Line", 10} {}
    void landOn(Player& p, Board& b) override;
    void sendToTims(Player& p);
    void getDisplayLines(std::string lines[4], const std::string& players) const override;
};

// Go to Tims (position 30)
class GoToTims : public NonOwnable {
public:
    GoToTims() : NonOwnable{"Go to Tims", 30} {}
    void landOn(Player& p, Board& b) override;
    void getDisplayLines(std::string lines[4], const std::string& players) const override;
};

// Goose Nesting (position 20)
class GooseNesting : public NonOwnable {
public:
    GooseNesting() : NonOwnable{"Goose Nesting", 20} {}
    void landOn(Player& p, Board& b) override;
    void getDisplayLines(std::string lines[4], const std::string& players) const override;
};

// Tuition (position 4)
class Tuition : public NonOwnable {
public:
    Tuition() : NonOwnable{"Tuition", 4} {}
    void landOn(Player& p, Board& b) override;
    void getDisplayLines(std::string lines[4], const std::string& players) const override;
};

// Coop Fee (position 38)
class CoopFee : public NonOwnable {
public:
    CoopFee() : NonOwnable{"Coop Fee", 38} {}
    void landOn(Player& p, Board& b) override;
    void getDisplayLines(std::string lines[4], const std::string& players) const override;
};

#endif
