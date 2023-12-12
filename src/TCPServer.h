#include <iostream>
#include <string>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <fstream>

using namespace std;

#define TCP_PORT 12345

class TCPServer
{
    private:

    //buffer to send and receive messages with client
    char msg[1024];
    sockaddr_in servAddr;
    int serverSd;
    sockaddr_in newSockAddr;
    socklen_t newSockAddrSize;
    int newSd;
    struct timeval start1, end1;
    int bytesRead;
    int bytesWritten;

    public:
        TCPServer(/* args */);
        ~TCPServer();

        void initSocket();
        bool openSocket();
        void listenForClient();
        bool connectToClient();
        void getSessionTime();
        bool receiveMsgFromClient();
        bool sendMsgToClient(string data);
        bool closeConnection();

        char* getClientMsg();        
};

TCPServer::TCPServer(){}
TCPServer::~TCPServer(){}

void TCPServer::initSocket()
{
    bzero((char*)&servAddr, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(TCP_PORT);
}


bool TCPServer::openSocket()
{
    serverSd = socket(AF_INET, SOCK_STREAM, 0);
    
    if(serverSd < 0)
    {
        cerr << "[TCPServer] Error establishing the server socket" << endl;
        return 0;
    }
    
    //bind the socket to its local address
    int bindStatus = bind(serverSd, (struct sockaddr*) &servAddr, 
        sizeof(servAddr));
    if(bindStatus < 0)
    {
        cerr << "[TCPServer] Error binding socket to local address" << endl;
        return 0;
    }

    return 1;
}

void TCPServer::listenForClient()
{
    cout << "[TCPServer] Waiting for a client to connect ..." << endl;
    //listen for up to 5 requests at a time
    listen(serverSd, 5);

}

bool TCPServer::connectToClient()
{
    //receive a request from client using accept
    //we need a new address to connect with the client
    //sockaddr_in newSockAddr;
    newSockAddrSize = sizeof(newSockAddr);
    //accept, create a new socket descriptor to 
    //handle the new connection with client
    newSd = accept(serverSd, (sockaddr *)&newSockAddr, &newSockAddrSize);
    if(newSd < 0)
    {
        cerr << "[TCPServer] Error accepting request from client" << endl;
        return 0;
    }
    
    cout << "[TCPServer] Connected with client" << endl;
    return 1;
}

void TCPServer::getSessionTime()
{
    gettimeofday(&start1, NULL);
    bytesRead = 0; 
    bytesWritten = 0;
}

bool TCPServer::receiveMsgFromClient()
{
    //Put this either in thread or while(1)-loop

    //receive a message from the client (listen)
    //cout << "Awaiting client response..." << endl;
    memset(&msg, 0, sizeof(msg));//clear the buffer
    bytesRead += recv(newSd, (char*)&msg, sizeof(msg), 0);
    
    if(!strcmp(msg, "exit"))
    {
        cout << "[TCPServer] Client has quit the session" << endl;
        return 0;
    }        
    cout << "[TCPServer] Client: " << msg << endl; 
    return 1;
}

bool TCPServer::sendMsgToClient(string data)
{
    
    cout << ">";
    getline(cin, data);
    memset(&msg, 0, sizeof(msg)); //clear the buffer
    strcpy(msg, data.c_str());
    if(data == "exit")
    {
        //send to the client that server has closed the connection
        send(newSd, (char*)&msg, strlen(msg), 0);
        return 0;
    }
    //send the message to client
    bytesWritten += send(newSd, (char*)&msg, strlen(msg), 0);
    return 1;

}

bool TCPServer::closeConnection()
{
    //we need to close the socket descriptors after we're all done
    gettimeofday(&end1, NULL);
    close(newSd);
    close(serverSd);
    cout << "[TCPServer] ********Session********" << endl;
    cout << "[TCPServer] Bytes written: " << bytesWritten << " Bytes read: " << bytesRead << endl;
    cout << "[TCPServer] Elapsed time: " << (end1.tv_sec - start1.tv_sec) 
        << " secs" << endl;
    cout << "[TCPServer] Connection closed..." << endl;
    return 0;   

}

char* TCPServer::getClientMsg()
{
    return msg;
}