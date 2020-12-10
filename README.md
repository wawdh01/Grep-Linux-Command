# Grep-Linux-Command
Grep - Linux Command Utility 

  Grep is a linux command utility.
 The grep filter searches a file for a particular pattern of characters, and displays all lines that contain that pattern. 
 The pattern that is searched in the file is referred to as the regular expression (grep stands for globally search for regular expression and print out).
 
 To compile and linking this program , there is a makefile.
 
 
 > make
 
 
 
 //This will create all thr required files and do the proper linking of the files
 
 
 Syntax to run this program
 > ./grep [options] pattern [files]
 
 
The options that can be used with this grep program is as follows:
Options Description
-c : This prints only a count of the lines that match a pattern

-h : Display the matched lines, but do not display the filenames.

-i : Ignores, case for matching

-l : Displays list of a filenames only.

-v : This prints out all the lines that do not matches the pattern

-e exp : Specifies expression with this option. Can use multiple times.

-f file : Takes patterns from file, one per line.

-E : Treats pattern as an extended regular expression (ERE)

-w : Match whole word

-o : Print only the matched parts of a matching line,
 with each such part on a separate output line.
 
 
 Some Examples of grep command:
 
 > ./grep int main_grep.c list.c
 
 This will print the lines which have 'int' word in the file main_grep.c and list.c
 
 > ./grep -ic int main_grep.c
 
 This will count the number of lines and which contains 'int' word (-i is for ignorance of the uppercase and lowercase letters).
