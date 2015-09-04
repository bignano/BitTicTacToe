#pragma once
#include "Player.h"
#include <string>

const U16 ABORT_GAME = 911;

class GameManager
{
public:
	/* Create a new game with two given players. */
	GameManager(Player *p1, Player *p2) :
		Player1(p1),
		Player2(p2) {}

	~GameManager() {}

	/* Start the game */
	void PlayGame(bool askAgain=true, bool verbose=true);

	/* Display a question with a given message and return true or false. */
	static bool DisplayPolarQuestion(std::string message);

	/* Display a question with a given message and return a numeric value. */
	static int DisplayNumberQuestion(std::string message);

private:

	/* Default constructor */
	GameManager() {};

	/* The two players and a pointer to the current one. */
	Player *Player1;
	Player *Player2;
	Player *cPlayer = Player1;

	/* The current game board. */
	Bitboard board;

	/* Scores of players and draws. */
	int xScore;
	int oScore;
	int dScore;

};

