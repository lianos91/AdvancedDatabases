#!/bin/bash

gnome-terminal --tab -t "tom" -e "/bin/bash -c './bidscenarios/user1.sh bidscenarios/bidders/4b_2a_u1 tom; read -n1'" --tab -t "rain" -e "/bin/bash -c './bidscenarios/user1.sh bidscenarios/bidders/4b_2a_u2 rain; read -n1'"
gnome-terminal --tab -t "dylan" -e "/bin/bash -c './bidscenarios/user2.sh bidscenarios/bidders/4b_2a_u3 dylan; read -n1'" --tab -t "sara" -e "/bin/bash -c './bidscenarios/user2.sh bidscenarios/bidders/4b_2a_u4 sara; read -n1'"

