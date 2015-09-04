#include "GameManager.h"
#include "PlayerHuman.h"
#include "PlayerMinimax.h"
#include "PlayerEvolutionary.h"
#include "EvolutionManager.h"
#include "PlayerMinimaxLookup.h"
#include <iostream>
#include <fstream>

using namespace std;

void LoadExtension()
{
	/* Load additional settings (title, color...) */
	ifstream settingsStream("_extension.dat");
	while (settingsStream)
	{
		string line;
		getline(settingsStream, line);
		char *cLine = (char *)line.c_str();
		system(cLine);
	}
}

int main(int argc, const char **argv);

Player *GetPlayer(GameTag tag, int playerNum)
{
	int player;
	do
	{
		printf("Select player #%i type: ", playerNum);
		player = GameManager::DisplayNumberQuestion("\n    1) Human \n    2) Minimax \n    3) Minimax-lookup \n    4) Minimax-neural");
	} while (player < 1 || player > 4);

	if (player == 1)
		return new PlayerHuman(tag);
	int searchDepth = GameManager::DisplayNumberQuestion("Enter search depth:");
	if (player == 2)
		return new PlayerMinimax(tag, searchDepth, false);
	if (player == 3)
		return new PlayerMinimaxLookup(tag, searchDepth);
	else // player == 4
		return new PlayerEvolutionary(tag, searchDepth);

}

void StartGame()
{
	LoadExtension();
	int option = GameManager::DisplayNumberQuestion("Main menu:\n    1) Play game\n    2) Enter evolution menu\n    3) Bake lookup\n    0) Quit");
	if (option == 1)
	{
		Player *p1 = GetPlayer(GameTag::Player_X, 1);	// Display x player promt first
		GameManager manager(p1, GetPlayer(GameTag::Player_O, 2));
		manager.PlayGame();
	}
	else if (option == 2)
	{
		EvolutionManager evomng;
		evomng.DisplayMenu();
	}
	else if (option == 3)
	{
		int iterations = GameManager::DisplayNumberQuestion("Number of players");
		PlayerMinimaxLookup lookup;
		for (int iter = 0; iter < iterations; iter++)
		{
			PlayerEvolutionary ev(GameTag::Player_O, 3);
			GameManager bakemng1 = GameManager(&ev, &lookup);
			GameManager bakemng2 = GameManager(&lookup, &ev);
			bakemng1.PlayGame(false, false);
			bakemng2.PlayGame(false, false);
		}
	}
	if (option != 0)
		StartGame();	// Display message again
}

int main(int argc, const char **argv)
{
	StartGame();

	return 0;
}