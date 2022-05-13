#ifndef __MAP_H_
#define __MAP_H_

#include "main.h"

typedef struct {
    int startCorner;
    int endCorner;
    int color;
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

extern wall_t** g_walls;
extern int g_wallCount;

extern sector_t** g_sectors;
extern int g_sectorCount;

extern int* colors;

void LoadMap();

#endif
