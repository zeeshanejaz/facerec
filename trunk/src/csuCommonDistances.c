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

/*  CSU INCLUDES */
#include <csuCommon.h>

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
FTYPE distanceCityBlock(const Matrix ims, int i, int j) {
    int k;
    FTYPE sum = 0.0;
    for (k = 0; k < ims->row_dim; k++) {
        sum += ABS((ME(ims, k, i) - ME(ims, k, j)));
    }
    return sum;
}

FTYPE distanceEuclidean(const Matrix ims, int i, int j) {
    int k;
    FTYPE sum = 0.0;
    for (k = 0; k < ims->row_dim; k++) {
        sum += (ME(ims, k, i) - ME(ims, k, j)) * (ME(ims, k, i) - ME(ims, k, j));
    }
    return sqrt(sum) ;
}


/* This is the standard covariance definition, it is the cosine
 of the angle between the two vectors. When the vectors are
 identical, it is 1.0, when they are orthogonal, it is zero.

 However, in order produce a distance, this function returns
 one minus the covariance.
*/

FTYPE distanceCovariance(const Matrix ims, int i, int j) {
    int k;
    FTYPE magi = 0.0, magj = 0.0, dot = 0.0;

    for (k = 0; k < ims->row_dim; k++) {
        magi += ME(ims, k, i) * ME(ims, k, i);
        magj += ME(ims, k, j) * ME(ims, k, j);
        dot += ME(ims, k, i) * ME(ims, k, j);
    }
    return (1.0 - (dot / (sqrt(magi) * sqrt(magj))));
}

/**
    This is a standard correlation measure between two vectors with
 one major caveat.  Because all subsequent processing treats these
 measures as distance, the correlation mapped into the range 0 to 2
 as follows:
  correlation  1  goes to 0
  correlation -1  goes to 2.
*/

FTYPE distanceCorrelation(const Matrix ims, int i, int j) {
    int k;
    FTYPE n;
    FTYPE mui = 0.0, muj = 0.0, stdi = 0.0, stdj = 0.0, dot = 0.0, corr;

    n = (FTYPE) ims->row_dim;
    for (k = 0; k < n; k++) {
        mui += ME(ims, k, i);
        muj += ME(ims, k, j);
    }
    mui = mui / n;
    muj = muj / n;

    for (k = 0; k < n; k++) {
        stdi += (ME(ims, k, i) - mui) * (ME(ims, k, i) - mui);
        stdj += (ME(ims, k, j) - muj) * (ME(ims, k, j) - muj);
        dot += (ME(ims, k, i) - mui) * (ME(ims, k, j) - muj);
    }
    stdi = sqrt((1.0 / (n - 1.0)) * stdi);
    stdj = sqrt((1.0 / (n - 1.0)) * stdj);

    corr = ( 1.0 / (n - 1.0)) * (dot / (stdi * stdj));

    return ( 1.0 - corr);
}

/* This is what gets called Mahalanobis distance in the FERET evaluations. It
 is somewhat ill named. It is closer to a covariance without normalization
 by length and with a correction applied to each dimension. The correction
 is indeed the scale factor required to tranform into the space of unit
 variance in all dimensions. Hence, lacking a better shorthand term, this
 is called Mahalanobis Angle.
*/
FTYPE distanceYamborAngle(Matrix ims, int i, int j, Matrix values) {
    int k;
    FTYPE sum = 0.0;
    FTYPE sum_i = 0.0;
    FTYPE sum_j = 0.0;
    for (k = 0;k < ims->row_dim;k++) {
        sum_i += ME(ims, k, i) * ME(ims, k, i);
        sum_j += ME(ims, k, j) * ME(ims, k, j);
    }
    for (k = 0; k < ims->row_dim; k++) {
        sum -= ME(ims, k, i) / sqrt(sum_i) * (ME(ims, k, j) / sqrt(sum_j)) * (1 / (sqrt(ME(values, k, 0))));
    }
    return sum;
}

