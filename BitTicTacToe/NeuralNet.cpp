#include "NeuralNet.h"
#include <exception>
#include <random>

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

NeuralNet::NeuralNet(vector<dVector> biases, vector<vector<dVector>> weights, double(*actFn)(double), double rndMin, double rndMax)
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
	is the /whole/ output of the last layer, i.e. all the neurons are connected
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
	static random_device rd;

	// Random number generator
	static mt19937 generator(rd());

	// Uniform distribution within range
	static uniform_real_distribution<> dist(m_RandRangeMin,m_RandRangeMax);

	int size = (int)vec.size();
	for (int i = 0; i < size; i++)
	{
		// Generate a random
		vec[i] = dist(generator);
	}
}