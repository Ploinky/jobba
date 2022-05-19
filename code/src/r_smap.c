#include "r_smap.h"
#include "r_main.h"

void renderMapStatic() {
    // Position in which the player will be drawn
    vec2_t playerScreen = { g_playerPos.x, g_playerPos.y };

    // End point of the FOV indicator, relative to player position
    vec2_t playerFovLeft = { sin(toRadians(g_playerA - g_fovH / 2)) + playerScreen.x, cos(toRadians(g_playerA - g_fovH / 2)) + playerScreen.y,  };
    vec2_t playerFovRight = { sin(toRadians(g_playerA + g_fovH / 2)) + playerScreen.x, cos(toRadians(g_playerA + g_fovH / 2)) + playerScreen.y,  };
        
    // Size of the image we're drawing to, in pixels
    float renderWindowWidth = drawClipBR.x - drawClipTL.x;
    float renderWindowHeight = drawClipBR.y - drawClipTL.y;

    // Scale to smaller dimension to avoid clipping and stretching
    float renderWindowSmallerSize = min(renderWindowWidth, renderWindowHeight);

    // Scale to screen coordinates
    playerScreen.x = playerScreen.x / g_worldWidth * renderWindowSmallerSize;
    playerScreen.y = playerScreen.y / g_worldHeight * renderWindowSmallerSize;

    playerFovLeft.x = playerFovLeft.x / g_worldWidth * renderWindowSmallerSize;
    playerFovLeft.y = playerFovLeft.y / g_worldHeight * renderWindowSmallerSize;

    playerFovRight.x = playerFovRight.x / g_worldWidth * renderWindowSmallerSize;
    playerFovRight.y = playerFovRight.y / g_worldHeight * renderWindowSmallerSize;

    // Flip y axis
    playerScreen.y *= -1;
    playerFovLeft.y *= -1;
    playerFovRight.y *= -1;

    // Move origin to center of screen
    playerScreen.x += renderWindowWidth / 2;
    playerScreen.y += renderWindowHeight / 2;

    playerFovRight.x += renderWindowWidth / 2;
    playerFovRight.y += renderWindowHeight / 2;

    playerFovLeft.x += renderWindowWidth / 2;
    playerFovLeft.y += renderWindowHeight / 2;

    // Draw an outline if we're not rendered across the entire client window
    if(renderWindowHeight != g_clientHeight || renderWindowWidth != renderWindowWidth) {
        drawRect(0, 0, renderWindowWidth, renderWindowHeight, 0xff0000);
    }

    fillRect(playerScreen.x - 1, playerScreen.y - 1, playerScreen.x + 1, playerScreen.y + 1, 0xffffff);
    drawLine(playerScreen.x, playerScreen.y,  playerFovLeft.x, playerFovLeft.y, 0xffffff);
    drawLine(playerScreen.x, playerScreen.y,  playerFovRight.x, playerFovRight.y, 0xffffff);
    
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