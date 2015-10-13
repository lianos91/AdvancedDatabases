#!/bin/bash

gnome-terminal --tab -t "tom" -e "/bin/bash -c './bidscenarios/user1.sh ./bidscenarios/bidders/2b_1a_u1 tom; read -n1'"
gnome-terminal --tab -t "rain" -e "/bin/bash -c './bidscenarios/user1.sh ./bidscenarios/bidders/2b_1a_u2 rain; read -n1'"
