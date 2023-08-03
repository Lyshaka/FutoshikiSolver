#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>

#define LESS	1
#define MORE	2

typedef struct CELL
{
	int	value;
	int	arrow_U;
	int	arrow_L;
}	CELL;

const	int	size = 9;
long	iteration = 0;
struct	timezone	tz;
struct	timeval		start;
struct	timeval		end;
long	hour = 0;
long	minute = 0;
long	second = 0;
time_t	totalStart;
time_t	totalEnd;


int		isValid(CELL puzzle[size][size], int row, int col, int num, int size);
int		getNextEmptyCell(CELL puzzle[size][size], int *row, int *col, int size);
int		solvePuzzle(CELL puzzle[size][size], int size);
char	*getPuzzle(char *puzzleFile);
void 	parsePuzzle(int size, CELL puzzle[size][size], char *puzzleString);
void	printPuzzle(CELL puzzle[size][size], int size);
void	printDebug(CELL puzzle[size][size], int size);

int		main(int argc, char const **argv)
{
	srand(time(0));
	time(&totalStart);

	CELL	test[size][size];

	char	*puzzle;

	puzzle = getPuzzle("9x9_2.txt");
	parsePuzzle(size, test, puzzle);
	free(puzzle);

	printf("Puzzle %dx%d :\n", size, size);
	printPuzzle(test, size);

	printf("\nSolving...\n\n");
	if (solvePuzzle(test, size))
	{
		//system("clear");
		printf("Puzzle résolu :\n");
		printPuzzle(test, size);
		printf("\n");
		//printDebug(test, size);
	}
	else
	{
		//system("clear");
		printf("Pas de solutions :(\n");
	}
	hour = difftime(totalEnd, totalStart) / 3600;
	minute = difftime(totalEnd, totalStart) / 60 - hour * 60;
	second = difftime(totalEnd, totalStart) - hour * 3600 - minute * 60;

	printf("Total time : %dh %dm %ds\n", hour, minute, second);
	printf("Cells checked : %ld ; %.3f millions\n", iteration, (float)iteration / 1000000);

	return (0);
}

int		isValid(CELL puzzle[size][size], int row, int col, int num, int size)
{
	int	i;

	i = 0;

	while (i < size)
	{
		if (puzzle[i][col].value == num || puzzle[row][i].value == num)
		{
			return (0);
		}
		i++;
	}

	//Check UP cell
	if (puzzle[row][col].arrow_U == LESS && num >= puzzle[row - 1][col].value && puzzle[row - 1][col].value > 0)
		return (0);
	if (puzzle[row][col].arrow_U == MORE && num <= puzzle[row - 1][col].value)
		return (0);
	//Check 1 and size
	if (puzzle[row][col].arrow_U == LESS && num == size)
		return (0);
	if (puzzle[row][col].arrow_U == MORE && num == 1)
		return (0);

	//Check DOWN cell
	if (puzzle[row + 1][col].arrow_U == LESS && num <= puzzle[row + 1][col].value)
		return (0);
	if (puzzle[row + 1][col].arrow_U == MORE && num >= puzzle[row + 1][col].value && puzzle[row + 1][col].value > 0)
		return (0);
	//Check 1 and size
	// if (row < size && puzzle[row + 1][col].arrow_U == LESS && num == 1)
	// 	return (0);
	if (row < size && puzzle[row + 1][col].arrow_U == MORE && num == size)
		return (0);

	//Check LEFT cell
	if (puzzle[row][col].arrow_L == LESS && num >= puzzle[row][col - 1].value && puzzle[row][col - 1].value > 0)
		return (0);
	if (puzzle[row][col].arrow_L == MORE && num <= puzzle[row][col - 1].value)
		return (0);
	//Check 1 and size
	if (puzzle[row][col].arrow_L == LESS && num == size)
		return (0);
	if (puzzle[row][col].arrow_L == MORE && num == 1)
		return (0);

	// //Check RIGHT cell
	if (puzzle[row][col + 1].arrow_L == LESS && num <= puzzle[row][col + 1].value)
		return (0);
	if (puzzle[row][col + 1].arrow_L == MORE && num >= puzzle[row][col + 1].value && puzzle[row][col + 1].value > 0)
		return (0);
	//Check 1 and size
	if (puzzle[row][col + 1].arrow_L == LESS && num == 1 && col < size)
		return (0);
	if (puzzle[row][col + 1].arrow_L == MORE && num == size && col < size)
		return (0);

	return (1);
}

int		getNextEmptyCell(CELL puzzle[size][size], int *row, int *col, int size)
{
	for (*row = 0; *row < size; (*row)++)
	{
		for (*col = 0; *col < size; (*col)++)
		{
			if (puzzle[*row][*col].value == 0)
				return (1);
		}
	}
	return (0);
}

int		solvePuzzle(CELL puzzle[size][size], int size)
{
	int	row;
	int	col;

	if (!(getNextEmptyCell(puzzle, &row, &col, size)))
		return (1); // Puzzle Solved

	for (int num = 1; num <= size; num++)
	{
		if (isValid(puzzle, row, col, num, size))
		{
			puzzle[row][col].value = num;
			gettimeofday(&end, &tz);
			// usleep(10000);
			// system("clear");
			// printf("Solving...\n");
			iteration++;
			if (iteration % 1000000 == 0)
				printf("Cells checked : %d millions\n", iteration / 1000000);
			time(&totalEnd);
			// printf("Cell [%d] solved; Iteration [%d]; Execution time : %ld µs; Total Time : %.0f s\n", (row * size + col + 1), iteration, (end.tv_sec - start.tv_sec) * 1000000L + (end.tv_usec - start.tv_usec), difftime(totalEnd, totalStart));
			gettimeofday(&start, &tz);
			if (solvePuzzle(puzzle, size))
				return (1);
			puzzle[row][col].value = 0;
		}
	}

	return (0);
}

