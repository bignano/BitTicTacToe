#pragma once
#include "Player.h"
#include <vector>

struct MinimaxMove
{
	U16 index;
	I8 value;
};

class PlayerMinimax :
	public Player
{
public:

	// Default player
	PlayerMinimax(U8 tag = PLAYER_X, int depth = 9, bool useChainScore = true, int maxNumberOfThreads=3) :
		Player(tag), 
		m_SearchDepth(CapSearchDepth(depth)),
		 m_bUseChainScore(useChainScore),
		 m_NumberOfThreads(maxNumberOfThreads) {};
	
	~PlayerMinimax() {}

	U16 GetMove(Bitboard board);

private:

	/* Gets a board and moves and retures the best move. 
	Used by GetMove to calculate part of the tree, to suppot multithreading. */
	MinimaxMove MinimaxWorker(Bitboard board, std::vector<U16> nextMoves);

	/* Distributes the number of moves evenly for each thread */
	std::vector<std::vector<int>> UniformDistribution(int moveCount, int numberOfThreads);

	/* Regular minimax functions */
	I8 Mini(Bitboard board, I8 depth, I8 alpha, I8 beta);
	I8 Max(Bitboard board, I8 depth, I8 alpha, I8 beta);

	/* The initial search depth*/
	int m_SearchDepth;
	/* Limit the depth to 16 (and above 0) */
	int  CapSearchDepth(int depth) { 
		if (depth > 16) return depth;
		if (depth < 1) return 1; 
		return depth; }

	bool m_bUseChainScore;
	int m_NumberOfThreads;

protected:
	I8 GetStaticValue(Bitboard board);

};

