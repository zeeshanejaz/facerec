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

Changes in this file:
	Changed the struct Subspace for support LPP
	Changed function prototype declaration	
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

  /*START 	Changed by Zeeshan: For LPP*/
  int useLPP;
  int neighbourCount;
  int useAdaptiveK;
  int lppKeepNVectors;
  char* lppDistance;
  /*END 	Changed by Zeeshan: For LPP*/

  /*START 	Changed by Zeeshan: For ICA*/
  int useICA;
  int arch;
  Matrix ica2Basis; // ICA2 requires separate basis
  double learningRate;
  int blockSize;
  int iterations;
  /*END 	Changed by Zeeshan: For ICA*/
} Subspace;

void subspaceTrain (Subspace *s, Matrix images, ImageList *srt, int numSubjects, int dropNVectors, CutOffMode cutOffMode, double cutOff, int useLDA, int writeTextInterm
  /*START 	Changed by Zeeshan: For LPP*/
  ,int useLPP, int neighbourCount, int useAdaptiveK, int lppKeepNVectors, char* lppDistance
  /*END 	Changed by Zeeshan: For LPP*/
  /*START 	Changed by Zeeshan: For ICA*/
  ,int useICA, int arch, double learningRate, int blockSize, int iterations
  /*END 	Changed by Zeeshan: For ICA*/
);

void writeSubspace (Subspace *s, char *training_filename, char *imageList, int argc, char**argv);
void readSubspace (Subspace *s, const char* trainingFile, int quiet);

void validateBasisIsOrthonormal (Matrix basis, int printlevel);

Matrix centerThenProjectImages (Subspace *s, Matrix images);

void readSubspace (Subspace *s, const char *trainingFile, int quiet);
Matrix readAndProjectImages (Subspace *s, char *imageNamesFile, char *imageDirectory, int *numImages, ImageList **srt);

/* The following are prototypes for functions found in the csuSubspace files. */

/* eigensolvers found in csuSubspaceCVEigen */
int  cvJacobiEigens_32f ( float* A, float* V, float* E, int n, float eps );
int  cvJacobiEigens_64d ( double* A, double* V, double* E, int n, double eps, int desc );

/* basis manipulation */
void basis_normalize (Matrix eigenvectors);
void mean_subtract_images (Matrix images, Matrix mean);
Matrix get_mean_image (Matrix images);

/* subspace training functions */
void eigentrain (Matrix *mean, Matrix *eigen_vals, Matrix *eigen_base, Matrix images);
void fisherTrain (Matrix imspca, ImageList *srt, Matrix *fisherBasis, Matrix *fisherValues, int writeTextInterm);

/*START 	Changed by Zeeshan: For LPP & ICA*/
void laplacianTrain (Matrix imspca,  ImageList *srt, Matrix * laplacianBasis,
 Matrix * laplacianValues, int K, int useAdaptiveK, int lppKeepNVectors, Matrix * pcaValues, char* distName, int writeTextInterm);
void independentTrain (Matrix pcaBasis, Matrix impca, Matrix * independentBasis, int architecture, int blockSize, double learningRate, int iterations);
/*END 		Changed by Zeeshan: For LPP & ICA*/
#endif /* CSU_COMMON_SUBSPACE_INCLUDED */
