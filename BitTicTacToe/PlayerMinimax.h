#pragma once
#include "Player.h"
#include <vector>
#include <unordered_map>

struct MinimaxMove
{
	U16 index;
	int value;
};

class PlayerMinimax :
	public Player
{
public:

	// Default player
	PlayerMinimax(
		GameTag tag = GameTag::Player_X,
		int depth = 9,
		bool useChainScore = false,
		int maxNumberOfThreads = 4,
		bool verbose = true,
		bool powerSave = false
		) :
		Player(tag),
		m_SearchDepth(CapSearchDepth(depth)),
		m_bUseChainScore(useChainScore),
		m_NumberOfThreads(maxNumberOfThreads),
		m_Verbose(verbose),
		m_PowerSaver(powerSave),
		svCount(0)
	{};
	
	virtual ~PlayerMinimax() {}

	virtual U16 GetMove(Bitboard board);

private:

	/* Gets a board and moves and retures the best move. 
	Used by GetMove to calculate part of the tree, to suppot multithreading. */
	MinimaxMove MinimaxWorker(Bitboard board, std::vector<U16> nextMoves);

	/* Distributes the number of moves evenly for each thread */
	std::vector<std::vector<int>> DistributeWork(int moveCount, int numberOfThreads);

	/* Regular minimax functions */
	int Alphabeta(Bitboard board, int depth, int alpha, int beta);

	/* Limit the depth to 16 (and above 0) */
	int  CapSearchDepth(int depth) { 
		if (depth > 16) return depth;
		if (depth < 1) return 1; 
		return depth; }

	bool m_bUseChainScore;

	// The initial search depth
	int m_SearchDepth;

	// Number of threads for minimax search
	int m_NumberOfThreads;

	// Should print result
	bool m_Verbose;

	// Sleep to avoid CPU overheating
	bool m_PowerSaver;

protected:

	// Default without chainscore (no need to use chain score because NN's override evaluation)
	PlayerMinimax(GameTag tag = GameTag::Player_X, int depth = 9, int maxNumberOfThreads = 4) :
		Player(tag),
		m_SearchDepth(CapSearchDepth(depth)),
		m_bUseChainScore(false),
		m_NumberOfThreads(maxNumberOfThreads),
		svCount(0)
	{};

	/* Returns static value for a board */
	virtual int GetStaticValue(Bitboard &board);

	// Count of static evaluations performed in last move
	int svCount;

};

