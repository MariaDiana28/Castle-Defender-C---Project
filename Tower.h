//
// Created by maria on 11/5/2025.
//

#ifndef CASTLE_DEFENDER_TOWER_H
#define CASTLE_DEFENDER_TOWER_H
#include "Block.h"

class Tower {
private:
    Block* tower_address;
    int range;
public:
    Tower(Block* tower_address, int range=2);
    Block* getTowerAddress();
};


#endif //CASTLE_DEFENDER_TOWER_H