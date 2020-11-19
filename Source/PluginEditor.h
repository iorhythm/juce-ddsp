/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "InputComponent.h"
#include "ModelComponent.h"
#include "AdditiveComponent.h"
#include "SubtractiveComponent.h"
#include "ReverbComponent.h"
#include "SpectogramComponent.h"
#include "OutputComponent.h"
#include "MainComponent.h"
#include "CustomLookAndFeel.h"


//==============================================================================

class DdspsynthAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    DdspsynthAudioProcessorEditor (DdspsynthAudioProcessor&);
    ~DdspsynthAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    DdspsynthAudioProcessor& audioProcessor;
    
    juce::Image backgroundTexture = juce::ImageFileFormat::loadFrom(BinaryData::background_texture_png, BinaryData::background_texture_pngSize);
    
    MainComponent mainComponent;
    
    OtherLookAndFeel otherLookAndFeel;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DdspsynthAudioProcessorEditor)
};
