#pragma once
#include "Player.h"
class PlayerHuman :
	public Player
{
public:
	PlayerHuman(GameTag tag = GameTag::Player_X) : Player(tag) {}
	~PlayerHuman() {}

	virtual U16 GetMove(Bitboard board);

private:
	bool IsValidAnswer(int ans);

};

