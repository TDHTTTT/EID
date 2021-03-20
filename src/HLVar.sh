#!/usr/bin/env bash
source ./init.sh
root -l ./src/HLVar.C
mkdir -p ./output/png/HL ./output/csv/HL
mv *.png ./output/png/HL
mv *.csv ./output/csv/HL
