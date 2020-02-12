#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/time.h>
#include <sys/timeb.h>

int main(int argc, char *argv[])
{
    struct timeval start2, end2;
    struct timeb start, end;
    int testInteger;
    int diff;

    int i, tmp;
    pid_t pid[4];
    int round = 0;

    FILE *fIn = fopen(argv[1], "rb");
    FILE *fOut = fopen(argv[2], "wb");
    if (!fIn || !fOut)
    {
        printf("File error.\n");
        return 0;
    }
    ftime(&start);
    gettimeofday(&start2, NULL);
    unsigned char header[54];
    fread(header, sizeof(unsigned char), 54, fIn);
    fwrite(header, sizeof(unsigned char), 54, fOut);

    int width = *(int *)&header[18];
    int height = abs(*(int *)&header[22]);
    int stride = (width * 3 + 3) & ~3;
    int padding = stride - width * 3;

    printf("width: %d (%d)\n", width, width * 3);
    printf("height: %d\n", height);
    printf("stride: %d\n", stride);
    printf("padding: %d\n", padding);

    unsigned char pixel[3];

    for (i = 0; i < 4; i++)
    {
        pid[i] = fork();
        if (pid[i] == 0)
        {

            break;
        }
    }

    if (pid[0] != 0 && pid[1] != 0 && pid[2] != 0 && pid[3] != 0)
    {
        // That's the father, it waits for all the childs
        printf("I'm the father [pid: %d, ppid: %d]\n", getpid(), getppid());
        for (i = 0; i < 4; i++)
        {
            wait(&tmp);
        }
        ftime(&end);
        gettimeofday(&end2, NULL);
        diff = (int)(1000.0 * (end.time - start.time) + (end.millitm - start.millitm));
        unsigned long time_in_microsStart = 1000000 * start2.tv_sec + start2.tv_usec;
        unsigned long time_in_microsStop = 1000000 * end2.tv_sec + end2.tv_usec;
        printf("\nTime2: %ld microsecons\n", time_in_microsStop - time_in_microsStart);

        fclose(fOut);
        fclose(fIn);
    }
    else
    {
        if (getpid() == getppid() + 1)
        {
            for (int y = 0; y < (height / 4)*1; ++y)
            {
                for (int x = 0; x < width; ++x)
                {
                    fread(pixel, 3, 1, fIn);
                    unsigned char gray = pixel[0] * 0.3 + pixel[1] * 0.58 + pixel[2] * 0.11;
                    memset(pixel, gray, sizeof(pixel));
                    fwrite(&pixel, 3, 1, fOut);
                }
                fread(pixel, padding, 1, fIn);
                fwrite(pixel, padding, 1, fOut);
            }
             printf("I'm one of the children [pid: %d, ppid: %d] round 1\n",getpid(),getppid());
        }
        else if (getpid() == getppid() + 2)
        {
            for (int y = (height / 4)*1; y < (height / 4)*2; ++y)
            {
               for (int x = 0; x < width; ++x)
                {
                    fread(pixel, 3, 1, fIn);
                    unsigned char gray = pixel[0] * 0.3 + pixel[1] * 0.58 + pixel[2] * 0.11;
                    memset(pixel, gray, sizeof(pixel));
                    fwrite(&pixel, 3, 1, fOut);
                }
                fread(pixel, padding, 1, fIn);
                fwrite(pixel, padding, 1, fOut);
            }
             printf("I'm one of the children [pid: %d, ppid: %d] round 2\n",getpid(),getppid());
        }
        else if (getpid() == getppid() + 3)
        {
            for (int y = (height / 4)*2; y < (height / 4)*3; ++y)
            {
                for (int x = 0; x < width; ++x)
                {
                    fread(pixel, 3, 1, fIn);
                    unsigned char gray = pixel[0] * 0.3 + pixel[1] * 0.58 + pixel[2] * 0.11;
                    memset(pixel, gray, sizeof(pixel));
                    fwrite(&pixel, 3, 1, fOut);
                }
                fread(pixel, padding, 1, fIn);
                fwrite(pixel, padding, 1, fOut);
            }
             printf("I'm one of the children [pid: %d, ppid: %d] round 3\n",getpid(),getppid());
        }
        else if (getpid() == getppid() + 4)
        {
            for (int y = (height / 4)*3; y < (height / 4)*4; ++y)
            {
                for (int x = 0; x < width; ++x)
                {
                    fread(pixel, 3, 1, fIn);
                    unsigned char gray = pixel[0] * 0.3 + pixel[1] * 0.58 + pixel[2] * 0.11;
                    memset(pixel, gray, sizeof(pixel));
                    fwrite(&pixel, 3, 1, fOut);
                }
                fread(pixel, padding, 1, fIn);
                fwrite(pixel, padding, 1, fOut);
            }
             printf("I'm one of the children [pid: %d, ppid: %d] round 4\n",getpid(),getppid());
        }

        // That's the child, it print its pid, its parent pid and exits.
    }

    return 0;
}