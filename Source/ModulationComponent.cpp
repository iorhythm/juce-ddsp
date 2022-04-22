/*
  ==============================================================================

	ModulationComponent.cpp
	Created: 2 Dec 2020 2:55:18pm
	Author:  Francesco Ganis

  ==============================================================================
*/
#include "ModulationComponent.h"


ModulationComponent::ModulationComponent(AudioProcessorValueTreeState& t) :
	onoffButton{ "Mod On", t, "modulationOn" },
	amountSlider{ "Amount", t, "modulationAmount" },
	delaySlider{ "Delay", t, "modulationDelay" },
	rateSlider{ "Rate", t, "modulationRate" }
{
	addAndMakeVisible(onoffButton);
	onoffButton.setBounds(0, 0, 50, 50);
	onoffButton.button.setImages(false, true, false,
		ImageFileFormat::loadFrom(BinaryData::power_png, BinaryData::power_pngSize), {}, Colour::fromRGB(100, 100, 100), //Normal
		ImageFileFormat::loadFrom(BinaryData::power_png, BinaryData::power_pngSize), {}, Colour::fromRGB(200, 200, 200), //Over
		ImageFileFormat::loadFrom(BinaryData::power_png, BinaryData::power_pngSize), {}, Colour::fromRGB(255, 255, 255), //Down
		0.0f);

	addAndMakeVisible(nameLabel);

	amountSlider.slider.setRange(0.0f, 10.0f, 0.1f);
	amountSlider.slider.setValue(5.0f);
	addAndMakeVisible(amountSlider);
	
	addAndMakeVisible(delaySlider);
	addAndMakeVisible(rateSlider);
}


void ModulationComponent::paint(Graphics& g)
{
	g.setColour(Colours::dimgrey);
	g.drawRect(getLocalBounds(), 1);
}


void ModulationComponent::resized()
{
	auto r { getLocalBounds() };

	auto titleArea { r.removeFromTop( 30 ) };

	onoffButton.setBounds( titleArea.removeFromRight( 18 ).reduced( 0, 6 ) );
	nameLabel.setBounds( titleArea );

	auto middleArea { r.removeFromTop( 70 ) };
	rateSlider.setBounds( middleArea.reduced( 10, 0 ) );

	amountSlider.setBounds( r.removeFromLeft( 50 ) );
	delaySlider.setBounds( r );
}
