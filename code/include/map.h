#ifndef __MAP_H_
#define __MAP_H_

#define SIDE_SOLID 0
#define SIDE_PORTAL 1

#include "main.h"

typedef struct {
    int type;
    int sector;
    int color;
} side_t;

typedef struct {
    int startCorner;
    int endCorner;
    int sideCount;
    int* sides;
} wall_t;

typedef struct {
    int* walls;
    int wallCount;
    float ceilHeight;
    float floorHeight;
} sector_t;

extern int g_worldWidth;
extern int g_worldHeight;

extern vec2_t** g_corners;
extern int g_cornerCount;

extern side_t** g_sides;
extern int g_sideCount;

extern wall_t** g_walls;
extern int g_wallCount;

extern sector_t** g_sectors;
extern int g_sectorCount;

extern int* g_colors;

void LoadMap();

#endif
