//
// Created by maria on 11/5/2025.
//

#ifndef CASTLE_DEFENDER_ENEMY_H
#define CASTLE_DEFENDER_ENEMY_H
#include "Block.h"

class Enemy{
private:
    Block* enemy_address;
    int hp;
    int default_hp;
    bool readyToMove;
    bool readyToDie;
public:
    Enemy(Block *enemy_address, int hp=3, bool readyToMove=false, bool readyToDie=false);
    Block* getEnemyAddress();
    void setEnemyAddress(Block* new_address);
    int getEnemyHP() const;
    void setEnemyHP(int new_hp);
    void changeDefaultHP();
    int getDefaultHP();
    bool canMove();
    void enableMovement();
};


#endif //CASTLE_DEFENDER_ENEMY_H