#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>

// Gameplay Related Constants
#define MAX_COLORS 26
#define MIN_TUBES 5
#define MAX_TUBES 50
#define BALLS_PER_TUBE 10

// Bonus Related Constants
#define BONUS_50 100
#define BONUS_75 150
#define BONUS_100 300

// Score Related Constants
#define VALID_MOVE_SCORE 5
#define INVALID_MOVE_SCORE -4

// Global Variables
char colors[MAX_COLORS] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G',  // Amaranth, Blue, Cinnamon, Daffodil, Ebony, Fuschia, Green
							'H', 'I', 'J', 'K', 'L', 'M', 'N',  // Hazel, Ivory, Jade, Kiwi, Lemon, Mango, Navy
							'O', 'P', 'Q', 'R', 'S', 'T', 'U',  // Olive, Pink, Quartz, Red, Silver, Teal, Uranus
							'V', 'W', 'X', 'Y', 'Z' };			// Vermilion, White, Xenon, Yellow, Zinc

char* colorBowl;	    // Container array to shuffle the balls
char** tubes;		    // 2D Array that'll represent the tubes
int tubeCount = 0;      // # of tubes to put in the 2D array
int emptyTubes = 2;     // # of empty tubes to be created to make sure the game is playable
int validMoves = 0;	    // # of successful moves 
int invalidMoves = 0;   // # of unsuccessful moves
bool gameOver = false;  // Keeps track of the game's status 

// Function Prototypes
void clear();
void initialize(int* _tubeCount);
void generateBalls(int _tubeCount);
void swap(char* a, char* b);
void shuffle();
void createTubes(int _tubeCount);
void fill();
void printTubes();
void updateGame();
void makeMove(int src, int trg);
int findLastNonEmptyIndex(int index);
bool isEmpty(int target);
bool isGameCompleted();

int main() {

	int temp;

	// Prompt the user for the number of tubes,
	// Check the boundaries
	do {
		printf("\nEnter the number of tubes (%d to %d): ", MIN_TUBES, MAX_TUBES);
		(void)scanf("%d", &tubeCount);

		// Boundary controls & Error feedback
		if (tubeCount < MIN_TUBES || tubeCount > MAX_TUBES) {
			clear();
			printf("Minimum number of tubes is %d.", MIN_TUBES);
			printf("\nMaximum number of tubes is %d.", MAX_TUBES);
		}
	} while (tubeCount < MIN_TUBES || tubeCount > MAX_TUBES);

	// Prompt the user for the number of empty tubes,
	do {
		printf("\nEnter the number of EMPTY tubes (Minimum: %d): ", emptyTubes);
		(void)scanf("%d", &temp);

		// Boundary controls & Error feedback
		// Minimum # of empty tubes is 2,
		// Maximum # of empty tubes can't be more than double of tubeCount.
		if (temp < 2 || temp > tubeCount*2) {
			clear();
			printf("Minimum number of empty tubes is %d.", emptyTubes);
			printf("\nMaximum number of empty tubes is %d.", tubeCount*2);
		}
	} while (temp < 2 || temp > tubeCount * 2);

	// Set the value of emptyTubes
	emptyTubes = temp;

	// Allocate memory for the color bowl
	size_t ballCount = (size_t)tubeCount * BALLS_PER_TUBE;
	colorBowl = malloc(ballCount * sizeof(char));

	clear();
	initialize(&tubeCount);

	while (gameOver == false) {

		int source, target;

		// MakeMove adds +1 since player sees the tubes starting from 1, so no 0'th index
		// Boundary checks to make sure source and target don't get picked out of range
		do {

			printf("\nEnter a tube to move from and destination (eg.: 1 7): ");
			(void)scanf("%d %d", &source, &target);

		} while (source < 1						  ||       // Source can't be a non-existent tube
				 target < 1					      ||	   // Target can't be a non-existent tube		 
				 source > tubeCount + emptyTubes  ||       // Source can't be more than the total number of tubes
			     target > tubeCount + emptyTubes  );       // Target can't be more than the total number of tubes						    

		
		/*printf("\nSelect a target: ");
		(void)scanf("%d", &target);*/

		makeMove(source, target);
		updateGame();
	}
	return 0;
}


// Clears screen
void clear() {
	system("cls");
}

void initialize(int* _tubeCount) {

	printf("\nInitializing the game...\n");

	// Generate balls
	generateBalls(*_tubeCount);

	// Shuffle the generated balls
	shuffle();

	// Create 2D tubes array
	createTubes(*_tubeCount);

	// Put the balls into the tubes
	fill();

	// Print the tubes:
	updateGame();


	//print();
}

