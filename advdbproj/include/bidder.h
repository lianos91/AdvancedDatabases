#ifndef BIDDER_H
#define BIDDER_H

#include <string>
#include "AbstractServer.h"

using namespace std;

class Bidder : public AbstractServer {
    public:
        Bidder(string, int, string);

        void getUsrMsg(void);
        void handleUsrMsg(string);
        int getListeningPort(){ return this->port; }
        int getListeningSocket(){ return this->sockfd; }

        int handleMessage(string, int);
        string createMessage(int, int);
        void start();
        void quit();

        int sockfd;

    protected:

    private:
        bool itemExists = false; //If an items exists under bidding
        int itemID;              // The item under bidding
        int amount;
        string holder = "no_holder"; //holder of item
        bool bidExists = false;      //if a bidding has been do
        bool startBidding = false;   //
        string itemDescription = "";
        int highBid;                //highest bid of item under bidding
        string host;
        int port;
        string name;

        int auctionPhase = NO_PHASE;
};

#endif // BIDDER_H
