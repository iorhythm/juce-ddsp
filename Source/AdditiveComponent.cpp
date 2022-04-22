/*
  ==============================================================================

	AdditiveComponent.cpp
	Created: 7 Nov 2020 2:08:52am
	Author:  Robin Otterbein

  ==============================================================================
*/

#include "AdditiveComponent.h"


AdditiveComponent::AdditiveComponent( AudioProcessorValueTreeState& t ) :
	onoffButton( "On/Off", t, "additiveOn" ),
	shiftSlider{ "Shift", t, "additiveShift" },
	stretchSlider{ "Stretch", t, "additiveStretch" },
	ampSlider{ "Amp", t, "additiveGain" }
{
	addAndMakeVisible( onoffButton );
	onoffButton.button.setImages( false, true, true,
		ImageFileFormat::loadFrom( BinaryData::power_png, BinaryData::power_pngSize ), {}, Colour::fromRGB( 100, 100, 100 ), //Normal
		ImageFileFormat::loadFrom( BinaryData::power_png, BinaryData::power_pngSize ), {}, Colour::fromRGB( 200, 200, 200 ), //Over
		ImageFileFormat::loadFrom( BinaryData::power_png, BinaryData::power_pngSize ), {}, Colour::fromRGB( 255, 255, 255 ), //Down
		0.0f );

	addAndMakeVisible( nameLabel );

	addChildAndSetID( &harmonicEditor, "harmonicEditor" );

	addAndMakeVisible( shiftSlider );
	addAndMakeVisible( stretchSlider );

	ampSlider.slider.setTextValueSuffix( " dB" );
	addAndMakeVisible( ampSlider );
}


void AdditiveComponent::paint( Graphics& g )
{
	g.setColour( Colours::white );
	g.drawRect( getLocalBounds(), 1 );
}


void AdditiveComponent::resized()
{
	static constexpr auto sliderDim{ 40 };

	auto r{ getLocalBounds().reduced( 4 ) };

	auto titleArea{ r.removeFromTop( 25 ) };

	onoffButton.setBounds( titleArea.removeFromLeft( 25 ) );
	nameLabel.setBounds( titleArea );

	harmonicEditor.setBounds( r.removeFromTop( 130 ) );

	shiftSlider.setBounds( r.removeFromTop( sliderDim ) );
	stretchSlider.setBounds( r.removeFromTop( sliderDim ) );
	ampSlider.setBounds( r.removeFromTop( sliderDim ) );
}


//void AdditiveComponent::sliderValueChanged(Slider* slider)
//{
//   if (additiveListener != NULL) {
//        if (slider == &ampSlider) {
//            addAmp = ampSlider.getValue();
//            additiveListener->onAddAmpChange(addAmp);
//        }
//        else if (slider == &shiftSlider)
//        {
//            shiftValue = slider->getValue();
//            additiveListener->onShiftValueChange(shiftValue);
//        }
//        else if (slider == &stretchSlider)
//        {
//            stretchValue = slider->getValue();
//            additiveListener->onStretchValueChange(stretchValue);
//        }
//    }
//}

//void AdditiveComponent::setAdditiveListener(AdditiveListener* addListener)
//{
//    additiveListener = addListener;
//    if (additiveListener != NULL)
//        additiveListener->onAddAmpChange(0);
//        additiveListener->onShiftValueChange(shiftValue);
//        additiveListener->onStretchValueChange(stretchValue);
//        additiveListener->onOnOffAddChange(onOffState);
//}
//
//void AdditiveComponent::buttonClicked(Button* button)
//{
//    additiveListener->onOnOffAddChange(button->getToggleState());
//}
