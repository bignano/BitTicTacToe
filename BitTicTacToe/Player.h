#pragma once
#include "Bitboard.h"

class Player
{
public:

	virtual ~Player() {};

	/* 
	Inherited by children, not to be called directly. 
	Returns a move for a given board.
	*/
	virtual U16 GetMove(Bitboard board) { return 0; }

protected:

	/* Default constructor, not meant to be called. */
	Player(U8 tag = PLAYER_X) : playerTag(tag) {};
	U8 playerTag;
};