// Generates necessary amount of balls,
// Specific amount (BALLS_PER_TUBE) of each color.
void generateBalls(int _tubeCount) {
	/*
	printf("\nGenerating balls...\n");

	// We'll use this to cycle over the colors
	int index = 0;

	// Run the code for the amount of tubes
	for (int i = 0; i < _tubeCount; i++) {

		// Get same amount of colors
		for (int j = 0; j < BALLS_PER_TUBE; j++) {

			// In case the loop runs out of available colors,
			// Start over by resetting the index.
			if (index >= sizeof(colors) / sizeof(colors[0])) {
				index = 0;
			}

			// Set the corresponding index equal to the respective color
			colorBowl[i * BALLS_PER_TUBE + j] = colors[index];

			// Debug purposes, Comment it out when you're done!
			//printf("\nPushed letter %c", colors[index]);
		}

		// Increment index
		index++;
	}

	*/

	// We'll use this to cycle over the colors
	size_t index = 0;

	// Run the code for the amount of tubes
	for (int i = 0; i < _tubeCount; i++) {

		// Get the same amount of colors
		for (int j = 0; j < BALLS_PER_TUBE; j++) {

			// Set the corresponding index equal to the respective color
			colorBowl[i * BALLS_PER_TUBE + j] = colors[index];

			// Debug purposes, Comment it out when you're done!
			//printf("\nPushed letter %c", colors[index]);
		}

		// Increment the index for colors with wrap-around
		index = (index + 1) % (sizeof(colors) / sizeof(colors[0]));
	}
}

// Axillary function for shuffling.
// Swaps two characters.
void swap(char* a, char* b) {
	char temp = *a;
	*a = *b;
	*b = temp;
}

// Shuffles the generated balls
void shuffle() {
	printf("\nShuffling the balls...\n");

	// Random number generating seed
	srand((unsigned int)time(NULL));
	size_t size = (size_t)tubeCount * BALLS_PER_TUBE;

	for (size_t i = size - 1; i > 0; i--) {
		// Generate a random index between 0 and i (inclusive)
		size_t j = rand() % (i + 1);

		// Swap the elements at indices i and j
		swap(&colorBowl[i], &colorBowl[j]);
	}
}

void createTubes(int _tubeCount) {

	printf("\nAllocating memory for 2D tubes array...\n");

	// Allocate memory for tubes (rows), including two additional tubes
	tubes = (char**)malloc((size_t)_tubeCount * sizeof(char*) + (size_t)emptyTubes * sizeof(char*));
	if (tubes == NULL) {
		// Handle memory allocation failure
		fprintf(stderr, "Failed to allocate memory for tubes.\n");
		exit(EXIT_FAILURE);
	}

	// Allocate memory for balls (cols) for each tube and initialize with a placeholder
	for (int i = 0; i < (_tubeCount + emptyTubes); i++) {
		tubes[i] = (char*)malloc(BALLS_PER_TUBE * sizeof(char));
		if (tubes[i] == NULL) {
			// Handle memory allocation failure
			fprintf(stderr, "Failed to allocate memory for tubes[%d].\n", i);
			exit(EXIT_FAILURE);
		}

		// Initialize tubes with a placeholder (e.g., space character)
		for (int j = 0; j < BALLS_PER_TUBE; j++) {
			tubes[i][j] = '*';  // Placeholder for empty space
		}
	}
}

// Fills the tubes
void fill() {
	/*
	printf("\nFilling the tubes...\n");
	size_t colorBowlIndex = 0;
	for (int i = 0; i < tubeCount; i++) {
		for (int j = 0; j < BALLS_PER_TUBE; j++) {
			tubes[i][j] = colorBowl[colorBowlIndex];
			colorBowlIndex++;
		}
	} */
	printf("\nFilling the tubes...\n");
	size_t colorBowlIndex = 0;
	for (int i = 0; i < tubeCount; i++) {
		for (int j = 0; j < BALLS_PER_TUBE; j++) {
			tubes[i][j] = colorBowl[colorBowlIndex];
			colorBowlIndex++;
		}
	}
}

// Prints the tubes
void printTubes() {
	int rows = tubeCount + emptyTubes;
	int cols = BALLS_PER_TUBE;

	// Calculate the width of each element
	int elementWidth = 4;

	// Find the maximum number of digits needed for row indices
	int rowIndexDigits = 1;
	int tempRows = rows;
	while (tempRows / 10 > 0) {
		rowIndexDigits++;
		tempRows /= 10;
	}

	// Find the maximum number of digits needed for column indices
	int colIndexDigits = 1;
	int tempCols = cols;
	while (tempCols / 10 > 0) {
		colIndexDigits++;
		tempCols /= 10;
	}

	// Calculate the overall width for proper alignment
	int overallWidth = (elementWidth + 1) * (colIndexDigits + 1) + cols * (elementWidth + 1);

	// Print top border
	for (int k = 0; k < overallWidth; k++) {
		printf("-");
	}

	// Print column indices
	printf("\nindex:");
	for (int j = 0; j < cols; j++) {
		printf("%*d", elementWidth + 1, j);
	}
	printf("\n");

	// Print horizontal line
	for (int i = 0; i < overallWidth; i++) {
		printf("-");
	}
	printf("\n");

	// Print array elements
	for (int i = 0; i < rows; i++) {
		printf("Tube%*d:", rowIndexDigits, i + 1);
		for (int j = 0; j < cols; j++) {
			printf("%*c", elementWidth + 1, tubes[i][j]);
		}
		printf("\n");

		// Print horizontal line after each row
		for (int k = 0; k < overallWidth; k++) {
			printf("-");
		}
		printf("\n");
	}


}

