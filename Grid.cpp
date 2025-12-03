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
    learntDangerousColumns.resize(cols, 0); // initially, no column is dangerous, no prior knowledge about the towers
    weights.resize(cols, 100); // initially, all of the columns have the same weights
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

// set the castle cell
void Grid::placeCastle() {
    blocks[castle_row][castle_col].setType(CellType::CASTLE);
}

// draw the grid
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

// Found an algorithm here: https://stackoverflow.com/questions/1761626/weighted-random-numbers
// Pick the column where to spawn the enemy by weights. Obviously, columns with smaller weights still have a probability to be picked
int Grid::selectColumnByWeight(const vector<int> &weights) {
    int sum = 0;
    for (int w : weights)
        sum += w;

    if (sum == 0)
        return rand() % cols; // fallback

    int r = rand() % sum;
    for (int i = 0; i < weights.size(); ++i) {
        if (r < weights[i])
            return i;
        r -= weights[i];
    }
    return cols - 1;
}

// PART OF THE AI COMPONENT. LEARNING IS ALSO DONE IN THE TOWER ATTACK FUNCTION, WHERE THE ENEMIES LEARN WHICH COLUMNS ARE IN RANGE OF TOWERS WHEN THEY GET ATTACKED. CHECK THE COMMENTS OF THAT FUNCTION TO UNDERSTAND THE LEARNING PART
// EXPLANATION OF HOW ADAPTIVE SPAWNING WORKS
// In order to avoid extremely powerful enemies, I have decided to include a boolean adaptive_spawn variables. For the first wave the enemies are only going to be spawned randomly, without using the learnt information.
// If after the first wave has been spawned, the castle's HP is >= 80, it means that the player is too powerful and I enable adaptive spawning. In the tower attack function, when an enemy gets attacked, the column where the enemy was attacked becomes more dangerous in the learntDangerousColumns vector. Higher values of learnt danger will decrease the odds of those columns being picked by future enemies. Thus, enemies have 0 prior information about where towers are placed, but learn which columns to avoid, there is no rule-based behavior.
// The enemies' HP is also adapted if the player is doing too well. That is relatively rule-based, but that's what the project instructions suggested we do.
// It may seem like the AI is too strong sometimes, but I purposely made it like this to showcase that it actually learns. However, if the towers are placed really well, they will span most of the grid and learning will be difficult. But, to accurately test if the learning and weighted selection work, you can place the towers on one half of the grid and the enemies will quickly learn to spawn on the other half.
// spawns a wave of enemies
void Grid::spawnEnemies() {
    // check if enemies still need to be spawned
    if (current_wave < max_waves && current_wave_enemy_count < enemies_per_wave) {
        int rand_col;
        if (!adaptive_spawn) {
            // before learning, spawn randomly
            rand_col = rand() % cols;
            cout << "[Random Spawn] Column: " << rand_col << endl;
        } else {
            // LEARNING-BASED SPAWNING: AI COMPONENT
            for (int c = 0; c < cols; ++c) {
                // Higher danger → smaller weight
                // at each step. update the column weights based on the changes in the learnt danger
                if (learntDangerousColumns[c] == 0)
                    weights[c] = 200;        // boost weights if column is safe. if towers are placed well, almost no columns are safer and AI behavior is not great if left at 100. . With 200, it may sometimes appear to be too strong, but I left it like this to clearly show that it works. You can decrease the weights here to make the game more balanced. To test if it works, I suggest placing the towers on one half of the grid, and you will see that it quickly learns to spawn enemies on the other half.
                else
                    weights[c] = 100 / (1 + learntDangerousColumns[c]);
            }
            // use the function defined above for weighted picking of columns -> pick safer columns with a higher probability
            rand_col = selectColumnByWeight(weights);

            // couts for debugging in the terminal
            cout << "[Adaptive Spawn]\n";
            cout << "Learnt Danger: ";
            for (int d : learntDangerousColumns) cout << d << " ";
            cout << "\nWeights: ";
            for (int w : weights) cout << w << " ";
            cout << "\nSELECTED COLUMN = " << rand_col << "\n\n";
        }

        // Place enemy if empty
        if (blocks[0][rand_col].isEmpty()) {
            blocks[0][rand_col].setType(CellType::ENEMY);
            Enemy newEnemy(&blocks[0][rand_col], default_hp, false, false);
            enemies.push_back(newEnemy); // add to the enemy vector to keep track of it
            current_wave_enemy_count++;
        }
    }

    // Post-wave logic
    // Adaptive HP, defined based on the formula in the project instructions
    if (current_wave_enemy_count == enemies_per_wave) { // after each wave is complete
        if (score >= current_wave * 80) {
            default_hp += 1; // increase HP if player is doing too well
        }
        current_wave++; // move to the next wave
        current_wave_enemy_count = 0; // start over with the next wave
    }

    // Start adaptive spawning only if the player is doing too well after the first wave or later waves
    if (!adaptive_spawn && current_wave >= 1 && castle_hp >= 80) {
        adaptive_spawn = true;
        cout << "[Adaptive Spawning ENABLED]\n";
    }
}

