#include "ownablesquare.h"
#include "player.h"
#include "board.h"
#include "auction.h"
#include <iostream>
#include <algorithm>

void OwnableSquare::mortgage() {
    if (mortgaged) {
        std::cout << name << " is already mortgaged.\n";
        return;
    }
    mortgaged = true;
    if (owner) {
        int proceeds = price / 2;
        owner->addMoney(proceeds);
        std::cout << owner->getName() << " mortgages " << name
                  << " and receives $" << proceeds << "\n";
    }
}

void OwnableSquare::unmortgage() {
    if (!mortgaged) {
        std::cout << name << " is not mortgaged.\n";
        return;
    }
    if (!owner) return;
    int cost = static_cast<int>(price * 0.6);  // 50% + 10% = 60%
    if (!owner->spendMoney(cost)) {
        std::cout << owner->getName() << " cannot afford to unmortgage " << name
                  << " (costs $" << cost << ")\n";
        return;
    }
    mortgaged = false;
    std::cout << owner->getName() << " unmortgages " << name
              << " for $" << cost << "\n";
}

void OwnableSquare::landOn(Player& p, Board& b) {
    if (owner == nullptr) {
        // Unowned: offer to buy
        std::cout << name << " is unowned. Price: $" << price << "\n";
        std::cout << p.getName() << ", do you want to buy it? (yes/no): ";
        std::string answer;
        std::cin >> answer;
        if (answer == "yes" || answer == "y") {
            if (p.spendMoney(price)) {
                p.addProperty(this);
                std::cout << p.getName() << " buys " << name << " for $" << price << "\n";
            } else {
                std::cout << p.getName() << " cannot afford " << name << "\n";
                // Auction
                std::vector<Player*> allPlayers;
                // We need to get all players - this is handled via board/game
                // For now, just run an auction with available players
                // The game handles getting player list
            }
        } else {
            std::cout << "Going to auction...\n";
            // Auction handled by game
        }
    } else if (owner == &p) {
        std::cout << p.getName() << " owns " << name << ". No rent.\n";
    } else if (mortgaged) {
        std::cout << name << " is mortgaged. No rent.\n";
    } else {
        int rent = getRent(p, b);
        std::cout << p.getName() << " pays $" << rent << " to " << owner->getName()
                  << " for landing on " << name << "\n";
        // Actual payment handled by game
    }
}

void OwnableSquare::getDisplayLines(std::string lines[4], const std::string& players) const {
    // Default: name on first line (for residences/gyms)
    auto pad = [](const std::string& s, int w) {
        if ((int)s.size() >= w) return s.substr(0, w);
        return s + std::string(w - s.size(), ' ');
    };
    lines[0] = pad(name, 7);
    lines[1] = "       ";
    lines[2] = "       ";
    lines[3] = pad(players, 7);
}
