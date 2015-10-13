
#ifndef COMMUNICATIONSERVICE_H
#define COMMUNICATIONSERVICE_H

#include <vector>
#include <sys/select.h>
#include "AbstractCommunicationService.h"
#define BID_MSG_SZE 3


class CommunicationService : public AbstractCommunicationService {
public:
    CommunicationService(class AbstractServer*);
	~CommunicationService();
	int sendMessage(std::string, int);
    int listenClients(int);
    int listenSocket(int);
    int connectToServer(int); //connect to another server. Returns the socket connection
    std::vector<int> incomingConnSetup(int,int); //accept connections in the server's listening port

private:
    //std::vector<int> ConnectionsList;
    //AbstractServer* server;
    fd_set read_fd_set, active_fd_set;
    fd_set connect_fd_set;
    int maxsocket = -1;
    int ServerSocket = -1;
};

#endif /* COMMUNICATIONSERVICE_H_ */
