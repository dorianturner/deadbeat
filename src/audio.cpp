#include "audio.hpp"
#include "utils.hpp"
#include <iostream>
#include <cmath>
#include <vector>
#include <stdexcept>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace deadbeat {

AudioEngine::AudioEngine()
: initialized_(false),
  sampleRate_(44100.0)
{
    // outParams_ will be filled in initialize()
}

AudioEngine::~AudioEngine() {
    if (initialized_) shutdown();
}

void AudioEngine::initialize() {
    PaError err = Pa_Initialize();
    if (err != paNoError) {
        throw std::runtime_error(std::string("Pa_Initialize failed: ") + Pa_GetErrorText(err));
    }
    initialized_ = true;
    utils::log("AudioEngine initialized (PortAudio)");
}

void AudioEngine::shutdown() {
    PaError err = Pa_Terminate();
    if (err != paNoError) {
        utils::log(std::string("Pa_Terminate error: ") + Pa_GetErrorText(err));
    } else {
        utils::log("AudioEngine shutdown (PortAudio)");
    }
    initialized_ = false;
}

void AudioEngine::playTone(double frequency, double duration, double amplitude, WaveformType waveform) {
    if (!initialized_) {
        utils::log("AudioEngine not initialized. Call initialize() first.");
        return;
    }
    if (duration <= 0.0) {
        utils::log("playTone called with non-positive duration.");
        return;
    }

    std::string waveName;
    switch (waveform) {
        case WaveformType::Sine: waveName = "Sine"; break;
        case WaveformType::Square: waveName = "Square"; break;
        case WaveformType::Saw: waveName = "Saw"; break;
        default: waveName = "Unknown"; break;
    }

    utils::log("playTone: freq=" + std::to_string(frequency) +
               "Hz dur=" + std::to_string(duration) + "s amp=" + std::to_string(amplitude) +
               " wave=" + waveName);

    PaStream* stream = nullptr;
    PaError err;

    // Setup output parameters (default output device)
    PaDeviceIndex dev = Pa_GetDefaultOutputDevice();
    if (dev == paNoDevice) {
        utils::log("No default output device.");
        return;
    }

    outParams_.device = dev;
    outParams_.channelCount = 1; // mono for simplicity
    outParams_.sampleFormat = paFloat32;
    outParams_.suggestedLatency = Pa_GetDeviceInfo(dev)->defaultLowOutputLatency;
    outParams_.hostApiSpecificStreamInfo = nullptr;

    unsigned long framesPerBuffer = 256; // small buffer for low-latency
    double sampleRate = sampleRate_;

    err = Pa_OpenStream(
        &stream,
        nullptr,               // no input
        &outParams_,
        sampleRate,
        framesPerBuffer,
        paClipOff,
        nullptr,               // no callback -> blocking API
        nullptr
    );
    if (err != paNoError) {
        utils::log(std::string("Pa_OpenStream failed: ") + Pa_GetErrorText(err));
        return;
    }

    err = Pa_StartStream(stream);
    if (err != paNoError) {
        utils::log(std::string("Pa_StartStream failed: ") + Pa_GetErrorText(err));
        Pa_CloseStream(stream);
        return;
    }

    // Generate and write samples in chunks
    const unsigned long totalFrames = static_cast<unsigned long>(duration * sampleRate);
    std::vector<float> buffer(framesPerBuffer);
    double phase = 0.0;
    double phaseIncrement = 2.0 * M_PI * frequency / sampleRate;

    unsigned long framesWritten = 0;
    while (framesWritten < totalFrames) {
        unsigned long framesThisBuffer = std::min<unsigned long>(framesPerBuffer, totalFrames - framesWritten);
        for (unsigned long i = 0; i < framesThisBuffer; ++i) {
            double t = (static_cast<double>(framesWritten + i)) / sampleRate;
            double sample = 0.0;
            switch (waveform) {
                case WaveformType::Sine:
                    sample = std::sin(phase);
                    break;
                case WaveformType::Square:
                    sample = (std::sin(phase) >= 0.0) ? 1.0 : -1.0;
                    break;
                case WaveformType::Saw: {
                    // naive saw using phase normalized to [-1,1]
                    double normalized = (phase / (2.0 * M_PI));
                    // keep normalized in [-0.5, 0.5)
                    normalized = normalized - std::floor(normalized + 0.5);
                    sample = 2.0 * normalized;
                    break;
                }
                default:
                    sample = std::sin(phase);
            }
            buffer[i] = static_cast<float>(sample * amplitude);
            phase += phaseIncrement;
            if (phase >= 2.0 * M_PI) phase -= 2.0 * M_PI;
        }

        // Write to stream (blocking)
        err = Pa_WriteStream(stream, buffer.data(), framesThisBuffer);
        if (err != paNoError) {
            utils::log(std::string("Pa_WriteStream error: ") + Pa_GetErrorText(err));
            break;
        }
        framesWritten += framesThisBuffer;
    }

    // Drain and close
    err = Pa_StopStream(stream);
    if (err != paNoError) {
        utils::log(std::string("Pa_StopStream error: ") + Pa_GetErrorText(err));
    }
    Pa_CloseStream(stream);
    utils::log("playTone finished");
}

void AudioEngine::loadSample(const std::string& path) {
    utils::log("Loading sample (stub): " + path);
    // TODO: implement libsndfile-based loading
}

void AudioEngine::playSample(const std::string& name) {
    utils::log("Playing sample (stub): " + name);
    // TODO: implement sample playback via PortAudio
}

void AudioEngine::mix() {
    utils::log("Mix (stub)");
}

void AudioEngine::stopAll() {
    utils::log("StopAll (stub)");
}

} // namespace deadbeat
