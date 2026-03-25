#include "nonownable.h"
#include "player.h"
#include "board.h"
#include <iostream>
#include <string>

static std::string padTo7(const std::string& s) {
    if ((int)s.size() >= 7) return s.substr(0, 7);
    return s + std::string(7 - s.size(), ' ');
}

// NonOwnable base display
void NonOwnable::getDisplayLines(std::string lines[4], const std::string& players) const {
    lines[0] = padTo7(name);
    lines[1] = "       ";
    lines[2] = "       ";
    lines[3] = padTo7(players);
}

// CollectOSAP
void CollectOSAP::landOn(Player& p, Board& b) {
    std::cout << p.getName() << " lands on Collect OSAP and collects $200!\n";
    p.addMoney(200);
}

void CollectOSAP::getDisplayLines(std::string lines[4], const std::string& players) const {
    lines[0] = "COLLECT";
    lines[1] = "OSAP   ";
    lines[2] = "       ";
    lines[3] = padTo7(players);
}

// DCTimsLine
void DCTimsLine::landOn(Player& p, Board& b) {
    // Just visiting - nothing happens
    if (!p.isInTims()) {
        std::cout << p.getName() << " is just visiting DC Tims Line.\n";
    }
}

void DCTimsLine::sendToTims(Player& p) {
    std::cout << p.getName() << " is sent to DC Tims Line!\n";
    p.setPosition(10, false);
    p.setInTims(true);
    p.setTurnsInTims(0);
}

void DCTimsLine::getDisplayLines(std::string lines[4], const std::string& players) const {
    lines[0] = "DC Tims";
    lines[1] = "Line   ";
    lines[2] = "       ";
    lines[3] = padTo7(players);
}

// GoToTims
void GoToTims::landOn(Player& p, Board& b) {
    std::cout << p.getName() << " lands on Go to Tims and is sent to DC Tims Line!\n";
    p.setPosition(10, false);
    p.setInTims(true);
    p.setTurnsInTims(0);
}

void GoToTims::getDisplayLines(std::string lines[4], const std::string& players) const {
    lines[0] = "GO TO  ";
    lines[1] = "TIMS   ";
    lines[2] = "       ";
    lines[3] = padTo7(players);
}

// GooseNesting
void GooseNesting::landOn(Player& p, Board& b) {
    std::cout << p.getName() << " lands on Goose Nesting and is attacked by geese!\n";
}

void GooseNesting::getDisplayLines(std::string lines[4], const std::string& players) const {
    lines[0] = "Goose  ";
    lines[1] = "Nesting";
    lines[2] = "       ";
    lines[3] = padTo7(players);
}

// Tuition
void Tuition::landOn(Player& p, Board& b) {
    int worth10 = p.getTotalWorth() / 10;
    std::cout << p.getName() << " lands on Tuition.\n";
    std::cout << "Pay $300 or 10% of total worth ($" << worth10 << ")? (300/10%): ";
    std::string choice;
    std::getline(std::cin, choice);
    int amount;
    if (choice == "10%" || choice == "10") {
        amount = worth10;
    } else {
        amount = 300;
    }
    std::cout << p.getName() << " pays $" << amount << " in tuition.\n";
    p.addMoney(-amount);
}

void Tuition::getDisplayLines(std::string lines[4], const std::string& players) const {
    lines[0] = "TUITION";
    lines[1] = "       ";
    lines[2] = "       ";
    lines[3] = padTo7(players);
}

// CoopFee
void CoopFee::landOn(Player& p, Board& b) {
    std::cout << p.getName() << " lands on Coop Fee and pays $150!\n";
    p.addMoney(-150);
}

void CoopFee::getDisplayLines(std::string lines[4], const std::string& players) const {
    lines[0] = "COOP   ";
    lines[1] = "FEE    ";
    lines[2] = "       ";
    lines[3] = padTo7(players);
}
