#include "savemanager.h"
#include "board.h"
#include "player.h"
#include "square.h"
#include "ownablesquare.h"
#include "academicbuilding.h"
#include "residence.h"
#include "gym.h"
#include <fstream>
#include <iostream>
#include <sstream>

// Save file format (from spec):
// numPlayers
// player1 char TimsCups money position [inTims turnsInTims]
// ...
// AL owner improvements
// ML owner improvements
// MKV owner improvements
// ... (all ownable squares in board order)

// Ownable squares in board order (positions 1,3,5,6,8,9,11,12,13,14,15,16,18,19,21,23,24,25,26,27,28,29,31,32,34,36,37,39):
static const char* OWNABLE_NAMES[] = {
    "AL","ML","MKV","ECH","PAS","HH","RCH","PAC","DWE","CPH",
    "UWP","LHI","BMH","OPT","EV1","EV2","EV3","V1","PHYS","B1",
    "CIF","B2","EIT","ESC","C2","REV","MC","DC"
};
static const int NUM_OWNABLES = 28;

void SaveManager::save(const std::string& file, const std::vector<std::unique_ptr<Player>>& players, const Board& board) {
    std::ofstream out(file);
    if (!out) {
        std::cerr << "Cannot open file for saving: " << file << "\n";
        return;
    }

    out << players.size() << "\n";
    for (const auto& p : players) {
        out << p->getName() << " " << p->getPiece() << " "
            << p->getTimsCups() << " " << p->getMoney() << " "
            << p->getPosition();
        if (p->isInTims()) {
            out << " 1 " << p->getTurnsInTims();
        } else if (p->getPosition() == 10) {
            out << " 0";  // visiting tims, not in line
        }
        out << "\n";
    }

    // Write ownable properties in board order
    for (int i = 0; i < NUM_OWNABLES; ++i) {
        int pos = board.findSquare(OWNABLE_NAMES[i]);
        if (pos == -1) continue;
        auto* sq = dynamic_cast<OwnableSquare*>(board.getSquare(pos));
        if (!sq) continue;

        std::string ownerName = "BANK";
        if (sq->getOwner()) ownerName = sq->getOwner()->getName();

        int improvements = 0;
        if (sq->isMortgaged()) {
            improvements = -1;
        } else {
            auto* ab = dynamic_cast<AcademicBuilding*>(sq);
            if (ab) improvements = ab->getImprovements();
        }
        out << OWNABLE_NAMES[i] << " " << ownerName << " " << improvements << "\n";
    }
    std::cout << "Game saved to " << file << "\n";
}

bool SaveManager::load(const std::string& file, std::vector<std::unique_ptr<Player>>& players, Board& board) {
    std::ifstream in(file);
    if (!in) {
        std::cerr << "Cannot open file: " << file << "\n";
        return false;
    }

    int numPlayers;
    in >> numPlayers;
    in.ignore();

    players.clear();
    for (int i = 0; i < numPlayers; ++i) {
        std::string line;
        std::getline(in, line);
        std::istringstream ss(line);

        std::string name;
        char piece;
        int cups, money, position;
        ss >> name >> piece >> cups >> money >> position;

        // Validate loaded values
        if (name == "BANK") {
            std::cerr << "Invalid save file: player named 'BANK' is not allowed.\n";
            players.clear();
            return false;
        }
        if (money < 0) {
            std::cerr << "Invalid save file: negative money for player " << name << ".\n";
            players.clear();
            return false;
        }
        if (position == 30) {
            std::cerr << "Invalid save file: position 30 (Go To Tims) is not a valid starting position.\n";
            players.clear();
            return false;
        }

        auto p = std::make_unique<Player>(name, piece, money);
        p->setPosition(position, false);
        p->setTimsCups(cups);

        if (position == 10) {
            int inTimsFlag = 0;
            ss >> inTimsFlag;
            if (inTimsFlag == 1) {
                int turns;
                ss >> turns;
                if (turns >= 3) {
                    std::cerr << "Invalid save file: turnsInTims value " << turns << " is invalid (must be < 3).\n";
                    players.clear();
                    return false;
                }
                p->setInTims(true);
                p->setTurnsInTims(turns);
            }
        }
        players.push_back(std::move(p));
    }

    // Validate total Tims cups across all players
    int totalCups = 0;
    for (const auto& p : players) totalCups += p->getTimsCups();
    if (totalCups > 4) {
        std::cerr << "Invalid save file: total Roll Up the Rim cups (" << totalCups << ") exceeds 4.\n";
        players.clear();
        return false;
    }

    // Read ownable squares
    for (int i = 0; i < NUM_OWNABLES; ++i) {
        std::string propName, ownerName;
        int improvements;
        in >> propName >> ownerName >> improvements;

        int pos = board.findSquare(propName);
        if (pos == -1) continue;
        auto* sq = dynamic_cast<OwnableSquare*>(board.getSquare(pos));
        if (!sq) continue;

        if (ownerName != "BANK") {
            // Find player with this name
            for (const auto& p : players) {
                if (p->getName() == ownerName) {
                    p->addProperty(sq);
                    break;
                }
            }
        }

        if (improvements == -1) {
            sq->setMortgaged(true);
        } else {
            auto* ab = dynamic_cast<AcademicBuilding*>(sq);
            if (ab) ab->setImprovements(improvements);
        }
    }

    board.setPlayers(&players);
    std::cout << "Game loaded from " << file << "\n";
    return true;
}
