#!/bin/bash

#Desired Number of Connections
NA=7
NB=7

# Starting Bidders to connect to either
# Auctioneer 1 or to Auctioneer 2
gnome-terminal --tab -t "auct1connections" -e "/bin/bash -c './bidscenarios/runBidders.sh 0 $((NA - 1)) auctioneer1'"
gnome-terminal --tab -t "auct2connections" -e "/bin/bash -c './bidscenarios/runBidders.sh $((NA)) $((NA + NB - 1)) auctioneer2'"
