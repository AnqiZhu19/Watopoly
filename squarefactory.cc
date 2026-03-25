#include "squarefactory.h"
#include "square.h"
#include "academicbuilding.h"
#include "residence.h"
#include "gym.h"
#include "nonownable.h"
#include "slc.h"
#include "needleshall.h"
#include <stdexcept>

// NormalSquareFactory creates all squares based on position
// (Board already constructs squares; factory is for extension/testing)
Square* NormalSquareFactory::createSquare(const std::string& name, int pos, std::mt19937& rng) {
    // This factory is called by Board::buildBoard if needed
    // For now, returns nullptr (board builds directly)
    return nullptr;
}

// TestingSquareFactory: same as normal but could use fixed-seed RNG
Square* TestingSquareFactory::createSquare(const std::string& name, int pos, std::mt19937& rng) {
    return nullptr;
}
