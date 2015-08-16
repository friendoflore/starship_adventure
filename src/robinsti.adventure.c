/******************************************************************************
 * Author: Tim Robinson
 * Date created: 7/14/2015
 * Last modification date: 7/15/2015 2:26 PM PST
 * Description: This program chooses 7 rooms at random from a bank of 10 and
 *		creates an adventure game from them. The program creates connections
 *		between the rooms at random, each room with at least 3 connections and at
 *		most 6 connections. The program then randomly selects a starting room and
 *		an ending room. The goal of the game is to traverse the rooms by their
 *		connections, navigating from the starting room to the ending room. Once
 *		the user has successfully reached the ending room, the program prints the
 *		path they took through the rooms to victory and how many steps they took.
 *****************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <time.h>

// This is used to store the connections to a room
struct ConnList {
	int size;
	int capacity;
	// This stores the names of the connected rooms
	char **connections;
};

// This is used to create the rooms and set up the game
struct Room {
	char *name;
	char *type;
	struct ConnList connectionList;
};

// This is used to track game progress
struct PlayerToken {
	int stepsTaken;
	char pathToVictory[32][32];
};

/******************************************************************************
 * Function: _initConnList
 * Description: This function initializes the ConnList struct that will reside
 *		in a Room struct. This struct is initialized with the number of 
 *		connections to be added as the capacity, a size of 0, and memory 
 *		allocated for an array of the maximum number of connections.
 *	Params: struct Room pointer, int numConns. The Room pointer is the room in 
 *		which the ConnList struct resides. numConns is the number of connections 
 *		that will be randomly created by the program.
 * Preconditions: This function needs a room in process of initialization and
 *		numConns of at least 3, at most 6.
 *	Postconditions: This function finishes with a Room's connection list struct
 *		initialized, a part of the room initialization.
 *****************************************************************************/

void _initConnList(struct Room *room, int numConns) {
	room->connectionList.connections = malloc(sizeof(char *) * 6);
	room->connectionList.capacity = numConns;
	room->connectionList.size = 0;
}

/******************************************************************************
 * Function: _initRoom
 * Description: This function initializes a Room struct that will be used to
 *		create files for gameplay. This function assigns a room its name and the
 *		number of connections it will have.
 *	Params: struct Room *, char *, int. The room struct passed in is the room
 *		this function is initializing, the char *initName is the room's name, and
 *		the int numConns is the number of connections this room will have.
 * Preconditions: This function needs a room to have a name and number of
 *		connections chosen to be initialized appropriately.
 *	Postconditions: This function finishes with a Room struct initialized, ready
 *		to be used to create the conditions for gameplay.
 *****************************************************************************/

void _initRoom(struct Room *room, char *initName, int numConns) {
	room->name = malloc(sizeof(char *) * 16);
	room->name = initName;
	room->type = malloc(sizeof(char *) * 10);

	_initConnList(room, numConns);

}

/******************************************************************************
 * Function: _createRoom
 * Description: This function creates a new Room struct and calls the function
 *		to initialize it. Then this function returns the initialized room.
 *	Params: char *, int. The char *initName is the new room's name, the
 *		int numOfConns is the number of connections that room will have.
 * Preconditions: This function needs a room name and the number of connections
 *		to be chosen by the program.
 *	Postconditions: This function returns an initialized room.
 *****************************************************************************/

struct Room * _createRoom(char *initName, int numOfConns) {
	struct Room *newRoom = (struct Room *) malloc(sizeof(struct Room));
	_initRoom(newRoom, initName, numOfConns);

	return newRoom;
}

/******************************************************************************
 * Function: _createEdge
 * Description: This function creates a connection between the two rooms passed
 *		in if the request for the connection between the rooms is valid. In order
 *		to be valid, the connecting rooms must not be at their maximum number of
 *		connections already and they must not already be connected. 
 *	Params: struct Room *, struct Room*. The struct Room *roomBase is the room
 *		that we know still needs a connection defined by the randomly chosen
 *		number of connections that room is to have. The connection to the 
 *		struct Room *roomConn is what this function validates and attempts.
 * Preconditions: This function needs two appropriately initialized rooms
 *		passed to it to work as expected.
 *	Postconditions: This function returns a 1 if the connection is created 
 *		between the two rooms. It returns a 0 if the connection is invalid.
 *****************************************************************************/

