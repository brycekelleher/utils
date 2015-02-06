#include <stdlib.h>	// only needed for malloc and free
#include <string.h>	// only needed for strcmp - could replace?

#define STAB_NUM_BUCKETS	256

typedef struct blah_s
{
	struct blah_s	*next;
	char	*string;

} blah_t;

// typedef struct stab
static blah_t	*buckets[STAB_NUM_BUCKETS];

static void* stab_malloc(int numbytes)
{
	return malloc(numbytes);
}

static void stab_free(void *p)
{
	free(p);
}

static blah_t *stab_alloc_string(const char *string)
{
	blah_t * b = (blah_t*)stab_malloc(sizeof(blah_t) + strlen(string) + 1);

	// point string at memory allocated after node
	b->next		= NULL;
	b->string	= (char*)(b + 1);

	strcpy(b->string, string);

	return b; 
}

static unsigned int hash_string(const char *string)
{
	unsigned int hash;
	const char *s = string;

	for(hash = 0; *s; s++)
	{
		hash += (unsigned int)(*s);
	}

	return hash;
}

const char *stab_insertf(const char *string)
{
	unsigned int	hash;
	blah_t		*b;

	hash = hash_string(string);
	hash %= STAB_NUM_BUCKETS;

	// search the hash table
	for(b = buckets[hash]; b; b = b->next)
	{
		if(strcmp(b->string, string))
			continue;

		return b->string;
	}

	// allocate a new entry and link into the hash table
	b = stab_alloc_string(string);
	b->next = buckets[hash];
	buckets[hash] = b;

	return b->string;
}

#include <stdio.h>

static int stab_count_bucket_entries(blah_t *b)
{
	int i;

	for(i = 0; b; b = b->next, i++)
	{
		i++;
	}

	return i;
}

static void stab_dumpstats()
{
	for(int i = 0; i < STAB_NUM_BUCKETS; i++)
	{
		printf("bucket %i: %i\n", i, stab_count_bucket_entries(buckets[i]));
	}
}
int main(int argc, char *argv[])
{
	while(!feof(stdin))
	{
		char buffer[1024];
		fscanf(stdin, "%s", buffer);

		const char *string = stab_insertf(buffer);
		printf("string: (%08x): \"%s\"\n", string, string);
	}

	stab_dumpstats();
}

