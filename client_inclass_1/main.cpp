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
#include <sys/time.h>

int main(int argc, char *argv[])
{
    // TESTING - following vars will be set from command line
    // TODO: remove hardcoded values and test user-provided args
    int serverPort = 40385; // last 5 digit of my student ID
    int repetition = 1;
    int nbufs = 1;
    int bufsize = 1500;
    char serverName[100]; // the server name
    gethostname(serverName, 100);
    int type = 1;

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

    char databuf[nbufs][bufsize];   // nbufs * bufsize = 1500

    // start timer
    // http://www.cs.loyola.edu/~jglenn/702/S2008/Projects/P3/time.html
    struct timeval start, end;
    gettimeofday(&start, NULL);

    // type 1
    for (int j = 0; j < nbufs; j++)
    {
        write(clientSD, databuf[j], bufsize);
    }

    // type 2
//    struct iovec vector[nbufs];
//    for (int j = 0; j < nbufs; j++)
//    {
//        vector[j].iov_base = databuf[j];
//        vector[j].iov_len = bufsize;
//    }
//    writev(clientSD, vector, nbufs);

    // type 3
//    int bytesWritten = write(clientSD, databuf, nbufs * bufsize);

    // read whatever the server has
    int count = 0;
    int bytesRead = read(clientSD, (char *)&count, sizeof(count));

    // end timer
    gettimeofday(&end, NULL);

//    std::cout << "BytesRead " << bytesRead << std::endl;
    std::cout << "Count " << count << std::endl;
    std::cout << "Time elapsed " << (end.tv_sec * 1000000 + end.tv_usec)
                                    - (start.tv_sec * 1000000 + start.tv_usec)
              << " microseconds" << std::endl;

    close(clientSD);
}