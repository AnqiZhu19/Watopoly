#ifndef GRAPHICDISPLAY_H
#define GRAPHICDISPLAY_H

#include "observer.h"
#include <vector>
#include <string>

class Board;
class Player;
class Xwindow;

class GraphicDisplay : public Observer {
    Xwindow* window;
    Board* board;
    std::vector<Player*>* players;

    static const int CELL = 60;       // pixels per board cell (kept small to fit macOS screens)
    static const int BOARD_PX = 660;  // 11 * CELL
    static const int PANEL_H = 90;    // info panel height below board

public:
    GraphicDisplay(Board* b, std::vector<Player*>* p);
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

#endif
