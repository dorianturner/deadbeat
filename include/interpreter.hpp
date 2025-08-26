#pragma once
#include "ast.hpp"

namespace deadbeat {
    class Interpreter {
    public:
        void run(Node* program, int bpm);
    };
}
