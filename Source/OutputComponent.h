/*
  ==============================================================================

	OutputComponent.h
	Created: 7 Nov 2020 2:09:43am
	Author:  Robin Otterbein

  ==============================================================================
*/
#pragma once

#include "DDSP_UI.h"


class OutputComponent : public Component
{
public:
	OutputComponent(AudioProcessorValueTreeState&);
	~OutputComponent() override = default;

	void paint (Graphics&) override;
	void resized() override;
	
	//void sliderValueChanged(Slider* slider) override;
	//
	//struct OutputListener
	//{
	//    virtual void onOutAmpChange(double outAmp) = 0;
	//};

	//void setOutputListener(OutputListener* outListener);

private:
	DDSPLabel nameLabel { "OUTPUT", 17.0f };

	AttachedLabelledSlider masterSlider;
	
	//OutputListener* outputListener = NULL;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OutputComponent)
};
