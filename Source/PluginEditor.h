#pragma once

#include "PluginProcessor.h"
#include "MainComponent.h"
#include "CustomLookAndFeel.h"


class DdspsynthAudioProcessorEditor : public AudioProcessorEditor, private Timer/*, public SubtractiveComponent::SubtractiveListener, public AdditiveComponent::AdditiveListener, public OutputComponent::OutputListener*/
{
public:
	DdspsynthAudioProcessorEditor( DdspsynthAudioProcessor&, AudioProcessorValueTreeState&, AudioProcessorValueTreeState& );
	~DdspsynthAudioProcessorEditor() override;

	//==============================================================================
	void paint( Graphics& ) override;
	void resized() override;

	//==============================================================================
	void timerCallback() override;

	void setNumberOfHarmonics( const int numberOfHarmonics );
	void resetParameters();

private:
	//AdditiveComponent* additive;
	//HarmonicEditor* harmEditor;
	//SubtractiveComponent* subtractive;
	//OutputComponent* output;

	DdspsynthAudioProcessor& audioProcessor;
	AudioProcessorValueTreeState& synthVTS;
	AudioProcessorValueTreeState& modelVTS;

	//Image backgroundTexture { ImageFileFormat::loadFrom(BinaryData::background_texture_dark_headline_png, BinaryData::background_texture_dark_headline_pngSize) };

	MainComponent mainComponent;

	OtherLookAndFeel otherLookAndFeel;

	ValueTree defaultTreeState;

	//// Inherited via SubtractiveListener
	//virtual void onNoiseColorChange(double color) override;
	//virtual void onOnOffSubChange(bool onOff) override;
	//virtual void onSubAmpChange(double subAmp) override;

	//// Inherited via AdditiveListener
	//virtual void onAddAmpChange(double addAmp) override;
	//virtual void onShiftValueChange(double shiftValue) override;
	//virtual void onStretchValueChange(double stretchValue) override;
	//virtual void onOnOffAddChange(bool onOff) override;

	//// Inherited via OutputListener
	//virtual void onOutAmpChange(double outAmp) override;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( DdspsynthAudioProcessorEditor )
};
