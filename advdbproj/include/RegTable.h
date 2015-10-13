#ifndef REGTABLE_H
#define REGTABLE_H


#include <string>
#include <map>
#include <mutex>

class RegTable
{
    public:
        RegTable();
        virtual ~RegTable();
        int insertConnectedBidder(std::string,int);
        int insertInterested(int);
        void removeBidder(int);
        int setBid(int,int);
        void setActiveItem(int);
        int getActiveItem();
        void printAll();
        void clearInterested();
        std::map<int, std::string> getInterested();
        std::map<int, std::string> getConnected();
        std::string getName(int);
        int getSyncSocket();
        void setSyncSocket(int);
        int maxBid = -1;
        std::string maxBidder="";

    protected:
    private:
        bool checkNameExists(std::string);
        std::map<int, std::string> ConnectedBidders; //<BidderName,socket>
        std::map<int, std::string> InterestedBidders; //<BidderName,socket>
        std::map<int, int> ItemBids;            //<ItemID,value>
        int activeItemID =-1; //the item under bidding
        int syncSocket;
};

#endif // REGTABLE_H
