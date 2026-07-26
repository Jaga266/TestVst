#include "processor.h"
#include "ids.h"
#include "base/source/fstreamer.h"
#include "pluginterfaces/vst/ivstparameterchanges.h"
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

tresult PLUGIN_API Processor::process(Vst::ProcessData& data) {
    if (data.inputParameterChanges) {
        const int32 count = data.inputParameterChanges->getParameterCount();
        for (int32 i = 0; i < count; ++i) {
            auto* queue = data.inputParameterChanges->getParameterData(i);
            if (!queue || queue->getPointCount() == 0) continue;
            int32 offset = 0; Vst::ParamValue value = 0;
            if (queue->getPoint(queue->getPointCount() - 1, offset, value) != kResultTrue) continue;
            if (queue->getParameterId() == kSweepId) sweep = value;
            else if (queue->getParameterId() == kCrunchId) crunch = value;
            else if (queue->getParameterId() == kBypassId) bypass = value > 0.5;
        }
    }
    if (data.numInputs == 0 || data.numOutputs == 0 || data.numSamples <= 0) return kResultOk;
    const int32 channels = data.outputs[0].numChannels;
    for (int32 ch = 0; ch < channels; ++ch) {
        auto* in = data.inputs[0].channelBuffers32[ch];
        auto* out = data.outputs[0].channelBuffers32[ch];
        if (in != out) std::memcpy(out, in, sizeof(float) * data.numSamples);
    }
    return kResultOk;
}

tresult PLUGIN_API Processor::setState(IBStream* state) {
    if (!state) return kResultFalse;
    IBStreamer s(state, kLittleEndian);
    float a = 0, b = 0; int32 c = 0;
    if (!s.readFloat(a) || !s.readFloat(b) || !s.readInt32(c)) return kResultFalse;
    sweep = a; crunch = b; bypass = c != 0; return kResultOk;
}

tresult PLUGIN_API Processor::getState(IBStream* state) {
    if (!state) return kResultFalse;
    IBStreamer s(state, kLittleEndian);
    s.writeFloat((float)sweep); s.writeFloat((float)crunch); s.writeInt32(bypass ? 1 : 0);
    return kResultOk;
}
}
