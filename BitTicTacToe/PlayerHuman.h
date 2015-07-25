#pragma once
#include "Player.h"
class PlayerHuman :
	public Player
{
public:
	PlayerHuman(int tag = PLAYER_X) : Player(tag) {}
	~PlayerHuman() {}

	U16 GetMove(Bitboard board);
};

