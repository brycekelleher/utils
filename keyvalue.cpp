#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *find_key(const char *data, const char *key)
{
	return strstr(data, key);
}

static const char *find_value(const char* data, const char *key)
{
	// find the key
	const char *k = find_key(data, key);

	// skip past it
	k = k + strlen(key);
	
	// eat whitespace up to the value
	k = k + strspn(k, " \t");

	return k;
}

int main(int argc, char *argv[])
{
	char k[128];
	char v[128];
	FILE *fp = stdin;

	static const char *data = 
	"posx 128\n"
	"posy 32\n"
	"posz 21\n"
	"width 1000\n"
	"height 1000\n";

	while(fscanf(fp, "%s", k) != EOF)
	{
		sscanf(find_value(data, k), "%s", v);
		printf("key %s is: %s\n", k, v);
	}
}

