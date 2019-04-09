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
    if(argc < 7){
        std::cout << "Usage: client_program_1 #serverPort #repetitions #numberOfBuffers"
                " #bufferSize serverIPName #type{1,2,3}" << std::endl;
        exit(1);
    }

    // user-defined values
    int serverPort; // 40385
    int repetition;
    int nbufs;
    int bufsize;
    int type;
    char *serverIp = argv[5];

    try{
        size_t sz;
        serverPort = std::stoi(argv[1], &sz); // 40385
        repetition = std::stoi(argv[2], &sz);
        nbufs = std::stoi(argv[3], &sz);
        bufsize = std::stoi(argv[4], &sz);
        type = std::stoi(argv[6], &sz);
    } catch (std::invalid_argument){
        std::cout << "Please enter valid arguments.\n";
        std::cout << "Usage: client_program_1 #serverPort #repetitions #numberOfBuffers"
                " #bufferSize serverIPName #type{1,2,3}" << std::endl;
        exit(1);
    }

//    if (gethostname(serverIp, 100) < 0)
//    {
//        std::cerr << "Socket Client: unable to identify host: " << serverIp << std::endl;
//        exit(1);
//    }

    struct hostent *host = gethostbyname(serverIp);
    if (!host)
    {
        std::cerr << "Socket Client: unknown host: " << serverIp << std::endl;
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
        std::cerr << "Socket Client: socket open failed" << std::endl;
        exit(1);
    }
    std::cout << "ServerSD " << clientSD << std::endl;

    // connect
    if(connect(clientSD, (sockaddr *) &sendSocketsAddress, sizeof
    (sendSocketsAddress)) < 0){
        std::cerr << "Socket Client: connect failed" << std::endl;
        close(clientSD);
        exit(1);
    }

    char databuf[nbufs][bufsize];   // nbufs * bufsize = 1500

    sleep(1);

    // start timer
    // http://www.cs.loyola.edu/~jglenn/702/S2008/Projects/P3/time.html
    struct timeval start, end;
    gettimeofday(&start, NULL);

    int sleepTime = 0;
    for (int i = 0; i < repetition; i++)
    {
        std::cout << "Type " << type << " Repetition "
                  << i << "/" << repetition << std::endl;
        if (type == 1)
        {
            for (int j = 0; j < nbufs; j++)
            {
                // sleep for 500000 microseconds = 0.5 seconds
                // mitigates race condition if write
                // arrives before server finishes its current work
//                usleep(500000);
//                sleepTime += 500000;
                std::cout << "buffer " << j << std::endl;
                write(clientSD, databuf[j], bufsize);
            }
            std::cout << "Done writing" << std::endl;
        }
        else if (type == 2)
        {
            struct iovec vector[nbufs];
            for (int j = 0; j < nbufs; j++)
            {
                vector[j].iov_base = databuf[j];
                vector[j].iov_len = bufsize;
            }
//            usleep(500000);   // mitigates race condition
//            sleepTime += 500000;
            writev(clientSD, vector, nbufs);
        }
        else
        {
//            usleep(500000);   // mitigates race condition
//            sleepTime += 500000;
            write(clientSD, databuf, nbufs * bufsize);
        }

    }

    std::cout << "Out of loop" << std::endl;

    // read how many times the server performed read
    int countReads = 0;
    int bytesRead = 0;
    if(read(clientSD, (char *) &countReads, sizeof(countReads)) < 0)
    {
        std::cout << "Socket Client: read failed" << std::endl;
    }

    std::cout << "BytesRead " << bytesRead << "\n";

    // end timer
    gettimeofday(&end, NULL);

    // subtract time spent sleeping from actual socket exchange info
    std::cout << "Count " << countReads << "\n";
    std::cout << "Time elapsed " << (end.tv_sec * 1000000 + end.tv_usec)
                                    - (start.tv_sec  * 1000000 + start.tv_usec)
                                    - sleepTime
              << " microseconds" << std::endl;

    close(clientSD);
}