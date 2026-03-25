#include "residence.h"
#include "player.h"
#include "board.h"
#include <iostream>

int Residence::getRentForOwned(int numOwned) const {
    switch (numOwned) {
        case 1: return 25;
        case 2: return 50;
        case 3: return 100;
        case 4: return 200;
        default: return 25;
    }
}

int Residence::getRent(Player& p, Board& b) const {
    if (!owner || mortgaged) return 0;
    int numOwned = b.countResidencesOwned(owner);
    return getRentForOwned(numOwned);
}

void Residence::landOn(Player& p, Board& b) {
    if (owner == nullptr) {
        // Handled by game
    } else if (owner == &p) {
        std::cout << p.getName() << " owns " << name << ". No rent.\n";
        return;
    } else if (!mortgaged) {
        int rent = getRent(p, b);
        std::cout << p.getName() << " owes $" << rent << " to " << owner->getName()
                  << " for landing on " << name << "\n";
    } else {
        std::cout << name << " is mortgaged. No rent.\n";
    }
}

void Residence::getDisplayLines(std::string lines[4], const std::string& players) const {
    auto pad = [](const std::string& s, int w) {
        if ((int)s.size() >= w) return s.substr(0, w);
        return s + std::string(w - s.size(), ' ');
    };
    lines[0] = pad(name, 7);
    lines[1] = "       ";
    lines[2] = "       ";
    lines[3] = pad(players, 7);
}
