#!/bin/bash

# Desired Number of Bidders
NA=4
NB=4

# Starting Bidders to connect to either
# Auctioneer 1 or Auctioneer 2
gnome-terminal --tab -t "auct1connections" -e "/bin/bash -c './bidscenarios/runBidders.sh 0 $((NA - 1)) auctioneer1'"
gnome-terminal --tab -t "auct2connections" -e "/bin/bash -c './bidscenarios/runBidders.sh $((NA)) $((NA + NB - 1)) auctioneer2'"

# Some users attempt to connect with names already used
gnome-terminal --tab -e "/bin/bash -c './bidscenarios/runBidders.sh 0 1 auctioneer2'"
gnome-terminal --tab -e "/bin/bash -c './bidscenarios/runBidders.sh 2 3 auctioneer1'"

