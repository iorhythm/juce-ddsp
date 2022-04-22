/*
  ==============================================================================

	MainComponent.h
	Created: 9 Nov 2020 11:22:48pm
	Author:  Robin Otterbein

  ==============================================================================
*/
#pragma once

#include "InputComponent.h"
#include "ModelComponent.h"
#include "AdditiveComponent.h"
#include "SubtractiveComponent.h"
#include "ModulationComponent.h"
#include "ReverbComponent.h"
#include "SpectogramComponent.h"
#include "OutputComponent.h"


class MainComponent : public Component
{
public:
	MainComponent( AudioProcessorValueTreeState&, AudioProcessorValueTreeState& );
	~MainComponent() override = default;

	void paint( Graphics& ) override;
	void resized() override;

	InputComponent inputComponent;
	ModelComponent modelComponent;
	OutputComponent outputComponent;

	AdditiveComponent additiveComponent;
	SubtractiveComponent subtractiveComponent;
	SpectogramComponent spectogramComponent;

	// WIP
	//ModulationComponent modulationComponent;
	//ReverbComponent reverbComponent;

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( MainComponent )
};
