#include "PluginProcessor.h"
#include "PluginEditor.h"


// ........ don't ask
FILE _iob[] = { *stdin, *stdout, *stderr };


extern "C" FILE * __cdecl __iob_func( void )
{
	return _iob;
}


extern "C" int __cdecl __ms_vsnprintf( char* s, size_t n, const char* format, va_list arg )
{
	//return printf(s, n, format, arg);
	printf( s, format );
	return 0;
}

//==============================================================================
DdspsynthAudioProcessor::DdspsynthAudioProcessor() :
#ifndef JucePlugin_PreferredChannelConfigurations
	AudioProcessor( BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
		.withInput( "Input", AudioChannelSet::mono(), true )
#endif
		.withOutput( "Output", AudioChannelSet::stereo(), true )
#endif
	),
#endif
	forwardFFT( fftOrder ),
	synthTree( *this, nullptr, Identifier( "DDSPSynth" ), createParameterLayout() ),
	modelTree( *this, nullptr, Identifier( "DDSPModels" ), createModelParameterLayout() )
{
	modelDir = File::getSpecialLocation( File::SpecialLocationType::currentApplicationFile ).getParentDirectory().getChildFile( "Models" );
	jassert( modelDir.exists() ); // Check that the Models folder exists in the same path as the plugin

	synthTree.addParameterListener( "inputIsLine", this );

	synthTree.addParameterListener( "additiveOn", this );
	synthTree.addParameterListener( "additiveShift", this );
	synthTree.addParameterListener( "additiveStretch", this );
	synthTree.addParameterListener( "additiveGain", this );
	synthTree.addParameterListener( "noiseOn", this );
	synthTree.addParameterListener( "noiseColor", this );
	synthTree.addParameterListener( "noiseGain", this );
	synthTree.addParameterListener( "modulationOn", this );
	synthTree.addParameterListener( "modulationRate", this );
	synthTree.addParameterListener( "modulationDelay", this );
	synthTree.addParameterListener( "modulationAmount", this );
	synthTree.addParameterListener( "reverbOn", this );
	synthTree.addParameterListener( "reverbMix", this );
	synthTree.addParameterListener( "reverbSize", this );
	synthTree.addParameterListener( "reverbGlow", this );
	synthTree.addParameterListener( "outputGain", this );
	synthTree.addParameterListener( "attack", this );
	synthTree.addParameterListener( "decay", this );
	synthTree.addParameterListener( "sustain", this );
	synthTree.addParameterListener( "release", this );

	modelTree.addParameterListener( "modelSelect", this );
	modelTree.addParameterListener( "modelOn", this );
}


DdspsynthAudioProcessor::~DdspsynthAudioProcessor()
{
	tfHandler.stopThread( 100 );
}

//==============================================================================
const String DdspsynthAudioProcessor::getName() const
{
	return JucePlugin_Name;
}

bool DdspsynthAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
	return true;
#else
	return false;
#endif
}

bool DdspsynthAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
	return true;
#else
	return false;
#endif
}

bool DdspsynthAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
	return true;
#else
	return false;
#endif
}

double DdspsynthAudioProcessor::getTailLengthSeconds() const
{
	return 0.0;
}

int DdspsynthAudioProcessor::getNumPrograms()
{
	return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
				// so this should be at least 1, even if you're not really implementing programs.
}

int DdspsynthAudioProcessor::getCurrentProgram()
{
	return 0;
}

void DdspsynthAudioProcessor::setCurrentProgram( int index )
{
}

const String DdspsynthAudioProcessor::getProgramName( int index )
{
	return {};
}

void DdspsynthAudioProcessor::changeProgramName( int index, const String& newName )
{
}

