#ifndef CONTROL_H_INCLUDED
#define CONTROL_H_INCLUDED

// Control Messages Sent by Bidder & Received By Auctioneer

#define CONNECT             0
#define I_AM_INTERESTED     1
#define MY_BID              2
#define QUIT                3

// Control Messages Sent by Auctioneer & Received By Bidder

#define BID_ITEM            4
#define START_BIDDING       5
#define NEW_HIGH_BID        6
#define STOP_BIDDING        7
#define AUCTION_COMPLETE    8
#define DUPLICATE_NAME      9
#define WAITING_WINNER      15


// Control Messages Sent by Synchronisation Server & Received by Auctioneer
#define NAME_EXISTS         10
#define DECLARE_WINNER      11
#define AUCT_ITEM_DONE      14

//Other

#define TICK                16

#define FAILURE             12
#define SUCCESS             13
#endif // CONTROL_H_INCLUDED
