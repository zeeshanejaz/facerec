@echo off
if not %0==scripts\GenerateFeretResults.bat goto INCORRECTPATH

rem this script selects distance directories in the feret directory and then generates results for each.

set RESULTS_DIR=results\CSUBaselineResults
set DIST_DIR=distances\feret
set VERSION=CSU Face Idenification Evaluation System 5.0

rd %RESULTS_DIR% /S /Q 
md %RESULTS_DIR%
md %RESULTS_DIR%\Perm

rem Collect all directories that have the proper number of distance files. 

set basenames=
set dists=
set oldPath=%CD% 
set tempvar=

cd %DIST_DIR%
for /d %%I IN (*) do (
set basenames=!basenames! %%I
set dists=!dists! !DIST_DIR!\%%I
)

cd %oldPath%

echo Generateing results for: %dists%

rem Calculating the rank curves for fafb
csuAnalyzeRankCurve -filePrefix FAFB imagelists\fafb.srt imagelists\feret_gallery.srt -outDir %RESULTS_DIR% %dists%

rem Calculating the rank curves for fafc
csuAnalyzeRankCurve -filePrefix FAFC imagelists\fafc.srt imagelists\feret_gallery.srt -outDir %RESULTS_DIR% %dists%

rem Calculating the rank curves for dup1
csuAnalyzeRankCurve -filePrefix DUP1 imagelists\dup1.srt imagelists\feret_gallery.srt -outDir %RESULTS_DIR% %dists%

rem Calculating the rank curves for dup2
csuAnalyzeRankCurve -filePrefix DUP2 imagelists\dup2.srt imagelists\feret_gallery.srt -outDir %RESULTS_DIR% %dists%

extras\compareRankCurves.py %RESULTS_DIR%\DUP1_Curve.txt %RESULTS_DIR%\DUP2_Curve.txt %RESULTS_DIR%\FAFB_Curve.txt %RESULTS_DIR%\FAFC_Curve.txt 

rem Run a permutation comparison for all of the results
csuAnalyzePermute imagelists\list640.srt %RESULTS_DIR%\Perm %dists%


echo ^<HTML^>^<HEAD^>^<TITLE^>CSU Baseline Results on the Feret Database^</TITLE^>^</HEAD^>^<BODY^> > %RESULTS_DIR%\index.html
echo ^<H1^>CSU Baseline Results on the Feret Database^</H1^>^<BR^> >> %RESULTS_DIR%\index.html
echo %VERSION% ^<BR^>Result generated at: %date% ^<HR^> >> %RESULTS_DIR%\index.html

set probes=FAFB FAFC DUP1 DUP2
for %%P in (%probes%) do (
    echo %%P
    echo ^<P^>^<H3^>%%P Results:^</H3^>^<BR^> >> !RESULTS_DIR!\index.html
    echo Recognition curves in text format. ^(^<A HREF^="%%P_Curve.txt"^>%%P_Curve.txt^</A^>^)^<BR^>  >> !RESULTS_DIR!\index.html
    echo Individual Recogntion Counts. ^(^<A HREF^="%%P_Images.txt"^>%%P_Images.txt^</A^>^)^<BR^>  >> !RESULTS_DIR!\index.html
    echo Recongnition curves image. ^(^<A HREF^="%%P_Curve.png"^>%%P_Curve.png^</A^>^)^<BR^>  >> !RESULTS_DIR!\index.html
    echo Postscript figure^(^<A HREF^="%%P_Curve.eps"^>%%P_Curve.eps^</A^>^)^<BR^>  >> !RESULTS_DIR!\index.html
    echo ^<IMG SRC^="%%P_Curve.png"^>  >> !RESULTS_DIR!\index.html
    echo ^<HR^> >> !RESULTS_DIR!\index.html
)

echo ^<P^>^<H3^>Permute Results:^</H3^>^<BR^> >> %RESULTS_DIR%\index.html
echo Summary ^(^<A HREF^="Perm_Rank1.html"^>Perm_Rank1.html^</A^>^)^<BR^>^<BR^>  >> %RESULTS_DIR%\index.html

for %%A in (%basenames%) do (
    echo Recognition rate curves with error bounds. ^(^<A HREF^="Perm\%%A_CMCurve.txt"^>Perm%%A_CMCurve.txt^</A^>^)^<BR^>  >> %RESULTS_DIR%\index.html
    echo Histogram data of recognition rate counts. ^(^<A HREF^="Perm\%%A_HistCounts.txt"^>Perm%%A_HistCounts.txt^</A^>^)^<BR^>  >> %RESULTS_DIR%\index.html
    echo Histogram data of recognition rate probablities. ^(^<A HREF^="Perm\%%A_HistProbs.txt"^>Perm%%A_HistProbs.txt^</A^>^)^<BR^>^<BR^>  >> %RESULTS_DIR%\index.html
)

echo ^</BODY^>^</HTML^> >> %RESULTS_DIR%\index.html

goto END

:INCORRECTPATH
echo Please run this command from the csuEvalFaceRec directory
:END