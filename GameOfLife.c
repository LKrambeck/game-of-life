#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct Generation {
	int **generation;
	int rows;
	int cols;
} generation_t;

typedef struct Game {
	generation_t this;
	generation_t next;
	int generationNumber;
	int totalGenerations;
} game_t;

#define ALIVE 1
#define DEAD 0
#define clear() printf("\033[H\033[J")

void startGame            (game_t *game, char **argv);
void defineSizes          (game_t *game, char **argv);
void memoryAlloc          (game_t *game);
void firstGeneration      (game_t *game);
int  testSize             (game_t *game, int x, int y);
void playGame             (game_t *game);
void printGeneration      (game_t *game);
void nextCellStatus       (game_t *game, int x, int y);
int  checkNeighbors       (game_t *game, int x, int y);
void createLife           (game_t *game, int x, int y);
void keepStatus           (game_t *game, int x, int y);
void killCell             (game_t *game, int x, int y);
void incrementGeneration  (game_t *game);

int main (int argc, char **argv)
{
	game_t game;

	if ( argc == 3 )
	{
		startGame (&game, argv);

		firstGeneration (&game);

		playGame (&game);

		return 0;
	}

	printf ("ERROR: Parameters are incompatible.\n");

	return 1;
}

void startGame (game_t *game, char **argv)
{
	defineSizes (game, argv);

	memoryAlloc (game);

	game->generationNumber = 1;

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
	game->next.cols = cols;
}

/* Allocate the generations matrices with all cells DEAD (0). */
void memoryAlloc (game_t *game)
{
	int i;

	game->this.generation = (int**) calloc ( game->this.rows, sizeof (int*) );

	for ( i=0; i < game->this.rows; i++ )
		game->this.generation[i] = (int *) calloc ( game->this.cols, sizeof (int) );


	game->next.generation = (int**) calloc ( game->next.rows, sizeof (int*) );

	for ( i=0; i < game->next.rows; i++ )
		game->next.generation[i] = (int *) calloc ( game->next.cols, sizeof (int) );
}	


void firstGeneration (game_t *game)
{
	int alive, x, y, i=0;

	printf ("\n");
	printf ("How many alive cells do you want in the first generation?\n");
	scanf ("%d", &alive);

	printf ("\n");
	printf ("Put the coordinates of the cells you want alive on the first generation:\n");

	while ( i < alive )
	{
		printf ("Cell [%d]: ", i);
		scanf ("%d %d", &x, &y);

		if ( testSize (game, x, y) )
		{
			game->this.generation[x][y] = ALIVE;
			i++;
		}

		else
			printf ("Invalid coordinates.\n");
	}
}

int testSize (game_t *game, int x, int y)
{
	if ( (x < 0) || (y < 0) || (x >= game->this.rows) || (y >= game->this.cols) )
		return 0;

	return 1;
}


void playGame (game_t *game)
{
	int i, j;

	printGeneration (game);

	while (game->generationNumber < game->totalGenerations)
	{
		for ( i=0; i < game->this.rows; i++ )
			for ( j=0; j < game->this.cols; j++ )
				nextCellStatus (game, i, j);					

		incrementGeneration (game);

		printGeneration (game);
	}

	sleep (5);

	clear();
}

void printGeneration (game_t *game)
{
	int i, j;

	clear();

	printf ("Generation: %d\n\n", game->generationNumber);

	for ( i=0; i < game->this.rows; i++ )
	{
		for ( j=0; j < game->this.cols; j++ )
			printf ("%d ", game->this.generation[i][j]);
		printf ("\n");
	}

	sleep (2);

}

void nextCellStatus (game_t *game, int x, int y)
{
	switch ( checkNeighbors (game, x, y) )
	{
		case 3:
			createLife (game, x, y);
			break;

		case 2:
			keepStatus (game, x, y);
			break;

		default:
			killCell (game, x, y);
	}
}

/* ------------- Here is the jump of the cat ---------------- */
int checkNeighbors (game_t *game, int x, int y)
{
	int i, j, aliveCellCount = 0;

	for ( i=-1; i <= 1; i++ )
		for ( j=-1; j <= 1; j++ )
			/* Checks if the coordinate is inside the matrix. */
			if ( !( (x+i < 0) || (y+j < 0) || (x+i >= game->this.rows) || (y+j >= game->this.cols) ) )
				/* Make sure to not compare with the middle cell. */
				if ( !( (i==0) && (j==0) ) )
					if ( game->this.generation[x+i][y+j] == 1 )
						aliveCellCount++;

	return aliveCellCount;
}

void createLife (game_t *game, int x, int y)
{
	game->next.generation[x][y] = ALIVE;
}

void keepStatus (game_t *game, int x, int y)
{
	game->next.generation[x][y] = game->this.generation[x][y];
}

void killCell (game_t *game, int x, int y)
{
	game->next.generation[x][y] = DEAD;
}

void incrementGeneration (game_t *game)
{
	int i, j;

	for ( i=0; i < game->next.rows; i++ )
			for ( j=0; j < game->next.cols; j++ )
				game->this.generation[i][j] = game->next.generation[i][j];

	game->generationNumber++;
}