int _createEdge(struct Room *roomBase, struct Room *roomConn) {

	// Check if the rooms have already been connected
	int i = 0;
	for(i = 0; i < roomBase->connectionList.size; i++) {
		if(roomBase->connectionList.connections[i] == roomConn->name) {
			return 0;
		}
	}

	// Check if room is at its max number of connections
	if(roomConn->connectionList.size == 6) {
		return 0;
	}
	
	// Check to see if we need to adjust the initial random parameters
	// to accomodate for needed connections
	if((roomConn->connectionList.size == roomConn->connectionList.capacity) && (roomConn->connectionList.size < 7)) {
		// Put "Room 2" as connected to "Room 1"
		roomBase->connectionList.connections[roomBase->connectionList.size] = roomConn->name;
		// Increase the capacity (which was randomly chosen initially)
		roomConn->connectionList.capacity = roomConn->connectionList.capacity + 1;
		// Put "Room 1" as connected to "Room 2"
		roomConn->connectionList.connections[roomConn->connectionList.size] = roomBase->name;

		// Increase the number of connections for each room by 1
		roomBase->connectionList.size = roomBase->connectionList.size + 1;
		roomConn->connectionList.size = roomConn->connectionList.size + 1;

		return 1;
	}
	
	// Put "Room 2" as connected to "Room 1"
	roomBase->connectionList.connections[roomBase->connectionList.size] = roomConn->name;
	// Put "Room 1" as connected to "Room 2"
	roomConn->connectionList.connections[roomConn->connectionList.size] = roomBase->name;

	// Increase the number of connections for each room by 1
	roomBase->connectionList.size = roomBase->connectionList.size + 1;
	roomConn->connectionList.size = roomConn->connectionList.size + 1;

	return 1;
}

/******************************************************************************
 * Function: _createNewPlayer
 * Description: This function creates and initializes a new player struct that
 *		will be used to track game progress.
 *	Params: None
 * Preconditions: None
 *	Postconditions: This function returns an newly created and initialized
 *		player struct.
 *****************************************************************************/

struct PlayerToken * _createNewPlayer() {
	struct PlayerToken * newPlayer = (struct PlayerToken *) malloc(sizeof(struct PlayerToken));
	newPlayer->stepsTaken = 0;


	return newPlayer;
}
/*
void _freeBoard(struct Room **rooms) {
	for(i = 0; i < 7; i++) {
		free(rooms[i]);
	}
	rooms = 0;
}
*/

