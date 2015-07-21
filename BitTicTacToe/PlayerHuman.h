#pragma once
#include "Player.h"
class PlayerHuman :
	public Player
{
public:
	PlayerHuman(U8 tag = PLAYER_X) : Player(tag) {}
	~PlayerHuman() {}

	U16 GetMove(Bitboard board);
};

