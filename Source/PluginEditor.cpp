#include "PluginEditor.h"


DdspsynthAudioProcessorEditor::DdspsynthAudioProcessorEditor( DdspsynthAudioProcessor& p, AudioProcessorValueTreeState& svts, AudioProcessorValueTreeState& mvts ) :
	AudioProcessorEditor( &p ),
	audioProcessor( p ),
	synthVTS( svts ),
	modelVTS( mvts ),
	mainComponent( svts, mvts )
{
	//backgroundTexture = backgroundTexture.rescaled(900, 600);
	//additive = (AdditiveComponent*)mainComponent.findChildWithID("additive");
	//harmEditor = (HarmonicEditor*)(additive->findChildWithID("harmonicEditor"));
	//subtractive = (SubtractiveComponent*)(mainComponent.findChildWithID("subtractive"));
	//output = (OutputComponent*)mainComponent.findChildWithID("output");

	mainComponent.additiveComponent.harmonicEditor.setListener( &p );
	//subtractive->setSubtractiveListener(this);
	//additive->setAdditiveListener(this);
	//output->setOutputListener(this);

	mainComponent.setLookAndFeel( &otherLookAndFeel );
	addAndMakeVisible( mainComponent );

	defaultTreeState = svts.copyState();

	startTimerHz( 60 );
	setSize( 900, 600 );
}


DdspsynthAudioProcessorEditor::~DdspsynthAudioProcessorEditor()
{
	mainComponent.setLookAndFeel( nullptr );
	stopTimer();
}


void DdspsynthAudioProcessorEditor::paint( Graphics& g )
{
	//g.drawImageAt(backgroundTexture, 0, 0);
	g.fillAll( Colour{ 0xFF111111 } );
}


void DdspsynthAudioProcessorEditor::resized()
{
	auto r{ getLocalBounds().reduced( 10 ) };

	mainComponent.setBounds( r );
}


// Spectrogram methods
void DdspsynthAudioProcessorEditor::timerCallback()
{
	if(audioProcessor.getNextFFTBlockReady())
	{
		mainComponent.spectogramComponent.drawNextLineOfSpectrogram( audioProcessor.getFftSize(),
			audioProcessor.getFftData(),
			*audioProcessor.getForwardFFT(),
			audioProcessor.getFftOrder() );
		audioProcessor.setNextFFTBlockReady( false );
	}

	setNumberOfHarmonics( audioProcessor.getNumberOfHarmonics() );
}


void DdspsynthAudioProcessorEditor::setNumberOfHarmonics( const int numberOfHarmonics )
{
	mainComponent.additiveComponent.harmonicEditor.setNumberOfHarmonicSliders( numberOfHarmonics );
}


void DdspsynthAudioProcessorEditor::resetParameters()
{
	mainComponent.additiveComponent.harmonicEditor.resetSliders();

	auto tmpCopy = defaultTreeState.createCopy();
	synthVTS.replaceState( defaultTreeState );
	defaultTreeState = tmpCopy;
}


//void DdspsynthAudioProcessorEditor::onNoiseColorChange(double color)
//{
//    audioProcessor.onNoiseColorChange(color);
//}
//
//void DdspsynthAudioProcessorEditor::onOnOffSubChange(bool onOff)
//{
//    audioProcessor.onOnOffSubChange(onOff);
//}
//
//void DdspsynthAudioProcessorEditor::onShiftValueChange(double shiftValue)
//{
//    audioProcessor.onShiftValueChange(shiftValue);
//}
//
//void DdspsynthAudioProcessorEditor::onStretchValueChange(double stretchValue)
//{
//    audioProcessor.onStretchValueChange(stretchValue);
//}
//
//void DdspsynthAudioProcessorEditor::onOnOffAddChange(bool onOff)
//{
//    audioProcessor.onOnOffAddChange(onOff);
//}
//
//void DdspsynthAudioProcessorEditor::onSubAmpChange(double subAmp)
//{
//    audioProcessor.onSubAmpChange(subAmp);
//}
//
//void DdspsynthAudioProcessorEditor::onAddAmpChange(double addAmp)
//{
//    audioProcessor.onAddAmpChange(addAmp);
//}
//
//void DdspsynthAudioProcessorEditor::onOutAmpChange(double outAmp)
//{
//    audioProcessor.onOutAmpChange(outAmp);
//}


