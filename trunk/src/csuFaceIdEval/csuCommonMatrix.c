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
Added a new function to subtract a scalar value from a matrix
*/

#include <csuCommon.h>
#include <stdio.h>

int alloc_matrix = 0;

Matrix makeMatrix(int row_dim, int col_dim) {
    /** creates and allocates memory for a matrix */
    int i;

    /* Allocate a matrix header */
    Matrix A = (Matrix)malloc(sizeof(matrix));
    assert(A);

    /* keep track of the number of matricies allcoated */
    alloc_matrix++;

    DEBUG(10, "Allocating Matrix");

    /** Set the matrix dimensions */
    A->row_dim = row_dim;
    A->col_dim = col_dim;

    /** Allocate the data section of the matrix */
    A->data = (double*)malloc(sizeof(double) * row_dim * col_dim);
    if (!A->data) {
        fprintf(stderr, "Matrix Size: %d X %d\n", A->row_dim, A->col_dim);
        fflush(stderr);
    }
    assert(A->data);
    A->cols = (double**)malloc(sizeof(double*)*col_dim);
    for(i = 0; i < col_dim; i++){
        A->cols[i] = A->data + i*row_dim;
    }

    return A;
}

Matrix makeZeroMatrix(int row_dim, int col_dim) {
    /** creates and allocates memory for a matrix */
    int i, j;
    Matrix A = makeMatrix(row_dim, col_dim);


    for (i = 0; i < row_dim; i++) {
        for (j = 0; j < col_dim; j++) {
            ME(A, i, j) = 0;
        }
    }
    return A;
}


void initializeMatrix(Matrix X, int value)
{
    int i, j;
    for(i = 0; i < X->row_dim; i++)
        for(j = 0; j < X->row_dim; j++)
            ME(X, i, j) = value;
}

void freeMatrix(Matrix A) {
    /** frees the area allocated to a matrix */

    /* keep track of the number of matricies allcoated */
    alloc_matrix--;
    DEBUG(10, "Freeing Matrix");

    free(A->data);
    free(A->cols);
    free(A);
}

Matrix makeIdentityMatrix(int dim) {
    /** creates and allocates memory for an identity matrix */
    Matrix I;
    int i, j;

    DEBUG(10, "Creating identity matrix");

    I = makeMatrix(dim, dim);
    for (i = 0; i < dim; i++) {
        for (j = 0; j < dim; j++) {
            ME(I, i, j) = (i == j);
        }
    }

    return I;
}

Matrix duplicateMatrix(const Matrix mat) {
    Matrix dup = makeMatrix(mat->row_dim, mat->col_dim);
    int i, j;
    for (i = 0; i < mat->row_dim; i++) {
        for (j = 0; j < mat->col_dim; j++) {
            ME(dup, i, j) = ME(mat, i, j);
        }
    }

    return dup;
}

Matrix multiplyMatrix(const Matrix A, const Matrix B) {
    /** creates a new matrix that is the product of A and B */

    int i, j, k;                                   /* Variables used as indices */
    Matrix P = makeMatrix(A->row_dim, B->col_dim); /* Product A * B */

    DEBUG(10, "Multiplying Matrix");

    assert(A->col_dim == B->row_dim); /* Verify that the Matrices can be multiplied */

    /** Build the product matrix P */
    for ( j = 0; j < B->col_dim; j++) {
        for ( i = 0; i < A->row_dim; i++) {
            ME( P, i, j) = 0;
            for (k = 0; k < A->col_dim; k++) {
                ME( P, i, j) += ME(A, i, k) * ME(B, k, j);
            }
        }
    }

    return P;
}

