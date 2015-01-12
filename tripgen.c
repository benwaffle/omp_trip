#include <stddef.h>
#include <openssl/des.h>

/* buf, salt, ret */
void tripgen(char *pass, char *pass_html, char *salt, char *ret)
{
	/* the index we've reached in the HTML-replaced pass */
	size_t i_html = 0;
	for (size_t i = 0; pass[i] != '\0'; ++i) {
		switch (pass[i])
		{
			case '"':
				/*
				instead of writing the '"' to the HTML'd pass,
				write "&quot;"
				*/
				pass_html[i_html] = '&';
				pass_html[i_html + 1] = 'q';
				pass_html[i_html + 2] = 'u';
				pass_html[i_html + 3] = 'o';
				pass_html[i_html + 4] = 't';
				pass_html[i_html + 5] = ';';
				i_html += 6;
				break;
			case '&':
				pass_html[i_html] = '&';
				pass_html[i_html + 1] = 'a';
				pass_html[i_html + 2] = 'm';
				pass_html[i_html + 3] = 'p';
				pass_html[i_html + 4] = ';';
				i_html += 5;
				break;
			case '<':
				pass_html[i_html] = '&';
				pass_html[i_html + 1] = 'l';
				pass_html[i_html + 2] = 't';
				pass_html[i_html + 3] = ';';
				i_html += 4;
				break;
			case '>':
				pass_html[i_html] = '&';
				pass_html[i_html + 1] = 'g';
				pass_html[i_html + 2] = 't';
				pass_html[i_html + 3] = ';';
				i_html += 4;
				break;
			default:
				/* simply copy the character over */
				pass_html[i_html] = pass[i];
				++i_html;
				break;
		} 
	}

	if (pass_html[i_html - 1] == '#') {
		pass_html[i_html - 1] = '\0';
	} else {
		pass_html[i_html] = '\0';
	}

	switch (i_html)
	{
		/*
		essentially, start the salt at the second character in the pass,
		and append "H." until it is filled
		*/
		case 0:
		case 1:
			salt[0] = 'H';
			salt[1] = '.';
			break;
		case 2:
			salt[0] = pass_html[1];
			salt[1] = 'H';
			break;
		default:
			salt[0] = pass_html[1];
			salt[1] = pass_html[2];
			break;
	}
	salt[2] = '\0';

	/* translate certain characters */
	for (int i = 0; i < 2; ++i) {
		if (salt[i] < '.' || salt[i] > 'z') {
			salt[i] = '.';
		} else if (salt[i] <= '@' && salt[i] >= ':') {
			salt[i] += 7;
		} else if (salt[i] <= '`' && salt[i] >= '[') {
			salt[i] += 6;
		}
	}

	/* finally, hash the password */
	DES_fcrypt(pass_html, salt, ret);
}
