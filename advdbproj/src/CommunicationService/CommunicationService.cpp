/*
 * Listener.cpp
 *
 *  Created on: Mar 20, 2015
 *      Author: lianos91
 */
#include <string>
#include <iostream>       // std::cout
#include <thread>         // std::thread
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/time.h>
#include <errno.h>
#include <strings.h>
#include <stdexcept>

#include "AbstractCommunicationService.h"
#include "CommunicationService.h"
#include "AbstractServer.h"
#include "control.h"
#include "MiscLib.h"

using namespace std;

CommunicationService::CommunicationService(AbstractServer* _A) {
    server = _A;
    FD_ZERO(&active_fd_set);
}

CommunicationService::~CommunicationService() {
    for(vector<int>::iterator it = ConnectionsList.begin(); it!=ConnectionsList.end(); it++){
        close(*it);
    }
    close(ServerSocket);
}

int readXbytes(int socket,char* buffer, int x )
{
    int bytesRead = 0;
    int result;
    while (bytesRead < x)
    {
        result = read(socket, buffer + bytesRead, x - bytesRead);
        if (result < 1 )
        {
            return EXIT_FAILURE;// Throw your error.
        }

        bytesRead += result;
    }
    return EXIT_SUCCESS;
}


//A disconnected socket is deleted after failing to write. Return val goes to
//Server and he deletes it from the table
int send_all(int socket, const char *buffer, size_t length, int flags)
{
    ssize_t n;
    int ret = 0;
    const char *p = buffer;
    while (length > 0)
    {
        n = send(socket, p, length, flags);
        if (n <= 0) {
            switch (errno) {
            case EPIPE:
                return -1;
            default:
                return -1;
            }
        }
        p += n;
        length -= n;
        ret += n;
    }
    return ret;
}

int CommunicationService::sendMessage(string msg, int socket){
    string toSend="";
    if (msg.size() < 10)
        toSend = to_string(0) + to_string(msg.size()) + ":" + msg;
    else
        toSend = to_string(msg.size()) + ":" + msg;

    int written = send_all(socket, toSend.c_str(), toSend.size(), MSG_NOSIGNAL);

    if (written != toSend.size()) {
        cout <<"failure to send,written: "<<written<<endl;
      //  FD_CLR(socket, &active_fd_set);
      //  FD_CLR(socket, &read_fd_set);
      //  close(socket);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;

    cout << "sync__exc on sending message to "<<socket<<endl;
    return EXIT_FAILURE;

}

//if fail to connect, exit_failure
int CommunicationService::connectToServer(int port){
    int sock;
    struct sockaddr_in serv;

    //Create socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
        printf("Could not create socket");
        return EXIT_FAILURE;
    }

    serv.sin_addr.s_addr = INADDR_ANY;
    serv.sin_family = AF_INET;
    serv.sin_port = htons( port );

    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&serv , sizeof(serv)) < 0){
        perror("connect failed. Error");
        return EXIT_FAILURE;
    }

    ServerSocket = sock;
    FD_SET(ServerSocket, &active_fd_set);
    if (sock > maxsocket)
        maxsocket = sock;

    cout <<ServerSocket<<" is server socket"<<endl;

    return sock;
}

/*
   Waits for L seconds for incomming connections (ONLY connect). Throws exception
*/

