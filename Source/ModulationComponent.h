/*
  ==============================================================================

	ModulationComponent.h
	Created: 2 Dec 2020 2:55:18pm
	Author:  Francesco Ganis

  ==============================================================================
*/
#pragma once

#include "DDSP_UI.h"


class ModulationComponent : public Component
{
public:
	ModulationComponent(AudioProcessorValueTreeState&);
	~ModulationComponent() override = default;

	void paint(Graphics&) override;
	void resized() override;

private:
	DDSPLabel nameLabel { "MOD", 17.0f };

	AttachedLabelledImageButton onoffButton;

	AttachedLabelledRotarySlider amountSlider;
	AttachedLabelledRotarySlider rateSlider;
	AttachedLabelledRotarySlider delaySlider;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModulationComponent)
};
