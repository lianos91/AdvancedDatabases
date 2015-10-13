#include "SynchrServer.h"
#include "MiscLib.h"
#include "Auctioneer.h"
#include "CommunicationService.h"
#include "logger.h"

#include <time.h>
#include <sys/time.h>

#include <string>
#include <iostream>       // std::cout
#include <thread>         // std::thread
#include <mutex>
#include <sstream>       // std::cout
#include <exception>

#include "control.h"

using namespace std;

SynchrServer::SynchrServer(int _numAucts)
{
    //ctor
    this->numAucts = _numAucts;
}

SynchrServer::~SynchrServer()
{
    //dtor
}

void SynchrServer::setActiveItem(int _id){
    activeItemID = _id;
    return;
}

int SynchrServer::getActiveItem(){
    return activeItemID;
}

bool SynchrServer::adduser(string name){
    for(vector<string>::iterator it = allusers.begin();it<allusers.end();it++){
        if (name.compare(*it)==0){
            return false;
        }
    }
    allusers.push_back(name);
    return true;
}

bool SynchrServer::setMax(string Bidder,int Bid){
    bool ret = false;
    if (Bid > maxBid){
        maxBidder = Bidder;
        maxBid = Bid;
        ret = true;
    }
    return ret;
}

void SynchrServer::start(){

    //Create log file
    Logger = new logger("SyncServer");
    comm = new CommunicationService(this);
try{
    //.1 Establish connection with AuctionServers
    cout << "SynchrServer: waiting for auctioneers to connect"<< endl;
    auctSockets = comm->incomingConnSetup(3,numAucts);
    if (auctSockets.size() < numAucts){
        Logger->log("not all required auctioneers connencted. Exiting...");
        throw new exception;
    }
    Logger->log("Auctioneers connected");

    gettimeofday(&tv[0],nullptr);
    gettimeofday(&tv[1],nullptr);

    finishedAuctions = 0;

    for(;;){

        comm->listenClients(4);

        gettimeofday(&synctv,nullptr);
        //find min time difference between aucts and sync
        int sec = (tv[0].tv_sec > tv[1].tv_sec) ? tv[0].tv_sec : tv[1].tv_sec;
        int synctdiff = synctv.tv_sec - sec;
        cout<<"sync__seconds diff of sync-auctioneers "<<synctdiff<<endl;
        if (synctdiff > 20){
            //if a lot of time has passed since last update, probably they are dead.
            Logger->log("Auctioneers probably dead. Exiting...");
            quit();
            return;
        }

        double aucttdiff = abs(((double)tv[0].tv_sec*1000000 + tv[0].tv_usec) -
                            ((double)tv[1].tv_sec*1000000 + tv[1].tv_usec)); //time between the 2 auctioneers.
        aucttdiff = aucttdiff/1000000.0;
        cout<<"sync__seconds diff of auctioneers: "<<aucttdiff<<endl;


        if ((aucttdiff > 9.0)&&(msgPending.compare("") !=0 )){
        //if a lot of time has passed since the last message of the other auctioneer (isdead)
            Logger->log("The Winner of item_"+to_string(activeItemID)+" is "+maxBidder);
            updateAuctioneers(msgPending);
            msgPending = "";
            activeItemID = -1;
            finishedAuctions = 0;
            numAucts--;
            maxBid = -1;
            maxBidder = "no_holder";
        }

        updateAuctioneers(createMessage(TICK,-1)); //is alive
    }
    //.2 Listen: for users and adduser

    //.3 Listen for activeID and setActiveItem

    //.4 Listen for bids and setMax

    //.5 Listen for end and declare winner to both
    quit();

 }catch (...){
    Logger->log("Exception was thrown. Auction is over");
    quit();

 }

    return;
}

int SynchrServer::getListeningPort(){
    return 7000;
}

void SynchrServer::quit(){
    delete(Logger);
    delete(comm);
}
///////////////////////////

