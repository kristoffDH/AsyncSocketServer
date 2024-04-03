//
// Created by kristoff on 4/1/24.
//

#include "socket_server.h"
#include "spdlog/spdlog.h"

namespace socket_server {
Server::~Server() { Close(); }

void Server::SetSockOption() {
    int n_ret = 0;
    int n_opt = 0;
    int n_reuse = 1;
    struct linger opt_linger {};

    // create server socket
    if ((server_fd = socket(AF_INET, server_type, 0)) < 0) {
        spdlog::error("Create server socket fail..");
        Close();
        exit(-1);
    }

    // Set socket option : Reuse Address
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&n_reuse,
                   sizeof(int)) < 0) {
        spdlog::error("setsockopt Reuse Address fail..");
        Close();
        exit(-1);
    }

    // Set Socket Option : Re-Use Port
    n_opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &n_opt, sizeof(n_opt)) <
        0) {
        spdlog::error("setsockopt Reuse Port fail..");
        Close();
        exit(-1);
    }

    // Set Socket Option : Linger : When Socket Closed, Prevent Time_Wait Status
    // of Socket
    opt_linger.l_onoff = 1;
    opt_linger.l_linger = 0;

    if (setsockopt(server_fd, SOL_SOCKET, SO_LINGER, (char *)&opt_linger,
                   sizeof(opt_linger)) < 0) {
        spdlog::error("setsockopt Linger fail..");
        Close();
        exit(-1);
    }

    // Set Socket Option : Recv. Socket Buffer
    n_opt = 8 * 1024 * 1024;
    if (setsockopt(server_fd, SOL_SOCKET, SO_RCVBUF, &n_opt, sizeof(n_opt)) <
        0) {
        spdlog::error("setsockopt Recv Socket Buffer fail..");
        Close();
        exit(-1);
    }

    // Set Socket Option : Send. Socket Buffer
    if (setsockopt(server_fd, SOL_SOCKET, SO_SNDBUF, &n_opt, sizeof(n_opt)) <
        0) {
        spdlog::error("setsockopt Send Socket Buffer fail..");
        Close();
        exit(-1);
    }
}

void Server::Open() {
    struct sockaddr_in sock_in_addr {};

    SetSockOption();

    // Binding
    sock_in_addr.sin_family = AF_INET;
    sock_in_addr.sin_port = htons(server_port);
    sock_in_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(server_fd, (struct sockaddr *)&sock_in_addr,
             sizeof(sock_in_addr))) {
        Close();
        exit(-1);
    }

    // Listen
    if (listen(server_fd, kMaxTcpListen) < 0) {
        Close();
        exit(-1);
    }
}

void Server::Close() const { close(server_fd); }

void Server::Accept() {

}

}  // namespace socket_server