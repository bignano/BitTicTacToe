#include "NeuralNet.h"
#include <exception>
#include <random>
#include <fstream>
#include <string>
#include <iomanip>
#include <sstream>

using namespace std;

NeuralNet::NeuralNet(vector<size_t> layerSizes, double(*activisionFunction)(double), double rndMin, double rndMax)
	:
	m_NumLayers(layerSizes.size()),
	m_ActivisionFunction(activisionFunction),
	m_RandRangeMin(rndMin),
	m_RandRangeMax(rndMax)
{
	if (m_NumLayers < 0)
		throw exception("Error: insufficient number of layers");

	// pre-allocate for speed
	m_Weights = vector<vector<dVector>>(layerSizes.size() - 1);

	// Initialize weights for each layer except for the input layer
	for (int layer = 1; layer < m_NumLayers; layer++)
	{
		size_t numOfNeurons = layerSizes[layer] + 1;

		vector<dVector> weights(numOfNeurons);

		for (int neuron = 0; neuron < numOfNeurons; neuron++)
		{
			// Get random weights for this neuron
			weights[neuron] = dVector(layerSizes[layer - 1] + 1);
			FillWithRandoms(weights[neuron]);
		}

		// Save weights for all neurons in this layer
		m_Weights[layer - 1] = weights;
	}

}

NeuralNet::NeuralNet(vector<vector<dVector>> &weights, double(*actFn)(double), double rndMin, double rndMax)
	:
	m_NumLayers(weights.size() + 1),
	//m_Biases(biases),
	m_Weights(weights),
	m_ActivisionFunction(actFn),
	m_RandRangeMin(rndMin),
	m_RandRangeMax(rndMax)
{
	if (m_NumLayers < 2)
		throw exception("Error: insufficient number of layers");
}

dVector NeuralNet::FeedForward(dVector& input)
{
	/* In the feed forward method, the input for each neuron
	is the output of the last layer, i.e. all the neurons are connected
	between each two adjacent layers
	*/
	dVector output;
	input.insert(input.begin(), 1);

	// Loop through each layer, except for the input layer
	for (int layer = 0; layer < m_NumLayers - 1; layer++)
	{
		size_t neuronsInLayer = m_Weights[layer].size();

		// The output for this layer
		output = dVector(neuronsInLayer);

		/* Calculate output of each neuron f(w•x), where:
			w is the weight vector,
			x is the input vector (output of previous layer)
			f is the activision function
			• denotes dot product
		*/
		input[0] = 1;
		for (size_t neuron = 1; neuron < neuronsInLayer; neuron++)
			output[neuron] = m_ActivisionFunction(
				DotProduct(input, m_Weights[layer][neuron]));

		// The output of this layer becomes the input of the next layer
		input = output;
	}

	return input;
}

NeuralNet NeuralNet::LoadNet(ifstream &in)
{
	size_t num_layers;
	in.read(reinterpret_cast<char *>(&num_layers), sizeof(size_t));

	// Read weights
	vector<vector<dVector>> weights(num_layers);
	for (int layer = 0; layer < num_layers; layer++)
	{
		size_t num_neurons;
		in.read(reinterpret_cast<char *>(&num_neurons), sizeof(size_t));
		vector<dVector> weightsForLayer(num_neurons);
		for (int neuron = 0; neuron < num_neurons; neuron++)
		{
			size_t num_weights;
			in.read(reinterpret_cast<char *>(&num_weights), sizeof(size_t));
			dVector weightsForNeuron(num_weights);
			for (int i = 0; i < num_weights; i++)
			{
				double weight;
				in.read(reinterpret_cast<char *>(&weight), sizeof(double));
				weightsForNeuron[i] = weight;
			}
			weightsForLayer[neuron] = weightsForNeuron;
		}
		weights[layer] = weightsForLayer;
	}

	return NeuralNet(weights);
}

void NeuralNet::SaveNet(ofstream &out)
{
	size_t numOfLayers = m_Weights.size();
	out.write(reinterpret_cast<char *>(&numOfLayers), sizeof(size_t));

	///* Save biases */
	//for (dVector &layer : m_Biases)
	//{
	//	int layerSize = (int)layer.size();
	//	out.write(reinterpret_cast<char *>(&layerSize), sizeof(int));
	//	for (double bias : layer)
	//		out.write(reinterpret_cast<char *>(&bias), sizeof(double));
	//}

	/* Save wieghts */
	for (vector<dVector> &layer : m_Weights)
	{
		size_t layerSize = (size_t)layer.size();
		out.write(reinterpret_cast<char *>(&layerSize), sizeof(size_t));
		for (dVector &neuron : layer)
		{
			size_t neuronSize = (size_t)neuron.size();
			out.write(reinterpret_cast<char *>(&neuronSize), sizeof(size_t));
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
		result += x[i] * y[i];

	return result;
}

void NeuralNet::FillWithRandoms(dVector &vec)
{
	size_t size = vec.size();
	for (size_t i = 0; i < size; i++)
	{
		// Generate a random
		vec[i] = GetRandomDouble(m_RandRangeMin, m_RandRangeMax);
	}
}

double NeuralNet::sigmoid(double x)
{
	return 1 / (1 + exp(-x));
}

double NeuralNet::sigmoid_sym(double x)
{
	return 2 / (1 + exp(-x)) - 1;
}