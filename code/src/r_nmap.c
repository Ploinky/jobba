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

char map[8][8] = {
	"########",
	"#......#",
	"#...#..#",
	"#......#",
	"#......#",
	"#......#",
	"#......#",
	"########",
};

void renderMapNew() {
	float px = g_playerPos.x + 4.0f;
	float py = g_playerPos.y + 4.0f;

	float lookX = cosf(toRadians(g_playerA));
	float lookY = sinf(toRadians(g_playerA));

	for (int x = 0; x < 8; x++) {
		for (int y = 0; y < 8; y++) {
			if (map[y][x] == '#') {
				fillRect(x * 20, y * 20, (x + 1) * 20, (y + 1) * 20, 0xffffff);
			}
			else {
				drawRect(x * 20, y * 20, (x + 1) * 20, (y + 1) * 20, 0xafafaf);
			}
		}
	}

	// current map tile position of player
	int mapX = (int)(px);
	int mapY = (int)(py);

	fillRect(px * 20 - 1.0f, py * 20 - 1.0f, px * 20 + 1.0f, py * 20 + 1.0f, 0xff0000);
	drawLine(px * 20, py * 20, px * 20 + (5 * lookX), py * 20 + (5 * lookY), 0x00ff00);

	float lastDistToWall = 0;
	for (float x = 0; x < g_clientWidth; x++) {
		/*
		*/
		// angle of the current ray
		float rayAngle = (g_playerA - g_fovH / 2.0f) + ((float)x / (float)g_clientWidth) * g_fovH;

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

				float ceiling = (g_clientHeight / 2.0f) - (g_clientHeight / 2.0f) / dist;
				float floor = g_clientHeight - ceiling;


				drawLine(x, 0, x, ceiling, 0xafafaf);
				drawLine(x, ceiling, x, floor, 0xffffff);
				drawLine(x, floor, x, g_clientHeight, 0x0f0f0f);
				hit = TRUE;
				break;
				/*
				drawLine(px * 20, py * 20, px * 20 + lookX * dist * 20, py * 20 + lookY * dist * 20, startX < startY ? 0x0000ff : 0xff00ff);
				hit = TRUE;
				break;
				*/
			}

			if (startX <= startY) {
				startX += stepX;
			}
			else {
				startY += stepY;
			}
		}

		/*
		/
		float dist = min(startX, startY);

		if (startX <= startY) {
			dist = startX;
		} else {
			dist = startY;
		}

		//drawLine(px * 20, py * 20, px * 20 + lookX * dist * 20, py * 20 + lookY * dist * 20, startX < startY ? 0x0000ff : 0xff00ff);

		int checkX = px + lookX * dist;
		int checkY = py + lookY * dist;

		if (startX < startY && lookX < 0) {
			checkX -= 1;
		}
		
		if (startY < startX && lookY < 0) {
			checkY -= 1;
		}

		if (startX < startY) {
			startX += stepX;
		}
		else {
			startY += stepY;
		}

		//if (map[checkY][checkX] == '#') {
			fillRect(checkX * 20, checkY * 20, (checkX + 1) * 20, (checkY + 1) * 20, 0x00ffff);
		//}
		//else {

			dist = min(startX, startY);

			if (startX < startY) {
				dist = startX;
			}
			else {
				dist = startY;
			}

			drawLine(px * 20, py * 20, px * 20 + lookX * dist * 20, py * 20 + lookY * dist * 20, startX < startY ? 0x0000ff : 0xff00ff);

			checkX = px + lookX * dist;
			checkY = py + lookY * dist;

			if (startX < startY && lookX < 0) {
				checkX -= 1;
			}

			if (startY < startX && lookY < 0) {
				checkY -= 1;
			}

			if (startX < startY) {
				startX += stepX;
			}
			else {
				startY += stepY;
			}

			fillRect(checkX * 20, checkY * 20, (checkX + 1) * 20, (checkY + 1) * 20, 0xffff00);
		//}

		//fillRect(checkX * 20, checkY * 20, (checkX + 1) * 20, (checkY + 1) * 20, 0x00ffff);
		/*
		*
		// angle of the current ray
		float rayAngle = (g_playerA - g_fovH / 2.0f) + ((float)x / (float)g_clientWidth) * g_fovH;

		int hitWall = FALSE;

		int xDir = FALSE;
		int yDir = FALSE;

		// partial vector of ray in x and y direction
		float lookX = cosf(toRadians(rayAngle));
		float lookY = sinf(toRadians(rayAngle));

		// left = false, right = true, up = true, down = false
		xDir = lookX > 0;
		yDir = lookY > 0;

		// step distance to cover 1 tile in x and y direction
		float stepX = fabs(1.0f / lookX);
		float stepY = fabs(1.0f / lookY);

		// initial distance to next tile in look direction (x and y)
		float firstX = lookX != 0 ? (px - (float)mapX) * stepX : 1e30;
		float firstY = lookY != 0 ? (py - (float) mapY) * stepY : 1e30;

		if (xDir) {
			firstX = lookX != 0 ? (1 - (px - (float)mapX)) * stepX : 1e30;
		}

		if (yDir) {
			firstY = lookX != 0 ? (1 - (py - (float)mapY)) * stepY : 1e30;
		}

		float distanceX = fabs(firstX);
		float distanceY = fabs(firstY);

		float distanceToWall = 0;

		printf("POS: %f - %f\n", px, py);
		printf("ANGLE: %f\n", rayAngle);

		while (!hitWall && (lookX == 0 || distanceX < 100.0f) && (lookY == 0 || distanceY < 100.0f)) {
			distanceToWall = max(distanceX, distanceY);

			printf("\trange: %f\n", distanceToWall);
			int checkX = px + (distanceToWall * lookX) * (xDir ? 1 : -1) + (xDir ? 0 : 1);
			int checkY = py + (distanceToWall * lookY) * (yDir ? 1 : -1) + (yDir ? 0 : 1);
			printf("\tchecking: %d(%f) - %d(%f)\n", checkX, px + (distanceToWall * lookX) * (xDir ? 1 : -1) + (xDir ? 0 : 1), checkY, py + (distanceToWall * lookY) * (yDir ? 1 : -1) + (yDir ? 0 : 1));

			// check if wall is hit
			if (checkX < 0 || checkX >= 8 || checkY < 0 || checkY >= 8) {
				hitWall = TRUE;
				distanceX = 100.0f;
				distanceY = 100.0f;
				printf("\tout of range\n");
				break;
			}
			else {
				if (map[8-checkY][checkX] == '#') {
					hitWall = TRUE;
					printf("\thit!\n");
					break;
				}
			}
			printf("\tnohit\n");

			if (distanceX + stepX < distanceY + stepY) {
				// step in x direction
				distanceX += stepX;
			}
			else {
				// step in y direction
				distanceY += stepY;
			}
		}


		printf("%f - %f\n", rayAngle, distanceToWall);
		lastDistToWall = distanceToWall;
		//distanceToWall *= cosf(toRadians(rayAngle - g_playerA));

		float ceiling = (g_clientHeight / 2.0f) - (g_clientHeight / 2.0f) / distanceToWall;
		float floor = g_clientHeight - ceiling;

		drawLine(px * 20, py * 20, px * 20 + lookX * distanceToWall * 20, py * 20 + lookY * distanceToWall * 20, 0x0000ff);
		//drawLine(x, 0, x, ceiling, 0xafafaf);
		//drawLine(x, ceiling, x, floor, 0xffffff);
		//drawLine(x, floor, x, g_clientHeight, 0x0f0f0f);

		/*
		*/
	}
}