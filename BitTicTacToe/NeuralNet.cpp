#include "NeuralNet.h"
#include <exception>
#include <random>
#include <fstream>
#include <string>
#include <iomanip>
#include <sstream>

using namespace std;

NeuralNet::NeuralNet(vector<int> layerSizes, double(*activisionFunction)(double), double rndMin, double rndMax)
	:
	m_NumLayers((int)layerSizes.size()),
	m_ActivisionFunction(activisionFunction),
	m_RandRangeMin(rndMin),
	m_RandRangeMax(rndMax)
{
	if (m_NumLayers < 0)
		throw exception("Error: insufficient number of layers");
	
	// Initialize weights and biases for each layer except for the input layer
	for (int layer = 1; layer < m_NumLayers; layer++)
	{
		int numOfNeurons = layerSizes[layer];

		// Get random biases for this layer
		m_Biases.push_back(dVector(numOfNeurons));
		FillWithRandoms(m_Biases[layer-1]);
		
		vector<dVector> weights;

		for (int neuron = 0; neuron < numOfNeurons; neuron++)
		{
			// Get random weights for this neuron
			weights.push_back(dVector(layerSizes[layer - 1]));
			FillWithRandoms(weights[neuron]);
		}

		// Save weights for all neurons in this layer
		m_Weights.push_back(weights);
	}

}

NeuralNet::NeuralNet(vector<dVector> &biases, vector<vector<dVector>> &weights, double(*actFn)(double), double rndMin, double rndMax)
	:
	m_NumLayers((int)biases.size()),
	m_Biases(biases),
	m_Weights(weights),
	m_ActivisionFunction(actFn),
	m_RandRangeMin(rndMin),
	m_RandRangeMax(rndMax)
{
	if (m_NumLayers < 2)
		throw exception("Error: insufficient number of layers");
}

dVector NeuralNet::FeedForward(dVector &input)
{
	/* In the feed forward method, the input for each neuron
	is the output of the last layer, i.e. all the neurons are connected
	between each two adjacent layers
	*/
	dVector output;
	
	// Loop through each layer, except for the input layer
	for (int layer = 0; layer < m_NumLayers-1; layer++)
	{
		int neuronsInLayer = (int)m_Biases[layer].size();

		// The output for this layer
		 output = dVector(neuronsInLayer);

		/* Calculate output of each neuron f(w•x+b), where:
			w is the weight vector, 
			x is the input vector (output of previous layer)
			b is the bias of the neuron
			f is the activision function
			• denotes dot product
		*/
		for (int neuron = 0; neuron < neuronsInLayer; neuron++)
		{
			output[neuron] = m_ActivisionFunction(
				DotProduct(input, m_Weights[layer][neuron]) + 
				m_Biases[layer][neuron]);
		}

		// The output of this layer becomes the input of the next layer
		input = output;
	}

	return input;
}

NeuralNet NeuralNet::LoadNet(ifstream &in)
{
	// Read biases
	vector<dVector> biases;
	int num_layers;
	
	in.read(reinterpret_cast<char *>(&num_layers), sizeof(int));

	for (int layer = 0; layer < num_layers; layer++)
	{
		dVector biasesForLayer;
		int biasCount;
		in.read(reinterpret_cast<char *>(&biasCount), sizeof(int));
		for (int i = 0; i < biasCount; i++)
		{
			double bias;
			in.read(reinterpret_cast<char *>(&bias), sizeof(double));
			biasesForLayer.push_back(bias);
		}

		biases.push_back(biasesForLayer);
	}
	
	// Read weights
	vector<vector<dVector>> weights;
	for (int layer = 0; layer < num_layers; layer++)
	{
		vector<dVector> weightsForLayer;
		int num_neurons;
		in.read(reinterpret_cast<char *>(&num_neurons), sizeof(int));
		for (int neuron = 0; neuron < num_neurons; neuron++)
		{
			dVector weightsForNeuron;
			int num_weights;
			in.read(reinterpret_cast<char *>(&num_weights), sizeof(int));
			for (int i = 0; i < num_weights; i++)
			{
				double weight;
				in.read(reinterpret_cast<char *>(&weight), sizeof(double));
				weightsForNeuron.push_back(weight);
			}
			weightsForLayer.push_back(weightsForNeuron);
		}
		weights.push_back(weightsForLayer);
	}

	return NeuralNet(biases, weights);
}

void NeuralNet::SaveNet(ofstream &out)
{
	int numOfLayers = (int)m_Biases.size();
	out.write(reinterpret_cast<char *>(&numOfLayers), sizeof(int));

	/* Save biases */
	for (dVector &layer : m_Biases)
	{
		int layerSize = (int)layer.size();
		out.write(reinterpret_cast<char *>(&layerSize), sizeof(int));
		for (double bias : layer)
			out.write(reinterpret_cast<char *>(&bias), sizeof(double));
	}

	/* Save wieghts */
	for (vector<dVector> &layer : m_Weights)
	{
		int layerSize = (int)layer.size();
		out.write(reinterpret_cast<char *>(&layerSize), sizeof(int));
		for (dVector &neuron : layer)
		{
			int neuronSize = (int)neuron.size();
			out.write(reinterpret_cast<char *>(&neuronSize), sizeof(int));
			for (double weight : neuron)
				out.write(reinterpret_cast<char *>(&weight), sizeof(double));
		}
	}
}

double NeuralNet::GetRandomDouble(double min, double max)
{
	static random_device rd;
	static mt19937 generator(rd());
	static uniform_real_distribution<> dist(min, max);

	return dist(generator);
}

int NeuralNet::GetRandomInt(int min, int max)
{
	static random_device rd;
	static mt19937 generator(rd());
	static uniform_int_distribution<> dist(min, max);

	return dist(generator);
}

double NeuralNet::DotProduct(const dVector& x, const dVector& y)
{
	if (x.size() != y.size())
		throw exception("Error: vector lengths don't match");

	double result = 0;

	// Sum the products of individual corresponding components of the vectors
	for (int i = 0; i < x.size(); i++)
	{
		result += x[i] * y[i];
	}

	return result;
}

void NeuralNet::FillWithRandoms(dVector &vec)
{	
	int size = (int)vec.size();
	for (int i = 0; i < size; i++)
	{
		// Generate a random
		vec[i] = GetRandomDouble(m_RandRangeMin, m_RandRangeMax);
	}
}