#include "academicbuilding.h"
#include "player.h"
#include "board.h"
#include <iostream>
#include <string>

bool AcademicBuilding::improve(Player& owner, Board& b) {
    if (improvements >= 5) {
        std::cout << name << " already has maximum improvements.\n";
        return false;
    }
    if (!b.ownsMonopoly(&owner, monopolyBlock)) {
        std::cout << owner.getName() << " does not own all properties in " << monopolyBlock << ".\n";
        return false;
    }
    // Cannot improve if any property in the block is mortgaged
    for (int i = 0; i < 40; ++i) {
        auto* sq = dynamic_cast<AcademicBuilding*>(b.getSquare(i));
        if (sq && sq->getMonopolyBlock() == monopolyBlock && sq->isMortgaged()) {
            std::cout << "Cannot improve " << name << " while any property in " << monopolyBlock << " is mortgaged.\n";
            return false;
        }
    }
    // Improvements must be built evenly: this building must not be ahead of others
    for (int i = 0; i < 40; ++i) {
        auto* sq = dynamic_cast<AcademicBuilding*>(b.getSquare(i));
        if (sq && sq != this && sq->getMonopolyBlock() == monopolyBlock) {
            if (improvements > sq->getImprovements()) {
                std::cout << "Must build evenly: " << sq->getName()
                          << " has fewer improvements.\n";
                return false;
            }
        }
    }
    if (!owner.spendMoney(improveCost)) {
        std::cout << owner.getName() << " cannot afford to improve " << name
                  << " (costs $" << improveCost << ")\n";
        return false;
    }
    ++improvements;
    std::cout << owner.getName() << " improves " << name
              << " to level " << improvements << "\n";
    return true;
}

bool AcademicBuilding::sellImprovement(Player& owner, Board& b) {
    if (improvements <= 0) {
        std::cout << name << " has no improvements to sell.\n";
        return false;
    }
    // Improvements must be sold evenly: this building must not be behind others
    for (int i = 0; i < 40; ++i) {
        auto* sq = dynamic_cast<AcademicBuilding*>(b.getSquare(i));
        if (sq && sq != this && sq->getMonopolyBlock() == monopolyBlock) {
            if (improvements < sq->getImprovements()) {
                std::cout << "Must sell evenly: " << sq->getName()
                          << " has more improvements.\n";
                return false;
            }
        }
    }
    int refund = improveCost / 2;
    --improvements;
    owner.addMoney(refund);
    std::cout << owner.getName() << " sells an improvement from " << name
              << " and receives $" << refund << "\n";
    return true;
}

int AcademicBuilding::getRent(Player& p, Board& b) const {
    if (mortgaged) return 0;
    if (improvements > 0) {
        return tuitionTable[improvements];
    }
    // No improvements: if owner has monopoly, charge double base rent
    if (b.ownsMonopoly(owner, monopolyBlock)) {
        return tuitionTable[0] * 2;
    }
    return tuitionTable[0];
}

void AcademicBuilding::landOn(Player& p, Board& b) {
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

std::string AcademicBuilding::getImprovementLine() const {
    if (improvements == 0) return "-------";
    std::string line(improvements, 'I');
    line += std::string(7 - improvements, ' ');
    return line;
}

void AcademicBuilding::getDisplayLines(std::string lines[4], const std::string& players) const {
    auto pad = [](const std::string& s, int w) {
        if ((int)s.size() >= w) return s.substr(0, w);
        return s + std::string(w - s.size(), ' ');
    };
    lines[0] = "       ";
    lines[1] = getImprovementLine();
    lines[2] = pad(name, 7);
    lines[3] = pad(players, 7);
}
