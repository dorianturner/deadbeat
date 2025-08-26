#include "audio.hpp"
#include "utils.hpp"

using namespace deadbeat;

void AudioEngine::playTone(double frequency, double duration, double amplitude) {
    utils::log("Playing tone: " + std::to_string(frequency) + "Hz");
}

void AudioEngine::loadSample(const std::string& path) {
    utils::log("Loading sample: " + path);
}

void AudioEngine::playSample(const std::string& name) {
    utils::log("Playing sample: " + name);
}
