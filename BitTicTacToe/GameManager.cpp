#include "GameManager.h"
#include <stdio.h>
#include <iostream>
#include <chrono>
#include <exception>

using namespace std;

void GameManager::PlayGame(bool askAgain, bool verbose)
{
	if (Player1->GetPlayerTag() == Player2->GetPlayerTag())
		throw exception("Players must have different tags.");

	/* Start with an empty board */
	board = Bitboard(cPlayer->GetPlayerTag());
	if (verbose)	board.Print();


	/* Statistics: longest and average time of 'thinking' (for both players). */
	std::chrono::duration<float, std::milli> stats_average(0);
	std::chrono::duration<float, std::milli> stats_highest(0);
	int turns = 0;

	/* 
	Game loop:
	Ask the current player for a move, update the board and switch players.
	Terminates when the game ends with a winner or draw.
	*/
	while (board.GetWinner() == GameTag::Result_None)
	{
		/* Ask the player for a move and record the time it took to return. */
		auto start = std::chrono::high_resolution_clock::now();
		U16 nextMove = cPlayer->GetMove(board);
		auto end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float, std::milli> elapsed = end - start;

		if (nextMove == ABORT_GAME)
		{
			printf("Game aborted.\n");
			if (DisplayPolarQuestion("Play Again?"))
				PlayGame();
			else return;
		}

		/* Collect stats. */
		if (elapsed.count() > stats_highest.count())
			stats_highest = elapsed;
		stats_average += elapsed; turns++;
		if (verbose) printf("Move returned in %f ms\n", elapsed.count());

		/* Update the board with the latest move, print it and switch players. */
		board = board.DoMove(nextMove, cPlayer->GetPlayerTag());
		if (verbose) board.Print();
		cPlayer = (cPlayer == Player1) ? Player2 : Player1;
	} 

	/* Display the result (winner) of the last match and add score to winner. */
	switch (board.GetWinner())
	{
	case GameTag::Player_X:
		xScore++;
		if (verbose) printf("X wins!\n");
		break;
	case GameTag::Player_O:
		oScore++;
		if (verbose) printf("O wins!\n");
		break;
	case GameTag::Result_Draw:
		dScore++;
		if (verbose) printf("It's a draw!\n");
		break;
	default:
		break;
	}

	/* Print scores*/
	if (verbose) printf("Scores:  X = %i  |  O = %i  | Draw = %i\n", xScore, oScore, dScore);
	/* Print stats*/
	if (verbose) printf("Time: total %fms | longest %fms | average %fms\n", stats_average.count(), stats_highest.count(), stats_average.count() / turns);
	/* Ask the player for another game*/
	if (askAgain && DisplayPolarQuestion("Play Again?"))
		PlayGame();
}

bool GameManager::DisplayPolarQuestion(string message)
{
	/* User must choose yes or no. */
	bool result = false;

	cout << message << " (y/n)\n>  ";	// Display message
	string input;
	getline(cin, input);				// preferred way of getting input
	cout << endl;

	switch (input[0])		// Check if the answer is valid
	{
	case 'y':
	case 'Y':
		result = true; 
		break;
	case 'n':
	case 'N':
		result = false; 
		break;
	default:
		return DisplayPolarQuestion(message);	// If the input is invalid, display message again
	}

	return result;
}

int GameManager::DisplayNumberQuestion(string message)
{
	cout << message << "\n> ";						// Display message
	string input;
	getline(cin, input);							// Get user input, safe way
	if (input == "")
		return DisplayNumberQuestion(message);		// If the input is invalid, display message again
	return atoi(input.c_str());						// Convert c-style string to int
}