#!/usr/bin/env bash
source ./init.sh
root -l ./src/finde.C'("'$1'")'
mkdir -p ./output/png ./output/csv
mv *.png ./output/png
mv *.csv ./output/csv
