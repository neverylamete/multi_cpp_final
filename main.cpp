#include <iostream>
#include "unistd.h"
#include "getopt.h"

int port_opt;
char *ip_opt;
char *dir_opt;


extern char *optarg;

int main(int argc, char *argv[])
{
    std::cout << "start" << std::endl;

    int opindex;
    char opchar;
    struct option opts[] = {
            {"ip",   required_argument, 0, 0},
            {"port", required_argument, 0, 0},
            {"dir",  required_argument, 0, 0},
            {0, 0,                      0, 0,}
    };


    while((opchar = getopt_long(argc, argv, "h:p:d:", opts, &opindex)) != -1 )
    {
        switch( opchar )
        {
            case 'h' :
                ip_opt = optarg;
                break;
            case 'p' :
                port_opt = atoi(optarg);
                break;
            case 'd' :
                dir_opt = optarg;
                break;
            case 0:
                break;
            case '?':
                break;
            default:
                break;
        }

    }


    std::cout << "ip=" << ip_opt << std::endl;
    std::cout << "port=" << port_opt << std::endl;
    std::cout << "dir=" << dir_opt << std::endl;

    return 0;
}
