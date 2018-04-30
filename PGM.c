#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include "PGM.h"
#include "macro_utils.h"

static FILE *readPGMHeader(PGMImage *pgm);
static void readPGMBody(PGMImage *pgm, FILE *fd);
static void read_dummy_line(FILE *fd);
static void read_comments_line(FILE *fd);

PGMImage *createPGM(const char* fname) {
    PGMImage *pgm = malloc(sizeof(PGMImage));
    ON_ERROR_EXIT(pgm == NULL, " ");

    pgm->width = 0;
    pgm->height = 0;

    pgm->file_name = strdup(fname);
    ON_ERROR_EXIT(pgm->file_name == NULL, " ");
    strncpy(pgm->file_format, "P2", sizeof(pgm->file_format));

    FILE *fd = readPGMHeader(pgm);

    pgm->pixels = malloc(sizeof(uint8_t) * pgm->width * pgm->height);
    ON_ERROR_EXIT(pgm->pixels == NULL, " ");

    readPGMBody(pgm, fd);
    return pgm;
}

PGMImage *creatEmptyPGM(int width, int height) {
    PGMImage *pgm = malloc(sizeof(PGMImage));
    ON_ERROR_EXIT(pgm == NULL, " ");

    pgm->width = width;
    pgm->height = height;
    pgm->max_val = 255;

    pgm->pixels = calloc(width * height, sizeof(uint8_t));
    ON_ERROR_EXIT(pgm->pixels == NULL, " ");

    pgm->file_name = NULL;
    strncpy(pgm->file_format, "P2", sizeof(pgm->file_format));

    return pgm;
}

PGMImage *createPGMFromArray(uint8_t *data, int width, int height, int max_val) {
    PGMImage *pgm = malloc(sizeof(PGMImage));
    ON_ERROR_EXIT(pgm == NULL, " ");

    pgm->width = width;
    pgm->height = height;
    pgm->max_val = max_val;

    pgm->pixels = malloc(sizeof(uint8_t) * pgm->width * pgm->height);
    ON_ERROR_EXIT(pgm->pixels == NULL, " ");

    memcpy(pgm->pixels, data, width * height * sizeof(uint8_t));

    pgm->file_name = NULL;
    strncpy(pgm->file_format, "P2", sizeof(pgm->file_format));

    return pgm;
}

void destroyPGM(PGMImage *pgm) {
    if(pgm->pixels != NULL) free(pgm->pixels);
    pgm->pixels = NULL;
    if(pgm->file_name != NULL) free(pgm->file_name);
    pgm->file_name = NULL;
    if(pgm != NULL) free(pgm);
    pgm = NULL;
}

FILE *readPGMHeader(PGMImage *pgm) {
    ON_ERROR_EXIT(pgm == NULL, "The input PGM image was not initialized.");
    FILE *fd = fopen(pgm->file_name, "rb");
    ON_ERROR_EXIT(fd == NULL, "Unable to open the file name store in the input PGM image.");

    fread(pgm->file_format, sizeof(uint8_t), 2, fd);
    read_dummy_line(fd);

    read_comments_line(fd);

    fscanf(fd, "%d", &pgm->width);
    fscanf(fd, "%d", &pgm->height);

    read_dummy_line(fd);

    fscanf(fd, "%d", &pgm->max_val);
    ON_ERROR_EXIT(pgm->max_val > 255 || pgm->max_val <= 0, "Max Val must be a positive number less or equal 255!");
    read_dummy_line(fd);

    return fd;
}

void readPGMBody(PGMImage *pgm, FILE *fd) {
    ON_ERROR_EXIT(pgm == NULL, "The input PGM image was not initialized.");
    ON_ERROR_EXIT(fd == NULL, "Unable to open the file name store in the input PGM image.");

    if(!strcmp(pgm->file_format, "P5")) {
        size_t read_inputs = fread(pgm->pixels, sizeof(uint8_t), pgm->width * pgm->height, fd);
        if(read_inputs != pgm->width * pgm->height) {
            printf("Warning ... not enough data ...");
        }
    } else if(!strcmp(pgm->file_format, "P2")) {
        int val;
        for(int i = 0; i < pgm->width * pgm->height; ++i) {
            fscanf(fd, "%d", &val);
            pgm->pixels[i] = (uint8_t) val;
        }
    } else {
        printf("Invalid file format for PGM images: %s\n", pgm->file_format);
        destroyPGM(pgm);
        fclose(fd);
        exit(1);
    }

    fclose(fd);
}

void savePGM(PGMImage *pgm, const char *fname) {
    ON_ERROR_EXIT(pgm == NULL, "The input PGM image was not initialized.");
    FILE *fd = fopen(fname, "wb");
    ON_ERROR_EXIT(fd == NULL, "Unable to open file for saving the input image");

    fprintf(fd, "P5\n");
    fprintf(fd, "%d %d\n", pgm->width, pgm->height);
    fprintf(fd, "%d\n", pgm->max_val);
    fwrite(pgm->pixels, sizeof(uint8_t), pgm->width * pgm->height, fd);
    fclose(fd);
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