char	*getPuzzle(char *puzzleFilename)
{
	FILE	*puzzleFile;
	char	*puzzle;
	int		size;


	if (!(puzzleFile = fopen(puzzleFilename, "r")))
		return (NULL);
	fseek(puzzleFile, 0, SEEK_END);
	size = ftell(puzzleFile);
	rewind(puzzleFile);

	if (!(puzzle = malloc(sizeof(char) * size + 1)))
		return (NULL);

	fread(puzzle, sizeof(char), size, puzzleFile);

	fclose(puzzleFile);

	return (puzzle);
}

void 	parsePuzzle(int size, CELL puzzle[size][size], char *puzzleString)
{

	int		row;
	int		col;
	int		i;
	int		j;
	char	**puzzleLines;

	row = 0;
	col = 0;
	i = 0;
	j = 0;

	if (!(puzzleLines = malloc(sizeof(char*) * ((size * 2) + 1))))
		return;

	while (j < (size * 2) + 1)
	{
		if (!(puzzleLines[j] = malloc(sizeof(char) * ((size * 4) + 2))))
			return;
		j++;
	}

	i = 0;
	j = 0;

	while (j < (size * 2 + 1))
	{
		while (puzzleString[i] != '\n' && puzzleString[i] != 0)
			i++;
		if (puzzleString[i] == '\n')
		{
			strncpy(puzzleLines[j], puzzleString + i - (size * 4) - 2, (size * 4) + 1);
			puzzleLines[j][(size * 4) + 2] = 0;
			//printf("%d (%d) : [%s]\n", j, strlen(puzzleLines[j]), puzzleLines[j]);
			j++;
		}
		if (puzzleString[i] == 0)
		{
			strncpy(puzzleLines[j], puzzleString + i - (size * 4) - 1, (size * 4) + 1);
			puzzleLines[j][(size * 4) + 2] = 0;
			//printf("%d (%d) : [%s]\n", j, strlen(puzzleLines[j]), puzzleLines[j]);
			j++;
		}
		i++;
	}

	while (row < size)
	{
		while (col < size)
		{

			//Get digit value
			if (puzzleLines[row * 2 + 1][col * 4 + 2] == ' ')
				puzzle[row][col].value = 0;
			else
				puzzle[row][col].value = puzzleLines[row * 2 + 1][col * 4 + 2] - '0';
			
			//Get up down arrows
			if (puzzleLines[row * 2][col * 4 + 2] == '^')
				puzzle[row][col].arrow_U = MORE;
			else if (puzzleLines[row * 2][col * 4 + 2] == 'v')
				puzzle[row][col].arrow_U = LESS;
			else
				puzzle[row][col].arrow_U = 0;
			
			//Get left right arrows
			if (puzzleLines[row * 2 + 1][col * 4] == '<')
				puzzle[row][col].arrow_L = MORE;
			else if (puzzleLines[row * 2 + 1][col * 4] == '>')
				puzzle[row][col].arrow_L = LESS;
			else
				puzzle[row][col].arrow_L = 0;
			col++;
		}
		col = 0;
		row++;
	}

	i = 0;

	while (i < (size * 2 + 1))
	{
		free(puzzleLines[i]);
		i++;
	}
	free(puzzleLines);
}

void	printPuzzle(CELL puzzle[size][size], int size)
{
	int	row;
	int	col;

	row = 0;
	col = 0;

	while (row < size)
	{
		while (col < size)
		{
			printf("+ ");
			if (puzzle[row][col].arrow_U == LESS)
				printf("v ");
			else if (puzzle[row][col].arrow_U == MORE)
				printf("^ ");
			else
				printf("- ");
			col++;
		}
		printf("+\n");
		col = 0;
		while (col < size)
		{
			if (puzzle[row][col].arrow_L == LESS)
				printf("> ");
			else if (puzzle[row][col].arrow_L == MORE)
				printf("< ");
			else
				printf("| ");
			if (puzzle[row][col].value >= 1 && puzzle[row][col].value <= 9)
				printf("%d ", puzzle[row][col].value);
			else
				printf("  ");
			col++;
		}
		printf("|\n");
		col = 0;
		row++;
	}
	while (col++ < size)
		printf("+ - ");
	printf("+\n");
}

void	printDebug(CELL puzzle[size][size], int size)
{
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			printf("{%d;%d}[%d]", i, j, puzzle[i][j].value);
			
			if (puzzle[i][j].arrow_U == LESS)
				printf("Uv");
			else if (puzzle[i][j].arrow_U == MORE)
				printf("U^");
			else if (puzzle[i][j].arrow_U == 0)
				printf("U-");

			if (puzzle[i][j].arrow_L == LESS)
				printf("L>");
			else if (puzzle[i][j].arrow_L == MORE)
				printf("L<");
			else if (puzzle[i][j].arrow_L == 0)
				printf("L-");
			printf(" ");
		}
		printf("\n");
	}
}