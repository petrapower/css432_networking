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

int main(int argc, char *argv[])
{
    int serverPort = 40385; // last 5 digit of my student ID
    char serverName[60]; // the server name
    gethostname(serverName, 60);
    char databuf[BUFFSIZE];

    struct hostent *host = gethostbyname(serverName);
    std::cout << "Hostname " << host->h_name << std::endl;

    sockaddr_in sendSocketsAddress;
    // zeroing oit sockaddr_in datastructure
    bzero((char *) &sendSocketsAddress, sizeof(sendSocketsAddress));
    sendSocketsAddress.sin_family = AF_INET;
    sendSocketsAddress.sin_addr.s_addr = inet_addr(inet_ntoa(*(struct in_addr *)
            (*host->h_addr_list)));
    // htonl changes the format to network format
    sendSocketsAddress.sin_port = htons(serverPort);

    // socket descriptor
    int clientSD = socket(AF_INET, SOCK_STREAM, 0);
    const int on = 1;
    std::cout << "ServerSD " << clientSD << std::endl;

    // connect
    int rc = connect(clientSD, (sockaddr *) &sendSocketsAddress, sizeof
    (sendSocketsAddress));

    //databuf = new char[BUFFSIZE];
    for (int i = 0; i < BUFFSIZE; i++)
    {
        databuf[i] = 'z';
    }
    // write something to server
    int bytesWritten = write(clientSD, databuf, BUFFSIZE);
    // read whatever the server has
    // in this case, the server changes the 78th char of the original message
    int bytesRead = read(clientSD, databuf, BUFFSIZE);
    std::cout << "BytesRead " << bytesRead << std::endl;
    std::cout << databuf[77] << std::endl;
    std::cout << databuf[78] << std::endl;
    std::cout << databuf[79] << std::endl;

    close(clientSD);
}