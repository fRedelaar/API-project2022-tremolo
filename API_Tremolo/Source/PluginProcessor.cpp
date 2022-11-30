

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "PluginParameter.h"

//==============================================================================

API_TremoloAudioProcessor::API_TremoloAudioProcessor():
#ifndef JucePlugin_PreferredChannelConfigurations
    AudioProcessor (BusesProperties()
                    #if ! JucePlugin_IsMidiEffect
                     #if ! JucePlugin_IsSynth
                      .withInput  ("Input",  AudioChannelSet::stereo(), true)
                     #endif
                      .withOutput ("Output", AudioChannelSet::stereo(), true)
                    #endif
                   ),
#endif
    parameters (*this)
    , parameter1 (parameters, "Parameter 1", "", 0.0f, 1.0f, 0.5f, [](float value){ return value * 127.0f; })
    , parameter2 (parameters, "Parameter 2", "", 0.0f, 1.0f, 0.5f)
    , parameter3 (parameters, "Parameter 3", false, [](float value){ return value * (-2.0f) + 1.0f; })
    , parameter4 (parameters, "Parameter 4", {"Option A", "Option B"}, 1)
{
    parameters.valueTreeState.state = juce::ValueTree (juce::Identifier (getName().removeCharacters ("- ")));
}

API_TremoloAudioProcessor::~API_TremoloAudioProcessor()
{
}

//==============================================================================

void API_TremoloAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    const double smoothTime = 1e-3;
    parameter1.reset (sampleRate, smoothTime);
    parameter2.reset (sampleRate, smoothTime);
    parameter3.reset (sampleRate, smoothTime);
    parameter4.reset (sampleRate, smoothTime);
}

void API_TremoloAudioProcessor::releaseResources()
{
}

void API_TremoloAudioProcessor::processBlock (juce::AudioSampleBuffer& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    const int numInputChannels = getTotalNumInputChannels();
    const int numOutputChannels = getTotalNumOutputChannels();
    const int numSamples = buffer.getNumSamples();

    //======================================

    float currentParameter2 = parameter2.getNextValue();
    float currentParameter3 = parameter3.getNextValue();
    float currentParameter4 = parameter4.getNextValue();

    float factor = currentParameter2 * currentParameter3 * currentParameter4;

    for (int channel = 0; channel < numInputChannels; ++channel) {
        float* channelData = buffer.getWritePointer (channel);

        for (int sample = 0; sample < numSamples; ++sample) {
            const float in = channelData[sample];
            float out = in * factor;

            channelData[sample] = out;
        }
    }

    for (int channel = numInputChannels; channel < numOutputChannels; ++channel)
        buffer.clear (channel, 0, numSamples);

    //======================================

    juce::MidiBuffer processedMidi;
    juce::MidiMessage message;
    int time;

    for (juce::MidiBuffer::Iterator iter (midiMessages); iter.getNextEvent (message, time);) {
        if (message.isNoteOn()) {
            juce::uint8 newVel = (juce::uint8)(parameter1.getTargetValue());
            message = juce::MidiMessage::noteOn (message.getChannel(), message.getNoteNumber(), newVel);
        }
        processedMidi.addEvent (message, time);
    }

    midiMessages.swapWith (processedMidi);
}

//==============================================================================






//==============================================================================

void API_TremoloAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    std::unique_ptr<juce::XmlElement> xml (parameters.valueTreeState.state.createXml());
    copyXmlToBinary (*xml, destData);
}

void API_TremoloAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState != nullptr)
        if (xmlState->hasTagName (parameters.valueTreeState.state.getType()))
            parameters.valueTreeState.state = juce::ValueTree::fromXml (*xmlState);
}

//==============================================================================

bool API_TremoloAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* API_TremoloAudioProcessor::createEditor()
{
    return new API_API_TremoloAudioProcessorEditor (*this);
}

//==============================================================================

#ifndef JucePlugin_PreferredChannelConfigurations
bool API_TremoloAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

//==============================================================================

const juce::String API_TremoloAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool API_TremoloAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool API_TremoloAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool API_TremoloAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double API_TremoloAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int API_TremoloAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int API_TremoloAudioProcessor::getCurrentProgram()
{
    return 0;
}

void API_TremoloAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String API_TremoloAudioProcessor::getProgramName (int index)
{
    return {};
}

void API_TremoloAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================

// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new API_TremoloAudioProcessor();
}

//==============================================================================
