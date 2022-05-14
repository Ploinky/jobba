#include "r_smap.h"
#include "r_main.h"

void renderMapStatic() {
    vec2_t playerScreen = { g_playerPos.x, g_playerPos.y };
    vec2_t playerLook = { sin(toRadians(g_playerA)) * 1, cos(toRadians(g_playerA)) * 1 };
    vec2_t playerFovLeft = { sin(toRadians(g_playerA - g_fovH / 2)) * 40,
        cos(toRadians(g_playerA - g_fovH / 2))  * 40,  };
    vec2_t playerFovRight = { sin(toRadians(g_playerA + g_fovH / 2)) * 40,
        cos(toRadians(g_playerA + g_fovH / 2))  * 40,  };
        
    float renderWindowSize = drawClipBR.x - drawClipTL.x;
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
    pScreen.x = pScreen.x / g_worldWidth * renderWindowSize;
    pScreen.y = pScreen.y / g_worldHeight * renderWindowSize;

    pLScreen.x = pLScreen.x / g_worldWidth * renderWindowSize;
    pLScreen.y = pLScreen.y / g_worldHeight * renderWindowSize;

    pFovL.x = pFovL.x / g_worldWidth * renderWindowSize;
    pFovL.y = pFovL.y / g_worldHeight * renderWindowSize;

    pFovR.x = pFovR.x / g_worldWidth * renderWindowSize;
    pFovR.y = pFovR.y / g_worldHeight * renderWindowSize;

    // Move origin to center of screen
    pScreen.x += renderWindowSize / 2;
    pScreen.y += renderWindowSize / 2;

    drawRect(0, 0, renderWindowSize, renderWindowSize, 0xff0000);
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

            // ---- Top view static ----
            {   
                vec2_t wSScreen = { wallStartScreen.x, wallStartScreen.y };
                vec2_t wEScreen = { wallEndScreen.x, wallEndScreen.y };

                // Flip y axis
                wSScreen.y *= -1;
                wEScreen.y *= -1;
                
                // Scale to screen coordinates
                wSScreen.x = wSScreen.x / g_worldWidth * renderWindowSize;
                wSScreen.y = wSScreen.y / g_worldHeight * renderWindowSize ;
                wEScreen.x = wEScreen.x / g_worldWidth * renderWindowSize;
                wEScreen.y = wEScreen.y / g_worldHeight * renderWindowSize;
                
                // Move origin to center of screen
                wSScreen.x += renderWindowSize / 2;
                wSScreen.y += renderWindowSize / 2;
                wEScreen.x += renderWindowSize / 2;
                wEScreen.y += renderWindowSize / 2;

                setDrawClip(0, 0, renderWindowSize, renderWindowSize);
                drawLine(wSScreen.x, wSScreen.y,  wEScreen.x, wEScreen.y, g_colors[wall->color]);
            }
        }
    }
}