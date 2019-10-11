#include <stdio.h>

typedef struct Generation {
	int **board;
	int rows;
	int cols;
} generation_t;

typedef struct Game {
	generation_t this;
	generation_t next;
} game_t;

void startGame (generation_t *game, int *argc, char ***argv)
void firstGeneration (generation_t *game)
void playGame (generation_t *game)

int main (int argc, char **argv)
{
	game_t game;

	startGame (&game, &argc, &argv);

	firstGeneration (&game)

	playGame (&game);

	return 0;
}

void startGame (generation_t *game, int *argc, char ***argv)
{

}

void firstGeneration (generation_t *game)
{

}

void playGame (generation_t *game)
{

}

