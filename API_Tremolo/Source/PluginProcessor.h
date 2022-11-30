

#pragma once

#include "JuceHeader.h"
#include "PluginParameter.h"

//==============================================================================

class API_TremoloAudioProcessor : public juce::AudioProcessor
{
public:

    API_TremoloAudioProcessor();
    ~API_TremoloAudioProcessor();
     
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock (juce::AudioSampleBuffer&, juce::MidiBuffer&) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;


    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;


   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif


    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect () const override;
    double getTailLengthSeconds() const override;


    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    juce::StringArray waveformItemsUI = {
        "Sine",
        "Triangle",
        "Sawtooth (rising)",
        "Sawtooth (falling)",
        "Square",
        "Square with sloped edges"
    };

    enum waveformIndex {
        waveformSine = 0,
        waveformTriangle,
        waveformSawtooth,
        waveformInverseSawtooth, change
        waveformSquare,
        waveformSquareSlopedEdges,
    };

    float lfoPhase;
    float inverseSampleRate;
    float twoPi;

    float lfo(float phase, int waveform);

    PluginParametersManager parameters;

    PluginParameterLinSlider paramDepth;
    PluginParameterLinSlider paramFrequency;
    PluginParameterComboBox paramWaveform;


private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (API_TremoloAudioProcessor)
};

