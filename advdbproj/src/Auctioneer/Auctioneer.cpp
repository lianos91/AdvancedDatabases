/*
 * Auctioneer.cpp
 *
 *  Created on: Mar 20, 2015
 *      Authors: Lianos Konstantinos-Nektarios
 *               Raillis Konstantinos
 */

#include "Auctioneer.h"
#include "CommunicationService.h"
#include "MiscLib.h"
#include "AbstractDataLayer.h"
#include "DBlayer.h"

#include "AbstractServer.h"
#include "control.h"
#include "logger.h"

#include <string>
#include <iostream>       // std::cout
#include <thread>         // std::thread
#include <vector>         //
#include <fstream>
#include <unistd.h>
#include <exception>

using namespace std;


string Auctioneer::conf = "";

/*
Reads the config file. If anything goes wrong, then returns 0. else 1
*/
int Auctioneer::readConf(){
    ifstream infile(Auctioneer::conf);
    if (!infile){
        cout << "Please specify an existing config file\n";
        return EXIT_FAILURE;
    }
    //open successful. read all lines and execute queries
    infile >> L;
    infile >> NumItems;
    infile.ignore();

    cout << "L,NumItems: "<< L << "," << NumItems << endl;
    if (!L||!NumItems){
        cout << "config file incorrect format\n";
        return EXIT_FAILURE;
    }
    string line;
    for(int itemid=1; itemid<=NumItems; itemid++){
        getline(infile, line);
        size_t posfound = line.find(" ");
        if (posfound==string::npos){
            cout << "wrong format\n";
            return EXIT_FAILURE;
        }
        else{
            int price = stoi(line.substr(0,posfound));
            string descr = MiscLib::ltrim(line.substr(posfound+1));
            //cout << price << "_" << descr << endl;

            //insert the item in the database
            Database->insertItem(itemid,descr,price);

            cout << " DB Records created successfully" << endl;
        }
    }
    return EXIT_SUCCESS;
}

///////////////////////////
/*
Handle the messages received from socket
Can Be from either Bidders or Synchronisation Server
*/
int Auctioneer::handleMessage(string input, int socket){
    vector<string> tokensVec = MiscLib::split(input,':');



    cout<< "auct"<<getID()<<" received message__"+input<<endl;



    int command = stoi(tokensVec[0]);
    cout<< "command:"<<command<<endl;
    switch (command) {
        case CONNECT:{
            //if CONNECT message received from Sync, then name should be added.
            if (socket == Table->getSyncSocket()){
                Logger->log("From:Sync. Bidder "+tokensVec[1]+" added at"+(tokensVec[2]));
                Table->insertConnectedBidder(tokensVec[1],stoi(tokensVec[2]));
                break;
            }
            //else send the connection request to Syncr Server to check for duplicates.
            //Can be done at any moment during the auction
            Logger->log("From:Bidder. User "+tokensVec[1]+" to add.Sending to Sync for validation");
            int r = comm->sendMessage(input+":"+to_string(socket),Table->getSyncSocket());
            cout << "ret code:" <<r << endl;
            break;
            }
        case I_AM_INTERESTED:
            if (auctionPhase == INTEREST_PHASE) {
                if (tokensVec.size() == 2){
                    if (stoi(tokensVec[1])==Table->getActiveItem()){
                        Logger->log("From:Client. " +Table->getName(socket)+ "interested");
                        Table->insertInterested(socket);
                    }
                    else{
                        cout << "Please specify the current ItemID to participate" << endl;
                    }
                }
                else{
                    cout << "Usage: i_am_interested <itemID>" << endl;
                }
            }
            break;
        case MY_BID:
            if (auctionPhase == BIDDING_PHASE) {
                int itemID; istringstream(tokensVec[2]) >> itemID;
                int newbid; istringstream(tokensVec[1]) >> newbid;

                if (itemID != Table->getActiveItem())
                    break;
                Logger->log("From:Client "+Table->getName(socket)+ ". Bid:"+to_string(newbid));
                string bidder = Table->getName(socket);


                bool updated = (newbid > Table->maxBid);
                cout << "auct__updated val?"<<updated<<endl;
                if(updated){
                    Table->maxBid = newbid;
                    Table->maxBidder = bidder;
                    //Update synchr server
                    string Hbid_msg = createMessage(NEW_HIGH_BID,itemID);
                    int r = comm->sendMessage(Hbid_msg,Table->getSyncSocket());
                    cout << "auct__sent to sync?"<<r<<endl;

                    updated_once = true;

                }

            }
            break;
        case NEW_HIGH_BID:
            //Message Received from Synchronisation Server
            Logger->log("From:Sync. New high bid. Forwarding to interested... ");
            Table->maxBid = stoi(tokensVec[1]);
            Table->maxBidder = (tokensVec[2]);
            updateBidders(input,Table->getInterested());

            updated_once = true;

        break;
        case QUIT:
            Logger->log("From:Client "+Table->getName(socket)+ ". Quit");
            Table->removeBidder(socket);
            return -1;
        case NAME_EXISTS:
            // if name exists in Syncr Server, send DUPLICATE_NAME
            comm->sendMessage(createMessage(DUPLICATE_NAME,-1),stoi(tokensVec[1]));
            break;
        case STOP_BIDDING:
        {
            //received from Sync and forwarded to all bidders
            Logger->log("From:Sync. Winner declared. Forwarding to interested... ");
            int itemID = stoi(tokensVec[3]);
            string maxbidder = tokensVec[1];
            int maxbid = stoi(tokensVec[2]);

            //end of auction for this item. Updating DB.....
            Database->updateItem(itemID,maxbidder,maxbid);

            updateBidders(input,Table->getInterested());
            auctionPhase = WINNER_DECLARED;
            break;
        }
        case TICK:
            //dummy
            break;
        default:
            string msg = createMessage(FAILURE, -1);
            Logger->log("Ignoring Unknown msg__"+msg);
    }
    return 0;
}


