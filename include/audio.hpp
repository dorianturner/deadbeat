#pragma once
#include <string>
#include <vector>
#include <portaudio.h>

namespace deadbeat {

enum class WaveformType { Sine, Square, Saw };

class AudioEngine {
    public:
        AudioEngine();
        ~AudioEngine();

        void initialize();
        void shutdown();

        // Blocking play: generates samples and writes them to the output device.
        void playTone(double frequency, double duration, double amplitude, WaveformType waveform);

        // Sample support (stubbed for now)
        void loadSample(const std::string& path); // support .wav files
        void playSample(const std::string& name);

        void mix();
        void stopAll();

    private:
        bool initialized_;
        PaStreamParameters outParams_;
        double sampleRate_;
};

} // namespace deadbeat
