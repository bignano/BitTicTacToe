#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <exception>
#include <ctime>

#include "EvolutionManager.h"
#include "GameManager.h"
#include "PlayerHuman.h"

#define REPEAT_N_TIMES(n) for( int iii = 0; iii < n; iii++)

using namespace std;

void EvolutionManager::DisplayMenu()
{
	int mode = GameManager::DisplayNumberQuestion("\nEvolution menu:\n    1) Init\n    2) Load\n    3) Create backup		\
		\n    4) Load backup \n    5) Play against best	\n    6) Run \n    0) Quit");

	switch (mode)
	{
	case 1:
		//Backup();
		Init();
		printf("Initialized.\n");
		break;
	case 2:
			LoadPopulation();
		break;
	case 3:
		if (!m_CanRun)
			printf("Can't create backup without loading or initializing.\n");
		else
			Backup();
		break;
	case 4:
	{
		cout << "Enter filename:\n";
		string fname;
		getline(cin, fname);
		LoadPopulation(fname.c_str());
		break;
	}
	case 5:
		if (!m_CanRun)
			printf("Can't run without loading or initializing.\n");
		else
			PlayAgainstBest();
		break;
	case 6:
		if (!m_CanRun)
			printf("Can't run without loading or initializing.\n");
		else
			RunEvolution(GameManager::DisplayNumberQuestion("Generations:"));
		break;
	case 0:
		return;
	default:
		break;;
	}
	DisplayMenu();
}

void EvolutionManager::Init()
{
	m_Population.resize(0);
	// Initialize population with new players with random ANN's
	REPEAT_N_TIMES(m_PopulationSize)
	{
		m_Population.push_back(PlayerEvolutionary(GameTag::Player_X, m_SearchDepth, m_ThreadsPerPlayer, false, m_UsePowerSave));
	}

	SavePopulation();
	m_CanRun = true;
}

void EvolutionManager::RunEvolution(int generations)
{
	printf("Running evolution: 0/%i (0%%) done", generations);
	for (int generation = 0; generation < generations; generation++)
	{
		Mutate();
		Play();
		SelectSurvivors();

		float percent = (generation + 1) * 100 / (float)generations;
		printf("\rRunning evolution: %i/%i (%i.%i%%) done", generation + 1, generations, (int)percent, (int)(percent * 10) % 10);	// Print progress
	}
	m_GenerationsDone += generations;
	printf("\nTotal generations: %i\n", m_GenerationsDone);
	SavePopulation();
}

void EvolutionManager::PlayAgainstBest()
{
	PlayerEvolutionary *bestPlayer = GetBestPlayer();

	printf("\nPlayer score: %i after %i games\n", bestPlayer->GetFitnessValue(), bestPlayer->GetGamesPlayed());

	// Use the nn of best player, but use greater search depth and more suitable parameters for human-computer games
	PlayerEvolutionary betterBestPlayer = PlayerEvolutionary(bestPlayer->GetNetwork(), GameTag::Player_X, m_SearchDepth+2, 4, true, false);

	GameManager gmng = GameManager(&betterBestPlayer, new PlayerHuman(GameTag::Player_O));
	gmng.PlayGame();
}

void EvolutionManager::Play()
{
	int stats[] = { 0,0,0 };
	/* Each player plays a number of games with random opponents */
	for (PlayerEvolutionary &player : m_Population)	
	{
		
		for (int game = 0; game < m_GamesPerPlayer; game++)
		{
			// Random player
			PlayerEvolutionary *otherPlayer = &m_Population[NeuralNet::GetRandomInt(0, (int)m_Population.size() - 1)];	
			
			// other player, player tag O
			PlayerEvolutionary opponent = PlayerEvolutionary(otherPlayer->GetNetwork(), 
				GameTag::Player_O, m_SearchDepth, m_ThreadsPerPlayer, false, false);

			//static PlayerMinimax opponent = PlayerMinimax(GameTag::Player_O, m_SearchDepth, true, m_ThreadsPerPlayer, false);

			GameTag winner = RunGameLoop(&player, &opponent);
			if (winner == GameTag::Player_X)
			{
				player.AddFitnessValue(1);
				stats[0]++;
				otherPlayer->AddFitnessValue(-1);
			}
			else if (winner == GameTag::Player_O)
			{
				player.AddFitnessValue(-1);
				stats[1]++;
				otherPlayer->AddFitnessValue(1);
			}
			else
			{
				player.AddFitnessValue(0);
				stats[2]++;
			}
		}
	}
	//printf("\nx %i o %i d %i\n", stats[0], stats[1], stats[2]);
}

