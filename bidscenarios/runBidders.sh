#!/bin/bash

Users=('john' 'adam' 'mike' 'bob' 'mary' 'nathan' 'lucas' 'hailley' 'cloud' 'riku' 'kairi' 'sora' 'takis' 'mitsos')
tab="--tab"
foo=""

for I in $(seq $1 $2)
do
	foo+=($tab -t ${Users[I]} -e "/bin/bash -c './advdbproj/bin/bidder `cat ./$3` ${Users[I]}; read -n1'")
done

gnome-terminal -t "Bidders" "${foo[@]}"
