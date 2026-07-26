#pragma once
#include "public.sdk/source/vst/vstaudioeffect.h"

namespace Rumble {
class Processor final : public Steinberg::Vst::AudioEffect {
public:
    Processor();
    static Steinberg::FUnknown* createInstance(void*) { return (Steinberg::Vst::IAudioProcessor*) new Processor(); }
    Steinberg::tresult PLUGIN_API initialize(Steinberg::FUnknown*) override;
    Steinberg::tresult PLUGIN_API setupProcessing(Steinberg::Vst::ProcessSetup&) override;
    Steinberg::tresult PLUGIN_API process(Steinberg::Vst::ProcessData&) override;
    Steinberg::tresult PLUGIN_API setState(Steinberg::IBStream*) override;
    Steinberg::tresult PLUGIN_API getState(Steinberg::IBStream*) override;

private:
    double sweep = 0.7;
    double crunch = 0.15;
    bool bypass = false;
    double sampleRate = 44100.0;
    float filterState[2] { 0.0f, 0.0f };
};
}
