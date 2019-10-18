#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define ALIVE 1
#define DEAD 0
#define clear() printf("\033[H\033[J")

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
	int cycleTime;
} game_t;

void startGame            (game_t *game, char **argv);
void defineSizes          (game_t *game, char **argv);
void memoryAlloc          (game_t *game);
void firstGeneration      (game_t *game);
void addCell              (game_t *game, int* i);
void removeCell           (game_t *game, int* i);
int  testSize             (game_t *game, int x, int y);
void playGame             (game_t *game);
void createNextGeneration (game_t *game);
void nextCellStatus       (game_t *game, int x, int y);
int  checkNeighbors       (game_t *game, int x, int y);
void createLife           (game_t *game, int x, int y);
void keepStatus           (game_t *game, int x, int y);
void killCell             (game_t *game, int x, int y);
void incrementGeneration  (game_t *game);
void printGeneration      (game_t *game);

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

/* Allocate the data structure and setup the game */
void startGame (game_t *game, char **argv)
{
	defineSizes (game, argv);

	memoryAlloc (game);

	game->generationNumber = 1;

	printf ("How many generations do you want to simulate?\n");
	scanf ("%d", &game->totalGenerations);	

	float time;

	printf ("How many seconds do you want each generation to last?\n");
	scanf ("%f", &time);
	time *= 1000000;
	game->cycleTime = time;
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

/* Setup the first generation with the user inputs */
void firstGeneration (game_t *game)
{
	int i=1;
	char input[] = {'i'};

	while ( !(input[0] == 's' && strlen(input) != 1) )
	{
		clear ();
		printGeneration (game);
		
		printf ("\nDigit [a] to add a live cell, [k] to kill a live cell and [s] to start the game.\n");
		scanf ("%c", input);

		if (input[0] == 'a' && strlen(input) == 1)
			addCell (game, &i);

		else if (input[0] == 'k' && strlen(input) == 1)
			removeCell (game, &i);

		else if (input[0] != 's' && strlen(input) != 1)
		{
			printf ("\nInvalid input, try again.");
			sleep (2);
		}
	}
}

void addCell (game_t *game, int* i)
{
	int x, y;

	printf ("\nCell [%d] coordinates: ", *i);
	scanf ("%d %d", &x, &y);

	if ( testSize (game, x-1, y-1) )
	{
		if (game->this.generation[x-1][y-1] == DEAD)
		{
			game->this.generation[x-1][y-1] = ALIVE;
			(*i)++;
		}
	
		else
		{
			printf ("\nThis cell is already live.\n");
			sleep (2);
		}
	}

	else
	{
		printf ("\nInvalid coordinates.\n");
		sleep (2);
	}
}

void removeCell (game_t *game, int* i)
{
	int x, y;

	printf ("\nCell coordinates: ");
	scanf ("%d %d", &x, &y);

	if ( testSize (game, x-1, y-1) )
	{
		if (game->this.generation[x-1][y-1] == ALIVE)
		{
			game->this.generation[x-1][y-1] = DEAD;
			(*i)--;
		}
	
		else
		{
			printf ("\nThis cell is already dead.\n");
			sleep (2);
		}
	}

	else
	{
		printf ("\nInvalid coordinates.\n");
		sleep (2);
	}
}

/* Checks if a Cell is inside the allocated data structure. */
int testSize (game_t *game, int x, int y)
{
	if ( (x < 0) || (y < 0) || (x >= game->this.rows) || (y >= game->this.cols) )
		return 0;

	return 1;
}


void playGame (game_t *game)
{
	printGeneration (game);

	while (game->generationNumber < game->totalGenerations)
	{

		createNextGeneration (game);

		incrementGeneration (game);

		usleep (game->cycleTime);

		printGeneration (game);
	}

	sleep (3);

	clear();
}

void createNextGeneration (game_t *game)
{
	int i, j;

	for ( i=0; i < game->this.rows; i++ )
		for ( j=0; j < game->this.cols; j++ )
			nextCellStatus (game, i, j);					
}

/* Updates the status of a Cell in the next generation. */
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

	/* Two fors that runs through all the neighbors of a cell. */
	for ( i=-1; i <= 1; i++ )
		for ( j=-1; j <= 1; j++ )
			/* Checks if the coordinate is inside the matrix. */
			if ( !( (x+i < 0) || (y+j < 0) || (x+i >= game->this.rows) || (y+j >= game->this.cols) ) )
				/* Make sure to not compare with the middle cell. */
				if ( !( (i==0) && (j==0) ) )
					/* Increment the counter if the cell is alive */
					if ( game->this.generation[x+i][y+j] == ALIVE )
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

/* Copy the board of the next generation to this generation and increment generationNumber */
void incrementGeneration (game_t *game)
{
	int i, j;

	for ( i=0; i < game->next.rows; i++ )
			for ( j=0; j < game->next.cols; j++ )
				game->this.generation[i][j] = game->next.generation[i][j];

	game->generationNumber++;
}

void printGeneration (game_t *game)
{
	int i, j;

	clear();

	printf ("Generation: %d\n\n", game->generationNumber);

	printf ("  ");
	for ( j=1; j <= game->this.cols; j++)
		if (j % 5 == 0 || j == 1)
			printf ("%2d", j);
		else
			printf ("  ");
	printf ("\n");

	for ( i=1; i <= game->this.rows; i++ )
	{
		if (i % 5 == 0 || i == 1)
			printf ("%2d", i);
		else
			printf ("  ");

		for ( j=1; j <= game->this.cols; j++ )
		{
			
			if (game->this.generation[i-1][j-1] == ALIVE)
				printf (" +");
			else if (game->this.generation[i-1][j-1] == DEAD)
				printf ("  ");
			else
				printf (" !");
		}

		if (i % 5 == 0 || i == 1)
			printf ("%2d", i);
		else
			printf ("  ");

		printf ("\n");
	}

	printf ("  ");
	for ( j=1; j <= game->this.cols; j++)
		if (j % 5 == 0 || j == 1)
			printf ("%2d", j);
		else
			printf ("  ");
	printf ("\n");
}
