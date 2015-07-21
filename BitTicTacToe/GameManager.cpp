#include "GameManager.h"
#include <stdio.h>
#include <iostream>
#include <time.h>

using namespace std;

void GameManager::PlayGame()
{
	/* Start with an empty board */
	board = Bitboard();
	board.Print();

	/* Statistics of the game: longest and average time of 'thinking' (for both players). */
	int stats_average = 0;
	int stats_highest = 0;
	int turns = 0;

	/* 
	Game loop:
	Ask the current player for a move, update the board and switch players.
	Terminates when the game ends with a winner or draw.
	*/
	while (board.GetWinner() == RESULT_NONE)
	{
		/* Ask the player for a move and record the time it took to return. */
		clock_t start = clock();
		U16 nextMove = cPlayer->GetMove(board);
		clock_t end = clock();
		int clocks = end - start;

		/* Collect stats. */
		if (clocks > stats_highest)
			stats_highest = clocks;
		stats_average += clocks; turns++;
		printf("Move returned in %i ms\n", clocks);

		/* Update the board with the latest move, print it and switch players. */
		board = board.DoMove(nextMove);
		board.Print();
		cPlayer = (cPlayer == Player1) ? Player2 : Player1;
	} 

	/* Display the result (winner) of the last match and add score to winner. */
	switch (board.GetWinner())
	{
	case PLAYER_X:
		xScore++;
		printf("X wins!\n");
		break;
	case PLAYER_O:
		oScore++;
		printf("O wins!\n");
		break;
	case RESULT_DRAW:
		dScore++;
		printf("It's a draw!\n");
		break;
	default:
		break;
	}

	/* Print scores*/
	printf("Scores:  X = %i  |  O = %i  | Draw = %i\n", xScore, oScore, dScore);
	printf("Stats: longest time = %i ms |  average time = %i ms\n", stats_highest, stats_average / turns);
	/* Ask the player for another game*/
	if (DisplayPolarQuestion("Play Again?"))
		PlayGame();
}

bool GameManager::DisplayPolarQuestion(string message)
{
	/* User must choose yes or no. */
	bool result = false;
	bool validInput = true;
	do
	{
		cout << message << " (y/n)\n>  ";
		string input;
		getline(cin, input);	// preferred way of getting input
		printf("\n");
		switch (input[0])
		{
		case 'y':
		case 'Y':
			result = true; 
			validInput = true;
			break;
		case 'n':
		case 'N':
			result = false; 
			validInput = true;
			break;
		default:
			validInput = false;
			break;
		}
	} while (!validInput);

	return result;
}

int GameManager::DisplayNumberQuestion(string message)
{
	cout << message << "\n> ";
	string input;
	getline(cin, input);
	if (input == "")
		return DisplayNumberQuestion(message);
	return atoi(input.c_str());
}

GameManager::~GameManager()
{
	delete Player1;
	delete Player2;
}