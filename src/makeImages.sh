#!/usr/bin/env bash
source ./init.sh
root -l ./src/makeImages.C'("'$1'")'
mkdir -p ./output/png/LL ./output/csv/LL
mv *.png ./output/png/LL
mv *.csv ./output/csv/LL
