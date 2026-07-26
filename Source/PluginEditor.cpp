#include "PluginEditor.h"

void RumbleLookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                                          float sliderPos, float startAngle, float endAngle,
                                          juce::Slider&)
{
    auto bounds = juce::Rectangle<float> (static_cast<float> (x), static_cast<float> (y),
                                          static_cast<float> (width), static_cast<float> (height))
                      .reduced (8.0f);
    const auto radius = juce::jmin (bounds.getWidth(), bounds.getHeight()) * 0.5f;
    const auto centre = bounds.getCentre();
    const auto angle = startAngle + sliderPos * (endAngle - startAngle);

    g.setColour (juce::Colour (0xff161616));
    g.fillEllipse (bounds);
    g.setColour (juce::Colour (0xff3b3b3b));
    g.drawEllipse (bounds, 3.0f);

    juce::Path pointer;
    pointer.addRoundedRectangle (-3.0f, -radius * 0.72f, 6.0f, radius * 0.45f, 3.0f);
    g.setColour (juce::Colour (0xffeee2c7));
    g.fillPath (pointer, juce::AffineTransform::rotation (angle).translated (centre.x, centre.y));
}

RumbleSweepAudioProcessorEditor::RumbleSweepAudioProcessorEditor (RumbleSweepAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    setLookAndFeel (&lookAndFeel);
    setSize (620, 360);

    for (auto* slider : { &sweepSlider, &crunchSlider })
    {
        slider->setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
        slider->setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 24);
        slider->setTextValueSuffix (" %");
        addAndMakeVisible (*slider);
    }

    sweepSlider.setRange (0.0, 100.0, 0.1);
    crunchSlider.setRange (0.0, 100.0, 0.1);

    sweepLabel.setText ("SWEEP", juce::dontSendNotification);
    crunchLabel.setText ("CRUNCH", juce::dontSendNotification);
    for (auto* label : { &sweepLabel, &crunchLabel })
    {
        label->setJustificationType (juce::Justification::centred);
        label->setFont (juce::FontOptions (20.0f, juce::Font::bold));
        label->setColour (juce::Label::textColourId, juce::Colour (0xff8e251c));
        addAndMakeVisible (*label);
    }

    sweepAttachment = std::make_unique<Attachment> (processor.parameters, "sweep", sweepSlider);
    crunchAttachment = std::make_unique<Attachment> (processor.parameters, "crunch", crunchSlider);
}

RumbleSweepAudioProcessorEditor::~RumbleSweepAudioProcessorEditor()
{
    setLookAndFeel (nullptr);
}

void RumbleSweepAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xffd8c08f));

    g.setColour (juce::Colour (0x22000000));
    for (int y = 0; y < getHeight(); y += 7)
        g.drawHorizontalLine (y, 0.0f, static_cast<float> (getWidth()));

    g.setColour (juce::Colour (0xff9e1e18));
    g.setFont (juce::FontOptions (46.0f, juce::Font::bold));
    g.drawFittedText ("RUMBLE SWEEP", 24, 18, getWidth() - 48, 58,
                      juce::Justification::centred, 1);

    g.setColour (juce::Colour (0xff2b2118));
    g.setFont (juce::FontOptions (14.0f));
    g.drawFittedText ("tempo-synced low-pass impact", 0, 72, getWidth(), 24,
                      juce::Justification::centred, 1);

    auto mascotArea = juce::Rectangle<float> (390.0f, 112.0f, 180.0f, 180.0f);
    g.setColour (juce::Colour (0xff9e1e18));
    g.fillEllipse (mascotArea);
    g.setColour (juce::Colour (0xfff1dfb8));
    g.fillRoundedRectangle (mascotArea.reduced (38.0f), 24.0f);
    g.setColour (juce::Colour (0xff231f1b));
    g.drawRoundedRectangle (mascotArea.reduced (38.0f), 24.0f, 5.0f);
    g.setFont (juce::FontOptions (28.0f, juce::Font::bold));
    g.drawFittedText ("BOOM", mascotArea.toNearestInt(), juce::Justification::centred, 1);
}

void RumbleSweepAudioProcessorEditor::resized()
{
    sweepLabel.setBounds (54, 105, 210, 28);
    sweepSlider.setBounds (54, 132, 210, 190);

    crunchLabel.setBounds (275, 150, 110, 28);
    crunchSlider.setBounds (270, 176, 120, 130);
}