Matrix transposeMultiplyMatrixL(const Matrix A, const Matrix B) {
    /** creates a new matrix that is the product of (A transpose) and B */

    int i, j, k;                                   /* Variables used as indices */
    Matrix P = makeMatrix(A->col_dim, B->col_dim); /* Product A * B */

    DEBUG(10, "Multiplying Matrix");

    assert(A->row_dim == B->row_dim); /* Verify that the Matrices can be multiplied */

    /** Build the product matrix P */

    /** Optimized code */
    for ( j = 0; j < B->col_dim; j++) {
        for ( i = 0; i < A->col_dim; i++) {
            ME( P, i, j) = 0;
        }
    }
    for ( j = 0; j < B->col_dim; j++) {
        for ( i = 0; i < A->col_dim; i++) {
            for (k = 0; k < A->row_dim; k++) {
                ME( P, i, j) += ME(A, k, i) * ME(B, k, j);
            }
        }
    }

    return P;
}

Matrix transposeMultiplyMatrixR(const Matrix A, const Matrix B) {
    /** creates a new matrix that is the product of the A and (B transpose) */

    int i, j, k;                                   /* Variables used as indices */
    Matrix P = makeMatrix(A->row_dim, B->row_dim); /* Product A * B */

    DEBUG(10, "Multiplying Matrix");

    assert(A->row_dim == B->row_dim); /* Verify that the Matrices can be multiplied */

    /* Optimized code */
    for ( i = 0; i < A->row_dim; i++) {
        for ( j = 0; j < B->row_dim; j++) {
            ME( P, i, j) = 0;
        }
    }

    for (k = 0; k < A->col_dim; k++) {
        for ( i = 0; i < A->row_dim; i++) {
            for ( j = 0; j < B->row_dim; j++) {
                ME( P, i, j) += ME(A, i, k) * ME(B, j, k);
            }
        }
    }

    return P;
}


Matrix transposeMatrix(const Matrix A) {
    /* creates a new matrix that is the transpose of A */
    int i, j;                                      /* Variables used as indices */
    Matrix T = makeMatrix(A->col_dim, A->row_dim); /* Transpose of A */

    DEBUG(10, "Transposing Matrix");

    /** Transpose data */
    for ( i = 0; i < A->row_dim; i++) {
        for ( j = 0; j < A->col_dim; j++) {
            ME(T, j, i) = ME(A, i, j);
        }
    }

    return T;
}

void rowSwap(Matrix m, int rSrc, int rDest){
    int col = 0;
    double tmp;

    for(col = 0; col < m->col_dim; col++){
        tmp = ME(m,rSrc,col);
        ME(m,rSrc,col) = ME(m,rDest,col);
        ME(m,rDest,col) = tmp;
    }
}

void rowMult(Matrix m, int rSrc, double value){
    int col = 0;

    for(col = 0; col < m->col_dim; col++){
        ME(m,rSrc,col) *= value;
    }
}

void rowMultAdd(Matrix m, int rSrc, int rDest, double value){
    int col = 0;

    for(col = 0; col < m->col_dim; col++){
        ME(m,rDest,col) += value*ME(m,rSrc,col);
    }
}

void matrixRREF(Matrix m){
    int prealloc = alloc_matrix;
    int pivotCol = 0;
    int pivotRow = 0;
    int row;
    double absVal;
    int tmp = 0;

    while(1){
        /* Select the row with the largest absolute value, or move to the next row
        if there is no value int the column */
        absVal = 0.0;
        while( absVal == 0.0 && pivotCol < m->col_dim){
            absVal = ABS(ME(m,pivotRow,pivotCol));
            tmp = pivotRow;

            for(row = pivotRow+1; row < m->row_dim; row++){
                if( ABS(ME(m,row,pivotCol)) > absVal ){
                    absVal = ABS(ME(m,row,pivotCol));
                    tmp = row;
                }
            }
            if(absVal == 0){
                pivotCol++;
            }
        }

        /* return if the RREF has been found */
        if( pivotCol >= m->col_dim || pivotRow >= m->row_dim) return; 

        /* make sure that the pivot row is in the correct position */
        if(pivotRow != tmp) rowSwap(m,tmp,pivotRow);

        /* rescale the Pivot Row */
        rowMult( m, pivotRow,1.0/ME(m,pivotRow,pivotCol) );

        /* This part of the algorithm is not as effecent as it could be,
        but it works for now. */
        for(row = 0; row < m->row_dim; row++){
            if(row != pivotRow) {
                rowMultAdd(m,pivotRow,row,-ME(m,row,pivotCol));
            }
        }
        pivotRow++;
        pivotCol++;
    }

    if(prealloc != alloc_matrix){
        printf("Error deallocating matricies <%s>: pre=%d post=%d",__FUNCTION__, prealloc, alloc_matrix);
        exit(1);
    }
}

