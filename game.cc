#include "game.h"
#include "graphicdisplay.h"
#include "board.h"
#include "player.h"
#include "square.h"
#include "ownablesquare.h"
#include "academicbuilding.h"
#include "residence.h"
#include "gym.h"
#include "nonownable.h"
#include "slc.h"
#include "needleshall.h"
#include "auction.h"
#include "savemanager.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <string>
#include <cctype>
#include <set>

Game::Game(bool testing, unsigned seed, bool graphic)
    : board{nullptr}, gfx{nullptr}, graphicMode{graphic},
      currentPlayerIdx{0}, testingMode{testing},
      rng{seed}, hasRolled{false}, doublesCount{0}, lastDiceSum{0},
      needToPayDebt{false}, debtAmount{0}, creditor{nullptr} {
    board = new Board(rng);
    if (graphicMode) gfx = new GraphicDisplay(board, &players);
}

Game::~Game() {
    for (auto* p : players) delete p;
    delete board;
    delete gfx;
}

static std::string toLower(std::string s) {
    for (char& c : s) c = std::tolower(c);
    return s;
}

void Game::startGame() {
    int n = 0;
    std::cout << "Enter number of players (2-6): ";
    {
        std::string line;
        while (std::getline(std::cin, line)) {
            try { n = std::stoi(line); } catch (...) { n = 0; }
            if (n >= 2 && n <= 6) break;
            std::cout << "Invalid. Enter number of players (2-6): ";
        }
    }

    std::set<char> usedPieces;
    const std::string validPieces = "GBDPSL$T";

    for (int i = 0; i < n; ++i) {
        std::string name;
        char piece = 0;
        bool valid = false;

        std::cout << "Player " << (i+1) << " name: ";
        {
            std::string line;
            while (std::getline(std::cin, line) && line.empty()) {}
            name = line;
        }
        while (name == "BANK") {
            std::cout << "Name 'BANK' is reserved. Choose another: ";
            std::getline(std::cin, name);
        }

        while (!valid) {
            std::cout << "Choose piece (G=Goose, B=GRT Bus, D=Doughnut, P=Professor, "
                      << "S=Student, $=Money, L=Laptop, T=Pink tie): ";
            std::string line;
            if (!std::getline(std::cin, line) || line.empty()) continue;
            piece = std::toupper(line[0]);
            if (validPieces.find(piece) == std::string::npos) {
                std::cout << "Invalid piece.\n";
            } else if (usedPieces.count(piece)) {
                std::cout << "Piece already taken.\n";
            } else {
                valid = true;
            }
        }
        usedPieces.insert(piece);
        players.push_back(new Player(name, piece));
    }

    board->setPlayers(&players);
    // Attach board as observer to each player
    for (auto* p : players) {
        p->attach(board);
    }
    if (gfx) { for (auto* p : players) p->attach(gfx); }
    board->display();
    if (gfx) gfx->redraw();
    run();
}

void Game::loadGame(const std::string& file) {
    if (!SaveManager::load(file, players, *board)) {
        std::cerr << "Failed to load game.\n";
        return;
    }
    for (auto* p : players) p->attach(board);
    if (gfx) { for (auto* p : players) p->attach(gfx); }
    board->display();
    if (gfx) gfx->redraw();
    run();
}

void Game::saveGame(const std::string& file) {
    SaveManager::save(file, players, *board);
}

std::pair<int,int> Game::rollDice(int d1, int d2) {
    if (d1 >= 0 && d2 >= 0) return {d1, d2};
    std::uniform_int_distribution<int> die(1, 6);
    return {die(rng), die(rng)};
}

int Game::activePlayers() const {
    int count = 0;
    for (auto* p : players) if (!p->isBankrupt()) ++count;
    return count;
}

bool Game::isGameOver() const { return activePlayers() <= 1; }

Player* Game::getWinner() const {
    for (auto* p : players) if (!p->isBankrupt()) return p;
    return nullptr;
}

Player* Game::findPlayer(const std::string& name) const {
    for (auto* p : players) if (p->getName() == name) return p;
    return nullptr;
}

