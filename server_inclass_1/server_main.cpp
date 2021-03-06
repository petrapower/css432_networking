#include <sys/types.h>    // socket, bind
#include <sys/socket.h>   // socket, bind, listen, inet_ntoa
#include <netinet/in.h>   // htonl, htons, inet_ntoa
#include <arpa/inet.h>    // inet_ntoa
#include <netdb.h>        // gethostbyname
#include <unistd.h>       // read, write, close
#include <cstring>       // bzero
#include <netinet/tcp.h>  // SO_REUSEADDR
#include <sys/uio.h>      // writev
#include <csignal>
#include <iostream>
#include <sys/fcntl.h>

int serverSD;
int newSD;
int repetition; // user-defined value

const int BUFSIZE = 1500;
const int NUM_CONNECTIONS = 5;

void sigioHandler_ReadFromClient(int sig_type);

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        std::cout << "Usage: server_program_1 #unusedPort #repetitions" << std::endl;
        exit(1);
    }

    // user-defined value
    int port;

    try
    {
        // http://www.cplusplus.com/reference/string/stoi/
        size_t sz;
        port = std::stoi(argv[1], &sz); // 40385
        repetition = std::stoi(argv[2], &sz);
    }
    catch (std::invalid_argument)
    {
        std::cout << "Please enter valid arguments.\n";
        std::cout << "Usage: server_program_1 #unusedPort #repetitions" << std::endl;
        exit(1);
    }

    sockaddr_in acceptSocketsAddress;
    // zeroing out sockaddr_in datastructure
    bzero((char *) &acceptSocketsAddress, sizeof(acceptSocketsAddress));
    acceptSocketsAddress.sin_family = AF_INET;
    // for this address, I'll be able to listen to any calls to it
    acceptSocketsAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    // changes int the format to network int format
    acceptSocketsAddress.sin_port = htons(port);

    // socket descriptor
    if ((serverSD = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        std::cerr << "Socket Server: socket open failed" << std::endl;
        exit(1);
    }
    std::cout << "Server starting" << std::endl;

    const int on = 1;
    // allows for reusing the port
    setsockopt(serverSD, SOL_SOCKET, SO_REUSEADDR, (char *) &on, sizeof(int));

    // binding
    int rc = bind(serverSD, (sockaddr *) &acceptSocketsAddress, sizeof
    (acceptSocketsAddress));
    if (rc < 0)
    {
        std::cerr << "Socket Server: bind failed" << std::endl;
    }

    // listen
    listen(serverSD, NUM_CONNECTIONS);

    sockaddr_in newSockAddr;
    socklen_t newSockAddrSize = sizeof(newSockAddr);

    // accept
    if ((newSD = accept(serverSD, (sockaddr *) &newSockAddr, &newSockAddrSize)) < 0)
    {
        std::cerr << "Socket Server: accept failed" << std::endl;
        exit(1);
    }
    std::cout << "Server ready" << std::endl;

    // https://www.tutorialspoint.com/cplusplus/cpp_signal_handling.htm
    //https://books.google.com/books?id=dmt_mERzxV4C&pg=PA109&lpg=PA109&dq=cpp+sigio+fcntl+signal&source=bl&ots=dt8W3MFIMH&sig=ACfU3U2LIxvfB_wvuuee3-j0mdUi1CPihA&hl=en&sa=X&ved=2ahUKEwiJ4bHI-bzhAhVTnJ4KHVVfA4s4ChDoATAFegQICRAB#v=onepage&q=cpp%20sigio%20fcntl%20signal%20accept&f=false
    signal(SIGIO, sigioHandler_ReadFromClient);

    fcntl(newSD, F_SETOWN, getpid());
    fcntl(newSD, F_SETFL, FASYNC);

    // https://books.google.com/books?id=dmt_mERzxV4C&pg=PA109&lpg=PA109&dq=cpp+sigio
    // +fcntl+signal&source=bl&ots=dt8W3MFIMH&sig=ACfU3U2LIxvfB_wvuuee3-j0mdUi1CPihA&hl=en&sa=X&ved=2ahUKEwiJ4bHI-bzhAhVTnJ4KHVVfA4s4ChDoATAFegQICRAB#v=onepage&q=cpp%20sigio%20fcntl%20signal%20accept&f=false
    for (;;)
    {
//        std::cout << "Sleeping..." << std::endl;
        sleep(3);
    }
}

// the signalID will not be used in the function body
void sigioHandler_ReadFromClient(int sig_type)
{
    char databuf[BUFSIZE];
    bzero(databuf, BUFSIZE);

    int count = 0;
    for (int i = 0; i < repetition; i++)
    {
        int nRead = 0;
        while (nRead < BUFSIZE)
        {
            int bytesRead = read(newSD, databuf, BUFSIZE - nRead);
            nRead += bytesRead;
            count++;
        }
    }

    if (write(newSD, (char *) &count, sizeof(count)) < 0)
    {
        std::cout << "Socket Server: write failed" << std::endl;
    }

    std::cout << "Server finished" << std::endl;
    close(serverSD);
    exit(0);
}