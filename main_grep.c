#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include "list.h"
#include <errno.h>
#include <regex.h>
#define _GNU_SOURCE 1
int ignorefile = 0;
char *myname;
int wholefile = 0;
int extended = 0;
int errors = 0;
regex_t pattern;
int ignore_case = 0;
int z;
int fc = 0;
int ew = 0;	
int indexi[100];
int ffc = 0;
int fw = 0;
int vcount = 0;
int wcount = 0;
int fwcount = 0;
int icount = 0;
int wvcount = 0;
int wicount = 0;
int ivwcount = 0;
int vicount = 0;
int fno = 2;
int max_count;
int fv = 0;
int fi = 0;
int dup_max_count;
int max_count1;
int max_count2;
void FileCheck (int, char**);
int CheckBinary (char []);
void listFilesRecursively(char *path);
void mismatch (char *line, char *temp);
int match (char *line, char *temp);
void wholeword (char*, char [], char*);
void ignore (char*, char*, char*);
void display (char*, char*);
void directory (list, char[]);
void compile_pattern(const char*);
void process (const char *name, FILE *fp);
void usage (void);
void RegexCheck (int, char**);
void FileGrep (int, char**);
void wholewordforfile (char*, char*, char*);
void extendedic (int, char**);
void displayic (char *, int);
void file_extended (int, char**);
list l;
int flg[13] = {0};
int main (int argc, char *argv[]) {
	z = argc;
	int total_line = 0;
	int c, fd, d;
	int i = 0, k = 0, j;
	int count;
	char filename[100];
	static int f = 0;
	char line[1000], temp[1000], ch;
	char *files;
	init (&l);
	if (argv[1][0] != '-') {
		int j = 2;
		while (argc != j) {
			add(&l, argv[j]);
			j++;
		}
		while (!isempty(&l)) {
			files = del (&l);
			fd = open (files, O_RDONLY);
			if (fd == -1) {
				printf("File %s.. Unable to open\n", files);
				continue;
			}
			strcpy(filename, files);
			while (1) {
				while (1) {
					d = read(fd, &ch, 1);
					if (ch == '\n' || d == 0) {
						line[i] = '\0';
						total_line++;
						i = 0;
						break;
					}
					line[i] = ch;
					i++;
				}
				if(strstr(line, argv[1]) != NULL) {
					if (argc > 3) {
						printf("\033[01;33m");
						printf("%s", filename);
						printf("\033[1;32m");
						printf(":");
						printf("\033[0m");
					}
					display (line, argv[1]);
				}
				if(d == 0) 
					break;
			}
		}
		exit(0);
	}
	while ((c = getopt(argc, argv, ":rivfwcmbqHhEe")) != -1) {
		switch (c) {
			case 'r':flg[0] = 1;
				 break;
			case 'i':flg[1] = 1;
				 break;
			case 'v':flg[2] = 1;
			         break;
			case 'f':flg[3] = 1;
			         break;
			case 'w':flg[4] = 1;
			         break;
			case 'c':flg[5] = 1;
				 break;
		    	case 'm':flg[6] = 1;
		        	 break;
		   	case 'b':flg[7] = 1;
		        	 break;
		    	case 'q':flg[8] = 1;
				 exit(1);
		      		 break;
		   	case 'H':flg[9] = 1;
		        	 break;
		   	case 'h':flg[10] = 1;
		        	 break;
		    	case 'E':flg[11] = 1;
		        	 break;
			case 'e':flg[12] = 1;
				 break;
			case '?':
				printf("Try 'grep --help' for more information\n");				
				printf("Invalid Option..\n");
				exit(0);
		}
	}
	if (flg[12]) {
		while (k < argc) {
			if (strcmp(argv[k], "-f") == 0) {
				file_extended (argc, argv);
				exit(0);
			}
			k++;
		}
	}
	else if (flg[3]){
		while ( k < argc) {
			if (strcmp(argv[k], "-e") == 0) {
				file_extended (argc, argv);
				exit(0);
			}
			k++;
		}
	}
	if (flg[0] == 1) {
		char path[100];
		if (flg[6] == 0) {
			if (argc == 3)
				strcpy(path, ".");
			else 
				strcpy(path, argv[3]);
		}
		else {
			if (argc == 4)
				strcpy(path, ".");
			else
				strcpy(path, argv[4]);
		}
    		listFilesRecursively(path);
	}
	if (flg[12] == 1) {
		extendedic (argc, argv);
	}
	if (flg[3] == 1)
		FileGrep (argc, argv);
	int r;
	if (flg[6] == 1) {
		r = 4;
		dup_max_count = atoi(argv[2]);
		max_count = dup_max_count;
		max_count1 = dup_max_count;
		max_count2 = dup_max_count;
		fno = 3;
	}
	else {
		r = 3;
	}
	if (flg[11] == 1) {
		RegexCheck (argc, argv);
	}
	if (flg[0] != 1) {
		if (flg[12] != 1) {	
			while (argc != r && flg[0] == 0) {
				add(&l, argv[r]);
				r++;
			}
		}
	}
	while (!isempty(&l)) {
		files = del (&l);
		int bin = CheckBinary (files);
		if (bin == 1) {
			continue;
		}
		if (flg[6] == 1) {
			max_count = dup_max_count;
			max_count1 = dup_max_count;
		}
		fc = 0;
		fv = 0;
		strcpy(filename, files);
		f = 0;
		fd = open(filename, O_RDONLY);
        while (1) {
		i = 0;
            	while (1) {
                	d =read(fd, &ch, 1);
                	if (ch == '\n' || d == 0) {
                    		line[i] = '\0';
		    		total_line++;
		    		i = 0;
                    		break;
                	}
                	line[i] = ch;
                	i++;
            	}
	    	if (d == 0) {
                	break;
            	}
            	if (flg[2] == 1 && flg[4] == 0 && flg[1] != 1) {
                	if (strstr(line, argv[fno]) == NULL) {
		    		if (flg[5] == 0) {
					if (((flg[10] == 0 && z > 4) || flg[9] == 1) && flg[5] == 0 || flg[0] == 1  && flg[10] == 0) {
					if (flg[6] == 1) {
						if (max_count != 0 && z > 5) {
							printf("\033[01;33m");
							printf("%s", filename);
							printf("\033[1;32m");
							printf(":");
							printf("\033[0m");
							max_count--;
						}
					}	
					if (flg[6] == 0) {
							printf("\033[01;33m");
							printf("%s", filename);
							printf("\033[1;32m");
							printf(":");
							printf("\033[0m");
					}	
			        }
				if (flg[7] == 1) {
					if (flg[6] == 1) {
						if (max_count != 0) {
							printf("\033[0;32m");
							printf("%d:", fv);
							printf("\033[0m");
						}
					}
					else {
						printf("\033[0;32m");
						printf("%d:", fv);
						printf("\033[0m");
					}
				}	
				if (flg[5] == 0) { 
             				if (flg[6] == 1) {
						if (max_count1 != 0 && z > 5) {
							printf("%s\n", line);
							max_count1--;
						}
					}	
					if (flg[6] == 0) {
						printf("%s\n", line);
					}			
				}
				}	
		   		else
					icount++;
                	}
			fv = fv + strlen(line) + 1;
            	}
            	if (flg[5] == 1) {
                	if (strstr(line, argv[fno]) != NULL) {
                   		k++;
                	}
            	}
            	if (flg[1] == 1 && flg[4] == 0) {
                	strcpy(temp, argv[fno]);
                	ignore (line, temp, filename);
            	} 
            	if (flg[4] == 1) {
			wholeword (line, argv[fno], filename);
	    	}	
	    	if (strcmp(argv[1], "-b") == 0) {
			if (strstr(line, argv[fno]) != NULL) {
				if ((z > 4 && flg[10] != 1) || flg[9] == 1 || flg[0] == 1) {
					if (flg[6] == 1) {
						if ( z > 5) {
							if (max_count != 0) {
								printf("\033[01;33m");
								printf("%s", filename);
								printf("\033[1;32m");
								printf(":");
								printf("\033[0m");
								max_count--;
							}
						}
					}
					else {
						printf("\033[01;33m");
						printf("%s", filename);
						printf("\033[1;32m");
						printf(":");
						printf("\033[0m");
					}
				}
				if (flg[6] == 1) {
					if (max_count1 != 0) {
						printf("\033[0;32m");
						printf("%d", f);
						printf("\033[0;34m");
						printf(":");
						printf("\033[0m");
						display (line, argv[fno]);
						max_count1--;
					}
				}
				else {
					printf("\033[0;32m");
					printf("%d", f);
					printf("\033[0;34m");
					printf(":");
					printf("\033[0m");
					display (line, argv[fno]);
				}
	   		}
			f = f + strlen(line) + 1;
	    	}
	   	if (strcmp(argv[1], "-h") == 0) {
			if(strstr(line, argv[fno]) != NULL) 
				display (line, argv[fno]);
	    	}
	   	if (strcmp(argv[1], "-H") == 0) {
			if(strstr(line, argv[fno]) != NULL) {
				printf("\033[01;33m");
				printf("%s", filename);
				printf("\033[1;32m");
				printf(":");
				printf("\033[0m");
				display (line, argv[fno]);
			}
	    	}
	    	if (strcmp(argv[1], "-r") == 0) {
			if(strstr(line, argv[fno]) != NULL) {
				if (((flg[10] == 0 && z > 4) || flg[9] == 1) && flg[5] == 0 || flg[0] == 1  && flg[10] == 0) {
					if (flg[6] == 1) {
						if (max_count != 0 && z > 5) {
							printf("\033[01;33m");
							printf("%s", filename);
							printf("\033[1;32m");
							printf(":");
							printf("\033[0m");
						}
					}	
					if (flg[6] == 0) {
							printf("\033[01;33m");
							printf("%s", filename);
							printf("\033[1;32m");
							printf(":");
							printf("\033[0m");
					}	
			        }
				if (flg[5] == 0) { 
             				if (flg[6] == 1) {
						if (max_count != 0) {
							display(line, argv[fno]);
							max_count--;
						}
					}	
					if (flg[6] == 0) {
						display(line, argv[fno]);
					}			
				}
			}
	    	}	                		
	}
     	if (flg[5] == 1) {
		if ((flg[10] == 0 && z > 4) || flg[9] == 1 || flg[0] == 1) {
			printf("\033[01;33m");
			printf("%s", filename);
			printf("\033[1;32m");
			printf(":");
			printf("\033[0m");
		}	
		if (flg[1] && flg[2] && flg[4] ){
			printf("%d\n", ivwcount);
			ivwcount = 0;
		}
		else if (flg[1] && flg[2]) {
			printf("%d\n", vicount);
			vicount = 0;
		}
		else if (flg[4] && flg[1]) {
			printf("%d\n", wicount);
			wicount = 0;
		}
		else if (flg[4] && flg[2]) {
			printf("%d\n", wvcount);
			wvcount = 0;
		}
		else if(flg[1] == 1) {
			printf("%d\n", vcount);
			vcount = 0;
		}
		else if(flg[4] == 1) {
			printf("%d\n", wcount);
			wcount = 0;
		}
		else if(flg[2] == 1) {
			printf("%d\n", icount);
			icount = 0;
		}
		else {
        		printf("%d\n", k);
			k = 0;
		}
		total_line = 0;
     	}
	}
	return 0;
}
void wholeword (char *line, char temp[100], char *filename) {
	int k = 0, j = 0, checkv = 0;
	char cpy[1000];
	char temp1[1000];
	strcpy(cpy, line);
	char *str = strtok(line, " 	");
	while (str != NULL) {
		while (str[k] != '\0') {
			if ((str[k] > 64 && str[k] < 91) || (str[k] < 123 && str[k] > 96)) {
				temp1[j] = str[k];
				j++;
			}
			k++;
		}
		temp1[j] = '\0';
		if (flg[1] == 0) {
		if (strcmp(temp1, temp) == 0) {
			if (flg[2] == 1) {
				checkv = 1;
				break;
			}
			if (flg[5] == 0) {
				wholefile = 1;
				if (((flg[10] == 0 && z > 4) || flg[9] == 1) && flg[5] == 0 || flg[0] == 1  && flg[10] == 0) {
					if (flg[6] == 1) {
						if (max_count != 0 && z > 5) {
							printf("\033[01;33m");
							printf("%s", filename);
							printf("\033[1;32m");
							printf(":");
							printf("\033[0m");
						}
					}	
					if (flg[6] == 0) {
							printf("\033[01;33m");
							printf("%s", filename);
							printf("\033[1;32m");
							printf(":");
							printf("\033[0m");
					}	
			        }
				if (flg[7] == 1) {
					if (flg[6] == 1) {
						if (max_count != 0) {
							printf("\033[0;32m");
							printf("%d:", fc);
							printf("\033[0m");
						}
					}
					else {
						printf("\033[0;32m");
						printf("%d:", fc);
						printf("\033[0m");
					}
				}	
				if (flg[5] == 0) { 
             				if (flg[6] == 1) {
						if (max_count != 0 && z > 5) {
							display(cpy, temp);
							max_count--;
						}
					}	
					if (flg[6] == 0) {
						display(cpy, temp);
					}			
				}
			}
			else
				wcount++;
			break;
		}
		}
		if (flg[1] == 1) {
		if (strcasecmp(temp1, temp) == 0) {
			if (flg[2] == 1) {
				checkv = 1;
				break;
			}
			//printf("%s\n",cpy);
			if (flg[5] == 0) {
				if (((flg[10] == 0 && z > 4) || flg[9] == 1) && flg[5] == 0 || flg[0] == 1  && flg[10] == 0) {
					if (flg[6] == 1) {
						if (max_count != 0 && z > 5) {
							printf("\033[01;33m");
							printf("%s", filename);
							printf("\033[1;32m");
							printf(":");
							printf("\033[0m");
						}
					}	
					if (flg[6] == 0) {
							printf("\033[01;33m");
							printf("%s", filename);
							printf("\033[1;32m");
							printf(":");
							printf("\033[0m");
					}	
			        }
				if (flg[7] == 1) {
					printf("\033[0;32m");
					printf("%d:", fc);
					printf("\033[0m");
				}
				if (flg[5] == 0) { 
             				if (flg[6] == 1) {
						if (max_count != 0 && z > 5) {
							display(cpy, temp);
							max_count--;
						}
					}	
					if (flg[6] == 0) {
						display(cpy, temp);
					}			
				}
			}
			else
				wicount++;
			break;
		}
		}
		str = strtok(NULL, " 	");
		//printf("%s\n", str);
		k = 0;
		j = 0;
	}
	if (checkv == 0 && flg[2] == 1 && flg[1] != 1) {
		if (flg[5] == 0) {
			if (((flg[10] == 0 && z > 4) || flg[9] == 1) && flg[5] == 0 || flg[0] == 1 && flg[10] == 0) {
					if (flg[6] == 1) {
						if (max_count != 0 && z > 5) {
							printf("\033[01;33m");
							printf("%s", filename);
							printf("\033[1;32m");
							printf(":");
							printf("\033[0m");
						}
					}	
					if (flg[6] == 0) {
							printf("\033[01;33m");
							printf("%s", filename);
							printf("\033[1;32m");
							printf(":");
							printf("\033[0m");
					}	
			        }
				if (flg[7] == 1) {
					if (flg[6] == 1) {
						if (max_count != 0) {
							printf("\033[0;32m");
							printf("%d:", fc);
							printf("\033[0m");
						}
					}
					else {
						printf("\033[0;32m");
						printf("%d:", fc);
						printf("\033[0m");
					}
				}	
				if (flg[5] == 0) { 
             				if (flg[6] == 1) {
						if (max_count != 0) {
							printf("%s\n", cpy);
							max_count--;
						}
					}	
					if (flg[6] == 0) {
						printf("%s\n", cpy);
					}			
				}
		}
		else
			wvcount++;
	}	
	if (checkv == 0 && flg[2] == 1 && flg[1] == 1) {
		if (flg[5] == 0) {
			if (((flg[10] == 0 && z > 4) || flg[9] == 1) && flg[5] == 0 || flg[0] == 1 && flg[10] == 0) {
					if (flg[6] == 1) {
						if (max_count != 0 && z > 5) {
							printf("\033[01;33m");
							printf("%s", filename);
							printf("\033[1;32m");
							printf(":");
							printf("\033[0m");
						}
					}	
					if (flg[6] == 0) {
							printf("\033[01;33m");
							printf("%s", filename);
							printf("\033[1;32m");
							printf(":");
							printf("\033[0m");
					}	
			        }
				if (flg[7] == 1) {
					if (flg[6] == 1) {
						if (max_count != 0) {
							printf("\033[0;32m");
							printf("%d:", fc);
							printf("\033[0m");
						}
					}
					else {
						printf("\033[0;32m");
						printf("%d:", fc);
						printf("\033[0m");
					}
				}
				if (flg[5] == 0) { 
             				if (flg[6] == 1) {
						if (max_count != 0) {
							printf("%s\n", cpy);
							max_count--;
						}
					}	
					if (flg[6] == 0) {
						printf("%s\n", cpy);
					}			
				}
		}
		else
			ivwcount++;
	}
	fc = fc + strlen(cpy) + 1;	
	//printf("%s\n", line);
	return;
}
void ignore (char *line, char *temp, char *filename) {
	int i = 0, j = 0, space = 0;
	int checkiv = 0;
	if (flg[2] != 1) {
		while (line[i] != '\0') {
			if (line[i] == 32)
				space++;
			i++;
		}
		if (space == strlen(line))
			return;
	}
	i = 0;
     	while (1) {
		if ((line[i] == temp[0] || line[i]-temp[0] == 32 || temp[0] - line[i] == 32) && line[i] != '\0') {
                	j = 0;
                	while ((line[i + j] == temp[j] || line[i + j] - temp[j] == 32 || temp[j] - line[i + j] == 32) && line[i + j] != '\0') {
                    		j++;
				if (temp[j] == '\0')
					break;
                	}
			if (j == strlen(temp)) {
				if (flg[1] == 1 && flg[3] == 1 && flg[4] == 1) {
					if (((line[i - 1] >= 0 && line[i - 1] < 65) || (line[i - 1] > 90 && line[i - 1] < 97)) && ((line[i + j] >= 0 && line[i + j] < 65) || (line[i + j] > 90 && line[i + j] < 97))) {
						indexi[fi++] = i;
						indexi[fi++] = i + j;
					}
					return;
				}
				if (flg[2] == 1) {
					checkiv = 1;
					break;
				}
				if (flg[3] == 1) {
					indexi[fi++] = i;
					indexi[fi++] = i + j;
					return;
				}
				if (((flg[10] == 0 && z > 4) || flg[9] == 1) && flg[5] == 0 || flg[0] == 1 && flg[10] == 0 && flg[2] != 1) {
					ignorefile = 1;
					if (flg[6] == 1 && flg[5] != 1) {
						if (max_count != 0 && z > 5) {
							printf("\033[01;33m");
							printf("%s", filename);
							printf("\033[1;32m");
							printf(":");
							printf("\033[0m");
						}
					}	
					if (flg[6] == 0 && flg[5] != 1) {
							printf("\033[01;33m");
							printf("%s", filename);
							printf("\033[1;32m");
							printf(":");
							printf("\033[0m");
					}	
			        }
				if (flg[7] == 1) {
					if (flg[6] == 1) {
						if (max_count != 0) {
							printf("\033[0;32m");
							printf("%d:", fc);
							printf("\033[0m");
						}
					}
					else {
						printf("\033[0;32m");
						printf("%d:", fc);
						printf("\033[0m");
					}
				}
				if (flg[5] == 0) { 
             				if (flg[6] == 1) {
						if (max_count != 0) {
							display (line, temp);
							max_count--;
						}
					}	
					if (flg[6] == 0) {
						display(line, temp);
					}			
				}
				else
					vcount++;
				break;
        		}
		}
		i++;
		if (line[i] == '\0') 
			break;
	}
	if (checkiv == 0 && flg[2] == 1 && flg[3] != 1) {
		if (flg[5] != 1) {
			if (flg[6] == 1) {
				if (max_count != 0) {
					if ((z > 4 && flg[10] == 1) || flg[9] == 1 || flg[0] == 1) {
						printf("\033[01;33m");
						printf("%s", filename);
						printf("\033[1;32m");
						printf(":");
						printf("\033[0m");
					}
					printf("%s\n", line);
					max_count--;
				}
			}
			else {
				if ((z > 4 && flg[10] == 1) || flg[9] == 1 || flg[0] == 1) {
					printf("\033[01;33m");
					printf("%s", filename);
					printf("\033[1;32m");
					printf(":");
					printf("\033[0m");
		  		}
				printf("%s\n", line);
			}
		}
		else {
			if (flg[6] == 1) {
				if (max_count != 0) {
					vicount++;
					max_count--;
				}
			}
			else {
				vicount++;
			}
		}
	}
	fc = fc + strlen(line) + 1;
}
void listFilesRecursively(char *basePath)
{
	char path[1000];
    	struct dirent *dp;
    	DIR *dir = opendir(basePath);
    	if (!dir)
        	return;
    	while ((dp = readdir(dir)) != NULL) {
        	if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0) {
			if (strcmp(basePath, ".") != 0) {
            			strcpy(path, basePath);
            			strcat(path, "/");
            			strcat(path, dp->d_name);
			}
			else {
	    			strcpy(path, dp->d_name);
			}
	    		int filec = 0;
	    		while (path[filec] != '\0') {
				if (path[filec] == '.') 
					add (&l, path);
	        		filec++;
	    		}
            		listFilesRecursively(path);
        	}
    	}
	closedir(dir);
}
int CheckBinary (char str[]) {
	int fd, flag;
	char ch;
	fd = open (str, O_RDONLY);
	if (fd == -1) {
		printf("Unable to open file\n");
		return 1;
	}
	while (read(fd, &ch, 1)) {
		if (ch < 128 && ch >= 0) {
			flag = 0;
		}
		else {
			flag = 1;
			if (flg[5] != 1) {
				printf("Binary file %s matches..\n", str);
			}
			return flag;
			break;
		}
	}
}
void compile_pattern(const char *pat)
{
	int flags = REG_NOSUB;
	int ret;
	#define MSGBUFSIZE	512
	char error[MSGBUFSIZE];
	if (ignore_case)
		flags |= REG_ICASE;
	if (extended)
		flags |= REG_EXTENDED;

	ret = regcomp(& pattern, pat, flags);
	if (ret != 0) {
		(void) regerror(ret, & pattern, error, sizeof error);
		fprintf(stderr, "%s: pattern `%s': %s\n", myname, pat, error);
		errors++;
	}
}
void process(const char *name, FILE *fp)
{
	char *buf = NULL;
	size_t size = 0;
	char error[MSGBUFSIZE];
	int ret;
	while (getline(& buf, &size, fp) != -1) {
		ret = regexec(& pattern, buf, 0, NULL, 0);
		if (ret != 0) {
			if (ret != REG_NOMATCH) {
				(void) regerror(ret, & pattern, error, sizeof error);
				fprintf(stderr, "%s: file %s: %s\n", myname, name, error);
				free(buf);
				errors++;
				return;
			}
		}
		else {
			//printf("%s: %s", name, buf);	/* print matching lines */
			if (((flg[10] == 0 && z > 4) || flg[9] == 1) && flg[5] == 0 || flg[0] == 1 && flg[10] == 0 && flg[2] != 1) {
					if (flg[6] == 1 && flg[5] != 1) {
						if (max_count != 0) {
							printf("\033[01;33m");
							printf("%s", name);
							printf("\033[1;32m");
							printf(":");
							printf("\033[0m");
						}
					}	
					if (flg[6] == 0 && flg[5] != 1) {
							printf("\033[01;33m");
							printf("%s", name);
							printf("\033[1;32m");
							printf(":");
							printf("\033[0m");
					}	
			        }
				if (flg[5] == 0) { 
             				if (flg[6] == 1) {
						if (max_count != 0) {
							printf("%s", buf);
							max_count--;
						}
					}	
					if (flg[6] == 0) {
						printf("%s", buf);
					}			
				}
				else {
					if (flg[6] == 1) {
						if (max_count != 0) {
							extended++;
							max_count--;
						}
					}
					if (flg[6] != 1)
						extended++;
				}		
		}
	}
	free(buf);
}