vector<int> CommunicationService::incomingConnSetup(int timeout,int desiredClients) {

    int sockfd, newsockfd;
    int portno = server->getListeningPort();
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        cerr << "Auctioneer : Opening socket" << endl;
        throw new exception;
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
        cerr << "Auctioneer : On Binding" << endl;
        throw new exception;

    }
    if (listen(sockfd,5) < 0) {
        cerr << "Auctioneer : Listen" << endl;
        throw new exception;
    }
    /*
     *  Select: Initialization of sets
     */
    if (sockfd > maxsocket){
        maxsocket = sockfd;
    }
    FD_ZERO(&connect_fd_set);
    FD_SET(sockfd, &connect_fd_set);
    //timeout setup
    struct timeval tv_select, tv1,tv2;
    tv_select.tv_sec = timeout;
    tv_select.tv_usec = 0;
    bool isfdset = false;
    timeout = timeout*1000000;
    while(timeout>0) {
        tv_select.tv_sec = 0;
        tv_select.tv_usec = timeout;
        gettimeofday(&tv1, nullptr);

        if (select(sockfd+1, &connect_fd_set, nullptr, nullptr, &tv_select) < 0) {
            cerr << "Communication : select";
            throw new exception;
        }
        if (FD_ISSET(sockfd, &connect_fd_set)) {
            clilen = sizeof(cli_addr);
            newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
            if (newsockfd < 0) {
                cerr << "Communication : On Accept";
                throw new exception;
            }
            //set the maximum sockets for the next select
            if (newsockfd > maxsocket){
                maxsocket = newsockfd;
            }
            FD_SET(newsockfd, &active_fd_set);
            FD_SET(sockfd, &connect_fd_set);
            cout <<"in connection, socket:"<<newsockfd<<endl;
            isfdset = true;
            ConnectionsList.push_back(newsockfd);
            if ((ConnectionsList.size() == desiredClients)&&(desiredClients>0))
                break;
        }
        if (!isfdset){
            break;
        }
        gettimeofday(&tv2, nullptr);
        timeout = timeout - (tv2.tv_sec - tv1.tv_sec)*1000000 - abs(tv2.tv_usec - tv1.tv_usec);
    }

    return ConnectionsList;
}

/*Block and listen from a specific socket*/
int CommunicationService::listenSocket(int fd){
    char input_size[3];
    int ret = -1;

    if ( (readXbytes(fd, input_size, BID_MSG_SZE)) == EXIT_FAILURE) {
        //cerr << "error Server : from one socket "<<ret << endl;
       return EXIT_FAILURE;
    }
        string str (input_size,BID_MSG_SZE);
                    vector<string> n = MiscLib::split(str, ':');
                    int command_size = stoi(n[0]);

      char input_command[command_size];

    if ((readXbytes(fd, input_command, command_size) )== EXIT_FAILURE) {
        return EXIT_FAILURE;
    }
    string str2 (input_command,command_size);

    server->handleMessage(str2,fd);

    return EXIT_SUCCESS;
}
/*
 * Timeout in Seconds
 */
 /*
    return value = 0 if  no value was read during timeout
 */
int CommunicationService::listenClients(int timeout) {
    //gather all connections
    vector<int> allConnections = ConnectionsList;
    if (ServerSocket>0)
        allConnections.push_back(ServerSocket);

    /*
     *  SOS : Check for error read after select in case the file descriptor is closed
     *
     */
    struct timeval tv2, tv1, tv_select;
    string success = to_string(SUCCESS);
    string failure = to_string(FAILURE);
    bool haveRead = false;

    timeout = timeout*1000000;

    while (timeout > 0) {
        read_fd_set = active_fd_set;
        tv_select.tv_sec = 0;
        tv_select.tv_usec = timeout;
        gettimeofday(&tv1, nullptr);
        //if empty fd_read_set then unpredictable behaviour: either Timeout or read<0
        if (select(maxsocket+1, &read_fd_set, nullptr, nullptr, &tv_select) < 0) {
            cout << "Communication : listenBidders : select" << endl;
            return EXIT_FAILURE;
        }
        for (int i=0; i<allConnections.size(); i++){
            int fd = allConnections[i];
            if (FD_ISSET(fd, &read_fd_set)) {
                int retval = listenSocket(fd);
                if (retval == EXIT_SUCCESS)
                    haveRead = true;
            }

        }

        gettimeofday(&tv2, nullptr);
        timeout = timeout - (tv2.tv_sec - tv1.tv_sec)*1000000 - abs(tv2.tv_usec - tv1.tv_usec);
    }
    if (haveRead)
        return EXIT_SUCCESS;
           //no read during this timeout
    return EXIT_FAILURE;
}