OwnableSquare* Game::findProperty(const std::string& name) const {
    for (int i = 0; i < 40; ++i) {
        auto* sq = dynamic_cast<OwnableSquare*>(board->getSquare(i));
        if (sq && sq->getName() == name) return sq;
    }
    return nullptr;
}

void Game::run() {
    while (!isGameOver() && std::cin) {
        Player& current = *players[currentPlayerIdx];
        if (current.isBankrupt()) {
            currentPlayerIdx = (currentPlayerIdx + 1) % players.size();
            continue;
        }
        runTurn(current);
        if (isGameOver()) break;
    }
    auto* winner = getWinner();
    if (winner) {
        std::cout << "\n*** " << winner->getName() << " wins Watopoly! ***\n";
    }
}

void Game::runTurn(Player& p) {
    hasRolled = false;
    doublesCount = 0;
    std::cout << "\n--- " << p.getName() << "'s turn (position: "
              << board->getSquare(p.getPosition())->getName()
              << ", money: $" << p.getMoney() << ") ---\n";

    if (p.isInTims()) {
        handleTimsTurn(p);
        return;
    }

    bool turnDone = false;
    while (!turnDone) {
        std::string line;
        std::cout << "> ";
        if (!std::getline(std::cin, line)) { turnDone = true; break; }
        if (line.empty()) continue;

        std::istringstream ss(line);
        std::string cmd;
        ss >> cmd;
        cmd = toLower(cmd);

        if (cmd == "roll") {
            if (hasRolled) {
                std::cout << "You have already rolled.\n";
            } else {
                std::string rest;
                std::getline(ss, rest);
                int d1 = -1, d2 = -1;
                if (testingMode) {
                    std::istringstream rs(rest);
                    if (!(rs >> d1 >> d2)) { d1 = -1; d2 = -1; }
                }
                doRoll(p, d1, d2);
                if (!p.isBankrupt() && !hasRolled) {
                    std::cout << "Doubles! Roll again.\n";
                } else if (!p.isBankrupt()) {
                    std::cout << "(Tip: type 'next' to end your turn, or use other commands first)\n";
                }
            }
        } else if (cmd == "next") {
            if (!hasRolled) {
                std::cout << "You must roll first.\n";
            } else {
                turnDone = true;
                currentPlayerIdx = (currentPlayerIdx + 1) % players.size();
            }
        } else if (cmd == "trade") {
            std::string args;
            std::getline(ss, args);
            doTrade(p, args);
            board->display();
            if (gfx) gfx->redraw();
        } else if (cmd == "improve") {
            std::string prop, action;
            ss >> prop >> action;
            doImprove(p, prop, toLower(action) == "buy");
            board->display();
            if (gfx) gfx->redraw();
        } else if (cmd == "mortgage") {
            std::string prop; ss >> prop;
            doMortgage(p, prop);
            board->display();
            if (gfx) gfx->redraw();
        } else if (cmd == "unmortgage") {
            std::string prop; ss >> prop;
            doUnmortgage(p, prop);
            board->display();
            if (gfx) gfx->redraw();
        } else if (cmd == "bankrupt") {
            std::cout << "You can only declare bankruptcy when you owe more than you have.\n";
        } else if (cmd == "assets") {
            p.displayAssets();
        } else if (cmd == "all") {
            doAll();
        } else if (cmd == "save") {
            std::string fname; ss >> fname;
            saveGame(fname);
        } else {
            std::cout << "Unknown command: " << cmd << "\n";
        }
    }
}

