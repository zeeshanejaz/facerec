@echo off
if not %0==scripts\runLDA_feret.bat goto INCORRECTPATH

rem Using normalized raster images from directory <data\FERET\normSep2002sfi>

rem Creating directory for training data.

md train\feret

rem Running standard feret PCA training.

csuSubspaceTrain -lda -imDir data\FERET\normSep2002sfi imagelists\feret_training.srt train\feret\feretLDA.train

rem Clearing out old distances and\or creating new distances directory

rd /S /Q distances\feret\LDA_ldaSoft
md distances\feret\LDA_ldaSoft

rem Projecting face images onto LDA basis and computing distances

csuSubspaceProject -imDir data\FERET\normSep2002sfi train\feret\feretLDA.train imagelists\all.srt distances\feret\LDA_ldaSoft ldaSoft

echo Indicator file used by the scripts to know whether the data in this directory is valid > distances\feret\ldaDataExists.txt

goto END

:INCORRECTPATH
echo Please run this command from the csuEvalFaceRec directory
:END