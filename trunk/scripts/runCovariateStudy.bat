@echo off
if not %0==scripts\runCovariateStudy.bat goto INCORRECTPATH

if %1.==. (
    echo Please specify at least one argument: example:
    echo    runCovariateStudy feret_training
    goto END
)

set IMAGEDIR=data\FERET\images\processed\norm26x30Sep2002sfi\
set TRAINING_SRT=imagelists\%1.srt 

md  results\CovStudy train\CovStudy^
    distances\CovStudy\%1_Cityblock^
    distances\CovStudy\%1_Euclidean^
    distances\CovStudy\%1_Correlation^
    distances\CovStudy\%1_Covariance^
    distances\CovStudy\%1_MahCosine^
    distances\CovStudy\%1_Euclidean^
    distances\CovStudy\%1_Bayesian_ML^
    distances\CovStudy\%1_Bayesian_MAP

rem Run nearest-neighbor classifier

csuSubspaceTrain -imDir %IMAGEDIR% -cutOffMode ENERGY %TRAINING_SRT% train\CovStudy\%1.train

csuSubspaceProject train\CovStudy\%1.train imagelists\all.srt -imDir %IMAGEDIR%^
    distances\CovStudy\%1_MahCosine\ MahCosine^
    distances\CovStudy\%1_Euclidean\ Euclidean

rem Run Bayesian classifier

csuBayesianTrain -imDir %IMAGEDIR% %TRAINING_SRT% train\CovStudy\%1.train

csuBayesianProject -imDir %IMAGEDIR% train\CovStudy\%1.train.intra train\CovStudy\%1.train.extra imagelists\all.srt^
    distances\CovStudy\%1_Bayesian_ML distances\CovStudy\%1_Bayesian_MAP

rem Generate the covariates

csuCovariateStudy imagelists\list2144.srt %TRAINING_SRT% results\CovStudy\%1.covStudy^
    -cov imagelists\facecovariates.cov^
    -cov imagelists\imagecovariates.cov^
    -dist distances\CovStudy\%1_Euclidean^
    -dist distances\CovStudy\%1_MahCosine^
    -dist distances\CovStudy\%1_Bayesian_ML^
    -dist distances\CovStudy\%1_Bayesian_MAP

csuCovariateStudy imagelists\list2144.srt %TRAINING_SRT% results\CovStudy\%1.rank5.covStudy^
    -nrank 5^
    -cov imagelists\facecovariates.cov^
    -cov imagelists\imagecovariates.cov^
    -dist distances\CovStudy\%1_Euclidean^
    -dist distances\CovStudy\%1_MahCosine^
    -dist distances\CovStudy\%1_Bayesian_ML^
    -dist distances\CovStudy\%1_Bayesian_MAP
   
goto END

:INCORRECTPATH
    echo PLEASE RUN THIS SCRIPT FROM THE TOP LEVEL DIRECTORY
    echo 
    echo This script runs a single covariate training experiment and
    echo generates covariate datafile for processing with statistical
    echo software.  The script expects one argument which is the
    echo basename WITHOUT the ".sfi" extention for an image list in
    echo the imagelist directory.  The script will train on the imagelist
    echo "imagelist\%1.srt" and produce distance files based on the list.
    echo To build csuCovariateStudy run "make extra"  this builds cpp
    echo codes in the extras directory need to run the experiment.
    echo You also may need csuToolAddSFI to add the sfi extention
    echo to the back of the imagelists.  This is used by the command:
    echo "csuToolAddSFI < base.list > new.list"

:END