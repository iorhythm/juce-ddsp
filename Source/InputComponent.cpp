/*
  ==============================================================================

	InputComponent.cpp
	Created: 6 Nov 2020 5:23:11pm
	Author:  Robin Otterbein

  ==============================================================================
*/
#include "InputComponent.h"


InputComponent::InputComponent( AudioProcessorValueTreeState& t ) :
	inputCombo{ "Input Combo", t, "inputIsLine" }
{
	addAndMakeVisible( nameLabel );

	addAndMakeVisible( inputCombo );
}


void InputComponent::paint( Graphics& g )
{
	g.setColour( Colours::white );
	g.drawRect( getLocalBounds(), 1 );
}


void InputComponent::resized()
{
	auto r{ getLocalBounds().reduced( 4 ) };

	auto titleArea{ r.removeFromTop( 25 ) };

	//onoffButton.setBounds( titleArea.removeFromLeft( 25 ) );
	nameLabel.setBounds( titleArea );

	inputCombo.setBounds( r );
}