/* invert a matrix using Gaussian Elimination */
Matrix invertRREF(Matrix m){
    int prealloc = alloc_matrix;
    int i,j;
    Matrix tmp = makeZeroMatrix(m->row_dim, m->col_dim*2);
    Matrix inverse = makeMatrix(m->row_dim, m->col_dim);
    DEBUG_CHECK(m->row_dim == m->col_dim,"Matrix can only be inverted if it is square");
    /* build the tmp Matrix which will be passed to RREF */
    for( i = 0; i < m->row_dim; i++){
        for( j = 0; j < m->col_dim; j++){
            ME(tmp,i,j) = ME(m,i,j); 
            if(i == j){ 
                ME(tmp,i,j+m->col_dim) = 1;
            }
        }
    }
    matrixRREF(tmp);

    for( i = 0; i < m->row_dim; i++){
        for( j = 0; j < m->col_dim; j++){
            ME(inverse,i,j) = ME(tmp,i,j+m->col_dim);
        }
    }

    freeMatrix(tmp);

    if(prealloc != alloc_matrix - 1){
        printf("Error deallocating matricies <%s>: pre=%d post=%d",__FUNCTION__, prealloc, alloc_matrix);
        exit(1);
    }
    return inverse;
}

Matrix leastSquares(Matrix A, Matrix b){
    int prealloc = alloc_matrix;
    Matrix At = transposeMatrix(A);
    Matrix AtA = transposeMultiplyMatrixL(A,A);
    Matrix AtAi = invertRREF(AtA);
    Matrix Atb = multiplyMatrix(At,b);
    Matrix a = multiplyMatrix(AtAi,Atb);

    freeMatrix(At);
    freeMatrix(AtA);
    freeMatrix(AtAi);
    freeMatrix(Atb);

    if(prealloc != alloc_matrix - 1){
        printf("Error deallocating matricies <%s>: pre=%d post=%d",__FUNCTION__, prealloc, alloc_matrix);
        exit(1);
    }
    return a;
}

Matrix weightedLeastSquares(Matrix A, Matrix b, Matrix W){
    int prealloc = alloc_matrix;

    Matrix WA = multiplyMatrix(W,A);
    Matrix WAt = transposeMatrix(WA);
    Matrix WAtW = multiplyMatrix(WAt,W);
    Matrix WAtWA = multiplyMatrix(WAtW,A);
    Matrix WAtWAi = invertRREF(WAtWA);
    Matrix WAtWAiWAt = multiplyMatrix(WAtWAi,WAt);
    Matrix WAtWAiWAtW = multiplyMatrix(WAtWAiWAt,W);
    Matrix a = multiplyMatrix(WAtWAiWAtW,b);

    freeMatrix(WA);
    freeMatrix(WAt);
    freeMatrix(WAtW);
    freeMatrix(WAtWA);
    freeMatrix(WAtWAi);
    freeMatrix(WAtWAiWAt);
    freeMatrix(WAtWAiWAtW);

    if(prealloc != alloc_matrix - 1){
        printf("Error deallocating matricies <%s>: pre=%d post=%d",__FUNCTION__, prealloc, alloc_matrix);
        exit(1);
    }
    return a;
}
/*START:	Changed By Zeeshan For LPP*/

/*Function for subtracting a scalar value from a matrix*/
void subtractScalarFromMatrix (Matrix A, const int n) {
    /** output A - n */

    int i, j;
    DEBUG(10, "Subtracting Scalar from  Matrix");

    for (i = 0; i < A->row_dim; i++) {
        for (j = 0; j < A->col_dim; j++) {
            ME(A, i, j) = ME(A, i, j) - n;
        }
    }
}

