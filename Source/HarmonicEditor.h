/*
  ==============================================================================

	HarmonicEditor.h
	Created: 10 Nov 2020 2:44:17pm
	Author:  Robin Otterbein

  ==============================================================================
*/
#pragma once

#include <JuceHeader.h>


class HarmonicSlider : public Component
{
public:
	HarmonicSlider() = default;
	~HarmonicSlider() override = default;

	void paint( Graphics& ) override;
	void resized() override;

	void setValue( const float );

private:
	bool isEntered;
	bool isDown;

	Colour colour{ Colours::black };
	Rectangle<int> rectangle;
	float value{ 0.5f };

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( HarmonicSlider )
};


class HarmonicEditor : public Component
{
public:
	static constexpr int maxNumberOfHarmonics = 100;
	
	HarmonicEditor();
	~HarmonicEditor() override = default;

	void paint (Graphics&) override;
	void resized() override;

	void mouseEnter(const MouseEvent&) override;
	void mouseExit(const MouseEvent&) override;
	void mouseDown(const MouseEvent&) override;
	void mouseUp(const MouseEvent&) override;
	void mouseDrag(const MouseEvent&) override;

	struct Listener
	{
		virtual void onHarmonicsChange(double* harmonics, int nHarmonics) = 0;
	};

	void setListener(Listener* pTheListener);

	void setNumberOfHarmonicSliders(const int nHarmonicsNew);
	void createHarmonicSliders();
	void resetSliders();

private:
	bool isEntered { false };
	bool isDown { false };
	
	OwnedArray<HarmonicSlider> harmonicSliders;

	int nHarmonics = 60;
	double harmonicValues[maxNumberOfHarmonics];
	Listener* pListener = nullptr;
	
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HarmonicEditor)
};
