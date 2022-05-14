#include "map.h"

int g_worldWidth = 20;
int g_worldHeight = 20;

vec2_t** g_corners;
int g_cornerCount;

wall_t** g_walls;
int g_wallCount;

sector_t** g_sectors;
int g_sectorCount;

int* g_colors;

void LoadMap() {
    g_colors = malloc(sizeof(int) * 6);
    g_colors[0] = 0xff0000;
    g_colors[1] = 0x00ff00;
    g_colors[2] = 0x0000ff;
    g_colors[3] = 0x00ffff;
    g_colors[4] = 0xffff00;
    g_colors[5] = 0xff00ff;

    g_wallCount = 6;
    g_walls = malloc(sizeof(wall_t) * g_wallCount);

    g_cornerCount = 6;
    g_corners = malloc(sizeof(vec2_t) * g_cornerCount);
    vec2_t* corner1 = malloc(sizeof(vec2_t));
    vec2_t* corner2 = malloc(sizeof(vec2_t));
    vec2_t* corner3 = malloc(sizeof(vec2_t));
    vec2_t* corner4 = malloc(sizeof(vec2_t));
    vec2_t* corner5 = malloc(sizeof(vec2_t));
    vec2_t* corner6 = malloc(sizeof(vec2_t));
    corner1->x = -8;
    corner1->y = 8;
    corner2->x = 2;
    corner2->y = 8;
    corner3->x = 8;
    corner3->y = 2;
    corner4->x = 8;
    corner4->y = -8;
    corner5->x = -2;
    corner5->y = -8;
    corner6->x = -8;
    corner6->y = -2;
    g_corners[0] = corner1;
    g_corners[1] = corner2;
    g_corners[2] = corner3;
    g_corners[3] = corner4;
    g_corners[4] = corner5;
    g_corners[5] = corner6;

    wall_t* wall1 = malloc(sizeof(wall_t));
    wall_t* wall2 = malloc(sizeof(wall_t));
    wall_t* wall3 = malloc(sizeof(wall_t));
    wall_t* wall4 = malloc(sizeof(wall_t));
    wall_t* wall5 = malloc(sizeof(wall_t));
    wall_t* wall6 = malloc(sizeof(wall_t));
    wall1->startCorner = 0; 
    wall1->endCorner = 1;
    wall1->color = 0;
    wall2->startCorner = 1; 
    wall2->endCorner = 2;
    wall2->color = 1;
    wall3->startCorner = 2; 
    wall3->endCorner = 3;
    wall3->color = 2;
    wall4->startCorner = 3; 
    wall4->endCorner = 4;
    wall4->color = 3;
    wall5->startCorner = 4; 
    wall5->endCorner = 5;
    wall5->color = 4;
    wall6->startCorner = 5; 
    wall6->endCorner = 0;
    wall6->color = 5;
    g_walls[0] = wall1;
    g_walls[1] = wall2;
    g_walls[2] = wall3;
    g_walls[3] = wall4;
    g_walls[4] = wall5;
    g_walls[5] = wall6;

    sector_t* sector = malloc(sizeof(sector_t));
    sector->wallCount = 6;
    sector->walls = malloc(sizeof(int) * sector->wallCount);
    sector->walls[0] = 0;
    sector->walls[1] = 1;
    sector->walls[2] = 2;
    sector->walls[3] = 3;
    sector->walls[4] = 4;
    sector->walls[5] = 5;
    sector->floorHeight = 0;
    sector->ceilHeight = 4;

    g_sectorCount = 1;
    g_sectors = malloc(sizeof(sector_t) * g_sectorCount);

    g_sectors[0] = sector;
}