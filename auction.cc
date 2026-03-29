#include "auction.h"
#include "ownablesquare.h"
#include "player.h"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

void Auction::run(OwnableSquare* property, std::vector<std::unique_ptr<Player>>& players) {
    std::cout << "\n=== Auction for " << property->getName()
              << " (price: $" << property->getPrice() << ") ===\n";

    std::vector<Player*> bidders;
    for (const auto& p : players) {
        if (!p->isBankrupt()) bidders.push_back(p.get());
    }

    if (bidders.empty()) {
        std::cout << "No active players to bid. Property remains unowned.\n";
        return;
    }

    int currentBid = 0;
    Player* currentWinner = nullptr;
    std::vector<bool> withdrawn(bidders.size(), false);

    bool auctionOver = false;
    while (!auctionOver) {
        int activeBidders = 0;
        for (int i = 0; i < (int)bidders.size(); ++i) {
            if (!withdrawn[i]) ++activeBidders;
        }
        if (activeBidders <= 1) {
            auctionOver = true;
            break;
        }

        for (int i = 0; i < (int)bidders.size(); ++i) {
            if (withdrawn[i]) continue;

            // Check how many are still in
            int remaining = 0;
            for (int j = 0; j < (int)bidders.size(); ++j)
                if (!withdrawn[j]) ++remaining;
            if (remaining <= 1) { auctionOver = true; break; }

            Player* p = bidders[i];
            std::cout << p->getName() << " (has $" << p->getMoney() << "), "
                      << "current bid: $" << currentBid << ". Raise or withdraw? (amount/withdraw): ";
            std::string input;
            std::getline(std::cin, input);

            if (input == "withdraw" || input == "w") {
                withdrawn[i] = true;
                std::cout << p->getName() << " withdraws.\n";
            } else {
                try {
                    int bid = std::stoi(input);
                    if (bid <= currentBid) {
                        std::cout << "Bid must be higher than current bid ($" << currentBid << "). Withdrawing.\n";
                        withdrawn[i] = true;
                    } else if (bid > p->getMoney()) {
                        std::cout << p->getName() << " cannot afford $" << bid << ". Withdrawing.\n";
                        withdrawn[i] = true;
                    } else {
                        currentBid = bid;
                        currentWinner = p;
                        std::cout << p->getName() << " bids $" << bid << "\n";
                    }
                } catch (...) {
                    std::cout << "Invalid input. " << p->getName() << " withdraws.\n";
                    withdrawn[i] = true;
                }
            }
        }
    }

    // Find winner (last non-withdrawn bidder)
    if (currentWinner == nullptr) {
        for (int i = 0; i < (int)bidders.size(); ++i) {
            if (!withdrawn[i]) { currentWinner = bidders[i]; break; }
        }
    }

    if (currentWinner && currentBid > 0) {
        currentWinner->spendMoney(currentBid);
        currentWinner->addProperty(property);
        std::cout << currentWinner->getName() << " wins auction for " << property->getName()
                  << " with bid $" << currentBid << "\n";
    } else if (currentWinner && currentBid == 0) {
        // No bids made - if only one bidder, they can get it for free or $1
        std::cout << "No bids made. " << currentWinner->getName()
                  << " can acquire " << property->getName() << " for $1.\n";
        std::cout << "Accept? (yes/no): ";
        std::string ans;
        std::getline(std::cin, ans);
        if (ans == "yes" || ans == "y") {
            currentWinner->spendMoney(1);
            currentWinner->addProperty(property);
            std::cout << currentWinner->getName() << " acquires " << property->getName() << " for $1\n";
        } else {
            std::cout << property->getName() << " remains unowned.\n";
        }
    } else {
        std::cout << "Property remains unowned.\n";
    }
}
