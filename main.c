/*
 * main.c
 *
 *  Created on: 7th August 2014
 *      Author: mvalerae
 */
#include "loader.h"

image_error_t InitImage(image_t** dst, int width, int height, int nChannels, int depth);
image_error_t ConvertColor(const image_t* src, image_t** dst);
image_error_t ThresholdImage(const image_t* src, image_t** dst,int threshold);

int main(int argc, char **argv) {

   /*
    if(argc == 3){
		
    }else{

        fprintf(stderr,"missing arguments \n");
        return EXIT_FAILURE;
    }
    */
	
	/*
    image_t *img;
    image_print_error(image_read("./images/input.ppm", &img));
    image_t *grayimg;
    image_print_error(ConvertColor(img, &grayimg));
    */
    image_t *grayimg;
    image_t *bwimg;
    image_print_error(image_read("./images/outhreshold.pgm", &grayimg));
    image_print_error(ThresholdImage(grayimg, &bwimg, 50));
    image_print_error(image_write("./images/outbinary.pgm", bwimg, PGM_FORMAT));
    image_free(grayimg);
    image_free(bwimg);
    return EXIT_SUCCESS;
}

image_error_t InitImage(image_t** dst, int width, int height, int nChannels, int depth) {

	image_t *imgptr = malloc(sizeof(image_t));
	if (!imgptr) {
		return IMG_INSUFFICIENT_MEMORY;
	}

	// Setting width and height
	imgptr->width = width;
	imgptr->height = height;
	
	// Checking nChannels
	if (nChannels != GRAY && nChannels != RGB) {
		return IMG_INVALID_FORMAT;
	}
	imgptr->nChannels = nChannels;
	
	imgptr->widthStep = width * nChannels;
	
	// Checking depth
	if (depth == 1 && nChannels != GRAY) {
		return IMG_INVALID_DEPTH;
	} else if (depth != DEPTH && depth != 1) {
		return IMG_INVALID_DEPTH;
	}
	imgptr->depth = depth;
	
	imgptr->pixelsData = NULL;
	
	*dst = imgptr;

    return IMG_OK;
}

image_error_t ConvertColor(const image_t* src, image_t** dst){

	image_error_t err = InitImage(dst, src->width, src->height, GRAY, DEPTH);
	if (err) {
		return err;
	}
	if (src == NULL) {
		return IMG_INVALID_FORMAT;
	}
	
	int numPixels = src->width * src->height;
	(*dst)->pixelsData = malloc(sizeof(uint8_t) * numPixels);
	
	if (!(*dst)->pixelsData) {
		return IMG_INSUFFICIENT_MEMORY;
	}
	
	uint8_t *pixel = (*dst)->pixelsData;
	for (int i = 0; i < numPixels; i++) {
		if (src->nChannels == RGB) {		
			uint8_t *srcpixel = src->pixelsData + (i * src->nChannels);
			pixel[i] = 0.2126*srcpixel[0] + 0.7152*srcpixel[1]
										  + 0.0722*srcpixel[2]; 
		} else {
			pixel[i] = src->pixelsData[i];
		}
	}

    return IMG_OK;
}

image_error_t ThresholdImage(const image_t* src, image_t** dst,int threshold){

	image_error_t err = InitImage(dst, src->width, src->height, GRAY, 1);
	if (err) {
		return err;
	}
	if (src == NULL) {
		return IMG_INVALID_FORMAT;
	}
	
	int numPixels = src->width * src->height;
	(*dst)->pixelsData = malloc(sizeof(uint8_t) * numPixels);
	
	if (!(*dst)->pixelsData) {
		return IMG_INSUFFICIENT_MEMORY;
	}
	
	if (src->nChannels != GRAY) {
		return IMG_INVALID_FORMAT;
	}
	
	uint8_t *pixel = (*dst)->pixelsData;
	for (int i = 0; i < numPixels; i++) {
		pixel[i] = src->pixelsData[i] > threshold;
	}
    return IMG_OK;
}

// r ../images/input.ppm ../images/outhreshold.pgm 
// r ../images/outhreshold.pgm ../images/outbinary.pgm
