#pragma once
#include "public.sdk/source/vst/vsteditcontroller.h"

namespace Rumble {
class Controller final : public Steinberg::Vst::EditControllerEx1 {
public:
    static Steinberg::FUnknown* createInstance(void*) { return (Steinberg::Vst::IEditController*) new Controller(); }
    Steinberg::tresult PLUGIN_API initialize(Steinberg::FUnknown*) override;
    Steinberg::IPlugView* PLUGIN_API createView(Steinberg::FIDString name) override;
    Steinberg::tresult PLUGIN_API setComponentState(Steinberg::IBStream*) override;
};
}
