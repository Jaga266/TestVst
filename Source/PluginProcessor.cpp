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
    filter.setResonance (0.78f);
}

bool RumbleSweepAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    const auto mainOut = layouts.getMainOutputChannelSet();
    return (mainOut == juce::AudioChannelSet::mono() || mainOut == juce::AudioChannelSet::stereo())
        && mainOut == layouts.getMainInputChannelSet();
}

void RumbleSweepAudioProcessor::updateFilter (float cutoffHz)
{
    filter.setCutoffFrequency (juce::jlimit (30.0f, 20000.0f, cutoffHz));
}

void RumbleSweepAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    for (int ch = getTotalNumInputChannels(); ch < getTotalNumOutputChannels(); ++ch)
        buffer.clear (ch, 0, buffer.getNumSamples());

    auto sweep = parameters.getRawParameterValue ("sweep")->load();
    auto crunch = parameters.getRawParameterValue ("crunch")->load();

    double bpm = fallbackBpm;
    if (auto* playHead = getPlayHead())
        if (auto position = playHead->getPosition())
            if (auto hostBpm = position->getBpm())
                bpm = juce::jlimit (20.0, 400.0, *hostBpm);

    const double samplesPerCycle = currentSampleRate * 60.0 / bpm;
    const double phaseInc = 1.0 / samplesPerCycle;

    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        const float ramp = static_cast<float> (1.0 - phase);
        const float envelope = std::pow (ramp, 2.4f);
        const float minCutoff = 75.0f;
        const float maxCutoff = 18000.0f;
        const float modulated = minCutoff * std::pow (maxCutoff / minCutoff,
                                                     juce::jlimit (0.0f, 1.0f,
                                                         (1.0f - sweep) + sweep * envelope));
        updateFilter (modulated);

        const float drive = 1.0f + crunch * 18.0f;
        const float normaliser = 1.0f / std::tanh (drive);

        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
        {
            auto* data = buffer.getWritePointer (channel);
            float x = data[sample];
            x = std::tanh (x * drive) * normaliser;
            data[sample] = filter.processSample (channel, x);
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
