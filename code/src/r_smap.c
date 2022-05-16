#include "r_smap.h"
#include "r_main.h"

void renderMapStatic() {
    vec2_t playerScreen = { g_playerPos.x, g_playerPos.y };
    vec2_t playerLook = { sin(toRadians(g_playerA)) * 1, cos(toRadians(g_playerA)) * 1 };
    vec2_t playerFovLeft = { sin(toRadians(g_playerA - g_fovH / 2)) * 40,
        cos(toRadians(g_playerA - g_fovH / 2))  * 40,  };
    vec2_t playerFovRight = { sin(toRadians(g_playerA + g_fovH / 2)) * 40,
        cos(toRadians(g_playerA + g_fovH / 2))  * 40,  };
        
    float renderWindowWidth = drawClipBR.x - drawClipTL.x;
    float renderWindowHeight = drawClipBR.y - drawClipTL.y;

    float renderWindowSmallerSize = min(renderWindowWidth, renderWindowHeight);

    vec2_t pScreen = { playerScreen.x, playerScreen.y };
    vec2_t pLScreen = { playerLook.x, playerLook.y };
    vec2_t pFovL = { playerFovLeft.x, playerFovLeft.y };
    vec2_t pFovR = { playerFovRight.x, playerFovRight.y };

    // Flip y axis
    pScreen.y *= -1;
    pLScreen.y *= -1;
    pFovL.y *= -1;
    pFovR.y *= -1;

    // Scale to screen coordinates
    pScreen.x = pScreen.x / g_worldWidth * renderWindowSmallerSize;
    pScreen.y = pScreen.y / g_worldHeight * renderWindowSmallerSize;

    pLScreen.x = pLScreen.x / g_worldWidth * renderWindowSmallerSize;
    pLScreen.y = pLScreen.y / g_worldHeight * renderWindowSmallerSize;

    pFovL.x = pFovL.x / g_worldWidth * renderWindowSmallerSize;
    pFovL.y = pFovL.y / g_worldHeight * renderWindowSmallerSize;

    pFovR.x = pFovR.x / g_worldWidth * renderWindowSmallerSize;
    pFovR.y = pFovR.y / g_worldHeight * renderWindowSmallerSize;

    // Move origin to center of screen
    pScreen.x += renderWindowWidth / 2;
    pScreen.y += renderWindowHeight / 2;

    // Draw an outline if we're not rendered across the entire client window
    if(renderWindowHeight != g_clientHeight || renderWindowWidth != renderWindowWidth) {
        drawRect(0, 0, renderWindowWidth, renderWindowHeight, 0xff0000);
    }


    fillRect(pScreen.x - 1, pScreen.y - 1, pScreen.x + 1, pScreen.y + 1, 0xffffff);
    drawLine(pScreen.x, pScreen.y,  pScreen.x + pLScreen.x, pScreen.y + pLScreen.y, 0xffffff);
    drawLine(pScreen.x, pScreen.y,  pScreen.x + pFovL.x, pScreen.y + pFovL.y, 0xffffff);
    drawLine(pScreen.x, pScreen.y,  pScreen.x + pFovR.x, pScreen.y + pFovR.y, 0xffffff);
    
    for(int s = 0; s < g_sectorCount; s++) {
        sector_t* sector = g_sectors[s];
        for(int w = 0; w < sector->wallCount; w++) {
            wall_t* wall = g_walls[sector->walls[w]];
            vec2_t wallStartScreen = { g_corners[wall->startCorner]->x, g_corners[wall->startCorner]->y };
            vec2_t wallEndScreen = { g_corners[wall->endCorner]->x, g_corners[wall->endCorner]->y };

            vec2_t wSScreen = { wallStartScreen.x, wallStartScreen.y };
            vec2_t wEScreen = { wallEndScreen.x, wallEndScreen.y };

            // Flip y axis
            wSScreen.y *= -1;
            wEScreen.y *= -1;
            
            // Scale to screen coordinates
            wSScreen.x = wSScreen.x / g_worldWidth * renderWindowSmallerSize;
            wSScreen.y = wSScreen.y / g_worldHeight * renderWindowSmallerSize ;
            wEScreen.x = wEScreen.x / g_worldWidth * renderWindowSmallerSize;
            wEScreen.y = wEScreen.y / g_worldHeight * renderWindowSmallerSize;
            
            // Move origin to center of screen
            wSScreen.x += renderWindowWidth / 2;
            wSScreen.y += renderWindowHeight / 2;
            wEScreen.x += renderWindowWidth / 2;
            wEScreen.y += renderWindowHeight / 2;

            if(g_sides[wall->sides[0]]->type == SIDE_SOLID) {
                drawLine(wSScreen.x, wSScreen.y,  wEScreen.x, wEScreen.y, g_colors[g_sides[wall->sides[0]]->color]);
            } else {
                drawLine(wSScreen.x, wSScreen.y,  wEScreen.x, wEScreen.y, 0x606060);
            }
        }
    }
}