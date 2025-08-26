#include "utils.hpp"
#include <iostream>

using namespace deadbeat;

void utils::log(const std::string& msg) {
    std::cout << "[deadbeat] " << msg << std::endl;
}
