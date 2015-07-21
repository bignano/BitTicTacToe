#pragma once
#include "Player.h"
class PlayerMinimax :
	public Player
{
public:

	// Default player
	PlayerMinimax(U8 tag = PLAYER_X, int depth = 9, bool useChainScore = true) :
		Player(tag), 
		searchDepth(CapSearchDepth(depth)),
		 bUseChainScore(useChainScore) {};

	~PlayerMinimax() {}

	U16 GetMove(Bitboard board);

private:

	//I8 Minimax(Bitboard board, I8 depth, I8 alpha, I8 beta);

	/* Minimax functions */
	I8 Mini(Bitboard board, I8 depth, I8 alpha, I8 beta);
	I8 Max(Bitboard board, I8 depth, I8 alpha, I8 beta);

	/* The initial search depth*/
	int searchDepth;
	/* Limit the depth to 16 (and above 0) */
	int  CapSearchDepth(int depth) { 
		if (depth > 16) return 16;
		if (depth < 1) return 1; 
		return depth; }

	bool bUseChainScore;

protected:
	I8 GetStaticValue(Bitboard board);

};

