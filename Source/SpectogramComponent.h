/*
  ==============================================================================

	SpectogramComponent.h
	Created: 7 Nov 2020 2:10:03am
	Author:  Robin Otterbein

  ==============================================================================
*/
#pragma once

#include <JuceHeader.h>


class SpectogramComponent : public Component
{
public:
	SpectogramComponent();
	~SpectogramComponent() override = default;

	void paint (Graphics&) override;
	void resized() override;
	
	void drawNextLineOfSpectrogram(int fftSize, float* fftData, dsp::FFT& forwardFFT, int fftOrder);

private:
	Image spectrogramImage;
	
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SpectogramComponent)
};
