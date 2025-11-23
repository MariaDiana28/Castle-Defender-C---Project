//
// Created by maria on 11/3/2025.
//

#ifndef CASTLE_DEFENDER_GRID_H
#define CASTLE_DEFENDER_GRID_H
#include <FL/Fl.H>
#include <FL/fl_draw.H>
#include "Block.h"
#include "Enemy.h"
#include "Tower.h"
#include <vector>
#include <algorithm>
#include <iostream>
using namespace std;

class Grid {
private:
    int rows=20, cols=20, cellSize;
    int startX, startY;
    int castle_row=rows-1;
    int castle_col=cols/2;
    vector<vector<Block>> blocks; // store all of the cells
    vector<Enemy> enemies; // keep track of enemies
    vector<Tower> towers;
    vector<int> threat_levels;
    vector<int> threat_weights;
    int tower_count=0;
    int castle_hp=100;
    int current_wave = 0; // wave of enemies
    int current_wave_enemy_count=0; // to spawn 10 enemies pwe turn
    int max_waves = 5;
    int enemies_per_wave=10;
    int score = 0;
    int default_hp;
    int max_threat=0;
    bool adaptive_spawn=false;
public:
    Grid(int startX, int startY, int cellSize);
    void createGrid();
    void placeCastle();
    void draw();
    void addTower(int mx, int my);
    void threatLevels();
    void threatWeights();
    int selectColumnByWeight();
    void spawnEnemies();
    void deleteEnemies(Enemy& to_be_deleted);
    bool moveDown(Enemy& enemy);
    bool moveDiagonally(Enemy& enemy);
    void moveEnemies();
    bool inRange(Tower tower, Enemy& enemy, int range);
    void towerAttack();
    void step();
    bool hasEnemies() const;
    bool wavesDone() const;
    int getTowerCount();
    int getCastleHP();
    int getScore();
    int getEnemiesLeft();
    bool startedAdaptiveSpawning();
};

#endif //CASTLE_DEFENDER_GRID_H