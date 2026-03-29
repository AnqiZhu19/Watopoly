#pragma once
#include <string>
#include <random>

class Square;

class SquareFactory {
public:
    virtual Square* createSquare(const std::string& name, int pos, std::mt19937& rng) = 0;
    virtual ~SquareFactory() = default;
};

class NormalSquareFactory : public SquareFactory {
public:
    Square* createSquare(const std::string& name, int pos, std::mt19937& rng) override;
};

class TestingSquareFactory : public SquareFactory {
public:
    Square* createSquare(const std::string& name, int pos, std::mt19937& rng) override;
};