/* This is what gets called Mahalanobis distance in the FERET evaluations. It
 is somewhat ill named. It is closer to a covariance without normalization
 by length and with a correction applied to each dimension. The correction
 is indeed the scale factor required to tranform into the space of unit
 variance in all dimensions. Hence, lacking a better shorthand term, this
 is called Mahalanobis Angle.
*/
FTYPE distanceMahCosine(Matrix ims, int i, int j, Matrix values) {
    static int first_call = 1;
    static Matrix optVals = NULL;
    int k;
    Matrix vi = makeMatrix(ims->row_dim,1);
    Matrix vj = makeMatrix(ims->row_dim,1);
    FTYPE sum_i = 0.0;
    FTYPE sum_j = 0.0;
    FTYPE scale_i = 0.0;
    FTYPE scale_j = 0.0;
    FTYPE sum = 0.0;

    if(first_call){
        first_call = 0;
        optVals = duplicateMatrix(values);
        for(k = 0; k < optVals->row_dim; k++){
            ME(optVals,k,0) = 1.0/sqrt(ME(values, k, 0));
        }
	//saveMatrixAscii("OP.mat", "OP", (optVals), matlabFormat);
    }

    /* Transform the vector into Mahalanobis space */
    for(k = 0; k < vi->row_dim; k++){
        ME(vi,k,0) = ME(ims,k,i)*ME(optVals,k,0);
        sum_i += ME(vi,k,0)*ME(vi,k,0);
        ME(vj,k,0) = ME(ims,k,j)*ME(optVals,k,0);
        sum_j += ME(vj,k,0)*ME(vj,k,0);
    }

    scale_i = 1.0/sqrt(sum_i);
    scale_j = 1.0/sqrt(sum_j);

    /* Unit length the vectors and find the dot product (cos of angle) */
    for(k = 0; k < vi->row_dim; k++){
        sum += ME(vi,k,0)*ME(vj,k,0);
    }
    sum = sum * scale_i * scale_j;

    /* Free the temporary vectors. */
    freeMatrix(vi);
    freeMatrix(vj);

    /* return the negetive similarity or distance */
    return (1.0 - sum);
}
/*    L2 norm distance in Mahalanobis space
 */
FTYPE distanceMahL2(const Matrix ims, int i, int j, const Matrix values) {
    int k;
    FTYPE sum = 0.0;
    static int first_call = 1;
    static Matrix optVals = NULL;

    if(first_call){
        first_call = 0;
        optVals = duplicateMatrix(values);
        for(k = 0; k < optVals->row_dim; k++){
            ME(optVals,k,0) = 1.0/ME(values, k, 0);
        }
    }

    for (k = 0; k < ims->row_dim; k++) {
        sum += (ME(ims, k, i) - ME(ims, k, j)) * (ME(ims, k, i) - ME(ims, k, j)) * ME(optVals,k,0);
    }

    return sqrt(sum);
}


FTYPE distanceYamborDistance(const Matrix ims, int i, int j, const Matrix values) {
    int k;
    FTYPE sum = 0.0;
    static int first_call = 1;
    static Matrix optVals = NULL;

    if(first_call){
        first_call = 0;
        optVals = duplicateMatrix(values);
        for(k = 0; k < optVals->row_dim; k++){
            ME(optVals,k,0) = 1.0/ME(values, k, 0);
        }
    }

    for (k = 0; k < ims->row_dim; k++) {
        sum += ME(ims, k, i)*ME(ims, k, j)*ME(optVals,k,0);
    }

    return -sum;
}

/*    L1 norm distance in Mahalanobis space
 */
FTYPE distanceMahL1(const Matrix ims, int i, int j, const Matrix values) {
    int k;
    FTYPE sum = 0.0;
    static int first_call = 1;
    static Matrix optVals = NULL;

    if(first_call){
        first_call = 0;
        optVals = duplicateMatrix(values);
        for(k = 0; k < optVals->row_dim; k++){
            ME(optVals,k,0) = 1.0/sqrt(ME(values, k, 0));
        }
    }

    for (k = 0; k < ims->row_dim; k++) {
        sum += ABS(ME(ims, k, i) - ME(ims, k, j)) * ME(optVals,k,0);
    }
    return sum;
}

/*START		Changed by Zeeshan: For LPP & ICA*/
int elementComparator (const void *a, const void *b)
{
  	const dbPair *p1 = (dbPair *)a;
  	const dbPair *p2 = (dbPair *)b;

  	if (p1->key < p2->key)
    		return -1;

  	if (p1->key > p2->key)
    		return 1;
  	
	return 0;
}

FTYPE GetKthItemInRowAfterSorting(Matrix m, int r, int K)
{
  	int i;
	FTYPE kthItem;
  	dbPair *toSort = (dbPair*) malloc (m->col_dim * sizeof (dbPair));

  	for(i = 0; i < m->col_dim; i++)
    	{
      		toSort[i].key   = ME(m, r, i);
      		toSort[i].index = i;
    	}

  	qsort (toSort, m->col_dim, sizeof (dbPair), elementComparator);
  	
  	kthItem = ME(m, r, toSort[K].index);
	
  	free (toSort);
	
  	return kthItem;
}

