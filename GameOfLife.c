#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Generation {
	int **board;
	int rows;
	int cols;
} generation_t;

typedef struct Game {
	generation_t this;
	generation_t next;
	int generationNumber;
	int totalGenerations;
} game_t;

void startGame (game_t *game, char **argv);
void defineSizes (game_t *game, char **argv);
void memmoryAlloc (game_t *game);
void firstGeneration (game_t *game);
void playGame (game_t *game);

int main (int argc, char **argv)
{
	game_t game;

	if ( argc == 3 )
	{
		startGame (&game, argv);

		if ( !firstGeneration (&game) )
			return 2;

		playGame (&game);

		return 0;
	}

	printf ("ERROR: Parameters are incompatible.\n");

	return 1;
}

void startGame (game_t *game, char **argv)
{
	defineSizes (game, argv);

	memmoryAlloc (game);

	printf ("How many generations do you want to simulate?\n");
	scanf ("%d", &game->totalGenerations);	
}

void defineSizes (game_t *game, char **argv)
{
	int rows, cols;

	rows = atoi (argv[1]);
	cols = atoi (argv[2]);

	game->this.rows = rows;
	game->this.cols = cols;

	game->next.rows = rows;
	game->next.rows = rows;
}

void memmoryAlloc (game_t *game)
{
	int i;

	game->this.board = (int**) calloc ( game->this.rows, sizeof (int*) );

	for ( i=0; i < game->this.rows; i++ )
		game->this.board[i] = (int *) calloc ( game->this.cols, sizeof (int) );


	game->next.board = (int**) calloc ( game->next.rows, sizeof (int*) );

	for ( i=0; i < game->next.rows; i++ )
		game->next.board[i] = (int *) calloc ( game->next.cols, sizeof (int) );
}	


void firstGeneration (game_t *game)
{
	int i, alive, x, y;

	printf ("\n");
	printf ("How many alive cells do you want in the first generation?\n");
	scanf ("%d", &alive);

	printf ("\n");
	printf ("Put the coordinates of the cells you want alive on the first generation:\n");

	for ( i=0; i < alive; i++ )
	{
		printf ("Cell [%d]: ", i+1);
		scanf ("%d %d", &x, &y);
		x--;
		y--;

		if ( testSize (game, x, y) )
			createLife (game);

		else
			printf ("Invalid coordinates.\n");
	}
}

void playGame (game_t *game)
{

}

