/*
 * input.c - contains all argument input methods to run sorting program.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <getopt.h>
#include <dirent.h>
#include <errno.h>

//Declare functions
void manual();
void getArgs(char* cArg, char* dArg, char* oArg, int argc, char* argv[]);
void checkValidityArgs(char* cArg, char* dArg, char* oArg);
void checkNumArgs(int argc);
int validDir(char* path);

//Checks arguments. Print examples if not valid. [working]
void checkNumArgs(int argc) {
	
	//invalid if <= 2 arguments
	if (argc <= 2) {
		manual();
		exit(0);
	}
}

//Grab arguments and set to strings [working]
void getArgs(char* cArg, char* dArg, char* oArg, int argc, char* argv[]) {

	int flag;

	//loop through and set flags
	while( (flag = getopt(argc, argv, "-c:-d:-o:")) != -1 ) {

		switch(flag) {
			case 'c':
				strcpy(cArg, optarg);
				break;
			case 'd':
				strcpy(dArg, optarg);
				break;
			case 'o':
				strcpy(oArg, optarg);
				break;
			case '?':
				printf("Invalid argument(s)\n");
				printf("\n");
				manual();
			default:
				manual();
		}
	}
}

//Check the validity of args [working]
void checkValidityArgs(char* cArg, char* dArg, char* oArg) {

	//Check cArg
	if (cArg[0] == '\0') {
		manual();
		exit(0);
	}

	//Check if -d flag present but is not valid dir
	if ( dArg != '\0' && !validDir(dArg)) {
		printf("Invalid root directory: %s\n", dArg);
		exit(0);
	}

	//Check if -o present is valid. If not, create dir
	if (oArg != '\0') {
		
		//If directory not present, create directory
		if (!validDir(oArg)) {
			if(mkdir(oArg, 0777) != -1) {
				printf("Error: Cannot create directory (%s)\n", oArg);
				exit(0);
			}
		}
	} //end oArg condition	
}

//Returns 1 if directory is present, 0 if not. [working]
int validDir(char* path) {

	DIR *dir = opendir(path);

	if (dir) {
		closedir(dir);
		return 1;
	}

	//Directory DNE if ENOENT == errno
	if (ENOENT == errno) {
		return 0;
	}

	return 0;
}

//Prints out how to use the program [working]
void manual() {
	printf("---------------------------------------------------------\n");
	printf("How to use this program:\n");
	printf("\n");
	printf("Flags (can be used in any arbitrary order): \n");
	printf("-c - title you wish to sort by [MANDATORY]\n");
	printf("-d - absolute root directory path [OPTIONAL]\n");
	printf("-o - NON-absolute output directory path [OPTIONAL]\n");
	printf("\n");
	printf("Example: ./sorter -c genre\n");
	printf("Example: ./sorter -c color -d /usr/root/ -o usr/result\n");
	printf("\n");
	printf("Note: if -d and -o are not indicated, they are defaulted to current directory.\n");
	printf("      If -o directory is not present, it is created\n");
	printf("---------------------------------------------------------\n");
}