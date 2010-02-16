@echo off
if not %0==scripts\runPCATrainingExp.bat goto INCORRECTPATH

rem Using normalized raster images from directory <data\FERET\normSep2002sfi>


echo Creating directory for training data.

md train\feret results

echo Running standard feret PCA training on file %1

csuSubspaceTrain -dropNVectors 1 -cutOffMode ENERGY -cutOff 90 -imDir data\FERET\normSep2002sfi %1 train\feret\PCA%1.train

echo Clearing out old distances and/or creating new distances directory


md distances\feret\PCA%1_MahAngle

echo Projecting face images onto PCA basis and computing distances

csuSubspaceProject -imDir data\FERET\normSep2002sfi train\feret\PCA%1.train imagelists\all.srt distances\feret\PCA%1_MahAngle MahAngle

echo Indicator file used by the scripts to know whether the data in this directory is valid > distances\feret\pcaDataExists.txt

echo Doing permutation tests for PCA%1_MahAngle

csuAnalyzePermute distances\feret\PCA%1_MahAngle imagelists\list1024_all_subset.srt results\PCA%1_MahAngle_

echo Computing feret curves for PCA%1_MahAngle

csuAnalyzeRankCurve imagelists\fafb.srt imagelists\feret_gallery.srt distances\feret\PCA%1_MahAngle -filePrefix results\PCA%1_MahAngle_FAFB
csuAnalyzeRankCurve imagelists\fafc.srt imagelists\feret_gallery.srt distances\feret\PCA%1_MahAngle -filePrefix results\PCA%1_MahAngle_FAFC
csuAnalyzeRankCurve imagelists\dup1.srt imagelists\feret_gallery.srt distances\feret\PCA%1_MahAngle -filePrefix results\PCA%1_MahAngle_DUP1
csuAnalyzeRankCurve imagelists\dup2.srt imagelists\feret_gallery.srt distances\feret\PCA%1_MahAngle -filePrefix results\PCA%1_MahAngle_DUP2

rd /S /Q distances\feret\PCA%1_MahAngle


goto END

:INCORRECTPATH
echo Please run this command from the csuEvalFaceRec directory
:END