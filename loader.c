/*
 * loader.c
 *
 *  Created on: 7th August 2014
 *      Author: mvalerae
 */

#include "loader.h"

/*
 * A table of error messages relating to the error codes defined in
 * image_error_t.
 */
static char *image_error_table[] =
  { "",
    "Error: could not open '%s'.\n",
    "Error: could not read image format from '%s'.\n",
    "Error: invalid image format found in '%s'.\n",
    "Error: insufficient memory to process '%s'.\n",
    "Error: '%s' does not contain valid image sizes.\n",
    "Error: '%s' contains an invalid depth.\n",
    "Error: could not read pixel data from '%s'.\n",
    "Error: could not write pixel data from '%s'.\n"
  };

/*
 * Prints an error message that both mentions the given filename and
 * corresponds to the supplied error code.
 */
void image_print_error(image_error_t error) {
  fprintf(stderr,"%s", image_error_table[error]);
 }

/*
 * Frees the memory associated with a PPM/PBM image. It is safe to pass NULL to
 * this function.
 */
void image_free(image_t *image) {
  if (image == NULL) {
    return;
  }

  if (image->pixelsData != NULL) {
    free(image->pixelsData);
  }

  free(image);
}

/*
 * Attempts to read a PGM/PPM/PBM image stored in P4,P5 OR P6 format from the given file.
 * Returns IMAGE_OK on success or an appropriate error code on failure. The
 * parameter out will contain the loaded image if and only if the return value
 * is IMAGE_OK.
 */
image_error_t image_read(const char *filename, image_t **out) {
  image_t *image;

  FILE *in;
  char buffer[BUFFER_SIZE], c;
  imageformat type;

  /*
   * Open the file for reading.
   */
  in = fopen(filename, "rb");
  if (!in) {
    return IMG_OPEN_FAILURE;
  }

  /*
   * Read in the image format; P6 and P4 is currently the only format supported.
   */
  if (fgets(buffer, BUFFER_SIZE, in) == NULL) {
    return IMG_MISSING_FORMAT;
  }

  if (buffer[0] != 'P' && ( buffer[1] != '4' || buffer[1] != '6' || buffer[1] == '5' )) {
    return IMG_INVALID_FORMAT;
  }

  /*
   * Allocate space for the image header.
   */
  image = malloc(sizeof(image_t));

  if (image == NULL) {
      return IMG_INSUFFICIENT_MEMORY;
  }

  if( buffer[1] == '4' ||buffer[1] == '5' ){
      image->nChannels = 1;
      image->depth = 255;
      type = PBM_FORMAT;
  }else{
      image->nChannels = 3;
      type = PPM_FORMAT;
  }

  c = fgetc(in);
   while (c == '#') {
     while (fgetc(in) != '\n') { }
     c = fgetc(in);
   }

   ungetc(c, in);

  /*
   * Read the image's size information.
   */
  if (fscanf(in, "%d %d", &image->width, &image->height) != 2) {
    return IMG_INVALID_SIZE;
  }
  /*
   * Add the widthstep information to allow to move between array
   */
  image->widthStep = image->nChannels * image->width;
  /*
   * Read and check the image's RGB component depth in PPM image.
   */
  if(type == PPM_FORMAT){
      if ((fscanf(in, "%d", &image->depth) != 1) || image->depth != DEPTH) {
          return IMG_INVALID_DEPTH;
      }
  }

  /*
   * Skip over any remaining cruft preceding the pixel data.
   */
  while (fgetc(in) != '\n') { }

  /*
   * Allocate and read pixel data.
   */
  image->pixelsData = malloc(image->width * image->height * image->nChannels * sizeof(uint8_t));
  if (!image->pixelsData) {
    return IMG_INSUFFICIENT_MEMORY;
  }

  if (fread(image->pixelsData, image->nChannels * image->width *sizeof(uint8_t),
        image->height, in) != image->height) {

    image_free(image);
    return IMG_READ_FAILURE;
  }

  fclose(in);
  *out = image;
  return IMG_OK;
}

/*
 * Writes the supplied image to the given file. Returns IMAGE_OK on success, or
 * an appropriate error code on failure.
 */
image_error_t image_write(const char *filename, image_t *image, imageformat type) {
  FILE *out;

  /*
   * Open file for output.
   */
  out = fopen(filename, "wb");
  if (!out) {
      return IMG_OPEN_FAILURE;
  }

  /*
   * Print the header.
   */
  if(type == PPM_FORMAT){
      fprintf(out, "P6\n");
      fprintf(out, "%d %d\n", image->width, image->height);
      fprintf(out, "%d\n", image->depth);
  }else if(type == PBM_FORMAT){
      fprintf(out, "P4\n");
      fprintf(out, "%d %d\n", image->width, image->height);
  }else if(type == PGM_FORMAT){
      fprintf(out, "P5\n");
      fprintf(out, "%d %d\n", image->width, image->height);
      fprintf(out, "%d\n", image->depth);
  }else{
	  fclose(out);
	  return IMG_INVALID_FORMAT;
  }

  /*
   * Print the pixel data.
   */
  if (!fwrite(image->pixelsData, image->nChannels * image->width, image->height, out)){

	  image_free(image);
	  return IMG_WRITE_FAILURE;
  }
  fclose(out);
  return IMG_OK;
}

