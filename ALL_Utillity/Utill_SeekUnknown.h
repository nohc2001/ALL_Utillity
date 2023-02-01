#pragma once
#include <iostream>
#include <math.h>
#include "Utill_FreeMemory.h"

using namespace std;

#define MAX2(a, b) (a) > (b) ? a : b

namespace seek_unknown {

	// seek_unknown_pattern

	double sigmoid(double value) {
		return 1.0 / (1.0 + exp(-value));
	}

	typedef enum class ActivationType {
		Linear = 0,
		ReLU = 1,
		sigmoid = 2
	};

	class Neuron {
	public:
		int inputSiz = 0;
		freemem::FM_Model* FM;

		freemem::InfiniteArray<double> weights;
		double bias;

		ActivationType activationType;

		freemem::InfiniteArray<Neuron*>* input_;
		double output_; // saved for back prop
		double target_;

		Neuron()
			: inputSiz(0), bias(1.0), activationType(ActivationType::Linear)
		{
			Init_VPTR<freemem::InfiniteArray<double>>(&weights);
			weights.NULLState();
		}

		Neuron(const double& w_input, const double& b_input)
			: inputSiz(0), bias(b_input), activationType(ActivationType::Linear)
		{
			Init_VPTR<freemem::InfiniteArray<double>>(&weights);
			weights.NULLState();
		}

		virtual ~Neuron() {
			
		}

		void SetFM(freemem::FM_Model* FM) {
			weights.SetFM(FM);
			input_->SetFM(FM);
		}

		void SetInputSize(int size) {
			inputSiz = size;
			weights.Init(inputSiz);
			for (int i = 0; i < inputSiz; ++i) {
				weights[i] = (double)(rand() % 1000) / 1000.0;
			}
			bias = (double)(rand() % 1000) / 1000.0;
			input_->Init(inputSiz);
		}
		
		double feedForward() {
			//output y = f(\sigma)
			//\sigma = weight * input + bias
			//for multiple inputs,
			// \sigma = w0 * x0 + w1 * x1 + ... + b

			double sigma = bias;
			for (int i = 0; i < inputSiz; ++i) {
				sigma += weights[i] * ((*input_)[i]->output_);
			}
			output_ = getActivation(sigma);
			return output_;
		}

		void propBackward(const double& target) {
			const double alpha = 0.1; // learning rate

			const double grad = (output_ - target) * getActivationGradiant(output_);

			for (int i = 0; i < inputSiz; ++i) {
				double w_ = weights[i];
				weights[i] -= alpha * grad * ((*input_)[i]->output_); // last input_ came from d(wx + b)/dw=x
				(*input_)[i]->target_ -= alpha * grad * w_;
			}
			bias -= alpha * grad * 1.0; // last 1.0 came frome d(wx+b)/db = 1
		}

		double getActivation(const double& x) {
			switch (activationType) {
			case ActivationType::Linear:
				return x;
				break;
			case ActivationType::ReLU:
				return MAX2(0.0, x);
			case ActivationType::sigmoid:
				return sigmoid(x);
				//return 
			}
		}

		double getActivationGradiant(const double& x) {
			switch (activationType) {
			case ActivationType::Linear:
				return 1.0;
				break;
			case ActivationType::ReLU:
				if (x > 0.0) return 1.0;
				else return MAX2(0.0, x);
			case ActivationType::sigmoid:
				return sigmoid(x) * (1 - sigmoid(x));
			}
		}
	};

	class Neural_Layer {
	public:
		int NeuronNum = 0;
		freemem::FM_Model* FM;

		freemem::InfiniteArray<Neuron*> Input; // input neuron
		freemem::InfiniteArray<Neuron> Neurons; // this layer's neuron
		freemem::InfiniteArray<double*> Output; // neuron's output
		freemem::InfiniteArray<double*> Targets;

		Neural_Layer() {
			Init_VPTR<freemem::InfiniteArray<Neuron*>>(&Input);
			Input.NULLState();

			Init_VPTR<freemem::InfiniteArray<Neuron>>(&Neurons);
			Neurons.NULLState();
		}

		Neural_Layer(int Neuron_Num)
			: NeuronNum(Neuron_Num)
		{
			Init_VPTR<freemem::InfiniteArray<Neuron*>>(&Input);
			Input.NULLState();

			Init_VPTR<freemem::InfiniteArray<Neuron>>(&Neurons);
			Neurons.NULLState();
		}

		virtual ~Neural_Layer() {

		}

		void SetFM(freemem::FM_Model* fm) {
			FM = fm;
		}

		void SetStat(int inputNum, int neuronNum) {
			NeuronNum = neuronNum;
			Input.NULLState();
			Input.SetVPTR();
			Input.SetFM(FM);
			Input.Init(inputNum);
			for (int i = 0; i < inputNum; ++i) {
				Input[i]->SetFM(FM);
				Input[i]->SetInputSize(inputNum);
				Input[i]->input_ = nullptr;
			}

			Neurons.NULLState();
			Neurons.SetVPTR();
			Neurons.SetFM(FM);
			Neurons.Init(neuronNum);
			for (int i = 0; i < neuronNum; ++i) {
				Neurons[i].SetFM(FM);
				Neurons[i].SetInputSize(inputNum);
				Neurons[i].input_ = &Input;
			}

			Output.NULLState();
			Output.SetVPTR();
			Output.SetFM(FM);
			Output.Init(neuronNum);
			for (int i = 0; i < neuronNum; ++i) {
				Output[i] = nullptr;
			}

			Targets.NULLState();
			Targets.SetVPTR();
			Targets.SetFM(FM);
			Targets.Init(neuronNum);
			for (int i = 0; i < neuronNum; ++i) {
				Targets[i] = &Neurons[i].target_;
			}

		}

		void feedForward() {
			for (int i = 0; i < NeuronNum; ++i) {
				Neurons[i].feedForward();
				Output[i] = &Neurons[i].output_;
			}
		}

		void propBackward() {
			for (int i = 0; i < NeuronNum; ++i) {
				Neurons[i].propBackward(*Targets[i]);
			}
		}
	};

	class Neural_Network {
	public:
		int LayerNum = 0;
		freemem::FM_Model* FM;

		freemem::InfiniteArray<Neural_Layer> layers; // [0] - input  |  [last] - output
		int lastNeuronNum = 0;

		freemem::InfiniteArray<double> Target;

		Neural_Network() {

		}

		virtual ~Neural_Network() {

		}

		void SetFM(freemem::FM_Model* fm) {
			FM = fm;
		}

		void AddLayer(int NeuronSiz) {
			Neural_Layer nl;
			nl.SetFM(FM);
			nl.SetStat(lastNeuronNum, NeuronSiz);
			if (lastNeuronNum != 0) {
				for (int i = 0; i < lastNeuronNum; ++i) {
					nl.Input[i] = &layers[layers.size() - 1].Neurons[i];
				}
			}
			lastNeuronNum = NeuronSiz;
			layers.push_back(nl);
		}

		void SetTargetArr() {
			Target.NULLState();
			Target.SetVPTR();
			Target.SetFM(FM);
			Target.Init(layers[layers.size() - 1].NeuronNum);

		}

		void feedForward() {
			for (int i = 0; i < layers.size(); ++i) {
				layers[i].feedForward();
			}
		}

		void propBackward() {
			for (int i = layers.size() - 1; i >= 0; --i) {
				layers[i].propBackward();
			}
		}

		void SetTarget() {

		}
	};

	// seek_unknown_parameter

	// seek_unknown_solution
};