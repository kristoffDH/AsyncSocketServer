
#include <csignal>

#include "socket_server.h"
#include "spdlog/spdlog.h"

int main() {
    // debug level init
    spdlog::set_level(spdlog::level::debug);

    socket_server::Server server(9000, 16);

    // signal setting
    signal(SIGTERM, socket_server::Server::SignalHandler);

    spdlog::info("Tcp Server Start");

    server.on_communication = [](std::string thread_id, int client_fd) {
        char message_buffer[4096];
        size_t recv_bytes = 0, send_bytes = 0;

        for (int idx = 0; idx < 8; idx++) {
            recv_bytes = recv(client_fd, &message_buffer, 4096, 0);
            if (recv_bytes == 0) {
                spdlog::warn("[{}] Recv fail. Client is disconnected..",
                             thread_id);
                throw socket_server::Exception("socket error. recv_byte is 0");
            }
            spdlog::debug("[{}] Recv data : {}", thread_id,
                          std::string(message_buffer));

            send_bytes = send(client_fd, &message_buffer, recv_bytes, 0);
            if (send_bytes == -1) {
                spdlog::warn("[{}] Send fail.", thread_id);
                break;
            }
        }
    };

    server.on_error_handle = [](std::string thread_id,
                                socket_server::Exception e) {
        spdlog::error("[{}] socket err : {}", thread_id, e.what());
    };

    server.Bind();
    server.Listen();
    server.RunServer();

    return 0;
}