void Game::handleTimsTurn(Player& p) {
    std::cout << p.getName() << " is in DC Tims Line (turn " << (p.getTurnsInTims()+1) << " of 3).\n";
    std::cout << "Options: roll (for doubles), pay $50, use tims cup\n";
    if (p.getTurnsInTims() >= 2) {
        std::cout << "Third turn - must leave!\n";
    }

    bool leftTims = false;
    bool turnDone = false;

    while (!turnDone) {
        std::string line;
        std::cout << "> ";
        if (!std::getline(std::cin, line)) { turnDone = true; break; }
        if (line.empty()) continue;

        std::istringstream ss(line);
        std::string cmd;
        ss >> cmd;
        cmd = toLower(cmd);

        if (cmd == "roll") {
            std::string rest;
            std::getline(ss, rest);
            int d1 = -1, d2 = -1;
            if (testingMode) {
                std::istringstream rs(rest);
                if (!(rs >> d1 >> d2)) { d1 = -1; d2 = -1; }
            }
            auto [r1, r2] = rollDice(d1, d2);
            lastDiceSum = r1 + r2;
            std::cout << p.getName() << " rolls " << r1 << " + " << r2 << " = " << lastDiceSum << "\n";
            if (r1 == r2) {
                std::cout << "Doubles! " << p.getName() << " leaves DC Tims Line!\n";
                p.setInTims(false);
                p.setTurnsInTims(0);
                movePlayer(p, r1 + r2);
                leftTims = true;
                turnDone = true;
            } else if (p.getTurnsInTims() >= 2) {
                // Must leave on third turn - pay $50 (use debt handling if can't afford)
                std::cout << "Must leave Tims Line. ";
                if (p.getMoney() >= 50) {
                    std::cout << "Paying $50.\n";
                    p.addMoney(-50);
                } else {
                    std::cout << "Cannot afford $50 - entering debt resolution.\n";
                    handleDebt(p, 50 - p.getMoney(), nullptr);
                    if (!p.isBankrupt()) p.addMoney(-50);
                }
                if (!p.isBankrupt()) {
                    p.setInTims(false);
                    p.setTurnsInTims(0);
                    movePlayer(p, r1 + r2);
                }
                leftTims = true;
                turnDone = true;
            } else {
                p.incrementTurnsInTims();
                std::cout << "No doubles. Staying in Tims Line.\n";
                turnDone = true;
            }
        } else if (cmd == "pay") {
            if (p.getMoney() < 50) {
                std::cout << "Cannot afford $50.\n";
            } else {
                p.addMoney(-50);
                p.setInTims(false);
                p.setTurnsInTims(0);
                leftTims = true;
                std::cout << p.getName() << " pays $50 and leaves Tims Line.\n";
                // Still needs to roll
                std::string rest;
                std::getline(ss, rest);
                int d1 = -1, d2 = -1;
                if (testingMode) {
                    std::istringstream rs(rest);
                    if (!(rs >> d1 >> d2)) { d1 = -1; d2 = -1; }
                }
                auto [r1, r2] = rollDice(d1, d2);
                lastDiceSum = r1 + r2;
                std::cout << p.getName() << " rolls " << r1 << " + " << r2 << "\n";
                movePlayer(p, r1 + r2);
                turnDone = true;
            }
        } else if (cmd == "cup" || cmd == "rimcup") {
            if (p.getTimsCups() == 0) {
                std::cout << "You have no Roll Up the Rim cups.\n";
            } else {
                p.useTimsCup();
                p.setInTims(false);
                p.setTurnsInTims(0);
                leftTims = true;
                std::cout << p.getName() << " uses a Roll Up the Rim cup and leaves Tims Line.\n";
                // Still needs to roll
                int d1 = -1, d2 = -1;
                auto [r1, r2] = rollDice(d1, d2);
                lastDiceSum = r1 + r2;
                std::cout << p.getName() << " rolls " << r1 << " + " << r2 << "\n";
                movePlayer(p, r1 + r2);
                turnDone = true;
            }
        } else if (cmd == "next") {
            if (!leftTims && p.getTurnsInTims() < 2) {
                // Didn't try to leave yet this turn - increment and end turn
                p.incrementTurnsInTims();
                turnDone = true;
                std::cout << "Ending turn in Tims Line.\n";
            } else if (leftTims) {
                turnDone = true;
            } else {
                std::cout << "Must try to leave on your third turn.\n";
            }
        } else if (cmd == "assets") {
            p.displayAssets();
        } else if (cmd == "all") {
            doAll();
        } else if (cmd == "trade") {
            std::string args; std::getline(ss, args);
            doTrade(p, args);
        } else if (cmd == "mortgage") {
            std::string prop; ss >> prop;
            doMortgage(p, prop);
        } else if (cmd == "unmortgage") {
            std::string prop; ss >> prop;
            doUnmortgage(p, prop);
        } else if (cmd == "save") {
            std::string fname; ss >> fname;
            saveGame(fname);
        } else {
            std::cout << "Unknown command.\n";
        }
    }

    currentPlayerIdx = (currentPlayerIdx + 1) % players.size();
    board->display();
    if (gfx) gfx->redraw();
}

