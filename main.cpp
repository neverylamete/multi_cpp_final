#include <iostream>
#include "unistd.h"
#include "getopt.h"
#include "sys/types.h"
#include "sys/socket.h"
#include <netinet/in.h>
#include "arpa/inet.h"
#include "vector"
#include <thread>
#include "sys/select.h"

class thread_param
{
public:
    std::thread th;
    int status;
    int fd;

    thread_param()
    {
        status = -1;
        fd = -1;
    }

};

extern char *optarg;

void client_thread(thread_param &param)
{
    int fd = param.fd;

    int buf_len_in = 1000;
    int len_out = 0;
    int read_len = 0;
    char *buf_in[buf_len_in];
    char *buf_out[buf_len_in];

    fd_set rfds;
    struct timeval tm;
    int retval;

    while (1)
    {

        tm.tv_sec = 0;
        tm.tv_usec = 500;

        FD_ZERO(&rfds);
        FD_SET(fd, &rfds);

        retval = select(fd + 1, &rfds, NULL, NULL, &tm);
        if (retval)
        {
            if (FD_ISSET(fd, &rfds))
            {
                read_len = read(fd, buf_in, buf_len_in);
                if (read_len < 0)
                {
                    shutdown(fd, SHUT_RDWR);
                    break;
                }
            }
        }

    }
    param.status = 1;

}

int main(int argc, char *argv[])
{
    int port_opt;
    char *ip_opt;
    char *dir_opt;
    std::cout << "start" << std::endl;

    int opindex;
    char opchar;
    struct option opts[] = {
            {"ip",   required_argument, 0, 0},
            {"port", required_argument, 0, 0},
            {"dir",  required_argument, 0, 0},
            {0, 0,                      0, 0,}
    };

    while ((opchar = getopt_long(argc, argv, "h:p:d:", opts, &opindex)) != -1)
    {
        switch (opchar)
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

    int server_sock = 0;
    struct sockaddr_in addr;

    server_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port_opt);
    addr.sin_addr.s_addr = inet_addr(ip_opt);

    if (bind(server_sock, (struct sockaddr *) &addr, sizeof(addr)) < 0)
    {
        perror("bind error");
        return 1;
    }

    listen(server_sock, 100);

    std::vector<thread_param> vc;
    while (1)
    {
        int client_fd = accept(server_sock, 0, 0);

        thread_param tmp;

        tmp.fd = client_fd;
        //make thread

        tmp.th = std::thread(client_thread, std::ref(tmp));
        vc.push_back(tmp);

        auto it = vc.begin();
        while (it != vc.end())
        {
            if (it->status == 1)
            {
                it->th.join();
                it = vc.erase(it);
            }
            else
            {
                it++;
            }
        }

    }

    return 0;
}
