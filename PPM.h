// Read/Write PPM images. The code accepts as input ascii/binary PPM images.
// Max Val must be a positve number, that is less or equal 255!
#pragma once

#include <stdio.h>
#include <stdint.h>

#include "PGM.h"

// Store PPM image data
typedef struct {
    int width;
    int height;
    int max_val;
    uint8_t *pixels;
    char file_format[3];
} PPMImage;

// Load PPM image from a file
PPMImage *createPPM(const char* fname);
// Create an empty PPM image, filled with zeros
PPMImage *createEmptyPPM(int width, int height);
// Save a PPM image in a file. The PPM is always saved as a binary PPM.
void savePPM(PPMImage *ppm, const char *fname);
// Free the PPM image structure
void destroyPPM(PPMImage *ppm);
// Convert the PPM image to a gray image
void convertPPMToGray(PPMImage *ppm);
// Extract one of the R, G, B channels from the PPM image.
// The user needs to free the memory for the returned pointer.
uint8_t *getPPMChannel(PPMImage *ppm, int channel);
// Convenience function to convert a PPM image to PGM. The input PPM is not modified
PGMImage *PPMtoPGM(PPMImage *ppm);
// Flip image horizontally (in place)
void flipPPMHorizontally(PPMImage *ppm);
// Flip image verically (in place)
void flipPPMVertically(PPMImage *ppm);
