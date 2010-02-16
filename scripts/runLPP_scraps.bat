@echo off
if not %0==scripts\runLPP_scraps.bat goto INCORRECTPATH

rem Using normalized raster images from directory <data\csuScrapShots\normSep2002sfi>


rem Creating directory for training data.

md train\scraps

rem Running standard feret PCA training.

csuSubspaceTrain -cutOffMode ENERGY -cutOff 98 -lpp -lppDist MahCosine -lppKeepNVectors 50 -k 5 -imDir data\csuScrapShots\normSep2002sfi imagelists\scrap_all.srt train\scraps\feretPCA.train

rem Clearing out old distances and\or creating new distances directory

rd /S /Q distances\scraps\LPP_Euclidean
md distances\scraps\LPP_Euclidean

rem Projecting face images onto LPP basis and computing distances

csuSubspaceProject -imDir data\csuScrapShots\normSep2002sfi train\scraps\feretPCA.train imagelists\scrap_all_x4.srt distances\scraps\LPP_Euclidean Euclidean
echo Indicator file used by the scripts to know whether the data in this directory is valid > distances\scraps\lppDataExists.txt

goto END

:INCORRECTPATH
echo Please run this command from the csuEvalFaceRec directory
:END