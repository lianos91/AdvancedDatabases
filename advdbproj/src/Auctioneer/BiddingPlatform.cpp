//============================================================================
// Name        : BiddingPlatform.cpp
// Author      : Lianos Konstantinos-Nektarios / Railis Konstantinos
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

using namespace std;

#include <iostream>
#include <iostream>       // std::cout
#include <thread>         // std::thread


int main(int argc, char* argv[]) {

	if (argc < 2){
		cout << "Usage: BiddingPlatform.main <auctioneer_config>"<< endl ;
	}

	string auctConfig = argv[1];

	Auctioneer A = new Auctioneer(argv[1]);
	Auctioneer A = new Auctioneer(argv[1]);
	thread a1(A.auct());
	thread a2(A.auct());

	a1.join();
	a2.join();
	cout << "" << endl; // prints 
	return 0;
}
