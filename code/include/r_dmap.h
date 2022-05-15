#pragma once

#include "map.h"

//
// Dynamic map renderer
//

vec2_t rayIntersect(vec2_t fromA, vec2_t toA, vec2_t rayStart, float rayAngle);
void renderMapDynamic();