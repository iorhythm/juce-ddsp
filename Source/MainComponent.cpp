/*
  ==============================================================================

	MainComponent.cpp
	Created: 9 Nov 2020 11:22:48pm
	Author:  Robin Otterbein

  ==============================================================================
*/
#include "MainComponent.h"


MainComponent::MainComponent( AudioProcessorValueTreeState& svts, AudioProcessorValueTreeState& mvts ) :
	inputComponent( svts ),
	modelComponent( mvts ),
	additiveComponent( svts ),
	subtractiveComponent( svts ),
	//modulationComponent( svts ),
	//reverbComponent(svts),
	outputComponent( svts )
{
	addChildAndSetID( &inputComponent, "input" );
	addChildAndSetID( &modelComponent, "model" );
	addChildAndSetID( &additiveComponent, "additive" );
	addChildAndSetID( &subtractiveComponent, "subtractive" );
	//addChildAndSetID(&modulationComponent, "modulation");
	//addChildAndSetID(&reverbComponent, "reverb");
	addChildAndSetID( &spectogramComponent, "spectogram" );
	addChildAndSetID( &outputComponent, "output" );

	// Grey out not implemented components
	//modulationComponent.setEnabled(false);
	//reverbComponent.setEnabled(false);
}


void MainComponent::paint( Graphics& )
{
}


void MainComponent::resized()
{
	auto r{ getLocalBounds().reduced( 4 ) };

	// Header area
	auto headerArea{ r.removeFromTop( 70 ) };
	
	inputComponent.setBounds( headerArea.removeFromLeft( r.getWidth() / 3 ) );
	outputComponent.setBounds( headerArea.removeFromRight( r.getWidth() / 3 ) );
	modelComponent.setBounds( headerArea );

	// Left 'sidebar'
	auto leftSidebar { r.removeFromLeft( 320 ) };
	
	additiveComponent.setBounds( leftSidebar.removeFromTop( 360 ) );
	subtractiveComponent.setBounds( leftSidebar );

	spectogramComponent.setBounds( r );
}
