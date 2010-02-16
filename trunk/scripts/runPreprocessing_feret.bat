@echo off
if not %0==scripts\runPreprocessing_feret.bat goto INCORRECTPATH

set FERET_PGM_SOURCE=data\FERET\source\pgm
set FERET_SFI_DIR=data\FERET\normSep2002sfi
set FERET_NRM_DIR=data\FERET\normSep2002nrm
set FERET_PGM_DIR=data\FERET\normSep2002pgm
set EYE_COORDS=imagelists\coords.3368

rd /S /Q %FERET_SFI_DIR% %FERET_NRM_DIR% %FERET_PGM_DIR%
md %FERET_SFI_DIR% %FERET_NRM_DIR% %FERET_PGM_DIR%

echo Running Preprocessing on scrapshots images. Using CSU Defaults for Optional Settings
csuPreprocessNormalize.exe %EYE_COORDS% %FERET_PGM_SOURCE% -sfi  %FERET_SFI_DIR% -nrm %FERET_NRM_DIR% -pgm %FERET_PGM_DIR%

goto END

:INCORRECTPATH
echo Please run this command from the csuEvalFaceRec directory
:END