void Game::doRoll(Player& p, int d1, int d2) {
    auto [r1, r2] = rollDice(d1, d2);
    lastDiceSum = r1 + r2;
    std::cout << p.getName() << " rolls " << r1 << " + " << r2 << " = " << lastDiceSum << "\n";

    if (r1 == r2) {
        ++doublesCount;
        if (doublesCount == 3) {
            std::cout << "Three doubles! " << p.getName() << " goes to DC Tims Line!\n";
            p.setPosition(10, false);
            p.setInTims(true);
            p.setTurnsInTims(0);
            hasRolled = true;
            doublesCount = 0;
            board->display();
            if (gfx) gfx->redraw();
            return;
        }
        std::cout << "Doubles! Will roll again.\n";
    }

    movePlayer(p, r1 + r2);

    if (p.isBankrupt()) {
        hasRolled = true;
        return;
    }

    // If doubles but player ended up in Tims (via GoToTims or card), end turn
    if (r1 == r2 && !p.isInTims() && doublesCount < 3) {
        hasRolled = false;  // Roll again
    } else {
        hasRolled = true;
    }
    if (!p.isBankrupt()) board->display();
    if (!p.isBankrupt() && gfx) gfx->redraw();
}

void Game::movePlayer(Player& p, int steps) {
    int oldPos = p.getPosition();
    int newPos = (oldPos + steps) % 40;

    // Check passing Collect OSAP (wrapped around but did NOT land on it)
    // Landing on pos 0 is handled by CollectOSAP::landOn to avoid double collection
    if (newPos < oldPos && newPos != 0) {
        std::cout << p.getName() << " passes Collect OSAP and collects $200!\n";
        p.addMoney(200);
    }

    p.setPosition(newPos, false);
    std::cout << p.getName() << " moves to " << board->getSquare(newPos)->getName() << "\n";
    handleLanding(p);
}

void Game::movePlayerTo(Player& p, int pos) {
    int oldPos = p.getPosition();
    // Check if passing Collect OSAP going forward
    if (pos > oldPos || pos == 0) {
        // going forward normally
    } else if (pos < oldPos && pos != 10) {
        // wrapping around - collect OSAP
        std::cout << p.getName() << " passes Collect OSAP and collects $200!\n";
        p.addMoney(200);
    }
    p.setPosition(pos, false);
    handleLanding(p);
}

