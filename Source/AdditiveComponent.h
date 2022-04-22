/*
  ==============================================================================

	AdditiveComponent.h
	Created: 7 Nov 2020 2:08:52am
	Author:  Robin Otterbein

  ==============================================================================
*/
#pragma once

#include "DDSP_UI.h"
#include "HarmonicEditor.h"


class AdditiveComponent : public Component/*, public Slider::Listener, public ToggleButton::Listener*/
{
public:	

	AdditiveComponent(AudioProcessorValueTreeState&);
	~AdditiveComponent() override = default;
	
	void paint (Graphics&) override;
	void resized() override;
	//
	//void sliderValueChanged(Slider* slider) override;
	//
	//struct AdditiveListener
	//{
	//    virtual void onAddAmpChange(double addAmp) = 0;
	//    virtual void onShiftValueChange(double shiftValue) = 0;
	//    virtual void onStretchValueChange(double stretchValue) = 0;
	//    virtual void onOnOffAddChange(bool onOff) = 0;
	//};

	//void setAdditiveListener(AdditiveListener* addListener);

	//void buttonClicked(Button*) override;

	HarmonicEditor harmonicEditor;

private:
	DDSPLabel nameLabel { "HARMONIC OSCILLATOR", 17.0f };

	AttachedImageButton onoffButton;

	AttachedLabelledSlider shiftSlider;
	AttachedLabelledSlider stretchSlider;
	AttachedLabelledSlider ampSlider;

	//double addAmp = 0;
	//double shiftValue;
	//double stretchValue;
	//bool onOffState = true;
	
	//AdditiveListener* additiveListener = NULL;
	
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AdditiveComponent)
};