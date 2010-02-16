@echo off
if not %0==scripts\runPreprocessing_scraps.bat goto INCORRECTPATH

set SCRAPS_PGM_SOURCE="data\csuScrapShots\source\pgm"
set SCRAPS_SFI_DIR="data\csuScrapShots\normSep2002sfi"
set SCRAPS_NRM_DIR="data\csuScrapShots\normSep2002nrm"
set SCRAPS_PGM_DIR="data\csuScrapShots\normSep2002pgm"
set EYE_COORDS="imagelists\coords.scraps"

rd /S /Q %SCRAPS_SFI_DIR% %SCRAPS_NRM_DIR% %SCRAPS_PGM_DIR%
md %SCRAPS_SFI_DIR% %SCRAPS_NRM_DIR% %SCRAPS_PGM_DIR%

echo "Running Preprocessing on scrapshots images. Using CSU Defaults for Optional Settings"
csuPreprocessNormalize.exe %EYE_COORDS% %SCRAPS_PGM_SOURCE% -sfi  %SCRAPS_SFI_DIR% -nrm %SCRAPS_NRM_DIR% -pgm %SCRAPS_PGM_DIR%

goto END

:INCORRECTPATH
echo Please run this command from the csuEvalFaceRec directory
:END