int SynchrServer::handleMessage(string inputMessage, int socket){
    vector<string> tokensVec = MiscLib::split(inputMessage,':');

    //update timestamps
    int s = 11;
    if (auctSockets[0] == socket)
        s = 0;
    else if (auctSockets[1] == socket)
        s = 1;
    gettimeofday(&tv[s],nullptr);


    int command = stoi(tokensVec[0]);

    switch (command) {
        case AUCTION_COMPLETE:
            //exit gracefully
            Logger->log("Auction complete. Exiting..");
            quit();
            exit(1);
            break;
        case BID_ITEM:
            Logger->log("From Auct: Interest phase of item "+tokensVec[1]+" auction...");
            // the beginning of an item auction
            activeItemID = stoi(tokensVec[1]);
        //    if (maxBid==-1){ //no one has entered Interest_phase for this item
         //       maxBid = stoi(tokensVec[3]);
           // }
            //Interest phase: no_holder maxbidder initially
            //finishedAuctions = 0;
            break;
        case START_BIDDING:
            Logger->log("From Auct: Beginning of item "+tokensVec[1]+" auction...");
            // the beginning of an item auction

            break;
        case CONNECT:{
            bool isnewname = adduser(tokensVec[1]);
            if (!isnewname){
                Logger->log("User "+tokensVec[1]+" already existing");
                comm->sendMessage(createMessage(NAME_EXISTS,stoi(tokensVec[2])),socket);
            }
            else {
                Logger->log("User connection accepted. Replying to auct");
                int r = comm->sendMessage(inputMessage,socket);
                cout << "sync sent:"<<r<<" msg: "+inputMessage<<endl;
            }
            break;
        }
        case NEW_HIGH_BID: {
            string bidder = tokensVec[2];
            int newbid; istringstream(tokensVec[1]) >> newbid;
            bool updated = setMax(bidder,newbid);
            if (updated){
                //this bid was the highest of the two, so update the OTHER
                Logger->log("From Auct: New_high_bid by"+bidder+", val:"+tokensVec[1]);
                updateAuctioneers(inputMessage);
            }
            break;
        }
        case AUCT_ITEM_DONE: {
            // auction finished for this auctioneer. Gather from the others and declare winner
            Logger->log("From Auct: ITEMDONE Msg for item "+to_string(activeItemID));
            finishedAuctions = finishedAuctions + 1;
            cout << "sync__finished auctions: "+to_string(finishedAuctions) << endl;
      if (finishedAuctions < numAucts){
        msgPending = createMessage(STOP_BIDDING,-1);
        break;
        }

            if (activeItemID == -1) //the other auctioneer triggered the message.
                break;
            Logger->log("Winner of item_"+to_string(activeItemID)+" is "+maxBidder);
            updateAuctioneers(createMessage(STOP_BIDDING,-1));
            finishedAuctions = 0;
            activeItemID = -1;
            msgPending = "";
            maxBid = -1;
            maxBidder = "no_holder";
        break;
        }
        case TICK:
            //dummy, to update timestamp and declare that is alive
        break;
        default:
            Logger->log("Unkown message:"+inputMessage+" received,ignored");
            cout << "UNKNOWN MESSAGE:"+inputMessage<<endl;
    }
    return 0;
}

void SynchrServer::updateAuctioneers(string message){
    int isalive;
    for (auto e : auctSockets){
        cout << e<<endl;
    }
    vector<int>::iterator it = auctSockets.begin();

    while ( it != auctSockets.end() ){
        isalive = comm->sendMessage(message,*it); //DIES IN 2ND WRITE, AFTER WIN.
        if (isalive==EXIT_FAILURE){
            //it = auctSockets.erase(it);
            it++;
            //numAucts--;
            //Logger->log("An auctioneer disconnected. numAucts="+to_string(numAucts));
        }
        else{
            it++;
        }
    }
}
/* : separated fields*/
string SynchrServer::createMessage(int todo,int targetSocket){

    string resMsg = "?";
    switch (todo) {
//---------------------------------------------------------------------
        case STOP_BIDDING: //Message to declare winner
            resMsg = to_string(todo)+":"+maxBidder+":"+to_string(maxBid)+":"+to_string(activeItemID);

            break;
//-----------------------------------------------------------------------
        case START_BIDDING:
         //   resMsg = to_string(todo)+":"+to_string(itemID);
            break;
//-----------------------------------------------------------------------
        case AUCTION_COMPLETE:
            resMsg = to_string(todo);
            break;
//-----------------------------------------------------------------------
        case NAME_EXISTS:
            resMsg = to_string(todo)+":"+to_string(targetSocket);
            break;
//-----------------------------------------------------------------------
        case SUCCESS:
            resMsg = to_string(todo);
//-----------------------------------------------------------------------
        case FAILURE:
            resMsg = to_string(todo);
        case TICK:
            resMsg = to_string(todo);
    }

   return resMsg;

}

int main(){

    SynchrServer* Sync = new SynchrServer(2);
    Sync->start();

    delete(Sync);

    return 1;
}
