#pragma once
#include <vector>
#include <math.h>

typedef std::vector<double> dVector;

const double RANDOM_BOUND = 0.5;

class NeuralNet
{
public:
	~NeuralNet() {}

	/* Construct net using random weights */
	NeuralNet(
		std::vector<std::size_t> layerSizes,
			double(*actFn)(double) = sigmoid,
			double rndMin = -RANDOM_BOUND,
			double rndMax = RANDOM_BOUND);

	/* Construct net using existing weights and biases */
	NeuralNet(
		std::vector<std::vector<dVector>> &weights,
		double(*actFn)(double) = sigmoid,
		double rndMin = -RANDOM_BOUND,
		double rndMax = RANDOM_BOUND);

	/* Get the output of the network for some input */
	dVector FeedForward(dVector &input);

	//std::vector<dVector> GetBiases()			   const { return m_Biases; }
	std::vector<std::vector<dVector>> GetWeights() const { return m_Weights; }

	static NeuralNet LoadNet(std::ifstream &in);
	void SaveNet(std::ofstream &out);

	/* Get a random real number in range */
	static double GetRandomDouble(double min, double max);

	/* Get a random integer in range */
	static int GetRandomInt(int min, int max);

	static double sigmoid(double x);
	static double sigmoid_sym(double x);

private:
	NeuralNet() {}

	std::size_t m_NumLayers;

	std::vector<std::vector<dVector>> m_Weights;
	
	// Pointer to the activision function
	double(*m_ActivisionFunction)(double);

	/* Calculate the dot product of two vectors */	
	static double DotProduct(const dVector& x, const dVector& y);

	/* Range for random number generation */
	double m_RandRangeMin;
	double m_RandRangeMax;

	/* Fill a vector of doubles with random numbers within a range*/
	void FillWithRandoms(dVector& vec);
};

