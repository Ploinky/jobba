#include "r_main.h"
#include "r_dmap.h"

vec2_t rayIntersect(vec2_t fromA, vec2_t toA, vec2_t rayStart, float rayAngle) {
    vec2_t ret = { NAN, NAN };
    vec2_t q = fromA;
    vec2_t s = subtract(toA, fromA);

    vec2_t p = rayStart;
    vec2_t r = { sin(toRadians(rayAngle)), cos(toRadians(rayAngle)) };

    if(cross(r, s) == 0) {
        return ret;
    }

    float t = cross(subtract(q, p), s) / cross(r, s);
    float u = cross(subtract(q, p), r) / 
    cross(r, s);

    ret.x = t;
    ret.y = u;

    return ret;
}

vec2_t vectorIntersect(vec2_t fromA, vec2_t toA, vec2_t fromB, vec2_t toB) {
    vec2_t ret = { NAN, NAN };

    return ret;
}

void renderMapDynamic() {
    vec2_t playerScreen = { g_playerPos.x, g_playerPos.y };
    vec2_t playerLook = { sin(toRadians(g_playerA)) * 1, cos(toRadians(g_playerA)) * 1 };
    vec2_t playerFovLeft = { sin(toRadians(g_playerA - g_fovH / 2)) * 40,
        cos(toRadians(g_playerA - g_fovH / 2))  * 40,  };
    vec2_t playerFovRight = { sin(toRadians(g_playerA + g_fovH / 2)) * 40,
        cos(toRadians(g_playerA + g_fovH / 2))  * 40,  };

    float renderWindowWidth = drawClipBR.x - drawClipTL.x;
    float renderWindowHeight = drawClipBR.y - drawClipTL.y;
    // Translate everything to player specific coordinate system
    playerScreen.x -= g_playerPos.x;
    playerScreen.y -= g_playerPos.y;

    // Rotate everything else around the player (opposite of player rotation)
    float pasin = sin(toRadians(g_playerA));
    float pacos = cos(toRadians(g_playerA));

    float temp = playerLook.x;
    playerLook.x = playerLook.x * pacos - playerLook.y * pasin;
    playerLook.y = temp * pasin + playerLook.y * pacos;
    
    temp = playerFovLeft.x;
    playerFovLeft.x = playerFovLeft.x * pacos - playerFovLeft.y * pasin;
    playerFovLeft.y = temp * pasin + playerFovLeft.y * pacos;

    temp = playerFovRight.x;
    playerFovRight.x = playerFovRight.x * pacos - playerFovRight.y * pasin;
    playerFovRight.y = temp * pasin + playerFovRight.y * pacos;

    // ---- Top view dynamic ----
    {
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
        pScreen.x = pScreen.x / g_worldWidth * renderWindowWidth;
        pScreen.y = pScreen.y / g_worldHeight * renderWindowHeight;

        pLScreen.x = pLScreen.x / g_worldWidth * renderWindowWidth;
        pLScreen.y = pLScreen.y / g_worldHeight * renderWindowHeight;

        pFovL.x = pFovL.x / g_worldWidth * renderWindowWidth;
        pFovL.y = pFovL.y / g_worldHeight * renderWindowHeight;

        pFovR.x = pFovR.x / g_worldWidth * renderWindowWidth;
        pFovR.y = pFovR.y / g_worldHeight * renderWindowHeight;
    
        // Move origin to center of screen
        pScreen.x += renderWindowWidth / 2;
        pScreen.y += renderWindowHeight / 4 * 3;

        pFovL.x = pFovL.x / g_worldWidth * renderWindowWidth;
        pFovL.y = pFovL.y / g_worldHeight * renderWindowHeight;

        pFovR.x = pFovR.x / g_worldWidth * renderWindowWidth;
        pFovR.y = pFovR.y / g_worldHeight * renderWindowHeight;

        drawRect(0, 0, renderWindowWidth, renderWindowHeight, 0x00ff00);
        fillRect(pScreen.x - 1, pScreen.y - 1, pScreen.x + 1, pScreen.y + 1, 0xffffff);
        drawLine(pScreen.x, pScreen.y,  pScreen.x + pLScreen.x, pScreen.y + pLScreen.y, 0xffffff);
        drawLine(pScreen.x, pScreen.y,  pScreen.x + pFovL.x, pScreen.y + pFovL.y, 0xffffff);
        drawLine(pScreen.x, pScreen.y,  pScreen.x + pFovR.x, pScreen.y + pFovR.y, 0xffffff);
    }
    
        
    for(int s = 0; s < g_sectorCount; s++) {
        sector_t* sector = g_sectors[s];
        for(int w = 0; w < sector->wallCount; w++) {
            wall_t* wall = g_walls[sector->walls[w]];

            vec2_t wallStartScreen = { g_corners[wall->startCorner]->x, g_corners[wall->startCorner]->y };
            vec2_t wallEndScreen = { g_corners[wall->endCorner]->x, g_corners[wall->endCorner]->y };

            wallStartScreen.x -= g_playerPos.x;
            wallStartScreen.y -= g_playerPos.y;

            wallEndScreen.x -= g_playerPos.x;
            wallEndScreen.y -= g_playerPos.y;

            // Rotate everything else around the player (opposite of player rotation)
            temp = wallStartScreen.x; 
            wallStartScreen.x = wallStartScreen.x * pacos - wallStartScreen.y * pasin;
            wallStartScreen.y = temp * pasin + wallStartScreen.y * pacos;

            temp = wallEndScreen.x;
            wallEndScreen.x = wallEndScreen.x * pacos - wallEndScreen.y * pasin;
            wallEndScreen.y = temp * pasin + wallEndScreen.y * pacos;
            
            // Clip
            if(wallStartScreen.y < 0 && wallEndScreen.y < 0) {
                // Both vertices behind player, definitely clip!
                // This seems like a pretty cheap check to do right off the bat
                continue;
            }
            
            float angleStart = toDegrees(atan2(wallStartScreen.y, wallStartScreen.x));
            if(angleStart < 0) {
                angleStart = 360 + angleStart;
            }

            float angleEnd = toDegrees(atan2(wallEndScreen.y, wallEndScreen.x));
            if(angleEnd < 0) {
                angleEnd = 360 + angleEnd;
            }

            int draw = 0;

            if(angleStart - angleEnd < 180 && angleStart - angleEnd > 0 && angleEnd < 135 && angleStart > 45) {
                draw = 1;
            }

            if(angleStart - angleEnd < -180 &&  angleStart > 45 && angleStart < 180 ) {
                draw = 1;
            }

            if(!draw) {
                // Cull backfaces
                if(wall->sideCount < 2) {
                    continue;
                }

                vec2_t tempStart = wallStartScreen;
                wallStartScreen = wallEndScreen;
                wallEndScreen = tempStart;

                angleStart = toDegrees(atan2(wallStartScreen.y, wallStartScreen.x));
                if(angleStart < 0) {
                    angleStart = 360 + angleStart;
                }

                angleEnd = toDegrees(atan2(wallEndScreen.y, wallEndScreen.x));
                if(angleEnd < 0) {
                    angleEnd = 360 + angleEnd;
                }

                draw = 0;

                if(angleStart - angleEnd < 180 && angleStart - angleEnd > 0 && angleEnd < 135 && angleStart > 45) {
                    draw = 1;
                }

                if(angleStart - angleEnd < -180 &&  angleStart > 45 && angleStart < 180 ) {
                    draw = 1;
                }
                
                if(!draw) {
                    continue;
                }
            }

            if(g_keys['I']) {
                printf("%X, %f, %f\n", g_colors[g_sides[wall->sides[0]]->color], angleStart, angleEnd);
            }

            vec2_t intersectLeft = rayIntersect(wallStartScreen, wallEndScreen, playerScreen, -g_fovH / 2);

            // Wall is clipped at left side of screen
            if(intersectLeft.x > 0 && intersectLeft.y > 0 && intersectLeft.y < 1) {
                wallStartScreen.x = wallStartScreen.x + (wallEndScreen.x - wallStartScreen.x) * intersectLeft.y;
                wallStartScreen.y = wallStartScreen.y + (wallEndScreen.y - wallStartScreen.y) * intersectLeft.y;
            }
            
            vec2_t intersectRight = rayIntersect(wallStartScreen, wallEndScreen, playerScreen, g_fovH / 2);

            // Wall is clipped at right side of screen
            if(intersectRight.x > 0 && intersectRight.y > 0 && intersectRight.y < 1) {
                wallEndScreen.x = wallStartScreen.x + (wallEndScreen.x - wallStartScreen.x) * intersectRight.y;
                wallEndScreen.y = wallStartScreen.y + (wallEndScreen.y - wallStartScreen.y) * intersectRight.y;
            }
            
            vec2_t wSScreen = { wallStartScreen.x, wallStartScreen.y };
            vec2_t wEScreen = { wallEndScreen.x, wallEndScreen.y };

            // Flip y axis
            wSScreen.y *= -1;
            wEScreen.y *= -1;
            
            // Scale to screen coordinates
            wSScreen.x = wSScreen.x / g_worldWidth * renderWindowWidth;
            wSScreen.y = wSScreen.y / g_worldHeight * renderWindowHeight ;
            wEScreen.x = wEScreen.x / g_worldWidth * renderWindowWidth;
            wEScreen.y = wEScreen.y / g_worldHeight * renderWindowHeight;
            
            // Move origin to center of screen
            wSScreen.x += renderWindowWidth / 2;
            wSScreen.y += renderWindowHeight / 4 * 3;
            wEScreen.x += renderWindowWidth / 2;
            wEScreen.y += renderWindowHeight / 4 * 3;


            if(g_sides[wall->sides[0]]->type == SIDE_SOLID) {
                drawLine(wSScreen.x, wSScreen.y,  wEScreen.x, wEScreen.y, g_colors[g_sides[wall->sides[0]]->color]);
            } else {
                drawLine(wSScreen.x, wSScreen.y,  wEScreen.x, wEScreen.y, 0x606060);
            }
        }
    }
    g_keys['I'] = 0;
    g_keys['J'] = 0;
}