#pragma once

#include <JuceHeader.h>
#include <cstdio>
#include "HarmonicEditor.h"
#include "SubtractiveComponent.h"
#include "AdditiveComponent.h"
#include "TensorflowHandler.h"
#include "AubioHandler.h"
#include "codegen/DDSPSynth.h"


class DdspsynthAudioProcessor : public AudioProcessor, AsyncUpdater, AudioProcessorValueTreeState::Listener, public HarmonicEditor::Listener
{
public:
	using FVOP = juce::FloatVectorOperations;

	static constexpr auto fftOrder { 10 };
	static constexpr auto fftSize { 1 << fftOrder };

	static constexpr auto& maxAmplitudes { DDSP::maxAmplitudes };
	static constexpr auto& maxMagnitudes { DDSP::maxMagnitudes };
	static constexpr auto& maxHarmonics { DDSP::maxHarmonics };
	
	//==============================================================================
	DdspsynthAudioProcessor();
	~DdspsynthAudioProcessor() override;

	//==============================================================================
	void prepareToPlay (double sampleRate, int samplesPerBlock) override;
	void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
	bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

	void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

	//==============================================================================
	AudioProcessorEditor* createEditor() override;
	bool hasEditor() const override;

	//==============================================================================
	const String getName() const override;

	bool acceptsMidi() const override;
	bool producesMidi() const override;
	bool isMidiEffect() const override;
	double getTailLengthSeconds() const override;

	//==============================================================================
	int getNumPrograms() override;
	int getCurrentProgram() override;
	void setCurrentProgram (int index) override;
	const String getProgramName (int index) override;
	void changeProgramName (int index, const String& newName) override;

	//==============================================================================
	void getStateInformation (MemoryBlock& destData) override;
	void setStateInformation (const void* data, int sizeInBytes) override;
	
	//==============================================================================
	void pushNextSampleIntoFifo ( const float sample) noexcept;
	
	int getFftSize() { return fftSize; };
	int getFftOrder() { return fftOrder; };
	float* getFftData() { return &fftData[0]; };
	bool getNextFFTBlockReady() { return nextFFTBlockReady; };
	dsp::FFT *getForwardFFT() { return &forwardFFT; };

	void setNextFFTBlockReady(bool nextFFTBlockReady_) { nextFFTBlockReady = nextFFTBlockReady_; };

	void parseModelConfigJSON(const String& path);
	void setModelOutput(const TensorflowHandler::ModelResults& results);

	void onHarmonicsChange(double* harmonics, int nHarmonics);
	int getNumberOfHarmonics();

	void parameterChanged(const String& parameterID, float newValue ) override;

	DDSP::DDSPSynth ddspSynth;
private:
	// Parameters
	AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
	AudioProcessorValueTreeState::ParameterLayout createModelParameterLayout();

	AudioProcessorValueTreeState synthTree;
	AudioProcessorValueTreeState modelTree;
	
	// Internal parameters
	bool shouldSynthesize = true;
	DDSP::Harmonics phaseBufferI;
	DDSP::Harmonics phaseBufferO;
	double ld;
	double f0_in;
	double f0_out;
	std::array< float, maxAmplitudes > mlInput;
	std::array< double, maxAmplitudes > f0;
	int n_harmonics { 60 };
	DDSP::Harmonics harmonics;
	DDSP::Harmonics harms_copy;
	DDSP::Harmonics userHarmonics;
	double initial_bias = -5.0f;

	DDSP::Amplitudes amplitudes;
	DDSP::Amplitudes amplitudesCopy;
	
	DDSP::Amplitudes addBuffer;
	DDSP::Amplitudes subBuffer;

	DDSP::Magnitudes magnitudes;
	DDSP::Magnitudes mags_copy;

	int numSamples {};

	dsp::Gain< float > addSectionGain;
	dsp::Gain< float > subSectionGain;
	dsp::Gain< float > outputGain;

	// Midi features
	ADSR adsr;
	ADSR::Parameters adsrParams { 0.1f, 0.05f, 1.0f, 0.1f };
	float midiVelocity;
	float adsrVelocity;
	float midiNoteHz;
	int currentMidiNote {};

	// FFT Window
	dsp::FFT forwardFFT;
	
	float fifo [fftSize];
	float fftData [2 * fftSize];
	int fifoIndex {};
	bool nextFFTBlockReady { false };

	// Tensorflow 
	TensorflowHandler tfHandler;
	TensorflowHandler::ModelResults tfResults;

	// TF test
	float tf_f0 {};
	float tf_amps { -120.0f };

	File modelDir;

	AubioHandler& abHandler = AubioHandler::getInstance();

	// Inherited via AsyncUpdater
	virtual void handleAsyncUpdate() override;

	//==============================================================================
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DdspsynthAudioProcessor)
};
