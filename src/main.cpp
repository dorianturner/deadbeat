#include "parser.hpp"
#include "interpreter.hpp"
#include "utils.hpp"
#include "audio.hpp"

using namespace deadbeat;

int main() {
    utils::log("Deadbeat starting...");

    // Initialize audio engine and play a tone for 2 seconds
    AudioEngine engine;
    try {
        engine.initialize();
        engine.playTone(440.0, 2.0, 0.5, WaveformType::Sine); // A4 for 2s
        engine.shutdown();
    } catch (const std::exception& ex) {
        utils::log(std::string("Audio error: ") + ex.what());
    }

    // Existing parser/interpreter flow (stubs)
    Parser parser;
    Node* program = parser.parse("example song");

    Interpreter interp;
    interp.run(program, 120);

    utils::log("Done.");
    return 0;
}

