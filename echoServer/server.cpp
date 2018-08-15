//
//  server.cpp
//  echoServer
//
//  Created by Alwin Mathew on 8/14/18.
//  Copyright © 2018 Alwin Mathew. All rights reserved.
//

#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/StreamSocket.h>
#include <Poco/Net/SocketAddress.h>
#include <Poco/FIFOBuffer.h>

#include <iostream>
#include <string>

using namespace Poco::Net;
using namespace Poco;
using namespace std;

#define BUFFER_SIZE 1024


int main(int argc, const char * argv[])
{
    
    if (argc < 2)
    {
        cout << "Error! Too few arguments.\nUsage: ./" << argv[0] << " [port]" << endl;
    }
    int port = atoi(argv[1]);
    
    
    //create the server on the localhost with the port number given as an argument and begin listening
    //DEBUG:print IP address and port that it is listening to
    ServerSocket myServer(port);
    SocketAddress debugAddress = myServer.address();
    cout << "Echo server is listening on port " << debugAddress.toString() << "." << endl;
    
    //create FIFO buffer for TCP interprocess communication
    FIFOBuffer buffer(BUFFER_SIZE);
    char charbuffer[BUFFER_SIZE];
    StreamSocket newStream = myServer.acceptConnection();
    cout << "Recieved a connection from client!" << endl;
    
    for (;;)
    {
        //recieveBytes will return 0 if the connection gets closed
        int recievedBytes = newStream.receiveBytes(buffer);
        if (recievedBytes == 0)
        {
            //the connection has been closed by client, so lets just cleanup, close the server, and exit
            cout << "Connection has been closed by client, will now exit." << endl;
            newStream.shutdown();
            newStream.~StreamSocket();
            buffer.drain();
            buffer.~BasicFIFOBuffer();
            myServer.~ServerSocket();
            exit(0);
        }
        
        //copy contents of the recieved buffer into char and print to stdout
        buffer.read(charbuffer, BUFFER_SIZE);
        cout << "Recieved from client :" << charbuffer << endl;
        
        //lets send the same data back to the client
        buffer.copy(charbuffer, BUFFER_SIZE);
        int sentBytes = newStream.sendBytes(buffer);
        if (sentBytes != BUFFER_SIZE)
        {
            //the SocketStream did not send all the bytes successfully
            cerr << "ERROR: The SocketStream did not send all of the bytes." << endl;
        }
    }
    
    
}
