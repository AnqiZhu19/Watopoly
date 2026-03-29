#include "board.h"
#include "square.h"
#include "player.h"
#include "academicbuilding.h"
#include "residence.h"
#include "gym.h"
#include "nonownable.h"
#include "slc.h"
#include "needleshall.h"
#include "ownablesquare.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <array>

// Board positions:
// 0:CollectOSAP  1:AL  2:SLC  3:ML  4:Tuition  5:MKV  6:ECH  7:NeedlesHall
// 8:PAS  9:HH  10:DCTimsLine  11:RCH  12:PAC  13:DWE  14:CPH  15:SLC  16:LHI
// 17:BMH  18:OPT  19:UWP  20:GooseNesting  21:EV1  22:NeedlesHall  23:EV2
// 24:EV3  25:V1  26:PHYS  27:B1  28:CIF  29:B2  30:GoToTims  31:EIT  32:ESC
// 33:SLC  34:C2  35:NeedlesHall  36:REV  37:MC  38:CoopFee  39:DC

void Board::buildBoard(std::mt19937& rng) {
    // Academic building data: {name, pos, price, improveCost, block, tuition[6]}
    struct ABData {
        const char* name; int pos, price, improveCost;
        const char* block;
        std::array<int,6> tuition;
    };
    static const ABData abData[] = {
        {"AL",   1,  40,  50, "Arts1",  {2,  10,  30,  90,  160, 250}},
        {"ML",   3,  60,  50, "Arts1",  {4,  20,  60,  180, 320, 450}},
        {"ECH",  6,  100, 50, "Arts2",  {6,  30,  90,  270, 400, 550}},
        {"PAS",  8,  100, 50, "Arts2",  {6,  30,  90,  270, 400, 550}},
        {"HH",   9,  120, 50, "Arts2",  {8,  40,  100, 300, 450, 600}},
        {"RCH",  11, 140, 100,"Eng",    {10, 50,  150, 450, 625, 750}},
        {"DWE",  13, 140, 100,"Eng",    {10, 50,  150, 450, 625, 750}},
        {"CPH",  14, 160, 100,"Eng",    {12, 60,  180, 500, 700, 900}},
        {"LHI",  16, 180, 100,"Health", {14, 70,  200, 550, 750, 950}},
        {"BMH",  18, 180, 100,"Health", {14, 70,  200, 550, 750, 950}},
        {"OPT",  19, 200, 100,"Health", {16, 80,  220, 600, 800, 1000}},
        {"EV1",  21, 220, 150,"Env",    {18, 90,  250, 700, 875, 1050}},
        {"EV2",  23, 220, 150,"Env",    {18, 90,  250, 700, 875, 1050}},
        {"EV3",  24, 240, 150,"Env",    {20, 100, 300, 750, 925, 1100}},
        {"PHYS", 26, 260, 150,"Sci1",   {22, 110, 330, 800, 975, 1150}},
        {"B1",   27, 260, 150,"Sci1",   {22, 110, 330, 800, 975, 1150}},
        {"B2",   29, 280, 150,"Sci1",   {24, 120, 360, 850, 1025,1200}},
        {"EIT",  31, 300, 200,"Sci2",   {26, 130, 390, 900, 1100,1275}},
        {"ESC",  32, 300, 200,"Sci2",   {26, 130, 390, 900, 1100,1275}},
        {"C2",   34, 320, 200,"Sci2",   {28, 150, 450, 1000,1200,1400}},
        {"MC",   37, 350, 200,"Math",   {35, 175, 500, 1100,1300,1500}},
        {"DC",   39, 400, 200,"Math",   {50, 200, 600, 1400,1700,2000}},
    };

    // Create academic buildings
    for (auto& ab : abData) {
        squares[ab.pos] = std::make_unique<AcademicBuilding>(ab.name, ab.pos, ab.price,
                                                              ab.improveCost, ab.block, ab.tuition);
    }

    // Residences (price = 200)
    squares[5]  = std::make_unique<Residence>("MKV", 5);
    squares[15] = std::make_unique<Residence>("UWP", 15);
    squares[25] = std::make_unique<Residence>("V1",  25);
    squares[36] = std::make_unique<Residence>("REV", 36);

    // Gyms (price = 150)
    squares[12] = std::make_unique<Gym>("PAC", 12);
    squares[28] = std::make_unique<Gym>("CIF", 28);

    // Non-ownable squares
    squares[0]  = std::make_unique<CollectOSAP>();
    squares[10] = std::make_unique<DCTimsLine>();
    squares[20] = std::make_unique<GooseNesting>();
    squares[30] = std::make_unique<GoToTims>();
    squares[4]  = std::make_unique<Tuition>();
    squares[38] = std::make_unique<CoopFee>();

    // SLC squares
    squares[2]  = std::make_unique<SLC>("SLC", 2,  rng);
    squares[17] = std::make_unique<SLC>("SLC", 17, rng);
    squares[33] = std::make_unique<SLC>("SLC", 33, rng);

    // Needles Hall squares
    squares[7]  = std::make_unique<NeedlesHall>("Needles Hall", 7,  rng);
    squares[22] = std::make_unique<NeedlesHall>("Needles Hall", 22, rng);
    squares[35] = std::make_unique<NeedlesHall>("Needles Hall", 35, rng);
}

