#pragma once
#include <string>
#include <vector>
#include <portaudio.h>
#include "ast.hpp"

namespace deadbeat {

class AudioEngine {
public:
    AudioEngine();
    ~AudioEngine();

    void initialize();
    void shutdown();

    // Low-level single note play
    void playTone(double frequency, double duration, double amplitude, WaveformType waveform);

    // Sample support (stub)
    void loadSample(const std::string& path);
    void playSample(const std::string& name);

    // Mix / render AST into PCM and play
    void mix(std::shared_ptr<Node> node, int bpm);

    void stopAll();

private:
    bool initialized_;
    PaStreamParameters outParams_;
    double sampleRate_;

    // Recursive helpers
    void renderNode(std::shared_ptr<Node> node, double beatDuration, std::vector<float>& buffer);
    void renderSequence(std::shared_ptr<Sequence> seq, double beatDuration, std::vector<float>& buffer);
    void renderParallel(std::shared_ptr<Parallel> par, double beatDuration, std::vector<float>& buffer);
};

} // namespace deadbeat

