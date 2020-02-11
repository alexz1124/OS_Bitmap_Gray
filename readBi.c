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

     /* highest packet sent */
     char *buffer = NULL;
     const int datasize = ftell(fp); /* data buffer size */
     rewind(fp);

     //get a buffer big enough to hold it entirely
     buffer = (char *)malloc(datasize);
     if (!buffer)
     {
          fclose(fp);
          return 0;
     }

     //Read the entire file into buffer
     if (!fread(buffer, datasize, 1, fp))
     {
          free(buffer);
          fclose(fp);
          return 0;
     }
     fclose(fp);
}