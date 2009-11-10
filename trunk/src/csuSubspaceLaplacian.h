#include "csuCommon.h"
#include <math.h>


//void laplacianTrain (Subspace*s, Matrix imspca, Matrix * laplacianBasis, Matrix * laplacianValues, int writeTextInterm);
void laplacianTrain (Matrix imspca,  ImageList *srt, Matrix * laplacianBasis, Matrix * laplacianValues,
int K, int useAdaptiveK, int lppKeepNVectors, Matrix * pcaValues, char* distName, int writeTextInterm);
Matrix computeKMat(Matrix Dist);
Matrix findLMatrix(Matrix W, Matrix D);
Matrix findDMatrix(Matrix Dist);
Matrix findWMatrix(Matrix Dist, int K, Matrix kMat);
void applyClassInfo(Matrix W, ImageList* srt);
void MakeSymmetric(Matrix X);
FTYPE GetKthNeighboursDistance(Matrix Dist, int item, int K);
Matrix ComputeDistanceMatrix(Matrix data, Matrix values);



