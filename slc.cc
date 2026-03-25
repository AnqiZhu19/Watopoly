#include "slc.h"
#include "player.h"
#include "board.h"
#include <iostream>

// SLC probabilities (out of 24):
// Back 3: 3/24 = 1/8
// Back 2: 4/24 = 1/6
// Back 1: 4/24 = 1/6
// Forward 1: 3/24 = 1/8
// Forward 2: 4/24 = 1/6
// Forward 3: 4/24 = 1/6
// Go to DC Tims: 1/24
// Advance to Collect OSAP: 1/24
// Total: 3+4+4+3+4+4+1+1 = 24

SLC::SLC(const std::string& name, int position, std::mt19937& rng)
    : NonOwnable{name, position}, rng{rng} {
    // Weights: index 0=back3, 1=back2, 2=back1, 3=fwd1, 4=fwd2, 5=fwd3, 6=tims, 7=osap
    weights = {3, 4, 4, 3, 4, 4, 1, 1};
    effects.push_back(new MoveEffect(-3));
    effects.push_back(new MoveEffect(-2));
    effects.push_back(new MoveEffect(-1));
    effects.push_back(new MoveEffect(1));
    effects.push_back(new MoveEffect(2));
    effects.push_back(new MoveEffect(3));
    effects.push_back(new GotoEffect(10));   // DC Tims Line
    effects.push_back(new GotoEffect(0));    // Collect OSAP
}

SLC::~SLC() {
    for (auto* e : effects) delete e;
}

void SLC::landOn(Player& p, Board& b) {
    // 1% chance of RimCup if fewer than 4 cups active
    if (b.totalTimsCups() < 4) {
        std::uniform_int_distribution<int> pct(1, 100);
        if (pct(rng) == 1) {
            RimCupEffect cup;
            cup.apply(p, b);
            return;
        }
    }
    // Random effect from distribution
    std::uniform_int_distribution<int> roll(0, 23);
    int r = roll(rng);
    int cumulative = 0;
    for (int i = 0; i < (int)effects.size(); ++i) {
        cumulative += weights[i];
        if (r < cumulative) {
            std::cout << p.getName() << " lands on SLC!\n";
            effects[i]->apply(p, b);
            return;
        }
    }
}

void SLC::getDisplayLines(std::string lines[4], const std::string& players) const {
    lines[0] = "SLC    ";
    lines[1] = "       ";
    lines[2] = "       ";
    auto pad = [](const std::string& s, int w) {
        if ((int)s.size() >= w) return s.substr(0, w);
        return s + std::string(w - s.size(), ' ');
    };
    lines[3] = pad(players, 7);
}
