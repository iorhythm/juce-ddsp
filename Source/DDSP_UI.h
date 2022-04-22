/*
  ==============================================================================

    DDSP_UI.h
    Created: 18 Apr 2022 10:50:03pm
    Author:  St

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==
// class DDSPLabel

class DDSPLabel : public Label
{
public:
	explicit DDSPLabel( const String& s ) :
		DDSPLabel{ s, 14.0f }
	{
	}

	DDSPLabel( const String& s, const float fsize ) :
		Label{ s, s }
	{
		setColour( Label::textColourId, Colours::white );
		setJustificationType( Justification::topLeft );
		setFont( fsize );
	}

	~DDSPLabel() override = default;


private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( DDSPLabel )
};

//--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==
// class AttachedSlider

class AttachedSlider : public juce::Slider
{
public:
	AttachedSlider( const juce::String& name,
		juce::AudioProcessorValueTreeState&, const juce::String& attach_name );
	~AttachedSlider() override = default;

	//void paint(Graphics&) override;
	//void resized() override;

private:
	std::unique_ptr< juce::AudioProcessorValueTreeState::SliderAttachment > attach;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( AttachedSlider )
};


//--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==
// class AttachedLabelledSlider

class AttachedLabelledSlider : public juce::Component
{
public:
	AttachedLabelledSlider( const juce::String& name,
		juce::AudioProcessorValueTreeState&, const juce::String& attach_name );
	~AttachedLabelledSlider() override = default;

	void paint( juce::Graphics& ) override;

	void resized() override;

	juce::Slider slider;
	juce::Label label;

private:
	std::unique_ptr< juce::AudioProcessorValueTreeState::SliderAttachment > attach;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( AttachedLabelledSlider )
};


//--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==
// class AttachedLabelledRotarySlider

class AttachedLabelledRotarySlider : public juce::Component
{
public:
	AttachedLabelledRotarySlider( const juce::String& name,
		juce::AudioProcessorValueTreeState&, const juce::String& attach_name );
	~AttachedLabelledRotarySlider() override = default;

	void paint( juce::Graphics& ) override;

	void resized() override;

	juce::Slider slider;
	juce::Label label;

private:
	std::unique_ptr< juce::AudioProcessorValueTreeState::SliderAttachment > attach;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( AttachedLabelledRotarySlider )
};


//--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==
// class AttachedImageButton

class AttachedImageButton : public juce::Component
{
public:
	AttachedImageButton( const juce::String& name,
		juce::AudioProcessorValueTreeState&, const juce::String& attach_name );
	~AttachedImageButton() override = default;

	//void paint( juce::Graphics& ) override;

	void resized() override;

	juce::ImageButton button;

private:
	std::unique_ptr< juce::AudioProcessorValueTreeState::ButtonAttachment > attach;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( AttachedImageButton )
};


//--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==
// class AttachedLabelledImageButton

class AttachedLabelledImageButton : public juce::Component
{
public:
	AttachedLabelledImageButton( const juce::String& name,
		juce::AudioProcessorValueTreeState&, const juce::String& attach_name );
	~AttachedLabelledImageButton() override = default;

	//void paint( juce::Graphics& ) override;

	void resized() override;

	juce::ImageButton button;
	juce::Label label;

private:
	std::unique_ptr< juce::AudioProcessorValueTreeState::ButtonAttachment > attach;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( AttachedLabelledImageButton )
};


class AttachedCombo : public juce::ComboBox
{
public:
	static constexpr auto w{ 200 };
	static constexpr auto h{ 30 };

	AttachedCombo( const juce::String& name, juce::AudioProcessorValueTreeState&,
		const juce::String& attachName );

	~AttachedCombo() override = default;

private:
	std::unique_ptr< juce::AudioProcessorValueTreeState::ComboBoxAttachment > attach;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( AttachedCombo )
};