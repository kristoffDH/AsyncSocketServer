//
// Created by kristoff on 4/1/24.
//

#include "socket_server.h"

#include <sstream>

#include "spdlog/spdlog.h"

namespace socket_server {
bool Server::is_running = false;

Server::~Server() {}

void Server::SetSockOption() {
    int n_ret = 0;
    int n_opt = 0;
    int n_reuse = 1;
    struct linger opt_linger {};

    // create server socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
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

void Server::WorkCommunication() {
    auto thread_id = IdToString(std::this_thread::get_id());

    spdlog::info("[{}] Start thread.", thread_id);

    while (is_running) {
        int client_fd = session_queue.Pop();

        if (client_fd == -1) {
            spdlog::info("[{}] server is stop.", thread_id);
            continue;
        }

        try {
            if(on_communication != nullptr) {
                on_communication(thread_id, client_fd);
            }
        }
        catch(Exception & e) {
            if(on_error_handle != nullptr) {
                on_error_handle(thread_id, e);
            }
        }

        close(client_fd);
        spdlog::debug("[{}] Client Disconnect.", thread_id);
    }
}

void Server::Bind() {
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
}

void Server::Listen() {
    // Listen
    if (listen(server_fd, kMaxTcpListen) < 0) {
        Close();
        exit(-1);
    }

    spdlog::info("Server Listening start.");
}

void Server::RunServer() {
    is_running = true;
    // run accept thread
    accept_thread = std::thread(&Server::Accept, this);

    // run communication thread
    for (int idx = 0; idx < work_thread_num; idx++) {
        work_thread.push_back(std::thread(&Server::WorkCommunication, this));
    }

    while (is_running) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    Close();
}

void Server::Accept() {
    while (is_running) {
        struct sockaddr_in sock_addr_in {};
        int addrLen = sizeof(sock_addr_in);
        int client_fd = accept(server_fd, (struct sockaddr *)&sock_addr_in,
                               (socklen_t *)&addrLen);

        if (client_fd < 0) {
            spdlog::warn("Accept thread terminated. client_fd < 0");
            return;
        }

        spdlog::info("Accept Client : {}", IpToString(sock_addr_in));
        session_queue.Push(client_fd);
    }
}

void Server::Close() {
    is_running = false;

    while (!session_queue.IsEmpty()) {
        int client_fd = session_queue.Pop();
        spdlog::debug("client fd {} close...", client_fd);
        close(client_fd);
    }

    spdlog::debug("server_fd Close");
    close(server_fd);

    // unlock queue
    for (int cnt = 0; cnt < work_thread_num; cnt++) {
        session_queue.Push(-1);
    }

    // accept, work_thread wait
    accept_thread.join();
    for (auto &iter : work_thread) {
        iter.join();
    }

    spdlog::debug("accept/work thread is terminated");
}

void Server::SignalHandler(int signum) {
    spdlog::warn("Receive signal {}.", signum);
    is_running = false;
}

std::string Server::IpToString(const sockaddr_in &addr) {
    char ip[INET_ADDRSTRLEN] = {
        0,
    };
    inet_ntop(AF_INET, &(addr.sin_addr), ip, INET_ADDRSTRLEN);

    return std::string(ip);
}

std::string Server::IdToString(const std::thread::id thread_id) {
    std::ostringstream ss;
    ss << std::this_thread::get_id();
    return ss.str();
}

}  // namespace socket_server