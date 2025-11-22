//
// Created by maria on 11/3/2025.
//

#include "Grid.h"
using namespace std;

// initialize grid
Grid::Grid(int startX, int startY, int cellSize) {
    this->startX=startX;
    this->startY=startY;
    this->cellSize=cellSize;
    blocks.resize(rows, vector<Block>(cols, Block(0, 0, cellSize, 0, 0))); // add cells to the vector
    createGrid();
    placeCastle();
}

// create the cells array and get the coordinates
void Grid::createGrid() {
    for (int r=0; r<rows; r++) {
        for (int c=0; c<cols; c++) {
            int x = startX + c*cellSize;
            int y = startY + r*cellSize;
            blocks[r][c]=Block(x, y, cellSize, r, c);
        }
    }
}

void Grid::placeCastle() {
    blocks[castle_row][castle_col].setType(CellType::CASTLE);
}

void Grid::draw() {
    for (int r=0; r<rows; r++) {
        for (int c=0; c<cols; c++) {
            blocks[r][c].draw();
        }
    }
}

// add tower at the coordinates where the player clicked (detected in GameWindow)
void Grid::addTower(int mx, int my) {
    // check if it is within grid limits
    if (mx < startX || my < startY || mx >= startX + cols * cellSize || my >= startY + rows * cellSize || tower_count==5) {
        return;
    }
    // transform coordinates into rows and cols
    int gx=mx-startX;
    int gy=my-startY;
    int clicked_col=gx/cellSize;
    int clicked_row=gy/cellSize;
    // check the placement conditions
    if (blocks[clicked_row][clicked_col].isEmpty()!=true || blocks[clicked_row][clicked_col].getType()==CellType::CASTLE || clicked_row==0 || clicked_row==1) {
        return;
    }
    blocks[clicked_row][clicked_col].setType(CellType::TOWER);

    // add tower to the vector tower
    Tower newTower(&blocks[clicked_row][clicked_col], 2);
    towers.push_back(newTower);
    tower_count++;
}

void Grid::threatWeights() {
    threat_weights.clear();
    int peak = max_threat / 2;
    for (int c = 0; c < cols; c++) {
        int t = threat_levels[c]; // get threat level at column c
        int weight = max(1, (peak + 1) - abs(t - peak));
        threat_weights.push_back(weight);
    }
}

// spawns a wave of enemies
void Grid::spawnEnemies() {
    if (current_wave < max_waves && current_wave_enemy_count < enemies_per_wave) {
        int rand_col = rand() % cols;
        if (blocks[0][rand_col].isEmpty()) {
            blocks[0][rand_col].setType(CellType::ENEMY);
            Enemy newEnemy(&blocks[0][rand_col], default_hp);
            enemies.push_back(newEnemy);
            current_wave_enemy_count++;
        }
    }

    // Finished spawning a wave
    if (current_wave_enemy_count == enemies_per_wave) {
        // Evaluate performance BEFORE incrementing wave
        if (score >= current_wave * 80) {
            default_hp += 1;
        } else {
            default_hp = 3;
        }
        current_wave++;
        current_wave_enemy_count = 0;
    }
}

void Grid::deleteEnemies(Enemy& to_be_deleted) {
    for (int i = 0; i < enemies.size(); i++) {
        if (enemies[i].getEnemyAddress() == to_be_deleted.getEnemyAddress()) {
            enemies[i].getEnemyAddress()->setType(CellType::EMPTY);
            enemies.erase(enemies.begin() + i);
            break;
        }
    }
}

// move to the cell below
bool Grid::moveDown(Enemy& enemy) {
    Block *current_enemy = enemy.getEnemyAddress();
    int curr_r = current_enemy->getRow();
    int curr_c = current_enemy->getCol();
    int next_r = curr_r + 1;
    Block *address_next_enemy = &blocks[next_r][curr_c]; // determine where to move
    if (blocks[next_r][curr_c].isEmpty()) {
        current_enemy->setType(CellType::EMPTY);
        blocks[next_r][curr_c].setType(CellType::ENEMY);
        enemy.setEnemyAddress(address_next_enemy); // relocate in the enemy vector as well
        return true;
    }
    return false; // failed to move -> move diagonally
}

