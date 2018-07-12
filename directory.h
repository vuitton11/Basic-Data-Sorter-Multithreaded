/*
 * directory.h - contains all methods that involve directory validity,
 * 				 creating directories, etc.
 */
#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>

typedef struct _Directories {

	char* cArg;
	char* dArg;
	char* oArg;

	char* currDir;
	char enterDir[100];
} directories;

void printDir(char* path);
void printStruct(directories *dirs);	//debug method
void addDir(directories* dir, char* newDir, char* path);
int checkExtension(char* file);

//debug method
void printStruct(directories *dirs) {
	printf("cArg: %s\n", dirs->cArg);
	printf("dArg: %s\n", dirs->dArg);
	printf("oArg: %s\n", dirs->oArg);
	printf("\n");
	printf("currDir: %s\n", dirs->currDir);
}

//edits the 
void addDir(directories* dir, char* newDir, char* path) {
	strcpy(dir->enterDir, newDir);
	strcat(dir->enterDir, "/");
	strcat(dir->enterDir, path);
}

//Returns 1 if extension is valid, 0 if not
int checkExtension(char* file) {
	
	file = strrchr(file, '.');

	if (strcmp(file, ".csv") == 0)
		return 1;

	return 0;
}

//INCOMPLETE
void printDir(char* path) {
	//printf("Current Path: %s\n", getDir(path));
}