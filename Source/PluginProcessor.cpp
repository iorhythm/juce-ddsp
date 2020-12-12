/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "HarmonicEditor.h"
#include "codegen/additive.h"
#include "codegen/subtractive.h"
#include "codegen/getPitch.h"
#include "codegen/compute_loudness.h"
#include "codegen/scale_f0.h"
#include "TensorflowHandler.h"


// ........ don't ask
FILE _iob[] = { *stdin, *stdout, *stderr };

extern "C" FILE * __cdecl __iob_func(void)
{
	return _iob;
}

extern "C" int __cdecl __ms_vsnprintf(char* s, size_t n, const char* format, va_list arg)
{
	//return printf(s, n, format, arg);
	printf(s, format);
	return 0;
}

//==============================================================================
DdspsynthAudioProcessor::DdspsynthAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
	: AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
		.withInput("Input", juce::AudioChannelSet::mono(), true)
#endif
		.withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
	),

#endif
	: forwardFFT(fftOrder),
	parameters(*this, nullptr, juce::Identifier("DDSPSynth"),
		{
			// Input
		   std::make_unique<juce::AudioParameterBool>("inputIsLine", "Input is line in", false),
		   // Model
		   std::make_unique<juce::AudioParameterBool>("modelOn", "Use model", true),
		   std::make_unique<juce::AudioParameterChoice>("modelSelect", "Model select", juce::StringArray({ "violin", "flute", "tenorsax", "trumpet" }), 0),
		   // Additive
		   std::make_unique<juce::AudioParameterBool>("additiveOn", "Additive synth on", true),
		   std::make_unique<juce::AudioParameterFloat>("additiveShift", "Shift amount", -12.0f, 12.0f, 0.0f),
		   std::make_unique<juce::AudioParameterFloat>("additiveStretch", "Stretch amount", -1.0f, 1.0f, 0.0f),
		   std::make_unique<juce::AudioParameterFloat>("additiveGain", "Additive gain", -60.0f, 0.0f, -6.0f),
		   // Subtractive
		   std::make_unique<juce::AudioParameterBool>("noiseOn", "Noise synth on", true),
		   std::make_unique<juce::AudioParameterFloat>("noiseColor", "Noise color", -1.0f, 1.0f, 0.0f),
		   std::make_unique<juce::AudioParameterFloat>("noiseGain", "Noise gain", -60.0f, 0.0f, -6.0f),
		   // Modulation
		   std::make_unique<juce::AudioParameterBool>("modulationOn", "Modulation on", false),
		   std::make_unique<juce::AudioParameterFloat>("modulationRate", "Rate", 0.0f, 10.0f, 1.0f),
		   std::make_unique<juce::AudioParameterFloat>("modulationDelay", "Delay", 0.01f, 0.5f, 0.03f),
		   std::make_unique<juce::AudioParameterFloat>("modulationAmount", "Amount", 0.0f, 100.0f, 50.0f),
		   // Reverb
		   std::make_unique<juce::AudioParameterBool>("reverbOn", "Reverb on", false),
		   std::make_unique<juce::AudioParameterFloat>("reverbMix", "Mix", 0.0f, 10.0f, 1.0f),
		   std::make_unique<juce::AudioParameterFloat>("reverbSize", "Size", 0.10f, 2.0f, 1.0f),
		   std::make_unique<juce::AudioParameterFloat>("reverbGlow", "Glow", 0.0f, 100.0f, 0.0f),
		   // Output
		   std::make_unique<juce::AudioParameterFloat>("outputGain", "Output gain", -60.0f, 0.0f, -6.0f),
		})/*,
		tfHandler(*this)*/
{
	inputSelectParameter = parameters.getRawParameterValue("inputIsLine");
	modelOnParameter = parameters.getRawParameterValue("modelOn");
	modelChoiceParameter = parameters.getRawParameterValue("modelSelect");
	additiveOnParameter = parameters.getRawParameterValue("additiveOn");
	additiveShiftParameter = parameters.getRawParameterValue("additiveShift");
	additiveStretchParameter = parameters.getRawParameterValue("additiveStretch");
	additiveGainParameter = parameters.getRawParameterValue("additiveGain");
	noiseOnParameter = parameters.getRawParameterValue("noiseOn");
	noiseColorParameter = parameters.getRawParameterValue("noiseColor");
	noiseGainParameter = parameters.getRawParameterValue("noiseGain");
	modulationOnParameter = parameters.getRawParameterValue("modulationOn");
	modulationRateParameter = parameters.getRawParameterValue("modulationRate");
	modulationDelayParameter = parameters.getRawParameterValue("modulationDelay");
	modulationAmountParameter = parameters.getRawParameterValue("modulationAmount");
	reverbOnParameter = parameters.getRawParameterValue("reverbOn");
	reverbMixParameter = parameters.getRawParameterValue("reverbMix");
	reverbSizeParameter = parameters.getRawParameterValue("reverbSize");
	reverbGlowParameter = parameters.getRawParameterValue("reverbGlow");
	outputGainParameter = parameters.getRawParameterValue("outputGain");

	parameters.addParameterListener("modelSelect", this);
	for (int i = 0; i < 65; i++) {
		magnitudes[i] = 0;
	}
	for (int i = 0; i < 4096; i++) {
		amplitudes[i] = 0;
		f0[i] = 0;
	}
	for (int i = 0; i < 50; i++) {
		harmonics[i] = 0.0;
	}

	tf_amps = -200;
	tf_f0 = 0;
}

