/*
Copyright (c) 2003 Colorado State University

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software, and to permit persons to whom
the Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef CSU_IMAGE_INCLUDED
#define CSU_IMAGE_INCLUDED

#define PIX_TYPE double
#define RASTER_ID "CSU_SFI"

/* #define INTERPOLATE_FAST 1 */

typedef struct{
    int width;
    int height;
    int channels;
    PIX_TYPE*** data;
} image;

typedef image* Image;

extern "C" Image makeImage(int width,int height,int channels);
extern "C" Image makeZeroImage(int width,int height,int channels);
extern "C" void freeImage(Image i);

extern "C" Image duplicateImage(Image im);



/********************* Image Access Functions *******************/

extern "C" double randBM();

/* Access a single image element using dope vector expantion. symantics: img[x,y,c] */
#define IE( img , x , y , c ) 	( (img)->data[(x)][(y)][(c)] )

/* Safe read-only/range-checked implementation of IE - returns 0.0 if off image. */
#if(!INTERPOLATE_FAST)
PIX_TYPE ie(Image img, int x, int y, int c);
#else
#define ie( img , x , y , c ) ((x < img->width && y < img->height && c < img->channels && x >= 0 && y >=  0 && c >= 0) ? IE(img, x, y, c) : 0.0 )
#endif
/* interpolate a pixel value at a point */


#if(!INTERPOLATE_FAST)
PIX_TYPE interpLinear(Image img, PIX_TYPE x, PIX_TYPE y, int c);
#else
/* Error in Code #define interpLinear(img, x, y, c) ; */
#endif

/********************* Image Manipulation Functions *******************/

extern "C" Image convolveImage(const Image im, const Image mask);

extern "C" double convolvePoint(PIX_TYPE x, PIX_TYPE y, int c, const Image im, const Image mask);

extern "C" void contrastImage(Image im, double scalar);
/* Multiplies everyvalue in im by scalar */

extern "C" void brightnessImage(Image im, double scalar);
/* Adds scalar to every value in im */

extern "C" void ZeroMeanUnitLength(Image im);
/* scales the pixel so that they have zero mean and the image is unit length*/
extern "C" void ZeroMeanUnitLengthMasked(Image im, const char **mask);
/* scales the pixel so that they have zero mean and the image is unit length*/

extern "C" void ZeroMeanOneStdDev(Image im);
/* scales the pixel so that they have zero mean and the image is unit length*/
extern "C" void ZeroMeanOneStdDevMasked(Image im, const char **mask);
/* scales the pixel so that they have zero mean and the image is unit length*/

extern "C" double correlateImages(const Image i1, const Image i2);
/* findes the correlation between two images */
extern "C" double dotImages(const Image i1, const Image i2);
/* find the correlation for images that have zero mean and unit length */

extern "C" Image computeMeanImage(Image* images, int count);
/* takes an array of images and returns the mean of those images.*/

extern "C" Image accumulateChannels(const Image im);
/* generates a new image that is the sum of all channels. */

extern "C" void accumulateImages(Image i1, const Image i2);
/* inplace adds image i2 to image i1 */

extern "C" void gaussianBlur(Image im, double sigma);

/* Add gausian noise to the image. s is the standard
* deviation relative to the total image range returns the actual standard deviation
* of the applied noise
*/
extern "C" double gaussianNoise(Image im, double s);

/* Performs a histogram equalization function.  Output values are in the range 0 to 1 */

extern "C" void histEqual(Image i, int num_bins);
extern "C" void histEqualMask(Image i, int num_bins, const char **mask);
extern "C" void histEqualRect(Image geo, int num_bins, int top, int left, int bottom, int right);

/* shift the pixel values such that the pixels on the left top and right average 0.0 */
extern "C" void ZeroBorder(Image im);

/* Smooth image borders */
extern "C" void smoothImageEdge(Image i, int width);

/* Zero all pixels not in a mask */
extern "C" void applyMask(Image im, const char **mask);

/* Converts a grayscale image into a mask */
extern "C" char** imageToMask(Image im, int c);

PIX_TYPE imageMax(Image im, const char **mask);
PIX_TYPE imageMin(Image im, const char **mask);
PIX_TYPE imageMean (Image im, const char **mask);
PIX_TYPE imageVariance (Image im, const char **mask);
PIX_TYPE imageStdDev (Image im, const char **mask);
PIX_TYPE imageMedian (Image im, const char **mask);
PIX_TYPE imageMad (Image im, const char **mask);
extern "C" int imageNumOfPixels (Image im, const char **mask);

extern "C" void markPoint(Image im, PIX_TYPE x, PIX_TYPE y);

extern "C" Image makeThermalColorImage(Image im);

extern "C" Image makeThermalComboImage(Image thrm, Image bw);


/********************* Image File I/O Functions *******************/

extern "C" Image readPGMImage(const char*);
extern "C" Image readRawImage(const char*);
extern "C" Image readFeretImage(const char *fname, int width, int height,int channels);

extern "C" void  writePGMImage(const Image, const char*,int channel);
extern "C" void  writePPMImage(const Image, const char*);
extern "C" void  writeRawImage(const Image, const char*);
extern "C" void  writeFeretImage(const Image im, const char *fname);

/********************* Image Statistics ************************************/

typedef struct{
    PIX_TYPE max, min, mean, variance, stddev;
    int max_x, max_y, max_c, min_x, min_y, min_c;
} ImageStatistics;

extern "C" void computeImageStatistics(ImageStatistics *is, Image im);
extern "C" void computeImageStatisticsMask(ImageStatistics *is, Image im, const char **mask);
extern "C" void computeImageStatisticsLocal(ImageStatistics *is, Image im, int x, int y, int dist);

#endif