// remove enemies when they die. remove them from the enemy vector, change the cell type to EMPTY
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
        current_enemy->setType(CellType::EMPTY); // update the cell of the enemy to be moved
        blocks[next_r][curr_c].setType(CellType::ENEMY); // new enemy cell
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
    // first try to move left
    if (curr_c!=0 && blocks[next_r][l_c].isEmpty()) {
        Block *address_next_enemy = &blocks[next_r][l_c];
        current_enemy->setType(CellType::EMPTY); // update the cell of the enemy to be moved
        blocks[next_r][l_c].setType(CellType::ENEMY); // new enemy cell
        enemy.setEnemyAddress(address_next_enemy); // relocate in the enemy vector as well
        return true;
    }
    // else move right
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
    // loop through the vector backwards, to avoid indices out of range when updating the enemy vector
    for (int i = int(enemies.size()) - 1; i >= 0; --i) {
        Enemy &enemy = enemies[i]; // enemy to be moved

        // Step 1: if enemy is not ready to move, enable and skip. This is important for visually updating the grid. Had to implement this because enemies were moving right after spawning and you couldn't see them on the first move. So, move them in the following turn after spawning
        if (!enemy.canMove()) {
            enemy.enableMovement();  // next turn it moves
            continue;
        }

        // Step 2: enemy can move now
        bool moved = false;
        if (moveDown(enemy)) {
            moved = true;
        } else if (moveDiagonally(enemy)) {
            moved = true;
        }

        // Step 3: reached castle row?
        if (moved) {
            Block* cell = enemy.getEnemyAddress();
            if (cell->getRow() == castle_row) {
                castle_hp -= 10; // damage castle
                deleteEnemies(enemy); // remove enemy
            }
        }
    }
}


// check if enemy is in range 2 from a tower
bool Grid::inRange(Tower tower, Enemy& enemy, int range) {
    int tower_row=tower.getTowerAddress()->getRow(); int tower_col=tower.getTowerAddress()->getCol(); // see where tower is
    int enemy_row=enemy.getEnemyAddress()->getRow(); int enemy_col=enemy.getEnemyAddress()->getCol(); // see where enemy is
    return (abs(tower_row-enemy_row)<=range && abs(tower_col-enemy_col)<=range); // condition from the project instructions
}

// implement attacking enemies
// for each tower, go through the vector of enemies and check which are in range. if in range -> attack
// if an enemy is attacked, the learntDangerousColumns vector element with the index of the column where the enemy is attacked is increased, so other enemies have a smaller chance of picking it. It acts sort of like reinforcement learning. If a bad column is constantly picked, the learnt danger gets bigger and the weights decrease. So, the learnt danger is not boolean, either dangerous or safe. By increasing the danger to more than one, the enemies have a greater chance to avoid that column, as the idea that it is unsafe is reinforced.
void Grid::towerAttack() {
    for (int t=0; t<towers.size(); t++) {
        for (int e=0; e<enemies.size(); e++) {
            if (inRange(towers[t], enemies[e], 2)) {
                // enemy is attacked
                int this_hp=enemies[e].getEnemyHP();
                int col = enemies[e].getEnemyAddress()->getCol();
                learntDangerousColumns[col] += 1; // FOR THE AI COMPONENT, LEARN THAT THIS COLUMN IS UNSAFE, SO OTHER ENEMIES HAVE A GREATER CHANCE OF AVOIDING IT
                // if hp>1 -> deal damage
                if (this_hp>1) {
                    this_hp--;
                    enemies[e].setEnemyHP(this_hp);
                }
                // if hp==1 -> damaging will kill the enemy, remove enemy
                else {
                    int curr_col=enemies[e].getEnemyAddress()->getCol();
                    int curr_row=enemies[e].getEnemyAddress()->getRow();
                    blocks[curr_row][curr_col].setType(CellType::EMPTY); // update cell
                    enemies.erase(enemies.begin() + e);
                    score+=10; // increase the score
                    e--;
                }
            }
        }
    }
}

// one step of the game: spawn enemies -> towers attack -> enemies move
void Grid::step() {
    // 1. Spawn at most 1 enemy per turn
    if (current_wave < max_waves && current_wave_enemy_count < enemies_per_wave) {
        spawnEnemies();
    }

    // 2. Towers attack enemies
    towerAttack();

    // 3. Enemies move
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

// helper
bool Grid::startedAdaptiveSpawning() {
    return adaptive_spawn;
}
