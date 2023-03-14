#include "r_main.h"
#include "r_dmap.h"

typedef struct {
    float startX;
    float endX;
    void* last;
    void* next;
} render_slice_t;

render_slice_t* first;

float* remaining;

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

void debug(const char *__format, ...) {
    if(g_keys['I']) {
        va_list argptr;
        va_start(argptr, __format);
        vfprintf(stderr, __format, argptr);
        va_end(argptr);
    }
}

node_t* findSector(node_t* currentNode) {
    debug("Currently at: %x\r\n", currentNode);

    vec2_t* v1 = g_corners[currentNode->start];
    vec2_t* v2 = g_corners[g_rootNode->end];

    float angle1 = toDegrees(atan2(v2->y - v1->y, v2->x - v1->x));
    float angle2 = toDegrees(atan2(g_playerPos.y - v1->y, g_playerPos.x - v1->x));

    if(angle1 > angle2) {
        debug("\tPlayer at right side\r\n");
        // right side
        if(currentNode->nodeRight != 0) {
            debug("\tRight side is node %x\r\n", currentNode->nodeRight);
            return findSector(currentNode->nodeRight);
        } else {
            debug("\tRight side can not be split\r\n");
            return currentNode;
        }
    } else {
        // left side
        debug("\tPlayer at left side\r\n");
        if(currentNode->nodeLeft != 0) {
            debug("\tLeft side is node %x\r\n", currentNode->nodeLeft);
            return findSector(currentNode->nodeLeft);
        } else {
            debug("\tLeft side can not be split\r\n");
            return currentNode;
        }
    }
}

void renderNode(node_t* node, vec2_t playerScreen) {
    if(first->startX == -45 && first->endX == g_fovH / 2) {
        return;
    }

    vec2_t* v1 = g_corners[node->start];
    vec2_t* v2 = g_corners[node->end];

    float angle1 = toDegrees(atan2(v2->y - v1->y, v2->x - v1->x));
    float angle2 = toDegrees(atan2(g_playerPos.y - v1->y, g_playerPos.x - v1->x));

    if(angle2 < angle1) {
        // right side
        if(node->sectorRight != -1) {
            renderSector(g_sectors[node->sectorRight], playerScreen);
        } else if(node->nodeRight != 0) {
            renderNode(node->nodeRight, playerScreen);
        }
    
        if(first->endX != g_fovH) {
            // left side
            if(node->sectorLeft != -1) {
                renderSector(g_sectors[node->sectorLeft], playerScreen);
            }
        }
    } else {
        // left side
        if(node->sectorLeft != -1) {
            renderSector(g_sectors[node->sectorLeft], playerScreen);
        } else if (node->nodeLeft != 0) {
            renderNode(node->nodeLeft, playerScreen);
        }
    
        if(first->endX != g_fovH) {
            // right side
            if(node->sectorRight != -1) {
                renderSector(g_sectors[node->sectorRight], playerScreen);
            }
        }
    }
}

