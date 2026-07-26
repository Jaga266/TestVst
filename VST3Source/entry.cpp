#include "public.sdk/source/main/pluginfactory.h"
#include "processor.h"
#include "controller.h"
#include "ids.h"

#define stringPluginName "Rumble VSTGUI Test"

using namespace Steinberg;

BEGIN_FACTORY_DEF("Jaga Studio", "https://github.com/Jaga266/TestVst", "")

DEF_CLASS2(INLINE_UID_FROM_FUID(Rumble::kProcessorUID),
           PClassInfo::kManyInstances,
           kVstAudioEffectClass,
           stringPluginName,
           Vst::kDistributable,
           "Fx|Filter",
           "0.2.0",
           kVstVersionString,
           Rumble::Processor::createInstance)

DEF_CLASS2(INLINE_UID_FROM_FUID(Rumble::kControllerUID),
           PClassInfo::kManyInstances,
           kVstComponentControllerClass,
           stringPluginName " Controller",
           0,
           "",
           "0.2.0",
           kVstVersionString,
           Rumble::Controller::createInstance)

END_FACTORY
