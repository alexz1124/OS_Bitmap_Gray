#include <stdio.h>
#include <stdlib.h>

/* Our structure */
struct rec
{
    int x, y, z;
};

int main()
{
    int counter;
    FILE *ptr_myfile;
    struct rec my_record;

    ptr_myfile = fopen("taxi.png", "rb");
    if (!ptr_myfile)
    {
        printf("Unable to open file!");
        return 1;
    }

    fseek(ptr_myfile, sizeof(struct rec), SEEK_END);
    rewind(ptr_myfile);

    for (counter = 1; counter <= 10; counter++)
    {
        fread(&my_record, sizeof(struct rec), 1, ptr_myfile);
        printf("%d\n", my_record.x);
    }
    fclose(ptr_myfile);
    return 0;
}
