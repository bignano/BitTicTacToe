#include "PlayerEvolutionary.h"
#include <exception>

int PlayerEvolutionary::GetStaticValue(Bitboard &board)
{
	svCount++;
	double value = m_Net.FeedForward(board.EncodeBoard())[0];
	return (int)(value * 100);
}
