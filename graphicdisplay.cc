#include "graphicdisplay.h"
#include "window.h"
#include "board.h"
#include "player.h"
#include "ownablesquare.h"
#include "academicbuilding.h"
#include <string>
#include <vector>

// ─── Grid coordinate tables ────────────────────────────────────────────────
// Board layout (matches text display in board.cc):
//   Top row  (row 0):  positions 20..30 left→right   (col 0..10)
//   Left col (col 0):  positions 19..11 top→bottom   (row 1..9)
//   Right col(col 10): positions 31..39 top→bottom   (row 1..9)
//   Bottom row(row 10):positions 10..0  left→right   (col 0..10)

static const int POS_COL[40] = {
    // pos 0..10 (bottom row, right→left: col 10 down to 0)
    10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0,
    // pos 11..19 (left col, bottom→top: row 9 down to 1)
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    // pos 20..30 (top row, left→right: col 0..10)
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
    // pos 31..39 (right col, top→bottom: col 10)
    10, 10, 10, 10, 10, 10, 10, 10, 10
};

static const int POS_ROW[40] = {
    // pos 0..10 (bottom row)
    10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
    // pos 11..19 (left col, rows 9 down to 1)
    9, 8, 7, 6, 5, 4, 3, 2, 1,
    // pos 20..30 (top row)
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // pos 31..39 (right col, rows 1..9)
    1, 2, 3, 4, 5, 6, 7, 8, 9
};

// Player piece background colors (cycle through 6 distinct colors)
static const int PIECE_COLORS[6] = {
    Xwindow::Red, Xwindow::Blue, Xwindow::Green,
    Xwindow::Orange, Xwindow::Magenta, Xwindow::Cyan
};

// ─── Construction ──────────────────────────────────────────────────────────

GraphicDisplay::GraphicDisplay(Board* b, std::vector<Player*>* p)
    : window{new Xwindow(BOARD_PX, BOARD_PX + PANEL_H)},
      board{b}, players{p} {}

GraphicDisplay::~GraphicDisplay() {
    delete window;
}

// ─── Observer interface ────────────────────────────────────────────────────

void GraphicDisplay::notify() { redraw(); }

void GraphicDisplay::redraw() {
    // Clear board zone to white
    window->fillRectangle(0, 0, BOARD_PX, BOARD_PX + PANEL_H, Xwindow::White);
    for (int pos = 0; pos < 40; ++pos) drawSquare(pos);
    drawCenter();
    drawInfoPanel();
}

// ─── Helpers ───────────────────────────────────────────────────────────────

int GraphicDisplay::squareCol(int pos) const { return POS_COL[pos]; }
int GraphicDisplay::squareRow(int pos) const { return POS_ROW[pos]; }

int GraphicDisplay::blockColor(const std::string& block) const {
    if (block == "Arts1")  return Xwindow::Magenta;
    if (block == "Arts2")  return Xwindow::Cyan;
    if (block == "Eng")    return Xwindow::Orange;
    if (block == "Health") return Xwindow::Red;
    if (block == "Env")    return Xwindow::Green;
    if (block == "Sci1")   return Xwindow::Blue;
    if (block == "Sci2")   return Xwindow::Yellow;
    if (block == "Math")   return Xwindow::Brown;
    return Xwindow::White;
}

// ─── Draw a single square ─────────────────────────────────────────────────

