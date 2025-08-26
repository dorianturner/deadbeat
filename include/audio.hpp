#pragma once
#include <string>

namespace deadbeat {
    class AudioEngine {
    public:
        void playTone(double frequency, double duration, double amplitude);
        void loadSample(const std::string& path);
        void playSample(const std::string& name);
    };
}
