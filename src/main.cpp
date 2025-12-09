#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <csignal>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>

#include "app.h"

void signal_handler(int signum)
{
    std::cout << "\nClosing application..." << std::endl;
    exit(signum);
}

int main()
{
    signal(SIGINT, signal_handler);
    try
    {
        App fsl("./config.xml");
        fsl.run();
    }
    catch (const std::exception &e)
    {
        std::cerr << "App error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
