#pragma once
#include "nonownable.h"
#include "cardeffect.h"
#include <vector>
#include <memory>
#include <random>

class NeedlesHall : public NonOwnable {
    std::vector<std::unique_ptr<CardEffect>> effects;
    std::vector<int> weights;  // numerators for probability (denominator = 18)
    std::mt19937& rng;

public:
    NeedlesHall(const std::string& name, int position, std::mt19937& rng);
    ~NeedlesHall();
    void landOn(Player& p, Board& b) override;
    void getDisplayLines(std::string lines[4], const std::string& players) const override;
};
