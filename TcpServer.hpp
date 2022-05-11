#ifndef __TCPSERVER__
#define __TCPSERVER__
#include "TcpSocket.hpp"

using namespace std;

class TcpServer {
public:
    TcpServer();
    ~TcpServer();
    int get_fd() {
        return m_fd;
    }
    int set_listen(unsigned short port);
    TcpSocket* accept_conn(sockaddr_in* addr = nullptr);
private:
    int m_fd;
};

TcpServer::TcpServer() {
    m_fd = socket(AF_INET, SOCK_STREAM, 0);
}
TcpServer::~TcpServer() {
    close(m_fd);
}

int TcpServer::set_listen(unsigned short port) {
    sockaddr_in addrin;
    addrin.sin_family = AF_INET;
    addrin.sin_port = htons(port);
    addrin.sin_addr.s_addr = INADDR_ANY;

    int ret = bind(m_fd, (sockaddr*)&addrin, sizeof(addrin));
    ERROR_CHECK(ret,-1,"bind");
    cout << "socket bind success, ip: "
         << inet_ntoa(addrin.sin_addr)
         << " port: " << port << endl; 

    ret = listen(m_fd, 128);
    ERROR_CHECK(ret, -1, "listen");
    cout << "set listen success" << endl;
    return ret;
}

TcpSocket* TcpServer::accept_conn(sockaddr_in* addr) {
    if (addr == NULL)    return nullptr;
    socklen_t addlen = sizeof(sockaddr_in);
    int cfd = accept(m_fd, (sockaddr*)addr, &addlen);
    if (cfd == -1) {
        perror("accept");
        return nullptr;
    }
    cout << "connect client success..." << endl;
    return new TcpSocket(cfd);
}
#endif