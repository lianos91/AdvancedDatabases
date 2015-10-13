#include <iostream>
#include <thread>         // std::thread
#include "Auctioneer.h"
#include "SynchrServer.h"

using namespace std;

void spawnAuctioneers(int auctId){
    Auctioneer * A = new Auctioneer();
    A->setID(auctId);

    A->start();
    delete(A);
}

int main(int argc, char* argv[]) {
	if (argc < 2){
		cout << "Usage: BiddingPlatform.main <auctioneer_config>"<< endl ;
		return -1;
	}
    Auctioneer::conf = argv[1]; //set configuration file

    //spawn the 2 Auctioneers
    std::thread a2(spawnAuctioneers,2);
	std::thread a1(spawnAuctioneers,1);
	a1.join();
	a2.join();

	cout << "mainEnd" << endl; // prints
	return 0;
}

