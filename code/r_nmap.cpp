#include "r_nmap.h"
#include "r_main.h"


vec2_t rayIntersect1(vec2_t fromA, vec2_t toA, vec2_t rayStart, float rayAngle) {
	vec2_t ret = { NAN, NAN };
	vec2_t q = fromA;
	vec2_t s = subtract(toA, fromA);

	vec2_t p = rayStart;
	vec2_t r = { sin(toRadians(rayAngle)), cos(toRadians(rayAngle)) };

	if (cross(r, s) == 0) {
		return ret;
	}

	float t = cross(subtract(q, p), s) / cross(r, s);
	float u = cross(subtract(q, p), r) /
		cross(r, s);

	ret.x = t;
	ret.y = u;

	return ret;
}

typedef struct wall_s {
	vec2_t start;
	vec2_t end;
} wall_t;

wall_t walls[] = {
	{ { -1, 10 }, { 2, 10} }
};

int worldToScreen(vec2_t point) {
	float ang = toDegrees(atan2(-(point.y - g_playerPos.y), point.x - g_playerPos.x));

	ang += 90;

	ang -= g_playerA;

	return g_clientWidth * ((ang + g_fovH / 2) / g_fovH);
}

void renderWall(wall_t wall) {
	for (int x = worldToScreen(wall.start); x < worldToScreen(wall.end); x++) {
		vec2_t i = rayIntersect1(wall.start, wall.end, g_playerPos, g_playerA + ((float) x / g_clientWidth) * g_fovH - g_fovH / 2 );
		uint32_t c = i.y > 0.5f ? 0x00ff00 : 0xff0000;
		uint32_t c1 = i.y > 0.5f ? 0xff0000 : 0x00ff00;
		drawLine(x, 0, x, g_clientHeight / 2, c);
		drawLine(x, g_clientHeight / 2, x, g_clientHeight, c1);
	}
}

void renderMapNew1() {
	for (int i = 0; i < sizeof(walls) / sizeof(walls[0]); i++) {
		renderWall(walls[i]);
	}
}

char map[9][9] = {
	"########",
	"#......#",
	"#...#..#",
	"#......#",
	"#......#",
	"#......#",
	"#......#",
	"########"
};

void renderMapNew() {
	float px = g_playerPos.x + 4.0f;
	float py = g_playerPos.y + 4.0f;

	// current map tile position of player
	int mapX = (int)(px);
	int mapY = (int)(py);

	float lastDistToWall = 0;

	// Compute worldspace width of the projection plane (at a depth of 1)
	// using the camera's field of view (which in Unity is a vertical FoV).
	float halfWidth = tanf(toRadians(g_fovH) / 2.0f);

	float forwardX = cosf(toRadians(g_playerA));
	float forwardY = sinf(toRadians(g_playerA));

	float rightX = -forwardY;
	float rightY = forwardX;

	for (float x = 0; x < g_clientWidth; x++) {
		// Compute a ray firing through that point on the image plane.
		float offset = (2.0f * x) / (g_clientWidth - 1.0f) - 1.0f;

		vec2_t lookAt = { forwardX, forwardY };
		lookAt.x += rightX * halfWidth * offset;
		lookAt.y += rightY * halfWidth * offset;

		// angle of the current ray
		float rayAngle = (g_playerA - g_fovH / 2.0f) + ((float)x / (float)g_clientWidth) * g_fovH;

		rayAngle = toDegrees(atan2f(lookAt.y, lookAt.x));

		// partial vector of ray in x and y direction
		float lookX = cosf(toRadians(rayAngle));
		float lookY = sinf(toRadians(rayAngle));
		
		float stepX = lookX == 0 ? 1e30 : fabs(1.0f / lookX);
		float stepY = lookY == 0 ? 1e30 : fabs(1.0f / lookY);

		float startX = lookX > 0 ? (1 - (px - mapX)) * stepX : (px - mapX) * stepX;
		float startY = lookY > 0 ? (1 - (py - mapY)) * stepY : (py - mapY) * stepY;

		int hit = FALSE;

		float dist = min(startX, startY);

		while (!hit) {
			if (startX <= startY) {
				dist = startX;
			}
			else {
				dist = startY;
			}

			int checkX = dist == startX ? roundf(px + lookX * dist) : px + lookX * dist;
			int checkY = dist == startY ? roundf(py + lookY * dist) : py + lookY * dist;

			if (startX < startY && lookX < 0) {
				checkX -= 1;
			}

			if (startY < startX && lookY < 0) {
				checkY -= 1;
			}
			if (map[checkY][checkX] == '#') {
				dist *= cosf(toRadians(rayAngle - g_playerA));

				float ceiling = (g_clientHeight / 2.0f) - (g_clientHeight) / dist;
				float floor = g_clientHeight - ceiling;


				ceiling = max(0, ceiling);
				floor = min(g_clientHeight, floor);

				drawLine(x, 0, x, ceiling, 0xafafaf);
				drawLine(x, ceiling, x, floor, 0xffffff);
				drawLine(x, floor, x, g_clientHeight, 0x0f0f0f);
				hit = TRUE;
				break;
			}

			if (startX <= startY) {
				startX += stepX;
			}
			else {
				startY += stepY;
			}
		}
	}
}