void renderMapDynamic() {
    remaining = malloc(sizeof(float) * g_clientWidth * 2);

    vec2_t playerScreen = { g_playerPos.x, g_playerPos.y };
    vec2_t playerLook = { sin(toRadians(g_playerA)) * 1, cos(toRadians(g_playerA)) * 1 };
    vec2_t playerFovLeft = { sin(toRadians(g_playerA - g_fovH / 2)) * 40,
        cos(toRadians(g_playerA - g_fovH / 2))  * 40,  };
    vec2_t playerFovRight = { sin(toRadians(g_playerA + g_fovH / 2)) * 40,
        cos(toRadians(g_playerA + g_fovH / 2))  * 40,  };

    float renderWindowWidth = drawClipBR.x - drawClipTL.x;
    float renderWindowHeight = drawClipBR.y - drawClipTL.y;
    
    float renderWindowSize = min(renderWindowWidth, renderWindowHeight);

    // Rotate everything else around the player (opposite of player rotation)
    float pasin = sin(toRadians(g_playerA));
    float pacos = cos(toRadians(g_playerA));
    
    // Translate everything to player specific coordinate system
    playerScreen.x -= g_playerPos.x;
    playerScreen.y -= g_playerPos.y;

    float temp = playerLook.x;
    playerLook.x = playerLook.x * pacos - playerLook.y * pasin;
    playerLook.y = temp * pasin + playerLook.y * pacos;
    
    temp = playerFovLeft.x;
    playerFovLeft.x = playerFovLeft.x * pacos - playerFovLeft.y * pasin;
    playerFovLeft.y = temp * pasin + playerFovLeft.y * pacos;

    temp = playerFovRight.x;
    playerFovRight.x = playerFovRight.x * pacos - playerFovRight.y * pasin;
    playerFovRight.y = temp * pasin + playerFovRight.y * pacos;

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
    pScreen.x += renderWindowWidth / 2;
    pScreen.y += renderWindowHeight / 4 * 3;

    // Draw an outline if we're not rendered across the entire client window
    if(renderWindowHeight != g_clientHeight || renderWindowWidth != renderWindowWidth) {
        drawRect(0, 0, renderWindowWidth, renderWindowHeight, 0x00ff00);
    }

    fillRect(pScreen.x - 1, pScreen.y - 1, pScreen.x + 1, pScreen.y + 1, 0xffffff);
    drawLine(pScreen.x, pScreen.y,  pScreen.x + pLScreen.x, pScreen.y + pLScreen.y, 0xffffff);
    drawLine(pScreen.x, pScreen.y,  pScreen.x + pFovL.x, pScreen.y + pFovL.y, 0xffffff);
    drawLine(pScreen.x, pScreen.y,  pScreen.x + pFovR.x, pScreen.y + pFovR.y, 0xffffff);


    debug("Root node: %x\r\n", g_rootNode);

    node_t* currentNode = findSector(g_rootNode);

    debug("Found current node: %x\r\n", currentNode);

    first = malloc(sizeof(render_slice_t));
    first->startX = -45;
    first->endX = -45;
    first->last = 0;

    render_slice_t* last = malloc(sizeof(render_slice_t));
    last->startX = 45;
    last->endX = 45;
    last->last = &first;
    last->next = 0;
    first->next = last;

    renderNode(currentNode, playerScreen);
    /*
    while(first->startX != -45 || first->endX != g_fovH / 2) {
        vec2_t* v1 = g_corners[currentNode->start];
        vec2_t* v2 = g_corners[currentNode->end];

        float angle1 = toDegrees(atan2(v2->y - v1->y, v2->x - v1->x));
        float angle2 = toDegrees(atan2(g_playerPos.y - v1->y, g_playerPos.x - v1->x));

        if(angle2 < angle1) {
            // right side
            if(currentNode->sectorRight != -1) {
                renderSector(g_sectors[currentNode->sectorRight], playerScreen);
            }
        
            if(first->endX != g_fovH) {
                // left side
                if(currentNode->sectorLeft != -1) {
                    renderSector(g_sectors[currentNode->sectorLeft], playerScreen);
                }
            }
        } else {
            // left side
            if(currentNode->sectorLeft != -1) {
                renderSector(g_sectors[currentNode->sectorLeft], playerScreen);
            }
        
            if(first->endX != g_fovH) {
                // right side
                if(currentNode->sectorRight != -1) {
                    renderSector(g_sectors[currentNode->sectorRight], playerScreen);
                }
            }
        }
    }
    */
    
    g_keys['I'] = 0;
    g_keys['J'] = 0;
}

void renderWallSegment(vec2_t wallStartScreen, vec2_t wallEndScreen, vec2_t playerScreen, float angleStart, float angleEnd, uint32_t color) {
    float renderWindowWidth = drawClipBR.x - drawClipTL.x;
    float renderWindowHeight = drawClipBR.y - drawClipTL.y;
    
    float renderWindowSize = min(renderWindowWidth, renderWindowHeight);

    vec2_t intersectLeft = rayIntersect(wallStartScreen, wallEndScreen, playerScreen, angleStart);

    wallStartScreen.x = wallStartScreen.x + (wallEndScreen.x - wallStartScreen.x) * intersectLeft.y;
    wallStartScreen.y = wallStartScreen.y + (wallEndScreen.y - wallStartScreen.y) * intersectLeft.y;

    vec2_t intersectRight = rayIntersect(wallStartScreen, wallEndScreen, playerScreen, angleEnd);

    wallEndScreen.x = wallStartScreen.x + (wallEndScreen.x - wallStartScreen.x) * intersectRight.y;
    wallEndScreen.y = wallStartScreen.y + (wallEndScreen.y - wallStartScreen.y) * intersectRight.y;

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
    wSScreen.x += renderWindowWidth / 2;
    wSScreen.y += renderWindowHeight / 4 * 3;
    wEScreen.x += renderWindowWidth / 2;
    wEScreen.y += renderWindowHeight / 4 * 3;
    // Completely draw wall
    drawLine(wSScreen.x, wSScreen.y, wEScreen.x, wEScreen.y, color);
}