void Game::handleLanding(Player& p, bool /*fromCard*/) {
    int pos = p.getPosition();
    Square* sq = board->getSquare(pos);

    // Handle GoToTims specially
    if (auto* gtt = dynamic_cast<GoToTims*>(sq)) {
        gtt->landOn(p, *board);
        return;
    }

    // Handle DC Tims Line (landing = just visiting)
    if (pos == 10 && !p.isInTims()) {
        std::cout << p.getName() << " is just visiting DC Tims Line.\n";
        return;
    }
    if (pos == 10 && p.isInTims()) {
        // Already handled (was sent here by card or GoToTims)
        return;
    }

    // Handle ownable squares
    if (auto* ownable = dynamic_cast<OwnableSquare*>(sq)) {
        if (ownable->getOwner() == nullptr) {
            promptBuy(p, ownable);
        } else if (ownable->getOwner() == &p) {
            std::cout << p.getName() << " owns " << sq->getName() << ".\n";
        } else if (!ownable->isMortgaged()) {
            int rent = 0;

            if (auto* ab = dynamic_cast<AcademicBuilding*>(ownable)) {
                rent = ab->getRent(p, *board);
            } else if (auto* res = dynamic_cast<Residence*>(ownable)) {
                rent = res->getRent(p, *board);
            } else if (auto* gym = dynamic_cast<Gym*>(ownable)) {
                // Use dice from this turn (not a re-roll)
                int numOwned = board->countGymsOwned(ownable->getOwner());
                rent = gym->getFee(lastDiceSum, numOwned);
                std::cout << "Gym fee based on dice sum " << lastDiceSum << "\n";
            }

            if (rent > 0) {
                std::cout << p.getName() << " owes $" << rent << " to " << ownable->getOwner()->getName()
                          << " for " << sq->getName() << "\n";
                handleDebt(p, rent, ownable->getOwner());
            }
        } else {
            std::cout << sq->getName() << " is mortgaged. No payment.\n";
        }
        return;
    }

    // Non-ownable: call landOn, then check for card-effect position change
    int prevPos = p.getPosition();
    sq->landOn(p, *board);

    // If a card effect (SLC/NeedlesHall) moved the player, handle the new landing
    if (!p.isBankrupt() && !p.isInTims() && p.getPosition() != prevPos) {
        int newPos = p.getPosition();
        Square* newSq = board->getSquare(newPos);
        // Do not chain into another SLC or NeedlesHall
        if (!dynamic_cast<SLC*>(newSq) && !dynamic_cast<NeedlesHall*>(newSq)) {
            handleLanding(p, true);
        }
    }

    // Debt resolution if any non-ownable square left player with negative balance
    if (!p.isBankrupt() && p.getMoney() < 0) {
        handleDebt(p, -p.getMoney(), nullptr);
    }
}

void Game::promptBuy(Player& p, OwnableSquare* sq) {
    std::cout << sq->getName() << " is unowned. Price: $" << sq->getPrice() << "\n";
    if (p.getMoney() < sq->getPrice()) {
        std::cout << p.getName() << " cannot afford " << sq->getName() << ". Going to auction.\n";
        Auction auction;
        auction.run(sq, players);
        return;
    }
    std::cout << p.getName() << ", buy " << sq->getName() << " for $" << sq->getPrice() << "? (yes/no): ";
    std::string ans;
    std::getline(std::cin, ans);

    if (ans == "yes" || ans == "y") {
        p.spendMoney(sq->getPrice());
        p.addProperty(sq);
        std::cout << p.getName() << " buys " << sq->getName() << "\n";
        // Check if receiving mortgaged property from bank somehow
        if (sq->isMortgaged()) {
            int fee = static_cast<int>(sq->getPrice() * 0.1);
            std::cout << "Property is mortgaged. Pay 10% fee: $" << fee << "\n";
            p.addMoney(-fee);
        }
    } else {
        std::cout << "Going to auction.\n";
        Auction auction;
        auction.run(sq, players);
    }
}

void Game::handleDebt(Player& p, int amount, Player* creditorPlayer) {
    // Player owes 'amount'. Try to collect.
    if (p.getMoney() >= amount) {
        p.addMoney(-amount);
        if (creditorPlayer) creditorPlayer->addMoney(amount);
        return;
    }

    // Can't afford - enter debt resolution loop
    std::cout << p.getName() << " cannot afford $" << amount
              << ". Sell improvements or mortgage to raise funds, or declare bankruptcy.\n";

    while (p.getMoney() < amount) {
        std::cout << p.getName() << " has $" << p.getMoney()
                  << " (needs $" << amount << "). Commands: improve <prop> sell, "
                  << "mortgage <prop>, bankrupt, assets\n";
        std::string line;
        std::cout << ">> ";
        if (!std::getline(std::cin, line)) break;
        if (line.empty()) continue;

        std::istringstream ss(line);
        std::string cmd;
        ss >> cmd;
        cmd = toLower(cmd);

        if (cmd == "bankrupt") {
            handleBankruptcy(p, creditorPlayer);
            return;
        } else if (cmd == "improve") {
            std::string prop, action;
            ss >> prop >> action;
            if (toLower(action) == "sell") {
                doImprove(p, prop, false);
            } else {
                std::cout << "Can only sell improvements to raise money.\n";
            }
        } else if (cmd == "mortgage") {
            std::string prop; ss >> prop;
            doMortgage(p, prop);
        } else if (cmd == "assets") {
            p.displayAssets();
        } else if (cmd == "trade") {
            std::string args; std::getline(ss, args);
            doTrade(p, args);
        } else {
            std::cout << "Invalid command during debt resolution.\n";
        }
    }

    // Now can pay
    if (p.getMoney() >= amount) {
        p.addMoney(-amount);
        if (creditorPlayer) creditorPlayer->addMoney(amount);
        std::cout << p.getName() << " pays $" << amount << "\n";
    } else {
        handleBankruptcy(p, creditorPlayer);
    }
}

