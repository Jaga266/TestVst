#pragma once

#include "pluginterfaces/base/funknown.h"
#include "pluginterfaces/vst/vsttypes.h"

namespace Rumble {

static const Steinberg::FUID kProcessorUID (0x8D479152, 0x78984CD4, 0xB13B4039, 0x6E7F0291);
static const Steinberg::FUID kControllerUID (0x6A8B2F41, 0x38A244DF, 0x9BC22CF8, 0xE1E907A5);

enum ParameterIds : Steinberg::Vst::ParamID
{
    kSweepId = 100,
    kCrunchId = 101,
    kBypassId = 102
};

} // namespace Rumble