void renderSector(sector_t* sector, vec2_t playerScreen) {

    // Rotate everything else around the player (opposite of player rotation)
    float pasin = sin(toRadians(g_playerA));
    float pacos = cos(toRadians(g_playerA));
        
    for(int w = 0; w < sector->wallCount; w++) {
        wall_t* wall = g_walls[sector->walls[w]];

        vec2_t wallStartScreen = { g_corners[wall->startCorner]->x, g_corners[wall->startCorner]->y };
        vec2_t wallEndScreen = { g_corners[wall->endCorner]->x, g_corners[wall->endCorner]->y };

        wallStartScreen.x -= g_playerPos.x;
        wallStartScreen.y -= g_playerPos.y;

        wallEndScreen.x -= g_playerPos.x;
        wallEndScreen.y -= g_playerPos.y;

        // Rotate everything else around the player (opposite of player rotation)
        float temp = wallStartScreen.x; 
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
        
        float angleStart = toDegrees(atan2(wallStartScreen.y, wallStartScreen.x)) - 90;

        float angleEnd = toDegrees(atan2(wallEndScreen.y, wallEndScreen.x)) - 90;
        
        if(angleStart < -180) {
            angleStart = 180 + (angleStart + 180);
        }

        if(angleEnd < -180) {
            angleEnd = 180 + (angleEnd + 180);
        }

        //debug("%X, %f, %f\n", g_colors[g_sides[wall->sides[0]]->color], angleStart, angleEnd);

        angleStart *= -1;
        angleEnd *= -1;

        if(wall->sideCount > 1) {
            continue;
        }

        // Cull backface
        if(angleStart >= angleEnd) {
            if(angleStart > 0 && angleEnd < 0) {
                if(angleStart + (-1 * angleEnd) <= 180 || (angleStart > 45 && angleEnd < -45)) {
                    continue;
                } 
            } else {
                continue;
            }
        }

        if(angleStart <= -45 && angleEnd <= -45) {
            continue;
        }

        if(angleStart >= 45 && angleEnd >= 45) {
            continue;
        }

        angleStart = max(-45, angleStart <= 45 ? angleStart : -360 + angleStart);
        angleEnd = min(45, angleEnd >= -45 ? angleEnd : 360 + angleEnd);

        //renderWallSegment(wallStartScreen, wallEndScreen, playerScreen, angleStart, angleEnd, g_colors[g_sides[wall->sides[0]]->color]);
        
        if(g_sides[wall->sides[0]]->type == SIDE_SOLID) {
            // Keep slicing until entire wall is rendered
            for(render_slice_t* currentSlice = first; currentSlice != 0 && angleStart < angleEnd; currentSlice = currentSlice->next) {
                // Ignore walls that start entirely behind the slice - the next wall will handle it
                if(angleStart > currentSlice->endX) {
                    continue;
                }

                // Wall starts before this slice
                if(angleStart < currentSlice->startX) {
                    // Wall also ends before this slice, so insert new slice
                    if(angleEnd < currentSlice->startX) {
                        // Draw entirety of wall...
                        renderWallSegment(wallStartScreen, wallEndScreen, playerScreen, angleStart, angleEnd, g_colors[g_sides[wall->sides[0]]->color]);


                        render_slice_t* newSlice = malloc(sizeof(render_slice_t));
                        newSlice->startX = angleStart;
                        newSlice->endX = angleEnd;
                        ((render_slice_t*) currentSlice->last)->next = newSlice;
                        newSlice->last = currentSlice->last;
                        currentSlice->last = newSlice;
                        newSlice->next = currentSlice;

                        // Completely rendered so break out
                        break;
                    } else {
                        // Draw new wall up to existing slice
                        renderWallSegment(wallStartScreen, wallEndScreen, playerScreen, angleStart, currentSlice->startX, g_colors[g_sides[wall->sides[0]]->color]);
                        currentSlice->startX = angleStart;

                        // Prepare angle for next slice calculations
                        angleStart = currentSlice->endX;
                    }
                }
                
                // Wall starts in this slice
                if(angleStart >= currentSlice->startX) {
                    if(angleEnd <= currentSlice->endX) {
                        // Wall is completely occluded, break out
                        break;
                    } else if(angleEnd >= ((render_slice_t*) currentSlice->next)->startX) {
                        // Wall covers complete gap between two slices, render into this gap
                        renderWallSegment(wallStartScreen, wallEndScreen, playerScreen, currentSlice->endX, ((render_slice_t*) currentSlice->next)->startX, g_colors[g_sides[wall->sides[0]]->color]);
                        
                        // Combine the 2 slices and the gap...
                        render_slice_t* oldNext = currentSlice->next;
                        currentSlice->next = ((render_slice_t*) currentSlice->next)->next;
                        currentSlice->endX = oldNext->endX;

                        angleStart = oldNext->endX;
                        free(oldNext);
                        continue;
                    } else if(angleStart >= currentSlice->endX) {
                        // Draw entirety of wall...
                        renderWallSegment(wallStartScreen, wallEndScreen, playerScreen, angleStart, angleEnd, g_colors[g_sides[wall->sides[0]]->color]);


                        render_slice_t* newSlice = malloc(sizeof(render_slice_t));
                        newSlice->startX = angleStart;
                        newSlice->endX = angleEnd;
                        newSlice->last = currentSlice;
                        newSlice->next = currentSlice->next;
                        ((render_slice_t*) currentSlice->next)->last = newSlice;
                        currentSlice->next = newSlice;

                        // Completely rendered so break out
                        break;
                    } else {
                        // Render rest of wall after this slice
                        renderWallSegment(wallStartScreen, wallEndScreen, playerScreen, currentSlice->endX, angleEnd, g_colors[g_sides[wall->sides[0]]->color]);
                        
                        // Extend this slice
                        currentSlice->endX = angleEnd;

                        // Completely rendered, break out
                        break;
                    }
                }
            }
        }
        /*
        */
    }
}