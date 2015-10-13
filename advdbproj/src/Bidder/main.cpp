#include <iostream>
#include <vector>
#include <string>
#include <unistd.h>
#include <strings.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "bidder.h"
#include "control.h"
#include "AbstractServer.h"
#include "logger.h"
#include "CommunicationService.h"

using namespace std;

int main (int argc, char* argv[])
{
    // The bidder's arguments should be <host>, <port> & <name>
    if (argc < 4) {
        cout << "Error!" << endl;
        cout << "Usage: bidder <host> <port> <name>" << endl;
        exit(EXIT_FAILURE);
    }
    // Construct the New Bidder
    Bidder bidder(argv[1], stoi(argv[2]), argv[3]);
    bidder.start();
    return 0;
}