void GraphicDisplay::drawSquare(int pos) {
    int col = squareCol(pos);
    int row = squareRow(pos);
    int x = col * CELL;
    int y = row * CELL;

    // Background
    window->fillRectangle(x, y, CELL, CELL, Xwindow::White);

    Square* sq = board->getSquare(pos);

    // Color stripe for ownable squares (12px on the inward edge)
    static const int STRIPE = 12;
    if (auto* ab = dynamic_cast<AcademicBuilding*>(sq)) {
        int color = blockColor(ab->getMonopolyBlock());
        if (row == 10) {
            // Bottom row: stripe at top edge
            window->fillRectangle(x, y, CELL, STRIPE, color);
        } else if (row == 0) {
            // Top row: stripe at bottom edge
            window->fillRectangle(x, y + CELL - STRIPE, CELL, STRIPE, color);
        } else if (col == 0) {
            // Left col: stripe at right edge
            window->fillRectangle(x + CELL - STRIPE, y, STRIPE, CELL, color);
        } else {
            // Right col: stripe at left edge
            window->fillRectangle(x, y, STRIPE, CELL, color);
        }

        // Improvement dots (6×6 px each, evenly spaced within cell)
        const int DOT_STEP = (CELL - 10) / 4;  // scales with CELL size
        int impr = ab->getImprovements();
        for (int i = 0; i < 5; ++i) {
            int dotColor = (i < impr) ? Xwindow::Orange : Xwindow::White;
            int dotX, dotY;
            if (row == 10) {
                dotX = x + 4 + i * DOT_STEP;
                dotY = y + STRIPE + 2;
            } else if (row == 0) {
                dotX = x + 4 + i * DOT_STEP;
                dotY = y + CELL - STRIPE - 8;
            } else if (col == 0) {
                dotX = x + CELL - STRIPE - 8;
                dotY = y + 4 + i * DOT_STEP;
            } else {
                dotX = x + STRIPE + 2;
                dotY = y + 4 + i * DOT_STEP;
            }
            window->fillRectangle(dotX, dotY, 6, 6, dotColor);
            // Border for empty dots
            if (i >= impr) {
                window->fillRectangle(dotX,     dotY,     6, 1, Xwindow::Black);
                window->fillRectangle(dotX,     dotY + 5, 6, 1, Xwindow::Black);
                window->fillRectangle(dotX,     dotY,     1, 6, Xwindow::Black);
                window->fillRectangle(dotX + 5, dotY,     1, 6, Xwindow::Black);
            }
        }
    }

    // Square name (truncated to fit)
    std::string name = sq->getName();
    if (name.length() > 7) name = name.substr(0, 7);
    window->drawString(x + 2, y + CELL/2, name, Xwindow::Black);

    // Ownership / mortgage indicator (bottom-left corner)
    if (auto* own = dynamic_cast<OwnableSquare*>(sq)) {
        if (own->isMortgaged()) {
            window->drawString(x + 2, y + CELL - 4, "M", Xwindow::Red);
        } else if (own->getOwner() != nullptr) {
            std::string ownerPiece(1, own->getOwner()->getPiece());
            window->drawString(x + 2, y + CELL - 4, ownerPiece, Xwindow::Black);
        }
    }

    // Player pieces at this square
    std::vector<Player*> here;
    if (players) {
        for (auto* p : *players) {
            if (!p->isBankrupt() && p->getPosition() == pos) {
                here.push_back(p);
            }
        }
    }
    // Draw up to 6 pieces in a 2-column grid in the center of the cell
    int pieceSize = 16;
    int startX = x + CELL/2 - pieceSize;
    int startY = y + CELL/2 - pieceSize;
    for (int i = 0; i < (int)here.size() && i < 6; ++i) {
        int px = startX + (i % 2) * (pieceSize + 2);
        int py = startY + (i / 2) * (pieceSize + 2);
        // Use global player index for color to match the info panel
        int globalIdx = 0;
        for (int j = 0; j < (int)players->size(); ++j) {
            if ((*players)[j] == here[i]) { globalIdx = j; break; }
        }
        int bgColor = PIECE_COLORS[globalIdx % 6];
        window->fillRectangle(px, py, pieceSize, pieceSize, bgColor);
        std::string piece(1, here[i]->getPiece());
        window->drawString(px + 3, py + pieceSize - 3, piece, Xwindow::White);
    }

    // Black border (4 thin lines, 1px)
    window->fillRectangle(x,            y,            CELL, 1,    Xwindow::Black);
    window->fillRectangle(x,            y + CELL - 1, CELL, 1,    Xwindow::Black);
    window->fillRectangle(x,            y,            1,    CELL,  Xwindow::Black);
    window->fillRectangle(x + CELL - 1, y,            1,    CELL,  Xwindow::Black);
}

