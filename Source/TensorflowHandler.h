/*
  ==============================================================================

    TensorflowHandler.h
    Created: 3 Dec 2020 4:39:32pm
    Author:  david

  ==============================================================================
*/
#pragma once

#include <JuceHeader.h>
#include "tensorflow_c/include/tensorflow/c/c_api.h"


class TensorflowHandler : public Thread
{
public:
	TensorflowHandler();
	~TensorflowHandler();

	// TODO: this should probably be read from a gin file
	// or possibly the model file?
	static constexpr auto numHarmonics { 60 };
	static constexpr auto numMagnitudes { 65 };
	static constexpr auto timeSteps { 1 };

	struct ModelResults
	{
		float amplitudes[timeSteps];
		float harmonicDistribution[timeSteps * numHarmonics];
		float noiseMagnitudes[timeSteps * numMagnitudes];
	};

	void loadModel(const char* path);
	void unloadModel();
	void setInputs(float f0, float amps);
	void setAsyncUpdater(AsyncUpdater* _asyncUpdater) { processUpdater = _asyncUpdater; };
	ModelResults getOutputs() { return results; };

	void run() override;

private:
	//DdspsynthAudioProcessor* audioProcessor;

	CriticalSection lock;

	DynamicLibrary tfLibrary;

	TF_Graph* tfGraph = nullptr;
	TF_Status* tfStatus = nullptr;
	TF_SessionOptions* tfSessionOpts = nullptr;
	TF_Session* tfSession = nullptr;
	TF_Buffer* tfRunOpts = nullptr;

	int numInputs = 2;
	TF_Output* tfInput = nullptr;
	TF_Output f0Input;
	TF_Output ldInput;

	int numOutputs = 3;
	TF_Output* tfOutput = nullptr;
	TF_Output ampsOutput;
	TF_Output harmsOutput;
	TF_Output magsOutput;

	TF_Tensor** tfInputValues = nullptr;
	TF_Tensor** tfOutputValues = nullptr;

	int numInputDims = 3;
	int64_t inputDims[3] = { 1,timeSteps,1 };

	int ntags = 1;

	const char* tags = "serve";

	int ndata = 4 * timeSteps;
	TF_Tensor* f0InputTensor;
	TF_Tensor* ldInputTensor;
	
	ModelResults results;

	AsyncUpdater* processUpdater;

	typedef TF_Graph* (*fptypeNewGraph) ();
	typedef void (*fptypeDeleteGraph) (TF_Graph*);
	typedef TF_Status* (*fptypeNewStatus) ();
	typedef void (*fptypeDeleteStatus) (TF_Status*);
	typedef TF_SessionOptions* (*fptypeNewSessionOpts) ();
	typedef void (*fptypeDeleteSessionOptions) (TF_SessionOptions*);
	typedef TF_Session* ( *fptypeLoadSession ) (
		const TF_SessionOptions*, const TF_Buffer*,
		const char*, const char* const*, int,
		TF_Graph*, TF_Buffer*, TF_Status*
		);
	typedef void ( *fptypeDeleteSession ) ( TF_Session*, TF_Status* );
	typedef TF_Operation* (*fptypeGraphOperation) (TF_Graph*, const char*);
	typedef TF_Tensor* ( *fptypeNewTensor ) (
		TF_DataType, const int64_t*, int, void*, size_t,
		void( *deallocator )( void* data, size_t len, void* arg ), void*
		);
	typedef void (*fptypeDeleteTensor) (TF_Tensor*);
	typedef void* ( *fptypeTensorData ) ( const TF_Tensor* );
	typedef void( *fptypeSessionRun ) (
		TF_Session*, const TF_Buffer* run_options,
		const TF_Output*, TF_Tensor* const*, int,
		const TF_Output*, TF_Tensor**, int,
		const TF_Operation* const*, int,
		TF_Buffer*, TF_Status*
		);

	fptypeNewGraph fpNewGraph;
	fptypeDeleteGraph fpDeleteGraph;
	fptypeNewStatus fpNewStatus;
	fptypeDeleteStatus fpDeleteStatus;
	fptypeNewSessionOpts fpNewSessionOpts;
	fptypeDeleteSessionOptions fpDeleteSessionOptions;
	fptypeLoadSession fpLoadSession;
	fptypeDeleteSession fpDeleteSession;
	fptypeGraphOperation fpGraphOperation;
	fptypeNewTensor fpNewTensor;
	fptypeDeleteTensor fpDeleteTensor;
	fptypeTensorData fpTensorData;
	fptypeSessionRun fpSessionRun;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TensorflowHandler)
};