int main(void) {
	srand(time(NULL));

	// For loop iterators
	int i = 0;
	int j = 0;

	// Create the temporary file to be used for gameplay, using the process ID
	// Get the process ID and store it
	pid_t PID = getpid();

	// This will be the base of the temporary directory we create, in addition
	// to the process id, added to the tmpFile char array below.
	char tmpFile[16] = "robinsti.rooms.";

	// Convert the PID to a string
	char pidString[6];
	sprintf(pidString, "%d", PID);

	// This will store the name of the temporary directory
	char * dirName = tmpFile;

	// Add the process ID to the base directory name string
	strcat(dirName, pidString);

	mkdir(dirName, 0777);

	// These are all of the possible room names
	char *roomBank[10];
	roomBank[0] = "Conference";
	roomBank[1] = "Lounge";
	roomBank[2] = "Bridge";
	roomBank[3] = "Transporter";
	roomBank[4] = "Holodeck";
	roomBank[5] = "Sickbay";
	roomBank[6] = "Engineering";
	roomBank[7] = "Cargo";
	roomBank[8] = "Lab";
	roomBank[9] = "Shuttlebay";

	// This will store all of the rooms created for gameplay
	struct Room *rooms[7];

	// Allocate memory for each of the rooms and their connection lists
	for(i = 0; i < 7; i++) {
		rooms[i] = (struct Room *) malloc(sizeof(struct Room) + sizeof(struct ConnList));
	}

	// This array will track all of the room indices already chosen at random in
	// order to prevent repeats
	int roomsChosen[7];

	// This is used to re-select a random room choice if a room is chosen more
	// than once
	int rerandom = 0;

	// Initialize the array to check the indices of randomly chosen rooms
	for(i = 0; i < 7; i++) {
		roomsChosen[i] = -1;
	}

	// Choose (at random) which 7 of the 10 rooms will be in this game
	for(i = 0; i < 7; i++) {

		// Use the do-while loop to re-select rooms that have already been chosen
		do {
			int random = rand() % 10;

			// Check the list of already chosen rooms
			for(j = 0; j < 7; j++) {
				if(random == roomsChosen[j]) {
					rerandom = 1;
					break;
				} else {
					rerandom = 0;
				}
			}

			// Choose the number of connections that room will have
			int randNum = rand() % 7;
			
			// Make sure that the number of connections is greater than or equal
			// to 3 and less than or equal to 6
			if(randNum > 3) {
				randNum = randNum - 3;
			}
			randNum = randNum + 3;

			// If we do not need to reselect the random room, create a room in the
			// array with that name and number of connections
			if(rerandom == 0) {
				rooms[i] = _createRoom(roomBank[random], randNum);
				roomsChosen[i] = random;
			}

		} while(rerandom);
	}


	// Create the connections for each room
	for(i = 0; i < 7; i++) {
		int rerandom0 = 0;
		int rerandom1 = 0;

		// Get the number of connections the current room still needs
		int connsToAdd = rooms[i]->connectionList.capacity - rooms[i]->connectionList.size;

		// If this room does not need any more connections, continue to the next
		// room
		if(connsToAdd < 1) {
			continue;
		}

		// Create the remaining connections this room needs
		for(j = 0; j < connsToAdd; j++) {
			int randConnIdx = -1;

			// Use this do-while loop to re-select the randomly chosen connection
			// if it is not a valid selection
			do {
				randConnIdx = rand() % 7;
				
				// If the randomly chosen connection is this room, re-select the
				// random number. A room cannot be connected to itself
				if(randConnIdx == i) {
					rerandom0 = 1;
				} else {
					rerandom0 = 0;

					// Create a connection between the two rooms. This function
					// validates the other requirements for a connection to be made
					rerandom1 = _createEdge(rooms[i], rooms[randConnIdx]);
				}
			} while(rerandom0 || (rerandom1 == 0));
		}
	}


// At this point, all of the rooms have been created and have connections
	
	// Choose the starting room
	int startIdx = rand() % 7;

	// Set the room type
	rooms[startIdx]->type = "START_ROOM";
	int endIdx = 0;

	// Use a do-while loop to ensure the ending room is not the starting room
	do {
		endIdx = rand() % 7;
		if(endIdx == startIdx) {
			rerandom = 1;
		} else {
			rerandom = 0;
		}
	} while(rerandom);

	// Set the room type
	rooms[endIdx]->type = "END_ROOM";

	// Set all of the other room types as a middle room
	for(i = 0; i < 7; i++) {
		if((strncmp(rooms[i]->type, "START_ROOM", 3) == 0) || (strncmp(rooms[i]->type, "END_ROOM", 3) == 0)) {
		} else {
			rooms[i]->type = "MID_ROOM";
		}
	}

	// Use an array of file pointers for the room files
	FILE *fptr[7];

	// Use an array to store all of the room file file paths
	char filepaths[7][80];
	strcat(dirName, "/");

	// Put each room's information into its own file in the temporary directory
	for(i = 0; i < 7; i++) { 
		char filename[80];
		strcpy(filename, dirName);
		strcat(filename, rooms[i]->name);

		// Store the file path in the array so that we can read them later
		strcpy(filepaths[i], filename);
		fptr[i] = fopen(filename, "w");

		// Write the room's information to the file, formatted
		fprintf(fptr[i], "ROOM NAME: %s\n", rooms[i]->name);
		for(j = 0; j < rooms[i]->connectionList.size; j++) {
			if(rooms[i]->connectionList.connections[j] != 0) {
				fprintf(fptr[i], "CONNECTION %d: %s\n", j + 1, rooms[i]->connectionList.connections[j]);
			}
		}
		fprintf(fptr[i], "ROOM TYPE: %s\n", rooms[i]->type);
		
		fclose(fptr[i]);
	}

	// Create a player for the game, which is a struct that tracks game progress
	struct PlayerToken * player = _createNewPlayer();
	
	// The game procedure when this equals 0
	int stillPlaying = 1;
	int beginIdx = -1;

	// Get the starting room index
	for(i = 0; i < 7; i++) {
		if(strncmp(rooms[i]->type, "START_ROOM", 1) == 0) {
			beginIdx = i;
		}
	}

	// This is used to track the number of connections
	int printcounter = 0;

	// This is used to format the interface, storing the line count of a file
	int linecounter = 0;

	// This stores the possible choices a user can make for room traversal
	char choices[6][16];
	do {

		// If the ending room has been selected, end the game loop
		if(strncmp(rooms[beginIdx]->type, "END", 3) == 0) {
			printf("\nYOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
			break;
		}

		// Open the appropriate room file
		fptr[beginIdx] = fopen(filepaths[beginIdx], "r");
		char myLine[80];
		char tmpString[16];
		char c;
		linecounter = 0;

		// Count the number of lines to format the user interface printout
		for(c = getc(fptr[beginIdx]); c != EOF; c=getc(fptr[beginIdx])) {
			if(c == '\n') {
				linecounter = linecounter + 1;
			}
		}

		rewind(fptr[beginIdx]);
		printcounter = 0;

		while(fgets(myLine, sizeof myLine, fptr[beginIdx])) {

			// Get the room name and print it
			if(strncmp(myLine, "ROOM NAME: ", 11) == 0) {
				strcpy(tmpString, &myLine[11]);
				printf("\nCURRENT LOCATION: %sPOSSIBLE CONNECTIONS: ", tmpString);
			}
		
			// Format the connection list with commas appropriately
			if((printcounter != 0) && (printcounter < linecounter - 2)) {
				printf(", ");
			}

			// 
			if(strncmp(myLine, "CONNECTION ", 10) == 0) {
				strcpy(tmpString, &myLine[14]);
				int length = strlen(tmpString) - 1;

				if(tmpString[length] == '\n') {
					tmpString[length] = '\0';
				}

				// Store the current connection to the choices array
				strcpy(choices[printcounter], tmpString);
				printf("%s", tmpString);

				fflush(stdout);
				printcounter++;
			}
		}

		printf(".\nWHERE TO? >");

		char inputString[32];
		fgets(inputString, 32, stdin);

		// This is equal to 1 if the user's input does not match one of the
		// available choices
		int error = 0;

		// Check the user's selected room with the available choices from their
		// current room
		for(i = 0; i < printcounter; i++) {

			// If the user's input matches a room
			if(strncmp(choices[i], inputString, 3) == 0) {

				// Add the user's selection to the path to victory
				strcpy(player->pathToVictory[player->stepsTaken], choices[i]);

				// Increase the number of steps taken
				player->stepsTaken = player->stepsTaken + 1;

				// Match the user's input to the list of rooms to get the next
				// index
				for(i = 0; i < 7; i++) {
					if(strncmp(rooms[i]->name, inputString, 3) == 0) {
						stillPlaying = 1;
						fclose(fptr[beginIdx]);

						beginIdx = i;
					}
				}
				error = 0;
				break;
			} else {
				error = 1;
			}

		}

		if(error) {
			printf("\nHUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n");
		}

	} while(stillPlaying);

	// Print the number of steps taken and the path to victory for the player
	printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n", player->stepsTaken);

	for(i = 0; i < player->stepsTaken; i++) {
		printf("%s\n", player->pathToVictory[i]);
	}

	printf("%c\n", '%');

	// Remove all of the room files and then remove the temporary directory
	for(i = 0; i < 7; i++) {
		char filename[80];
		strcpy(filename, dirName);
		strcat(filename, rooms[i]->name);

		remove(filename);
	}

	printf("\n");
	rmdir(dirName);
	
	return 0;
}
