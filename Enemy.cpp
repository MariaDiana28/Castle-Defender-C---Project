//
// Created by maria on 11/5/2025.
//

#include "Enemy.h"

// constructor
Enemy::Enemy(Block* enemy_address, int hp, bool readyToMove, bool readyToDie) {
    this->enemy_address=enemy_address;
    this->hp=hp;
    this->readyToMove=readyToMove;
    this->readyToDie=readyToDie;
}

// helper: know which enemy we are looking at in order to be able to sync with blocks
Block* Enemy::getEnemyAddress() {
    return this->enemy_address;
}

// helper: set a different address when moving enemies
void Enemy::setEnemyAddress(Block* new_address) {
    this->enemy_address=new_address;
}

// helper
int Enemy::getEnemyHP() const {
    return this->hp;
}

// helper
void Enemy::setEnemyHP(int new_hp) {
    this->hp=new_hp;
}

// helper, delay enemy movement for visual update
bool Enemy::canMove() {
    if (this->readyToMove==false)  {
        return false;
    }
    else {return true;}
}

// At first, enemies were moving too fast after spawning and you couldn't see them spawn on the first row. I introduced the attribute readyToMove, set to false by default, in order to see enemies on the first row before they start moving. The, movement is enabled
void Enemy::enableMovement() {
    this->readyToMove = true;
}


