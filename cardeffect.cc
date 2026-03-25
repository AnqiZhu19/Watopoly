#include "cardeffect.h"
#include "player.h"
#include "board.h"
#include "square.h"
#include <iostream>
#include <cstdlib>

// MoveEffect: move delta squares (positive = forward, negative = backward)
void MoveEffect::apply(Player& p, Board& b) {
    int newPos = (p.getPosition() + delta + 40) % 40;
    // Check if passing Collect OSAP going forward (but not landing on it)
    if (delta > 0 && newPos < p.getPosition() && newPos != 0) {
        std::cout << p.getName() << " passes Collect OSAP and collects $200!\n";
        p.addMoney(200);
    }
    std::cout << p.getName() << " moves " << (delta > 0 ? "forward " : "back ")
              << std::abs(delta) << " to " << b.getSquare(newPos)->getName() << "\n";
    p.setPosition(newPos);
    // Landing handled by Game::handleLanding after card effect returns
}

// MoneyEffect: give or take money
void MoneyEffect::apply(Player& p, Board& b) {
    if (amount >= 0) {
        std::cout << p.getName() << " receives $" << amount << "\n";
        p.addMoney(amount);
    } else {
        std::cout << p.getName() << " loses $" << -amount << "\n";
        p.addMoney(amount);  // negative = loss
    }
}

// GotoEffect: move to absolute position
void GotoEffect::apply(Player& p, Board& b) {
    if (targetPos == 10) {
        // Send to DC Tims Line - does NOT collect OSAP
        std::cout << p.getName() << " is sent to DC Tims Line!\n";
        p.setPosition(10);
        p.setInTims(true);
        p.setTurnsInTims(0);
    } else {
        // Advance to Collect OSAP (position 0) - landing handled by Game
        std::cout << p.getName() << " advances to " << b.getSquare(targetPos)->getName() << "\n";
        p.setPosition(targetPos);
        // Game::handleLanding will handle the landing effect (CollectOSAP gives $200)
    }
}

// RimCupEffect: give player a Roll Up the Rim cup
void RimCupEffect::apply(Player& p, Board& b) {
    if (b.totalTimsCups() < 4) {
        std::cout << p.getName() << " wins a Roll Up the Rim cup!\n";
        p.addTimsCup();
    }
}