Board::Board(std::mt19937& rng) : players{nullptr} {
    buildBoard(rng);
}

Board::~Board() = default;

void Board::notify() {
    display();
}

// Returns players on given position as a string (up to 7 chars)
std::string Board::getPlayersAt(int pos) const {
    if (!players) return "";
    std::string result;
    for (const auto& p : *players) {
        if (!p->isBankrupt() && p->getPosition() == pos) {
            result += p->getPiece();
        }
    }
    return result;
}

int Board::countResidencesOwned(const Player* owner) const {
    int count = 0;
    for (const auto& sq : squares) {
        auto* res = dynamic_cast<Residence*>(sq.get());
        if (res && res->getOwner() == owner) ++count;
    }
    return count;
}

int Board::countGymsOwned(const Player* owner) const {
    int count = 0;
    for (const auto& sq : squares) {
        auto* gym = dynamic_cast<Gym*>(sq.get());
        if (gym && gym->getOwner() == owner) ++count;
    }
    return count;
}

bool Board::ownsMonopoly(const Player* owner, const std::string& block) const {
    if (!owner) return false;
    for (const auto& sq : squares) {
        auto* ab = dynamic_cast<AcademicBuilding*>(sq.get());
        if (ab && ab->getMonopolyBlock() == block) {
            if (ab->getOwner() != owner) return false;
        }
    }
    return true;
}

int Board::countImprovementsInBlock(const std::string& block) const {
    int count = 0;
    for (const auto& sq : squares) {
        auto* ab = dynamic_cast<AcademicBuilding*>(sq.get());
        if (ab && ab->getMonopolyBlock() == block) count += ab->getImprovements();
    }
    return count;
}

int Board::totalTimsCups() const {
    if (!players) return 0;
    int total = 0;
    for (const auto& p : *players) total += p->getTimsCups();
    return total;
}

int Board::findSquare(const std::string& name) const {
    for (const auto& sq : squares) {
        if (sq->getName() == name) return sq->getPosition();
    }
    return -1;
}

// Display: build board as string
// Each cell: 7 chars wide. Board: 11 columns of cells.
// Total width: 11*7 + 12 borders = 89 chars.

// Get 4 display lines for a square at position pos
static void getLines(const Square* sq, const std::string& players, std::string lines[4]) {
    sq->getDisplayLines(lines, players);
}

