@echo off
if not %0==scripts\runPCA_feret.bat goto INCORRECTPATH

rem Using normalized raster images from directory <data\csuScrapShots\normSep2002sfi>

rem Creating directory for training data.

md train\feret

rem Running standard feret PCA training.

csuSubspaceTrain -imDir data\FERET\normSep2002sfi imagelists\feret_training.srt train\feret\feretPCA.train

rem Clearing out old distances and\or creating new distances directory

rd \S \Q distances\feret\PCA_Euclidean distances\feret\PCA_MahCosine
md distances\feret\PCA_Euclidean distances\feret\PCA_MahCosine

rem Projecting face images onto PCA basis and computing distances

csuSubspaceProject -imDir data\FERET\normSep2002sfi train\feret\feretPCA.train imagelists\all.srt distances\feret\PCA_Euclidean Euclidean distances\feret\PCA_MahCosine MahCosine
echo Indicator file used by the scripts to know whether the data in this directory is valid > distances\scraps\pcaDataExists.txt


goto END

:INCORRECTPATH
echo Please run this command from the csuEvalFaceRec directory
:END