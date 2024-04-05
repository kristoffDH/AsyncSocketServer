
#include "socket_server.h"
#include "spdlog/spdlog.h"

#include <csignal>

int main() {
    // debug level init
    spdlog::set_level(spdlog::level::debug);

    socket_server::Server server(9000, 16);

    // signal setting
    signal(SIGTERM, socket_server::Server::SignalHandler);

    spdlog::info("Tcp Server Start");

    server.Bind();
    server.Listen();
    server.RunServer();

    return 0;
}
