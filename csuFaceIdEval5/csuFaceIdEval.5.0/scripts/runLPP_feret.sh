#!/bin/sh
PATH=$PATH:bin/`arch`
export PATH
run() {
    $@ || { echo Command failed: $@; echo; exit 1; }
}

# Using normalized raster images from directory <data//FERET/normSep2002sfi>


# Creating directory for training data.

run mkdir -p train/feret

# Running feret LPP training.

run csuSubspaceTrain -lpp -t 20000 -k 5 -imDir data/FERET/normSep2002sfi imagelists/feret_training.srt train/feret/feretLPP.train -writeTextInterm

# Clearing out old distances and/or creating new distances directory

rm -rf distances/feret/LPP_Euclidean
run mkdir -p distances/feret/LPP_Euclidean

# Projecting face images onto LPP basis and computing distances

run csuSubspaceProject -imDir data/FERET/normSep2002sfi train/feret/feretLPP.train imagelists/all.srt distances/feret/LPP_Euclidean Euclidean
run echo 'Indicator file used by the scripts to know whether the data in this directory is valid' > distances/feret/lppDataExists.txt
