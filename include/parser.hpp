#pragma once
#include <string>
#include "ast.hpp"

namespace deadbeat {
    class Parser {
    public:
        Node* parse(const std::string& source);
    };
}
