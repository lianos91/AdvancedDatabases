#ifndef ABSTRACTCOMMUNICATIONSERVICE_H_INCLUDED
#define ABSTRACTCOMMUNICATIONSERVICE_H_INCLUDED

#include<vector>

class AbstractCommunicationService{
 public:

    virtual ~AbstractCommunicationService(){};

	virtual int sendMessage(std::string, int)=0;
    virtual int listenClients(int)=0;
    virtual int listenSocket(int)=0;
    virtual int connectToServer(int)=0; //connect to another server. Returns the socket connection
    virtual std::vector<int> incomingConnSetup(int,int)=0; //accept connections in the server's listening port

protected:
    std::vector<int> ConnectionsList;
    class AbstractServer* server;

};

#endif // ABSTRACTCOMMUNICATIONSERVICE_INCLUDED

