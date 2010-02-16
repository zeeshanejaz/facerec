#include <csuSubspaceIndependent.h>
#include <stdlib.h>
#include <float.h>

void independentTrain (Matrix pcaBasis, Matrix imspca, Matrix * independentBasis, int arch, int blockSize, double learningRate, int iterations)
{	
    Matrix m, x, xx, c, csq, wz, xxt, w_wz;
    Matrix w, id, input;
    int *order;

    //ICA1
    if(arch == 1)
    {
        //use PCA basis
        input = pcaBasis;
    }	
    //ICA2
    else if (arch == 2)
    {
        //use PCA projected images 
        input = imspca;
    }
    //Invalid ArchType
    else
    {	
        printf("Invalid ArchType: %d. Should be 1 or 2\n", arch);
        exit(0);
    }

    printf("Computing random permutation to remove stationarity of data.\n");
    order = shuffle(input->row_dim);
    xx = orderRows(input, order);

    printf("Performing Normalization of images by subtracting mean.\n");
    m = get_mean_each_column(xx);
    each_mean_subtract_column(xx, m);

    printf("Computing the whitening filter.\n");
    c = computeCovariance(xx);
    csq = sqrtMatrix(c);
    wz = invertRREF(csq);		
    multiplyMatrixScalar(wz, 2.0);

    printf("Size of whitening filter is %d by %d.\n", wz->row_dim, wz->col_dim);

    //computing whitened data
    xxt = transposeMatrix(xx);	
    x = multiplyMatrix(wz, xxt);

    //making identity matrix
    printf("Making Identity matrices of size %d.\n", x->row_dim);
    w = makeIdentityMatrix(x->row_dim);
    id = makeIdentityMatrix(x->row_dim); 

    multiplyMatrixScalar(id, blockSize);

    printf("Starting ICA Learning ... \n");

    printf("Learning Parameters: Learning Rate = %e ; Iterations %d \n", learningRate, iterations);
    learnWeightMatrix(w, x, id, learningRate, blockSize, iterations);

    //annealing 1	
    learningRate /= 2.0;
    iterations = (iterations / 4) + 1;
    printf("Annealing 1: Learning Rate = %e ; Iterations %d \n", learningRate, iterations);
    learnWeightMatrix(w, x, id, learningRate, blockSize, iterations);

    //annealing 2
    learningRate /= 2.0;
    iterations = (iterations / 4) + 1;
    printf("Annealing 2: Learning Rate = %e ; Iterations %d \n", learningRate, iterations);
    learnWeightMatrix(w, x, id, learningRate, blockSize, iterations);

    //annealing 3
    learningRate /= 2.0;
    iterations = (iterations / 4) + 1;
    printf("Annealing 3: Learning Rate = %e ; Iterations %d \n", learningRate, iterations);
    learnWeightMatrix(w, x, id, learningRate, blockSize, iterations);

    printf("Finished ICA Learning. \n");

    w_wz = multiplyMatrix(w, wz);
    (*independentBasis) = invertRREF(w_wz);	

    //=================================================//
    freeMatrix(xx);
    freeMatrix(xxt);
    freeMatrix(m);
    freeMatrix(c);
    freeMatrix(wz);
    freeMatrix(x);
    freeMatrix(w_wz);
}

void learnWeightMatrix(Matrix W, Matrix X, Matrix I, double L, int B, int iterations)
{
    int i;
    for(i = 0; i < iterations ; i++)
    {
        printf("Iteration %d with learning rate %e \n", i, L);
        sep96(W, X, I, L, B);	
    }
}

void sep96(Matrix W, Matrix XOrg, Matrix I, double L, int B)
{	
    Matrix U, Y, YU, YU_W;
    Matrix X;

    int i, t;
    int P = XOrg->col_dim;
    int nblocks = floor((double)P/(double)B); 

    for(t = 0, i = 0; t < (nblocks * B); t += B, i++)
    {
        printf("     Block no. %d with block size  %d \n", i, B);	

        X = matrixCols(XOrg, t, t + B - 1); 

        //U = W * X
        U = multiplyMatrix(W, X);

        //Y = (1 - 2 * ( 1 / ( 1 + exp(-U) ) ) )
        Y = getYMatrix(U);

        //YU = Y * U'
        YU = transposeMultiplyMatrixR(Y, U);

        // YU += I
        addMatrixEquals(YU, I);

        //YU_W = YU * W 
        YU_W = multiplyMatrix(YU, W);

        //YU_W *= L
        multiplyMatrixScalar(YU_W, L);

        //W += YU_W
        addMatrixEquals(W, YU_W);

        if(_isnan(ME(W, 0, 0)))
        {
            printf("NaN detected!\n");
            printf("Please adjust your learning rate and block size.\n");
            exit(0);
        }

        freeMatrix(X);
        freeMatrix(U);
        freeMatrix(Y);
        freeMatrix(YU);
        freeMatrix(YU_W);
    }
}

Matrix getYMatrix(Matrix U)
{
    int i, j;
    double value;
    Matrix Y = makeMatrix(U->row_dim, U->col_dim);

    for (i = 0; i < U->col_dim; i++) 
    {
        for (j = 0; j < U->row_dim; j++) 
        {
            value = -1.0 * ME(U, j, i);
            value = exp(value);
            value	 = 1.0 / (1.0 + value);
            value = 1.0 - (2.0 * value);
            ME(Y, j, i) = value;
        }
    }	

    return Y;
}

void verifyIndependentBasis(Matrix basis, Matrix imspca)
{
    //Matrix imt = transposeMatrix(imspca);
    Matrix UU = multiplyMatrix(basis, imspca);

    printf("Dimension of basis is %d by %d \n", basis->row_dim,basis->col_dim);
    printf("Dimension of imspca is %d by %d \n", imspca->row_dim,imspca->col_dim);
    printf("Verifying the independence of basis vectors. \n");

    printf("Exporting the projected images. \n");
    saveMatrixAscii("ica_UU.mat", "UU", UU, matlabFormat);
    //freeMatrix(imt);		
    freeMatrix(UU);	
}
