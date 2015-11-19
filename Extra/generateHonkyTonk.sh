#!/bin/sh

source ../Design/functions.sh

OUTPUT=honkyTonky.cc

transformDesign designToHonkyTonk.xslt $OUTPUT 0 1 
if [ $? -ne 0 ]; then 
	exit 1 
fi
