@echo off
if not %0==scripts\runLDA_scraps.bat goto INCORRECTPATH

rem Using normalized raster images from directory <data\csuScrapShots\normSep2002sfi>


rem Creating directory for training data.

md train\scraps

rem Running standard feret PCA training.

csuSubspaceTrain -lda -imDir data\csuScrapShots\normSep2002sfi imagelists\scrap_all.srt train\scraps\scrapsLDA.train

rem Clearing out old distances and\or creating new distances directory

rd /S /Q distances\scraps\LDA_ldaSoft
md distances\scraps\LDA_ldaSoft

rem Projecting face images onto LDA basis and computing distances

csuSubspaceProject -imDir data\csuScrapShots\normSep2002sfi train\scraps\scrapsLDA.train imagelists\scrap_all_x4.srt distances\scraps\LDA_ldaSoft ldaSoft
echo Indicator file used by the scripts to know whether the data in this directory is valid > distances\scraps\ldaDataExists.txt

goto END

:INCORRECTPATH
echo Please run this command from the csuEvalFaceRec directory
:END