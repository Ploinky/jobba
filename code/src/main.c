#include "main.h"
#include "texture.h"

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")


extern texture_t** g_textures;
extern int g_textureCount;

void draw_pixel(int x, int y, u32 color) {
    if(x < 0 || x >= client_width || y < 0 || y >= client_height) {
        return;
    }
    memory[(y * client_width + x)] = color;
}

void clear_screen(u32 color) {
    for(int i = 0; i < client_width * client_height; ++i) {
        memory[i] = color;
    }
}

void draw_line(int x1, int y1, int x2, int y2, u32 color) {
    int x = 0;
    int y = 0;

    int xinc1 = 0;
    int xinc2 = 0;
    int yinc1 = 0;
    int yinc2 = 0;

    float deltax = abs(x2 - x1); // The difference between the x's
    float deltay = abs(y2 - y1); // The difference between the y's
    x = x1; // Start x off at the first pixel
    y = y1; // Start y off at the first pixel

    if (x2 >= x1) // The x-values are increasing
    {
        xinc1 = 1;
        xinc2 = 1;
    }
    else // The x-values are decreasing
    {
        xinc1 = -1;
        xinc2 = -1;
    }

    if (y2 >= y1) // The y-values are increasing
    {
        yinc1 = 1;
        yinc2 = 1;
    }
    else // The y-values are decreasing
    {
        yinc1 = -1;
        yinc2 = -1;
    }

    
    float den = deltax;
    float num = deltax / 2;
    float numadd = deltay;
    float numpixels = deltax; // There are more x-values than y-values

    if (deltax >= deltay) // There is at least one x-value for every y-value
    {
        xinc1 = 0; // Don't change the x when numerator >= denominator
        yinc2 = 0; // Don't change the y for every iteration
        den = deltax;
        num = deltax / 2;
        numadd = deltay;
        numpixels = deltax; // There are more x-values than y-values
    }
    else // There is at least one y-value for every x-value
    {
        xinc2 = 0; // Don't change the x for every iteration
        yinc1 = 0; // Don't change the y when numerator >= denominator
        den = deltay;
        num = deltay / 2;
        numadd = deltax;
        numpixels = deltay; // There are more y-values than x-values
    }

    for (int curpixel = 0; curpixel <= numpixels; curpixel++)
    {
        draw_pixel(x, y, color); // Draw the current pixel
        num += numadd; // Increase the numerator by the top of the fraction
        if (num >= den) // Check if numerator >= denominator
        {
            num -= den; // Calculate the new numerator value
            x += xinc1; // Change the x as appropriate
            y += yinc1; // Change the y as appropriate
        }
    
        x += xinc2; // Change the x as appropriate
        y += yinc2; // Change the y as appropriate
    } 
}

void fill_square(int x1, int y1, int x2, int y2, u32 color) {
    if(x2 < x1) {
        int temp = x1;
        x1 = x2;
        x2 = temp;
    }

    if(y2 < y1) {
        int temp = y1;
        y1 = y2;
        y2 = temp;
    }

    //drawing line starts here
    int dx = x2 - x1;
    int dy = y2 - y1;

    for(int x = x1; x < x2; x++) {
        for (int y = y1; y < y2; y++) {
            draw_pixel(x, y, color);
        }
    }
}

short keys[WM_KEYLAST];

LRESULT CALLBACK window_proc(HWND window, UINT message, WPARAM w_param, LPARAM l_param) {
    switch(message)
    {
        case WM_KEYDOWN:
        {
            keys[w_param] = 1;
        } break;

        case WM_KEYUP:
        {
            keys[w_param] = 0;
            
            break;
        }
        
        case WM_DESTROY:
        {
            PostQuitMessage(0);
        } break;

        default:
        {
            return DefWindowProc(window, 
                                 message, 
                                 w_param, 
                                 l_param);
        }
    }
    
    return 0;
}

float cross(vec2_t a, vec2_t b) {
    return a.x * b.y - a.y * b.x;
}

float dot (vec2_t a, vec2_t b) {
    return a.x * b.x + a.y * b.y;
}

vec2_t subtract(vec2_t a, vec2_t b) {
    vec2_t ret = {a.x - b.x, a.y - b.y};

    return ret;
}

float length(vec2_t vec) {
    return sqrt(vec.x * vec.x + vec.y * vec.y);
}


float toDegrees(float radians) {
    return radians * (180.0 / M_PI);
}

