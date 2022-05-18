#include "r_smap.h"
#include "r_main.h"

void renderMapStatic() {
    vec2_t playerScreen = { g_playerPos.x, g_playerPos.y };
    vec2_t playerFovLeft = { sin(toRadians(g_playerA - g_fovH / 2)) * 40, cos(toRadians(g_playerA - g_fovH / 2))  * 40,  };
    vec2_t playerFovRight = { sin(toRadians(g_playerA + g_fovH / 2)) * 40, cos(toRadians(g_playerA + g_fovH / 2))  * 40,  };
        
    float renderWindowWidth = drawClipBR.x - drawClipTL.x;
    float renderWindowHeight = drawClipBR.y - drawClipTL.y;

    float renderWindowSmallerSize = min(renderWindowWidth, renderWindowHeight);

    // Flip y axis
    playerScreen.y *= -1;
    playerFovLeft.y *= -1;
    playerFovRight.y *= -1;

    // Scale to screen coordinates
    playerScreen.x = playerScreen.x / g_worldWidth * renderWindowSmallerSize;
    playerScreen.y = playerScreen.y / g_worldHeight * renderWindowSmallerSize;

    playerFovLeft.x = playerFovLeft.x / g_worldWidth * renderWindowSmallerSize;
    playerFovLeft.y = playerFovLeft.y / g_worldHeight * renderWindowSmallerSize;

    playerFovRight.x = playerFovRight.x / g_worldWidth * renderWindowSmallerSize;
    playerFovRight.y = playerFovRight.y / g_worldHeight * renderWindowSmallerSize;

    // Move origin to center of screen
    playerScreen.x += renderWindowWidth / 2;
    playerScreen.y += renderWindowHeight / 2;

    // Draw an outline if we're not rendered across the entire client window
    if(renderWindowHeight != g_clientHeight || renderWindowWidth != renderWindowWidth) {
        drawRect(0, 0, renderWindowWidth, renderWindowHeight, 0xff0000);
    }

    fillRect(playerScreen.x - 1, playerScreen.y - 1, playerScreen.x + 1, playerScreen.y + 1, 0xffffff);
    drawLine(playerScreen.x, playerScreen.y,  min(renderWindowWidth - 1, max(0, playerScreen.x + playerFovLeft.x)), min(renderWindowHeight - 1, max(0, playerScreen.y + playerFovLeft.y)), 0xffffff);
    drawLine(playerScreen.x, playerScreen.y,  min(renderWindowWidth - 1, max(0, playerScreen.x + playerFovRight.x)), min(renderWindowHeight - 1, max(0, playerScreen.y + playerFovRight.y)), 0xffffff);
    
    for(int s = 0; s < g_sectorCount; s++) {
        sector_t* sector = g_sectors[s];
        for(int w = 0; w < sector->wallCount; w++) {
            wall_t* wall = g_walls[sector->walls[w]];
            vec2_t wallStartScreen = { g_corners[wall->startCorner]->x, g_corners[wall->startCorner]->y };
            vec2_t wallEndScreen = { g_corners[wall->endCorner]->x, g_corners[wall->endCorner]->y };

            // Flip y axis
            wallStartScreen.y *= -1;
            wallEndScreen.y *= -1;
            
            // Scale to screen coordinates
            wallStartScreen.x = wallStartScreen.x / g_worldWidth * renderWindowSmallerSize;
            wallStartScreen.y = wallStartScreen.y / g_worldHeight * renderWindowSmallerSize ;
            wallEndScreen.x = wallEndScreen.x / g_worldWidth * renderWindowSmallerSize;
            wallEndScreen.y = wallEndScreen.y / g_worldHeight * renderWindowSmallerSize;
            
            // Move origin to center of screen
            wallStartScreen.x += renderWindowWidth / 2;
            wallStartScreen.y += renderWindowHeight / 2;
            wallEndScreen.x += renderWindowWidth / 2;
            wallEndScreen.y += renderWindowHeight / 2;

            if(g_sides[wall->sides[0]]->type == SIDE_SOLID) {
                drawLine(wallStartScreen.x, wallStartScreen.y,  wallEndScreen.x, wallEndScreen.y, g_colors[g_sides[wall->sides[0]]->color]);
            } else {
                drawLine(wallStartScreen.x, wallStartScreen.y,  wallEndScreen.x, wallEndScreen.y, 0x606060);
            }
        }
    }
}