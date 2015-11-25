#include <stdio.h>
#include <ctype.h>

int currentline = 0;

// this misses the first character from the parsed word
char *getword1(FILE *fp, char *s)
{
	int	c;

	while ((c = fgetc(fp) != EOF && isspace(c)))
		if (c == '\n')
			currentline++;
	while ((c = fgetc(fp) != EOF && !isspace(c)))
		*s++ = c;
	
	return s;
}

// do while loops are awkward
char *getword2(FILE *fp, char *s)
{
	int c;

	c = fgetc(fp);

	do
	{
		if (c == '\n')
			currentline++;
	}
	while ((c != EOF) && isspace(c) && (c = fgetc(fp)));

	do
	{
		*s++ = c;
	}
	while ((c != EOF) && !isspace(c) && (c = fgetc(fp)));

	*s++ = 0;

	return s;
}

// get the character, if it passes the test get the next character, otherwise fall through to the next test
int getword3(FILE *fp, char *s)
{
	int	c;

	c = fgetc(fp);
	for (; c != EOF && isspace(c); c = fgetc(fp))
		if (c == '\n')
			currentline++;
	int i = 0;
	for (; c != EOF && !isspace(c); c = fgetc(fp))
		s[i++] = c;
	
	s[i] = 0;

	return i;
}

int accept1(const char *s)
{
	if (s[0] != 'i') return 0;
	if (s[1] != 'n') return 0;
	if (s[2] != 'c') return 0;
	if (s[3] != 'l') return 0;
	if (s[4] != 'u') return 0;
	if (s[5] != 'd') return 0;
	if (s[6] != 'e') return 0;

	return 1;
}

// this allows the parser to be restarted by storing state
int accept2(const char *s)
{
	int state = 0;
	for(; *s; s++)
	{
	switch (state)
	{
		case 0: if (*s != 's') state = -1; else state++; break;
		case 1: if (*s != 't') state = -1; else state++; break;
		case 2: if (*s != 'a') state = -1; else state++; break;
		case 3: if (*s != 't') state = -1; else state++; break;
		case 4: if (*s != 'i') state = -1; else state++; break;
		case 5: if (*s != 'c') state = -1; else return 1;
		case -1: break;
	}
	}

	printf("found static\n");

	return state != -1;
}

int main(int argc, char *argv[])
{
	FILE *fp = stdin;

	if (argc > 1)
		fp = fopen(argv[1], "rb");

	char s[256];
	while (getword3(fp, s))
	{
		printf("%i \"%s\"\n", currentline, s);
		accept2(s);
	}

	return 0;
}