/*END:		Changed By Zeeshan For LPP*/

Matrix subtractMatrix(const Matrix A, const Matrix B) {
    /** output A - B */
    Matrix diff = makeMatrix(A->row_dim, A->col_dim);
    int i, j;
    DEBUG(10, "Subtracting Matrix");

    assert(A->row_dim == B->row_dim);
    assert(A->col_dim == B->col_dim);

    for (i = 0; i < A->row_dim; i++) {
        for (j = 0; j < A->col_dim; j++) {
            ME(diff, i, j) = ME(A, i, j) - ME(B, i, j);
        }
    }

    return diff;
}


Matrix addMatrix( const Matrix A, const Matrix B) {
    /** output A + B */
    Matrix sum = makeMatrix(A->row_dim, A->col_dim);
    int i, j;
    DEBUG(10, "Adding Matrix");

    assert(A->row_dim == B->row_dim);
    assert(A->col_dim == B->col_dim);

    for (i = 0; i < A->row_dim; i++) {
        for (j = 0; j < A->col_dim; j++) {
            ME(sum, i, j) = ME(A, i, j) + ME(B, i, j);
        }
    }

    return sum;
}

void addMatrixEquals(Matrix A, const Matrix B) {
    /** output A += B */
    int i, j;
    DEBUG(10, "Adding Matrix");

    assert(A->row_dim == B->row_dim);
    assert(A->col_dim == B->col_dim);

    for (i = 0; i < A->row_dim; i++) {
        for (j = 0; j < A->col_dim; j++) {
            ME(A, i, j) += ME(B, i, j);
        }
    }
}

Matrix matrixCols( const Matrix mat, int col1, int col2) {
    Matrix cols = makeMatrix(mat->row_dim, col2 - col1 + 1);
    int i, j;

    DEBUG_CHECK(col1 <= col2 && col2 < mat->col_dim, "Poorly chosen columns for extract columns operation");

    for (i = col1; i <= col2; i++) {
        for (j = 0; j < mat->row_dim; j++) {
            ME(cols, j, i - col1) = ME(mat, j, i);
        }
    }

    return cols;
}

Matrix matrixRows( const Matrix mat, int row1, int row2) {
    Matrix rows = makeMatrix(row2 - row1 + 1, mat->col_dim);
    int i, j;

    DEBUG_CHECK(row1 <= row2 && row2 < mat->row_dim, "Poorly chosen rows for extract rows operation");

    for (i = 0; i < mat->col_dim; i++) {
        for (j = row1; j <= row2; j++) {
            ME(rows, j - row1, i) = ME(mat, j, i);
        }
    }

    return rows;
}

Matrix valueMatrix(int row_dim, int col_dim, double value)
{
    int i, j;
    Matrix m = makeMatrix(row_dim, col_dim);

    for (i = 0; i < row_dim; i++) 
    {
        for (j = 0; j < col_dim; j++) 
        {
            ME(m, i, j) = value;
        }
    }

    return m;	
}

Matrix rangeCheck(Matrix mat, int i, int j, const char* file_name,
                  const char* func_name, int line_num, const char* mat_name) {
                      if (i >= 0 && i < mat->row_dim && j >= 0 && j < mat->col_dim)
                          return mat;

                      printf("*********************  ERROR  *********************\n");
                      printf("Error: Matrix bounds out of range     \n");
                      printf("  in function: %s  Matrix: %s     \n", func_name, mat_name);
                      printf("        Index: (%d,%d)  Size: (%dX%d) \n", i, j, mat->row_dim, mat->col_dim);
                      printf("                     file: %s         \n", file_name);
                      printf("                     line: %d         \n", line_num );
                      printf("***************************************************\n");

                      exit(1);
                      return mat;
}


/*
This is useful for debugging. It prints a matrix to standard out
*/

