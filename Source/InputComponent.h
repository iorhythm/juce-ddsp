/*
  ==============================================================================

	InputComponent.h
	Created: 6 Nov 2020 5:23:11pm
	Author:  Robin Otterbein

  ==============================================================================
*/
#pragma once

#include "DDSP_UI.h"


class InputComponent : public Component
{
public:
	InputComponent(AudioProcessorValueTreeState&);
	~InputComponent() override = default;

	void paint (Graphics&) override;
	void resized() override;

private:
	DDSPLabel nameLabel{ "INPUT", 17.0f };

	AttachedCombo inputCombo;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InputComponent)
};
