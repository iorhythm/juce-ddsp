/*
  ==============================================================================

	SubtractiveComponent.cpp
	Created: 7 Nov 2020 2:09:03am
	Author:  Robin Otterbein

  ==============================================================================
*/
#include "SubtractiveComponent.h"


SubtractiveComponent::SubtractiveComponent( AudioProcessorValueTreeState& t ) :
	onoffButton{ "OnOff", t, "noiseOn" },
	ampSlider( "Amp", t, "noiseGain" ),
	colourSlider( "Colour", t, "noiseColor" )
{
	addAndMakeVisible( onoffButton );
	onoffButton.button.setImages( false, true, false,
		ImageFileFormat::loadFrom( BinaryData::power_png, BinaryData::power_pngSize ), {}, Colour::fromRGB( 100, 100, 100 ), //Normal
		ImageFileFormat::loadFrom( BinaryData::power_png, BinaryData::power_pngSize ), {}, Colour::fromRGB( 200, 200, 200 ), //Over
		ImageFileFormat::loadFrom( BinaryData::power_png, BinaryData::power_pngSize ), {}, Colour::fromRGB( 255, 255, 255 ), //Down
		0.0f );

	addAndMakeVisible( nameLabel );

	ampSlider.slider.setTextValueSuffix( " dB" );
	addAndMakeVisible( ampSlider );

	addAndMakeVisible( colourSlider );
}


void SubtractiveComponent::paint( Graphics& g )
{
	g.setColour( Colours::white );
	g.drawRect( getLocalBounds(), 1 );
}


void SubtractiveComponent::resized()
{
	static constexpr auto sliderDim{ 50 };

	auto r{ getLocalBounds().reduced( 4 ) };

	auto titleArea{ r.removeFromTop( 25 ) };

	onoffButton.setBounds( titleArea.removeFromLeft( 25 ) );
	nameLabel.setBounds( titleArea );

	colourSlider.setBounds( r.removeFromTop( sliderDim ) );
	ampSlider.setBounds( r.removeFromTop( sliderDim ) );
}


//void SubtractiveComponent::sliderValueChanged(Slider* slider)
//{
//    if (slider == &colourSlider) {
//        noiseColor = colourSlider.getValue();
//        if (subtractiveListener != NULL)
//            subtractiveListener->onNoiseColorChange(noiseColor);
//    }
//    
//    if (slider == &ampSlider) {
//        subAmp = ampSlider.getValue();
//        if (subtractiveListener != NULL)
//            subtractiveListener->onSubAmpChange(subAmp);
//    }
//}
//
//void SubtractiveComponent::setSubtractiveListener(SubtractiveListener* subListener)
//{
//    subtractiveListener = subListener;
//    if (subtractiveListener != NULL) {
//        subtractiveListener->onNoiseColorChange(noiseColor);
//        subtractiveListener->onSubAmpChange(subAmp);
//        subtractiveListener->onOnOffSubChange(onOffState);
//    }
//}
//
//void SubtractiveComponent::buttonClicked(Button* button)
//{
//    onOffState = button->getToggleState();
//    subtractiveListener->onOnOffSubChange(onOffState);
//}
