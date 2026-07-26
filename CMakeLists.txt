cmake_minimum_required(VERSION 3.22)
project(RumbleVSTGUI VERSION 0.2.0.0 DESCRIPTION "Rumble VSTGUI compatibility test")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# Build only this plug-in. The SDK examples require extra utility targets
# such as moduleinfotool and are not needed here.
set(SMTG_ENABLE_VST3_PLUGIN_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SMTG_ENABLE_VST3_HOSTING_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SMTG_CREATE_PLUGIN_LINK 0 CACHE BOOL "" FORCE)

FetchContent_Declare(
  vst3sdk
  GIT_REPOSITORY https://github.com/steinbergmedia/vst3sdk.git
  GIT_TAG v3.7.14_build_55
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(vst3sdk)

smtg_enable_vst3_sdk()

smtg_add_vst3plugin(RumbleVSTGUI
  VST3Source/ids.h
  VST3Source/processor.h
  VST3Source/processor.cpp
  VST3Source/controller.h
  VST3Source/controller.cpp
  VST3Source/entry.cpp
  resource/Rumble.uidesc
)

target_link_libraries(RumbleVSTGUI PRIVATE sdk vstgui_support)

smtg_target_add_plugin_resources(RumbleVSTGUI RESOURCES resource/Rumble.uidesc)
smtg_target_configure_version_file(RumbleVSTGUI)

if(SMTG_WIN)
  set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT RumbleVSTGUI)
endif()
