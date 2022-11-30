
#pragma once

#include "JuceHeader.h"
#include "PluginProcessor.h"

//==============================================================================

class API_TremoloAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    //==============================================================================

    API_TremoloAudioProcessorEditor (API_TremoloAudioProcessor&);
    ~API_TremoloAudioProcessorEditor();

    //==============================================================================

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    //==============================================================================

    API_TremoloAudioProcessor& processor;

    enum {
        editorWidth = 500,
        editorMargin = 10,
        editorPadding = 10,

        sliderTextEntryBoxWidth = 100,
        sliderTextEntryBoxHeight = 25,
        sliderHeight = 25,
        buttonHeight = 25,
        comboBoxHeight = 25,
        labelWidth = 100,
    };

    //======================================

    juce::OwnedArray<juce::Slider> sliders;
    juce::OwnedArray<juce::ToggleButton> toggles;
    juce::OwnedArray<juce::ComboBox> comboBoxes;

    juce::OwnedArray<juce::Label> labels;
    juce::Array<Component*> components;

    typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
    typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;
    typedef juce::AudioProcessorValueTreeState::ComboBoxAttachment ComboBoxAttachment;

    juce::OwnedArray<SliderAttachment> sliderAttachments;
    juce::OwnedArray<ButtonAttachment> buttonAttachments;
    juce::OwnedArray<ComboBoxAttachment> comboBoxAttachments;

    //==============================================================================

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (API_TremoloAudioProcessorEditor)
};
