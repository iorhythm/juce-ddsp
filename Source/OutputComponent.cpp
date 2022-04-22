/*
  ==============================================================================

	OutputComponent.cpp
	Created: 7 Nov 2020 2:09:43am
	Author:  Robin Otterbein

  ==============================================================================
*/
#include "OutputComponent.h"


OutputComponent::OutputComponent(AudioProcessorValueTreeState& t) : 
	masterSlider{ "Master", t, "outputGain" }
{
	addAndMakeVisible(nameLabel);

	masterSlider.slider.setTextValueSuffix (" dB");
	addAndMakeVisible(masterSlider);
}


void OutputComponent::paint (Graphics& g)
{
	g.setColour (Colours::white);
	g.drawRect (getLocalBounds(), 1);
}


void OutputComponent::resized()
{
	auto r{ getLocalBounds().reduced( 4 ) };

	auto titleArea{ r.removeFromTop( 25 ) };

	//onoffButton.setBounds( titleArea.removeFromLeft( 25 ) );
	nameLabel.setBounds( titleArea );

	masterSlider.setBounds( r );
}


//void OutputComponent::sliderValueChanged(Slider* slider)
//{
//   
//    if (slider == &masterSlider) {
//        outAmp = masterSlider.getValue();
//        if (outputListener != NULL)
//            outputListener->onOutAmpChange(outAmp);
//    }
//}
//
//void OutputComponent::setOutputListener(OutputListener* outListener)
//{
//    outputListener = outListener;
//    if (outputListener != NULL)
//        outputListener->onOutAmpChange(0);
//}
