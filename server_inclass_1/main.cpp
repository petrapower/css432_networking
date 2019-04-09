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
#include <sys/fcntl.h>

int serverSD;
int repetition;

const int BUFSIZE = 1500;
const int NUM_CONNECTIONS = 5;

void sigioHandler_ReadFromClient(int sig_type);

int main(int argc, char *argv[])
{
    // TODO: remove hardcoded values and test user-provided args
    int port = 40385; // last 5 digit of my student ID
    repetition = 10;

    sockaddr_in acceptSocketsAddress;
    // zeroing oit sockaddr_in datastructure
    bzero((char *) &acceptSocketsAddress, sizeof(acceptSocketsAddress));
    acceptSocketsAddress.sin_family = AF_INET;
    // for this address, I'll be able to listen to any calls to it
    acceptSocketsAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    // changes int the format to network int format
    acceptSocketsAddress.sin_port = htons(port);

    // socket descriptor
    if((serverSD = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        std::cerr << "Socket Server: socket open failed" << std::endl;
        exit(1);
    }
    const int on = 1;
    // allows for reusing the port
    setsockopt(serverSD, SOL_SOCKET, SO_REUSEADDR, (char *) &on, sizeof(int));
    std::cout << "ServerSD " << serverSD << std::endl;

    // binding
    int rc = bind(serverSD, (sockaddr *) &acceptSocketsAddress, sizeof
    (acceptSocketsAddress));
    if (rc < 0)
    {
        std::cerr << "Bind Failed" << std::endl;
    }

    // listen
    listen(serverSD, NUM_CONNECTIONS);

    // https://www.tutorialspoint.com/cplusplus/cpp_signal_handling.htm
    //https://books.google.com/books?id=dmt_mERzxV4C&pg=PA109&lpg=PA109&dq=cpp+sigio+fcntl+signal&source=bl&ots=dt8W3MFIMH&sig=ACfU3U2LIxvfB_wvuuee3-j0mdUi1CPihA&hl=en&sa=X&ved=2ahUKEwiJ4bHI-bzhAhVTnJ4KHVVfA4s4ChDoATAFegQICRAB#v=onepage&q=cpp%20sigio%20fcntl%20signal%20accept&f=false
    signal(SIGIO, sigioHandler_ReadFromClient);

    std::cout << "Out of signal" << std::endl;

    fcntl(serverSD, F_SETOWN, getpid());
    fcntl(serverSD, F_SETFL, FASYNC);

    // https://books.google.com/books?id=dmt_mERzxV4C&pg=PA109&lpg=PA109&dq=cpp+sigio
    // +fcntl+signal&source=bl&ots=dt8W3MFIMH&sig=ACfU3U2LIxvfB_wvuuee3-j0mdUi1CPihA&hl=en&sa=X&ved=2ahUKEwiJ4bHI-bzhAhVTnJ4KHVVfA4s4ChDoATAFegQICRAB#v=onepage&q=cpp%20sigio%20fcntl%20signal%20accept&f=false
    for(;;){
        std::cout << "Sleeping..." << std::endl;
        sleep(3);
    }
}

// the signalID will not be used in the function body
void sigioHandler_ReadFromClient(int sig_type)
{
    char databuf[BUFSIZE];
    bzero(databuf, BUFSIZE);
    std::cout << "In signal" << std::endl;

    sockaddr_in newSockAddr;
    socklen_t newSockAddrSize = sizeof(newSockAddr);

    // accept
    int newSD;
    if((newSD = accept(serverSD, (sockaddr *) &newSockAddr, &newSockAddrSize)) < 0){
        std::cerr << "Socket Server: accept failed" << std::endl;
        exit(1);
    }
    std::cout << "Accepted Socket " << newSD << std::endl;

    int count = 0;
    for(int i = 0; i < repetition; i++)
    {
        int nRead = 0;
        while (nRead < BUFSIZE)
        {
            int bytesRead = read(newSD, databuf, BUFSIZE - nRead);
            std::cout << "BytesRead " << bytesRead << std::endl;
            nRead += bytesRead;
            count++;
        }
        std::cout << "CurrentCount " << count << std::endl;
    }
    write(newSD, (char *) &count, sizeof(count));

    close(serverSD);
    exit(0);
}