//==============================================================================
void DdspsynthAudioProcessor::prepareToPlay( double sampleRate, int samplesPerBlock )
{
	const auto numChannels{ jmax( getTotalNumInputChannels(), getTotalNumOutputChannels() ) };

	const dsp::ProcessSpec spec{
		sampleRate,
		static_cast<uint32>( samplesPerBlock ),
		static_cast<uint32>( numChannels )
	};

	tfHandler.setAsyncUpdater( this );

	const auto param = (AudioParameterChoice*) modelTree.getParameter( "modelSelect" );
	const String modelName = param->getCurrentChoiceName();
	const String modelPath = modelDir.getChildFile( modelName ).getFullPathName();

	parseModelConfigJSON( modelPath );
	tfHandler.loadModel( modelPath.getCharPointer() );

	adsr.setSampleRate( sampleRate );
	adsr.setParameters( adsrParams );
	adsr.reset();

	abHandler.prepare( "yinfft", maxAmplitudes, samplesPerBlock, (uint_t) sampleRate );
	abHandler.setTolerance( 0.8 );
	abHandler.setSilence( -50 );

	amplitudes.clear();
	amplitudesCopy.clear();

	magnitudes.fill( 6.0 );
	f0.fill( 0.0 );
	harmonics.fill( 0.0 );

	addBuffer.clear();
	subBuffer.clear();

	
	addSectionGain.prepare( spec );
	addSectionGain.setGainDecibels( *synthTree.getRawParameterValue( "additiveGain" ) );
	addSectionGain.setRampDurationSeconds( 0.1 );

	subSectionGain.prepare( spec );
	subSectionGain.setGainDecibels( *synthTree.getRawParameterValue( "noiseGain" ) );
	subSectionGain.setRampDurationSeconds( 0.1 );

	outputGain.prepare( spec );
	outputGain.setGainDecibels( *synthTree.getRawParameterValue( "outputGain" ) );
	outputGain.setRampDurationSeconds( 0.1 );
}


