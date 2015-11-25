#include <stdio.h>

int main(int argc, char *argv[])
{
	FILE *fp = stdin;

	if (argc > 1)
		fp = fopen(argv[1], "rb");

	int linecount = 0;
	int c;
	while ((c = fgetc(fp)) != EOF)
		if (c == '\n')
			linecount++;

	printf("num lines: %i\n", linecount);

	return 0;
}
