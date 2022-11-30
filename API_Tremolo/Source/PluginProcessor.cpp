

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "PluginParameter.h"
#define _USE_MATH_DEFINES
#include <math.h>

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
    , paramDepth(parameters, "Depth", "", 0.0f, 1.0f, 0.5f)
    , paramFrequency(parameters, "LFO Frequency", "Hz", 0.0f, 10.0f, 2.0f)
    , paramWaveform(parameters, "LFO Waveform", waveformItemsUI, waveformSine)
{
    parameters.apvts.state = juce::ValueTree (juce::Identifier (getName().removeCharacters ("- ")));
}

API_TremoloAudioProcessor::~API_TremoloAudioProcessor()
{
}


void API_TremoloAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    const double smoothTime = 1e-3;
    paramDepth.reset(sampleRate, smoothTime);
    paramFrequency.reset(sampleRate, smoothTime);
    paramWaveform.reset(sampleRate, smoothTime);

    lfoPhase = 0.0f;
    inverseSampleRate = 1.0f / (float)sampleRate;
    twoPi = 2.0f * M_PI;
}

void API_TremoloAudioProcessor::releaseResources()
{
}

void API_TremoloAudioProcessor::processBlock(juce::AudioSampleBuffer& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    const int numInputChannels = getTotalNumInputChannels();
    const int numOutputChannels = getTotalNumOutputChannels();
    const int numSamples = buffer.getNumSamples();

    //======================================

    float currentDepth = paramDepth.getNextValue();
    float currentFrequency = paramFrequency.getNextValue();
    float phase;



    for (int channel = 0; channel < numInputChannels; ++channel) {
        float* channelData = buffer.getWritePointer(channel);
        phase = lfoPhase;

        for (int sample = 0; sample < numSamples; ++sample) {
            const float in = channelData[sample];
            float modulation = lfo(phase, (int)paramWaveform.getTargetValue());
            float out = in * (1 - currentDepth + currentDepth * modulation);

            channelData[sample] = out;

            phase += currentFrequency * inverseSampleRate;
            if (phase >= 1.0f)
                phase -= 1.0f;
        }
    }

    lfoPhase = phase;
}

float API_TremoloAudioProcessor::lfo(float phase, int waveform)
{
	float out = 0.0f;
	switch (waveform) {
	case waveformSine: {
		out = 0.5f + 0.5f * sinf(twoPi * phase);
		break;
	}
	case waveformTriangle: {
		if (phase < 0.25f)
			out = 0.5f + 2.0f * phase;
		else if (phase < 0.75f)
			out = 1.0f - 2.0f * (phase - 0.25f);
		else
			out = 2.0f * (phase - 0.75f);
		break;
	}
	case waveformSawtooth: {
		if (phase < 0.5f)
			out = 0.5f + phase;
		else
			out = phase - 0.5f;
		break;
	}
	case waveformInverseSawtooth: {
		if (phase < 0.5f)
			out = 0.5f - phase;
		else
			out = 1.5f - phase;
		break;
	}
	case waveformSquare: {
		if (phase < 0.5f)
			out = 0.0f;
		else
			out = 1.0f;
		break;
	}
	case waveformSquareSlopedEdges: {
		if (phase < 0.48f)
			out = 1.0f;
		else if (phase < 0.5f)
			out = 1.0f - 50.0f * (phase - 0.48f);
		else if (phase < 0.98f)
			out = 0.0f;
		else
			out = 50.0f * (phase - 0.98f);
		break;
	    }
	}
	return out;
}


void API_TremoloAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = parameters.apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary (*xml, destData);
}

void API_TremoloAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(parameters.apvts.state.getType()))
                parameters.apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

juce::AudioProcessorEditor* API_TremoloAudioProcessor::createEditor()
{
    return new API_API_TremoloAudioProcessorEditor(*this);
}

bool API_TremoloAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}


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
    return 1;

}

int API_TremoloAudioProcessor::getCurrentProgram()
{
    return 0;
}

void API_TremoloAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String API_TremoloAudioProcessor::getProgramName(int index)
{
    return {};
}

void API_TremoloAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}



// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new API_TremoloAudioProcessor();
}

