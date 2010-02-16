#!/bin/sh
PATH=$PATH:bin/`arch`
export PATH
run() {
    $@ || { echo Command failed: $@; echo; exit 1; }
}

# Using normalized raster images from directory <data/csuScrapShots/normSep2002sfi>


# Creating directory for training data.

run mkdir -p train/scraps

# Running standard feret PCA training.

run csuSubspaceTrain -lpp -t 2 -k 4 -imDir data/csuScrapShots/normSep2002sfi imagelists/scrap_all.srt train/scraps/feretPCA.train

# Clearing out old distances and/or creating new distances directory

rm -rf distances/scraps/LPP_Euclidean
run mkdir -p distances/scraps/LPP_Euclidean

# Projecting face images onto LPP basis and computing distances

run csuSubspaceProject -imDir data/csuScrapShots/normSep2002sfi train/scraps/feretPCA.train imagelists/scrap_all_x4.srt distances/scraps/LPP_Euclidean Euclidean
run echo 'Indicator file used by the scripts to know whether the data in this directory is valid' > distances/scraps/lppDataExists.txt
