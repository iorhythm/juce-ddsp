/*
  ==============================================================================

	ReverbComponent.cpp
	Created: 7 Nov 2020 2:13:37am
	Author:  Robin Otterbein

  ==============================================================================
*/
#include "ReverbComponent.h"


ReverbComponent::ReverbComponent( AudioProcessorValueTreeState& t ) :
	onoffButton( "Reverb Switch", t, "reverbOn" ),
	sizeSlider{ "Size", t, "reverbSize" },
	glowSlider{ "Glow", t, "reverbGlow" },
	drywetSlider{ "Dry/Wet", t, "reverbMix" }
{
	addAndMakeVisible( onoffButton );
	//onoffButton.addListener(this);
	//onoffButton.setClickingTogglesState(true);
	onoffButton.button.setImages( false, true, false,
		ImageFileFormat::loadFrom( BinaryData::power_png, BinaryData::power_pngSize ), {}, Colour::fromRGB( 100, 100, 100 ), //Normal
		ImageFileFormat::loadFrom( BinaryData::power_png, BinaryData::power_pngSize ), {}, Colour::fromRGB( 200, 200, 200 ), //Over
		ImageFileFormat::loadFrom( BinaryData::power_png, BinaryData::power_pngSize ), {}, Colour::fromRGB( 255, 255, 255 ), //Down
		0.0f );

	addAndMakeVisible( nameLabel );

	addAndMakeVisible( drywetSlider );
	addAndMakeVisible( sizeSlider );
	addAndMakeVisible( glowSlider );
}


void ReverbComponent::paint( Graphics& g )
{
	g.setColour( Colours::dimgrey );
	g.drawRect( getLocalBounds(), 1 );
}


void ReverbComponent::resized()
{
	Grid grid;

	using Track = Grid::TrackInfo;
	using Fr = Grid::Fr;

	//7 rows
	grid.templateRows = {
		Track( Fr( 1 ) ), Track( Fr( 1 ) ), Track( Fr( 1 ) ), Track( Fr( 1 ) ),
		Track( Fr( 1 ) ), Track( Fr( 1 ) ), Track( Fr( 1 ) )
	};
	//3 column
	grid.templateColumns = {
		Track( Fr( 1 ) ), Track( Fr( 1 ) ), Track( Fr( 1 ) )
	};

	grid.rowGap = Grid::Px( 10.0f );
	grid.columnGap = Grid::Px( 10.0f );

	float sliderDim = 50.0f;

	grid.items = {
		GridItem( nameLabel ).withSize( 100.0f, 30.0f )
		.withAlignSelf( GridItem::AlignSelf::start )
		.withJustifySelf( GridItem::JustifySelf::start )
		.withMargin( GridItem::Margin( 10.0f ) )
		.withArea( 1, 1 ),

		GridItem( onoffButton ).withSize( 17.0f, 17.0f )
		.withAlignSelf( GridItem::AlignSelf::start )
		.withJustifySelf( GridItem::JustifySelf::end )
		.withMargin( GridItem::Margin( 10.0f, 10.0f, 10.0f, 0.0f ) )
		.withArea( 1, 3 ),

		GridItem( drywetSlider ).withSize( 70.0f, 70.0f )
		.withAlignSelf( GridItem::AlignSelf::start )
		.withJustifySelf( GridItem::JustifySelf::center )
		.withMargin( GridItem::Margin( 10.0f, 0, 0, 0 ) )
		.withArea( 2, 2 ),

		GridItem( sizeSlider ).withSize( sliderDim, sliderDim )
		.withAlignSelf( GridItem::AlignSelf::center )
		.withJustifySelf( GridItem::JustifySelf::start )
		.withMargin( GridItem::Margin( 0.0f, 0.0f, 10.0f, 10.0f ) )
		.withArea( 6, 1 ),

		GridItem( glowSlider ).withSize( sliderDim, sliderDim )
		.withAlignSelf( GridItem::AlignSelf::center )
		.withJustifySelf( GridItem::JustifySelf::end )
		.withMargin( GridItem::Margin( 0.0f, 10.0f, 10.0f, 0 ) )
		.withArea( 6, 3 )
	};

	grid.performLayout( getLocalBounds() );
}
