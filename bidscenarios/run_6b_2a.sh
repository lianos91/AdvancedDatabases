#!/bin/bash

gnome-terminal --tab -t "tom" -e "/bin/bash -c './bidscenarios/user1.sh bidscenarios/bidders/6b_2a_u1 tom; read -n1'" --tab -t "rain" -e "/bin/bash -c './bidscenarios/user1.sh bidscenarios/bidders/6b_2a_u2 rain; read -n1'" --tab -t "manu" -e "/bin/bash -c './bidscenarios/user1.sh bidscenarios/bidders/6b_2a_u3 manu; read -n1'"
gnome-terminal --tab -t "dylan" -e "/bin/bash -c './bidscenarios/user2.sh bidscenarios/bidders/6b_2a_u4 dylan; read -n1'" --tab -t "sara" -e "/bin/bash -c './bidscenarios/user2.sh bidscenarios/bidders/6b_2a_u5 sara; read -n1'" --tab -t "rain" -e "/bin/bash -c './bidscenarios/user2.sh bidscenarios/bidders/6b_2a_u6 rain; read -n1'"

