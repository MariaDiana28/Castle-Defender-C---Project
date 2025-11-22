//
// Created by maria on 11/3/2025.
//

#ifndef CASTLE_DEFENDER_BLOCK_H
#define CASTLE_DEFENDER_BLOCK_H
#include <FL/Fl.H>
#include <FL/fl_draw.H>
// have different types of cells, manage game state
enum class CellType { EMPTY, TOWER, ENEMY, CASTLE };

class Block {
private:
    int x, y; // coordinates in the window
    int size; // of a cell in px
    int row, col; // coordinates in the grid
    CellType type;
public:
    Block(int x, int y, int size, int row, int col);
    void setType(CellType type);
    CellType getType();
    bool isEmpty();
    void draw();
    int getRow();
    int getCol();
};


#endif //CASTLE_DEFENDER_BLOCK_H