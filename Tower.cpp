//
// Created by maria on 11/5/2025.
//

#include "Tower.h"
using namespace std;

// constructor
Tower::Tower(Block* tower_address, int range) {
    this->tower_address=tower_address;
    this->range=range;
}

// helper
Block* Tower::getTowerAddress() {
    return this->tower_address;
}

