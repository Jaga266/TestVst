#include "processor.h"
#include "ids.h"
#include "base/source/fstreamer.h"
#include "pluginterfaces/vst/ivstparameterchanges.h"
#include <algorithm>
#include <cmath>
#include <cstring>

using namespace Steinberg;
namespace Rumble {

Processor::Processor() { setControllerClass(kControllerUID); }

tresult PLUGIN_API Processor::initialize(FUnknown* context) {
    auto result = AudioEffect::initialize(context);
    if (result != kResultOk) return result;
    addAudioInput(STR16("Stereo In"), Vst::SpeakerArr::kStereo);
    addAudioOutput(STR16("Stereo Out"), Vst::SpeakerArr::kStereo);
    return kResultOk;
}

tresult PLUGIN_API Processor::setupProcessing(Vst::ProcessSetup& setup) {
    sampleRate = setup.sampleRate > 1.0 ? setup.sampleRate : 44100.0;
    filterState[0] = filterState[1] = 0.0f;
    return AudioEffect::setupProcessing(setup);
}

tresult PLUGIN_API Processor::process(Vst::ProcessData& data) {
    if (data.inputParameterChanges) {
        const int32 count = data.inputParameterChanges->getParameterCount();
        for (int32 i = 0; i < count; ++i) {
            auto* queue = data.inputParameterChanges->getParameterData(i);
            if (!queue || queue->getPointCount() == 0) continue;
            int32 offset = 0;
            Vst::ParamValue value = 0.0;
            if (queue->getPoint(queue->getPointCount() - 1, offset, value) != kResultTrue) continue;
            if (queue->getParameterId() == kSweepId) sweep = value;
            else if (queue->getParameterId() == kCrunchId) crunch = value;
            else if (queue->getParameterId() == kBypassId) bypass = value > 0.5;
        }
    }

    if (data.numInputs == 0 || data.numOutputs == 0 || data.numSamples <= 0)
        return kResultOk;

    const int32 channels = std::min<int32>(data.inputs[0].numChannels, data.outputs[0].numChannels);

    // Sweep maps from a clearly dark 120 Hz to an open 18 kHz response.
    const float cutoff = 120.0f * std::pow(18000.0f / 120.0f, static_cast<float>(sweep));
    const float alpha = 1.0f - std::exp(-2.0f * 3.14159265358979323846f * cutoff / static_cast<float>(sampleRate));

    // Dividing by drive keeps the small-signal level close to unity, avoiding the
    // apparent volume boost of tanh(x * drive) / tanh(drive).
    const float drive = 1.0f + static_cast<float>(crunch) * 12.0f;
    const float wet = static_cast<float>(crunch);

    for (int32 ch = 0; ch < channels; ++ch) {
        auto* in = data.inputs[0].channelBuffers32[ch];
        auto* out = data.outputs[0].channelBuffers32[ch];
        float z = filterState[ch < 2 ? ch : 1];

        if (bypass) {
            if (in != out) std::memcpy(out, in, sizeof(float) * data.numSamples);
            continue;
        }

        for (int32 i = 0; i < data.numSamples; ++i) {
            const float dry = in[i];
            const float shaped = std::tanh(dry * drive) / drive;
            const float saturated = dry + wet * (shaped - dry);
            z += alpha * (saturated - z);
            out[i] = z;
        }
        filterState[ch < 2 ? ch : 1] = z;
    }

    return kResultOk;
}

tresult PLUGIN_API Processor::setState(IBStream* state) {
    if (!state) return kResultFalse;
    IBStreamer s(state, kLittleEndian);
    float a = 0, b = 0; int32 c = 0;
    if (!s.readFloat(a) || !s.readFloat(b) || !s.readInt32(c)) return kResultFalse;
    sweep = a; crunch = b; bypass = c != 0;
    return kResultOk;
}

tresult PLUGIN_API Processor::getState(IBStream* state) {
    if (!state) return kResultFalse;
    IBStreamer s(state, kLittleEndian);
    s.writeFloat(static_cast<float>(sweep));
    s.writeFloat(static_cast<float>(crunch));
    s.writeInt32(bypass ? 1 : 0);
    return kResultOk;
}

} // namespace Rumble