void printMatrix(const char* label, Matrix m) {
    int i, j;

    printf("%s = dimentions: %dX%d\n", label, m->row_dim, m->col_dim);
    for (i = 0; i < m->row_dim; i++) {
        for (j = 0; j < m->col_dim; j++) {
            printf("%6.12f ", ME(m, i, j));
        }
        printf("\n");
    }

} 


MatrixSaveMode asciiFormat = octaveFormat;

void saveMatrixAscii(const char *fname, const char* label, const Matrix m, MatrixSaveMode mode) {
    /* saves a matrix to an octave/matlab compatible ascii file */
    FILE* matfile;
    int i, j;
    char filename[1024];

    DEBUG_CHECK (!(((mode & formatMask) == matlabFormat) && (mode & appendToFile)),
        "Writing multiple matrices to an ascii file not supported by Matlab");

    if (fname == NULL) {
        if ((mode & formatMask) == octaveFormat) {
            sprintf(filename, "%s.mat", label[0] == '*' ? label + 1 : label );
        } else {
            sprintf(filename, "%s.txt", label[0] == '*' ? label + 1 : label );
        }
        fname = filename;
    }

    matfile = fopen(fname, (mode & appendToFile) ? "a" : "w");

    if (!matfile) {
        printf("Error could not open file: \"%s\" for writing.\n", fname);
        exit(1);
    }

    /*  The following lines are incompatible with matlab but
    neccessary for octave.  */

    if ((mode & formatMask) == octaveFormat) { /* save to octave format */
        fprintf(matfile, "# name: %s\n", label[0] == '*' ? label + 1 : label);
        fprintf(matfile, "# type: matrix\n");
        fprintf(matfile, "# rows: %d\n", m->row_dim);
        fprintf(matfile, "# columns: %d\n", m->col_dim);
    }


    for (i = 0; i < m->row_dim; i++) {
        for (j = 0; j < m->col_dim; j++) {
            fprintf(matfile, " %e", ME(m, i, j));
        }
        fprintf(matfile, "\n");
    }

    fclose(matfile);

}

/* Return a matrix of random elements (from 0.0 to 1.0)
*/
Matrix
makeRandomMatrix (int row_dim, int col_dim)
{
    int i,j;
    Matrix m = makeMatrix(row_dim,col_dim);
    for (i = 0; i < m->row_dim; i++) {
        for (j = 0; j < m->col_dim; j++) {
            ME(m,i,j) = RANDOM;
        }
    }
    return m;
}

/* Return the sum of the squares of the elements of a matrix.
* The square-root of this quantity is the L2 norm of the matrix.
*/
double
matrixSumOfSquares (Matrix m)
{
    double sumOfSquares = 0.0;
    int i, j;
    for (i = 0; i < m->row_dim; i++) {
        for (j = 0; j < m->col_dim; j++) {
            sumOfSquares += SQR(ME(m, i, j));
        }
    }
    return sumOfSquares;
}

/*START: Changed by Zeeshan: for LPP*/

/* multiplyMatrixScalar

This function multiplies each element of a given Matrix with
a given scalar value.
*/
void multiplyMatrixScalar(Matrix M, double scale)
{
    int i, j;
    for (i = 0; i < M->col_dim; i++) {
        for (j = 0; j < M->row_dim; j++) {
            ME(M, j, i) *= scale;
        }
    }	
}

/* ReshapeMatrix 

This function reshapes a given matrix into a matrix of given rows and cols.
*/
Matrix reshapeMatrix (Matrix M, int row, int col)
{
    int i, j, k, l;
    Matrix E;

    assert((M->row_dim * M->col_dim) == (row * col));

    printf("Reshaping Matrix.\n");

    E = makeMatrix(M->row_dim, M->col_dim);
    k = 0; l = 0;

    for( i = 0; i < M->row_dim ; i++ )
        for( j = 0 ; j < M->col_dim ; j++ )	
        {
            ME(E, i, j) = ME(M, k, l);

            l ++;
            if(l >= M->col_dim)
            {
                l = 0;
                k ++; 
            } 
        }

        printf("Finished Computing Reshaping Matrix.\n");

        return E;
}

