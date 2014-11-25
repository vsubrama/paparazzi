#!/bin/bash

export RLIMIT_RTTIME=60

echo "> the following script will need root right!\n"
echo "> Do you want to recompile the executable, "y/n"(default:n)"
read IS_RECOMPILE
#we need to configure the fiji compile to lift the max threads to 2000 for exp
#we also need to use "ulimit -s 2014" to increase the stack size for thread creation.
if [ "$IS_RECOMPILE" = "y" ] || [ "$IS_RECOMPILE" = "Y" ]; then
	cd ./output/jar/
	/home/gassa/research/fivm/bin/fivmc --rt-library=NONE -o juav-desktop \
    jpapabench-core-flightplans.jar  jpapabench-core.jar  jpapabench-pj.jar \
    --max-threads=2000 --32
	echo "complete compile. \n"
	cd ../..
fi
