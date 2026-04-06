#include <ncurses.h>   
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <algorithm>   

const int MAP_W = 40;
const int MAP_H = 20;

const char WALL   = '#';
const char FLOOR  = '.';
const char EXIT_T = '>';   

const int COL_WALL   = 1;   
const int COL_ENEMY  = 2;   
const int COL_EXIT   = 3;   
const int COL_PLAYER = 4;   

struct Entity {
    int x, y;      
    int hp;        
    int attack;    
};

char              map[MAP_H][MAP_W];   
Entity            player;
std::vector<Entity> enemies;
int               level   = 1;
std::string       message = "Find the exit '>'! WASD to move, Q to quit.";

void generateMap() {

    
    for (int y = 0; y < MAP_H; y++)
        for (int x = 0; x < MAP_W; x++)
            map[y][x] = WALL;

    
    auto room = [](int x1, int y1, int x2, int y2) {
        for (int y = y1; y < y2; y++)
            for (int x = x1; x < x2; x++)
                map[y][x] = FLOOR;
    };

    
    room( 2,  2, 12,  8);   
    room(16,  2, 28,  8);   
    room( 2, 12, 15, 18);   
    room(20, 12, 38, 18);   

    
    for (int x = 12; x < 16; x++)
        map[4][x] = FLOOR;

    
    for (int y = 8; y < 12; y++)
        map[y][6] = FLOOR;

    
    for (int y = 8; y < 12; y++)
        map[y][22] = FLOOR;

    
    for (int x = 15; x < 20; x++)
        map[14][x] = FLOOR;

    
    map[13][36] = EXIT_T;
}

void spawnEnemies() {
    enemies.clear();

    
    std::vector<std::pair<int,int>> positions = {
        {20, 4}, {24, 5},    
        { 6,14}, {10,15},    
        {28,14}, {32,15}     
    };

    for (auto& p : positions) {
        Entity e;
        e.x      = p.first;
        e.y      = p.second;
        e.hp     = 3 + level;   
        e.attack = 1;
        enemies.push_back(e);
    }
}

int enemyAt(int x, int y) {
    for (int i = 0; i < (int)enemies.size(); i++)
        if (enemies[i].x == x && enemies[i].y == y)
            return i;
    return -1;
}

void draw() {
    clear();   

    
    for (int y = 0; y < MAP_H; y++) {
        for (int x = 0; x < MAP_W; x++) {
            char tile = map[y][x];

            if (tile == WALL) {
                attron(COLOR_PAIR(COL_WALL));
                mvaddch(y, x, tile);
                attroff(COLOR_PAIR(COL_WALL));
            } else if (tile == EXIT_T) {
                attron(COLOR_PAIR(COL_EXIT));
                mvaddch(y, x, tile);
                attroff(COLOR_PAIR(COL_EXIT));
            } else {
                mvaddch(y, x, tile);   
            }
        }
    }

    
    for (auto& e : enemies) {
        attron(COLOR_PAIR(COL_ENEMY));
        mvaddch(e.y, e.x, 'E');
        attroff(COLOR_PAIR(COL_ENEMY));
    }

    
    attron(COLOR_PAIR(COL_PLAYER) | A_BOLD);
    mvaddch(player.y, player.x, '@');
    attroff(COLOR_PAIR(COL_PLAYER) | A_BOLD);

    
    mvprintw(MAP_H + 1, 0, "Level: %-3d  HP: %-3d  ATK: %-2d",
             level, player.hp, player.attack);
    mvprintw(MAP_H + 2, 0, ">> %s", message.c_str());
    mvprintw(MAP_H + 3, 0, "[WASD] Move   [Q] Quit");

    refresh();   
}

void enemyTurn() {
    for (auto& e : enemies) {

        
        int dx = 0, dy = 0;
        if      (e.x < player.x) dx =  1;
        else if (e.x > player.x) dx = -1;
        if      (e.y < player.y) dy =  1;
        else if (e.y > player.y) dy = -1;

        
        int nx = e.x + dx;
        int ny = e.y;

        if (nx == player.x && ny == player.y) {
            player.hp -= e.attack;
            message = "An enemy hit you for 1 damage!";
        } else if (map[ny][nx] == FLOOR && enemyAt(nx, ny) == -1) {
            e.x = nx;
        } else {
            
            nx = e.x;
            ny = e.y + dy;

            if (nx == player.x && ny == player.y) {
                player.hp -= e.attack;
                message = "An enemy hit you for 1 damage!";
            } else if (map[ny][nx] == FLOOR && enemyAt(nx, ny) == -1) {
                e.y = ny;
            }
            
        }
    }
}

void resetLevel() {
    generateMap();
    spawnEnemies();
    player.x = 3;
    player.y = 3;
}

int main() {
    srand(static_cast<unsigned>(time(nullptr)));

    
    initscr();            
    cbreak();             
    noecho();             
    keypad(stdscr, TRUE); 
    curs_set(0);          

    start_color();
    init_pair(COL_WALL,   COLOR_WHITE,  COLOR_BLACK);
    init_pair(COL_ENEMY,  COLOR_RED,    COLOR_BLACK);
    init_pair(COL_EXIT,   COLOR_YELLOW, COLOR_BLACK);
    init_pair(COL_PLAYER, COLOR_GREEN,  COLOR_BLACK);

    
    player = {3, 3, 10, 2};   

    resetLevel();

    
    bool running = true;

    while (running) {

        draw();

        
        int ch = getch();

        int dx = 0, dy = 0;
        switch (ch) {
            case 'w': case 'W': case KEY_UP:    dy = -1; break;
            case 's': case 'S': case KEY_DOWN:  dy =  1; break;
            case 'a': case 'A': case KEY_LEFT:  dx = -1; break;
            case 'd': case 'D': case KEY_RIGHT: dx =  1; break;
            case 'q': case 'Q': running = false; continue;
            default:            continue;   
        }

        
        int nx = player.x + dx;
        int ny = player.y + dy;

        
        if (nx < 0 || nx >= MAP_W || ny < 0 || ny >= MAP_H)
            continue;

        char target = map[ny][nx];

        
        if (target == WALL) {
            message = "That's a wall!";
            continue;   
        }

        
        int ei = enemyAt(nx, ny);
        if (ei != -1) {
            enemies[ei].hp -= player.attack;
            if (enemies[ei].hp <= 0) {
                enemies.erase(enemies.begin() + ei);
                message = "You killed an enemy!";
            } else {
                message = "You attacked an enemy!";
            }

        
        } else {
            player.x = nx;
            player.y = ny;
            message  = "";

            
            if (target == EXIT_T) {
                level++;
                player.hp     = std::min(player.hp + 3, 10);   
                player.attack++;                                 
                message = "You found the exit! Entering level " +
                          std::to_string(level) + "...";
                resetLevel();
                continue;   
            }
        }

        
        enemyTurn();

        
        if (player.hp <= 0) {
            draw();
            mvprintw(MAP_H / 2, MAP_W / 2 - 10,
                     "  *** YOU DIED! ***  Press any key.");
            refresh();
            getch();
            running = false;
        }
    }

    
    endwin();

    
    if (player.hp > 0)
        printf("Thanks for playing! You survived to level %d.\n", level);
    else
        printf("Game Over — you fell on level %d. Better luck next time!\n", level);

    return 0;
}
