#ifndef SYNCHRSERVER_H
#define SYNCHRSERVER_H

#include <string>
#include <vector>
#include <map>

#include "AbstractServer.h"

class SynchrServer : public AbstractServer
{
    public:
        SynchrServer(int);
        virtual ~SynchrServer();
        void setActiveItem(int);
        int getActiveItem();
        std::vector<int> auctSockets; // store (auctioneerID,port)
        bool adduser(std::string);
        void updateAuctioneers(std::string);

        void start();
        void quit();
        int getListeningPort();
        int handleMessage(std::string,int);

    protected:

    private:
        std::string createMessage(int,int);
        bool setMax(std::string,int);

        int numAucts; //number of auctioneers to synchronise
        std::string maxBidder="no_holder";
        float maxBid=-1;
        int activeItemID =-1; //the item under bidding
        std::vector<std::string> allusers;
        int finishedAuctions;

        std::string msgPending = "";

        struct timeval tv[2]; //timestamp for auctioneer1 and auctioneer2
        struct timeval synctv; //timestamp for synchronisationServer
};

#endif // SYNCHRSERVER_H
