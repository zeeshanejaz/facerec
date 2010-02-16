/*
*  csuCommonSubspace.h
*
*  Created by Marcio L. Teixeira on Tue Jun 19 2002.
*
*/

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

/*
Changes Under License Permision By Zeeshan Ejaz Bhatti
Center for Advanced Studies in Engineering, Islamabad, Pakistan.
Implementation of Laplacianfaces Algorithm for Face Recognition
November 2007...
*/

#ifndef CSU_COMMON_SUBSPACE_INCLUDED
#define CSU_COMMON_SUBSPACE_INCLUDED

#include <csuCommon.h>

typedef enum {
    CUTOFF_NONE    = 0,
    CUTOFF_SIMPLE  = 1,
    CUTOFF_ENERGY  = 2,
    CUTOFF_STRETCH = 3,
    CUTOFF_CLASSES = 4,
    CUTOFF_DROPVEC = 5
} CutOffMode;

#define DEFAULT_CUTOFF_PERCENT_SIMPLE  60.0
#define DEFAULT_CUTOFF_PERCENT_ENERGY  90.0
#define DEFAULT_CUTOFF_PERCENT_STRETCH 1.0

typedef struct
{
    int numSubjects;
    int numPixels;
    int basisDim;

    Matrix values;
    Matrix basis;
    Matrix mean;

    int useLDA;
    CutOffMode cutOffMode;
    double cutOff;
    int dropNVectors;

    /*START 	Changed by Zeeshan*/
    int useLPP;
    int neighbourCount;
    int useAdaptiveK;
    int lppKeepNVectors;
    char* lppDistance;
    /*END 	Changed by Zeeshan*/
} Subspace;

extern "C" void subspaceTrain (Subspace *s, Matrix images, ImageList *srt, int numSubjects, int dropNVectors, CutOffMode cutOffMode, double cutOff, int useLDA, int writeTextInterm
                               /*START 	Changed by Zeeshan*/
                               ,int useLPP, int neighbourCount, int useAdaptiveK, int lppKeepNVectors, char* lppDistance
                               /*END 	Changed by Zeeshan*/
                               );

extern "C" void writeSubspace (Subspace *s, char *training_filename, char *imageList, int argc, char**argv);
extern "C" void readSubspace (Subspace *s, const char* trainingFile, int quiet);

extern "C" void validateBasisIsOrthonormal (Matrix basis, int printlevel);

extern "C" Matrix centerThenProjectImages (Subspace *s, Matrix images);

extern "C" void readSubspace (Subspace *s, const char *trainingFile, int quiet);
extern "C" Matrix readAndProjectImages (Subspace *s, char *imageNamesFile, char *imageDirectory, int *numImages, ImageList **srt);

/* The following are prototypes for functions found in the csuSubspace files. */

/* eigensolvers found in csuSubspaceCVEigen */
extern "C" int  cvJacobiEigens_32f ( float* A, float* V, float* E, int n, float eps );
extern "C" int  cvJacobiEigens_64d ( double* A, double* V, double* E, int n, double eps, int desc );

/* basis manipulation */
extern "C" void basis_normalize (Matrix eigenvectors);
extern "C" void mean_subtract_images (Matrix images, Matrix mean);
extern "C" Matrix get_mean_image (Matrix images);
extern "C" void mean_subtract_images_2D (Matrix* images, int numImages, Matrix mean);
extern "C" Matrix get_mean_image_2d (Matrix* images, int numImages);


/* subspace training functions */
extern "C" void eigentrain (Matrix *mean, Matrix *eigen_vals, Matrix *eigen_base, Matrix images);
extern "C" void fisherTrain (Matrix imspca, ImageList *srt, Matrix *fisherBasis, Matrix *fisherValues, int writeTextInterm);

extern "C" void laplacianTrain (Matrix imspca,  ImageList *srt, Matrix * laplacianBasis, Matrix * laplacianValues,
                                int K, int useAdaptiveK, int lppKeepNVectors, Matrix * pcaValues, char* distName, int writeTextInterm);
#endif /* CSU_COMMON_SUBSPACE_INCLUDED */
