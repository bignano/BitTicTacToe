#pragma once
#include "PlayerMinimax.h"
#include <unordered_map>

class PlayerMinimaxLookup :
	public PlayerMinimax
{
public:
	/* Uses a lookup map that 'remembers' the best move for each board.
	Otherwise, it uses regular seach and saves the board in memory.*/
	PlayerMinimaxLookup(GameTag tag = GameTag::Player_X, int depth = 9, int maxNumberOfThreads = 4, const char *filename = "_lookup.dat");

	~PlayerMinimaxLookup() {}

	virtual U16 GetMove(Bitboard board) final;

private:

	const char *m_LookupFilename;
	std::unordered_map<U32, U16> m_LookupMap;
};