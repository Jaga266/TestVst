#include "PluginProcessor.h"
#include "PluginEditor.h"

RumbleSweepAudioProcessor::RumbleSweepAudioProcessor()
    : AudioProcessor (BusesProperties()
        .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
        .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      parameters (*this, nullptr, "PARAMETERS", createParameterLayout())
{
}

juce::AudioProcessorValueTreeState::ParameterLayout RumbleSweepAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> p;
    p.push_back (std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "sweep", 1 }, "Sweep",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f), 0.72f));
    p.push_back (std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "crunch", 1 }, "Crunch",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f), 0.18f));
    return { p.begin(), p.end() };
}

void RumbleSweepAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    phase = 0.0;

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32> (samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32> (getTotalNumOutputChannels());

    filter.reset();
    filter.prepare (spec);
    filter.setType (juce::dsp::StateVariableTPTFilterType::lowpass);
    filter.setResonance (0.72f);
    filter.setCutoffFrequency (18000.0f);
}

bool RumbleSweepAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    const auto mainOut = layouts.getMainOutputChannelSet();
    return (mainOut == juce::AudioChannelSet::mono() || mainOut == juce::AudioChannelSet::stereo())
        && mainOut == layouts.getMainInputChannelSet();
}

void RumbleSweepAudioProcessor::updateFilter (float cutoffHz)
{
    filter.setCutoffFrequency (juce::jlimit (35.0f, 19500.0f, cutoffHz));
}

void RumbleSweepAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    for (int ch = getTotalNumInputChannels(); ch < getTotalNumOutputChannels(); ++ch)
        buffer.clear (ch, 0, buffer.getNumSamples());

    const float sweep = parameters.getRawParameterValue ("sweep")->load();
    const float crunch = parameters.getRawParameterValue ("crunch")->load();

    double bpm = fallbackBpm;
    if (auto* playHead = getPlayHead())
        if (auto position = playHead->getPosition())
            if (auto hostBpm = position->getBpm())
                bpm = juce::jlimit (20.0, 400.0, *hostBpm);

    const double phaseInc = bpm / (60.0 * currentSampleRate);
    const float drive = 1.0f + crunch * 10.0f;
    const float normaliser = 1.0f / std::tanh (drive);
    constexpr int controlInterval = 16;

    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        if ((sample % controlInterval) == 0)
        {
            const float ramp = static_cast<float> (1.0 - phase);
            const float envelope = ramp * ramp;
            const float shaped = juce::jlimit (0.0f, 1.0f, (1.0f - sweep) + sweep * envelope);
            const float cutoff = 70.0f * std::exp (shaped * std::log (18000.0f / 70.0f));
            updateFilter (cutoff);
        }

        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
        {
            auto* data = buffer.getWritePointer (channel);
            const float saturated = std::tanh (data[sample] * drive) * normaliser;
            data[sample] = filter.processSample (channel, saturated);
        }

        phase += phaseInc;
        if (phase >= 1.0)
            phase -= 1.0;
    }
}

void RumbleSweepAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    if (auto xml = parameters.copyState().createXml())
        copyXmlToBinary (*xml, destData);
}

void RumbleSweepAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary (data, sizeInBytes))
        if (xml->hasTagName (parameters.state.getType()))
            parameters.replaceState (juce::ValueTree::fromXml (*xml));
}

juce::AudioProcessorEditor* RumbleSweepAudioProcessor::createEditor()
{
    return new RumbleSweepAudioProcessorEditor (*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new RumbleSweepAudioProcessor();
}
