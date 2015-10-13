#!/bin/bash

coproc ./$1.sh

./advdbproj/bin/bidder `cat ./auctioneer2` $2 <&${COPROC[0]} >&${COPROC[1]}

wait $COPROC_PID