/* : separated fields*/
string Auctioneer::createMessage(int todo,int itemID){
//------------------------------------------------------------------
    string* fields = NULL;

    //retuns null if no item found (e.g. ID = -1)
    fields = Database->selectItem(itemID);
    //0:itemid,1:description,2:curr_bid,3:curr_bidder

    string resMsg = "?";
    switch (todo) {
        case BID_ITEM:
            Table->maxBid = stoi(fields[2]); //The initial
            Table->maxBidder = fields[3];
            resMsg = to_string(todo)+":"+to_string(itemID)+":"+fields[1]+":"+fields[2];
            break;
//--------------------------------------------------------------------
        case NEW_HIGH_BID:
            //Table->maxBid = stoi(fields[2]);
            resMsg = to_string(todo)+":"+to_string(Table->maxBid)+":"+Table->maxBidder;
            break;
//---------------------------------------------------------------------
        case AUCT_ITEM_DONE:
            cout<<"auct__creating auct item done"<<endl;
            resMsg = to_string(todo)+":"+to_string(itemID);
            break;
        case STOP_BIDDING:
            resMsg = to_string(todo)+":"+fields[3]+":"+to_string(itemID);
            break;
//-----------------------------------------------------------------------
        case START_BIDDING:
            resMsg = to_string(todo)+":"+to_string(itemID);
            break;
//-----------------------------------------------------------------------
        case AUCTION_COMPLETE:
            resMsg = to_string(todo);
            break;
//-----------------------------------------------------------------------
        case DUPLICATE_NAME:
            resMsg = to_string(todo);
            break;
//-----------------------------------------------------------------------
        case SUCCESS:
            resMsg = to_string(todo);
            break;
//-----------------------------------------------------------------------
        case FAILURE:
            resMsg = to_string(todo);
            break;
        case WAITING_WINNER:
            resMsg = to_string(todo);
        case TICK:
            resMsg = to_string(todo);
    }
    return resMsg;
}



/*
Send the appropriate message to all of its bidders
*/
void Auctioneer::updateBidders(string message,map<int,string> List){
    int sock;
    for(auto elem : List){
        sock = elem.first;
        std::cout << sock << "_(socket,usr)_" << elem.second << "\n";
        int retval = comm->sendMessage(message,sock);
        //if failed to send in a bidder (no socket), delete the user from the registration table
        if (retval == EXIT_FAILURE){
            std::cout << sock << "_(socket removing usr)_" << elem.second << "\n";
            Table->removeBidder(sock);
            Logger->log("User "+elem.second+" disconnected unexpectedly");
        }
    }
}

int Auctioneer::create_port_file(){
    this->listeningPort = 8000+getID();
    ofstream myfile;
    const char* filename = ("auctioneer"+to_string(getID())).c_str();
    myfile.open (filename,fstream::trunc); //replace existing contents
    myfile << "127.0.0.1 "+to_string(this->listeningPort)+"\n";
    myfile.close();
    return EXIT_SUCCESS;
}

/**TODO: Listening FOR loop INSIDE AUCT
    Needed for synchronisation and wait of the messages
 **/

