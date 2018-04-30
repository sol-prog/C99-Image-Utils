#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include "PPM.h"
#include "macro_utils.h"

static FILE *readPPMHeader(PPMImage *ppm);
static void readPPMBody(PPMImage *ppm, FILE *fd);
static void read_dummy_line(FILE *fd);
static void read_comments_line(FILE *fd);

PPMImage *createPPM(const char* fname) {
    PPMImage *ppm = malloc(sizeof(PPMImage));
    ON_ERROR_EXIT(ppm == NULL, " ");

    ppm->width = 0;
    ppm->height = 0;

    ppm->file_name = strdup(fname);
    ON_ERROR_EXIT(ppm->file_name == NULL, " ");
    strncpy(ppm->file_format, "P3", sizeof(ppm->file_format));

    FILE *fd = readPPMHeader(ppm);

    ppm->pixels = malloc(sizeof(uint8_t) * ppm->width * ppm->height * 3);
    ON_ERROR_EXIT(ppm->pixels == NULL, " ");

    readPPMBody(ppm, fd);
    return ppm;
}

PPMImage *createEmptyPPM(int width, int height) {
    PPMImage *ppm = malloc(sizeof(PPMImage));
    ON_ERROR_EXIT(ppm == NULL, " ");

    ppm->width = width;
    ppm->height = height;
    ppm->max_val = 255;

    ppm->pixels = calloc(ppm->width * ppm->height * 3, sizeof(uint8_t));
    ON_ERROR_EXIT(ppm->pixels == NULL, " ");

    ppm->file_name = NULL;
    strncpy(ppm->file_format, "P3", sizeof(ppm->file_format));

    return ppm;
}


void destroyPPM(PPMImage *ppm) {
    if(ppm->pixels != NULL) free(ppm->pixels);
    ppm->pixels = NULL;
    if(ppm->file_name != NULL) free(ppm->file_name);
    ppm->file_name = NULL;
    if(ppm != NULL) free(ppm);
    ppm = NULL;
}

void read_dummy_line(FILE *fd) {
    char dummy = '\0';
    while(dummy != '\n') {
        fread(&dummy, sizeof(char), 1, fd);
    }
}

void read_comments_line(FILE *fd) {
    char dummy = '\0';

    for(;;) {
        fread(&dummy, sizeof(char), 1, fd);
        if(dummy == '#') {
            read_dummy_line(fd);
        } else {
            fseek(fd, -sizeof(char), SEEK_CUR);
            break;
        }
    }
}

FILE *readPPMHeader(PPMImage *ppm) {
    ON_ERROR_EXIT(ppm == NULL, "The input PPM image was not initialized.");
    FILE *fd = fopen(ppm->file_name, "rb");
    ON_ERROR_EXIT(fd == NULL, "Unable to open the file name store in the input PPM image.");

    fread(ppm->file_format, sizeof(uint8_t), 2, fd);
    read_dummy_line(fd);

    read_comments_line(fd);

    fscanf(fd, "%d", &ppm->width);
    fscanf(fd, "%d", &ppm->height);

    read_dummy_line(fd);

    fscanf(fd, "%d", &ppm->max_val);
    ON_ERROR_EXIT(ppm->max_val > 255 || ppm->max_val <= 0, "Max Val must be a positive number less or equal 255!");
    read_dummy_line(fd);

    return fd;
}

void readPPMBody(PPMImage *ppm, FILE *fd) {
    ON_ERROR_EXIT(ppm == NULL, "The input PPM image was not initialized.");
    ON_ERROR_EXIT(fd == NULL, "Unable to open the file name store in the input PPM image.");

    if(!strcmp(ppm->file_format, "P6")) {
        size_t read_inputs = fread(ppm->pixels, sizeof(uint8_t), ppm->width * ppm->height * 3, fd);
        if(read_inputs != ppm->width * ppm->height * 3) {
            printf("Warning ... not enough data ...");
        }
    } else if(!strcmp(ppm->file_format, "P3")) {
        int val;
        for(int i = 0; i < ppm->width * ppm->height * 3; ++i) {
            fscanf(fd, "%d", &val);
            ppm->pixels[i] = (uint8_t) val;
        }
    } else {
        printf("Invalid file format for PPM images: %s\n", ppm->file_format);
        destroyPPM(ppm);
        fclose(fd);
        exit(1);
    }

    fclose(fd);
}

void savePPM(PPMImage *ppm, const char *fname) {
    ON_ERROR_EXIT(ppm == NULL, "The input PPM image was not initialized.");
    FILE *fd = fopen(fname, "wb");
    ON_ERROR_EXIT(fd == NULL, "Unable to open file for saving the input image");

    fprintf(fd, "P6\n");
    fprintf(fd, "%d %d\n", ppm->width, ppm->height);
    fprintf(fd, "%d\n", ppm->max_val);
    fwrite(ppm->pixels, sizeof(uint8_t), ppm->width * ppm->height * 3, fd);
    fclose(fd);
}

void convertPPMToGray(PPMImage *ppm) {
    for(int y = 0; y < ppm->height; ++y) {
        for(int x = 0; x < ppm->width; ++x) {
            int index = 3 * (y * ppm->width + x);
            int gray = ((ppm->pixels[index] + ppm->pixels[index + 1] + ppm->pixels[index + 2]) / 3) % 255;

            ppm->pixels[index] = (uint8_t) gray;
            ppm->pixels[index + 1] = (uint8_t) gray;
            ppm->pixels[index + 2] = (uint8_t) gray;
        }
    }
}

uint8_t *getPPMChannel(PPMImage *ppm, int channel) {
    ON_ERROR_EXIT(channel < 0 || channel >= 3, "channel must be a number from 0 to 2");
    uint8_t *ch = malloc(sizeof(uint8_t) * ppm->width * ppm->height);
    ON_ERROR_EXIT(ch == NULL, "Memory allocation error");

    for(int y = 0; y < ppm->height; ++y) {
        for(int x = 0; x < ppm->width; ++x) {
            int index = 3 * (y * ppm->width + x);

            ch[y * ppm->width + x] = ppm->pixels[index + channel];
        }
    }
    return ch;
}

PGMImage *PPMtoPGM(PPMImage *ppm) {
    convertPPMToGray(ppm);
    PGMImage *pgm = malloc(sizeof(PGMImage));
    ON_ERROR_EXIT(pgm == NULL, "Memory allocation error");
    pgm->width = ppm->width;
    pgm->height = ppm->height;
    pgm->max_val = ppm->max_val;
    pgm->pixels = getPPMChannel(ppm, 0);
    return pgm;
}

