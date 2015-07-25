#pragma once
#include "PlayerMinimax.h"
#include "NeuralNet.h"

class PlayerEvolutionary :
	public PlayerMinimax
{
public:
	PlayerEvolutionary(int tag = PLAYER_X, int depth = 9, int maxNumberOfThreads = 4) :
		PlayerMinimax(tag, depth, maxNumberOfThreads),
		m_Net(arch)
	{}
	~PlayerEvolutionary(){}

private:
	
	// NN feedforward output for a board as input
	virtual int GetStaticValue(Bitboard &board) final;

	// The architechture of the network
	const std::vector<int> arch = std::vector<int>({ 16, 8, 1 });

	NeuralNet m_Net;

	
};

