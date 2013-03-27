#!/usr/bin/env bash

cd "$(dirname "$0")"

maxCpu=4
maxThread=64

for (( i = 1; i < $maxCpu; ++i )); do
	if [[ $i != 1 ]]; then
		printf " "
	fi
	printf "$i"
done
for (( i = $maxCpu; i <= $maxThread * $maxCpu; i *= 2 )); do
	printf " $i"
done
printf "\n"

for (( x = 0; x < $4; ++x )); do
	if [[ $x != 0 ]]; then
		printf ";\n"
	fi
	for (( i = 1; i < $maxCpu; ++i )); do
		if [[ $i != 1 ]]; then
			printf " "
		fi
		printf "%s" "`./$1 $2 $3 1 $i | env LC_ALL=C awk '{s+=$0} END {print s}'`"
	done
	for (( i = 1; i <= $maxThread; i *= 2 )); do
		printf " %s" "`./$1 $2 $3 $i $maxCpu | env LC_ALL=C awk '{s+=$0} END {print s}'`"
	done
done
printf "\n"
