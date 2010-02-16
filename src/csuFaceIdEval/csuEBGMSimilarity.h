/*
$RCSfile: csuEBGMSimilarity.h,v $
$Author: teixeira $
$Date: 2003/04/23 14:02:31 $
*/

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

#ifndef EBGM_SIMILARITY
#define EBGM_SIMILARITY

#include <csuEBGMUtil.h>
/************************ Jet Similarity Functions *************************/

/* The following functions compute similarity between two
* Gabor Jets.  They are described in detail csuGaborJets.c
*/
typedef double (*JetSimilarityMeasure)(GaborJet j1, GaborJet j2);

double JetSimilarityMag(GaborJet j1, GaborJet j2);
double JetSimilarityPhase(GaborJet j1, GaborJet j2);
double JetSimilarityDEGridSample(GaborJet j1, GaborJet j2);
double JetSimilarityDEPredictiveStep(GaborJet j1, GaborJet j2);
double JetSimilarityDEPredictiveIter(GaborJet j1, GaborJet j2);
double JetSimilarityDEFixedLocalSearch(GaborJet j1, GaborJet j2);
double JetSimilarityDENarrowingLocalSearch(GaborJet j1, GaborJet j2);
double JetSimilarityCityBlock(GaborJet j1, GaborJet j2);
double JetSimilarityCorrelation(GaborJet j1, GaborJet j2);
double JetSimilarityCovariance(GaborJet j1, GaborJet j2);

/* These similarty measures are discussed in David Bolme's thesis.
Results were generated using csuEBGMLocalization */
double JetSimilarityResponseMag(GaborJet j1, GaborJet j2);
double JetSimilarityResponsePhase(GaborJet j1, GaborJet j2);
double JetSimilarityDisplacementCheckMag(GaborJet j1, GaborJet j2);
double JetSimilarityDisplacementCheckPhase(GaborJet j1, GaborJet j2);

/********************** Jet Displacement Estimation ************************/

/* This function estimates the displacement of gabor jet
* relative to a sample jet.  The method that is used is
* simmillar to auto correlation.  Also look at the similarity
* function for phase 1 and 3 for related code. */
typedef double (*JetDisplacementEstimator)(GaborJet j1, GaborJet j2, double *tdx, double *tdy);

double estimateDisplacementRandom (GaborJet j1, GaborJet j2, double *tdx, double *tdy);
double DEGridSample(GaborJet j1, GaborJet j2, double *tdx, double *tdy);
double DEPredictiveStep(GaborJet j1, GaborJet j2, double *tdx, double *tdy);
double DEPredictiveIter(GaborJet j1, GaborJet j2, double *tdx, double *tdy);
double DEFixedLocalSearch(GaborJet j1, GaborJet j2, double *tdx, double *tdy);
double DENarrowingLocalSearch(GaborJet j1, GaborJet j2, double *dx, double *dy);

/********************** Face Graph Similarity ************************/
typedef double (*FaceGraphSimilarity)(FaceGraph, FaceGraph);

double GeometrySimL2(FaceGraph, FaceGraph);
double GeometrySimCorrelation(FaceGraph, FaceGraph);
double GeometrySimLeastSquares(FaceGraph, FaceGraph);
double GeometrySimLeastSquaresPS(FaceGraph f1, FaceGraph f2);
double GeometrySimLeastSquaresPI(FaceGraph f1, FaceGraph f2);
double GeometrySimLeastSquaresFLS(FaceGraph f1, FaceGraph f2);
double GeometrySimLeastSquaresNLS(FaceGraph f1, FaceGraph f2);
double GeometrySimWLeastSquaresPI(FaceGraph f1, FaceGraph f2);

double fgSimMagnitude(FaceGraph, FaceGraph);
double fgSimPhase(FaceGraph, FaceGraph);
double fgSimPhaseGridSample(FaceGraph, FaceGraph);
double fgSimPhasePredictiveStep(FaceGraph, FaceGraph);
double fgSimPhasePredictiveIter(FaceGraph, FaceGraph);
double fgSimPhaseFixedLocalSearch(FaceGraph, FaceGraph);
double fgSimPhaseNarrowingLocalSearch(FaceGraph, FaceGraph);

#endif



