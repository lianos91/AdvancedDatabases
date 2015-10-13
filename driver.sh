#!/bin/bash

tput setaf 2;
echo
echo $'National Technical University of Athens,'
echo $'School of Electrical and Computer Engineering'
tput setaf 1;
echo
echo $'Advanced Topics in Database Systems (Semester Assignment):'
echo $'Auction Server with two Auctioneers and Multiple Bidders'
tput setaf 3;
echo
echo $'Authors:'
echo $'Konstantinos-Nektarios Lianos	<03109054> <kn_lianos@hotmail.com>'
echo $'Konstantinos Railis		<03109118> <krailis@hotmail.com>'
echo
tput setaf 7;

./clean.sh

# Starting Synchronization Server
gnome-terminal --tab -t "Synchronization" -e "/bin/bash -c './advdbproj/bin/SyncrServer; read -n1'"

# Starting Auctioneers
gnome-terminal --tab -t "Auctioneers" -e "/bin/bash -c './bidscenarios/runAuct.sh; read -n1'"
sleep 1

# Executing scenario
./bidscenarios/$1.sh
