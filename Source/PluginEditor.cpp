#include "PluginEditor.h"

void RumbleLookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                                           float sliderPos, float startAngle, float endAngle,
                                           juce::Slider&)
{
    auto bounds = juce::Rectangle<float> ((float) x, (float) y, (float) width, (float) height).reduced (12.0f);
    const auto radius = juce::jmin (bounds.getWidth(), bounds.getHeight()) * 0.5f;
    const auto centre = bounds.getCentre();
    const auto angle = startAngle + sliderPos * (endAngle - startAngle);

    g.setColour (juce::Colour (0xff101216));
    g.fillEllipse (bounds);
    g.setColour (juce::Colour (0xff313742));
    g.drawEllipse (bounds, 2.0f);

    juce::Path arc;
    arc.addCentredArc (centre.x, centre.y, radius - 5.0f, radius - 5.0f,
                       0.0f, startAngle, angle, true);
    g.setColour (juce::Colour (0xffee6a3b));
    g.strokePath (arc, juce::PathStrokeType (4.0f, juce::PathStrokeType::curved,
                                             juce::PathStrokeType::rounded));

    juce::Path pointer;
    pointer.addRoundedRectangle (-2.5f, -radius * 0.72f, 5.0f, radius * 0.36f, 2.5f);
    g.setColour (juce::Colour (0xffffeadf));
    g.fillPath (pointer, juce::AffineTransform::rotation (angle).translated (centre.x, centre.y));
}

RumbleSweepAudioProcessorEditor::RumbleSweepAudioProcessorEditor (RumbleSweepAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    setOpaque (true);
    setLookAndFeel (&lookAndFeel);
    setSize (560, 320);

    for (auto* slider : { &sweepSlider, &crunchSlider })
    {
        slider->setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
        slider->setTextBoxStyle (juce::Slider::TextBoxBelow, false, 76, 22);
        slider->setTextValueSuffix (" %");
        slider->setColour (juce::Slider::textBoxTextColourId, juce::Colour (0xffe8edf3));
        slider->setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour (0xff171a20));
        slider->setColour (juce::Slider::textBoxOutlineColourId, juce::Colour (0xff343a46));
        slider->setNumDecimalPlacesToDisplay (0);
        addAndMakeVisible (*slider);
    }

    sweepSlider.setRange (0.0, 100.0, 1.0);
    crunchSlider.setRange (0.0, 100.0, 1.0);

    sweepLabel.setText ("SWEEP", juce::dontSendNotification);
    crunchLabel.setText ("CRUNCH", juce::dontSendNotification);
    for (auto* label : { &sweepLabel, &crunchLabel })
    {
        label->setJustificationType (juce::Justification::centred);
        label->setFont (juce::FontOptions (17.0f, juce::Font::bold));
        label->setColour (juce::Label::textColourId, juce::Colour (0xffd6dce5));
        addAndMakeVisible (*label);
    }

    sweepAttachment = std::make_unique<Attachment> (processor.parameters, "sweep", sweepSlider);
    crunchAttachment = std::make_unique<Attachment> (processor.parameters, "crunch", crunchSlider);
}

RumbleSweepAudioProcessorEditor::~RumbleSweepAudioProcessorEditor()
{
    setLookAndFeel (nullptr);
}

void RumbleSweepAudioProcessorEditor::parentHierarchyChanged()
{
    if (auto* peer = getPeer())
    {
        const auto engines = peer->getAvailableRenderingEngines();
        for (int i = 0; i < engines.size(); ++i)
            if (engines[i].containsIgnoreCase ("software"))
            {
                peer->setCurrentRenderingEngine (i);
                break;
            }
    }
    repaint();
}

void RumbleSweepAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff0d0f13));

    auto panel = getLocalBounds().reduced (14).toFloat();
    g.setColour (juce::Colour (0xff181c23));
    g.fillRoundedRectangle (panel, 14.0f);
    g.setColour (juce::Colour (0xff303642));
    g.drawRoundedRectangle (panel, 14.0f, 1.5f);

    g.setColour (juce::Colour (0xfff1f4f8));
    g.setFont (juce::FontOptions (28.0f, juce::Font::bold));
    g.drawText ("RUMBLE SWEEP", 30, 24, getWidth() - 60, 40, juce::Justification::centredLeft);

    g.setColour (juce::Colour (0xff8c96a6));
    g.setFont (juce::FontOptions (13.0f));
    g.drawText ("tempo-synced filter punch", 32, 60, getWidth() - 64, 22,
                juce::Justification::centredLeft);

    g.setColour (juce::Colour (0xffee6a3b));
    g.fillRoundedRectangle (juce::Rectangle<float> (398.0f, 36.0f, 116.0f, 34.0f), 17.0f);
    g.setColour (juce::Colour (0xff121419));
    g.setFont (juce::FontOptions (14.0f, juce::Font::bold));
    g.drawText ("RHYTHMIC FX", 398, 36, 116, 34, juce::Justification::centred);

    g.setColour (juce::Colour (0xff242a33));
    g.fillRoundedRectangle (juce::Rectangle<float> (30.0f, 98.0f, 500.0f, 184.0f), 12.0f);
}

void RumbleSweepAudioProcessorEditor::resized()
{
    sweepLabel.setBounds (76, 112, 170, 26);
    sweepSlider.setBounds (66, 136, 190, 132);

    crunchLabel.setBounds (314, 112, 170, 26);
    crunchSlider.setBounds (304, 136, 190, 132);
}
