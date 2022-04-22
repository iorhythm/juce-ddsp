/*
  ==============================================================================

    DDSP_UI.cpp
    Created: 18 Apr 2022 10:50:03pm
    Author:  St

  ==============================================================================
*/

#include "DDSP_UI.h"

//--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==
// class AttachedSlider

AttachedSlider::AttachedSlider( const juce::String& name, juce::AudioProcessorValueTreeState& t, const juce::String& attach_name ) :
	Slider{ name },
	//tree{ t },
	attach{ std::make_unique< juce::AudioProcessorValueTreeState::SliderAttachment >( t, attach_name, *this ) }
{
	setSliderStyle( juce::Slider::SliderStyle::LinearBarVertical );
	setTextBoxStyle( juce::Slider::TextEntryBoxPosition::TextBoxAbove, true, 50, 17 );
}


//--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==
// class AttachedLabelledSlider

AttachedLabelledSlider::AttachedLabelledSlider( const juce::String& name, juce::AudioProcessorValueTreeState& t, const juce::String& attach_name ) :
	slider{ name },
	label{ name, name },
	attach{ std::make_unique< juce::AudioProcessorValueTreeState::SliderAttachment >( t, attach_name, slider ) }
{
	slider.setSliderStyle( juce::Slider::SliderStyle::LinearBar );
	addAndMakeVisible( slider );

	label.setJustificationType( juce::Justification::centredLeft );
	label.attachToComponent( &slider, true );
	addAndMakeVisible( label );
}


void AttachedLabelledSlider::paint( juce::Graphics& g )
{
	g.setColour( juce::Colour{ 0xFF111111 } );
	g.drawRect( getLocalBounds() );
}


void AttachedLabelledSlider::resized()
{
	auto r{ getLocalBounds().reduced( 2 ) };

	label.setBounds( r.removeFromLeft( 100 ) );
	slider.setBounds( r );
}


//--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==
// class AttachedLabelledRotarySlider

AttachedLabelledRotarySlider::AttachedLabelledRotarySlider( const juce::String& name, juce::AudioProcessorValueTreeState& t, const juce::String& attach_name ) :
	slider{ name },
	label{ name, name },
	attach{ std::make_unique< juce::AudioProcessorValueTreeState::SliderAttachment >( t, attach_name, slider ) }
{

	// Stretch slider
	slider.setSliderStyle( Slider::SliderStyle::RotaryVerticalDrag );
	slider.setTextBoxStyle( Slider::NoTextBox, true, 0, 0 );
	slider.setPopupDisplayEnabled( true, true, this );
	slider.setBounds( 0, 0, 70, 86 );
	slider.setDoubleClickReturnValue( true, 0.0f );
	addAndMakeVisible( slider );

	label.setColour( Label::textColourId, Colours::white );
	label.setJustificationType( juce::Justification::centredLeft );
	label.setFont( 15.0f );
	label.attachToComponent( &slider, true );
	addAndMakeVisible( label );
}


void AttachedLabelledRotarySlider::paint( juce::Graphics& g )
{
	//g.setColour( juce::Colour{ 0xFF111111 } );
	//g.drawRect( getLocalBounds() );
}


void AttachedLabelledRotarySlider::resized()
{
	auto r{ getLocalBounds() };

	label.setBounds( r.removeFromBottom( 16 ) );
	slider.setBounds( r );
}


//--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==
// class AttachedImageButton

AttachedImageButton::AttachedImageButton( const juce::String& name, juce::AudioProcessorValueTreeState& t, const juce::String& attach_name ) :
	button{ name },
	attach{ std::make_unique< juce::AudioProcessorValueTreeState::ButtonAttachment >( t, attach_name, button ) }
{
	button.setClickingTogglesState( true );
	addAndMakeVisible( button );
}


void AttachedImageButton::resized()
{
	auto r{ getLocalBounds() };

	button.setBounds( r );
}


//--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==
// class AttachedLabelledImageButton

AttachedLabelledImageButton::AttachedLabelledImageButton( const juce::String& name, juce::AudioProcessorValueTreeState& t, const juce::String& attach_name ) :
	button{ name },
	label{ name, name },
	attach{ std::make_unique< juce::AudioProcessorValueTreeState::ButtonAttachment >( t, attach_name, button ) }
{
	button.setClickingTogglesState( true );
	addAndMakeVisible( button );

	label.setColour( Label::textColourId, Colours::white );
	label.setJustificationType( Justification::centred );
	label.attachToComponent( &button, true );
	addAndMakeVisible( label );
}


void AttachedLabelledImageButton::resized()
{
	auto r { getLocalBounds() };

	label.setBounds( r.removeFromBottom( 16 ) );
	button.setBounds( r );
}


AttachedCombo::AttachedCombo( const juce::String& name,
	juce::AudioProcessorValueTreeState& t,
	const juce::String& atch ) :
	ComboBox{ name },
	attach{ std::make_unique< juce::AudioProcessorValueTreeState::ComboBoxAttachment >( t, atch, *this ) }
{
	addItemList( t.getParameter( atch )->getAllValueStrings(), 1 );
	setSelectedItemIndex( static_cast<int>( t.getRawParameterValue( atch )->load() ) );

	setSize( w, h );
}