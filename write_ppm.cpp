#include <stdio.h>

static void WritePPMFile(const char *filename, int w, int h, unsigned char *data)
{
        int x, y;

        FILE *fp = fopen("reference.ppm", "w");

        fprintf(fp, "P3\n%d\n%d\n255\n", w, h);

        for(y = 0; y < h; y++)
        {
                for(x = 0; x < w; x++)
                {
                        fprintf(fp, "\t%3i %3i %3i", data[0], data[1], data[2]);
			data += 3;
                }

                fprintf(fp, "\n");
        }

	fclose(fp);
}

