// PGM images helper functions. The code accepts as input ascii/binary PGM images.
// Max Val must be a positve number, that is less or equal 255!
#pragma once

#include <stdio.h>
#include <stdint.h>

// Store PGM image data
typedef struct {
    int width;
    int height;
    int max_val;
    uint8_t *pixels;
    char *file_name;
    char file_format[3];
} PGMImage;

// Load PGM image from a file
PGMImage *createPGM(const char* fname);
// Create an empty PGM image, filled with zeros
PGMImage *createEmptyPGM(int width, int height);
// Create a PGM image from a data buffer
PGMImage *createPGMFromArray(uint8_t *data, int width, int height, int max_val);
// Save a PGM image to a file. The PGM is always saved as a binary PGM.
void savePGM(PGMImage *pgm, const char *fname);
// Free the PGM image structure
void destroyPGM(PGMImage *ppm);
