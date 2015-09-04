#include "PlayerEvolutionary.h"
#include <exception>
#include <fstream>
#include <string>

int PlayerEvolutionary::GetStaticValue(Bitboard &board)
{
	svCount++;

	if (PlayerMinimax::GetStaticValue(board) != 0)
		return PlayerMinimax::GetStaticValue(board);

	double value = m_Net.FeedForward(board.EncodeBoardDVec(m_PlayerTag))[0];
	return (int)(value * 1000000000);	// value * 1B, for maximum precision
}

void PlayerEvolutionary::LoadPlayer(std::ifstream &in)
{
	in.read(reinterpret_cast<char *>(&this->m_FitnessValue), sizeof(int));
	in.read(reinterpret_cast<char *>(&this->m_GamesPlayed), sizeof(int));

	this->m_Net = NeuralNet::LoadNet(in);
}

void PlayerEvolutionary::SavePlayer(std::ofstream &out)
{
	out.write(reinterpret_cast<char *>(&m_FitnessValue), sizeof(int));
	out.write(reinterpret_cast<char *>(&m_GamesPlayed), sizeof(int));

	this->m_Net.SaveNet(out);
}