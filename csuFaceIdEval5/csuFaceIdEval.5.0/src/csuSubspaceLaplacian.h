#include "csuCommon.h"
#include <math.h>

/**
 * A data structure for associating a pair of integers.
 *
 * This structure is used internally
 */
typedef struct {
  FTYPE key;
  long index;
} dbPair;

//void laplacianTrain (Subspace*s, Matrix imspca, Matrix * laplacianBasis, Matrix * laplacianValues, int writeTextInterm);
void laplacianTrain (Matrix imspca,  ImageList *srt, Matrix * laplacianBasis, Matrix * laplacianValues, int K, int writeTextInterm);
Matrix findLMatrix(Matrix W, Matrix D);
Matrix findDMatrix(Matrix Dist);
Matrix findWMatrix(Matrix Dist, int K);
void applyClassInfo(Matrix W, ImageList* srt);
void MakeSymmetric(Matrix X);
FTYPE GetKthNeighboursDistance(Matrix Dist, int item, int K);
Matrix ComputeDistanceMatrix(Matrix data);
FTYPE distEuclidean(const Matrix ims, int i, int j);