GameTag EvolutionManager::RunGameLoop(PlayerMinimax *player, PlayerMinimax *opponent)
{
	Bitboard board = Bitboard();
	PlayerMinimax *currPlayer = player;
	do
	{
		board = board.DoMove(currPlayer->GetMove(board), currPlayer->GetPlayerTag());
		currPlayer = (currPlayer == player) ? opponent : player;
	} while (board.GetWinner() == GameTag::Result_None);

	return board.GetWinner();
}

void EvolutionManager::Mutate()
{
	for (int parent = 0; parent < m_PopulationSize; parent++)
	{
		/* Get new, slightly varied weights and biases for the new player */

		vector<dVector> newBiases = m_Population[parent].GetNetwork().GetBiases();
		RanodmizeVector(newBiases);

		vector<vector<dVector>> newWeights = m_Population[parent].GetNetwork().GetWeights();
		for (int layer = 0; layer < newWeights.size(); layer++)
		{
			RanodmizeVector(newWeights[layer]);
		}

		// Add the player to the population
		m_Population.push_back(PlayerEvolutionary(NeuralNet(newBiases, newWeights), 
			GameTag::Player_X, m_SearchDepth, m_ThreadsPerPlayer, false, m_UsePowerSave));
	}
}

void EvolutionManager::RanodmizeVector(vector<dVector> &vec)
{
	for (int vIndex = 0; vIndex < vec.size(); vIndex++)
	{
		for (int dIndex = 0; dIndex < vec[vIndex].size(); dIndex++)
		{
			vec[vIndex][dIndex] += m_LearningRate * NeuralNet::GetRandomDouble(-0.1, 0.1);
		}
	}
}

void EvolutionManager::SelectSurvivors()
{
	// Sort population by fitness in descending order
	sort(m_Population.begin(), m_Population.end(),
		[](const PlayerEvolutionary &x, const PlayerEvolutionary &y) { return x.GetFitnessValue() > y.GetFitnessValue(); } );

	// Resize the population to fixed size
	m_Population.resize(m_PopulationSize);
}

void EvolutionManager::LoadPopulation(const char *filename)
{
	ifstream in(filename, ios::binary);
	
	if (!in.is_open())
	{
		printf("Couldn't load population.\n");
		return;
	}

	// Read generations done
	in.read(reinterpret_cast<char *>(&this->m_GenerationsDone), sizeof(int));

	// Read population

	m_Population.resize(0);
	for (int i = 0; i < m_PopulationSize; i++)
	{
		PlayerEvolutionary player = PlayerEvolutionary(GameTag::Player_X, m_SearchDepth, m_ThreadsPerPlayer, false, m_UsePowerSave);
		player.LoadPlayer(in);
		m_Population.push_back(player);
	}

	printf("Loaded. number of generations: %i\n", m_GenerationsDone);
	m_CanRun = true;
}

// Helpers for file padding
int pow2roundup(int x){x--;x|=x>>1;x|=x>>2;x|=x>>4;x|=x>>8;x|=x>>16;return x+1;}
class Zero{public:Zero(){static bool b=false;if(b)z=0x95;else z=0xB0;b=!b;}unsigned char z;};
class LotsofZeros{public:LotsofZeros(int sizebytes=0){zp=new Zero[sizebytes];}~LotsofZeros(){delete[]zp;}Zero*zp;};

void EvolutionManager::SavePopulation(const char *filename)
{
	ofstream out(filename, ios::binary);
	
	if (!out.is_open())
	{
		cout << "Unable to save population.\n";
		return;
	}
	
	out.write(reinterpret_cast<char *>(&m_GenerationsDone), sizeof(int));
	for (PlayerEvolutionary &player : m_Population)
		player.SavePlayer(out);

	out.close();

	// Add padding zeros for power of 2 filesize
	struct stat stat_buf;
	int rc = stat(filename, &stat_buf);
	int filesize = stat_buf.st_size;
	int complement = pow2roundup(filesize) - filesize;

	out.open(filename, ios::binary | ios::app);
	LotsofZeros zeros = LotsofZeros(complement);
	out.write(reinterpret_cast<char *>(zeros.zp), complement);
}

void EvolutionManager::Backup()
{
	string fname = "population_";
	fname.append(to_string(m_GenerationsDone));
	fname.append("_");
	fname.append(to_string(static_cast<long int>(time(0))));
	fname.append(".dat");

	SavePopulation(fname.c_str());
	cout << "Backup created.\n";
}

PlayerEvolutionary *EvolutionManager::GetBestPlayer()
{
	PlayerEvolutionary *best = &m_Population[0];
	
	for (PlayerEvolutionary &player : m_Population)
		if (player.GetFitnessValue() > best->GetFitnessValue())
			best = &player;
	
	return best;
}