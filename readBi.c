#include <stdio.h>

int main(char *argv[])
{
     FILE *fp = NULL;
     fp = fopen(argv[5], "rb");
     if (!fp)
          return 0;
     //get its length (in bytes)
     if (fseek(fp, 0, SEEK_END) != 0)
     {
          fclose(fp);
     }
}