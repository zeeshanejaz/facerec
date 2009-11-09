#include <csuSubspaceLaplacian.h>
void laplacianTrain (Matrix imspca, ImageList *srt, Matrix * laplacianBasis, Matrix * laplacianValues,
int K, int useAdaptiveK, int lppKeepNVectors, Matrix * pcaValues, char* distName, int writeTextInterm)
{	
	int i, n;
	Matrix Data, Dist, L;
	Matrix D, DPrime1, B, BInv;
	Matrix W, LPrime1, A;
	Matrix laplacianImages, m;
	Matrix tmp, kMat;
	FTYPE maxLembda = 0;
	
	assert(lppKeepNVectors >= 0);
	assert(K > 0);
	
	//we will use Data as input matrix
	Data = imspca;
		
	//computing the distances between image vectors
	printf("\nComputing distance using %s distance measure.", distName);
	Dist = computeDistances(Data, *pcaValues, Data->col_dim, K, distName);

	if(useAdaptiveK)
	{
		//compute the KMAT 
		kMat = computeKMat(Dist);	
	}
	else
	{
		kMat = valueMatrix(Dist->row_dim, 1, 1.0);
	}		

	//Computing neighbourhood weight map	
	W = findWMatrix(Dist, K, kMat);
	MakeSymmetric(W);
	
	//Apply class info	
	//applyClassInfo(W, srt);

	//computing sum of rows into diagonal matrix	
	D = findDMatrix(W);
	
	//L = W - D
	L = findLMatrix(W, D);	

	//B = Data * D * Data'
	DPrime1 = multiplyMatrix(Data, D);
	B = transposeMultiplyMatrixR(DPrime1, Data);
	MakeSymmetric(B);	

	//A = Data * L * Data'
	//Minimization eigen problem is solved using maximum eigen solution
	//use W instead of L, see seciton below to see why
	LPrime1 = multiplyMatrix(Data, L);
	A = transposeMultiplyMatrixR(LPrime1, Data);
	MakeSymmetric(A);	
	
/*
	We need to solve the generalized eigen value problem
	(Data * L * Data') * V = lembda * (Data * D * Data') * V

	which is basically the generalized eigen vector problem
	A * V = lemba * B * V
	
	Since B is a positive definitive matrix, its inverse can be computed
	this makes our problem as	
	(B^-1) * (A) * V = lemba * V
	
	substitute symbol	
		(B^-1) * (A) = C
	
	the problem becomes simple eigen value problem
	C * V = lemba * V
*/

	//Taking its inverse
	BInv = invertRREF(B);
	
	//Computing laplacianImages
	laplacianImages = multiplyMatrix(BInv, A);
	(*laplacianBasis) = makeMatrix(laplacianImages->row_dim, laplacianImages->col_dim);
	(*laplacianValues) = makeMatrix(laplacianImages->row_dim, 1);
	//MakeSymmetric(laplacianImages);	

	printf("Computing laplacianfaces.\n");	
        cvJacobiEigens_64d(laplacianImages->data, (*laplacianBasis)->data, (*laplacianValues)->data, laplacianImages->col_dim, 0.0, 0);
   	    	
	printf("Finished computing laplacianspace.\n");
	
	maxLembda = ME((*laplacianValues), ((*laplacianValues)->row_dim - 1), 0);
	printf("Max Lembda = %e.\n", maxLembda);	
				
	
	n = (*laplacianBasis)->col_dim;
	if(lppKeepNVectors < n && lppKeepNVectors > 0)
	{
		printf("Droping %d vectors of laplacian subspace. \n", n - lppKeepNVectors);
		n = lppKeepNVectors;
 	    	tmp = matrixCols ((*laplacianBasis), 0, n - 1);
  	    	freeMatrix (*laplacianBasis);
  		*laplacianBasis = tmp;
	}

	///////////////////////////////////////////////////////////////////////////////////////
	
	
	
	///////////////////////////////////////////////////////////////////////////////////////

	printf("Keeping %d vectors of laplacian subspace. \n", n);

	/* Numerical roundoff errors may lead to small negative values.
     	Strip those before saving the matrix. */	
  	m = (*laplacianValues);

	for (i = 0; i < m->row_dim; i++)
	{
		ME(m, i ,0) = maxLembda - ME(m, i ,0);
		if (ME(m,i,0) <= 0)
		{
			if (ME(m,i,0) < -1e-10)
		  		printf("WARNING: Large negative eigenvalue found %f. Truncating to zero.\n", ME(m,i,0));
			ME(m,i,0) = 0.0000001;
	      	}
		//printf("LaplacianValues [%d] = %e \n", i, ME(m, i ,0));
	}

	/*********************************************************************/

	//release memory
	freeMatrix(Data);
	freeMatrix(Dist);
	freeMatrix(kMat);
	freeMatrix(W);
	freeMatrix(D);
	freeMatrix(B);
	freeMatrix(A);
	freeMatrix(DPrime1);
	freeMatrix(LPrime1);
	freeMatrix(BInv);
	freeMatrix(laplacianImages);
}