FTYPE GetSumOfKItemInRowAfterSorting(Matrix m, int r, int K)
{
  	int i;
	FTYPE sum = 0.0;
  	dbPair *toSort = (dbPair*) malloc (m->col_dim * sizeof (dbPair));

  	for(i = 0; i < m->col_dim; i++)
    	{
      		toSort[i].key   = ME(m, r, i);
      		toSort[i].index = i;
    	}

  	qsort (toSort, m->col_dim, sizeof (dbPair), elementComparator);
  	
    	for(i = 0; i <= K; i++)
    	{
  		sum += ME(m, r, toSort[i].index);
	}  	

  	free (toSort);
	
  	return sum;
}

FTYPE distanceNormalizedCosine(Matrix ims, int i, int j, Matrix values) 
{
    static int first_call = 1;
    static Matrix nIms = NULL;

    FTYPE sum = 0.0;
    int k, l;

    if(first_call)
    {
		first_call = 0;
		
		nIms = makeMatrix(ims->row_dim, ims->col_dim);
			
		//normalize all vectors to have magnitude 1
		for (k = 0; k < ims->col_dim; k++)
		{
			sum = 0.0; // will store magnitude
			for (l = 0; l < ims->row_dim; l++)
			{
       			sum += (ME(ims, l, k) * ME(ims, l, k));
			}	
		
			sum = sqrt(sum);

			for (l = 0; l < ims->row_dim; l++)
			{		
       			ME(nIms, l, k) = ME(ims, l, k) / sum; // devide by magnitude	
			}		
		}
	}

    sum = 0.0;
    for (k = 0; k < ims->row_dim; k++)
    {
       	sum += (ME(nIms, k, i) * ME(nIms, k, j) / ME(values, k, 0));
	}

    return (1.0 - sum);
}

FTYPE distanceUZI(Matrix ims, int i, int j, int k) {
    static int P = -1;
    static Matrix t = NULL;
    Matrix optVals = NULL;    

    int x, y, z;
    FTYPE sum = 0.0;
    
    if(i != P)
    {
	//printf("Computing T Matrix for image %d. \n", i);
	P = i;
	optVals = makeMatrix(ims->row_dim, ims->col_dim);
	
	for( x = 0; x < ims->row_dim; x++ )
	    for( y = 0; y < ims->col_dim; y++ )
	    {
		ME(optVals, x, y) = (ME(ims, x, y) - ME(ims, x, i)) * (ME(ims, x, y) - ME(ims, x, i)); 
	    }
	
	if(t == NULL)
	    t = makeMatrix(ims->row_dim, ims->col_dim);

	for( y = 0; y < t->row_dim; y++ )
	{
	    ME(t, y, i) = GetSumOfKItemInRowAfterSorting(optVals, y, k) / k;//K is hardcoded here
	    ME(t, y, i) = abs(ME(t, y, i)) + 1; 
	}

	freeMatrix(optVals);
    }

    for (z = 0; z < ims->row_dim; z++) 
    {
          sum += (ME(ims, z, i) - ME(ims, z, j)) * (ME(ims, z, i) - ME(ims, z, j)) / (ME(t, z, i) * ME(t, z, i) * ims->col_dim);
    }

    return sum;
}
/*END		Changed by Zeeshan: For LPP & ICA*/

FTYPE distanceLDASoft(Matrix ims, int i, int j, Matrix values) {
    int k;
    FTYPE sum = 0.0;
    for (k = 0; k < ims->row_dim; k++) {
        sum += pow(ME(values, k, 0), 0.2) * (ME(ims, k, i) - ME(ims, k, j)) * (ME(ims, k, i) - ME(ims, k, j));
    }
    return sum;
}

FTYPE distanceLDASoftX(Matrix ims, int i, int j, Matrix values, FTYPE X) {
    int k;
    FTYPE sum = 0.0;
    for (k = 0; k < ims->row_dim; k++) {
        sum += pow(ME(values, k, 0), X) * (ME(ims, k, i) - ME(ims, k, j)) * (ME(ims, k, i) - ME(ims, k, j));
    }
    return sum;
}


