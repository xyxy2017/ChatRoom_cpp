#ifndef __TCPSOCKET__
#define __TCPSOCKET__
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#define ERROR_CHECK(ret,num,msg){if(ret==num){\
    perror(msg);return -1;\
}}

using namespace std;

class TcpSocket {
public:
    TcpSocket();
    TcpSocket(int socket) : m_fd(socket) {};
    ~TcpSocket();

    int connect_to_host(string ip, unsigned short port);
    int sendMsg(string msg);
    string recvMsg();
private:
    int writen(const char* msg, int size);
    int readn(char* buf, int size);
private:
    int m_fd;   // socket
};

TcpSocket::TcpSocket() {
    m_fd = socket(AF_INET, SOCK_STREAM, 0);
}
// TcpSocket::TcpSocket(int socket) : m_fd(socket) { };
TcpSocket::~TcpSocket() {
    if (m_fd > 0)   close(m_fd);
}

int TcpSocket::connect_to_host(string ip, unsigned short port) {
    sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);
    inet_pton(AF_INET, ip.data(), &saddr.sin_addr.s_addr);
    int ret = connect(m_fd, (struct sockaddr*)&saddr, sizeof(saddr));
    ERROR_CHECK(ret, -1, "connect");
    cout << "connect to server success..." << endl;
    return ret;
}

int TcpSocket::sendMsg(string msg) {
    // 申请内存空间：数据长度 + 4字节（用于存储数据长度）
    char* data = new char[msg.size() + 4];
    int bigLen = htonl(msg.size());
    memcpy(data, &bigLen, 4);   // 存放数据长度
    memcpy(data + 4, msg.data(), msg.size());   // 存放数据
    // 发送数据
    int ret = writen(data, msg.size() + 4);
    delete[] data;
    return ret;
}

string TcpSocket::recvMsg() {
    // 1.接收数据头
    int len = 0;
    int ret = readn((char*)&len, 4);
    // if (ret == 0)   return string();
    len = ntohl(len);
    cout << "the data size:" << len << endl;
    // 2.接收数据
    char* buf = new char[len + 1];
    ret = readn(buf, len);
    if (ret != len) {
        return string();
    }
    buf[len] = '\0';
    string retStr(buf);
    delete[] buf;

    return retStr;
}

int TcpSocket::writen(const char* msg, int size) {
    int len = size;
    int nwrite = 0;
    const char* p = msg;
    while (len > 0) {
        if ((nwrite = write(m_fd, p, len)) > 0) {
            len -= nwrite;
            p += nwrite;
        } else if (nwrite == -1) {
            return -1;
        }
    }
    return size;
}
int TcpSocket::readn(char* buf, int size) {
    int len = size;
    int nread = 0;
    char* p = buf;
    while (len > 0) {
        if ((nread = read(m_fd, p, len)) > 0) {
            p += nread;
            len -= nread;
        } else if (nread == -1 || nread == 0) {
            return -1;
        }
    }
    return size;
}
#endif