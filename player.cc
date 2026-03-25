#include "player.h"
#include "ownablesquare.h"
#include "academicbuilding.h"
#include <iostream>
#include <algorithm>

Player::Player(const std::string& name, char piece, int money)
    : name{name}, piece{piece}, money{money}, position{0},
      timsCups{0}, turnsInTims{0}, inTims{false}, bankrupt{false} {}

void Player::setMoney(int amount) {
    money = amount;
    notifyObservers();
}

void Player::addMoney(int amount) {
    money += amount;
    notifyObservers();
}

bool Player::spendMoney(int amount) {
    if (money < amount) return false;
    money -= amount;
    notifyObservers();
    return true;
}

void Player::setPosition(int pos, bool notifyObs) {
    position = pos;
    if (notifyObs) notifyObservers();
}

void Player::addProperty(OwnableSquare* sq) {
    properties.push_back(sq);
    sq->setOwner(this);
}

void Player::removeProperty(OwnableSquare* sq) {
    properties.erase(std::remove(properties.begin(), properties.end(), sq), properties.end());
    sq->setOwner(nullptr);
}

int Player::getTotalWorth() const {
    int worth = money;
    for (auto* sq : properties) {
        worth += sq->getPrice();
        // Add improvement costs if academic building
        auto* ab = dynamic_cast<AcademicBuilding*>(sq);
        if (ab) {
            worth += ab->getImprovements() * ab->getImproveCost();
        }
    }
    return worth;
}

void Player::displayAssets() const {
    std::cout << name << " (piece: " << piece << ")\n";
    std::cout << "  Money: $" << money << "\n";
    std::cout << "  Total worth: $" << getTotalWorth() << "\n";
    std::cout << "  Roll Up the Rim cups: " << timsCups << "\n";
    if (properties.empty()) {
        std::cout << "  Properties: none\n";
    } else {
        std::cout << "  Properties:\n";
        for (auto* sq : properties) {
            std::cout << "    " << sq->getName();
            if (sq->isMortgaged()) std::cout << " [mortgaged]";
            auto* ab = dynamic_cast<AcademicBuilding*>(sq);
            if (ab && ab->getImprovements() > 0) {
                std::cout << " [" << ab->getImprovements() << " improvements]";
            }
            std::cout << "\n";
        }
    }
}
