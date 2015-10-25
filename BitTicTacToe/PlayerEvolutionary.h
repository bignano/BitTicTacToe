#pragma once
#include "PlayerMinimax.h"
#include "NeuralNet.h"

class PlayerEvolutionary :
	public PlayerMinimax
{
public:

	// New player with random network
	PlayerEvolutionary(
		GameTag tag = GameTag::Player_O, 
		int depth = 9, 
		int maxNumberOfThreads = 4, 
		bool verbose = false, 
		bool powerSave = false
		) :
		PlayerMinimax(tag, depth, false, maxNumberOfThreads, verbose, powerSave),
		m_Net(NeuralNet(std::vector<size_t>({ 16, 32, 8, 1 }))),
		m_FitnessValue(0),
		m_GamesPlayed(0)
	{}

	// New player with given network
	PlayerEvolutionary(
		NeuralNet &net, 
		GameTag tag = GameTag::Player_X, 
		int depth = 9, int maxNumberOfThreads = 4, 
		bool verbose = false, 
		bool powerSave = true
		) :
		PlayerMinimax(tag, depth, false, maxNumberOfThreads, verbose, powerSave),
		m_Net(net),
		m_FitnessValue(0),
		m_GamesPlayed(0)
	{}

	~PlayerEvolutionary(){}

	 // m_Net

	NeuralNet &GetNetwork() { return m_Net; }

	// m_FitnessValue

	int GetFitnessValue  () const			{ return m_FitnessValue; }
	void AddFitnessValue (int fitnessValue) { m_FitnessValue += fitnessValue; m_GamesPlayed++; }

	// m_GamesPlayed

	int GetGamesPlayed() const { return m_GamesPlayed; }

	void LoadPlayer(std::ifstream &in);
	void SavePlayer(std::ofstream &out);

private:
	
	// NN feedforward output for a board as input
	virtual int GetStaticValue(Bitboard &board) final;

	NeuralNet m_Net;
	
	int m_FitnessValue;
	int m_GamesPlayed;
};