/* usage --- print usage message and exit */

void usage(void)
{
	fprintf(stderr, "usage: %s [-i] [-E] pattern [ files ... ]\n", myname);
	exit(1);
}
void RegexCheck (int argc, char **argv) {
	int c;
	int i;
	FILE *fp;
	int fcount;
	char *files;
	int optindc;
	if (flg[6] == 1) {
		optindc = optind + 1;
	}
	else {
		optindc = optind;
	}
	if (flg[0] != 1) {
		if (flg[6] == 1)
			fcount = 4;
		else
			fcount = 3;
		while (fcount != argc) {
			add (&l, argv[fcount]);
			fcount++;
		}
	}
	myname = argv[0];
	while ((c = getopt(argc, argv, ":iE")) != -1) {
		switch (c) {
		case 'i':
			ignore_case = 1;
			break;
		case 'E':
			extended = 1;
			break;
		case '?':
			usage();
			break;
		}
	}
	if (optindc == argc)		/* sanity check */
		usage();
	if (flg[6] != 1)
		compile_pattern(argv[optindc]);	/* compile the pattern */
	else
		compile_pattern(argv[optindc]);
	if (errors)			/* compile failed */
		return;
	else
		optindc++;

	if (optindc == argc)		/* no files, default to stdin */
		process("standard input", stdin);
	else {
		/* loop over files */
		while (!isempty(&l)) {
			if (flg[6] == 1)
				max_count = dup_max_count;
			files = del (&l);
			if(CheckBinary(files))
				continue;
			if (strcmp(argv[1], "-") == 0)
				process("standard input", stdin);
			else if ((fp = fopen(files, "r")) != NULL) {
				process(files, fp);
				if (flg[5] == 1) {
					printf("\033[01;33m");
					printf("%s", files);
					printf("\033[1;32m");
					printf(":");
					printf("\033[0m");
					printf("%d\n", extended);
					extended = 0;
				}
				fclose(fp);
			} else {
				fprintf(stderr, "%s: %s: could not open: %s\n",
					argv[0], files, strerror(errno));
				errors++;
			}
		}
	}
	regfree(& pattern);
	exit(0);
	//return errors != 0;
}
void FileGrep (int argc, char *argv[]) {
	char *files;
	int form = 2, i = 0;
	int argccount, c, d, k = 0;
	char dualarray[100][100];
	if (flg[6] == 1)
		form = 3;
	if (argc < 4 && flg[0] != 1) {
		printf("Insufficient Arguments \n");
		exit(0);
	}
	int fdpattern, fd;
	char ch;
	char *str;
	int rn = 0;
	char linepattern[1000];
	char line[1000];
	fdpattern = open (argv[form], O_RDONLY);
	if (fdpattern == -1) {
		printf("Unable to open Pattern file\n");
		exit(0);
	}
	while (1) {
		while (1) {
			c = read(fdpattern, &ch, 1);
			if (ch == '\n' || c == 0) {
				line[i] = '\0';
				i = 0;
				break;
			}
			line[i] = ch;
			i++;
		}
		if (c == 0)
			break;
		strcpy(dualarray[k], line);
		k++;
	}
	int filepatterncount = k;
	k = 0;
	int h = 0;
	argccount = 3;
	if (flg[6] == 1) {
		argccount = 4;
	}
	if (flg[0] != 1) {
		while (argccount != argc) {
			add (&l, argv[argccount]);
			argccount++;
		}
	}
		while (!isempty(&l)) {
			files = del (&l);
			if (CheckBinary(files)) {
				continue;
			}
			if (flg[6] == 1)
				max_count = atoi(argv[2]);
			ffc = 0;
			fwcount = 0;
			fd = open(files, O_RDONLY);
			if (fd == -1) {
				printf("Unable to open File\n");
				continue;
			}
			while (1) {
				while (1) {
					c = read(fd, &ch, 1);
					if (ch == '\n') {
						line[i] = '\0';
						i = 0;
						break;
	
					}
					line[i] = ch;
					i++;
				}
				if (c == 0)
					break;
				i = 0;
				k = 0;
				if (flg[1] == 1) {
					while (k < filepatterncount) {
						ignore (line, dualarray[k], files);
						k++;
					}
					if (fi > 0) {
						if (((flg[5] != 1 && z > 4 && flg[10] == 0) || flg[9] == 1 || flg[0] == 1) && flg[2] != 1) {
							printf("\033[01;33m");
							printf("%s", files);
							printf("\033[1;32m");
							printf(":");
							printf("\033[0m");
						}
							displayic (line, fi - 1);
					}
					else if (flg[2] == 1 && fi == 0) {
						printf("%s\n", line);
					}
					fi = 0;		
				}
				else if (flg[4] == 1 && flg[1] != 1) {
					while (k < filepatterncount) {
						wholewordforfile (line, dualarray[k], files);
						k++;
					}
					if (fw > 0) {
						if ((flg[5] != 1 && z > 4 && flg[10] == 0) || flg[9] == 1 || flg[0] == 1) {
							printf("\033[01;33m");
							printf("%s", files);
							printf("\033[1;32m");
							printf(":");
							printf("\033[0m");
						}
						displayic (line, fw - 1);
						fw = 0;
					}
				}
				else {
					while (k < filepatterncount ) {
						if ((str = strstr(line, dualarray[k])) != NULL) {
							h = strlen(str);
							h = strlen(line) - h;
							indexi[rn] = h;
							rn++;
							indexi[rn] = h + strlen(dualarray[k]);
							rn++;
						}
						k++;
					}
					if (rn > 0) {		
						if ((flg[5] != 1 && z > 4 && flg[10] == 0) || flg[9] == 1 || flg[0] == 1) {
							printf("\033[01;33m");
							printf("%s", files);
							printf("\033[1;32m");
							printf(":");
							printf("\033[0m");
						}
						displayic (line, rn - 1);
					}
					rn = 0;
				}
				i = 0;
				while (i < 100) {
					indexi[i] = 0;
					i++;
				}
				i = 0;
		}	
		if (flg[5] == 1) {
			if (flg[4] == 1) {
				printf("hi = %d\n", fwcount);
			}
		}
	}
	exit(0);
	return;
}
void display (char *line, char *temp) {
	int number[10];
	int k = 0, i = 0;
	while (line[i] != '\0') {
		if ((line[i] == temp[0] || line[i] == temp[0] + 32 || line[i] == temp[0] - 32) && line[i] != '\0') {
			int j = 0;
			while ((line[i + j] == temp[j] || line[i + j] == temp[j] + 32 || line[i + j] == temp[j] - 32) && temp[j] != '\0') {
				j++;
			}
			if (j == strlen(temp)) {
				 number[k] = i;
				 k++;
			}
		}
		i++;
	}
	i = 0;
	while (line[i] != '\0') {
		int dup_k = k;
		while (k) {
			int y = number[dup_k - k];
			while (i < (y)) {
				printf("%c", line[i]);
				i++;
			}
			int strc = strlen(temp);
			printf("\033[1;31m");
			while (strc) {
				int kite = 1;
				printf("%c", line[i]);
				i++;
				strc--;
			}
			printf("\033[0m");
			k--;
		}
		printf("%c", line[i]);
		i++;
	}
	printf("\n");
}
void wholewordforfile (char *line, char *temp, char *filename) {
	int i = 0, j = 0;
	char *str;
	int k = strlen(line);
	if ((str = strstr(line, temp)) != NULL) {
		j = strlen(str);
		j = k - j;
		if (((line[j - 1] >= 0 && line[j - 1] < 65) || (line[j - 1] > 90 && line[j - 1] < 97)) && ((line[j + strlen(temp)] >= 0 && line[j + strlen(temp)] < 65) || (line[j + strlen(temp)] > 90 && line[j + strlen(temp)] < 97))) {
			if (flg[12] == 1) {		
				indexi[ew++] = j;
				indexi[ew++] = j+ strlen(temp);
				return;
			}
			if (flg[3] == 1) {
				indexi[fw++] = j;
				indexi[fw++] = j + strlen(temp);
				return;
			}
			if (((flg[10] == 0 && z > 4) || flg[9] == 1) && flg[5] == 0 || flg[0] == 1 && flg[10] == 0 && flg[2] != 1 && flg[11] != 1) {
				if (flg[6] == 1 && flg[5] != 1) {
					wholefile = 1;
					if (max_count != 0 && z > 5) {
						printf("\033[01;33m");
						printf("%s", filename);
						printf("\033[1;32m");
						printf(":");
						printf("\033[0m");
						}
				}	
				if (flg[6] == 0 && flg[5] != 1) {
					printf("\033[01;33m");
					printf("%s", filename);
					printf("\033[1;32m");
					printf(":");
					printf("\033[0m");
				}
			}
			if (flg[7] == 1) {
				if (flg[6] == 1) {
					if (max_count != 0) {
						printf("\033[0;32m");
						printf("%d:", ffc);
						printf("\033[0m");
					}
				}
				else {
					printf("\033[0;32m");
					printf("%d:", ffc);
					printf("\033[0m");
				}
			}
			if (flg[6] == 1) {
				if (max_count != 0) {
					if (flg[5] == 0)
						printf("%s\n", line);
					else
						fwcount++;
					max_count--;
				}
			}	
			if (flg[6] == 0) {
				if (flg[5] == 0)
					printf("%s\n", line);
				else
					fwcount++;
			}		
		}
	}
	ffc = ffc + k + 1;
}
void extendedic (int argc, char *argv[]) {
	char dualarray[100][20];
	char line[100];
	char *str;
	char *filesext;
	char ch;
	int j = 0, i = 0, k = 0;
	int cdtech = 0;
	while (k < argc) {
		if(strcmp(argv[k], "-e") == 0 || strcmp(argv[k], "-ev") == 0 || strcmp(argv[k], "-ve") == 0 || strcmp(argv[1], "-we") == 0 || strcmp(argv[k], "-ew") == 0 ) {
			j++;
		}
		k++;
	}
	int dup_i = j;
	k = 0;
	int filesidentity = 2 * j + 1;
	while ((j + k) < argc - 1) {
		strcpy(dualarray[k], argv[j + k + 1]);
		k++;
	}
	while (filesidentity < argc) {
		add (&l, argv[filesidentity]);
		filesidentity++;
	}
	int fd, h, rn = 0;
	int c;
	i = 0;
	while (!isempty(&l)) {
		filesext = del(&l);
		if (CheckBinary(filesext))
			continue;
	fd = open(filesext, O_RDONLY);
	if (fd == -1) {
		printf("Unable to open file\n");
		exit(0);
	}
	while (1) {
		while (1) {
			c = read(fd, &ch, 1);
			if (ch == '\n' || c == 0) {
				line[i] = '\0';
				i = 0;
				break;
			}
			line[i] = ch;
			i++;
		}
		if (c == 0)
			break;
		k = 0;
		cdtech = 0;
		if (strcmp(argv[1], "-ev") == 0 || strcmp(argv[1], "-ve") == 0) {
			while (k < dup_i) {
				if ((strstr(line, dualarray[k])) != NULL) {
					cdtech++;
				}
				k++;
			}
			if (cdtech == 0) {
				printf("%s\n", line);
			}
		}
		else if (strcmp(argv[1], "-ew") == 0 || strcmp(argv[1], "-we") == 0) {
			while (k < dup_i) {
				wholewordforfile(line, dualarray[k], argv[argc - 1]);
				k++;
			}
			if (ew != 0) {
				displayic (line, ew - 1);
			}
			ew = 0;
		}
		else {
			while (k < dup_i) {
				if ((str = strstr(line, dualarray[k])) != NULL) {
					h = strlen(str);
					h = strlen(line) - h;
					indexi[rn] = h;
					rn++;
					indexi[rn] = h + strlen(dualarray[k]);
					rn++;
				}
				k++;
			}
		}
		if (rn != 0) {
			displayic (line, rn - 1);
		}
		i = 0;
		while (i < 100) {
			indexi[i] = 0;
			i++;
		}
		i = 0;
		rn = 0;
		k = 0;
	}
	}
	exit(0);
}
void displayic (char *line, int rn) {
	int i = 0;
	int j = 0;
	int k = 0;
	while (line[i] != '\0') {
		if (i < indexi[j]) {
			printf("%c", line[i]);
		}
		else {
			if (j >= rn) {
				printf("%c", line[i]);
			}
		}
		if (i == indexi[j]) {
			k = 1;
			while (i < indexi [j + 1]) {
				printf("\033[1;31m");
				printf("%c", line[i]);
				printf("\033[0m");
				i++;
			}
			j = j + 2;
		}
		if (k != 1)
			i++;
		else
			k = 0;
	}
	printf("\n");
	return;
}
void file_extended (int argc, char *argv[]) {
	int k = 0, j = 0, fd, c, i = 0, ef = 0, extended_count = 0, h = 0;
	char ch;
	char line[100];
	char *str;
	list l1;
	char *files;
	init (&l1);
	char dualarray[100][100];
	while ( k < argc) {
		if (strcmp(argv[k], "-e") == 0) {
			strcpy(dualarray[j], argv[k + optind - 1]);
			ef++;
			j++;
		}
		if (strcmp(argv[k], "-f") == 0) {
			add (&l1, argv[k + optind - 1]);
			ef++;
		}
		k++;
	}
	while (!isempty(&l1)) {
		files = del (&l1);
		if (CheckBinary(files))
			continue;
		fd = open (files, O_RDONLY);
		while (1) {
			while (1) {
				c = read (fd, &ch, 1);
				if (ch == '\n' || c == 0) {
					line[i] = '\0';
					i = 0;
					break;
				}
				line[i] = ch;
				i++;
			}
			if ( c == 0)
				break;
			strcpy(dualarray[j++], line);
		}
	}
	ef = 2 * ef + 1;
	while (ef < argc) {
		add (&l, argv[ef]);
		ef++;
	}
	while (!isempty(&l)) {
		files = del(&l);
		fd = open (files, O_RDONLY);
		if (CheckBinary(files))
			continue;
		while (1) {
			while (1) {
				c = read(fd, &ch, 1);
				if (ch == '\n' || c == 0) {
					line[i] = '\0';
					i = 0;
					break;
				}
				line[i] = ch;
				i++;
			}
			if (c == 0)
				break;
			int k = 0;
			extended_count = 0;
			while (k < j - 1) {
				if((str = strstr(line, dualarray[k])) != NULL) {
					h = strlen(str);
					h = strlen(line) - h;
					indexi[extended_count++] = h;
					indexi[extended_count++] = h + strlen(dualarray[k]);
				}
				k++;
			}
			if (extended_count != 0)
				displayic (line, extended_count - 1);
			i = 0;
			while (i < 99) {
				indexi[i] = 0;
				i++;
			}
			i = 0;
		}
	}
	return;
}
