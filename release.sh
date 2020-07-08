#!/bin/bash

border () {
	local str="$*"      # Put all arguments into single string
	local len=${#str}
	local i
	for (( i = 0; i < len - 12; ++i )); do
		printf '='
	done
	printf "\n| $str |\n"
	for (( i = 0; i < len - 12; ++i )); do
		printf '='
	done
	echo
}

make distclean
cmake -DCMAKE_BUILD_TYPE=Release .
make -j4
cpack -G DEB

if ls *.deb; then
	echo
	RED='\033[0;31m'
	NC='\033[0m'
	VAL="${RED}`ls --color=never *.deb`${NC}"
	border "Created: ${VAL} — `ls -lha *.deb | awk -F " " {'printf $5'}`"
	echo
fi
