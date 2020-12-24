#include <iostream>
#include "unistd.h"
#include "getopt.h"
#include "sys/types.h"
#include "sys/socket.h"
#include <netinet/in.h>
#include "arpa/inet.h"
#include "vector"
#include <thread>
#include <cstring>
#include "sys/select.h"
#include "list"
#include "string"
#include "fcntl.h"
#include "fstream"

#include "httpparser/request.h"
#include "httpparser/httprequestparser.h"

//GLOBAL VARS
char *dir_opt;


class thread_param
{
public:
    std::thread th;
    int closed;
    int need_close;
    int fd;

    thread_param()
    {
        closed = 0;
        need_close = 0;
        fd = -1;
    }

};

extern char *optarg;

std::string make_http_header(bool good)
{
    std::string tmp;
    if(good)
    {

    }
    else
    {

    }
    return tmp;
}


std::string http_cmd_resolver(const httpparser::Request &request)
{
    std::string tmp;
    if(request.method == "GET")
    {
        std::string fl_path(dir_opt);
//        if(fl_path[fl_path.size() - 1] != '/')
//        {
//            fl_path += '/';
//        }
        fl_path += request.uri;


        std::ifstream fst(fl_path.c_str());
        if(fst.is_open())
        {
            tmp = make_http_header(true);

            while(1)
            {
                std::string str("");

                std::getline(fst, str);

                if(str != "")
                {
                    tmp += str;
                }

            }
        }
        else
        {
            tmp = make_http_header(false);
        }
        fst.close();

        tmp += "\r\n";

    }
    else if(request.method == "POST")
    {
        int i = 0;
    }
    return tmp;

}


void client_thread(thread_param *param)
{
    int fd = param->fd;

    int buf_len_in = 1000;
    int len_out = 0;
    int read_len = 0;
    char buf_in[buf_len_in];
    char buf_out[buf_len_in];

    fd_set rfds;
    struct timeval tm;
    int retval;


    while(param->need_close == 0)
    {
        memset(buf_in, 0, buf_len_in);
        tm.tv_sec = 0;
        tm.tv_usec = 500;

        FD_ZERO(&rfds);
        FD_SET(fd, &rfds);

        retval = select(fd + 1, &rfds, NULL, NULL, &tm);
        if(retval)
        {
            if(FD_ISSET(fd, &rfds))
            {
                read_len = read(fd, buf_in, buf_len_in);
                std::cout << buf_in << std::endl;

                if(read_len < 0)
                {
                    shutdown(fd, SHUT_RDWR);
                    break;
                }
                else
                {
                    httpparser::Request request;
                    httpparser::HttpRequestParser parser;

                    httpparser::HttpRequestParser::ParseResult res = parser.parse(request, buf_in,
                                                                                  buf_in + strlen(buf_in));

                    if(res == httpparser::HttpRequestParser::ParsingCompleted)
                    {
                        std::cout << request.inspect() << std::endl;

                        std::string ans = http_cmd_resolver(request);

                        if(send(fd, ans.c_str(), strlen(ans.c_str()), 0) < 0)
                        {
                            shutdown(fd, SHUT_RDWR);
                            break;
                        }

                    }
                    else
                    {
                        std::cerr << "Parsing failed" << std::endl;

                    }
                }
            }
        }
    }


    param->closed = 1;

}

int main(int argc, char *argv[])
{
    int port_opt;
    char *ip_opt;

    std::cout << "start" << std::endl;

    int opindex;
    char opchar;
    struct option opts[] = {
            {"ip",   required_argument, 0, 0},
            {"port", required_argument, 0, 0},
            {"dir",  required_argument, 0, 0},
            {0, 0,                      0, 0,}
    };

    while((opchar = getopt_long(argc, argv, "h:p:d:", opts, &opindex)) != -1)
    {
        switch(opchar)
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

    if(bind(server_sock, (struct sockaddr *) &addr, sizeof(addr)) < 0)
    {
        perror("bind error");
        return 1;
    }

    listen(server_sock, 100);

    std::vector<thread_param *> vc;
    while(1)
    {
        int client_fd = accept(server_sock, 0, 0);


        thread_param *tmp = new thread_param();

        vc.push_back(tmp);

        vc.back()->fd = client_fd;

        //make thread

        vc.back()->th = std::thread(client_thread, vc.back());

        auto it = vc.begin();
        while(it != vc.end())
        {
            if((*it)->closed == 1)
            {
                (*it)->th.join();
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
