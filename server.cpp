#include "TcpServer.hpp"
#include "TcpSocket.hpp"
#include <pthread.h>
#include <memory>
struct SockInfo {
    TcpServer* srv;
    // TcpSocket* tcp;
    unique_ptr<TcpSocket> tcp;
    sockaddr_in addr;
};

void* working(void* arg) {
    SockInfo* info = static_cast<SockInfo*>(arg);
    cout << "socket connect success, ip: "
         << inet_ntoa(info->addr.sin_addr)
         << " port: " << ntohs(info->addr.sin_port) << endl; 

    string s;
    while (1) {
        cout << "start recv msg..." << endl;
        string s = info->tcp->recvMsg();
        if (s.empty()) {
            break;
        }
        cout << s << endl;
    }
    cout << "end recv msg..." << endl;
    // delete info->tcp;
    delete info;
    return nullptr;
}

int main() {
    TcpServer srv;
    srv.set_listen(10000);
    while (1) {
        SockInfo *info = new SockInfo;
        TcpSocket* c = srv.accept_conn(&info->addr);
        if (c == nullptr) {
            cout << "connect failed, retry..." << endl;
            continue;
        }
        unique_ptr<TcpSocket> c_ptr(c);
        
        info->tcp = move(c_ptr);
        info->srv = &srv;
        pthread_t pid;
        pthread_create(&pid, NULL, working, info);
        pthread_detach(pid);
    }
    cout << "the server exit..." << endl;
    return 0;

}