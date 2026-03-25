#include "needleshall.h"
#include "player.h"
#include "board.h"
#include <iostream>

// Needles Hall probabilities (out of 18):
// -200: 1/18
// -100: 2/18
//  -50: 3/18
//   25: 6/18 = 1/3
//   50: 3/18 = 1/6
//  100: 2/18 = 1/9
//  200: 1/18
// Total: 1+2+3+6+3+2+1 = 18

NeedlesHall::NeedlesHall(const std::string& name, int position, std::mt19937& rng)
    : NonOwnable{name, position}, rng{rng} {
    weights = {1, 2, 3, 6, 3, 2, 1};
    effects.push_back(new MoneyEffect(-200));
    effects.push_back(new MoneyEffect(-100));
    effects.push_back(new MoneyEffect(-50));
    effects.push_back(new MoneyEffect(25));
    effects.push_back(new MoneyEffect(50));
    effects.push_back(new MoneyEffect(100));
    effects.push_back(new MoneyEffect(200));
}

NeedlesHall::~NeedlesHall() {
    for (auto* e : effects) delete e;
}

void NeedlesHall::landOn(Player& p, Board& b) {
    // 1% chance of RimCup if fewer than 4 cups active
    if (b.totalTimsCups() < 4) {
        std::uniform_int_distribution<int> pct(1, 100);
        if (pct(rng) == 1) {
            RimCupEffect cup;
            cup.apply(p, b);
            return;
        }
    }
    std::cout << p.getName() << " lands on Needles Hall!\n";
    std::uniform_int_distribution<int> roll(0, 17);
    int r = roll(rng);
    int cumulative = 0;
    for (int i = 0; i < (int)effects.size(); ++i) {
        cumulative += weights[i];
        if (r < cumulative) {
            effects[i]->apply(p, b);
            return;
        }
    }
}

void NeedlesHall::getDisplayLines(std::string lines[4], const std::string& players) const {
    lines[0] = "NEEDLES";
    lines[1] = "HALL   ";
    lines[2] = "       ";
    auto pad = [](const std::string& s, int w) {
        if ((int)s.size() >= w) return s.substr(0, w);
        return s + std::string(w - s.size(), ' ');
    };
    lines[3] = pad(players, 7);
}
