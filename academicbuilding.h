#pragma once
#include "ownablesquare.h"
#include <string>
#include <array>

class Board;

class AcademicBuilding : public OwnableSquare {
    std::string monopolyBlock;
    int improvements;      // 0-5
    std::array<int,6> tuitionTable;
    int improveCost;

public:
    AcademicBuilding(const std::string& name, int position, int price,
                     int improveCost, const std::string& block,
                     const std::array<int,6>& tuition)
        : OwnableSquare{name, position, price},
          monopolyBlock{block}, improvements{0},
          tuitionTable{tuition}, improveCost{improveCost} {}

    std::string getMonopolyBlock() const { return monopolyBlock; }
    int getImprovements() const { return improvements; }
    int getImproveCost() const { return improveCost; }
    void setImprovements(int n) { improvements = n; }

    // Try to buy one improvement; returns true on success
    bool improve(Player& owner, Board& b);
    // Sell one improvement; returns true on success
    bool sellImprovement(Player& owner, Board& b);

    int getTuition() const { return tuitionTable[improvements]; }
    int getTuitionDoubled() const { return tuitionTable[0] * 2; }

    int getRent(Player& p, Board& b) const override;
    void landOn(Player& p, Board& b) override;

    std::string getImprovementLine() const override;
    void getDisplayLines(std::string lines[4], const std::string& players) const override;
};
