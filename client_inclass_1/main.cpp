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
    // user-defined values
    int serverPort = 40385; // last 5 digit of my student ID
    int repetition = 10;
    int nbufs = 3;
    int bufsize = 500;
    int type = 1;
    char serverIp[100];

    if (gethostname(serverIp, 100) < -1)
    {
        std::cout << "Socket Client: unable to identify host: " << serverIp << std::endl;
        exit(1);
    }

    struct hostent *host = gethostbyname(serverIp);
    if (!host)
    {
        std::cout << "Socket Client: unknown host: " << serverIp << std::endl;
        exit(1);
    }
    std::cout << "Hostname " << host->h_name << std::endl;

    sockaddr_in sendSocketsAddress;
    // zeroing out sockaddr_in datastructure
    bzero((char *) &sendSocketsAddress, sizeof(sendSocketsAddress));
    sendSocketsAddress.sin_family = AF_INET;
    sendSocketsAddress.sin_addr.s_addr = inet_addr(inet_ntoa(*(struct in_addr *)
            (*host->h_addr_list)));
    // changes the int format to network int format
    sendSocketsAddress.sin_port = htons(serverPort);

    // socket descriptor
    int clientSD;
    if((clientSD = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        std::cout << "Socket Client: socket open failed" << std::endl;
        exit(1);
    }
    std::cout << "ServerSD " << clientSD << std::endl;

    // connect
    if(connect(clientSD, (sockaddr *) &sendSocketsAddress, sizeof
    (sendSocketsAddress)) < 0){
        std::cout << "Socket Client: connect failed" << std::endl;
        close(clientSD);
        exit(1);
    }

    char databuf[nbufs][bufsize];   // nbufs * bufsize = 1500

    // start timer
    // http://www.cs.loyola.edu/~jglenn/702/S2008/Projects/P3/time.html
    struct timeval start, end;
    gettimeofday(&start, NULL);

    int countReads = 0;
    for (int i = 0; i < repetition; i++)
    {

        if (type == 1)
        {
            for (int j = 0; j < nbufs; j++)
            {
                sleep(1);   // mitigates race condition
                // if write arrives before server finishes its current work
                write(clientSD, databuf[j], bufsize);
            }
        }
        else if (type == 2)
        {
            struct iovec vector[nbufs];
            for (int j = 0; j < nbufs; j++)
            {
                vector[j].iov_base = databuf[j];
                vector[j].iov_len = bufsize;
            }
            sleep(1);   // mitigates race condition
            // if write arrives before server finishes its current work
            writev(clientSD, vector, nbufs);
        }
        else
        {
            sleep(1);   // mitigates race condition
            // if write arrives before server finishes its current work
            write(clientSD, databuf, nbufs * bufsize);
        }

    }

    // read how many times the
    int bytesRead = read(clientSD, (char *) &countReads, sizeof(countReads));

    // end timer
    gettimeofday(&end, NULL);

//    std::cout << "BytesRead " << bytesRead << std::endl;
    std::cout << "Count " << countReads << std::endl;
    std::cout << "Time elapsed " << (end.tv_sec * 1000000 + end.tv_usec)
                                    - (start.tv_sec * 1000000 + start.tv_usec)
              << " microseconds" << std::endl;

    close(clientSD);
}