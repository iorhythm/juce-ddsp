#pragma once

#include <JuceHeader.h>
#include <cstdio>
#include "HarmonicEditor.h"
#include "SubtractiveComponent.h"
#include "AdditiveComponent.h"
#include "TensorflowHandler.h"
#include "AubioHandler.h"


class DdspsynthAudioProcessor : public AudioProcessor, AsyncUpdater, AudioProcessorValueTreeState::Listener, public HarmonicEditor::Listener
{
public:
	static constexpr auto fftOrder { 10 };
	static constexpr auto fftSize { 1 << fftOrder };

	static constexpr auto maxAmplitudes { 4096 };
	static constexpr auto max_n_harmonics { 100 };

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
	void pushNextSampleIntoFifo (float sample) noexcept;
	
	int getFftSize() { return fftSize; };
	int getFftOrder() { return fftOrder; };
	float* getFftData() { return &fftData[0]; };
	bool getNextFFTBlockReady() { return nextFFTBlockReady; };
	dsp::FFT *getForwardFFT() { return &forwardFFT; };

	void setNextFFTBlockReady(bool nextFFTBlockReady_) { nextFFTBlockReady = nextFFTBlockReady_; };

	void parseModelConfigJSON(String path);
	void setModelOutput(TensorflowHandler::ModelResults results);

	void onHarmonicsChange(double* harmonics, int nHarmonics);
	int getNumberOfHarmonics();

	void parameterChanged(const String &parameterID, float newValue ) override;

private:
	AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
	AudioProcessorValueTreeState::ParameterLayout createModelParameterLayout();

	// Parameters
	AudioProcessorValueTreeState synthParameters;
	AudioProcessorValueTreeState modelParameters;

	std::atomic<float>* inputSelectParameter = nullptr;
	std::atomic<float>* modelOnParameter = nullptr;
	std::atomic<float>* modelChoiceParameter = nullptr;
	std::atomic<float>* additiveOnParameter = nullptr;
	std::atomic<float>* additiveShiftParameter = nullptr;
	std::atomic<float>* additiveStretchParameter = nullptr;
	std::atomic<float>* additiveGainParameter = nullptr;
	std::atomic<float>* noiseOnParameter = nullptr;
	std::atomic<float>* noiseColorParameter = nullptr;
	std::atomic<float>* noiseGainParameter = nullptr;
	std::atomic<float>* modulationOnParameter = nullptr;
	std::atomic<float>* modulationRateParameter = nullptr;
	std::atomic<float>* modulationDelayParameter = nullptr;
	std::atomic<float>* modulationAmountParameter = nullptr;
	std::atomic<float>* reverbOnParameter = nullptr;
	std::atomic<float>* reverbMixParameter = nullptr;
	std::atomic<float>* reverbSizeParameter = nullptr;
	std::atomic<float>* reverbGlowParameter = nullptr;
	std::atomic<float>* outputGainParameter = nullptr;
	std::atomic<float>* attackParameter = nullptr;
	std::atomic<float>* decayParameter = nullptr;
	std::atomic<float>* sustainParameter = nullptr;
	std::atomic<float>* releaseParameter = nullptr;
	
	// Internal parameters
	bool shouldSynthesize = true;
	double phaseBuffer_in[max_n_harmonics];
	double phaseBuffer_out[max_n_harmonics];
	double amplitudes[maxAmplitudes];
	double ld;
	double f0_in;
	double f0_out;
	double f0[maxAmplitudes];
	double n_harmonics = 60;
	double harmonics[max_n_harmonics];
	double userHarmonics[max_n_harmonics];
	double addBuffer[maxAmplitudes];
	double initial_bias = -5.0f;
	double subBuffer[maxAmplitudes];
	double magnitudes[65];
	double numSamples;

	// Midi features
	ADSR adsr;
	ADSR::Parameters adsrParams { 0.1f, 0.05f, 1.0f, 0.1f };
	float midiVelocity;
	float adsrVelocity;
	float midiNoteHz;
	int currentMidiNote = 0;

	// FFT Window
	dsp::FFT forwardFFT;
	
	float fifo [fftSize];
	float fftData [2 * fftSize];
	int fifoIndex = 0;
	bool nextFFTBlockReady = false;

	// Tensorflow 
	TensorflowHandler tfHandler;
	TensorflowHandler::ModelResults tfResults;

	// TF test
	float tf_f0;
	float tf_amps;

	File modelDir;

	AubioHandler& abHandler = AubioHandler::getInstance();

	// Inherited via AsyncUpdater
	virtual void handleAsyncUpdate() override;

	//==============================================================================
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DdspsynthAudioProcessor)
};