// ─── Center logo ──────────────────────────────────────────────────────────

void GraphicDisplay::drawCenter() {
    // The inner 9×9 cells (cols 1-9, rows 1-9) are unused board space.
    // Fill it and draw the WATOPOLY pixel-art logo there.
    const int cx = CELL;
    const int cy = CELL;
    const int cw = 9 * CELL;
    const int ch = 9 * CELL;

    // Light background for center
    window->fillRectangle(cx, cy, cw, ch, Xwindow::White);

    // WATOPOLY pixel-art rows (same pattern as text display)
    static const char* logo[4] = {
        "  # # ## ##### ### ### ### # # #  ",
        "  # # #  # # # #  # #   # # # #  ",
        "  ### #### # ### ## # # # #  #   ",
        "  ### #  # # #   # # # # #   #   ",
    };
    // Each '#' becomes a PS×PS filled block; characters spaced CW px apart.
    const int PS  = 7;                   // block pixel size
    const int CW  = 9;                   // x step per character
    const int RH  = 11;                  // y step per row
    const int COLS = 34;                 // max char columns in logo
    const int totalW = COLS * CW;
    const int totalH = 4 * RH;
    const int lx = cx + (cw - totalW) / 2;
    const int ly = cy + (ch - totalH) / 2;

    for (int r = 0; r < 4; ++r) {
        for (int c = 0; logo[r][c] != '\0' && c < COLS; ++c) {
            if (logo[r][c] == '#') {
                window->fillRectangle(lx + c * CW, ly + r * RH, PS, PS, Xwindow::Blue);
            }
        }
    }

    // Small text label centered just above the pixel art
    const std::string title = "WATOPOLY";
    window->drawString(lx + (totalW - (int)title.size() * 6) / 2, ly - 10,
                       title, Xwindow::Black);
}

// ─── Info panel below board ───────────────────────────────────────────────

void GraphicDisplay::drawInfoPanel() {
    int panelY = BOARD_PX;
    window->fillRectangle(0, panelY, BOARD_PX, PANEL_H, Xwindow::White);
    // Top border of panel
    window->fillRectangle(0, panelY, BOARD_PX, 1, Xwindow::Black);

    if (!players || players->empty()) return;

    int colW = BOARD_PX / 6;  // max 6 players
    int idx = 0;
    for (auto* p : *players) {
        int cx = idx * colW;
        int cy = panelY + 8;

        // Piece icon background
        int bgColor = PIECE_COLORS[idx % 6];
        window->fillRectangle(cx + 4, cy, 16, 16, bgColor);
        std::string piece(1, p->getPiece());
        window->drawString(cx + 7, cy + 13, piece, Xwindow::White);

        // Name
        std::string name = p->getName();
        if (name.length() > 8) name = name.substr(0, 8);
        window->drawString(cx + 4, cy + 30, name, Xwindow::Black);

        if (p->isBankrupt()) {
            window->drawString(cx + 4, cy + 46, "BANKRUPT", Xwindow::Red);
        } else {
            // Money
            std::string money = "$" + std::to_string(p->getMoney());
            window->drawString(cx + 4, cy + 46, money, Xwindow::Black);

            // Position name
            std::string pos = board->getSquare(p->getPosition())->getName();
            if (pos.length() > 8) pos = pos.substr(0, 8);
            window->drawString(cx + 4, cy + 62, pos, Xwindow::Black);

            // Tims cups
            if (p->getTimsCups() > 0) {
                std::string cups = "Cups:" + std::to_string(p->getTimsCups());
                window->drawString(cx + 4, cy + 78, cups, Xwindow::Black);
            }
        }
        ++idx;
    }
}
