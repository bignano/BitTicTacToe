#pragma once
#include "PlayerEvolutionary.h"

class EvolutionManager	// aka Nature
{
public:
	EvolutionManager(
		int    populationSize   = 16,
		int    gamesPerPlayer	= 4,
		int    searchDepth		= 4,
		int    threadsPerPlayer = 4,
		double learningRate	    = 1.0,
		bool   usePoweSave		= false
		) :
		m_PopulationSize	(populationSize),
		m_GamesPerPlayer	(gamesPerPlayer),
		m_SearchDepth		(searchDepth),
		m_ThreadsPerPlayer	(threadsPerPlayer),
		m_LearningRate		(learningRate),
		m_UsePowerSave		(usePoweSave)
	{}

	~EvolutionManager() {}

	/* Access evolution manager*/
	void DisplayMenu();

private:

	/* Parameters */
	const int  	 m_PopulationSize;
	const int	 m_GamesPerPlayer;
	const int	 m_SearchDepth;
	const int	 m_ThreadsPerPlayer;
	const double m_LearningRate;
	const bool	 m_UsePowerSave;

	int m_GenerationsDone;

	// Init or LoadPopulation were called before
	bool m_CanRun;

	/* The players consisting the current population*/
	std::vector<PlayerEvolutionary> m_Population;

	/* Initialize the evolution process with randomly-generated players */
	void Init();

	/* Run the evolution process a number of generations */
	void RunEvolution(int generations);

	/* Play against the best player of the population */
	void PlayAgainstBest();

	/* Vary the weights and biases of every player*/
	void Mutate();

	/* Helper function for Mutate() */
	void RanodmizeVector(std::vector<dVector> &vec);

	/* */
	void Play();

	GameTag RunGameLoop(PlayerMinimax *player, PlayerMinimax *opponent);

	/* Select the best strategies */
	void SelectSurvivors();


	void LoadPopulation(const char *filename="_population.dat");
	void SavePopulation(const char *filename="_population.dat");

	void Backup();

	/* Return the highest scored player from current population */
	PlayerEvolutionary *GetBestPlayer();
};