Matrix computeKMat(Matrix Dist)
{
	int  i, j;
	FTYPE sum = 0.0, min = 32000.0, max = 0.0;
	Matrix kMat = makeMatrix(Dist->row_dim, 1);
	
	
	for(i = 0; i < Dist->row_dim; i++)
	{
		sum = 0.0;

		for(j = 0; j < Dist->col_dim; j++)
		{
			sum += ME(Dist, i, j);
		}
	
		ME(kMat, i, 0) = sum;
	}

	for(i = 0; i < kMat->row_dim; i++)
	{
		if(ME(kMat, i, 0) > max)
			max = ME(kMat, i, 0);

		if(ME(kMat, i, 0) < min)
			min = ME(kMat, i, 0);

	}

	for(i = 0; i < kMat->row_dim; i++)
	{
		//min max normalization with inverting polarity
		ME(kMat, i, 0) = 1 - (ME(kMat, i, 0) - min) / (max - min);
	}

	return kMat;	
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
		for (j = 0 ; j < D->col_dim; j++)
		{
			ME(D, i, j) = 0;
			Sum = Sum + ME(Dist, i, j);
		}
		
		ME(D, i, i) = Sum;
	}

	return D;
}

// For Taj

Matrix findWMatrix(Matrix Dist, int K, Matrix kMat)
{	int i, j;
	Matrix W;
	FTYPE weight;

	W = makeMatrix (Dist->row_dim, Dist->col_dim);
	
	for(i = 0; i < Dist->row_dim; i++)
	{
		for(j = 0; j < Dist->col_dim; j++)
		{
			weight = exp(-1 * ME(Dist, i, j));
			
			ME(W, i, j) = weight;			
		}
	}
		
	return W;
}
/*

Matrix findWMatrix(Matrix Dist, int K, Matrix kMat)
{
	int i, j, kNrm;
	Matrix W;
	FTYPE weight, t;

	W = makeMatrix (Dist->row_dim, Dist->col_dim);

	FTYPE avgKthDistance = 0.0;
	for(i = 0; i < Dist->row_dim; i++)
	{
		kNrm = (int)((FTYPE)K * ME(kMat, i, 0));
		//printf("[%d * %e = %d],", K, ME(kMat, i, 0), kNrm);
		//take weighted average to avoid overflow
		avgKthDistance = (avgKthDistance) + GetKthItemInRowAfterSorting(Dist, i, kNrm);
	}
	
	avgKthDistance = ((FTYPE)(avgKthDistance) / (FTYPE)(Dist->row_dim));

	//2*Sigma^2
	t = 2 * avgKthDistance * avgKthDistance;
	//printf("\n\nt = %e\n\n", t);
	for(i = 0; i < Dist->row_dim; i++)
	{
		for(j = 0; j < Dist->col_dim; j++)
		{
			weight = exp(-1 * ME(Dist, i, j) * ME(Dist, i, j) /  t  );
			ME(W, i, j) = weight;
			ME(W, j, i) = weight;
		}
	}
	
	//MakeSymmetric(W);
	return W;
}
*/
void applyClassInfo(Matrix W, ImageList* srt)
{
	int i, j;
	ImageList *subject, *rep1, *rep2;

	for (subject = srt; subject; subject = subject->next_subject) 
	{		
		for (rep1 = subject; rep1; rep1 = rep1->next_replicate) 
		{
			i = rep1->imageIndex;	
			for (rep2 = subject; rep2; rep2 = rep2->next_replicate) 
			{
				j = rep2->imageIndex;
				ME(W, i, j) = 1.0;		
				ME(W, j, i) = 1.0;
			}
		}
	}	
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