Matrix computeDistances(const Matrix ims, const Matrix values, int numImages, int K, char* distName) {
    int i, j;
    Matrix distances = makeMatrix(numImages, numImages);

    if (strcmp(distName, "Euclidean") == 0) {
        for (i = 0; i < numImages; i++) {
            writeProgress("Computing distances for Euclidean", i,numImages);
            for (j = 0; j < numImages; j++) {
                ME(distances, i, j) = distanceEuclidean(ims, i, j);
            }
        }
    } else if ((strcmp(distName, "Cityblock") == 0) || (strcmp(distName, "CityBlock") == 0)) {
        for (i = 0; i < numImages; i++) {
            writeProgress("Computing distances for Cityblock", i,numImages);
            for (j = 0; j < numImages; j++) {
                ME(distances, i, j) = distanceCityBlock(ims, i, j);
            }
        }
    } else if (strcmp(distName, "Covariance") == 0) {
        for (i = 0; i < numImages; i++) {
            writeProgress("Computing distances for Covariance", i,numImages);
            for (j = 0; j < numImages; j++) {
                ME(distances, i, j) = distanceCovariance(ims, i, j);
            }
        }//Added for LPP and ICA
    } else if ((strcmp(distName, "uzi") == 0) || (strcmp(distName, "UZI") == 0)) {
        for (i = 0; i < numImages; i++) {
            writeProgress("Computing distances for UZI", i,numImages);
            for (j = 0; j < numImages; j++) {
                ME(distances, i, j) = distanceUZI(ims, i, j, K);
            }
        }
    } else if (strcmp(distName, "Correlation") == 0) {
        for (i = 0; i < numImages; i++) {
            writeProgress("Computing distances for Correlation", i,numImages);
            for (j = 0; j < numImages; j++) {
                ME(distances, i, j) = distanceCorrelation(ims, i, j);
            }
        }
    } else if (strcmp(distName, "YamborAngle") == 0) {
        for (i = 0; i < numImages; i++) {
            writeProgress("Computing distances for YamborAngle", i,numImages);
            for (j = 0; j < numImages; j++) {
                ME(distances, i, j) = distanceYamborAngle(ims, i, j, values);
            }
        }
    } else if (strcmp(distName, "MahCosine") == 0) {
        for (i = 0; i < numImages; i++) {
            writeProgress("Computing distances for MahCosine", i,numImages);
            for (j = 0; j < numImages; j++) {
                ME(distances, i, j) = distanceMahCosine(ims, i, j, values);
            }
        }//Added for LPP and ICA
    } else if (strcmp(distName, "NormCosine") == 0) {
        for (i = 0; i < numImages; i++) {
            writeProgress("Computing distances for NormCosine", i,numImages);
            for (j = 0; j < numImages; j++) {
                ME(distances, i, j) = distanceNormalizedCosine(ims, i, j, values);
            }
        }  
    } else if (strcmp(distName, "MahL1") == 0) {
        for (i = 0; i < numImages; i++) {
            writeProgress("Computing distances for MahL1", i,numImages);
            for (j = 0; j < numImages; j++) {
                ME(distances, i, j) = distanceMahL1(ims, i, j, values);
            }
        }
    } else if (strcmp(distName, "MahL2") == 0) {
        for (i = 0; i < numImages; i++) {
            writeProgress("Computing distances for MahL2", i,numImages);
            for (j = 0; j < numImages; j++) {
                ME(distances, i, j) = distanceMahL2(ims, i, j, values);
            }
        }
    } else if (strcmp(distName, "YamborDistance") == 0) {
        for (i = 0; i < numImages; i++) {
            writeProgress("Computing distances for Windy", i,numImages);
            for (j = 0; j < numImages; j++) {
                ME(distances, i, j) = distanceYamborDistance(ims, i, j, values);
            }
        }
    } else if ((strcmp(distName, "ldaSoft") == 0) || (strcmp(distName, "LDASoft") == 0)) {
        for (i = 0; i < numImages; i++) {
            writeProgress("Computing distances for LDASoft", i,numImages);
            for (j = 0; j < numImages; j++) {
                ME(distances, i, j) = distanceLDASoft(ims, i, j, values);
            }
        }
    } else {
        fprintf(stderr, "Error: The distance measure %s is not implemented! \n", distName);
        exit(1);
    }


    return distances;
}

void writeDistancesForImage(char* distDirectory, char* filename, const Matrix distances, int col, char** names) {
    char name[MAX_FILENAME_LENGTH];
    FILE *file;
    int i;

    //remove directory structure and get file name only
    //cleanFileName(filename);
    sprintf(name, "%s/%s", distDirectory, filename);
    file = fopen(name, "w");
    if (!file) {
        printf("Error: could not open file <%s>\n", name);
        exit(1);
    }
    for (i = 0; i < distances->row_dim; i++) {
        fprintf(file, "%s\t%.16e\n", names[i], ME(distances, i, col));
    }
    fclose(file);
}
