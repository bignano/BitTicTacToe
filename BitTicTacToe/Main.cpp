#include "GameManager.h"
#include "PlayerHuman.h"
#include "PlayerMinimax.h"
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
	/* */
	int preset = GameManager::DisplayNumberQuestion("Choose preset: \n1) Human vs Human\n2)    AI vs AI   \n3) Human vs AI \n4)    AI vs Human\n5)      quit\n");
	int minimaxDepth = 0;
	bool useChainScore;

	if (preset == 2 ||
		preset == 3 ||
		preset == 4)
	{
		minimaxDepth = GameManager::DisplayNumberQuestion("Enter minimax search depth: ");
		useChainScore = GameManager::DisplayPolarQuestion("Use smart evaluation?");
	}

	GameManager human = GameManager(new PlayerHuman(), new PlayerHuman(PLAYER_O));
	GameManager minimax = GameManager(new PlayerMinimax(PLAYER_X, minimaxDepth, useChainScore), new PlayerMinimax(PLAYER_O, minimaxDepth, useChainScore));
	GameManager hybrid1 = GameManager(new PlayerHuman(), new PlayerMinimax(PLAYER_O, minimaxDepth, useChainScore));
	GameManager hybrid2 = GameManager(new PlayerMinimax(PLAYER_X, minimaxDepth, useChainScore), new PlayerHuman(PLAYER_O));

	switch (preset)
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
		return;
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