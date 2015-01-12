#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <omp.h>

#define TOTAL_PASSES 0x278afe0f
#define TOTAL_LAST_CHARS 16
#define TRIP_START 3
#define TRIP_LEN 10
#define PASS_LEN 9
#define PASS_HTML_LEN 50
#define SALT_LEN 3
#define BUFFER_LEN 14

void tripgen(char *pass, char *pass_html, char *salt, char *buffer);

#define TOTAL_CHARS 95
#define CHAR_OFFSET 32

/*
essentially, convert a decimal integer to a base-94 number where 1 corresponds
with ' '
*/
void int2string(long long int decimal, char *pass)
{
	unsigned idx = 0;

	while (decimal > 0) {
		/* 95 possible characters, starting from ' ' */
		pass[idx] = decimal % TOTAL_CHARS + CHAR_OFFSET;
		decimal /= TOTAL_CHARS;
		++idx;
	}
}

int n = -1;
unsigned long *count;

void end(int sig){
	unsigned long sum = 0;
	for (int i = 0; i < n; ++i)
		sum += count[i];
	printf("%lu trips generated\n", sum);

	exit(0);
}

int main(int argc, char **argv) {
	char *search = "ben";
	if (argc > 1)
		search = argv[1];

	signal(SIGINT, end);

	char pass[PASS_LEN];
	char pass_html[PASS_HTML_LEN];
	char salt[SALT_LEN];
	char ret[BUFFER_LEN];

	n = omp_get_max_threads();
	count = calloc(n, sizeof(unsigned long));
	
	omp_set_num_threads(omp_get_num_procs());
	#pragma omp parallel for private(pass, pass_html, salt, ret)
	for (long long int i = 0; i < TOTAL_PASSES; ++i) {
		memset(pass, '\0', PASS_LEN);
		int2string(++i, pass);
		if (pass[0] == '#')
			continue;

		tripgen(pass, pass_html, salt, ret);

		if (strcasestr(ret+TRIP_START, search)) {
			printf("%s: %s\n", pass, ret+TRIP_START);
		}

		count[omp_get_thread_num()]++;
	}
}
