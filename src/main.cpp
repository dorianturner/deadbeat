#include "parser.hpp"
#include "interpreter.hpp"
#include "utils.hpp"

using namespace deadbeat;

int main() {
    utils::log("Deadbeat starting...");

    Parser parser;
    Node* program = parser.parse("example song");

    Interpreter interp;
    interp.run(program, 120);

    utils::log("Done.");
    return 0;
}
