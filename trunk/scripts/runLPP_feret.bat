@echo off
if not %0==scripts\runLPP_feret.bat goto INCORRECTPATH

rem Using normalized raster images from directory <data\FERET\normSep2002sfi>


rem Creating directory for training data.

md train\feret

rem Running feret LPP training.

csuSubspaceTrain -cutOffMode ENERGY -cutOff 98 -lpp -lppDist MahCosine -imDir data\FERET\normSep2002sfi imagelists\feret_training.srt train\feret\feretLPP.train -writeTextInterm

rem Clearing out old distances and\or creating new distances directory

rd /S /Q distances\feret\LPP_Euclidean
md distances\feret\LPP_Euclidean

rem Projecting face images onto LPP basis and computing distances

csuSubspaceProject -imDir data\FERET\normSep2002sfi train\feret\feretLPP.train imagelists\all.srt distances\feret\LPP_Euclidean Euclidean
echo Indicator file used by the scripts to know whether the data in this directory is valid > distances\feret\lppDataExists.txt

goto END

:INCORRECTPATH
echo Please run this command from the csuEvalFaceRec directory
:END