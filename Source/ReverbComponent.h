/*
  ==============================================================================

	ReverbComponent.h
	Created: 7 Nov 2020 2:13:37am
	Author:  Robin Otterbein

  ==============================================================================
*/
#pragma once

#include "DDSP_UI.h"


class ReverbComponent : public Component
{
public:
	ReverbComponent(AudioProcessorValueTreeState&);
	~ReverbComponent() override = default;

	void paint (Graphics&) override;
	void resized() override;

private:
	DDSPLabel nameLabel { "REVERB", 17.0f };

	AttachedLabelledImageButton onoffButton;

	AttachedLabelledRotarySlider drywetSlider;
	AttachedLabelledRotarySlider sizeSlider;
	AttachedLabelledRotarySlider glowSlider;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ReverbComponent)
};
