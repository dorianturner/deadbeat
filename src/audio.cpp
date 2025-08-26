#include "audio.hpp"
#include "utils.hpp"
#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace deadbeat {

AudioEngine::AudioEngine()
: initialized_(false), sampleRate_(44100.0) {}

AudioEngine::~AudioEngine() { if (initialized_) shutdown(); }

void AudioEngine::initialize() {
    PaError err = Pa_Initialize();
    if (err != paNoError)
        throw std::runtime_error(std::string("Pa_Initialize failed: ") + Pa_GetErrorText(err));
    initialized_ = true;
    utils::log("AudioEngine initialized");
}

void AudioEngine::shutdown() {
    Pa_Terminate();
    initialized_ = false;
    utils::log("AudioEngine shutdown");
}

// ---------- Low-level single note ----------
void AudioEngine::playTone(double frequency, double duration, double amplitude, WaveformType waveform) {
    if (!initialized_) return;

    PaStream* stream = nullptr;
    PaDeviceIndex dev = Pa_GetDefaultOutputDevice();
    if (dev == paNoDevice) return;

    outParams_.device = dev;
    outParams_.channelCount = 1;
    outParams_.sampleFormat = paFloat32;
    outParams_.suggestedLatency = Pa_GetDeviceInfo(dev)->defaultLowOutputLatency;
    outParams_.hostApiSpecificStreamInfo = nullptr;

    Pa_OpenStream(&stream, nullptr, &outParams_, sampleRate_, paFramesPerBufferUnspecified, paClipOff, nullptr, nullptr);
    Pa_StartStream(stream);

    const unsigned long totalFrames = static_cast<unsigned long>(duration * sampleRate_);
    std::vector<float> buffer(totalFrames);

    double phase = 0.0;
    double inc = 2.0 * M_PI * frequency / sampleRate_;
    for (unsigned long i = 0; i < totalFrames; ++i) {
        double sample = 0.0;
        switch (waveform) {
            case WaveformType::Sine: sample = std::sin(phase); break;
            case WaveformType::Square: sample = (std::sin(phase) >= 0) ? 1.0 : -1.0; break;
            case WaveformType::Saw: sample = 2.0 * (phase/(2*M_PI) - std::floor(phase/(2*M_PI)+0.5)); break;
            case WaveformType::Triangle: sample = 2.0 * std::abs(2.0*(phase/(2*M_PI)-std::floor(phase/(2*M_PI)+0.5))) -1.0; break;
        }
        buffer[i] = static_cast<float>(sample * amplitude);
        phase += inc;
        if (phase >= 2.0*M_PI) phase -= 2.0*M_PI;
    }

    Pa_WriteStream(stream, buffer.data(), totalFrames);
    Pa_StopStream(stream);
    Pa_CloseStream(stream);
}

// ---------- Stub samples ----------
void AudioEngine::loadSample(const std::string& path) { utils::log("loadSample stub: "+path); }
void AudioEngine::playSample(const std::string& name) { utils::log("playSample stub: "+name); }

// ---------- Mix / render AST ----------
void AudioEngine::mix(std::shared_ptr<Node> node, int bpm) {
    if (!initialized_ || !node) return;

    double beatDuration = 60.0 / bpm; // seconds per beat
    std::vector<float> buffer;
    renderNode(node, beatDuration, buffer);

    // Play buffer via PortAudio
    PaStream* stream = nullptr;
    PaDeviceIndex dev = Pa_GetDefaultOutputDevice();
    if (dev == paNoDevice) return;

    outParams_.device = dev;
    outParams_.channelCount = 1;
    outParams_.sampleFormat = paFloat32;
    outParams_.suggestedLatency = Pa_GetDeviceInfo(dev)->defaultLowOutputLatency;
    outParams_.hostApiSpecificStreamInfo = nullptr;

    Pa_OpenStream(&stream, nullptr, &outParams_, sampleRate_, paFramesPerBufferUnspecified, paClipOff, nullptr, nullptr);
    Pa_StartStream(stream);
    Pa_WriteStream(stream, buffer.data(), buffer.size());
    Pa_StopStream(stream);
    Pa_CloseStream(stream);
    utils::log("Mix finished");
}

// ---------- Recursive render helpers ----------
void AudioEngine::renderNode(std::shared_ptr<Node> node, double beatDuration, std::vector<float>& buffer) {
    if (!node) return;

    if (auto n = std::dynamic_pointer_cast<Note>(node)) {
        unsigned long frames = static_cast<unsigned long>(n->duration * beatDuration * sampleRate_);
        double phase=0, inc=2*M_PI*n->freq/sampleRate_;
        std::vector<float> noteBuf(frames);
        for (unsigned long i=0;i<frames;++i){
            double sample=0;
            switch(n->waveform){
                case WaveformType::Sine: sample=std::sin(phase); break;
                case WaveformType::Square: sample=(std::sin(phase)>=0)?1:-1; break;
                case WaveformType::Saw: sample=2.0*(phase/(2*M_PI)-std::floor(phase/(2*M_PI)+0.5)); break;
                case WaveformType::Triangle: sample=2.0*std::abs(2.0*(phase/(2*M_PI)-std::floor(phase/(2*M_PI)+0.5)))-1.0; break;
            }
            noteBuf[i]=static_cast<float>(sample*n->amplitude);
            phase+=inc; if(phase>=2*M_PI) phase-=2*M_PI;
        }
        buffer.insert(buffer.end(), noteBuf.begin(), noteBuf.end());
    } else if (auto s = std::dynamic_pointer_cast<Sequence>(node)) {
        renderSequence(s, beatDuration, buffer);
    } else if (auto p = std::dynamic_pointer_cast<Parallel>(node)) {
        renderParallel(p, beatDuration, buffer);
    } else if (auto id = std::dynamic_pointer_cast<Identifier>(node)) {
        utils::log("Identifier render stub: "+id->name);
    }
}

void AudioEngine::renderSequence(std::shared_ptr<Sequence> seq, double beatDuration, std::vector<float>& buffer){
    for(auto& child : seq->children){
        std::vector<float> tmp;
        renderNode(child, beatDuration, tmp);
        buffer.insert(buffer.end(), tmp.begin(), tmp.end());
    }
}

void AudioEngine::renderParallel(std::shared_ptr<Parallel> par, double beatDuration, std::vector<float>& buffer){
    std::vector<std::vector<float>> childBuffers;
    size_t maxSize = 0;

    // Render all children separately
    for(auto& child : par->children){
        std::vector<float> tmp;
        renderNode(child, beatDuration, tmp);
        maxSize = std::max(maxSize, tmp.size());
        childBuffers.push_back(std::move(tmp));
    }

    // Resize buffer to maxSize
    buffer.resize(maxSize, 0.0f);

    // Sum each child buffer into buffer, pad shorter ones with zeros
    for(auto& cb : childBuffers){
        for(size_t i = 0; i < cb.size(); ++i) buffer[i] += cb[i];
    }
}

void AudioEngine::stopAll(){ utils::log("stopAll stub"); }

} // namespace deadbeat
