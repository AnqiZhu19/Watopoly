#ifndef GAME_H
#define GAME_H

#include <vector>
#include <string>
#include <random>

class Player;
class Board;
class OwnableSquare;

class Game {
    std::vector<Player*> players;
    Board* board;
    int currentPlayerIdx;
    bool testingMode;
    std::mt19937 rng;

    // Game state flags
    bool hasRolled;
    int doublesCount;
    int lastDiceSum;   // dice sum from most recent roll (for gym fees)
    bool needToPayDebt;
    int debtAmount;
    Player* creditor;  // nullptr = owed to bank

public:
    Game(bool testing = false, unsigned seed = std::random_device{}());
    ~Game();

    void startGame();
    void loadGame(const std::string& file);
    void saveGame(const std::string& file);

    void run();

private:
    void runTurn(Player& p);
    void handleTimsTurn(Player& p);
    void doRoll(Player& p, int d1 = -1, int d2 = -1);
    void movePlayer(Player& p, int steps);
    void movePlayerTo(Player& p, int pos);
    void handleLanding(Player& p, bool fromCard = false);  // fromCard: don't chain SLC/NH

    void promptBuy(Player& p, OwnableSquare* sq);
    void handleDebt(Player& p, int amount, Player* creditor);
    void handleBankruptcy(Player& p, Player* creditor);
    void transferAssets(Player& from, Player* to);  // to=nullptr means bank

    void processCommand(Player& p, const std::string& cmd);
    void doTrade(Player& p, const std::string& args);
    void doImprove(Player& p, const std::string& prop, bool buy);
    void doMortgage(Player& p, const std::string& prop);
    void doUnmortgage(Player& p, const std::string& prop);
    void doAssets(const Player& p) const;
    void doAll() const;

    Player* findPlayer(const std::string& name) const;
    OwnableSquare* findProperty(const std::string& name) const;

    bool isGameOver() const;
    Player* getWinner() const;

    std::pair<int,int> rollDice(int d1 = -1, int d2 = -1);
    int activePlayers() const;
};

#endif
