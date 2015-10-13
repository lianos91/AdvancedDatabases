#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "AbstractServer.h"
#include "CommunicationService.h"
#include "logger.h"
#include "MiscLib.h"
#include "bidder.h"
#include "control.h"

#define USR_MSG_SZE 32
#define AUCT_MSG_SZE 128

using namespace std;

Bidder::Bidder(string host_name, int port_num, string bidder_name):AbstractServer() {

    host = host_name;
    port = port_num;
    name = bidder_name;
     //First initialize fields of the parent
    comm = new CommunicationService(this);
    Logger = new logger(name);
    Logger->log("Bidder "+name+" started...\n");
}

void Bidder::start(){

    fd_set read_fd_set, active_fd_set;
    int userfd = 0; // User is acquired the stdin file descriptor

	sockfd = comm->connectToServer(getListeningPort());

    // Send Connect Message to the specified auctioneer
    if (comm->sendMessage(createMessage(CONNECT, 0),getListeningSocket())==EXIT_FAILURE){
        cout << "Sent Message Connect Fail"<<endl;
        auctionPhase = AUCTION_END;
        //quit();
        return ;
    }

    // The file descriptors on which we are waiting for input
    // are added to the read_fd_set so that select knows when
    // the auctioneer or the user have written on

    while(auctionPhase != AUCTION_END){

        FD_ZERO(&active_fd_set);
        FD_SET(getListeningSocket(), &active_fd_set);
        FD_SET(userfd, &active_fd_set);
        read_fd_set = active_fd_set;

        if (select(getListeningSocket()+1, &read_fd_set, nullptr, nullptr, nullptr) < 0) {
            Logger->log("Internal Error : select : Bidder shutting down...\n");
            cerr << "Bidder : main : select" << endl;
            auctionPhase = AUCTION_END;
            //quit();
            //return;
        }

        if (FD_ISSET(getListeningSocket(), &read_fd_set)) {
            //cout << "command from server" << endl;
            //("New incoming message from auctioneer...\n");
            int retval = comm->listenSocket(getListeningSocket());
            if (retval == EXIT_FAILURE){
                Logger->log("Auctioneer Connection problem. Shutting Down");
                auctionPhase = AUCTION_END;
                //quit();
            }
        }
        if (FD_ISSET(userfd, &read_fd_set)) {
            //cout << "command from user" << endl;
            //createLog("New incoming message from user...\n");
            getUsrMsg();
        }
    }

    quit();

    return;
}

/**CHANGE CIN**/
void Bidder::getUsrMsg(void) {

    string str = "";
    getline(cin,str);
    if (str.compare("")==0)
        return;
    Logger->log("Successfully received user message: "+str);
    handleUsrMsg(str);
    return;
}

void Bidder::handleUsrMsg(string usr_msg) {

    if (usr_msg.compare("list_high_bid") == 0) {
        cout << "Current High Bid: " << highBid << endl;
    }
    else if (usr_msg.compare("list_description") == 0) {
        cout << "Current Item Description: " << itemDescription << endl;
    }
    else if (usr_msg.compare("quit") == 0) {
        comm->sendMessage(createMessage(QUIT, 0),getListeningSocket());
        Logger->log("Quitting");
        if (shutdown(sockfd, SHUT_RDWR) < 0) {
            cerr << "Bidder : Shutting Down Connection" << endl;
            return;
        }
        auctionPhase = AUCTION_END;

    }
    else if (usr_msg.compare("i_am_interested")==0){
        Logger->log("I_AM_INTERESTED");
        comm->sendMessage(createMessage(I_AM_INTERESTED, 0),getListeningSocket());
    }
    else if(usr_msg.find(' ')!=string::npos) {

        vector<string> usr_command = MiscLib::split(usr_msg, ' ');

        if (usr_command[0].compare("bid") == 0) {

            if (auctionPhase == INTEREST_PHASE) {
                comm->sendMessage(createMessage(I_AM_INTERESTED, 0),getListeningSocket());
                bidExists = true;
                amount = stoi(usr_command[1]);
            }
            else if (auctionPhase == BIDDING_PHASE) {
                amount = stoi(usr_command[1]);
                comm->sendMessage(createMessage(MY_BID, 0),getListeningSocket());
            }
            else {
               Logger->log("Not a proper time for bidding..");
            }
        }
        else {
            Logger->log("Ignoring false command by user");
            cout << "Invalid Command! Your options are:" << endl;
            cout << "   bid <amount>" << endl;
            cout << "   list_high_bid" << endl;
            cout << "   list_description" << endl;
            cout << "   quit" << endl;
        }
    }
    return;
}

int Bidder::handleMessage(string auct_msg, int dummy) {

    vector<string> tokensVec = MiscLib::split(auct_msg, ':');
    int auctCommand = stoi(tokensVec[0]);

    switch (auctCommand) {
        case BID_ITEM:
            auctionPhase = INTEREST_PHASE;

            itemExists = true;
            highBid = stoi(tokensVec[3]);
            itemID = stoi(tokensVec[1]);
            itemDescription = tokensVec[2];
            Logger->log("Show Interest for Item:"+to_string(itemID)+", "+itemDescription+". Initial value:"+
                        to_string(highBid));
            break;
        case START_BIDDING:

            auctionPhase = BIDDING_PHASE;

            startBidding = true;
            itemID = stoi(tokensVec[1]);

            Logger->log("Start bids for Item:"+to_string(itemID));

            if (bidExists)
                comm->sendMessage(createMessage(MY_BID, 0),getListeningSocket());
            break;
        case NEW_HIGH_BID:
            highBid = stoi(tokensVec[1]);

            Logger->log("new high bid: "+to_string(highBid));

            if (tokensVec[2].compare(name) == 0){
                Logger->log("You are currently the holder of the highest bid!");
            }
            break;
        case STOP_BIDDING:
            auctionPhase = NO_PHASE;

            startBidding = false;
            Logger->log("Stop Bidding");
            if (tokensVec[1].compare(name) == 0){
                Logger->log("You won the bid for item "+to_string(itemID)+" !!");
            }
            itemExists = false;
            break;
        case AUCTION_COMPLETE:
            auctionPhase = NO_PHASE;

            Logger->log("Auction Completed");
            auctionPhase = AUCTION_END;
            break;

        case DUPLICATE_NAME:
            // Duplicate Name : Error Message
            Logger->log("Duplicate Name: PLease type another");
            break;
        case AUCT_ITEM_DONE:
            Logger->log("Biddings stopped. Waiting for winner declaration ...");
            break;
        default:
            Logger->log("From: server, message unkown. Exiting..");
            auctionPhase = AUCTION_END;
    }
    return 0;
}

void Bidder::quit(){
    close(sockfd);
    delete(Logger);
}

string Bidder::createMessage(int msgno, int dummy) {

    string msg = "";
    switch (msgno) {
        case CONNECT:
            msg = to_string(msgno) + ":" + name;
            break;
        case I_AM_INTERESTED:
            msg = to_string(msgno) + ":" + to_string(itemID);
            break;
        case MY_BID:
            msg = to_string(msgno) + ":" + to_string(amount) + ":" + to_string(itemID);
            break;
        case QUIT:
            msg = to_string(msgno);
            break;
    }
    return msg;
}



