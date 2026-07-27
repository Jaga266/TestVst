#include "controller.h"
#include "ids.h"
#include "base/source/fstreamer.h"
#include "vstgui/plugin-bindings/vst3editor.h"
#include <cstring>

using namespace Steinberg;
namespace Rumble {

tresult PLUGIN_API Controller::initialize(FUnknown* context) {
    auto result = EditControllerEx1::initialize(context);
    if (result != kResultOk) return result;
    parameters.addParameter(STR16("Sweep"), STR16("%"), 0, 0.70, Vst::ParameterInfo::kCanAutomate, kSweepId);
    parameters.addParameter(STR16("Crunch"), STR16("%"), 0, 0.15, Vst::ParameterInfo::kCanAutomate, kCrunchId);
    parameters.addParameter(STR16("Bypass"), nullptr, 1, 0.0, Vst::ParameterInfo::kCanAutomate | Vst::ParameterInfo::kIsBypass, kBypassId);
    return kResultOk;
}

IPlugView* PLUGIN_API Controller::createView(FIDString name) {
    if (name && std::strcmp(name, Vst::ViewType::kEditor) == 0)
        return new VSTGUI::VST3Editor(this, "view", "Rumble.uidesc");
    return nullptr;
}

tresult PLUGIN_API Controller::setComponentState(IBStream* state) {
    if (!state) return kResultFalse;
    IBStreamer s(state, kLittleEndian);
    float sweep = 0, crunch = 0; int32 bypass = 0;
    if (!s.readFloat(sweep) || !s.readFloat(crunch) || !s.readInt32(bypass)) return kResultFalse;
    setParamNormalized(kSweepId, sweep);
    setParamNormalized(kCrunchId, crunch);
    setParamNormalized(kBypassId, bypass ? 1.0 : 0.0);
    return kResultOk;
}
} // namespace Rumble
