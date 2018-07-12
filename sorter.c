#include "sorter.h"
#include "mergeSort.h"

//Declare functions
void* recursiveDir(void* dirs);
void importData(char* file);
void mergeSort(int target, char det, group** rowArr, int low, int high);
void merge(int target, char det, group** a, int low, int high, int mid);

pthread_mutex_t testLock;
int main(int argc, char* argv[]) {

	//Check if we have enough arguments
	checkNumArgs(argc);

	//Initialize strings and get args
	char* cArg = malloc(30 * sizeof(char) );
	char* dArg = malloc(100 * sizeof(char) );
	char* oArg = malloc(100 * sizeof(char) );

	getArgs(cArg, dArg, oArg, argc, argv);

	//printf("c: %s | d: %s | o: %s\n", cArg, dArg, oArg);

	//Check if the arguments are valid
	checkValidityArgs(cArg, dArg, oArg);

	//declare struct type directories (to pass onto thread);
	directories dirs;
	dirs.cArg = cArg;
	dirs.dArg = dArg;
	dirs.oArg = oArg;
	dirs.currDir = dArg;

	//Create counters, structs, and recurse.
	database = malloc(mallocSize * sizeof(group*));	//create database for all structs
	//int i;
	//for( i = 0; i < mallocSize; i++) {
	//	pthread_mutex_init(&database[i]->lock, NULL);
	//}
	createCounter(&threadCount);					//start threadCount = 0
	createCounter(&dbIndex);							//start database index = 0;
	recursiveDir(&dirs);							//loop through all directories

	//Print final results
	printf("Total threads: %d\n", getCounter(&threadCount) );

	//Grab info for mergeSort (which col & number)
	rows = getCounter(&dbIndex);
	int low = 0;
	int high = rows - 1;
	int colSortIndex = cArgColumn(cArg);

	mergeSort(colSortIndex, det[colSortIndex], database, low, high);

	//Print the CSV
	printCSV(database);

}

//Pass in a struct as a void* for threading
void* recursiveDir(void* voidDirs) {

	directories *originalDir = (directories*) voidDirs;
	directories *dirArr = malloc(TOTAL_THREADS * sizeof(directories));
	directories *enterDirArr = malloc(TOTAL_THREADS * sizeof(directories) );

	pthread_t *threadArr = malloc(TOTAL_THREADS * sizeof(pthread_t));

	//void * id;	//used to return thread ID

	DIR * dp;	//traverse through directories
	
	//This struct is available through <dirent.h>, which includes 2 memebers:
	// ino_t d_ino;		(file serial number)
	// char d_name[];	(name of entry)
	// d_type is also included in this struct for LINUX. DT_DIR is a directory
	struct dirent *path;	
	
	int i = 0; //keeps track of array of structs
	//If we cannot enter anymore directories, exit thread
	if ( (dp = opendir(originalDir->currDir)) == NULL ) {
		printf("Successfully traversed through %s\n", originalDir->currDir);
		pthread_exit(NULL);
	}

	//Loop through the entire directory. Identify if file or directory
	while ( (path = readdir(dp)) ) {

		//Case 1) We find a directory, create a thread to recurse deeper into the dir
		if (path->d_type == DT_DIR) {
			
			//Continue if it's a directory (maybe redundant?)
			if ( !strcmp(path->d_name, ".") || !strcmp(path->d_name, "..") ) {
				continue;
			}

			//Output thread ID for directory & increase thread
			printf("DIR: %s | %ld\n", path->d_name, pthread_self());

			//save new struct info
			dirArr[i] = *originalDir;
			addDir(&enterDirArr[i], originalDir->currDir, path->d_name);
			dirArr[i].currDir = enterDirArr[i].enterDir;

			//Recurse deeper into the directory w/ new thread. Returns 0 if successful
			int code = pthread_create(&threadArr[i], NULL, recursiveDir, &dirArr[i]);
			incrementCounter(&threadCount);

			if ( code != 0 ) {
				printf("Error creating thread (%d)\n", code);
				pthread_exit(NULL);
			}

		}	//end case 1

		//Case 2) we find a file, check extension, begin threaded sort
		else {
			//pthread_mutex_lock(&testLock);

			if (checkExtension(path->d_name)) {
				
				//Output thread ID for directory & increase thread
				printf("FILE: %s | %ld\n", path->d_name, pthread_self());
				incrementCounter(&threadCount);

				//Create new path
				addDir(&enterDirArr[i], originalDir->currDir, path->d_name);
				dirArr[i].currDir = enterDirArr[i].enterDir;

				//Import data here
				int test = checkHeader(dirArr[i].currDir, originalDir->cArg);
				//printf("IMPORT DATA STRING: %s\n", dirArr[i].currDir);
				if ( test ) {
					//pthread_mutex_lock(&testLock);
					importData(dirArr[i].currDir);
					//pthread_mutex_unlock(&testLock);				
				}

			}

					//pthread_mutex_unlock(&testLock);
		}
		i++;

	} //end (while) directory traversal

	//Join back all threads
	//int j;
	//for(j = 0; j < i; j++) {
	//	pthread_join(threadArr[j], &id);
	//}

	return 0;
}

