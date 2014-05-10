#!/bin/bash
shopt -s nullglob
cd build/native/
for script in *.test
do
	if [[ -x "$script" ]]; then
		script2=`echo $script | sed -E s/\.test$//`
		echo " TEST $script2"
		./"$script" || exit 1
	fi
done