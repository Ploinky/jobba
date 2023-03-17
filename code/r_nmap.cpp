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

	double t = cross(subtract(q, p), s) / cross(r, s);
	double u = cross(subtract(q, p), r) /
		cross(r, s);

	ret.x = t;
	ret.y = u;

	return ret;
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
	double px = g_playerPos.x + 4.0;
	double py = g_playerPos.y + 4.0;

	// current map tile position of player
	int mapX = (int)(px);
	int mapY = (int)(py);

	// Compute worldspace width of the projection plane (at a depth of 1)
	// using the camera's field of view (which in Unity is a vertical FoV).
	double halfWidth = tan(toRadians(g_fovH) / 2.0f);

	double forwardX = cos(toRadians(g_playerA));
	double forwardY = sin(toRadians(g_playerA));

	double rightX = -forwardY;
	double rightY = forwardX;

	for (int x = 0; x < g_clientWidth; x++) {
		// Compute a ray firing through that point on the image plane.
		double offset = (2.0 * x) / (g_clientWidth - 1.0) - 1.0;

		vec2_t lookAt = { forwardX, forwardY };
		lookAt.x += rightX * halfWidth * offset;
		lookAt.y += rightY * halfWidth * offset;

		// angle of the current ray
		double rayAngle = (g_playerA - g_fovH / 2.0f) + ((float)x / (float)g_clientWidth) * g_fovH;

		rayAngle = toDegrees(atan2(lookAt.y, lookAt.x));

		// partial vector of ray in x and y direction
		double lookX = cos(toRadians(rayAngle));
		double lookY = sin(toRadians(rayAngle));
		
		double stepX = lookX == 0 ? 1e30 : fabs(1.0 / lookX);
		double stepY = lookY == 0 ? 1e30 : fabs(1.0 / lookY);

		double startX = lookX > 0 ? (1 - (px - mapX)) * stepX : (px - mapX) * stepX;
		double startY = lookY > 0 ? (1 - (py - mapY)) * stepY : (py - mapY) * stepY;

		int hit = FALSE;

		double dist = min(startX, startY);

		while (!hit) {
			if (startX <= startY) {
				dist = startX;
			}
			else {
				dist = startY;
			}

			double checkX = dist == startX ? round(px + lookX * dist) : px + lookX * dist;
			double checkY = dist == startY ? round(py + lookY * dist) : py + lookY * dist;

			if (startX < startY && lookX < 0) {
				checkX -= 1;
			}

			if (startY < startX && lookY < 0) {
				checkY -= 1;
			}
			if (map[static_cast<int>(checkY)][static_cast<int>(checkX)] == '#') {
				dist *= cos(toRadians(rayAngle - g_playerA));

				int ceiling = static_cast<int>((g_clientHeight / 2.0) - (g_clientHeight) / dist);
				int floor = static_cast<int>(g_clientHeight - ceiling);


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