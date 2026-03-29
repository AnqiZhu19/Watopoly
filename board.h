#pragma once
#include "observer.h"
#include "player.h"
#include <array>
#include <vector>
#include <memory>
#include <string>
#include <random>

class Square;

class Board : public Observer {
    std::array<std::unique_ptr<Square>, 40> squares;
    std::vector<std::unique_ptr<Player>>* players;  // pointer to game's player list (not owned)

public:
    Board(std::mt19937& rng);
    ~Board();

    Square* getSquare(int pos) const { return squares[pos].get(); }
    void setPlayers(std::vector<std::unique_ptr<Player>>* p) { players = p; }

    void notify() override;   // redraws board
    void display() const;

    // Count how many residences/gyms of same type owner has
    int countResidencesOwned(const Player* owner) const;
    int countGymsOwned(const Player* owner) const;

    // Count academic buildings in same monopoly block owned by same player
    bool ownsMonopoly(const Player* owner, const std::string& block) const;
    int countImprovementsInBlock(const std::string& block) const;

    // Check total tims cups in play
    int totalTimsCups() const;

    // Helper to get square position by name
    int findSquare(const std::string& name) const;

private:
    void buildBoard(std::mt19937& rng);
    std::string buildDisplayRow(int positions[], int count, bool academic_style) const;
    std::string getPlayersAt(int pos) const;
};