/* ACOSMatrix 

This function computes acos of each element of a given
matrix.
*/
Matrix ACOSMatrix (Matrix M)
{
    int i, j;
    Matrix E;

    printf("Computing ACOS.\n");

    E = makeMatrix(M->row_dim, M->col_dim);

    for( i = 0 ; i < M->row_dim ; i++ )
        for( j = 0 ; j < M->col_dim ; j++ )	
        {
            ME(E, i, j) = acos(ME(M, i, j));	
        }

        printf("Finished Computing ACOS.\n");

        return E;
}

/* sqrtElem 

This function computes square root of each element of a given
matrix.
*/
Matrix sqrtElem (Matrix M)
{
    int i, j;
    Matrix E;

    printf("Computing SQRTE.\n");

    E = makeMatrix(M->row_dim, M->col_dim);

    for( i = 0 ; i < M->row_dim ; i++ )
        for( j = 0 ; j < M->col_dim ; j++ )	
        {
            ME(E, i, j) = sqrt(ME(M, i, j));	
        }

        printf("Finished Computing SQRTE.\n");

        return E;
}


/* sqrtMatrix 

This function computes square root of a given matrix such
that X * X' = M
matrix.
*/
Matrix sqrtMatrix (Matrix M)
{
    int i;
    Matrix E, D, Sq, Temp, Result;

    printf("Computing SQRTM.\n");

    E = makeMatrix(M->row_dim, M->col_dim);
    D = makeMatrix(M->row_dim, 1);

    cvJacobiEigens_64d(M->data, E->data, D->data, M->col_dim, 0.0, -99);

    Sq = makeMatrix(M->row_dim, M->col_dim);

    for( i = 0 ; i < Sq->row_dim ; i++)
    {
        if(ME(D, i, 0) < 0.0)
        {
            ME(D, i, 0) *= -1;
        }

        ME(Sq, i, i) = sqrt(ME(D, i, 0));	
    }

    Temp = transposeMultiplyMatrixR(Sq, E);

    Result = multiplyMatrix(E, Temp);

    freeMatrix(E);	
    freeMatrix(D);
    freeMatrix(Sq);
    freeMatrix(Temp);

    printf("Finished Computing SQRTM.\n");

    return Result;
}

Matrix orderColumns(Matrix M, int order[])
{
    int i,j;
    Matrix N = makeMatrix(M->row_dim, M->col_dim);

    for(i = 0; i < M->col_dim; i++)
        for(j = 0; j < M->row_dim; j++)
        {
            ME(N, j, i) = ME(M, j, order[i]);
        }

        return N;
}


Matrix orderRows(Matrix M, int order[])
{
    int i,j;
    Matrix N = makeMatrix(M->row_dim, M->col_dim);

    for(i = 0; i < M->row_dim; i++)
        for(j = 0; j < M->col_dim; j++)
        {
            ME(N, i, j) = ME(M, order[i], j);
        }

        return N;
}


/* each_mean_subtract_column

This function subtracts a mean of each image from all pixels
in that image
*/
void each_mean_subtract_column (Matrix ims, Matrix mean)
{
    int i, j;
    for (i = 0; i < ims->col_dim; i++) {
        for (j = 0; j < ims->row_dim; j++) {
            ME(ims, j, i) -= ME(mean, 0, i);
        }
    }
}

/* get_mean_each_image

This function returns the mean of all elements in a
column of each  column in the matrix.
*/
Matrix get_mean_each_column (Matrix ims)
{
    int i, j;
    Matrix mean = makeMatrix(1, ims->col_dim);

    for (i = 0; i < ims->col_dim; i++)
    {
        ME(mean, 0, i) = 0.0;

        for (j = 0; j < ims->row_dim; j++)
        {
            ME(mean, 0, i) = (ME(ims, j, i) + ME(mean, 0, i));
        }

        ME(mean, 0, i) = ((double)ME(mean, 0, i)) / ((double)ims->row_dim);
    }

    return mean;
}

