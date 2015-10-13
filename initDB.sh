#!/bin/bash

psql -d advdb1 -c "
CREATE TABLE ITEMS(
itemID INT NOT NULL PRIMARY KEY,
description VARCHAR(255) NOT NULL,
curr_bid INT NOT NULL,
curr_bidder VARCHAR(20) );
"

psql -d advdb2 -c "
CREATE TABLE ITEMS(
itemID INT NOT NULL PRIMARY KEY,
description VARCHAR(255) NOT NULL,
curr_bid INT NOT NULL,
curr_bidder VARCHAR(20) );
"

