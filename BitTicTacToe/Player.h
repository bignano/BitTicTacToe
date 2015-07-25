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

	int GetPlayerTag() { return m_PlayerTag; }

protected:

	/* Default constructor, not meant to be called. */
	Player(int tag = PLAYER_X) : m_PlayerTag(tag) {};
	int m_PlayerTag;
};

