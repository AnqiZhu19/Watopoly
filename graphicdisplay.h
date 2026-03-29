#pragma once
#include "observer.h"
#include "player.h"
#include <vector>
#include <memory>
#include <string>

class Board;
class Xwindow;

class GraphicDisplay : public Observer {
    std::unique_ptr<Xwindow> window;
    Board* board;
    std::vector<std::unique_ptr<Player>>* players;

    static constexpr int CELL = 60;       // pixels per board cell (kept small to fit macOS screens)
    static constexpr int BOARD_PX = 660;  // 11 * CELL
    static constexpr int PANEL_H = 90;    // info panel height below board

public:
    GraphicDisplay(Board* b, std::vector<std::unique_ptr<Player>>* p);
    ~GraphicDisplay() override;

    void notify() override;
    void redraw();

private:
    void drawSquare(int pos);
    void drawCenter();
    void drawInfoPanel();

    int squareCol(int pos) const;
    int squareRow(int pos) const;
    int blockColor(const std::string& block) const;
};