DdspsynthAudioProcessor::~DdspsynthAudioProcessor()
{
	tfHandler.stopThread(100);
}

//==============================================================================
const juce::String DdspsynthAudioProcessor::getName() const
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

void DdspsynthAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String DdspsynthAudioProcessor::getProgramName(int index)
{
	return {};
}

void DdspsynthAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}

//==============================================================================
void DdspsynthAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
	tfHandler.setAsyncUpdater(this);
	tfHandler.loadModel((modelDir + "violin").getCharPointer());

	adsr.setSampleRate(sampleRate);
	adsr.setParameters(adsrParams);

	aubioPitch.reset(new_aubio_pitch("yinfft", 4096, samplesPerBlock, (uint_t)sampleRate));

}

void DdspsynthAudioProcessor::releaseResources()
{
	// When playback stops, you can use this as an opportunity to free up any
	// spare memory, etc.
	aubioPitch.release();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DdspsynthAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
	juce::ignoreUnused(layouts);
	return true;
#else
	// This is the place where you check if the layout is supported.
	// In this template code we only support mono or stereo.
	if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
		&& layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
		return false;

	// This checks if the input layout matches the output layout
   //#if ! JucePlugin_IsSynth
	if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
		return false;
	//#endif

	return true;
#endif
}
#endif

void DdspsynthAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
	const float* in_l = buffer.getReadPointer(0);
	numSamples = buffer.getNumSamples();
	if (*inputSelectParameter) //Input is line-in
	{
		fvec_t aubioInput;
		fvec_t aubioOutput;
		float aubiof0;

		// need write pointer because the data type is not const
		aubioInput.data = buffer.getWritePointer(0);
		aubioInput.length = buffer.getNumSamples();

		aubioOutput.data = &aubiof0;
		aubioOutput.length = 1;

		aubio_pitch_do(aubioPitch.get(), &aubioInput, &aubioOutput);
		DBG(aubiof0);
		double mlInput[4096];
		for (int i = 0; i < 4096; i++)
		{
			if (i < numSamples)
				mlInput[i] = in_l[i];
			else
				mlInput[i] = 0;
		}

		tf_amps = compute_loudness((double)numSamples, mlInput, getSampleRate());

		f0_in = aubiof0;
		tf_f0 = f0_in;
		for (int i = 0; i < 4096; i++)
		{
			f0[i] = f0_in;
		}
	}
	else //Input is midi
	{
		buffer.clear();
		int time;
		juce::MidiMessage m;
		for (juce::MidiBuffer::Iterator i(midiMessages); i.getNextEvent(m, time);)
		{
			if (m.isNoteOn())
			{
				adsr.reset();
				adsr.noteOn();
				midiVelocity = m.getFloatVelocity();
				int note = m.getNoteNumber();
				midiNoteHz = m.getMidiNoteInHertz(note);
				tf_amps = -120;
				tf_f0 = midiNoteHz;
				for (int i = 0; i < numSamples; i++)
				{
					f0[i] = midiNoteHz;
					adsrVelocity = adsr.getNextSample();
				}

			}
			else if (m.isNoteOff())
			{
				adsr.noteOff();
			}
		}
		if (adsr.isActive())
		{
			tf_f0 = midiNoteHz;
			tf_amps = (120.0f * midiVelocity * adsrVelocity) - 120.0f;
			DBG((120.0f * midiVelocity * adsrVelocity) - 120.0f);
			for (int i = 0; i < numSamples; i++)
			{
				f0[i] = midiNoteHz;
				adsrVelocity = adsr.getNextSample();
			}
		}
	}

	if (!tfHandler.isThreadRunning())
	{
		tfHandler.setInputs(tf_f0, tf_amps);
		tfHandler.startThread();
	}

	double harms_copy[60];
	double mags_copy[65];
	double amps_copy[4096];
	for (int i = 0; i < 50; i++) {
		harms_copy[i] = harmonics[i];
	}
	for (int i = 0; i < 65; i++) {
		mags_copy[i] = magnitudes[i];
	}
	for (int i = 0; i < 4096; i++) {
		amps_copy[i] = amplitudes[i];
	}

	if (*additiveOnParameter) {
		additive((double)numSamples, getSampleRate(), amps_copy, n_harmonics, harms_copy, f0, phaseBuffer_in, (double)*additiveShiftParameter, (double)*additiveStretchParameter, addBuffer, phaseBuffer_out);
	}
	else {
		for (int i = 0; i < 4096; i++)
		{
			addBuffer[i] = 0;
		}
	}


	if (*noiseOnParameter)
	{
		subtractive(numSamples, mags_copy, (double)*noiseColorParameter, initial_bias, subBuffer);
	}
	else {
		for (int i = 0; i < 4096; i++)
		{
			subBuffer[i] = 0;
		}
	}
	for (int i = 0; i < 50; ++i) {
		phaseBuffer_in[i] = phaseBuffer_out[i];
	}

	auto outL = buffer.getWritePointer(0);
	auto outR = buffer.getWritePointer(1);

	for (int i = 0; i < buffer.getNumSamples(); i++) {
		float additiveGain = pow(10.0f, (*additiveGainParameter / 20));
		float noiseGain = pow(10.0f, (*noiseGainParameter / 20));
		float outGain = pow(10.0f, (*outputGainParameter / 20));
		float out = (addBuffer[i] * additiveGain + subBuffer[i] * noiseGain) * outGain;
		pushNextSampleIntoFifo(out);
		outL[i] = out;
		outR[i] = out;
	}
}

void DdspsynthAudioProcessor::setModelOutput(TensorflowHandler::ModelResults tfResults)
{
	for (int i = 0; i < 50; i++) {
		harmonics[i] = tfResults.harmonicDistribution[i];
	}
	for (int i = 0; i < 65; i++) {
		magnitudes[i] = tfResults.noiseMagnitudes[i];
	}
	for (int i = 0; i < numSamples; i++) {
		amplitudes[i] = tfResults.amplitudes[0];
	}
}

void DdspsynthAudioProcessor::parameterChanged(const juce::String & parameterID, float newValue)
{
	if (parameterID == "modelSelect")
	{
		// getRawParameterValue is "not guaranteed" to contain up-to-date value
		auto param = (juce::AudioParameterChoice*) parameters.getParameter("modelSelect");
		juce::String modelName = param->getCurrentChoiceName();
		DBG("Processor notified to select model " + modelName);
		tfHandler.loadModel((modelDir + modelName).getCharPointer());
	}
}

void DdspsynthAudioProcessor::handleAsyncUpdate()
{
	setModelOutput(tfHandler.getOutputs());
}


//==============================================================================
bool DdspsynthAudioProcessor::hasEditor() const
{
	return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* DdspsynthAudioProcessor::createEditor()
{
	return new DdspsynthAudioProcessorEditor(*this, parameters);
}

//==============================================================================
void DdspsynthAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
	// You should use this method to store your parameters in the memory block.
	// You could do that either as raw data, or use the XML or ValueTree classes
	// as intermediaries to make it easy to save and load complex data.
}

void DdspsynthAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
	// You should use this method to restore your parameters from this memory block,
	// whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new DdspsynthAudioProcessor();
}

//===============================================================================
// Spectrogram methods

void DdspsynthAudioProcessor::pushNextSampleIntoFifo(float sample) noexcept
{
	// if the fifo contains enough data, set a flag to say
	// that the next line should now be rendered..
	if (fifoIndex == fftSize)
	{
		if (!nextFFTBlockReady)
		{
			juce::zeromem(fftData, sizeof(fftData));
			memcpy(fftData, fifo, sizeof(fifo));
			nextFFTBlockReady = true;
		}

		fifoIndex = 0;
	}

	fifo[fifoIndex++] = sample;
}
