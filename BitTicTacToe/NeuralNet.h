#pragma once
#include <vector>
#include <math.h>

typedef std::vector<double> dVector;

class NeuralNet
{
public:
	~NeuralNet() {}

	/* Construct net using random weights and biases */
	NeuralNet(
		std::vector<int> layerSizes,
			double(*actFn)(double) = tanh,
			double rndMin = -1.0,
			double rndMax = 1.0);

	/* Construct net using existing weights and biases */
	NeuralNet(
		std::vector<dVector> biases, 
		std::vector<std::vector<dVector>> weights, 
		double(*actFn)(double) = tanh, 
		double rndMin = -1.0, 
		double rndMax = 1.0);

	/* Get the output of the network for some input */
	dVector FeedForward(dVector &input);

private:
	NeuralNet() {}

	int m_NumLayers;

	std::vector<dVector> m_Biases;
	std::vector<std::vector<dVector>> m_Weights;
	
	// Pointer to the activision function
	double(*m_ActivisionFunction)(double);

	/* Calculate the dot product of two vectors */	
	double DotProduct(const dVector& x, const dVector& y);

	double m_RandRangeMin;
	double m_RandRangeMax;

	/* Fill a vector of doubles with random numbers within a range*/
	void FillWithRandoms(dVector& vec);

};

