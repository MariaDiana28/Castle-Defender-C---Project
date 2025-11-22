//
// Created by maria on 11/3/2025.
//

#include "Block.h"

// constructor
Block::Block(int x, int y, int size, int row, int col) {
    this->x = x;
    this->y = y;
    this->size = size;
    this->row = row;
    this->col = col;
    this->type = CellType::EMPTY;
}

// helper: change type of a cell (e.g. empty to tower or enemy)
void Block::setType(CellType type) {
    this->type = type;
}

// helper
CellType Block::getType() {
    return this->type;
}

// helper: useful when placing towers or moving enemies
bool Block::isEmpty() {
    if (this->type == CellType::EMPTY) {
        return true;
    } else {
        return false;
    }
}

// draw each cell in the grid and set its color according to its type
void Block::draw() {
    switch (type) {
        case CellType::EMPTY:
            fl_color(FL_WHITE);
            break;
        case CellType::TOWER:
            fl_color(FL_DARK_GREEN);
            break;
        case CellType::ENEMY:
            fl_color(FL_DARK_RED);
            break;
        case CellType::CASTLE:
            fl_color(FL_YELLOW);
            break;
    }

    fl_rectf(x, y, size, size);
    fl_color(FL_GRAY);
    fl_rect(x, y, size, size);
}

// helper
int Block::getRow() {
    return this->row;
}

// helper
int Block::getCol() {
    return this->col;
}