// Prints the updated state of the game
void updateGame() {

	clear();
	// Check if the game is completed
	if (isGameCompleted()) {
		int bonus;
		int percentage = (validMoves * 100) / (validMoves + invalidMoves);
		int positiveScore = validMoves * VALID_MOVE_SCORE;
		int negativeScore = invalidMoves * INVALID_MOVE_SCORE;

		printf("\nCongratulations! You completed the game.\n");
		printf("%d%% of your moves were succesful!\n", percentage);

		if (percentage > 50) {
			bonus = BONUS_50;
		}
		else if (percentage > 75) {
			bonus = BONUS_75;
		}
		else if (percentage == 100) {
			bonus = BONUS_100;
		}
		else {
			bonus = 0;
		}

		if (bonus > 0) {
			printf("\nYou earned %d bonus points!\n", bonus);
		}

		int totalScore = positiveScore + negativeScore + bonus;

		printf("Succesful moves: %d = %d points\n", validMoves, positiveScore);
		printf("Unsuccessful moves: %d = %d points\n", invalidMoves, negativeScore);
		printf("Your total score is: %d\n", totalScore);
		gameOver = true;
		return;  // Exit the game loop
	}
	else {
		printf("\n%d successful moves made out of total %d moves.\n", validMoves, validMoves + invalidMoves);
		printf("\nCurrent state of the game (ball order: bottom -> top):\n");
		printTubes();
	}
}

// Function that handles ball movement from source to target
void makeMove(int src, int trg) {

	// Because we show the tube numbers starting from 1
	int source = src - 1;
	int target = trg - 1;

	// Check if source and target are valid numbers
	if (source > tubeCount + emptyTubes || target > tubeCount + emptyTubes) {

		// Out of boundaries
		printf("\nYou are trying to access non-existent tubes.\n");

		// Increase the # of unsuccessful moves
		invalidMoves++;
	}
	else {

		// Is source empty or not?
		if (!isEmpty(source)) {

			// Source is not empty, but the target is
			if (isEmpty(target)) {

				// Get the index of the last element of the source
				int index = findLastNonEmptyIndex(source);

				// Assign the zero'th index of the target
				tubes[target][0] = tubes[source][index];

				// Remove the moved element from the source
				tubes[source][index] = '*';

				// Increase the # of successful moves
				validMoves++;
			}
			else {
				// Source is not empty, and the target is not empty either
				// But is target full?
				if (findLastNonEmptyIndex(target) == BALLS_PER_TUBE - 1) {

					// Show the error
					printf("\nInvalid move: The target is full.\n");

					// Increase the # of unsuccessful moves
					invalidMoves++;
				}
				else {
					// Source is not empty,
					// Target is not empty, but it's not full either
					// We should compare if the colors match

					// Get the index of the last element of the source
					int sourceIndex = findLastNonEmptyIndex(source);

					// Get the index of the last element of the target
					int targetIndex = findLastNonEmptyIndex(target);

					// Colors are matching! Fantastic.
					if (tubes[source][sourceIndex] == tubes[target][targetIndex]) {

						// Move the ball
						tubes[target][targetIndex + 1] = tubes[source][sourceIndex];

						// Remove the element from the source
						tubes[source][sourceIndex] = '*';

						// Increase the # of successful moves
						validMoves++;
					}
					else {
						// Sorry, the colors don't match.
						printf("\nInvalid move. The colors are different.\n");

						// Increase the # of unsuccessful moves
						invalidMoves++;
					}
				}
			}
		}
		else {

			// Source is empty!
			printf("\nInvalid move. The source is empty.\n");

			// Increase the # of unsuccessful moves
			invalidMoves++;
		}
	}
}

bool isEmpty(int target) {
	for (int i = 0; i < BALLS_PER_TUBE; i++) {
		if (tubes[target][i] != '*') {
			return false; // Target tube is not empty
		}
	}
	return true; // Target tube is empty
}

int findLastNonEmptyIndex(int index) {
	size_t lastIndex = SIZE_MAX;

	for (size_t i = 0; i < BALLS_PER_TUBE; i++) {
		if (tubes[index][i] != '*') {
			lastIndex = i;
		}
	}

	return (int)lastIndex;
}

bool isGameCompleted() {
	int numRows = tubeCount + emptyTubes;
	int numCols = BALLS_PER_TUBE;

	for (int row = 0; row < numRows; row++) {
		for (int col = 1; col < numCols; col++) {
			if (tubes[row][col] != tubes[row][0]) {
				// Elements in the current row are not equal, return false
				return false;
			}
		}
	}

	// All columns in all rows have equal elements
	return true;
}