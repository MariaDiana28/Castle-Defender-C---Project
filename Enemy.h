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
public:
    Enemy(Block *enemy_address, int hp=3);
    Block* getEnemyAddress();
    void setEnemyAddress(Block* new_address);
    int getEnemyHP() const;
    void setEnemyHP(int new_hp);
    void changeDefaultHP();
    int getDefaultHP();
};


#endif //CASTLE_DEFENDER_ENEMY_H