#include "csuSubspaceLaplacian.h"
void laplacianTrain (Subspace*s, Matrix imspca, Matrix * laplacianBasis, Matrix * laplacianValues, int writeTextInterm)
{	
	int i;
	Matrix Data, Dist, L;
	Matrix D, DPrime1, B;
	Matrix W, LPrime1, A;
	Matrix LCHOL, LCHOLInv, LCHOLInv_x_A;
	Matrix laplacianImages;
	Matrix meanValues, m;

	//we will use Data as input matrix
	Data = imspca;

	//computing the distances between image vectors
	Dist = ComputeDistanceMatrix(Data);

	//Computing neighbourhood weight map	
	W = findWMatrix(Dist, s->neighbourCount, s->t);

	//computing sum of rows into diagonal matrix	
	D = findDMatrix(W);
	
	L = findLMatrix(W, D);

	//B = Data * D * Data'
	DPrime1 = multiplyMatrix(Data, D);
	B = transposeMultiplyMatrixR(DPrime1, Data);
	MakeSymmetric(B);

	//A = Data * L * Data'
	//Minimization eigen problem is solved using maximum eigen solution
	//use W instead of L, see seciton below to see why
	LPrime1 = multiplyMatrix(Data, W);
	A = transposeMultiplyMatrixR(LPrime1, Data);
	MakeSymmetric(A);
	
/*
	We need to solve the generalized eigen value problem
	(Data * L * Data') * V = neta * (Data * D * Data') * V
	(Data * (W-D) * Data') * V = neta * (Data * D * Data') * V
	(Data * W * Data') * V - (Data * D * Data') * V = lembda * (Data * D * Data') * V
	(Data * W * Data') * V = neta * (Data * D * Data') * V + (Data * D * Data') * V
	(Data * W * Data') * V = (neta + 1) * (Data * D * Data') * V

	here we put
	lembda = neta + 1

	(Data * W * Data') * V = lembda * (Data * D * Data') * V

	REQUEST FOR COMMENT
	if we ADD 1 to the eigenvalue for maximization problem,
	it gives us the minimum value solutions????????????????

	Substituting Symbols
		(Data * L * Data') = A
		(Data * D * Data') = B
	
	The problem becomes
	A * V = lemba * B * V
	
	Since B is a positive definitive matrix,
	We can perform cholesky decomposition
	B = l * l'

	this makes our problem as	
	A * V = lemba * l * l' * V
	(l^-1 * A * l'^-1) * V = lemba * V
	
	substitute symbol	
		(l^-1 * A * l'^-1) = C
	
	the problem becomes simple eigen value problem
	C * V = lemba * V
*/

	//Perform cholesky decomposition over positive definitive matrix B
	LCHOL = choleskyDecomposition(B);

	//Taking its inverse
	LCHOLInv = invertRREF(LCHOL);
	
	//Computing laplacianImages
	LCHOLInv_x_A = multiplyMatrix(LCHOLInv, A);
	laplacianImages = transposeMultiplyMatrixR(LCHOLInv_x_A, LCHOLInv);

	printf("Computing laplacianfaces.\n");
	
	eigentrain (&meanValues, laplacianValues, laplacianBasis, laplacianImages);

	printf("Finished computing laplacianspace.\n");

	/* Numerical roundoff errors may lead to small negative values.
     	Strip those before saving the matrix. */

	subtractMatrix(laplacianValues - 1);
  	m = *laplacianValues;

	for (i = 0; i < m->row_dim; i++)
	{
		if (ME(m,i,0) < 0)
		{
			if (ME(m,i,0) < -1e-10)
		  		printf("WARNING: Large negative eigenvalue found %f. Truncating to zero.\n", ME(m,i,0));
			ME(m,i,0) = 0;
	      	}
	}

	/* output textfiles of intermediate matrices */
	if (writeTextInterm)
	{ 
	//	SAVE_MATRIX(*laplacianBasis);
	//	SAVE_MATRIX(*laplacianValues);
	} 
	/*********************************************************************/

	//release memory
	freeMatrix(Data);
	freeMatrix(Dist);
	freeMatrix(W);
	freeMatrix(D);
	freeMatrix(B);
	freeMatrix(A);
	freeMatrix(DPrime1);
	freeMatrix(LPrime1);
	freeMatrix(LCHOL);
	freeMatrix(LCHOLInv);
	freeMatrix(LCHOLInv_x_A);
	freeMatrix(laplacianImages);
	freeMatrix(meanValues);

}

