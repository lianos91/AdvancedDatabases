#include "RegTable.h"
#include "MiscLib.h"

using namespace std;

RegTable::RegTable()
{
    //ctor
}
RegTable::~RegTable() {

}

void RegTable::setActiveItem(int _id){
    activeItemID = _id;
    return;
}
int RegTable::getActiveItem(){
    return activeItemID;
}

string RegTable::getName(int socket){
    return ConnectedBidders.find(socket)->second;
}

void RegTable::removeBidder(int socket){

    ConnectedBidders.erase(socket);
    InterestedBidders.erase(socket);
    return;
}

int RegTable::insertConnectedBidder(string name,int socket){
    if (checkNameExists(name)){
        return 0;
    }
    if (ConnectedBidders.count(socket) == 0){ //if same socket does not exists
        ConnectedBidders.insert(make_pair(socket,name));
        return 1;
    }
    return 0;

    //if successful return 1
    //else 0, if user already existed
}

int RegTable::insertInterested(int socket){
    int ret = 0;
    if (!checkNameExists(ConnectedBidders.find(socket)->second)){
        return 0;
    }
    if (InterestedBidders.count(socket) == 0){ //if item does not exists
        InterestedBidders.insert( make_pair(socket,ConnectedBidders.find(socket)->second) );
        ret = 1;
    }
    return ret;
    //if successful return 1
    //else 0, if user already active
}

void RegTable::clearInterested(){
    InterestedBidders.clear();
}

map<int,string> RegTable::getInterested(){
    map<int,string> m2;
    m2.insert(InterestedBidders.begin(),InterestedBidders.end());
    return m2;
}

map<int,string> RegTable::getConnected(){
    map<int,string> map2;
    map2.insert(ConnectedBidders.begin(),ConnectedBidders.end());
    return map2;
}

int RegTable::setBid(int itemID,int value){

    if (ItemBids.count(itemID) == 0){ //if item does not exists
        ItemBids.insert(make_pair(itemID,value));
        return 1;
    }
    else{ //item exists
        ItemBids[itemID] = max(ItemBids[itemID],value);
        return value > ItemBids[itemID];
    }
    //if it was the higher return 1
    //else 0
}

void RegTable::setSyncSocket(int _socket){
    syncSocket = _socket;
}

int RegTable::getSyncSocket(){
    return syncSocket;
}

void RegTable::printAll(){
    //MiscLib::printMap<int,string>(ConnectedBidders);
   // MiscLib::printMap<int,string>(ActiveBidders);
   // MiscLib::printMap<int,int>(ItemBids);
    cout << "Connected Bidders\n";
    MiscLib::printMap(ConnectedBidders);
    cout << "Interested Bidders\n";
    MiscLib::printMap(InterestedBidders);
    cout << "Item Bids\n";
    MiscLib::printMap(ItemBids);
}

bool RegTable::checkNameExists(string name){

    bool name_exists = false;
    for(map<int,string>::iterator it = ConnectedBidders.begin(); it!=ConnectedBidders.end(); it++){
        if (name.compare(it->second)==0){
            name_exists = true;
            break;
        }
    }
    return name_exists;
}

