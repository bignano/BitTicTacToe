#include "PlayerMinimaxLookup.h"
#include "GameManager.h"
#include <fstream>
#include <string>

using namespace std;

PlayerMinimaxLookup::PlayerMinimaxLookup(GameTag tag, int depth, int maxNumberOfThreads, const char *filename) :
	PlayerMinimax(tag, depth, maxNumberOfThreads), m_LookupFilename(filename)
{
	// Try open the file
	ifstream in(filename, ios::binary);

	// If failed, ask to create new file. 
	if (!in.is_open())
	{
		if (GameManager::DisplayPolarQuestion("Create new lookup file?"))
		{
			ofstream createFile(filename);
			if (!createFile.is_open())
				printf("Unable to create file.\n");
		} // createfile closed
	}
	else
	{
		string line;
		while (getline(in, line))
		{
			/* Interpret each line as std::pair<U32, U16> */
			U32 key;
			U16 move;
			size_t pos = line.find("->");
			string key_s = line.substr(0, pos);
			string move_s = line.substr(pos+2, line.size() - pos - 1);
			key	 = stoul(key_s);
			move = (U16)stoul(move_s);
			m_LookupMap.insert(pair<const U32, U16>(key, move));
		}
		in.close();
	}
}

U16 PlayerMinimaxLookup::GetMove(Bitboard board)
{
	U32 key = board.EncodeBoardU32(m_PlayerTag);
	
	// If the key doesn't exist, get move using minimax and save it
	// map.find() is better than map.count() because it's <= O(n), while count is == O(n)
	if (m_LookupMap.find(key) == m_LookupMap.end())		
	{
		// Each entry is saved in a seperate line: board$bestmove\n
		ofstream out(m_LookupFilename, ios::app);
		U16 move = PlayerMinimax::GetMove(board);
		out << key << "->" << move << endl;
		m_LookupMap.insert(pair<const U32, U16>(key, move));
		return move;
	}
	// Return the move found in lookup map
	else return m_LookupMap[key];
}