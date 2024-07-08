/********************************
Author: Sravanthi Kota Venkata
********************************/

#include "sdvbs_common.h"

int my_scanf(FILE *stream,  const char *format, int *result){
    int ch;
    int value = 0;
    int digitEncountered = 0;

    // Skip non-digit characters
    while ((ch = ff_fgetc(stream)) != EOF && !isdigit(ch));

    // Read digits until a non-digit character is encountered
    while (ch != EOF && isdigit(ch)) {
        digitEncountered = 1;
        value = value * 10 + (ch - '0');
        ch = ff_fgetc(stream);
    }

    // If at least one digit was encountered, update the result
    if (digitEncountered) {
        *result = value;
        return 1;  // Successfully read an integer
    } else {
        return 0;  // No digit was encountered
    }
}

F2D* readFile(unsigned char* fileName)
{
    FILE* fp;
    F2D *fill;
    float temp;
    int rows, cols;
    int i, j;

    fp = fopen(fileName, "r");
    if(fp == NULL)
    {
        printf("Error in file %s\n", fileName);
        return NULL;
    }

    fscanf(fp, "%d", &cols);
    fscanf(fp, "%d", &rows);

    fill = fSetArray(rows, cols, 0);

    for(i=0; i<rows; i++)
    {
        for(j=0; j<cols; j++)
        {
            fscanf(fp, "%f", &(subsref(fill,i,j)) );
        }
    }

    fclose(fp);    
    return fill;
}





