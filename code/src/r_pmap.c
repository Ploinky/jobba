#include "r_pmap.h"

float length(vec2_t vec) {
    return sqrt(vec.x * vec.x + vec.y * vec.y);
}

void renderMapPerspective() {
    vec2_t playerScreen = { g_playerPos.x, g_playerPos.y };
    vec2_t playerLook = { sin(toRadians(g_playerA)) * 1, cos(toRadians(g_playerA)) * 1 };
    vec2_t playerFovLeft = { sin(toRadians(g_playerA - g_fovH / 2)) * 40, cos(toRadians(g_playerA - g_fovH / 2))  * 40,  };
    vec2_t playerFovRight = { sin(toRadians(g_playerA + g_fovH / 2)) * 40, cos(toRadians(g_playerA + g_fovH / 2))  * 40,  };

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

    // Draw an outline if we're not rendered across the entire client window
    if(renderWindowHeight != g_clientHeight || renderWindowWidth != renderWindowWidth) {
        drawRect(0, 0, renderWindowWidth, renderWindowHeight, 0x00ff00);
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
            
            if(g_sides[wall->sides[0]]->type == SIDE_SOLID) {
                // Draw wall
                
                float angleStart = toDegrees(atan2(wallStartScreen.y, wallStartScreen.x));
                if(angleStart < 0) {
                    angleStart = 360 + angleStart;
                }

                float angleEnd = toDegrees(atan2(wallEndScreen.y, wallEndScreen.x));
                if(angleEnd < 0) {
                    angleEnd = 360 + angleEnd;
                }
                
                // Angle starts on x-axis, but y is forward and should be 0
                angleStart -= 90;
                angleEnd -= 90;

                float xWallStart = renderWindowWidth - (angleStart / g_fovH + 0.5)  * renderWindowWidth;
                float xWallEnd = renderWindowWidth - (angleEnd / g_fovH + 0.5)  * renderWindowWidth;

                vec2_t st = {(wallStartScreen.x - playerScreen.x), (wallStartScreen.y - playerScreen.y)};
                vec2_t en = {(wallEndScreen.x - playerScreen.x), (wallEndScreen.y - playerScreen.y)};
                
                float wallStartDist = length(st);
                float wallEndDist = length(en);

                float yWallStartTop = renderWindowHeight / 2 - renderWindowHeight / 2 * (1 / wallStartDist);
                float yWallStartBottom = renderWindowHeight / 2 + renderWindowHeight / 2 * (1 / wallStartDist);
                float yWallEndTop = renderWindowHeight / 2 - renderWindowHeight / 2 * (1 / wallEndDist);
                float yWallEndBottom = renderWindowHeight / 2 + renderWindowHeight / 2 * (1 / wallEndDist);

                wallStartScreen.y *= -1;
                wallEndScreen.y *= -1;

                wallStartScreen.x = wallStartScreen.x / g_worldWidth * renderWindowWidth + renderWindowWidth / 2;
                wallStartScreen.y = wallStartScreen.y / g_worldHeight * renderWindowHeight  + renderWindowHeight / 2;
                wallEndScreen.x = wallEndScreen.x / g_worldWidth * renderWindowWidth + renderWindowWidth / 2;
                wallEndScreen.y = wallEndScreen.y / g_worldHeight * renderWindowHeight + renderWindowHeight / 2;

                if(g_keys['P']) {
                    for(float x = xWallStart; x < xWallEnd; x++) {
                        float frac = (x - xWallStart) / (xWallEnd - xWallStart);

                        float yTop = yWallStartTop + frac * (yWallEndTop - yWallStartTop);
                        float yBottom = yWallStartBottom + frac * (yWallEndBottom - yWallStartBottom);

                        drawLine(x, 0, x, yTop, 0x606060);
                        drawLine(x, yTop, x, yBottom, g_colors[g_sides[wall->sides[0]]->color]);
                        drawLine(x, yBottom, x, renderWindowHeight, 0x060606);       
                    }
                } else {
                    drawLine(xWallStart, yWallStartTop, xWallEnd, yWallEndTop, g_colors[g_sides[wall->sides[0]]->color]);

                    drawLine(xWallStart, yWallStartBottom, xWallEnd, yWallEndBottom, g_colors[g_sides[wall->sides[0]]->color]);

                    if(xWallStart > 0) {
                        drawLine(xWallStart, yWallStartTop, xWallStart, yWallStartBottom, g_colors[g_sides[wall->sides[0]]->color]);
                    }
                    if(xWallEnd < g_clientWidth) {
                        drawLine(xWallEnd, yWallEndTop, xWallEnd, yWallEndBottom, g_colors[g_sides[wall->sides[0]]->color]);
                    }                
                }
            } else {
                // Draw portal indicator
            }
        }
    }
    g_keys['I'] = 0;
    g_keys['J'] = 0;
}