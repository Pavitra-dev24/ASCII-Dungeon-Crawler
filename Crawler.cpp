#include <iostream>
#include <queue>
#include <utility>
#include <random>
#include <ctime>
#include <conio.h>
#include <cstring>
#include <algorithm>

using namespace std;

#define WIDTH 40
#define HEIGHT 20
#define MAX_ROOMS 6
#define ROOM_MIN 3
#define ROOM_MAX 8
#define ENEMY_HP 3
#define PLAYER_HP 10
#define AGGRO_RANGE 6

struct Room {
    int x; int y; int w; int h;
    int cx(){ return x + w/2; }
    int cy(){ return y + h/2; }
    bool intersects(const Room& o) const {
        return !(x + w < o.x || o.x + o.w < x || y + h < o.y || o.y + o.h < y);
    }
};

struct Entity {
    int x; int y; char glyph; int hp;
    bool alive(){ return hp > 0; }
};

char grid[HEIGHT][WIDTH+1];
Room rooms[MAX_ROOMS];
int room_count = 0;
Entity enemies[MAX_ROOMS];
int enemy_count = 0;
Entity player;
std::mt19937 rng((unsigned)time(nullptr));
pair<int,int> dirs[4] = {{0,-1},{0,1},{-1,0},{1,0}};

int rand_int(int a,int b){
    uniform_int_distribution<int> d(a,b);
    return d(rng);
}

bool in_bounds(int x,int y){ return x>=0 && x<WIDTH && y>=0 && y<HEIGHT; }

void init_grid(){
    for(int y=0;y<HEIGHT;++y){
        for(int x=0;x<WIDTH;++x) grid[y][x] = '#';
        grid[y][WIDTH] = '\0';
    }
}

void carve_room(const Room &r){
    for(int yy=r.y; yy<r.y + r.h; ++yy)
        for(int xx=r.x; xx<r.x + r.w; ++xx)
            if(in_bounds(xx,yy)) grid[yy][xx] = '.';
}

void carve_hcorr(int x1,int x2,int y){
    if(y<0 || y>=HEIGHT) return;
    if(x1>x2) swap(x1,x2);
    for(int x=x1; x<=x2; ++x) if(in_bounds(x,y)) grid[y][x] = '.';
}
void carve_vcorr(int y1,int y2,int x){
    if(x<0 || x>=WIDTH) return;
    if(y1>y2) swap(y1,y2);
    for(int y=y1; y<=y2; ++y) if(in_bounds(x,y)) grid[y][x] = '.';
}

void place_rooms_and_corridors(){
    room_count = 0;
    int attempts = 0;
    while(room_count < MAX_ROOMS && attempts < 300){
        int w = rand_int(ROOM_MIN, ROOM_MAX);
        int h = rand_int(ROOM_MIN, ROOM_MAX);
        int x = rand_int(1, WIDTH - w - 2);
        int y = rand_int(1, HEIGHT - h - 2);
        Room r; r.x = x; r.y = y; r.w = w; r.h = h;
        bool ok = true;
        for(int i=0;i<room_count;++i) if(r.intersects(rooms[i])){ ok=false; break; }
        if(ok){
            carve_room(r);
            if(room_count > 0){
                int x1 = rooms[room_count-1].cx(), y1 = rooms[room_count-1].cy();
                int x2 = r.cx(), y2 = r.cy();
                if(rand_int(0,1)){
                    carve_hcorr(x1,x2,y1);
                    carve_vcorr(y1,y2,x2);
                } else {
                    carve_vcorr(y1,y2,x1);
                    carve_hcorr(x1,x2,y2);
                }
            }
            rooms[room_count++] = r;
        }
        ++attempts;
    }
}

bool is_walkable(int x,int y){
    if(!in_bounds(x,y)) return false;
    return grid[y][x] == '.' || grid[y][x] == '>';
}

pair<int,int> bfs_next_step(int sx,int sy,int tx,int ty){
    static bool vis[HEIGHT][WIDTH];
    static pair<int,int> parent[HEIGHT][WIDTH];
    for(int y=0;y<HEIGHT;++y) for(int x=0;x<WIDTH;++x){ vis[y][x] = false; parent[y][x] = {-1,-1}; }
    queue<pair<int,int>> q;
    q.push({sx,sy});
    vis[sy][sx] = true;
    bool found = false;
    while(!q.empty()){
        auto p = q.front(); q.pop();
        int x = p.first, y = p.second;
        if(x==tx && y==ty){ found = true; break; }
        for(int i=0;i<4;++i){
            int nx = x + dirs[i].first, ny = y + dirs[i].second;
            if(!in_bounds(nx,ny)) continue;
            if(vis[ny][nx]) continue;
            if(!is_walkable(nx,ny) && !(nx==tx && ny==ty)) continue;
            vis[ny][nx] = true;
            parent[ny][nx] = {x,y};
            q.push({nx,ny});
        }
    }
    if(!found) return {-1,-1};
    int cx = tx, cy = ty;
    pair<int,int> prev = {-1,-1};
    while(!(cx==sx && cy==sy)){
        prev = {cx,cy};
        auto p = parent[cy][cx];
        cx = p.first; cy = p.second;
        if(cx == -1) break;
    }
    if(prev.first == -1) return {-1,-1};
    return prev;
}

