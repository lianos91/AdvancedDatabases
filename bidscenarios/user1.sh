#!/bin/bash

coproc ./$1.sh

./advdbproj/bin/bidder `cat ./auctioneer1` $2 <&${COPROC[0]} >&${COPROC[1]}

wait $COPROC_PID
