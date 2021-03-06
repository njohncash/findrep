/*
findrep: A tool to easily find and replace from files

Copyright (c) 2015 Arjun Sreedharan

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define VERSION "0.0.1"
#define MAX_COMMAND_LENGTH 2048
#define MAX_FILE_PATH_LENGTH 1024

static char *search_folder = ".";
static char command[MAX_COMMAND_LENGTH];
static char match[MAX_FILE_PATH_LENGTH];

static void usage(char **argv, int es)
{
	fprintf(es == EXIT_SUCCESS ? stdout : stderr,
		"usage: %s [-h] <FIND-PATTERN> <REPLACE-WITH> [LOCATION]\n",
		argv[0]);
	_exit(es);
}

static void no_match(char *argv1)
{
	printf("Found no instances of '%s'\n", argv1);
	_exit(EXIT_SUCCESS);
}

int main(int argc, char **argv)
{
	int rc, rc_each;
	FILE *fp_grep;
	char option;

	while ((option = getopt(argc, argv, "h")) != -1) {
		switch (option) {
		case 'h':
			fprintf(stdout, "A tool to easily find and replace from"
				" files.\n");
			usage(argv, EXIT_SUCCESS);
		}
	}

	if (system("command -v grep > /dev/null") &&
	system("command -v sed > /dev/null")) {
		fprintf(stderr, "grep and sed are required\n");
		_exit(EXIT_FAILURE);
	}

	if (system("command -v grep > /dev/null")) {
		fprintf(stderr, "grep is required\n");
		_exit(EXIT_FAILURE);
	}

	if (system("command -v sed > /dev/null")) {
		fprintf(stderr, "sed is required\n");
		_exit(EXIT_FAILURE);
	}

	if (argc < 3) {
		usage(argv, EXIT_FAILURE);
	}

	if (argc > 3 && *argv[3]) {
		search_folder = argv[3];
	}

	rc = 1;

	snprintf(command, sizeof command, "grep -rl '%s' %s",
		argv[1], search_folder);

	fp_grep = popen(command, "r");

	if (!fgets(match, sizeof match, fp_grep)) {
		fclose(fp_grep);
		no_match(argv[1]);
	}

	do {
		if (match[strlen(match) - 1] == '\n')
			match[strlen(match) - 1] = ' ';
		snprintf(command, sizeof command, "echo %s | xargs sed -i 's/%s/%s/g'",
			match, argv[1], argv[2]);
		rc_each = system(command);
		if (rc_each) {
			/* sed also prints its  error into stderr */
			fprintf(stderr, "ERROR when performing action on file '%s':\n",
				match);
		} else {
			rc = 0;
			printf("Replaced '%s' in: %s\n", argv[1], match);
		}

	} while(fgets(match, sizeof match, fp_grep));

	fclose(fp_grep);

	return rc;

}
