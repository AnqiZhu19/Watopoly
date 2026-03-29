#pragma once
class Player;
class Board;

class CardEffect {
public:
    virtual void apply(Player& p, Board& b) = 0;
    virtual ~CardEffect() = default;
};

// Move player relative to current position
class MoveEffect : public CardEffect {
    int delta;
public:
    MoveEffect(int delta) : delta{delta} {}
    void apply(Player& p, Board& b) override;
};

// Give/take money
class MoneyEffect : public CardEffect {
    int amount;
public:
    MoneyEffect(int amount) : amount{amount} {}
    void apply(Player& p, Board& b) override;
};

// Move player to absolute position
class GotoEffect : public CardEffect {
    int targetPos;
public:
    GotoEffect(int targetPos) : targetPos{targetPos} {}
    void apply(Player& p, Board& b) override;
};

// Give player a Roll Up the Rim cup
class RimCupEffect : public CardEffect {
public:
    void apply(Player& p, Board& b) override;
};
