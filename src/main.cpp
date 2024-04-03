
#include "socket_server.h"
#include "spdlog/spdlog.h"

int main() {
    // debug level init
    spdlog::set_level(spdlog::level::debug);

    socket_server::Server server(socket_server::SocketType::TCP, 9000);

    spdlog::info("Tcp Server Start");

    server.Open();
    server.Accept();

    return 0;
}
