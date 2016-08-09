#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define BUFFER_SIZE 8
#define BUFFER_MASK 0x7

// buffer
static FILE* fpbuf;
static int wi;
static int ri;
static int buffer[BUFFER_SIZE];

static int buf_count()
{
	return (wi - ri);
}

static int buf_pop()
{
	return buffer[ri++ & BUFFER_MASK];
}

static void buf_push(int c)
{
	buffer[wi++ & BUFFER_MASK] = c;
}

static void buf_fill()
{
	while(buf_count() < BUFFER_SIZE)
		buf_push(fgetc(fpsrc));
}

// interface
void bopen(FILE *fp);
int bfgetc();
int bpeekchar(int i);
int bnext();
int bskip();

void bopen(FILE *fp)
{
	fpsrc = fp;
	wi = 0;
	ri = 0;
}

int bfgetc()
{
	// handle base condition when buffer is empty
	fill_buffer();

	int c = buf_pop();
	fill_buffer();

	return c;
}

int bpeekchar(int i)
{
	fill_buffer();
	return buffer[(ri + i) & BUFFER_MASK];
}

int bnext()
{
	bfgetc();
}

int bskip(int count)
{
	while(count--)
		bnext();

int peekchar(int i)
{
	int c = bpeekchar(i);

	if (isspace(c) || !isprint(c))
		return '.';
	else if (c == EOF)
		return '?';
	else
		return c;
}

void peek()
{
	printf("peekchar[0]=%c, peekchar[1]=%c, peekchar[6]=%c, peekchar[7]=%c\n",
		peekchar(0),
		peekchar(1),
		peekchar(6),
		peekchar(7));
}

int main(int argc, char *argv[])
{
	bopen(stdin);
	
	int c;
	while((c = bfgetc()) != EOF)
	{
		peek();

		fputc(c, stdout);
	}

	return 0;
}
