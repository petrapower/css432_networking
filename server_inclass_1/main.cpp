#include <sys/types.h>    // socket, bind
#include <sys/socket.h>   // socket, bind, listen, inet_ntoa
#include <netinet/in.h>   // htonl, htons, inet_ntoa
#include <arpa/inet.h>    // inet_ntoa
#include <netdb.h>        // gethostbyname
#include <unistd.h>       // read, write, close
#include <string.h>       // bzero
#include <netinet/tcp.h>  // SO_REUSEADDR
#include <sys/uio.h>      // writev
#include <iostream>

const int BUFFSIZE = 1500;
const int NUM_CONNECTIONS = 5;

int main(int argc, char *argv[])
{
    int port = 40385; // last 5 digit of my student ID
    char databuf[BUFFSIZE];
    bzero(databuf, BUFFSIZE);

    sockaddr_in acceptSocketsAddress;
    // zeroing oit sockaddr_in datastructure
    bzero((char*) &acceptSocketsAddress, sizeof(acceptSocketsAddress));
    acceptSocketsAddress.sin_family = AF_INET;
    // for this address, I'll be able to listen to any calls to it
    acceptSocketsAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    // htons changes int the format to network int format
    acceptSocketsAddress.sin_port = htons(port);

    // socket descriptor
    int serverSD = socket(AF_INET, SOCK_STREAM, 0);
    const int on = 1;
    // allows for reusing the port
    setsockopt(serverSD, SOL_SOCKET, SO_REUSEADDR, (char *) &on, sizeof(int));
    std::cout << "ServerSD " << serverSD << std::endl;

    // binding
    int rc = bind(serverSD, (sockaddr *) &acceptSocketsAddress, sizeof
    (acceptSocketsAddress));
    if(rc < 0){
        // on UNIX, if code < 0 => error
        std::cerr << "Bind Failed" << std::endl;
    }

    // listen
    listen(serverSD, NUM_CONNECTIONS);

    sockaddr_in newSockAddr;
    socklen_t newSockAddrSize = sizeof(newSockAddr);

    // accept
    int newSD = accept(serverSD, (sockaddr *) &newSockAddr, &newSockAddrSize);
    std::cout << "Accepted Socket " << newSD << std::endl;

    int bytesRead = read(newSD, databuf, BUFFSIZE);
    std::cout << "Bytesread " << bytesRead << std::endl;

    int bytesWritten = write(newSD, databuf, BUFFSIZE); // send it back

    close(newSD);
    close(serverSD);
}