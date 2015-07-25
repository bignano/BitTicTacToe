#include "GameManager.h"
#include "PlayerHuman.h"
#include "PlayerMinimax.h"
#include "PlayerEvolutionary.h"
#include <iostream>
#include <fstream>

using namespace std;

void LoadExtension()
{
	/* Load additional settings (title, color...) */
	ifstream settingsStream("extension.txt");
	while (settingsStream)
	{
		string line;
		getline(settingsStream, line);
		char *cLine = (char *)line.c_str();
		system(cLine);
	}
	settingsStream.close();
}

int main();
void StartGame()
{
	int  gameMode = GameManager::DisplayNumberQuestion(
		"Choose mode: \n1)  Human vs Human \n2)     AI vs AI    \n3)  Human vs AI    \
		\n4)     AI vs Human \n5) neural vs human\n6)       quit\n");

	int  minimaxDepth = 9;
	bool useChainScore = true;
	int  numberOfThreads = 4;

	if (1 < gameMode && gameMode < 6)
	{
		minimaxDepth = GameManager::DisplayNumberQuestion("Minimax search depth: ");
		if (gameMode != 5)
			useChainScore = GameManager::DisplayPolarQuestion("Use smart evaluation?");
		numberOfThreads = GameManager::DisplayNumberQuestion("Threads: ");
	}

	GameManager human = GameManager(
		new PlayerHuman(), 
		new PlayerHuman(PLAYER_O));

	GameManager minimax = GameManager(
		new PlayerMinimax(PLAYER_X, minimaxDepth, useChainScore, numberOfThreads), 
		new PlayerMinimax(PLAYER_O, minimaxDepth, useChainScore, numberOfThreads));

	GameManager hybrid1 = GameManager(
		new PlayerHuman(), 
		new PlayerMinimax(PLAYER_O, minimaxDepth, useChainScore, numberOfThreads));

	GameManager hybrid2 = GameManager(
		new PlayerMinimax(PLAYER_X, minimaxDepth, useChainScore, numberOfThreads),
		new PlayerHuman(PLAYER_O));

	GameManager neural = GameManager(
		new PlayerEvolutionary(PLAYER_X, minimaxDepth, numberOfThreads),
		new PlayerHuman(PLAYER_O));//	new PlayerEvolutionary(PLAYER_O, minimaxDepth, numberOfThreads));

	switch (gameMode)
	{
	case 1:
		human.PlayGame();
		break;
	case 2:
		minimax.PlayGame();
		break;
	case 3:
		hybrid1.PlayGame();
		break;
	case 4:
		hybrid2.PlayGame();
	case 5:
		neural.PlayGame();
		break;
	case 6:
		return;		// Exit app
	default:
		break;
	}
	main();
}

int main()
{
	LoadExtension();
	StartGame();

	return 0;
}