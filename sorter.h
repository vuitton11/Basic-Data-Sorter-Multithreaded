#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <pthread.h>
#include "directory.h"
#include "input.h"
#include "counter.h"

#define TOTAL_THREADS 1024

/* STRUCT PURPOSE: This struct holds a pointer
 * to point to a custom created dynamicaly allocated
 * array. The benefit of this is to associate all
 * entries in array row with each other when merge
 * sorting. 
 */
typedef struct _group{
	char* metadata[28];
	pthread_mutex_t lock;
} group;

//Global Headers Array & determinant array
char* headers[28] = {
	"color", "director_name", "num_critic_for_reviews", "duration",
	"director_facebook_likes", "actor_3_facebook_likes", "actor_2_name",
	"actor_1_facebook_likes", "gross", "genres", "actor_1_name", "movie_title",
	"num_voted_users", "cast_total_facebook_likes", "actor_3_name", "facenumber_in_poster",
	"plot_keywords", "movie_imdb_link", "num_user_for_reviews", "language", "country",
	"content_rating", "budget", "title_year", "actor_2_facebook_likes", "imdb_score",
	"aspect_ratio", "movie_facebook_likes"
};

char det[28] = {
	's', 's', 'd', 'd', 'd', 'd', 's', 'd', 'd', 's', 's', 's',
	'd', 'd', 's', 'd', 's', 's', 'd', 's', 's', 's', 'd', 'd',
	'd', 'd', 'd', 'd'
};


//Global variables
int cols = 28;		//stores number of columns
int rows = 0;		//stores number of rows
int mallocSize = 80000;	//free this amount when done

//Global counters
counter threadCount;
counter dbIndex;

//Global Array of Pointers to Structs
group ** database;

//Checks to see if header matches the one we need to sort.
int checkHeader(char* file, char* title) {

	//Open the file to read
	FILE * input = fopen(file, "r");

	//fgets() the first line for the headers
	char line[10000];
	fgets(line, 10000, input);

	char* string = strdup(line);
	if ( strstr(string, title) != NULL) {
		return 1;
	}

	return 0;
}

//Strips whitespace at the ends of a string.
char* stripSpace(char* string) {
	
	int length = strlen(string);
	char ans[length-1];
	int i;

	//Special Case: if the string is empty
	if (length == 0) {
		//printf("hit special case:\n");
		return string;
	}
	//Check if there are any spaces at all.
	if (string[0] != ' ' && string[length-1] != ' ') {
		return string;
	}

	//Case 1: if we have one one space in the front
	else if(string[0] == ' ' && string[length-1] != ' ') {

		//Start copying the string from the char after the space.
		for (i = 1; i < length; i++) {
			ans[i-1] = string[i];
		}

		//Tack on '\0'
		ans[i-1] = '\0';		
	}

	//Case 2: if we only have one space in the end
	else if(string[0] != ' ' && string[length-1] == ' ') {

		//Start copying the string until the char before the space
		for (i = 0; i < length-1; i++) {
			ans[i] = string[i];
		}

		//Tack on '\0'
		ans[i] = '\0';
	}

	//Case 3: if we have both spaces on the ends
	else {

		//Start copying the string from the char after the space.
		for (i = 1; i < length-1; i++) {
			ans[i-1] = string[i];
		}

		//Tack on '\0'
		ans[i-1] = '\0';
	}

	return strdup(ans);
}

void inputStruct(group** database, group* currStruct) {

	
	//pthread_mutex_lock(&dbIndex.lock);

	database[getCounter(&dbIndex)] = currStruct;

	incrementCounter(&dbIndex);

	printf("-------dbIndex is now: %d\n", getCounter(&dbIndex));
	//pthread_mutex_unlock(&dbIndex.lock);
}

//returns the number of the row in which we have to sort by
int cArgColumn(char* cArg) {

	int i;
	for(i = 0; i < 28; i++) {

		if( strcmp(headers[i], cArg) ) {
			return i;
		}
	}

	return -1;
}

int checkComma(char* string) {

	int i;
	if (string == NULL) {
		return 0;
	}
	char* test = strdup(string);
	int length = strlen(test);

	//if (test == NULL) {
	//	return 0;
	//}

	for (i = 0; i < length; i++) {

		if (string[i] == ',')
			return 1;
	}

	return 0;
}

void printCSV(group** rowArr) {

	int i, j;
	char* test;

	//for (j = 0; j < cols; j++) {
	//	printf("string: %s\n", rowArr[667]->metadata[j]);
	//}
	//printf("-------------------\n");
	//printf("rows: %d\n", rows);
	for(i = 0; i < rows; i++) {
		
		//int counter = 0;
		for (j = 0; j < cols; j++) {
			
			test = rowArr[i]->metadata[j];

			if (j != cols-1 && !checkComma(test) ) {
				printf("%s,", rowArr[i]->metadata[j]);				
			}
			
			else if (j != cols-1 && checkComma(test) ) {
				printf("\"%s\",", rowArr[i]->metadata[j]);				
			}	

			else if ( checkComma(test) ) {
				printf("\"%s\"", rowArr[i]->metadata[j]);				
			}	

			else {
				printf("%s", rowArr[i]->metadata[j]);
			}
		}
	} //end printf double for loop

}
