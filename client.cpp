#include "TcpSocket.hpp"

int main() {
    TcpSocket tcp;
    int ret = tcp.connect_to_host("127.0.0.1", 10000);
    ERROR_CHECK(ret, -1, "connect");
    char buf[1024];
    while (1) {
        memset(buf, 0, sizeof(buf));
        ret = read(STDIN_FILENO, buf, sizeof(buf));
        if (ret == 0) {
            printf("the link is closed\n");
            break;
        }
        tcp.sendMsg(buf);
        usleep(300);
    }
    sleep(10);
    return 0;
}