// move diagonally: first try left, then right
bool Grid::moveDiagonally(Enemy& enemy) {
    Block *current_enemy = enemy.getEnemyAddress();
    int curr_r = current_enemy->getRow();
    int curr_c = current_enemy->getCol();
    int next_r = curr_r + 1;
    // move left
    int l_c=curr_c-1;
    // move right
    int r_c=curr_c+1;
    if (curr_c!=0 && blocks[next_r][l_c].isEmpty()) {
        Block *address_next_enemy = &blocks[next_r][l_c];
        current_enemy->setType(CellType::EMPTY);
        blocks[next_r][l_c].setType(CellType::ENEMY);
        enemy.setEnemyAddress(address_next_enemy); // relocate in the enemy vector as well
        return true;
    }
    else if (curr_c!=cols-1 && blocks[next_r][r_c].isEmpty()) {
        Block *address_next_enemy = &blocks[next_r][r_c];
        current_enemy->setType(CellType::EMPTY);
        blocks[next_r][r_c].setType(CellType::ENEMY);
        enemy.setEnemyAddress(address_next_enemy); // relocate in the enemy vector as well
        return true;
    }
    return false; // failed to move

}

// move all enemies at once
void Grid::moveEnemies() {
    if (enemies.empty()) return;

    Enemy& enemy = enemies[0];
    bool moved = moveDown(enemy) || moveDiagonally(enemy);

    if (moved) {
        Block* cell = enemy.getEnemyAddress();
        if (cell->getRow() == castle_row) {
            castle_hp -= 10;
            deleteEnemies(enemy);
        }
    }
}

// check if enemy is in range 2 from a tower
bool Grid::inRange(Tower tower, Enemy& enemy, int range) {
    int tower_row=tower.getTowerAddress()->getRow(); int tower_col=tower.getTowerAddress()->getCol();
    int enemy_row=enemy.getEnemyAddress()->getRow(); int enemy_col=enemy.getEnemyAddress()->getCol();
    return (abs(tower_row-enemy_row)<=range && abs(tower_col-enemy_col)<=range); // condition from the project instructions
}

// implement attacking enemies
// for each tower, go through the vector of enemies and check which are in range. if in range -> attack
void Grid::towerAttack() {
    for (int t=0; t<towers.size(); t++) {
        for (int e=0; e<enemies.size(); e++) {
            if (inRange(towers[t], enemies[e], 2)) {
                int this_hp=enemies[e].getEnemyHP();
                // if hp>1 -> deal damage
                if (this_hp>1) {
                    this_hp--;
                    enemies[e].setEnemyHP(this_hp);
                }
                // if hp==1 -> damaging will kill the enemy, remove enemy
                else {
                    int curr_col=enemies[e].getEnemyAddress()->getCol();
                    int curr_row=enemies[e].getEnemyAddress()->getRow();
                    blocks[curr_row][curr_col].setType(CellType::EMPTY);
                    enemies.erase(enemies.begin() + e);
                    score+=10;
                    e--;
                }
            }
        }
    }
}

// one step of the game: spawn enemies -> towers attack -> enemies move
void Grid::step() {
    // Only spawn if there are no enemies on the grid and we still have enemies left in this wave
    if (current_wave < max_waves && enemies.empty() && current_wave_enemy_count < enemies_per_wave) {
        spawnEnemies();
    }

    towerAttack();
    moveEnemies();
}

// helper
int Grid::getCastleHP() {
    return castle_hp;
}

// helper: check if there are any enemies for game-end condition
bool Grid::hasEnemies() const {
    if (enemies.size()>0) {return true;}
    else {return false;}
}

// helper: check if there aren't enemies left to spawn or alive. if true -> win
bool Grid::wavesDone() const {
    return (current_wave >= max_waves) && enemies.empty();
}

// helper
int Grid::getTowerCount() {
    return tower_count;
}


// helper for displaying
int Grid::getScore() {
    return score;
}

// helper for displaying
int Grid::getEnemiesLeft() {
    return enemies.size();
}
