#include <csuCommon.h>

void independentTrain (Matrix pcaBasis, Matrix impca, Matrix * independentBasis, int arch, int blockSize, double learningRate, int iterations);
void learnWeightMatrix(Matrix W, Matrix X, Matrix I, double L, int B, int iterations);
void sep96(Matrix W, Matrix XOrg, Matrix I, double L, int B);
Matrix getYMatrix(Matrix U);
void verifyIndependentBasis(Matrix basis, Matrix imspca);
