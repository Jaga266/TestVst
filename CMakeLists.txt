cmake_minimum_required(VERSION 3.22)
project(RumbleSweep VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  JUCE
  GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
  GIT_TAG 8.0.7
)
FetchContent_MakeAvailable(JUCE)

juce_add_plugin(RumbleSweep
  COMPANY_NAME "Jaga Studio"
  BUNDLE_ID "studio.jaga.rumblesweep"
  PLUGIN_MANUFACTURER_CODE Jaga
  PLUGIN_CODE RmSw
  FORMATS VST3 AU Standalone
  PRODUCT_NAME "Rumble Sweep"
  IS_SYNTH FALSE
  NEEDS_MIDI_INPUT FALSE
  NEEDS_MIDI_OUTPUT FALSE
  IS_MIDI_EFFECT FALSE
  COPY_PLUGIN_AFTER_BUILD FALSE
)

target_sources(RumbleSweep PRIVATE
  Source/PluginProcessor.cpp
  Source/PluginProcessor.h
  Source/PluginEditor.cpp
  Source/PluginEditor.h
)

target_compile_definitions(RumbleSweep PUBLIC
  JUCE_WEB_BROWSER=0
  JUCE_USE_CURL=0
  JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(RumbleSweep PRIVATE
  juce::juce_audio_utils
  juce::juce_dsp
  PUBLIC
  juce::juce_recommended_config_flags
  juce::juce_recommended_lto_flags
  juce::juce_recommended_warning_flags
)
