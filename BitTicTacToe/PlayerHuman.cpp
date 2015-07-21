#include "PlayerHuman.h"
#include "GameManager.h"
#include <math.h>

U16 PlayerHuman::GetMove(Bitboard board)
{
	U16 move;
	do
	{
		/* Make sure the index is valid */
		int answer;
		do
		{
			answer = GameManager::DisplayNumberQuestion("Enter move index:");
		} while (answer > 44 || answer < 11);

		/* Convert from human index to bitboard index. */

		answer -= 11;		// We count indecies from 0, not 1

		/* Convert the move */
		move = (U16)pow(
			2,										// Indicies are powers of two, the power is the number of the sqaure
			15 -									// Index mapping and bit mapping are inverted (see Bitboard.h). i ==> 15-i
			(answer / 10 + 4 * (answer % 10))		// Convert index: (x,y) ==> (x + 4*y)
			);

	} while (!board.CheckMove((move)));				// Make sure the move is legal 

	return move;
}