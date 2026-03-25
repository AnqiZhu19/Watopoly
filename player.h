#ifndef PLAYER_H
#define PLAYER_H

#include "observer.h"
#include <string>
#include <vector>

class OwnableSquare;

class Player : public Subject {
    std::string name;
    char piece;
    int money;
    int position;
    int timsCups;
    int turnsInTims;  // number of turns spent in tims (0 if not in tims)
    bool inTims;      // true if actually in tims line (not just visiting)
    bool bankrupt;
    std::vector<OwnableSquare*> properties;

public:
    Player(const std::string& name, char piece, int money = 1500);

    std::string getName() const { return name; }
    char getPiece() const { return piece; }
    int getMoney() const { return money; }
    int getPosition() const { return position; }
    int getTimsCups() const { return timsCups; }
    int getTurnsInTims() const { return turnsInTims; }
    bool isInTims() const { return inTims; }
    bool isBankrupt() const { return bankrupt; }
    const std::vector<OwnableSquare*>& getProperties() const { return properties; }

    void setMoney(int amount);
    void addMoney(int amount);
    bool spendMoney(int amount);  // returns false if insufficient funds
    void setPosition(int pos, bool notifyObs = true);
    void setTimsCups(int cups) { timsCups = cups; }
    void addTimsCup() { ++timsCups; }
    void useTimsCup() { if (timsCups > 0) --timsCups; }
    void setInTims(bool in) { inTims = in; }
    void setTurnsInTims(int t) { turnsInTims = t; }
    void incrementTurnsInTims() { ++turnsInTims; }
    void setBankrupt(bool b) { bankrupt = b; }

    void addProperty(OwnableSquare* sq);
    void removeProperty(OwnableSquare* sq);

    // Total worth: money + property prices + improvement costs
    int getTotalWorth() const;

    void declareBankrupt() { bankrupt = true; }
    void displayAssets() const;
};

#endif
