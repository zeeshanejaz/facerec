#!/bin/sh

########## PREAMBLE #########

if [ ! -f scripts/runPreprocessing_scraps.sh ]; then
    echo "Please run this command from the csuEvalFaceRec directory"
    exit 1
fi

run() {
    $@ || { echo "Command failed: $@"; echo; exit 1; }
}

###### End of PREAMBLE ######

PATH=$PATH:bin/`arch`
export PATH

SCRAPS_PGM_SOURCE="data/csuScrapShots/source/pgm"
SCRAPS_SFI_DIR="data/csuScrapShots/normSep2002sfi"
SCRAPS_NRM_DIR="data/csuScrapShots/normSep2002nrm"
SCRAPS_PGM_DIR="data/csuScrapShots/normSep2002pgm"
EYE_COORDS="imagelists/coords.scraps"

mkdir -p $SCRAPS_SFI_DIR $SCRAPS_NRM_DIR $SCRAPS_PGM_DIR

echo "Running Preprocessing on scrapshots images. Using CSU Defaults for Optional Settings"
run csuPreprocessNormalize $EYE_COORDS $SCRAPS_PGM_SOURCE -sfi  $SCRAPS_SFI_DIR -nrm $SCRAPS_NRM_DIR -pgm $SCRAPS_PGM_DIR
