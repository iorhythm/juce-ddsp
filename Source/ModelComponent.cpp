/*
  ==============================================================================

	ModelComponent.cpp
	Created: 7 Nov 2020 1:55:54am
	Author:  Robin Otterbein

  ==============================================================================
*/
#include "ModelComponent.h"


//==============================================================================
ModelComponent::ModelComponent(AudioProcessorValueTreeState& t ) : 
	tree{ t },
	modelCombo{ "Model Select", t, "modelSelect" }
{
	addAndMakeVisible(nameLabel);

	addAndMakeVisible(onoffButton);
	onoffButton.setBounds(0, 0, 50, 50);
	//onoffButton.addListener(this);
	onoffButton.setClickingTogglesState(true);
	onoffButton.setImages(false, true, false,
		ImageFileFormat::loadFrom(BinaryData::power_png, BinaryData::power_pngSize), {}, Colour::fromRGB(255, 255, 255), //Normal
		ImageFileFormat::loadFrom(BinaryData::power_png, BinaryData::power_pngSize), {}, Colour::fromRGB(200, 200, 200), //Over
		ImageFileFormat::loadFrom(BinaryData::power_png, BinaryData::power_pngSize), {}, Colour::fromRGB(100, 100, 100), //Down
		0.0f);
	onoffButton.setToggleState(true, NULL);
	onoffButton.setRadioGroupId(1001);
	onoffButton.onClick = [this] { updateModelState(&onoffButton); };

	addAndMakeVisible( modelCombo );
}


void ModelComponent::paint (Graphics& g)
{
	g.setColour(Colours::white);
	g.drawRect( getLocalBounds(), 1 );
}


void ModelComponent::resized()
{
	//static constexpr auto buttonDim { 65 };

	auto r{ getLocalBounds().reduced( 4 ) };

	auto titleArea{ r.removeFromTop( 25 ) };

	onoffButton.setBounds( titleArea.removeFromLeft( 25 ) );
	nameLabel.setBounds( titleArea );

	modelCombo.setBounds( r );
}


void ModelComponent::updateModelState(Button *button)
{
	auto selectParam = (AudioParameterChoice*) tree.getParameter("modelSelect");
	auto onParam = (AudioParameterBool*) tree.getParameter("modelOn");
	
	if(button == &onoffButton && onoffButton.getToggleState() == true)
	{
		DBG("no model activated");
		*onParam = false;
	}
}
