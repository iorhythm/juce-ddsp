/*
  ==============================================================================

	SubtractiveComponent.h
	Created: 7 Nov 2020 2:09:03am
	Author:  Robin Otterbein

  ==============================================================================
*/
#pragma once

#include "DDSP_UI.h"


class SubtractiveComponent : public Component/*, public Slider::Listener, public ToggleButton::Listener*/
{
public:
	SubtractiveComponent(AudioProcessorValueTreeState&);
	~SubtractiveComponent() override = default;

	void paint (Graphics&) override;
	void resized() override;

	//struct SubtractiveListener
	//{
	//    virtual void onNoiseColorChange(double color) = 0;
	//    virtual void onOnOffSubChange(bool onOff) = 0;
	//    virtual void onSubAmpChange(double subAmp) = 0;
	//};

	//void setSubtractiveListener(SubtractiveListener* subListener);

private:
	DDSPLabel nameLabel { "NOISE", 17.0f };

	AttachedImageButton onoffButton;

	AttachedLabelledSlider ampSlider;
	AttachedLabelledSlider colourSlider;

	double noiseColor {};
	double subAmp {};
	bool onOffState { true };

	//SubtractiveListener* subtractiveListener = NULL;

	//// Inherited via Listener
	//void buttonClicked(Button*) override;
	//void sliderValueChanged(Slider* slider) override;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SubtractiveComponent)
};