int find_enemy_at(int x,int y){
    for(int i=0;i<enemy_count;++i) if(enemies[i].alive() && enemies[i].x==x && enemies[i].y==y) return i;
    return -1;
}

void draw(int player_hp){
    static char disp[HEIGHT][WIDTH+1];
    for(int y=0;y<HEIGHT;++y) memcpy(disp[y], grid[y], WIDTH+1);
    for(int i=0;i<enemy_count;++i) if(enemies[i].alive()) disp[enemies[i].y][enemies[i].x] = enemies[i].glyph;
    disp[player.y][player.x] = player.glyph;
    system("cls");
    cout << "HP: " << player_hp << "   (WASD to move, Q to quit)\n";
    for(int y=0;y<HEIGHT;++y) cout << disp[y] << "\n";
    cout.flush();
}

int main(){
    init_grid();
    place_rooms_and_corridors();
    if(room_count == 0) return 1;
    player.x = rooms[0].cx(); player.y = rooms[0].cy(); player.glyph = '@'; player.hp = PLAYER_HP;
    Room rlast = rooms[room_count-1];
    grid[rlast.y + rlast.h/2][rlast.x + rlast.w/2] = '>';
    enemy_count = 0;
    for(int i=1;i<room_count;++i){
        enemies[enemy_count].x = rooms[i].cx();
        enemies[enemy_count].y = rooms[i].cy();
        enemies[enemy_count].glyph = 'E';
        enemies[enemy_count].hp = ENEMY_HP;
        ++enemy_count;
    }
    int player_hp = PLAYER_HP;
    bool running = true;
    while(running){
        draw(player_hp);
        int raw = _getch();
        char ch = (char)raw;
        if(ch=='q' || ch=='Q') break;
        int dx=0, dy=0;
        if(ch=='w' || ch=='W') dy = -1;
        else if(ch=='s' || ch=='S') dy = 1;
        else if(ch=='a' || ch=='A') dx = -1;
        else if(ch=='d' || ch=='D') dx = 1;
        else continue;
        int nx = player.x + dx, ny = player.y + dy;
        if(!in_bounds(nx,ny)) continue;
        if(grid[ny][nx] == '>'){
            cout << "You found the exit! You win!\n"; cout.flush(); break;
        }
        int idx = find_enemy_at(nx,ny);
        if(idx != -1){
            enemies[idx].hp -= 2;
        } else if(is_walkable(nx,ny)){
            player.x = nx; player.y = ny;
        }
        for(int ei=0; ei<enemy_count; ++ei){
            if(!enemies[ei].alive()) continue;
            int ex = enemies[ei].x, ey = enemies[ei].y;
            int dist = abs(ex - player.x) + abs(ey - player.y);
            if(dist <= AGGRO_RANGE){
                auto step = bfs_next_step(ex,ey,player.x,player.y);
                if(step.first != -1){
                    int exn = step.first, eyn = step.second;
                    if(exn == player.x && eyn == player.y){
                        player_hp -= 1;
                        if(player_hp <= 0){ cout << "You died. Game over.\n"; cout.flush(); running = false; break; }
                    } else {
                        bool occupied = false;
                        for(int oj=0; oj<enemy_count; ++oj) if(oj!=ei && enemies[oj].alive() && enemies[oj].x==exn && enemies[oj].y==eyn) { occupied = true; break; }
                        if(!occupied){ enemies[ei].x = exn; enemies[ei].y = eyn; }
                    }
                }
            } else {
                int cand_count = 0;
                pair<int,int> cands[4];
                for(int d=0; d<4; ++d){
                    int rx = ex + dirs[d].first, ry = ey + dirs[d].second;
                    if(!in_bounds(rx,ry)) continue;
                    if(!is_walkable(rx,ry)) continue;
                    if(rx==player.x && ry==player.y) continue;
                    bool occ = false;
                    for(int oj=0; oj<enemy_count; ++oj) if(oj!=ei && enemies[oj].alive() && enemies[oj].x==rx && enemies[oj].y==ry) { occ = true; break; }
                    if(!occ) cands[cand_count++] = {rx,ry};
                }
                if(cand_count > 0){
                    int choice = rand_int(0, cand_count-1);
                    enemies[ei].x = cands[choice].first;
                    enemies[ei].y = cands[choice].second;
                }
            }
        }
        if(!running) break;
    }
    cout << "Thanks for playing.\n";
    return 0;
}
