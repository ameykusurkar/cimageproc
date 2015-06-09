/*
 * loader.h
 *
 *  Created on: 7th August 2014
 *      Author: mvalerae
 */

#ifndef LOADER_H_
#define LOADER_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * The different type of image formats that loader accept
 */
typedef enum {PBM_FORMAT, PPM_FORMAT,PGM_FORMAT} imageformat;

/*
 * The maximum intensity of an RGB and GRAY value.
 */
enum { DEPTH = 255 };
/*
 * number of channels in image
 */
enum {GRAY = 1, RGB= 3};
/*
 * The buffer size to be used by the PPM loader.
 */
enum { BUFFER_SIZE = 16 };
/*
 * An image.
 */
typedef struct {
  int width, height;
  int nChannels;
  int widthStep;
  int depth;
  uint8_t *pixelsData;
} image_t;


/*
 * An image error code.
 */
typedef enum{
  IMG_OK,
  IMG_OPEN_FAILURE,
  IMG_MISSING_FORMAT,
  IMG_INVALID_FORMAT,
  IMG_INSUFFICIENT_MEMORY,
  IMG_INVALID_SIZE,
  IMG_INVALID_DEPTH,
  IMG_READ_FAILURE,
  IMG_WRITE_FAILURE
} image_error_t;

/*
 * API prototypes.
 */

extern void image_print_error(image_error_t);
extern void image_free(image_t *);
extern image_error_t image_read(const char *, image_t **);
extern image_error_t image_write(const char *, image_t *, imageformat);


#endif /* LOADER_H_ */
