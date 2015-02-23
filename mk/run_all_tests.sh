#!/bin/bash

red='\033[0;31m'
green='\033[0;32m'
NC='\033[0m'

function silence_cmd
{
	./"$1" 
	#> /dev/null 2> /dev/null
	#./"$0" > /dev/null 2> /dev/null
}

shopt -s nullglob
cd build/native/
for script in *.test
do
	if [[ -x "$script" ]]; then
		script2=`echo $script | sed -E s/\.test$//`
		silence_cmd "${script}" && echo -e "${green}TEST ${script2} - correct" || echo -e "${red}TEST ${script2} - error"
		printf ${NC}
	fi
done
