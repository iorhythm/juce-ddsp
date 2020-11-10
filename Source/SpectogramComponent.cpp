/*
  ==============================================================================

    SpectogramComponent.cpp
    Created: 7 Nov 2020 2:10:03am
    Author:  Robin Otterbein

  ==============================================================================
*/

#include <JuceHeader.h>
#include "SpectogramComponent.h"

//==============================================================================
SpectogramComponent::SpectogramComponent()
{
    
}

SpectogramComponent::~SpectogramComponent()
{
}

void SpectogramComponent::paint (juce::Graphics& g)
{
    g.setColour (juce::Colours::white);
    g.drawRect (getLocalBounds(), 3);   // draw an outline around the component

    g.setColour (juce::Colours::white);
    g.setFont (14.0f);
    g.drawText ("Spectogram", getLocalBounds(),
                juce::Justification::centred, true);   // draw some placeholder text
}

void SpectogramComponent::resized()
{

}
