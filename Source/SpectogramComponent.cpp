/*
  ==============================================================================

	SpectogramComponent.cpp
	Created: 7 Nov 2020 2:10:03am
	Author:  Robin Otterbein

  ==============================================================================
*/
#include "SpectogramComponent.h"


SpectogramComponent::SpectogramComponent() : 
	spectrogramImage (Image::RGB, 512, 512, true)
{
}


void SpectogramComponent::paint (Graphics& g)
{
	g.fillAll(Colours::transparentBlack);

	g.setOpacity(1.0f);
	g.drawImage(spectrogramImage, getLocalBounds().toFloat());
	
	g.setColour (Colours::white);
	g.drawRect (getLocalBounds(), 1);
}


void SpectogramComponent::resized()
{
}


void SpectogramComponent::drawNextLineOfSpectrogram(int fftSize, float* fftData, dsp::FFT& forwardFFT, int fftOrder)
{
	auto rightHandEdge = spectrogramImage.getWidth() - 1;
	auto imageHeight   = spectrogramImage.getHeight();

	Array<float> extractedFftData = Array<float>();
	
	for (int i = 0; i < (2 * fftSize - 1); ++i)
	{
		extractedFftData.add(*fftData);
		fftData++;
	}
	
	// first, shuffle our image leftwards by 1 pixel..
	spectrogramImage.moveImageSection (0, 0, 1, 0, rightHandEdge, imageHeight);

	// then render our FFT data..
	forwardFFT.performFrequencyOnlyForwardTransform (extractedFftData.getRawDataPointer());

	// find the range of values produced, so we can scale our rendering to
	// show up the detail clearly
	auto maxLevel = FloatVectorOperations::findMinAndMax (extractedFftData.getRawDataPointer(), fftSize / 2);

	for (auto y = 1; y < imageHeight; ++y)
	{
		auto skewedProportionY = 1.0f - std::exp (std::log ((float) y / (float) imageHeight) * 0.2f);
		auto fftDataIndex = jlimit (0, fftSize / 2, (int) (skewedProportionY * (int) fftSize / 2));
		auto level = jmap (extractedFftData[fftDataIndex], 0.0f, jmax (maxLevel.getEnd(), 1e-5f), 0.0f, 1.0f);

		spectrogramImage.setPixelAt (rightHandEdge, y, Colour::fromHSL (-level*0.6+1, 1.0f, 0.5f, level));
	}
	
	repaint();
}
