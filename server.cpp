#include "TcpServer.hpp"
#include "TcpSocket.hpp"
#include <pthread.h>
#include <memory>
#include <sys/epoll.h>
struct SockInfo {
    TcpServer* srv;
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
    delete info;
    return nullptr;
}

int main() {
    TcpServer srv;
    int sfd = srv.get_fd();
    srv.set_listen(10000);

    int epfd = epoll_create1(0);
    epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = sfd;
    int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, sfd, &ev);
    ERROR_CHECK(ret, -1, "epoll_ctl");
    epoll_event ev_stdin;
    ev_stdin.events = EPOLLIN;
    ev.data.fd = STDIN_FILENO;
    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, STDIN_FILENO, &ev_stdin);

    epoll_event evs[1024];
    int size = sizeof(evs) / sizeof(epoll_event);
    while (1) {
        int num = epoll_wait(epfd, evs, size, -1);
        for (int i = 0; i < num; ++i) {
            int cur_fd = evs[i].data.fd;
            if (cur_fd == sfd) {
                SockInfo* info = new SockInfo;
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
            } else if (cur_fd == STDIN_FILENO) {
                
            }
        }

    }
    cout << "the server exit..." << endl;
    return 0;

}