void Game::handleBankruptcy(Player& p, Player* creditorPlayer) {
    std::cout << p.getName() << " declares bankruptcy!\n";
    transferAssets(p, creditorPlayer);
    p.declareBankrupt();
    p.setBankrupt(true);

    if (isGameOver()) return;
}

void Game::transferAssets(Player& from, Player* to) {
    // Transfer all money
    if (to) {
        to->addMoney(from.getMoney());
        std::cout << from.getName() << "'s $" << from.getMoney()
                  << " goes to " << to->getName() << "\n";
    } else {
        std::cout << from.getName() << "'s $" << from.getMoney() << " goes to the bank.\n";
    }
    from.setMoney(0);

    // Transfer all properties
    auto props = from.getProperties();  // copy
    for (auto* sq : props) {
        from.removeProperty(sq);
        if (to) {
            // Transfer to creditor
            to->addProperty(sq);
            // If mortgaged, creditor must pay 10%
            if (sq->isMortgaged()) {
                int fee = static_cast<int>(sq->getPrice() * 0.1);
                std::cout << to->getName() << " receives mortgaged " << sq->getName()
                          << " and pays 10% fee: $" << fee << "\n";
                to->addMoney(-fee);
            }
        } else {
            // Return to bank - sell all improvements, unmortgage, auction
            auto* ab = dynamic_cast<AcademicBuilding*>(sq);
            if (ab) ab->setImprovements(0);
            sq->setMortgaged(false);
            // Auction to remaining players
            std::cout << sq->getName() << " goes to auction.\n";
            Auction auction;
            auction.run(sq, players);
        }
    }

    // Destroy tims cups
    from.setTimsCups(0);
}

