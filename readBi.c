#include <stdio.h>
#include <strings.h>
#include <sndfile.h>
#include <stdlib.h>
#include <math.h>

#define RATE 44100

typedef struct
{
     unsigned short type; /* Magic identifier            */
     unsigned int size;   /* File size in bytes          */
     unsigned int reserved;
     unsigned int offset; /* Offset to image data, bytes */
} HEADER;
typedef struct
{
     unsigned int size;             /* Header size in bytes      */
     int width, height;             /* Width and height of image */
     unsigned short planes;         /* Number of colour planes   */
     unsigned short bits;           /* Bits per pixel            */
     unsigned int compression;      /* Compression type          */
     unsigned int imagesize;        /* Image size in bytes       */
     int xresolution, yresolution;  /* Pixels per meter          */
     unsigned int ncolours;         /* Number of colours         */
     unsigned int importantcolours; /* Important colours         */
} INFOHEADER;
typedef struct
{
     unsigned char r, g, b, junk;
} COLOURINDEX;

int main(int argc, char *argv[])
{
     int i, j, rd;
     int gotindex = 0;
     unsigned char grey, r, g, b;
     double ampl;
     short _2byte[2];
     HEADER header;
     INFOHEADER infoheader;
     COLOURINDEX colourindex[256];
     FILE *fptr;
     SNDFILE *sndfile = NULL;
     SF_INFO sfinfo;
     long rate = RATE;

     void (*bmpread)();
     void _eightbit()
     {
          if (fread(&grey, sizeof(unsigned char), 1, fptr) != 1)
          {
               fprintf(stderr, "Image read failed\n");
               exit(-1);
          }
          if (gotindex)
          {
               ampl = colourindex[grey].r * 64. +
                      colourindex[grey].g * 128. +
                      colourindex[grey].b * 64.;
          }
          else
          {
               ampl = grey * 256. - 32768.;
          }
          //      printf("%.2f\n", ampl);
     }
     void _twentyfourbit()
     {
          do
          {
               if ((rd = fread(&b, sizeof(unsigned char), 1, fptr)) != 1)
                    break;
               if ((rd = fread(&g, sizeof(unsigned char), 1, fptr)) != 1)
                    break;
               if ((rd = fread(&r, sizeof(unsigned char), 1, fptr)) != 1)
                    break;
          } while (0);
          if (rd != 1)
          {
               fprintf(stderr, "Image read failed\n");
               exit(-1);
          }
          ampl = r * 64. + g * 128. + b * 64. - 32768.;
          //      printf("%.2f\n", ampl);
     }
     if (argc < 3)
     {
          printf("Usage: %s <input.bmp> <output.wav> [samplerate]\n", argv[0]);
          printf("For example:\n\t%s pict.bmp sample.wav 44100 2\n", argv[0]);
          exit(0);
     }
     printf("Input file: %s\n", argv[1]);
     printf("Output file: %s\n", argv[2]);
     if (argc > 3)
          rate = atoi(argv[3]);
     if (rate < 4000)
          rate = 4000;
     //if(argc > 4) channels = atoi(argv[4]);
     sfinfo.samplerate = rate;
     sfinfo.channels = 2;
     sfinfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
     if ((fptr = fopen(argv[1], "r")) == NULL)
     {
          fprintf(stderr, "Unable to open BMP file \"%s\"\n", argv[1]);
          exit(-1);
     }
     /* Read and check BMP header */
     if (fread(&header.type, 2, 1, fptr) != 1)
     {
          fprintf(stderr, "Failed to read BMP header\n");
          exit(-1);
     }
     if (header.type != 'M' * 256 + 'B')
     {
          fprintf(stderr, "File is not bmp type\n");
          exit(-1);
     }
     do
     {
          if ((rd = fread(&header.size, 4, 1, fptr)) != 1)
               break;
          printf("File size: %d bytes\n", header.size);
          if ((rd = fread(&header.reserved, 4, 1, fptr)) != 1)
               break;
          if ((rd = fread(&header.offset, 4, 1, fptr)) != 1)
               break;
          printf("Offset to image data is %d bytes\n", header.offset);
     } while (0);
     if (rd = !1)
     {
          fprintf(stderr, "Error reading file\n");
          exit(-1);
     }
     /* Read and check the information header */
     if (fread(&infoheader, sizeof(INFOHEADER), 1, fptr) != 1)
     {
          fprintf(stderr, "Failed to read BMP info header\n");
          exit(-1);
     }
     printf("Image size = %d x %d\n", infoheader.width, infoheader.height);
     printf("Number of colour planes is %d\n", infoheader.planes);
     printf("Bits per pixel is %d\n", infoheader.bits);
     printf("Compression type is %d\n", infoheader.compression);
     printf("Number of colours is %d\n", infoheader.ncolours);
     printf("Number of required colours is %d\n", infoheader.importantcolours);
     /* Read the lookup table if there is one */
     for (i = 0; i < 255; i++)
     {
          colourindex[i].r = rand() % 256;
          colourindex[i].g = rand() % 256;
          colourindex[i].b = rand() % 256;
          colourindex[i].junk = rand() % 256;
     }
     if (infoheader.ncolours > 0)
     {
          for (i = 0; i < infoheader.ncolours; i++)
          {
               do
               {
                    if ((rd = fread(&colourindex[i].b, sizeof(unsigned char), 1, fptr)) != 1)
                         break;
                    if ((rd = fread(&colourindex[i].g, sizeof(unsigned char), 1, fptr)) != 1)
                         break;
                    if ((rd = fread(&colourindex[i].r, sizeof(unsigned char), 1, fptr)) != 1)
                         break;
                    if ((rd = fread(&colourindex[i].junk, sizeof(unsigned char), 1, fptr)) != 1)
                         break;
               } while (0);
               if (rd != 1)
               {
                    fprintf(stderr, "Image read failed\n");
                    exit(-1);
               }
               printf("%3d\t%3d\t%3d\t%3d\n", i,
                      colourindex[i].r, colourindex[i].g, colourindex[i].b);
          }
          gotindex = 1;
     }
     if (infoheader.bits < 8)
     {
          printf("Too small image map depth (%d < 8)\n", infoheader.bits);
          exit(-1);
     }
     /* Seek to the start of the image data */
     fseek(fptr, header.offset, SEEK_SET);
     printf("Creating 16bit WAV %liHz.\n", rate);
     sndfile = sf_open(argv[2], SFM_WRITE, &sfinfo);
     if (sndfile == NULL)
     {
          fprintf(stderr, "Cannot open output file!\n");
          exit(-1);
     }
     bmpread = _eightbit;
     if (infoheader.bits == 24)
          bmpread = _twentyfourbit;

     /* Read the image */
     for (j = 0; j < infoheader.height; j++)
     {
          _2byte[1] = 32700;
          for (i = 0; i < infoheader.width; i++)
          {
               bmpread();
               _2byte[0] = (short)ampl;
               sf_write_short(sndfile, _2byte, 2);
               _2byte[1] = 0;
          } // i
     }      // j
     fclose(fptr);
     sf_close(sndfile);
}