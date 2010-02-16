
#ifndef DISTANCES_INCLUDED
#define DISTANCES_INCLUDED

/*  CSU INCLUDES */
#include <csuCommon.h>
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


/* ===========================================================================
New distance measure functions. These functions take a single matrix of
column vectors, each column representing a different image after projection
into subspace. They also take the column indices of the two images to be
compared.

All of these functions return "distances" in the sense that smaller is better.
So, for example, covariance and correlation are flipped and shifted so that
zero is the best score between two images.

The last measures also take the variance estimates for the subspace dimension,
in other words the eigenvalues from the subspace decomposition. These are used
to normalize measures by variance along the dimensions.
*/

/*  The L1 norm and L2 norms are standard. This code actually does the square root
for L2, not strictly speaking necessary for a nearest neighbor classifier.
*/
extern "C" double distanceCityBlock(const Matrix ims, int i, int j) ;

extern "C" double distanceEuclidean(const Matrix ims, int i, int j);

/* This is the standard covariance definition, it is the cosine
of the angle between the two vectors. When the vectors are
identical, it is 1.0, when they are orthogonal, it is zero.

However, in order produce a distance, this function returns
one minus the covariance.
*/

extern "C" double distanceCovariance(const Matrix ims, int i, int j) ;

/**
This is a standard correlation measure between two vectors with
one major caveat.  Because all subsequent processing treats these
measures as distance, the correlation mapped into the range 0 to 2
as follows:
correlation  1  goes to 0
correlation -1  goes to 2.
*/
extern "C" double distanceCorrelation(const Matrix ims, int i, int j);

/* This is what gets called Mahalanobis distance in the FERET evaluations. It
is somewhat ill named. It is closer to a covariance without normalization
by length and with a correction applied to each dimension. The correction
is indeed the scale factor required to tranform into the space of unit
variance in all dimensions. Hence, lacking a better shorthand term, this
is called Mahalanobis Angle.
*/
extern "C" double distanceYamborAngle(Matrix ims, int i, int j, Matrix values);

/* This is what gets called Mahalanobis distance in the FERET evaluations. It
is somewhat ill named. It is closer to a covariance without normalization
by length and with a correction applied to each dimension. The correction
is indeed the scale factor required to tranform into the space of unit
variance in all dimensions. Hence, lacking a better shorthand term, this
is called Mahalanobis Angle.
*/
extern "C" double distanceMahCosine(Matrix ims, int i, int j, Matrix values);

/*    L2 norm distance in Mahalanobis space
*/
extern "C" double distanceMahL2(const Matrix ims, int i, int j, const Matrix values) ;

extern "C" double distanceYamborDistance(const Matrix ims, int i, int j, const Matrix values);

/*    L1 norm distance in Mahalanobis space
*/
extern "C" double distanceMahL1(const Matrix ims, int i, int j, const Matrix values);

/* The soft distance measure proposed by WenYi Zhao is essentially the L2 norm
with each axis weighted by the associated eigen value, i.e. variance, raised
to the the power 0.2.  This is not obvious, but considerable discussion
of this design appears in WenYi's Dissertation.
*/
extern "C" double distanceLDASoft(Matrix ims, int i, int j, Matrix values);

extern "C" Matrix computeDistances(const Matrix ims, const Matrix values, int numImages, int K, char* distName);
extern "C" void writeDistancesForImage(char* distDirectory, char* filename, const Matrix distances, int col, char** names);


extern "C" double GetKthItemInRowAfterSorting(Matrix m, int r, int K);
#endif