void DdspsynthAudioProcessor::releaseResources()
{
	abHandler.releaseResources();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DdspsynthAudioProcessor::isBusesLayoutSupported( const BusesLayout& layouts ) const
{
#if JucePlugin_IsMidiEffect
	ignoreUnused( layouts );
	return true;
#else
	// This is the place where you check if the layout is supported.
	// In this template code we only support mono or stereo.
	if(layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
		&& layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
		return false;

	// This checks if the input layout matches the output layout
   //#if ! JucePlugin_IsSynth
	if(layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
		return false;
	//#endif

	return true;
#endif
}
#endif

void DdspsynthAudioProcessor::processBlock( AudioBuffer<float>& buffer, MidiBuffer& midiMessages )
{
	shouldSynthesize = true;

	numSamples = buffer.getNumSamples();

	const auto modelOn { static_cast< bool >( *modelTree.getRawParameterValue( "modelOn" ) ) };

	if( *synthTree.getRawParameterValue( "inputIsLine" ) ) //Input is line-in
	{
		const auto* in_l = buffer.getReadPointer( 0 );
		auto aubioResults { abHandler.process( buffer ) };
		DBG( "AUBIO Confidence = " + String{  aubioResults.confidence } );

		FVOP::copy( mlInput.data(), buffer.getReadPointer( 0 ), numSamples );

		//for(int i {}; i < maxAmplitudes; ++i)
		//{
		//	if(i < numSamples)
		//		mlInput[ i ] = in_l[ i ];
		//	else
		//		mlInput[ i ] = 0.0f;
		//}

		//tf_amps = compute_loudness((double)numSamples, mlInput, getSampleRate());
		tf_amps = aubioResults.loudness;
		f0_in = aubioResults.pitch;
		tf_f0 = f0_in;

		f0.fill( f0_in );
	}
	else //Input is midi
	{
		buffer.clear();

		int time;
		MidiMessage m;

		for(MidiBuffer::Iterator i( midiMessages ); i.getNextEvent( m, time );)
		{
			if(m.isNoteOn())
			{
				adsr.reset();
				adsr.noteOn();
				midiVelocity = m.getFloatVelocity();
				currentMidiNote = m.getNoteNumber();
				midiNoteHz = m.getMidiNoteInHertz( currentMidiNote );
				tf_amps = -120.0f;
				tf_f0 = midiNoteHz;
			}
			else if(m.isNoteOff())
			{
				if(m.getNoteNumber() == currentMidiNote)
					adsr.noteOff();
			}
		}
		if(!adsr.isActive())
			shouldSynthesize = false;

		for(int i = 0; i < numSamples && adsr.isActive(); i++)
			f0[ i ] = midiNoteHz;

		for(int i = 0; i < numSamples - 1 && adsr.isActive(); i++)
			adsrVelocity = adsr.getNextSample();

		tf_f0 = midiNoteHz;
		tf_amps = Decibels::gainToDecibels( jmax( midiVelocity * adsrVelocity, 0.000001f ) );
	}

	if(modelOn)
	{
		if(!tfHandler.isThreadRunning())
		{
			tfHandler.setInputs( tf_f0, tf_amps );
			tfHandler.startThread();
		}
	}

	for(int i = 0; i < n_harmonics; i++)
	{
		if(modelOn)
			harms_copy[ i ] = harmonics[ i ] * ( 1.0 - userHarmonics[ i ] ) * 2.0;
		else
			harms_copy[ i ] = 10.0 * ( 1.0 - userHarmonics[ i ] ) - 5.0;
	}

	mags_copy = magnitudes;

	if(modelOn)
		amplitudesCopy = amplitudes;
	else
		FVOP::fill( amplitudesCopy.getWritePointer( 0 ), static_cast< double >( midiVelocity * adsrVelocity ), maxAmplitudes );

	if(*synthTree.getRawParameterValue( "additiveOn" ) && shouldSynthesize)
	{
		additive( (double) numSamples, getSampleRate(), 
			amplitudesCopy.getReadPointer( 0 ), 
			n_harmonics, 
			harms_copy.data(), 
			f0.data(), 
			phaseBuffer_in.data(), 
			(double) *synthTree.getRawParameterValue( "additiveShift" ), 
			(double) *synthTree.getRawParameterValue( "additiveStretch" ), 
			addBuffer.getWritePointer( 0 ), 
			phaseBuffer_out.data()
		);
	}
	else
		addBuffer.clear();

	if(*synthTree.getRawParameterValue( "noiseOn" ) && shouldSynthesize)
	{
		subtractive( numSamples, mags_copy.data(), (double) *synthTree.getRawParameterValue( "noiseColor" ), initial_bias, subBuffer.getWritePointer(0));
	}
	else
		subBuffer.clear();

	for(int i = 0; i < maxHarmonics; ++i)
		phaseBuffer_in[ i ] = phaseBuffer_out[ i ];

	auto* outL = buffer.getWritePointer( 0 );
	auto* outR = buffer.getWritePointer( 1 );

	for(int i = 0; i < numSamples; i++)
	{
		// workaround to prevent MIDI-related hearing damage
		const auto outGain { outputGain.getGainLinear() * (!modelOn ? 0.02f : 1.0f ) };

		const auto out { ( addSectionGain.processSample( addBuffer.getSample( 0, i ) ) + subSectionGain.processSample( subBuffer.getSample( 0, i ) ) ) * outGain };

		pushNextSampleIntoFifo( out );

		outL[ i ] = out;
		outR[ i ] = out;
	}
}


void DdspsynthAudioProcessor::parseModelConfigJSON( const String& path )
{
	// Parses config.json for model selected. Sets default values if file isn't found.
	File config_file = File( path + "/config.json" );
	jassert( config_file.existsAsFile() );
	var config = JSON::parse( config_file );
	n_harmonics = config.getProperty( "n_harmonics", 50 );
	initial_bias = config.getProperty( "initial_bias", -5.0f );

	if(config.hasProperty( "attack" ))
		adsrParams.attack = config.getProperty( "attack", var( *synthTree.getRawParameterValue( "attack" ) ) );

	if(config.hasProperty( "decay" ))
		adsrParams.decay = config.getProperty( "attack", var( *synthTree.getRawParameterValue( "decay" ) ) );

	if(config.hasProperty( "sustain" ))
		adsrParams.sustain = config.getProperty( "attack", var( *synthTree.getRawParameterValue( "sustain" ) ) );

	if(config.hasProperty( "release" ))
		adsrParams.release = config.getProperty( "attack", var( *synthTree.getRawParameterValue( "release" ) ) );

	adsr.setParameters( adsrParams );
}


void DdspsynthAudioProcessor::setModelOutput( const TensorflowHandler::ModelResults& tfResults )
{
	for(int i = 0; i < n_harmonics; i++)
		harmonics[ i ] = tfResults.harmonicDistribution[ i ];

	for(int i = 0; i < maxMagnitudes; i++)
		magnitudes[ i ] = tfResults.noiseMagnitudes[ i ];

	FVOP::fill( amplitudes.getWritePointer( 0 ), (double) tfResults.amplitudes[ 0 ] + 1.0f, numSamples );
}


void DdspsynthAudioProcessor::onHarmonicsChange( double* newHarmonics, int nHarmonics )
{
	for(int i = 0; i < nHarmonics && i < n_harmonics; i++)
		userHarmonics[ i ] = newHarmonics[ i ];
}


int DdspsynthAudioProcessor::getNumberOfHarmonics()
{
	return n_harmonics;
}


void DdspsynthAudioProcessor::parameterChanged( const String& parameterID, float newValue )
{
	//DBG(parameterID);

	if(parameterID == "modelSelect")
	{
		// getRawParameterValue is "not guaranteed" to contain up-to-date value
		auto param = (AudioParameterChoice*) modelTree.getParameter( "modelSelect" );
		String modelName = param->getCurrentChoiceName();
		String modelPath = modelDir.getChildFile( modelName ).getFullPathName();

		DBG( "Processor notified to select model " + modelName );

		if(modelDir.getChildFile( modelName ).exists())
		{
			parseModelConfigJSON( modelPath );
			tfHandler.loadModel( modelPath.getCharPointer() );
		}
		else
		{
			AlertWindow::showMessageBoxAsync( AlertWindow::AlertIconType::WarningIcon, "Model not found!", "The plugin expected to find the " + modelName + " model in this path:\n" + modelPath );
		}
	}

	if(parameterID == "modelSelect" || parameterID == "modelOn")
	{
		if(hasEditor())
			( (DdspsynthAudioProcessorEditor*) getActiveEditor() )->resetParameters();

		magnitudes.fill( 6.0 );
	}

	if( parameterID == "attack" )
	{
		adsrParams.attack = newValue;
		adsr.setParameters( adsrParams );
	}
	else if( parameterID == "decay" )
	{
		adsrParams.decay = newValue;
		adsr.setParameters( adsrParams );
	}
	else if( parameterID == "sustain" )
	{
		adsrParams.sustain = newValue;
		adsr.setParameters( adsrParams );
	}
	else if( parameterID == "release" )
	{
		adsrParams.release = newValue;
		adsr.setParameters( adsrParams );
	}
	else if( parameterID == "additiveGain" )
	{
		addSectionGain.setGainDecibels( newValue );
	}
	else if( parameterID == "noiseGain" )
	{
		subSectionGain.setGainDecibels( newValue );
	}
	else if( parameterID == "outputGain" )
	{
		outputGain.setGainDecibels( newValue );
	}
}

AudioProcessorValueTreeState::ParameterLayout DdspsynthAudioProcessor::createParameterLayout()
{
	AudioProcessorValueTreeState::ParameterLayout layout;

	// Input
	layout.add( std::make_unique<AudioParameterChoice>( "inputIsLine", "Input Selector", StringArray{ "MIDI", "AUDIO IN" }, 0 ) );
	// Additive
	layout.add( std::make_unique<AudioParameterBool>( "additiveOn", "Additive synth on", true ) );
	layout.add( std::make_unique<AudioParameterFloat>( "additiveShift", "Shift amount", NormalisableRange< float >{ -12.0f, 12.0f, 0.1f }, 0.0f ) );
	layout.add( std::make_unique<AudioParameterFloat>( "additiveStretch", "Stretch amount", NormalisableRange< float >{-1.0f, 1.0f, 0.01f }, 0.0f ) );
	layout.add( std::make_unique<AudioParameterFloat>( "additiveGain", "Additive gain", NormalisableRange< float >{ -12.0f, 6.0f, 0.01f }, 0.0f ) );
	// Subtractive
	layout.add( std::make_unique<AudioParameterBool>( "noiseOn", "Noise synth on", true ) );
	layout.add( std::make_unique<AudioParameterFloat>( "noiseColor", "Noise color", NormalisableRange< float >{ -1.0f, 1.0f, 0.01f }, 0.0f ) );
	layout.add( std::make_unique<AudioParameterFloat>( "noiseGain", "Noise gain", -12.0f, 6.0f, 0.0f ) );
	// Modulation
	layout.add( std::make_unique<AudioParameterBool>( "modulationOn", "Modulation on", false ) );
	layout.add( std::make_unique<AudioParameterFloat>( "modulationRate", "Rate", NormalisableRange< float >{ 0.0f, 10.0f, 0.01f }, 0.0f ) );
	layout.add( std::make_unique<AudioParameterFloat>( "modulationDelay", "Delay", NormalisableRange< float >{ 0.01f, 0.5f, 0.01f }, 0.0f ) );
	layout.add( std::make_unique<AudioParameterFloat>( "modulationAmount", "Amount", NormalisableRange< float >{ 0.0f, 100.0f, 0.01f }, 0.0f ) );
	// Reverb
	layout.add( std::make_unique<AudioParameterBool>( "reverbOn", "Reverb on", false ) );
	layout.add( std::make_unique<AudioParameterFloat>( "reverbMix", "Mix", NormalisableRange< float >{ 0.0f, 10.0f, 0.01f }, 0.0f ) );
	layout.add( std::make_unique<AudioParameterFloat>( "reverbSize", "Size", NormalisableRange< float >{ 0.10f, 2.0f, 0.01f }, 0.0f ) );
	layout.add( std::make_unique<AudioParameterFloat>( "reverbGlow", "Glow", NormalisableRange< float >{ 0.0f, 100.0f, 0.01f }, 0.0f ) );
	// Output
	layout.add( std::make_unique<AudioParameterFloat>( "outputGain", "Output gain", NormalisableRange< float >{ -12.0f, 6.0f, 0.01f }, 0.0f ) );
	// Midi
	layout.add( std::make_unique<AudioParameterFloat>( "attack", "Attack", NormalisableRange< float >{ 0.0f, 2.0f, 0.01f }, 0.1f ) );
	layout.add( std::make_unique<AudioParameterFloat>( "decay", "Decay", NormalisableRange< float >{ 0.0f, 2.0f, 0.01f }, 0.1f ) );
	layout.add( std::make_unique<AudioParameterFloat>( "sustain", "Sustain", NormalisableRange< float >{ 0.0f, 1.0f, 0.01f }, 0.1f ) );
	layout.add( std::make_unique<AudioParameterFloat>( "release", "Release", NormalisableRange< float >{ 0.0f, 2.0f, 0.01f }, 0.1f ) );

	return layout;
}


AudioProcessorValueTreeState::ParameterLayout DdspsynthAudioProcessor::createModelParameterLayout()
{
	AudioProcessorValueTreeState::ParameterLayout layout;

	layout.add( std::make_unique<AudioParameterBool>( "modelOn", "Use model", true ) );
	layout.add( std::make_unique<AudioParameterChoice>( "modelSelect", "Model select", StringArray( { "violin", "flute", "tenorsax", "trumpet" } ), 0 ) );

	return layout;
}


void DdspsynthAudioProcessor::handleAsyncUpdate()
{
	setModelOutput( tfHandler.getOutputs() );
}


//==============================================================================
bool DdspsynthAudioProcessor::hasEditor() const
{
	return true; // (change this to false if you choose to not supply an editor)
}


AudioProcessorEditor* DdspsynthAudioProcessor::createEditor()
{
	return new DdspsynthAudioProcessorEditor( *this, synthTree, modelTree );
}

//==============================================================================
void DdspsynthAudioProcessor::getStateInformation( MemoryBlock& destData )
{
	// You should use this method to store your parameters in the memory block.
	// You could do that either as raw data, or use the XML or ValueTree classes
	// as intermediaries to make it easy to save and load complex data.
}


void DdspsynthAudioProcessor::setStateInformation( const void* data, int sizeInBytes )
{
	// You should use this method to restore your parameters from this memory block,
	// whose contents will have been created by the getStateInformation() call.
}


//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new DdspsynthAudioProcessor();
}

//===============================================================================
// Spectrogram methods

void DdspsynthAudioProcessor::pushNextSampleIntoFifo( const float sample ) noexcept
{
	// if the fifo contains enough data, set a flag to say
	// that the next line should now be rendered..
	if(fifoIndex == fftSize)
	{
		if(!nextFFTBlockReady)
		{
			zeromem( fftData, sizeof( fftData ) );
			memcpy( fftData, fifo, sizeof( fifo ) );
			nextFFTBlockReady = true;
		}

		fifoIndex = 0;
	}

	fifo[ fifoIndex++ ] = sample;
}
