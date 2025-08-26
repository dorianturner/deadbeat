#include "audio.hpp"
#include "utils.hpp"
#include "ast.hpp"
#include <memory>

using namespace deadbeat;

int main() {
    utils::log("Deadbeat starting...");

    AudioEngine engine;
    try {
        engine.initialize();

        // First chord: A4, C#5, E5
        auto noteA4 = std::make_shared<Note>();
        noteA4->name = "A4"; noteA4->freq = 440.0; noteA4->duration = 2.0; noteA4->amplitude = 0.3; noteA4->waveform = WaveformType::Sine;
        auto noteC5 = std::make_shared<Note>();
        noteC5->name = "C#5"; noteC5->freq = 550.0; noteC5->duration = 2.0; noteC5->amplitude = 0.3; noteC5->waveform = WaveformType::Sine;
        auto noteE5 = std::make_shared<Note>();
        noteE5->name = "E5"; noteE5->freq = 660.0; noteE5->duration = 2.0; noteE5->amplitude = 0.3; noteE5->waveform = WaveformType::Sine;

        auto chord1 = std::make_shared<Parallel>();
        chord1->children.push_back(noteA4);
        chord1->children.push_back(noteC5);
        chord1->children.push_back(noteE5);

        // Second chord: F4, A4, C5
        auto noteF4 = std::make_shared<Note>();
        noteF4->name = "F4"; noteF4->freq = 349.23; noteF4->duration = 2.0; noteF4->amplitude = 0.3; noteF4->waveform = WaveformType::Sine;
        auto noteA4b = std::make_shared<Note>();
        noteA4b->name = "A4"; noteA4b->freq = 440.0; noteA4b->duration = 2.0; noteA4b->amplitude = 0.3; noteA4b->waveform = WaveformType::Sine;
        auto noteC5b = std::make_shared<Note>();
        noteC5b->name = "C5"; noteC5b->freq = 523.25; noteC5b->duration = 2.0; noteC5b->amplitude = 0.3; noteC5b->waveform = WaveformType::Sine;

        auto chord2 = std::make_shared<Parallel>();
        chord2->children.push_back(noteF4);
        chord2->children.push_back(noteA4b);
        chord2->children.push_back(noteC5b);

        // Wrap chords in a sequence
        auto song = std::make_shared<Sequence>();
        song->children.push_back(chord1);
        song->children.push_back(chord2);

        // Mix and play
        engine.mix(song, 120);

        engine.shutdown();
    } catch (const std::exception& ex) {
        utils::log(std::string("Audio error: ") + ex.what());
    }

    utils::log("Done.");
    return 0;
}

