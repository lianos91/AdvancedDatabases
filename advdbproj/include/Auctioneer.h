
#ifndef AUCTIONEER_H_
#define AUCTIONEER_H_

#include "RegTable.h"
#include "AbstractServer.h"

#include <string>
#include <iostream>       // std::cout

class Auctioneer : public AbstractServer {
public:
	static std::string conf;

	Auctioneer(){};

	virtual ~Auctioneer(){};

    void start();
    void quit();

	int getID(){ return auctioneerID; }
    void setID(int id){ auctioneerID = id; }

    int handleMessage(std::string,int);
    int getListeningPort(){ return this->listeningPort; }

private:
    RegTable* Table;

    int auctioneerID;

    int L;
    int NumItems;

    void updateBidders(std::string, std::map<int,std::string>);

    int create_port_file();
    int readConf();

    class AbstractDataLayer* Database;

    std::string createMessage(int,int);

    int auctionPhase = NO_PHASE;

    bool updated_once=false;

    int listeningPort;

};

#endif /* AUCTIONEER_H_ */
