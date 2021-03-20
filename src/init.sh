#!/usr/bin/env bash
# source init.sh

export DELPHES=/c/Users/tdhttt/workspace/hep/delphes/Delphes-3.4.1
export LD_LIBRARY_PATH=$DELPHES:$LD_LIBRARY_PATH
export ROOT_INCLUDE_PATH=./:$DELPHES:$DELPHES/external:$ROOT_INCLUDE_PATH
