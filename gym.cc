#include "gym.h"
#include "player.h"
#include "board.h"
#include <iostream>
#include <random>

int Gym::getFee(int diceSum, int numOwned) const {
    if (numOwned == 1) return 4 * diceSum;
    return 10 * diceSum;
}

int Gym::getRent(Player& p, Board& b) const {
    if (!owner || mortgaged) return 0;
    int numOwned = b.countGymsOwned(owner);
    // Roll dice for gym fee - use a temporary roll
    // This is called from landOn which handles the dice
    return 0;  // actual rent computed in landOn
}

void Gym::landOn(Player& p, Board& b) {
    if (owner == nullptr) {
        // Handled by game
    } else if (owner == &p) {
        std::cout << p.getName() << " owns " << name << ". No fee.\n";
        return;
    } else if (!mortgaged) {
        // Roll dice for fee
        std::random_device rd;
        std::mt19937 rng(rd());
        std::uniform_int_distribution<int> die(1, 6);
        int d1 = die(rng), d2 = die(rng);
        int diceSum = d1 + d2;
        int numOwned = b.countGymsOwned(owner);
        int fee = getFee(diceSum, numOwned);
        std::cout << "Dice rolled: " << d1 << " + " << d2 << " = " << diceSum << "\n";
        std::cout << p.getName() << " owes $" << fee << " to " << owner->getName()
                  << " for using " << name << "\n";
    } else {
        std::cout << name << " is mortgaged. No fee.\n";
    }
}

void Gym::getDisplayLines(std::string lines[4], const std::string& players) const {
    auto pad = [](const std::string& s, int w) {
        if ((int)s.size() >= w) return s.substr(0, w);
        return s + std::string(w - s.size(), ' ');
    };
    lines[0] = pad(name, 7);
    lines[1] = "       ";
    lines[2] = "       ";
    lines[3] = pad(players, 7);
}
