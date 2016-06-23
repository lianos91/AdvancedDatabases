#advdb

This project is an auction application. 

The front-end, i.e a bidding application, connects to the back-end, i.e an auction application which manages the user-server communication (e.g. new bid, inspect current item, drop from auction etc.). 

The backend storage makes use of postgreSQL database system. Two separate database servers are running, and each bidder connects to one of them. The consistency and synchronisation of the backend databases is carried out by a thirdserver in a gossip-like fashion. Each bidder and server runs in a separate thread, to simulate a real multi-node application. The current project is implemented in C++.

##Execution

Before doing anything change your directory to /advdbproj and compile the code by running "make".

--------------------------------------------------------------------------------------------------------------------------------------------
Run

    ./initDB.sh
    
before the first time you run the auction server. You should have previously created two databases called "advdb1" and "advdb2"
under the user "postgres" with password "postgres". More info is included in the documentation.
--------------------------------------------------------------------------------------------------------------------------------------------
Run

    ./driver.sh <arg>

with argument

    many_conn	: many bidders are connecting on both auctioneers. no biddings.
    
    dup_name	: some duplicate names are given. no biddings.
    
    run_2b_1a	: 2 bidders on one auctioneer. The bids are made in a way that the initial
               	  price is reduced by the auctioneer and then occurs the bidding from users
                
    run_4b_2a 	: 4 bidders, 2 on each auctioneer. All bidders are interested and bidding.

    run_6b_2a 	: 6 bidders, 3 on each auctioneer. One duplicate name. Two are not interested
		          on all items.
--------------------------------------------------------------------------------------------------------------------------------------------
Run
    
    ./clean.sh
    
if you wish to clear the output files from a previous execution. The script driver.sh cleans the output files of a previous execution
before running again.
