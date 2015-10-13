#ifndef ABSTRACTSERVER_H
#define ABSTRACTSERVER_H

#include <string>

#define NO_PHASE           -1
#define CONNECTING_PHASE    0
#define BIDDING_PHASE       1
#define INTEREST_PHASE      2
#define WINNER_DECLARED     3
#define AUCTION_END         4


class AbstractServer
{
    public:

        virtual int getListeningPort() = 0;
        virtual int handleMessage(std::string,int)=0;

        virtual void start()=0; //start the service
        virtual void quit()=0; //do the necessary cleanup

    protected:
        class AbstractCommunicationService* comm;
        class logger *Logger;

        virtual std::string createMessage(int,int)=0;

    private:

};

#endif // ABSTRACTSERVER_H
