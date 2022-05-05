#include "TcpSocket.hpp"

int main() {
    TcpSocket tcp;
    int ret = tcp.connect_to_host("127.0.0.1", 10000);
    ERROR_CHECK(ret, -1, "connect");
    char buf[1024];
    for (int i = 0; i < 10; ++i) {
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "this is send msg: %d\n", i);

        tcp.sendMsg(buf);
        usleep(300);
    }
    sleep(10);
    return 0;
}