// Simple Bob Jenkins's hash algorithm taken from the wikipedia description.
unsigned int hash_bj(void *data, int numbytes)
{
	unsigned char	*bytes = (unsigned char*)data;
	unsigned int	hash = 0;

	for (int i = 0; i < numbytes; ++i)
	{
		hash += bytes[i];
		hash += (hash << 10);
		hash ^= (hash >> 6);
	}

	hash += (hash << 3);
	hash ^= (hash >> 11);
	hash += (hash << 15);

	return hash;
}

// settings taken from
// http://www.isthe.com/chongo/tech/comp/fnv/index.html#FNV-param
const unsigned int FNV_PRIME = 16777619;
const unsigned int FNV_OFFSET_BASIS = 2166136261;

unsigned int hash_fnv1a(void *data, int numbytes)
{
	unsigned char	*bytes = (unsigned char*)data;
	unsigned int	hash = FNV_OFFSET_BASIS;

	for (int i = 0; i < numbytes; i++)
	{
		hash ^= bytes[i];
		hash *= FNV_PRIME;
	}

	return hash;
}

const int MOD_ADLER = 65521;

unsigned int hash_adler32(void *data, int numbytes)
{
	unsigned char	*bytes = (unsigned char*)data;
	unsigned int 	a = 1, b = 0;

	for (int i = 0; i < numbytes; i++)
	{
		a = (a + bytes[i]) % MOD_ADLER;
		b = (b + a) % MOD_ADLER;
	}

	return (b << 16) | a;
}

unsigned int hash_djb(void *data, int numbytes)
{
	unsigned char	*bytes = (unsigned char*)data;
	unsigned int	hash = 5381;

	for (int i = 0; i < numbytes; i++)
		hash = ((hash << 5) + hash) + bytes[i]; /* hash * 33 + c */

	return hash;
}