void Auctioneer::start(){

//==INITIALIZATIONS============
    //-1 Open Log file
    Logger = new logger("auctioneer"+to_string(getID()));

try{
    //0.1. Create file with ports
    if (create_port_file()==EXIT_FAILURE)
        throw new exception;
    cout <<auctioneerID <<" Auct:: StartAuct "<< Auctioneer::conf << endl;

    //1. establish DB connection and empty DB table
    Database = new DBlayer("advdb"+to_string(getID()));
    Database->connect();

    //2. read configuration file and add items
    if (readConf()==EXIT_FAILURE){
        throw new exception;
    }
    //3. create new RegistrationTable BEFORE creating listening threads.
    Table = new RegTable();
    //4. create communication object
	comm = new CommunicationService(this);

    Logger->log("Initializations done. Connecting to Sync Server...");
	//4.05 connect to Sync Server, port 7000
	int SyncServSocket = comm->connectToServer(7000);
	if (SyncServSocket == EXIT_FAILURE){
        throw new exception;
    }
	Table->setSyncSocket(SyncServSocket);


    Logger->log("Sync connection done. Waiting for clients to connect...");
    //4.1 Accept Connections from Bidders
    if ((comm->incomingConnSetup(3,-5)).empty()){
        Logger->log("No clients connected. Exiting...");
        throw new exception;
    }
    Logger->log("client connections established");

    //5. wait for bidders to connect
    auctionPhase = CONNECTING_PHASE;
    Logger->log("Listening for connect messages...");

    comm->sendMessage(createMessage(TICK,-1),Table->getSyncSocket());
    comm->listenClients(L);

    if(Table->getConnected().empty()){
        Logger->log("No bidders connected. Exiting...");
        throw new exception;
    }
    Logger->log(to_string(Table->getConnected().size())+" Bidders connected. starting auction...");




//after L+3 SECONDS


    //6. start Bidding Process for each item in the DB
    for(int itemID=1; itemID<=NumItems; itemID++){
        cout <<"ItemID auctioned = "+to_string(itemID)<<endl;
        //6.0 reset InterestedBidders List
        Table->clearInterested();
        Table->setActiveItem(itemID);

        //6.1 send bid_item(..,..,..) message in all Connected bidders
        auctionPhase = INTEREST_PHASE;
        Logger->log("Show_interest phase. Message: BID ITEM "+to_string(itemID));
        updateBidders(createMessage(BID_ITEM,itemID),Table->getConnected());//+Log
        comm->listenClients(L);
        cout <<auctioneerID<<" interest phase over"<<endl;

        auctionPhase = NO_PHASE;
        //6.1.2 if no InterestedBidders, discard item
        map<int, string> BiddersList = Table->getInterested();


        Logger->log("Bidding phase...");
        //6.2 start bidding with BiddersList (so that new interested/connected users are ignored)
        comm->sendMessage(createMessage(BID_ITEM,itemID),Table->getSyncSocket());
        comm->sendMessage(createMessage(TICK,-1),Table->getSyncSocket());
        Logger->log("START_BIDDING message sent");
        auctionPhase = BIDDING_PHASE;

        if (BiddersList.empty()){
            Logger->log("No bidders interested. Unable to bid");
        }
    else{

        updateBidders(createMessage(START_BIDDING,itemID),BiddersList);

        //..6.3 do__while checking for new bids etc.
        updated_once = false; //CHANGED by NEW_HIGH_BID and MY_BID
        bool have_read = false;
        int noUpds=0;
        do{
        cout <<"AUCT___INSIDEEEE"<<endl;
            bool have_read = false;
        //for(int noUpds=0;noUpds<5;noUpds++){ //if 5 rounds with no update, then discard
            //LISTEN FOR BIDS
            comm->sendMessage(createMessage(TICK,-1),Table->getSyncSocket());
            have_read = (comm->listenClients(L)==EXIT_SUCCESS);

            if (!updated_once){ //if a value was read by sync server, firstUpd = true;
                noUpds++;
                have_read = true;
                // still no bid, so reduce value by 10%
                Logger->log("No bidding for "+to_string(L)+" seconds. Dropping value..");
                Database->updateItem(itemID);
                Table->maxBid = Database->getValue(itemID);
                Table->maxBidder = "no_holder";
                updateBidders(createMessage(NEW_HIGH_BID,itemID),BiddersList);
                updated_once=false;
            }
            else
                break;

        }while((noUpds < 5));
    }
        //6.4 when L seconds pass from a bid, send stop_bidding and go to next item.
        auctionPhase = NO_PHASE;
        //updateBidders(createMessage(STOP_BIDDING,itemID),BiddersList);
        Logger->log("Bidding for item finished. Waiting for Sync server to declare winner");
        comm->sendMessage(createMessage(AUCT_ITEM_DONE,itemID),Table->getSyncSocket());
        updateBidders(createMessage(AUCT_ITEM_DONE,itemID),Table->getConnected());

        //WAIT!!! to get winner message from syncr server.
        int c = 0;
        while (auctionPhase != WINNER_DECLARED){
            //cout << "Auct__sync sock:"<<Table->getSyncSocket()<<endl;
            int retval = comm->listenClients(1);
            comm->sendMessage(createMessage(TICK,-1),Table->getSyncSocket());

            if(retval==EXIT_FAILURE)
                Logger->log("auct Waiting..");
            if ((c+=retval==EXIT_FAILURE) > 10){
                Logger->log("Sync server not responding.");
                throw new exception;
            }
        }

        //!!->DATABASE updated when handling STOP_BIDDING message
        Table->setActiveItem(-1);
	}

    Logger->log("Auction done.");

    //6.5 send auction_complete message to connected bidders
    comm->sendMessage(createMessage(AUCTION_COMPLETE,-1),Table->getSyncSocket());

    quit();

}
catch (...){
    Logger->log("Shutting down.");
    quit();
}
	return;
}

void Auctioneer::quit(){
    updateBidders(createMessage(AUCTION_COMPLETE,-1),Table->getConnected());
    delete(Logger);
    delete(Table);
    delete(comm);
    delete(Database);
}