void Board::display() const {
    // Board layout:
    // Top row (5 lines): positions 20..30 left to right
    // Side rows (5 lines each x 9): left=19..11 top-to-bottom, right=31..39 top-to-bottom
    // Bottom row (5 lines): positions 10..0 left to right

    const std::string HBORDER = "_________________________________________________________________________________________";
    const std::string SEP     = "|_______|_______|_______|_______|_______|_______|_______|_______|_______|_______|_______|";

    // Top border
    std::cout << HBORDER << "\n";

    // Top row: positions 20,21,22,23,24,25,26,27,28,29,30
    int topRow[] = {20,21,22,23,24,25,26,27,28,29,30};
    std::string topLines[11][4];
    for (int c = 0; c < 11; ++c) {
        std::string pl = getPlayersAt(topRow[c]);
        getLines(squares[topRow[c]].get(), pl, topLines[c]);
    }
    for (int row = 0; row < 4; ++row) {
        std::cout << "|";
        for (int c = 0; c < 11; ++c) {
            std::cout << topLines[c][row] << "|";
        }
        std::cout << "\n";
    }
    std::cout << SEP << "\n";

    // Middle rows: left col 19..11, right col 31..39 (9 rows)
    // Each side square: 5 lines (line 0-3 content + line 4 = ___ border)
    // Middle width = 89 - 2*9 - 2 = 89 - 20 = 69 chars interior
    // Actually: |LLLLLLL| + middle spaces + |RRRRRRR| = 1+7+1 + 55 + 1+7+1 = 73? No...
    // Width of one side cell = 9 chars (|LLLLLLL|)
    // Remaining interior = 89 - 9 - 9 = 71 chars
    // But we need the interior to be exactly 71 chars wide (including the | separators)
    // Actually full row = |LLLLLLL|<71 spaces>|RRRRRRR| = 1+7+1+71+7+1 = nope
    // Let me count from board.txt:
    // |OPT    |                                                                       |EIT    |
    // Each side cell = "|OPT    |" = 9 chars
    // Total = 89, so interior = 89 - 18 = 71 spaces

    int leftCol[]  = {19,18,17,16,15,14,13,12,11};  // top to bottom
    int rightCol[] = {31,32,33,34,35,36,37,38,39};  // top to bottom

    for (int i = 0; i < 9; ++i) {
        std::string lLines[4], rLines[4];
        std::string lPl = getPlayersAt(leftCol[i]);
        std::string rPl = getPlayersAt(rightCol[i]);
        getLines(squares[leftCol[i]].get(), lPl, lLines);
        getLines(squares[rightCol[i]].get(), rPl, rLines);

        // Print 5 lines for this row
        // Lines 0-3: content
        for (int row = 0; row < 4; ++row) {
            // Special WATOPOLY text in the center when i==4 (UWP/REV row) rows 0-4
            std::string center(71, ' ');
            if (i == 4) {
                // UWP/REV row - show WATOPOLY text
                static const char* watoLines[] = {
                    "  # # ## ##### ### ### ### # # #  ",
                    "  # # #  # # # #  # #   # # # #  ",
                    "  ### #### # ### ## # # # #  #   ",
                    "  ### #  # # #   # # # # #   #   ",
                    "  ##### #  # ### # ### ## # #    ",
                };
                // Place centered in 71 chars
                std::string wline = watoLines[row];
                int offset = (71 - (int)wline.size()) / 2;
                if (offset < 0) offset = 0;
                center = std::string(offset, ' ') + wline.substr(0, 71 - offset);
                if ((int)center.size() < 71) center += std::string(71 - center.size(), ' ');
            }
            std::cout << "|" << lLines[row] << "|" << center << "|" << rLines[row] << "|\n";
        }
        // Line 4: bottom border for this row
        std::cout << "|_______|" << std::string(71, ' ') << "|_______|\n";
    }

    // Bottom separator (doubles as top border of bottom row)
    // Already printed by last row's border above
    // Now bottom row: positions 10,9,8,7,6,5,4,3,2,1,0
    int botRow[] = {10,9,8,7,6,5,4,3,2,1,0};
    std::string botLines[11][4];
    for (int c = 0; c < 11; ++c) {
        std::string pl = getPlayersAt(botRow[c]);
        getLines(squares[botRow[c]].get(), pl, botLines[c]);
    }
    for (int row = 0; row < 4; ++row) {
        std::cout << "|";
        for (int c = 0; c < 11; ++c) {
            std::cout << botLines[c][row] << "|";
        }
        std::cout << "\n";
    }
    std::cout << SEP << "\n";
}
