#pragma once
#include "square.h"
#include <string>

class Player;

class OwnableSquare : public Square {
protected:
    int price;
    Player* owner;   // nullptr = unowned
    bool mortgaged;

public:
    OwnableSquare(const std::string& name, int position, int price)
        : Square{name, position}, price{price}, owner{nullptr}, mortgaged{false} {}

    int getPrice() const { return price; }
    Player* getOwner() const { return owner; }
    bool isMortgaged() const { return mortgaged; }

    void setOwner(Player* p) { owner = p; }
    void setMortgaged(bool m) { mortgaged = m; }

    void mortgage();
    void unmortgage();

    // How much to charge player p when they land here
    virtual int getRent(Player& p, Board& b) const = 0;

    void landOn(Player& p, Board& b) override;

    void getDisplayLines(std::string lines[4], const std::string& players) const override;
    virtual std::string getImprovementLine() const { return "       "; }
};
