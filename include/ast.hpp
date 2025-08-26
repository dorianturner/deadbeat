#pragma once
#include <string>
#include <vector>
#include <memory>

namespace deadbeat {

// ---------- Waveforms ----------
enum class WaveformType { Sine, Square, Saw, Triangle };

// ---------- Base Node ----------
struct Node {
    virtual ~Node() = default;
};

// ---------- Atoms ----------
struct Note : Node {
    std::string name;   // e.g. "C4"
    double freq;
    double duration;    // beats
    double amplitude;
    WaveformType waveform;
};

struct Sample : Node {
    std::string name;   // e.g. "piano_C4"
    std::string path;   // e.g. "piano_C4.wav"
};

// Reference to something in scope (local or imported)
struct Identifier : Node {
    std::string name;
};

// ---------- Containers ----------
struct Sequence : Node {
    std::vector<std::shared_ptr<Node>> children;
};

struct Parallel : Node {
    std::vector<std::shared_ptr<Node>> children;
};

// ---------- Modifiers / transforms ----------
struct Transpose : Node {
    std::shared_ptr<Node> target;
    int semitones;
};

struct Repeat : Node {
    std::shared_ptr<Node> target;
    int times;
};

struct Amp : Node {
    std::shared_ptr<Node> target;
    double factor;
};

struct FreezeBpm : Node {
    std::shared_ptr<Node> target;
    int bpm;
};

// ---------- Program-level ----------
struct Import : Node {
    std::string module;                 // e.g. "riffs.db"
    std::vector<std::string> names;     // e.g. { "riff1", "riff2" }
};

struct Run : Node {
    std::shared_ptr<Node> target;
    int bpm;
};

// Root program
struct Program : Node {
    std::vector<std::shared_ptr<Node>> statements;
};

} // namespace deadbeat
