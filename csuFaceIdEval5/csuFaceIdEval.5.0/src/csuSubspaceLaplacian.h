#include "csuCommon.h"
#include <math.h>

/**
 * A data structure for associating a pair of integers.
 *
 * This structure is used internally
 */
typedef struct {
  double key;
  long index;
} dbPair;

void laplacianTrain (Subspace*s, Matrix imspca, Matrix * laplacianBasis, Matrix * laplacianValues, int writeTextInterm);

Matrix findLMatrix(Matrix W, Matrix D);
Matrix findDMatrix(Matrix Dist);
Matrix findWMatrix(Matrix Dist, int neighbourCount, double t);
void MakeSymmetric(Matrix X);
Matrix ComputeConnectivityGraph(Matrix Dist, int neighbourCount, double t);
int* GetKNeighbours(Matrix Dist, int item, int K);
int* GetEpsNeighbours(Matrix Dist, int item, double Eps);
Matrix ComputeDistanceMatrix(Matrix data);
FTYPE distEuclidean(const Matrix ims, int i, int j);
