#include "texture.h"
#include <Windows.h>

texture_t** g_textures;
int g_textureCount;

void loadTextures() {
    g_textureCount = 1;

    texture_t* notex = malloc(sizeof(texture_t));

    g_textures = malloc(sizeof(void*) * g_textureCount);
    g_textures[0] = notex;

    notex->width = 4;
    notex->height = 4;

    notex->color = malloc(notex->width * notex->height * sizeof(uint32_t));

    for(int i = 0; i < notex->width; i++) {
        for(int j = 0; j < notex->height; j++) {
            uint32_t c = 0xff00ff;
            
            if((i < 2 && j > 1) || (i > 1 && j < 2)) {
                c = 0x000000;
            }

            notex->color[i + j * notex->width] = c;
        }
    }
    
    /*
    create a image with a pixel format of 8bit and the doom palette, set the background colour to a contrasting colour (cyan).

    read width from doom image (word)
    read height from doom image (word)
    read left from doom image (word)
    read top from doom image (word)

    create column_array with width number of elements

    for loop, i = 0, break on i = width - 1
        column_array[i] = read from doom image, 4 bytes
    end block

    for loop, i = 0, break on i = width - 1
        seek doom image to column_array[i] from beginning of doom image

        rowstart = 0

        while loop, rowstart != 255
            read rowstart from doom image, 1 byte
            
            if rowstart = 255, break from this loop

            read pixel_count from doom image, 1 byte

            read dummy_value from doom image, 1 byte

            for loop, j = 0, break on j = pixel_count - 1
                read Pixel from doom image, 1 byte
                
                write Pixel to image, j + rowstart = row, i = column
            end block
            
            read dummy_value from doom image, 1 byte
        end block
    end block
    */
}