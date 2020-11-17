/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "AdditiveComponent.h"

//==============================================================================
DdspsynthAudioProcessorEditor::DdspsynthAudioProcessorEditor (DdspsynthAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    backgroundTexture = backgroundTexture.rescaled(900, 600);
    addAndMakeVisible(mainComponent);
	auto additive = mainComponent.findChildWithID("additive");
	auto harmEditor = (HarmonicEditor*)(additive->findChildWithID("harmonicEditor"));

	harmEditor->setListener(&p);
	
    mainComponent.setBounds(20, 20, 860, 560);
    setSize (900, 600);
}

DdspsynthAudioProcessorEditor::~DdspsynthAudioProcessorEditor()
{
}

//==============================================================================
void DdspsynthAudioProcessorEditor::paint (juce::Graphics& g)
{
    
    g.drawImageAt(backgroundTexture, 0, 0);
    
}

void DdspsynthAudioProcessorEditor::resized()
{
    
}
