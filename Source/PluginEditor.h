#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"

class RumbleLookAndFeel final : public juce::LookAndFeel_V4
{
public:
    void drawRotarySlider (juce::Graphics&, int, int, int, int, float,
                           float, float, juce::Slider&) override;
};

class RumbleSweepAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit RumbleSweepAudioProcessorEditor (RumbleSweepAudioProcessor&);
    ~RumbleSweepAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;

    RumbleSweepAudioProcessor& processor;
    RumbleLookAndFeel lookAndFeel;
    juce::Slider sweepSlider, crunchSlider;
    juce::Label sweepLabel, crunchLabel;
    std::unique_ptr<Attachment> sweepAttachment, crunchAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RumbleSweepAudioProcessorEditor)
};
