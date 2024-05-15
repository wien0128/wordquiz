#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <tchar.h>
#include <ctype.h>

typedef 
	enum {
		C_ZERO,
		C_LIST,
		C_SHOW,
		C_TEST,
		C_EXIT,
	}
	command_t;

// strndup 함수 구현
char* strndup(const char* s, size_t n) {
	char* result;
	size_t len = strnlen(s, n);
	result = (char*)malloc(len + 1);
	if (result == NULL) {
		fprintf(stderr, "Error: Memory Allocation Fail\n\n");
		exit(EXIT_FAILURE);	// End program if allocation fails
	}
	memcpy(result, s, len);
	result[len] = '\0';
	return result;
}

char * read_a_line (FILE * fp)
{
	static char buf[BUFSIZ] ;
	static int buf_n = 0 ;
	static int curr = 0 ;

	if (feof(fp) && curr == buf_n - 1)
		return 0x0 ;

	char * s = 0x0 ;
	size_t s_len = 0 ;
	do {
		int end = curr ;
		while (!(end >= buf_n || !iscntrl(buf[end]))) {
			end++ ;
		}
		if (curr < end && s != 0x0) {
			curr = end ;
			break ;
		}
		curr = end ;
		while (!(end >= buf_n || iscntrl(buf[end]))) {
			end++ ;
		}
		if (curr < end) {
			if (s == 0x0) {
				s = strndup(buf + curr, end - curr) ;
				s_len = end - curr ;
			}
			else {
				char* new_s = realloc(s, s_len + end - curr + 1);
				if (new_s == NULL)
				{
					fprintf(stderr, "Error: Memory Allocation Fail\n\n");
					free(s);	// free existing allocated memory
					exit(EXIT_FAILURE);	// End program if allocation fails
				}
				s = new_s;
				strncat(s, buf + curr, end - curr) ;
				s_len = s_len + end - curr ;
			}
		}
		if (end < buf_n) {
			curr = end + 1 ;
			break ;
		}

		buf_n = fread(buf, 1, sizeof(buf), fp) ;
		curr = 0 ;
	} while (buf_n > 0) ;
	return s ;
}

void print_menu() {

	printf("1. List all wordbooks\n") ;
	printf("2. Show the words in a wordbook\n") ;
	printf("3. Test with a wordbook\n") ;
	printf("4. Exit\n") ;
}

int get_command() {
	int cmd;
	int res;

	while(1) 
	{
		printf(">");
		res = scanf("%d", &cmd);

		if (res != 1 || cmd > 4) 
		{
			fprintf(stderr, "Error: Invalid command. Please enter a number between 1 ~ 4.\n\n");
			while (getchar() != '\n'); // Remove invalid input
			print_menu();
			continue; // return to the begining of the loop
		}
		break;	// End the loop if it is a valid input
	}

	return cmd;
}

void list_wordbooks ()
{
	WIN32_FIND_DATA findFileData;
	HANDLE hFind = FindFirstFile("wordbooks\\*", &findFileData);

	if (hFind == INVALID_HANDLE_VALUE) {
		printf("\nNo wordbooks found.\n");
		fprintf(stderr, "Error: Unable to access the worbks directory.\n\n");
		return;
	}

	printf("\n  ----\n");
	do {
		const char* name = findFileData.cFileName;
		if (strcmp(name, ".") != 0 && strcmp(name, "..") != 0) {
			printf("  %s\n", name);
		}
	} while (FindNextFile(hFind, &findFileData) != 0);
	FindClose(hFind);

	printf("  ----\n");
}

void show_words ()
{
	char wordbook[128] ;
	char filepath[256] ;

	list_wordbooks() ;

	printf("Type in the name of the wordbook?\n") ;
	printf(">") ;
	scanf("%s", wordbook) ;

	sprintf(filepath, "wordbooks\\%s", wordbook) ;

	FILE * fp = fopen(filepath, "r") ;

	if (!fp) {
		printf("\nFailed to open wordbook %s\n", wordbook);
		fprintf(stderr, "Error: Failed to open file, %s\n\n", filepath);
		return;
	}

	printf("\n  -----\n") ;
	char * line ;
	while (line = read_a_line(fp)) {
		char * word = strtok(line, "\"") ;
		strtok(NULL, "\"") ;
		char * meaning = strtok(NULL, "\"") ;

		printf("  %s : %s\n", word, meaning) ;

		free(line) ;
	}
	printf("  -----\n\n") ;

	fclose(fp) ;
}

void run_test ()
{
	char wordbook[128] ;
	char filepath[256] ;

	printf("Type in the name of the wordbook?\n") ;
	printf(">") ;
	scanf("%s", wordbook) ;

	sprintf(filepath, "wordbooks\\%s", wordbook) ;

	FILE * fp = fopen(filepath, "r") ;

	if (!fp) {
		printf("\nFailed to open wordbook %s\n", wordbook);
		fprintf(stderr, "Error: Failed to open file, %s.\n\n", filepath);
		return;
	}

	printf("\n-----\n") ;

	int n_questions = 0 ;
	int n_correct = 0 ; 

	char * line ;
	while (line = read_a_line(fp)) {
		char * word = strtok(line, "\"") ;
		strtok(NULL, "\"") ;
		char * meaning = strtok(NULL, "\"") ;

		printf("Q. %s\n", meaning) ;
		printf("?  ") ;

		char answer[128] ;
		scanf("%s", answer) ;

		if (strcmp(answer, word) == 0) {
			printf("- correct\n") ;
			n_correct++ ;
		}
		else {
			printf("- wrong: %s\n", word) ;
		}

		n_questions++ ;
		free(line) ;
	}

	printf("(%d/%d)\n", n_correct, n_questions) ;

	printf("-----\n\n") ;

	fclose(fp) ;
}

int main ()
{
	printf(" *** Word Quiz *** \n\n") ;

	int cmd ;
	do {
		print_menu() ;

		cmd = get_command() ;
		switch (cmd) {
			case C_LIST : {
				list_wordbooks() ;
				break ;
			}

			case C_SHOW: {
				show_words() ;
				break ;
			}

			case C_TEST: {
				run_test() ;
				break ;
			}

			case C_EXIT: {
				return EXIT_SUCCESS ;
			}
		}
	}
	while (cmd != C_EXIT) ;

	return EXIT_SUCCESS ;
}
