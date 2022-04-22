/*
  ==============================================================================

	ModelComponent.h
	Created: 7 Nov 2020 1:55:54am
	Author:  Robin Otterbein

  ==============================================================================
*/
#pragma once

#include "DDSP_UI.h"


class ModelComponent : public Component
{
public:
	ModelComponent(AudioProcessorValueTreeState&);
	~ModelComponent() override = default;

	void paint (Graphics&) override;
	void resized() override;
	
	void updateModelState(Button* button);

private:
	AudioProcessorValueTreeState& tree;

	DDSPLabel nameLabel { "MODEL", 17.0f };

	ImageButton onoffButton;
	Label onoffLabel { "BLAHHAH", "BLAHAHAHA" };

	AttachedCombo modelCombo;

	//ImageButton violinButton { "Violin" };
	//ImageButton fluteButton { "Flute" };
	//ImageButton saxophoneButton { "Saxophone" };
	//ImageButton trumpetButton { "Trumpet" };

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ModelComponent)
};
