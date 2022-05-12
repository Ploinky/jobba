#ifndef __TEXTURE_H_
#define __TEXTURE_H_

#include <stdint.h>
#include <stdio.h>

typedef struct {
    int width;
    int height;
    uint32_t* color;
} texture_t;

void loadTextures(void);

#endif
