How to use this program:

Flags (can be used in any arbitrary order): 
-c - title you wish to sort by [MANDATORY]
-d - absolute root directory path [OPTIONAL]
-o - NON-absolute output directory path [OPTIONAL]

Example: ./sorter -c genre
Example: ./sorter -c color -d /usr/root/ -o usr/result

Note: if -d and -o are not indicated, they are defaulted to current directory.
      If -o directory is not present, it is created