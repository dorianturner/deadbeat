#include "interpreter.hpp"
#include "utils.hpp"

using namespace deadbeat;

void Interpreter::run(Node* program, int bpm) {
    utils::log("Interpreter running with bpm = " + std::to_string(bpm));
}
