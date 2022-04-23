#pragma once

#include <JuceHeader.h>
#include <cstdio>
#include "HarmonicEditor.h"
#include "SubtractiveComponent.h"
#include "AdditiveComponent.h"
#include "TensorflowHandler.h"
#include "AubioHandler.h"
#include "codegen/additive.h"
#include "codegen/subtractive.h"
#include "codegen/getPitch.h"
#include "codegen/compute_loudness.h"
#include "codegen/scale_f0.h"


class DdspsynthAudioProcessor : public AudioProcessor, AsyncUpdater, AudioProcessorValueTreeState::Listener, public HarmonicEditor::Listener
{
public:
	using FVOP = juce::FloatVectorOperations;

	static constexpr auto fftOrder { 10 };
	static constexpr auto fftSize { 1 << fftOrder };

	static constexpr auto maxAmplitudes { 4096 };
	static constexpr auto maxMagnitudes { 65 };
	static constexpr auto maxHarmonics { 100 };
	
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

private:
	// Parameters
	AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
	AudioProcessorValueTreeState::ParameterLayout createModelParameterLayout();

	AudioProcessorValueTreeState synthTree;
	AudioProcessorValueTreeState modelTree;
	
	// Internal parameters
	bool shouldSynthesize = true;
	std::array< double, maxHarmonics > phaseBuffer_in;
	std::array< double, maxHarmonics > phaseBuffer_out;
	double ld;
	double f0_in;
	double f0_out;
	std::array< float, maxAmplitudes > mlInput;
	std::array< double, maxAmplitudes > f0;
	int n_harmonics { 60 };
	std::array< double, maxHarmonics > harmonics;
	std::array< double, maxHarmonics > harms_copy;
	std::array< double, maxHarmonics > userHarmonics;
	double initial_bias = -5.0f;

	juce::AudioBuffer< double > amplitudes { 1, maxAmplitudes };
	juce::AudioBuffer< double > amplitudesCopy { 1, maxAmplitudes };
	
	juce::AudioBuffer< double > addBuffer { 1, maxAmplitudes };
	juce::AudioBuffer< double > subBuffer { 1, maxAmplitudes };

	std::array< double, maxMagnitudes > magnitudes;
	std::array< double, maxMagnitudes > mags_copy;

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
