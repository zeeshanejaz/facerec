@echo off
if not %0==scripts\runPCA_scraps.bat goto INCORRECTPATH

rem Using normalized raster images from directory <data\csuScrapShots\normSep2002sfi>

rem Creating directory for training data.

md train\scraps

rem Running standard feret PCA training.

csuSubspaceTrain -imDir data\csuScrapShots\normSep2002sfi imagelists\scrap_all.srt train\scraps\scrapsPCA.train

rem Clearing out old distances and\or creating new distances directory

rd /S /Q distances\scraps\PCA_Euclidean distances\scraps\PCA_MahCosine
md distances\scraps\PCA_Euclidean distances\scraps\PCA_MahCosine

rem Projecting face images onto PCA basis and computing distances

csuSubspaceProject -imDir data\csuScrapShots\normSep2002sfi train\scraps\feretPCA.train imagelists\scrap_all_x4.srt distances\scraps\PCA_Euclidean Euclidean distances\scraps\PCA_MahCosine MahCosine
echo Indicator file used by the scripts to know whether the data in this directory is valid > distances\scraps\pcaDataExists.txt


goto END

:INCORRECTPATH
echo Please run this command from the csuEvalFaceRec directory
:END