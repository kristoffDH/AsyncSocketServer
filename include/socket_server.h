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
#include <thread>

#include "thread_safe_queue.h"


namespace socket_server {

class Server {
  public:
    Server(uint16_t port, int thread_num) : server_fd(0), server_port(port), work_thread_num(thread_num){};
    ~Server();

    Server() = delete;
    Server(const Server& server) = delete;
    Server(Server&& server) = delete;
    Server& operator=(Server&& other) = delete;

  private:
    int server_fd;
    uint16_t server_port;
    static bool is_running;

    thread_safe_queue::Queue<int> session_queue;

    int work_thread_num;
    std::thread accept_thread;
    std::vector<std::thread> work_thread;

    const int kMaxTcpListen = 128;
    const int kDefaultBufferSize = 4096;

  private:
    void SetSockOption();

    static std::string IpToString(const sockaddr_in& addr);
    static std::string IdToString(const std::thread::id thread_id);

  public:
    void Bind();
    void Listen();
    void RunServer();
    void Close();
    void Accept();
    void WorkCommunication();

    static void SignalHandler(int signum);
};

}  // namespace socket_server

#endif