void importData(char* file) {
	int j, k;

	//Now, open the file to read
	FILE * input = fopen(file, "r");

	//Skip the headers array
	char line[10000];
	fgets(line, 10000, input);
	fgets(line, 10000, input);

	//input everything into database struct
	char* token;
	char* string = strdup(line);

	//Everytime we get a new line, we want to create a new struct to insert
	while ( fgets(line, 10000, input) != NULL) {	//traverse through all rows
		
		//Initialize mutex for database
		//pthread_mutex_lock(&database[getCounter(&dbIndex)]->lock);

		//printf("init i: %d\n", i);
		//pthread_mutex_init(&database[i]->lock, NULL);

		//Make the new struct
		group* currStruct = malloc(sizeof(group));

		//printf("%s", line);
		//Increment total number of rows
		rows++;	

		//Create char* for strsep()
		string = strdup(line);

		//printf("%s", string); //strings are all working

		/* CHECK PARSING CASES
		 */

		// 1) Average case: there are no "" in the row.
		if ( strchr(string, '\"') == NULL) {
			
			for ( j = 0; j < cols; j++ ) {
				token = strsep(&string, ",");
				token = stripSpace(token);

				if (token == NULL) {
					//printf("we hit null on average case");
				}			
				currStruct->metadata[j] = token;			
			}
		}

		// 2) If we have "" in the line
		else { //strchr returns != NULL

			//printf("inserted into special strsep\n");
			//Figure out how many commas before ""
			int commas = 0;
			int length = strlen(string);
			for (k = 0; k < length; k++) {

				if (string[k] == ',') commas++;

				else if (string[k] == '\"') break;
			}

			//strsep() that many times
			for (j = 0; j < commas; j++) {				
				token = strsep(&string, ",");
				token = stripSpace(token);			
				currStruct->metadata[j] = token;
				//printf("token: %s\n", token);
			}

			//Now deal with the commas
			token = strsep(&string, "\"");
			token = strsep(&string, "\"");

			//put special case into array
			token = stripSpace(token);			
			currStruct->metadata[j] = token;
			//printf("token: %s\n", token);

			j++;

			//skip over next comma
			token = strsep(&string, ",");

			//strsep() the rest of the line
			while ( (token = strsep(&string, ",")) != NULL ) {				
				token = stripSpace(token);
				if (token == NULL) {
					//printf("returned null");
				}
				currStruct->metadata[j] = token;
				j++;
			}

		} //end else special case

		//After finalizing struct, input into database and increment struct
		//pthread_mutex_lock(&testLock);
		inputStruct(database, currStruct);
		//pthread_mutex_lock(&database[getCounter(&dbIndex)]->lock);
		//pthread_mutex_unlock(&testLock);

	} //end getting lines (while statement)
}

void merge(int target, char det, group** arr, int low, int high, int mid) {
   int left = low, right = mid+1, i;

   group** sorted = malloc( rows * sizeof(group*) );

   for(i = low; left <= mid && right <= high; i++) {
      
		if (det == 'd') {
	    	
	    	if( atof(arr[left]->metadata[target]) <= atof(arr[right]->metadata[target]) )
	        	sorted[i] = arr[left++];
			else
	         sorted[i] = arr[right++];

	 	}

	 	else if (det == 's') {
	    	
	    	//Set variables for easier reading
	    	char* s1 = arr[left]->metadata[target];
	    	char* s2 = arr[right]->metadata[target];
	    	int cmp = strcmp(s1, s2);

	    	if(cmp <= 0)
	        	sorted[i] = arr[left++];
			else
	        	sorted[i] = arr[right++];	 

	 	}

	}
	
  	while (left <= mid)    
      sorted[i++] = arr[left++];

   	while (right <= high)   
      sorted[i++] = arr[right++];

   	for (i = low; i <= high; i++)
      arr[i] = sorted[i];

}

void mergeSort(int target, char det, group** rowArr, int low, int high) {

   if (low < high) {
      int mid = (low + high) / 2;

      mergeSort(target, det, rowArr, low, mid);
      mergeSort(target, det, rowArr, mid+1, high);

      merge(target, det, rowArr, low, high, mid);
   }

}