float toRadians(float degrees) {
    return degrees * (M_PI / 180.0); 
}

vec2_t intersect(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4) {
    
            vec2_t q = { x1, y1 };
            vec2_t s = { x2 - x1, y2 - y1 };

            vec2_t p = { x3, y3};
            vec2_t r = { x4 - x3, y4 - y3 };
            
            vec2_t ret = { NAN, NAN };

            if(cross(r, s) == 0) {
                return ret;
            }

            float u = cross(subtract(q, p), r) / cross(r, s);

            if(u < 0 || u > 1) {
                return ret;
            }

            float t = cross(subtract(q, p), s) / cross(r, s);

            if(t < 0 || t > 1 ) {
                return ret;
            }

            ret.x = p.x + r.x * t;
            ret.y = p.y + r.y * t;
            
            return ret;
}

float clamp(float d, float min, float max) {
  const float t = d < min ? min : d;
  return t > max ? max : t;
}

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE prev_instance, PWSTR cmd_line, int cmd_show) {
    AllocConsole();

    freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
    freopen_s((FILE**)stderr, "CONOUT$", "w", stderr);

    loadTextures();

    WNDCLASS window_class = {0};
    
    const wchar_t class_name[] = L"MyWindowClass";
    
    window_class.lpfnWndProc = window_proc;
    window_class.hInstance = instance;
    window_class.lpszClassName = class_name;
    window_class.hCursor = LoadCursor(0, IDC_CROSS);
    
    if(!RegisterClass(&window_class))
    {
        MessageBox(0, L"RegisterClass failed", 0, 0);
        return GetLastError();
    }
    
    HWND window = CreateWindowEx(0,
                                 class_name,
                                 L"Window",
                                 WS_OVERLAPPEDWINDOW|WS_VISIBLE,
                                 CW_USEDEFAULT,
                                 CW_USEDEFAULT,
                                 680,
                                 460,
                                 0,
                                 0,
                                 instance,
                                 0);
    
    if(!window) {
        MessageBox(0, L"CreateWindowEx failed", 0, 0);
        return GetLastError();
    }
    
    // allocate memory
    
    RECT rect;
    GetClientRect(window, &rect);
    client_width = rect.right - rect.left;
    client_height = rect.bottom - rect.top;
    
    memory = VirtualAlloc(0,
                          client_width * client_height * 4,
                          MEM_RESERVE|MEM_COMMIT,
                          PAGE_READWRITE
                          );

    float fovH = 90;
    float fovV = fovH * (((float) client_height) / ((float) client_width));
        
    // create BITMAPINFO struct for StretchDIBits
    
    BITMAPINFO bitmap_info;
    bitmap_info.bmiHeader.biSize = sizeof(bitmap_info.bmiHeader);
    bitmap_info.bmiHeader.biWidth = client_width;
    bitmap_info.bmiHeader.biHeight = client_height;
    bitmap_info.bmiHeader.biPlanes = 1;
    bitmap_info.bmiHeader.biBitCount = 32;
    bitmap_info.bmiHeader.biCompression = BI_RGB;
    
    HDC hdc = GetDC(window);
    
    // loop
    
    int running = 1;

    // clear screen with gray color
    clear_screen(0x333333);

    long tickCount = clock();
    
    struct timeb tmb;
    ftime(&tmb);
    tickCount = tmb.time * 1000 + tmb.millitm;

    playerHeight = 2;

    int wallCount = 1;
    wall_t* walls = malloc(sizeof(wall_t) * wallCount);

    wall_t wall1 = {90, 10, 90, 90, 0, 6, 0};
    wall_t wall2 = {90, 90, 10, 90, 0, 6, 0};
    wall_t wall3 = {10, 90, 10, 10, 0, 6, 0};
    wall_t wall4 = {10, 10, 90, 10, 0, 6, 0};
    walls[0] = wall1;
    //walls[1] = wall2;
    //walls[2] = wall3;
    //walls[3] = wall4;

    u32* colors = malloc(sizeof(u32) * wallCount);
    colors[0] = 0xff0000;
    //colors[1] = 0xff0000;
    //colors[2] = 0xff0000;
    //colors[3] = 0xff0000;

    playerX = 15;
    playerY = 15;

    playerA = 0;

    for(int i = 0; i < WM_KEYLAST; i++) {
        keys[i] = 0;
    }

    while(running)
    {
        ftime(&tmb);
        long now = tmb.time * 1000 + tmb.millitm;

        float dt = (now - tickCount) / 1000.0f;

        tickCount = now;

        MSG msg;
        while(PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
        {
            if(msg.message == WM_QUIT) running = 0;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if(keys[VK_ESCAPE]) {
            running = 0;
        }
        
        if(keys['E']) {
            playerA -= 180.0f * dt;
        }

        if(keys['Q']) {
            playerA += 180.0f * dt;
        }

        float newX = 0;
        float newY = 0;

        if(keys['W']) {
            newX += cos(toRadians(playerA));
            newY += sin(toRadians(playerA));
        }
        
        if(keys['S']) {
            newX -= cos(toRadians(playerA));
            newY -= sin(toRadians(playerA));
        }
        
        if(keys['D']) {
            newX += sin(toRadians(playerA));
            newY -= cos(toRadians(playerA));
        }
        
        if(keys['A']) {
            newX -= sin(toRadians(playerA));
            newY += cos(toRadians(playerA));
        }

        playerX += newX * dt * 30.0f;
        playerY += newY * dt * 30.0f;
        
        if(playerA > 360.0f) {
            playerA -= 360;
        } else if(playerA < -360.0f) {
            playerA += 360;
        }

        clear_screen(0x000000);

        draw_line(10, 10, 10, 210, 0x0000ff);
        draw_line(10, 210, 210, 210, 0x0000ff);
        draw_line(210, 210, 210, 10, 0x0000ff);
        draw_line(210, 10, 10, 10, 0x0000ff);

        draw_line(220, 10, 220, 210, 0x00ff00);
        draw_line(220, 210, 420, 210, 0x00ff00);
        draw_line(420, 210, 420, 10, 0x00ff00);
        draw_line(420, 10, 220, 10, 0x00ff00);

        draw_line(430, 10, 430, 210, 0xff0000);
        draw_line(430, 210, 630, 210, 0xff0000);
        draw_line(630, 210, 630, 10, 0xff0000);
        draw_line(630, 10, 430, 10, 0xff0000);

        for(int w = 0; w < wallCount; w++) {
            vec2_t wallStart = { walls[w].startX - playerX, walls[w].startY - playerY };
            vec2_t wallEnd = { walls[w].endX - playerX, walls[w].endY - playerY };
            
            float playerACos = cos(toRadians(playerA));
            float playerASin = sin(toRadians(playerA));

            // Wall start transformed to player coordinate system
            vec2_t wst = { wallStart.x * playerASin - wallStart.y * playerACos, wallStart.x * playerACos  + wallStart.y * playerASin };

            // Wall end transformed to player coordinate system
            vec2_t wet = { wallEnd.x * playerASin - wallEnd.y * playerACos, wallEnd.x * playerACos  + wallEnd.y * playerASin };
            
            draw_line(walls[w].startX + 10.0f, walls[w].startY + 10.0f, walls[w].endX + 10.0f, walls[w].endY + 10.0f, 0x00ffff);
            draw_line(wst.x + 320.0f, wst.y + 105.0f, wet.x + 320.0f, wet.y + 105.0f, 0x00ffff);

            
            // Clip parts of wall outside view
            if(wst.y > 0 || wet.y > 0)
            {
                float nearz = 1e-4f, farz = 5, nearside = 1e-5f, farside = 20.f;
                // Find an intersection between the wall and the approximate edges of player's view
                vec2_t i1 = intersect(wst.x,wst.y,wet.x,wet.y, -nearside,nearz, -farside,farz);
                vec2_t i2 = intersect(wst.x,wst.y,wet.x,wet.y,  nearside,nearz,  farside,farz);
                if(wst.y < nearz) { if(i1.y > 0) { wst.x = i1.x; wst.y = i1.y; } else { wst.x = i2.x; wst.y = i2.y; } }
                if(wet.y < nearz) { if(i1.y > 0) { wet.x = i1.x; wet.y = i1.y; } else { wet.x = i2.x; wet.y = i2.y; } }
            
                float x1 = -wst.x * 16 / wst.y, y1a = -50 / wst.y, y1b = 50 / wst.y;
                float x2 = -wet.x * 16 / wet.y, y2a = -50 / wet.y, y2b = 50 / wet.y;
                draw_line(x1 + 530.0f, y1a + 105.0f, x2 + 530.0f, y2a + 105.0f, 0x00ffff);
                draw_line(x1 + 530.0f, y1b + 105.0f, x2 + 530.0f, y2b + 105.0f, 0x00ffff);
                draw_line(x1 + 530.0f, y1a + 105.0f, x1 + 530.0f, y1b + 105.0f, 0x00ffff);
                draw_line(x2 + 530.0f, y2a + 105.0f, x2 + 530.0f, y2b + 105.0f, 0x00ffff);
            }
        }

        fill_square(playerX - 1 + 10, playerY - 1 + 10, playerX + 1 + 10, playerY + 1 + 10, 0x00ffff);
        fill_square(319, 104, 321, 106, 0x00ffff);
        draw_line(playerX + 10, playerY + 10, playerX + 10 + 10 * cos(toRadians(playerA)), playerY + 10 + 10 * sin(toRadians(playerA)), 0xff0000);
        draw_line(320, 105, 320, 115, 0xff0000);
        /*
        
            // Wall vertices relative to player location
            vec2_t wallStart = { walls[w].startX - playerX, walls[w].startY - playerY };
            vec2_t wallEnd = { walls[w].endX - playerX, walls[w].endY - playerY };

            float playerACos = cos(toRadians(playerA));
            float playerASin = sin(toRadians(playerA));

            // Wall start transformed to player coordinate system
            vec2_t wst = { wallStart.x * playerASin - wallStart.y * playerACos, wallStart.x * playerACos  + wallStart.y * playerASin };

            // Wall end transformed to player coordinate system
            vec2_t wet = { wallEnd.x * playerASin - wallEnd.y * playerACos, wallEnd.x * playerACos  + wallEnd.y * playerASin };

            if(wst.y <= 0 && wet.y <= 0) {
                printf("Clipped wall #%d\n", w);
                continue;
            }

            // Clip parts of wall outside view
            if(wst.y <= 0 || wet.y <= 0)
            {
                float nearz = 1e-4f, farz = 5, nearside = 1e-5f, farside = 20.f;
                // Find an intersection between the wall and the approximate edges of player's view
                vec2_t i1 = intersect(wst.x,wst.y,wet.x,wet.y, -nearside,nearz, -farside,farz);
                vec2_t i2 = intersect(wst.x,wst.y,wet.x,wet.y,  nearside,nearz,  farside,farz);
                if(wst.y < nearz) { if(i1.y > 0) { wst.x = i1.x; wst.y = i1.y; } else { wst.x = i2.x; wst.y = i2.y; } }
                if(wet.y < nearz) { if(i1.y > 0) { wet.x = i1.x; wet.y = i1.y; } else { wet.x = i2.x; wet.y = i2.y; } }
            }

            /* Do perspective transformation 
            float xscale1 = fovH / wst.y, yscale1 = fovV / wst.y;    int x1 = client_width/2.0f - (int)(wst.x * xscale1);
            float xscale2 = fovH / wet.y, yscale2 = fovV / wet.y;    int x2 = client_width/2.0f - (int)(wet.x * xscale2);

            // Only render if it's visible
            // if(x1 >= x2 || x2 < now.sx1 || x1 > now.sx2) continue;

            /* Acquire the floor and ceiling heights, relative to where the player's view is 
            float yceil  = 4  - 2;
            float yfloor = 0 - 2;

            /* Project our ceiling & floor heights into screen coordinates (Y coordinate) 
            #define Yaw(y,z) (y + z*playerA)
            int y1a  = client_height / 2 - (int)(Yaw(yceil, wst.y) * yscale1),  y1b = client_height / 2 - (int)(Yaw(yfloor, wst.y) * yscale1);
            int y2a  = client_height / 2 - (int)(Yaw(yceil, wet.y) * yscale2),  y2b = client_height / 2 - (int)(Yaw(yfloor, wet.y) * yscale2);
           
        
            int beginx = x1, endx = x2;
            for(int x = beginx; x <= endx; ++x)
            {
                /* Calculate the Z coordinate for this point. (Only used for lighting.) 
                int z = ((x - x1) * (wet.y-wst.y) / (x2-x1) + wst.y) * 8;
                /* Acquire the Y coordinates for our ceiling & floor for this X coordinate. Clamp them. 
                int ya = (x - x1) * (y2a-y1a) / (x2-x1) + y1a, cya = clamp(ya, 4,0); // top
                int yb = (x - x1) * (y2b-y1b) / (x2-x1) + y1b, cyb = clamp(yb, 4,0); // bottom

                cya = ya;
                cyb = yb;
                
                printf("%d - %d\n", cya, cyb);
                /* Render ceiling: everything above this sector's ceiling height. 
                // vline(x, 4, cya-1, 0x111111 ,0x222222,0x111111);
                for(int y = client_height - 1; y > cya+1; y--) {
                    // draw_pixel(x, y, 0x222222);
                    draw_pixel(x, y, 0xff00000);
                }

                /* Render floor: everything below this sector's floor height. 
                for(int y = cyb+1; y > 0; y--) {
                    // int texY = (y - startY) / height * texture->height;
                    // int texIndex = (texY * texture->width + texX);
                    // draw_pixel(x, y, texture->color[texIndex]);

                    //draw_pixel(x, cyb+1, 0x0000AA);
                    draw_pixel(x, cyb+1, 0x0000FF);
                }


                unsigned r = 0x010101 * (255-z);
                /* There's no neighbor. Render wall from top (cya = ceiling level) to bottom (cyb = floor level). 
                for(int y = cya; y >= cyb; y--) {
                    //draw_pixel(x, y, 0xf0f0f0);
                    draw_pixel(x, y, 0x00ff00);
                    //vline(x, cya, cyb, 0, x==x1||x==x2 ? 0 : r, 0);
                }
                */
            /*
            vec2_t s = { walls[w].endX - walls[w].startX, walls[w].endY - walls[w].startY };

            vec2_t playerNormal = { cos(toRadians(playerA)), sin(toRadians(playerA)) };
            vec2_t wallNormal = { -s.y, s.x };
            
            float ang = toDegrees(acos(dot(playerNormal, wallNormal) / length(playerNormal) / length(wallNormal)));

            if(ang < 45 || ang > 315) {
                continue;
            }

            vec2_t p = { playerX, playerY};
            vec2_t q = { walls[w].startX, walls[w].startY };
            texture_t* texture = g_textures[0];

            for(int x = 0; x < client_width; x++) {

                float rayAngle = (playerA - fovH / 2.0f) + ((float) x / ((float) client_width)) * fovH;

                vec2_t r = { cos(toRadians(rayAngle)), sin(toRadians(rayAngle)) };
                
                if(cross(r, s) == 0) {
                    continue;
                }

                float u = cross(subtract(q, p), r) / cross(r, s);

                if(u < 0 || u > 1) {
                    continue;
                }

                float t = cross(subtract(q, p), s) / cross(r, s);

                if(t < 0 ) {
                    continue;
                }

                vec2_t intersect = { p.x + r.x * t, p.y + r.y * t };
                vec2_t dist = { intersect.x - playerX, intersect.y - playerY };

                float a = walls[w].height / 2;
                float c = length(dist);
                float b = sqrt(a * a + c * c);

                float oA = (-playerHeight + walls[w].posY) / 2;
                float oB = sqrt(oA * oA + c * c);

                float angle = acos((b*b + c*c - a*a) / (2.0f * b * c)) * 2;
                float angleOffset = acos((oB*oB + c*c - oA*oA) / (2.0f * oB * c)) * 2;
                float height = ((float) client_height) * (toDegrees(angle) / fovV);
                float offset = ((float) client_height) * (toDegrees(angleOffset) / fovV);

                if(oA < 0) {
                    offset *= -1;
                }

                int startY = client_height / 2.0f + offset;

                if(startY > client_height / 2) {
                    printf("%f - %f - %d\n", height, offset, startY);
                }
                int endY = startY + height;

                int texX = u * length(s) / length(s)  * texture->width;

                for(int y = 0; y < startY; y++) {
                    draw_pixel(x, y, 0x0f0f0f);
                }
                for(int y = max(0.0f, startY); y < min(client_height, endY + 1); y++) {
                    int texY = (y - startY) / height * texture->height;
                    int texIndex = (texY * texture->width + texX);
                    
                    draw_pixel(x, y, texture->color[texIndex]);
                }
                for(int y = endY + 1; y < client_height; y++) {
                    draw_pixel(x, y, 0xf0f0f0);
                }
            }
        */

        StretchDIBits(hdc,
                      0,
                      0,
                      client_width,
                      client_height,
                      0,
                      0,
                      client_width,
                      client_height,
                      memory,
                      &bitmap_info,
                      DIB_RGB_COLORS,
                      SRCCOPY
                      );
   }

   VirtualFree(memory, client_width * client_height * 4, MEM_RELEASE);
   free(walls);
   free(colors);
    
    return 0;
}