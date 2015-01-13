#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <inttypes.h>
#include <string.h>
#include <omp.h>

#define TOTAL_LAST_CHARS 16
#define TRIP_START 3
#define TRIP_LEN 10
#define PASS_LEN 9
#define PASS_HTML_LEN 50
#define SALT_LEN 3
#define BUFFER_LEN 14

void tripgen(char *pass, char *pass_html, char *salt, char *buffer);

/**************************************************************************/
#define TOTAL_CHARS 95
#define CHAR_OFFSET 32

/*
essentially, convert a decimal integer to a base-94 number where 1 corresponds
with ' '
*/
inline void int2string(long long int decimal, char *pass)
{
	for (unsigned i = 0; decimal > 0; ++i){
		/* 95 possible characters, starting from ' ' */
		pass[i] = decimal % TOTAL_CHARS + CHAR_OFFSET;
		decimal /= TOTAL_CHARS;
	}
}
/**************************************************************************/

double start;
uint64_t count = 0;

void sigint(int s){
	double end = omp_get_wtime();
	printf("%f trips/sec\n", count/(end-start));
	exit(0);
}

int main(int argc, char **argv) {
	char *search = "ben";
	if (argc > 1)
		search = argv[1];

	signal(SIGINT, sigint);

	char pass[PASS_LEN];
	char pass_html[PASS_HTML_LEN];
	char salt[SALT_LEN];
	char ret[BUFFER_LEN];
	
	start = omp_get_wtime();
	
#pragma omp parallel for private(pass,pass_html,ret) schedule(static, 1)
	for (uint64_t i = 0; i < UINT64_MAX; i++) {
		memset(pass, '\0', PASS_LEN);
		int2string(i, pass);
		if (pass[0] == '#')
			continue;
		tripgen(pass, pass_html, salt, ret);
		if (strcasestr(ret+TRIP_START, search))
			printf("%s: %s\n", pass, ret+TRIP_START);

		#pragma omp atomic
		count++;
	}

	printf("done\n");
}