void Game::doTrade(Player& p, const std::string& args) {
    std::istringstream ss(args);
    std::string targetName, give, receive;
    ss >> targetName >> give >> receive;

    if (give.empty() || receive.empty()) {
        std::cout << "Usage: trade <name> <give> <receive>\n";
        return;
    }

    Player* target = findPlayer(targetName);
    if (!target) {
        std::cout << "Player '" << targetName << "' not found.\n";
        return;
    }
    if (target == &p) {
        std::cout << "Cannot trade with yourself.\n";
        return;
    }

    // Check if both are money (invalid)
    bool giveIsMoney = true, receiveIsMoney = true;
    int giveAmount = 0, receiveAmount = 0;
    try { giveAmount = std::stoi(give); } catch (...) { giveIsMoney = false; }
    try { receiveAmount = std::stoi(receive); } catch (...) { receiveIsMoney = false; }

    if (giveIsMoney && receiveIsMoney) {
        std::cout << "Cannot trade money for money.\n";
        return;
    }

    // Validate property trades
    OwnableSquare* giveProp = nullptr;
    OwnableSquare* receiveProp = nullptr;

    if (!giveIsMoney) {
        giveProp = findProperty(give);
        if (!giveProp) { std::cout << "Property '" << give << "' not found.\n"; return; }
        if (giveProp->getOwner() != &p) { std::cout << p.getName() << " does not own " << give << "\n"; return; }
        // Check no improvements in monopoly
        auto* ab = dynamic_cast<AcademicBuilding*>(giveProp);
        if (ab && board->countImprovementsInBlock(ab->getMonopolyBlock()) > 0) {
            std::cout << "Cannot trade " << give << " - monopoly block has improvements.\n";
            return;
        }
    }

    if (!receiveIsMoney) {
        receiveProp = findProperty(receive);
        if (!receiveProp) { std::cout << "Property '" << receive << "' not found.\n"; return; }
        if (receiveProp->getOwner() != target) {
            std::cout << target->getName() << " does not own " << receive << "\n"; return;
        }
        auto* ab = dynamic_cast<AcademicBuilding*>(receiveProp);
        if (ab && board->countImprovementsInBlock(ab->getMonopolyBlock()) > 0) {
            std::cout << "Cannot trade " << receive << " - monopoly block has improvements.\n";
            return;
        }
    }

    // Offer to target
    std::cout << target->getName() << ", " << p.getName() << " offers: ";
    if (giveIsMoney) std::cout << "$" << giveAmount;
    else std::cout << give;
    std::cout << " for ";
    if (receiveIsMoney) std::cout << "$" << receiveAmount;
    else std::cout << receive;
    std::cout << ". Accept? (accept/reject): ";

    std::string response;
    std::getline(std::cin, response);

    if (response != "accept") {
        std::cout << "Trade rejected.\n";
        return;
    }

    // Execute trade
    if (giveIsMoney) {
        if (p.getMoney() < giveAmount) { std::cout << p.getName() << " cannot afford.\n"; return; }
        p.addMoney(-giveAmount);
        target->addMoney(giveAmount);
    } else {
        p.removeProperty(giveProp);
        target->addProperty(giveProp);
        if (giveProp->isMortgaged()) {
            int fee = static_cast<int>(giveProp->getPrice() * 0.1);
            std::cout << target->getName() << " receives mortgaged " << give
                      << " and pays 10% fee: $" << fee << "\n";
            target->addMoney(-fee);
        }
    }

    if (receiveIsMoney) {
        if (target->getMoney() < receiveAmount) { std::cout << target->getName() << " cannot afford.\n"; return; }
        target->addMoney(-receiveAmount);
        p.addMoney(receiveAmount);
    } else {
        target->removeProperty(receiveProp);
        p.addProperty(receiveProp);
        if (receiveProp->isMortgaged()) {
            int fee = static_cast<int>(receiveProp->getPrice() * 0.1);
            std::cout << p.getName() << " receives mortgaged " << receive
                      << " and pays 10% fee: $" << fee << "\n";
            p.addMoney(-fee);
        }
    }

    std::cout << "Trade complete.\n";
}

void Game::doImprove(Player& p, const std::string& prop, bool buy) {
    OwnableSquare* sq = findProperty(prop);
    if (!sq) { std::cout << "Property '" << prop << "' not found.\n"; return; }
    if (sq->getOwner() != &p) { std::cout << p.getName() << " does not own " << prop << "\n"; return; }

    auto* ab = dynamic_cast<AcademicBuilding*>(sq);
    if (!ab) { std::cout << prop << " is not an academic building.\n"; return; }

    if (buy) {
        ab->improve(p, *board);
    } else {
        ab->sellImprovement(p, *board);
    }
}

void Game::doMortgage(Player& p, const std::string& prop) {
    OwnableSquare* sq = findProperty(prop);
    if (!sq) { std::cout << "Property '" << prop << "' not found.\n"; return; }
    if (sq->getOwner() != &p) { std::cout << p.getName() << " does not own " << prop << "\n"; return; }

    auto* ab = dynamic_cast<AcademicBuilding*>(sq);
    if (ab && ab->getImprovements() > 0) {
        std::cout << "Must sell improvements before mortgaging.\n";
        return;
    }
    sq->mortgage();
}

void Game::doUnmortgage(Player& p, const std::string& prop) {
    OwnableSquare* sq = findProperty(prop);
    if (!sq) { std::cout << "Property '" << prop << "' not found.\n"; return; }
    if (sq->getOwner() != &p) { std::cout << p.getName() << " does not own " << prop << "\n"; return; }
    sq->unmortgage();
}

void Game::doAll() const {
    for (auto* p : players) {
        if (!p->isBankrupt()) p->displayAssets();
    }
}

// Needed to declare handleTimsTurn - add declaration to game.h