/* calculateStandardDeviation

Calculates the standard deviation of all elements in matrix
*/
Matrix calculateStandardDeviation(Matrix ims)
{
    double Sum;
    int i, j;
    Matrix smean = calculateMean(ims);
    Matrix stdev = makeMatrix(smean->row_dim, 1);
    for (i = 0 ; i < ims->row_dim; i++)
    {
        Sum = 0;
        for (j = 0 ; j < ims->col_dim; j++)
        {		
            Sum = Sum + ((ME(ims, i, j) - ME(smean, i, 0)) 
                * (ME(ims, i, j) - ME(smean, i, 0)));
        }

        ME(stdev, i, 0) = (double) sqrt(((double)Sum) / ((double)ims->col_dim));		
    }

    freeMatrix(smean);
    return stdev;
}

/* calculateMean

Calculates the mean of all elements of a matrix
*/
Matrix calculateMean(Matrix ims)
{
    int i, j;
    double Sum;
    Matrix mean = makeMatrix(ims->row_dim, 1);

    for (i = 0 ; i < ims->row_dim; i++)
    {
        Sum = 0;
        for (j = 0 ; j < ims->col_dim; j++)
        {		
            Sum = Sum + ME(ims, i, j);
        }

        ME(mean, i, 0) = Sum / (double)ims->col_dim;
    }

    return mean;
}

/* computeCovariance

Computes the covairance of a matrix 
*/
Matrix computeCovariance(Matrix A)
{
    int i, j, k;
    double sum;

    Matrix X, M, C;	

    C = makeMatrix(A->col_dim, A->col_dim);
    X = duplicateMatrix(A);

    //mean subtraction
    M = get_mean_each_column(X);
    each_mean_subtract_column(X, M); 

    for(i = 0; i < X->col_dim; i ++)
        for(j = i; j < X->col_dim; j ++)
        {
            sum = 0;
            for(k = 0; k < X->row_dim; k ++)
            {
                //printf("k = %d and SUM = %e \n", k, sum);
                sum += (ME(X, k, i) * ME(X, k, j));
                //getchar();
            }

            k = X->row_dim - 1;

            //printf("ME(X, %d, %d) = %e and  ME(X, %d, %d) = %e \n", k, i, ME(X, k, i), k, j, ME(X, k, j)); 
            //printf("SUM = %e and X->row_dim = %d \n", sum, X->row_dim);
            ME(C, i, j) = ((double)sum / (double)(X->row_dim - 1));
            ME(C, j, i) = ME(C, i, j);
            //printf("ME(C, %d, %d) = %e and ME(C, %d, %d) = %e \n", i, j, ME(C, i, j), j, i, ME(C, j, i));
            //getchar();
        }

        freeMatrix(X);
        freeMatrix(M);
        return C;	
}

/*B if is a positive definitive matrix, then
We can find B = LL' using cholesky decomposition
returns L*/
Matrix choleskyDecomposition (Matrix B)
{
    int i, j;
    Matrix LP = makeZeroMatrix(B->row_dim, B->col_dim);

    //iterate over lower matrix only
    for( i = 0 ; i < B->row_dim ; i++ )
        for( j = 0 ; j <= i ; j++ )
        {
            if(i == j)
                ME(LP, i, j) = sqrt(ME(B, i, j) - computeDigonalSum(LP, i, i));
            else
                ME(LP, i, j) = (ME(B, i, j) - computeDigonalSum(LP, i, j) ) / ME(LP, j, j);
        }

        return LP;
}

/*used by cholesky decomposition, computes sum 
upto a specified row and specified column*/
double computeDigonalSum (Matrix LP, int i, int j)
{
    int k;
    double Sum = 0;

    for(k = 0; k < j; k++)
    {
        Sum = Sum + (ME(LP, i, k) * ME(LP, j, k));
    }

    return Sum;
}
/*END: Changed by Zeeshan: for LPP*/