Matrix findLMatrix(Matrix W, Matrix D)
{
	return subtractMatrix(D, W);
}

Matrix findDMatrix(Matrix Dist)
{
	int i, j;
	Matrix D;
	FTYPE Sum;

	D = makeMatrix(Dist->row_dim, Dist->col_dim);
	
	for (i = 0 ; i < D->row_dim; i++)
	{
		Sum = 0;
		for (j = 0 ; j < D->row_dim; j++)
		{
			ME(D, i, j) = 0;
			Sum = Sum + ME(Dist, i, j);
		}
		
		ME(D, i, i) = Sum;
	}

	return D;
}

Matrix findWMatrix(Matrix Dist, int neighbourCount, double t)
{
	Matrix W;

	/*Compute connected graph of data points*/
	W = ComputeConnectivityGraph(Dist, neighbourCount, t);
	
	return W;
}	

Matrix ComputeConnectivityGraph(Matrix Dist, int neighbourCount, double t)
{	int i, j, *neighbours;
	Matrix C;
	FTYPE weight;

	C = makeMatrix (Dist->row_dim, Dist->col_dim);

	for(i = 0; i < Dist->row_dim; i++)
		for(j = 0; j < Dist->col_dim; j++)
			ME(C, i, j) = 0;
	
	for(i = 0; i < Dist->row_dim; i++)
	{
		neighbours = GetKNeighbours(Dist, i, neighbourCount);
	
		for(j = 0; j < neighbourCount; j++)
		{
			weight = exp( (-1 * ME(Dist, i, neighbours[j]))  /  (2 * pow(t,2))  );
			ME(C, i, neighbours[j]) = weight;
		}
	}
	
	MakeSymmetric(C);
	return C; 		
}

FTYPE max (FTYPE A, FTYPE B)
{
	FTYPE tmp;
	(A > B) ? (tmp = A) : (tmp = B);
	return tmp;
}

void MakeSymmetric(Matrix X)
{
	int i, j;

	for(i = 0; i < X->row_dim; i++)
		for(j = 0; j < X->col_dim; j++)
		{
			ME(X, i, j) = max(ME(X, i, j), ME(X, j, i));
			ME(X, j, i) = ME(X, i, j);
		}
}

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

int* GetKNeighbours(Matrix Dist, int item, int K)
{
  int *indices, i;
  dbPair *toSort = (dbPair*) malloc (Dist->col_dim * sizeof (dbPair));

  for(i = 0; i < Dist->col_dim; i++)
    {
      toSort[i].key   = ME(Dist, item, i);
      toSort[i].index = i;
    }

  qsort (toSort, Dist->col_dim, sizeof (dbPair), elementComparator);
  indices = (int*) malloc (Dist->col_dim * sizeof (int));

  for(i = 0; i < K; i++) 
  {
    indices[i] = toSort[i].index;
  } 

  free (toSort);
  return indices;
}


Matrix ComputeDistanceMatrix(Matrix data)
{
	int i, j;

	/*Computing distance matrix*/
	Matrix D = makeMatrix(data->col_dim, data->col_dim);
	for( i = 0 ; i < D->col_dim ; i++)
		for( j = i ; j < D->row_dim ; j++)
			//fill upper diagonal triangle
		{			ME(D, i, j) = distEuclidean(data, i, j);
			
			ME(D, j, i) = ME(D, i, j);
		}
	/*Computing distance matrix*/
	return D;
}

FTYPE distEuclidean(const Matrix ims, int i, int j)
{
    int k;
    FTYPE sum = 0.0;
    for (k = 0; k < ims->row_dim; k++) 
	{
        sum += (  (ME(ims, k, i) - ME(ims, k, j)) 
				* (ME(ims, k, i) - ME(ims, k, j)) );
    }
	
	return sum;
}

