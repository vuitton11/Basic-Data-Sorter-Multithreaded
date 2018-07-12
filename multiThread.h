/*
 * multiThread.h - contains any multithreading processes
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <pthread.h>

#define TOTAL_THREADS 1024	//max iLab machine can hold

void* recursiveDir(void* dirs);

//Pass in a struct as a void* for threading
void* recursiveDir(void* dirs) {

	//Create storage for threads
	pthread_t *threadArr = malloc( TOTAL_THREADS * sizeof(pthread_t) );

	//turn the void* into a struct for usage & create storage of structs for threads
	struct directories *dirArr = malloc(TOTAL_THREADS * sizeof(directories) );

	void * id;	//used to return thread ID

	DIR * dp;	//traverse through directories
	
	//This struct is available through <dirent.h>, which includes 2 memebers:
	// ino_t d_ino;		(file serial number)
	// char d_name[];	(name of entry)
	// d_type is also included in this struct for LINUX. DT_DIR is a directory
	struct dirent *path;	

	//If we cannot enter anymore directories, exit thread
	if ( (dp = opendir(dirs->currDir)) == NULL ) {
		printf("successfully traversed through %s\n", dirs->currDir);
		pthread_exit(NULL);
	}

	//Loop through the entire directory. Identify if file or directory
	int i = 0; //keeps track of array of structs
	while ( (path = readdir(dp)) ) {

		//Case 1) We find a directory, create a thread to recurse deeper into the dir
		if (path->d_type == DT_DIR) {
			
			//Continue if it's a directory (maybe redundant?)
			if ( strcmp(path->d_name, ".") || strcmp(path->d_name, "..") ) {
				continue;
			}

			//Output thread ID for directory & increase thread
			printf("Dir: %s | %ld\n", path->d_name, pthread_self());
			incrementCounter(&threadCount);

			//Create the new path
			addDir(dirs[i], path->d_name);

			//Recurse deeper into the directory w/ new thread. Returns 0 if successful
			int code;
			if ( (code = pthread_create(&threadArr[i], NULL, recursiveDir, &dirArr[i])) != 0 ) {
				printf("Error creating thread (%d)\n", code);
				pthread_exit(NULL);
			}

			i++;	//increment array
		}	//end case 1

		//Case 2) we find a file, check extension, begin threaded sort
		else {

			if (checkExtension(path->d_name)) {
				
				//Output thread ID for directory & increase thread
				printf("Dir: %s | %ld\n", path->d_name, pthread_self());
				incrementCounter(&threadCount);

				//Create new path
				addDir(dirArr[i], path->d_name);

				//sort here
			}

		}
	} //end (while) directory traversal

	//Join back all threads
	int j;
	for(j = 0; j < i; j++) {
		pthread_join(threadArr[j], &id);
	}

	return 0;
}