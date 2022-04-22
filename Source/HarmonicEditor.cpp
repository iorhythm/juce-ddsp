/*
  ==============================================================================

	HarmonicEditor.cpp
	Created: 10 Nov 2020 2:44:17pm
	Author:  Robin Otterbein

  ==============================================================================
*/
#include "HarmonicEditor.h"


void HarmonicSlider::paint( Graphics& g )
{
	g.fillAll( Colours::black );

	ColourGradient colourGradient = ColourGradient::vertical( Colour{ 0xFF232323 }, Colour{ 0xFF999999 }, rectangle );
	g.setGradientFill( colourGradient );
	g.fillRect( rectangle );
}


void HarmonicSlider::resized()
{
	rectangle = Rectangle<int>( 0, getHeight() * value, getWidth(), getHeight() * ( 1 - value ) + 1 );
}


void HarmonicSlider::setValue( const float newValue )
{
	value = std::clamp( newValue, 0.0f, 1.0f );

	repaint();
	resized();
}


HarmonicEditor::HarmonicEditor()
{
	createHarmonicSliders();
}


void HarmonicEditor::paint( Graphics& g )
{
}


void HarmonicEditor::setListener( HarmonicEditor::Listener* pTheListener )
{
	pListener = pTheListener;

	if(pListener != nullptr)
		pListener->onHarmonicsChange( harmonicValues, nHarmonics );
}


void HarmonicEditor::setNumberOfHarmonicSliders( const int nHarmonicsNew )
{
	if(nHarmonics != nHarmonicsNew)
	{
		nHarmonics = nHarmonicsNew;
		createHarmonicSliders();
	}
}


void HarmonicEditor::createHarmonicSliders()
{
	harmonicSliders.clear();

	for(int i = 0; i < nHarmonics; i++)
	{
		harmonicSliders.add( new HarmonicSlider() );
		harmonicValues[ i ] = 0.5;
	}

	for(HarmonicSlider* harmonicSlider : harmonicSliders)
	{
		addAndMakeVisible( harmonicSlider );
		harmonicSlider->setBounds( 0, 0, 100, 100 );
		harmonicSlider->addMouseListener( this, true );
	}
	resized();
}


void HarmonicEditor::resetSliders()
{
	for(int i = 0; i < nHarmonics; i++)
	{
		harmonicSliders[ i ]->setValue( 0.5 );
		harmonicValues[ i ] = 0.5;
	}

	repaint();

	if(pListener != nullptr)
	{
		pListener->onHarmonicsChange( harmonicValues, nHarmonics );
	}
}


void HarmonicEditor::resized()
{
	Grid grid;

	using Track = Grid::TrackInfo;
	using Fr = Grid::Fr;

	grid.templateRows = { Track( Fr( 1 ) ) };

	for(int i = 0; i < nHarmonics; i++)
	{
		grid.templateColumns.add( Track( Fr( 1 ) ) );
		grid.items.add( GridItem( harmonicSliders[ i ] ) );
	}

	grid.performLayout( getLocalBounds() );
}


void HarmonicEditor::mouseEnter( const MouseEvent& event )
{
	isEntered = true;
}


void HarmonicEditor::mouseExit( const MouseEvent& event )
{
	isEntered = false;
}


void HarmonicEditor::mouseDown( const MouseEvent& event )
{
	isDown = true;
}


void HarmonicEditor::mouseUp( const MouseEvent& event )
{
	isDown = false;
}


void HarmonicEditor::mouseDrag( const MouseEvent& event )
{
	if(isEntered && isDown)
	{
		const auto mousePositionX = event.getScreenX();
		const auto mousePositionY = event.getScreenY();
		const auto editorPositionX = getScreenX();
		const auto editorPositionY = getScreenY();
		const auto relativePositionX = mousePositionX - editorPositionX;
		const auto relativePositionY = mousePositionY - editorPositionY;
		const auto editorWidth = getWidth();
		const auto editorHeight = getHeight();
		const auto nActiveSlider = relativePositionX / ( editorWidth / nHarmonics );
		const auto newValue = float( relativePositionY ) / float( editorHeight );

		if(nActiveSlider >= 0 && nActiveSlider < nHarmonics)
		{
			harmonicSliders[ nActiveSlider ]->setValue( newValue );
			harmonicValues[ nActiveSlider ] = newValue;
		}

		if(pListener != nullptr)
		{
			pListener->onHarmonicsChange( harmonicValues, nHarmonics );
		}
		repaint();
	}
}
