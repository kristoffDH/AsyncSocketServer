//
// Created by kristoff on 4/1/24.
//

#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cerrno>
#include <functional>
#include <string>

#ifndef DEFAULT_BUFFER_SIZE
#define DEFAULT_BUFFER_SIZE 0x1000 /*4096 bytes*/
#endif

namespace socket_server {
enum SocketType { TCP = SOCK_STREAM, UDP = SOCK_DGRAM };

class Server {
  public:
    Server(SocketType type, uint16_t port)
        : server_fd(0), server_type(type), server_port(port){};
    ~Server();

    Server() = delete;
    Server(const Server& server) = delete;
    Server(Server&& server) = delete;
    Server& operator=(Server&& other) = delete;

  private:
    int server_fd;
    SocketType server_type;
    uint16_t server_port;

    const int kMaxTcpListen = 128;

    void SetSockOption();

  public:
    void Open();
    void Close() const;
    void Accept();
};

}  // namespace socket_server

#endif
