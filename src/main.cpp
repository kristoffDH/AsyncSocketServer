#include "spdlog/spdlog.h"

int main() {

    spdlog::set_level(spdlog::level::info);


    spdlog::debug("Welcome to spdlog!");

    return 0;
}
