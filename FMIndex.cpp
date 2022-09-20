#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstring>
#include <cmath>
#include <stdlib.h>
#include <sys/time.h>

#include "FMIndex.h"

// #include <sys/mman.h> //for mremap()

using namespace std;

int read_count = 0;
int read_length = 0;
int read_total;

int **SA_Final;
int **L_counts;
char *L;
int F_counts[] = {0, 0, 0, 0};

//Read file to get reads
char **inputReads(char *file_path, int *read_count, int *length)
{
    FILE *read_file = fopen(file_path, "r");
    int ch, lines = 0;
    char **reads;
    do
    {
        ch = fgetc(read_file);
        if (ch == '\n')
            lines++;
    } while (ch != EOF);
    rewind(read_file);
    reads = (char **)malloc(lines * sizeof(char *));
    *read_count = lines;
    int i = 0;
    size_t len = 0;
    for (i = 0; i < lines; i++)
    {
        reads[i] = NULL;
        len = 0;
        getline(&reads[i], &len, read_file);
    }
    fclose(read_file);
    int j = 0;

    while (reads[0][j] != '\n')
    {
        j++;
        cout << "program input reads = " << reads[0][j] << endl; // should not modify
    }

    cout << "lines: " << lines << endl;

    *length = j + 1;
    for (i = 0; i < lines; i = i + 1)
    {
        cout << "program input reads number = " << i << endl; // should not modify
        reads[i][j] = '$';

        if (i + 1 >= lines)
        {
            for (; i < lines; i++)
                reads[i][j] = '$';
        }
    }
    return reads;
}

//Check correctness of values
void checker(char *file_path)
{
    fstream fout;
    fout.open(file_path, ios::out);

    for (int i = 0; i < read_count * read_length; i++)
    {
        fout << L[i] << " ";
        for (int j = 0; j < 2; j++)
        {
            fout << SA_Final[i][j] << " ";
        }

        for (int j = 0; j < 4; j++)
        {
            fout << L_counts[i][j] << " ";
        }
    }

    for (int i = 0; i < 4; i++)
    {
        fout << F_counts[i] << " ";
    }
}

//Rotate read by 1 character
void rotateRead(char *read, char *rotatedRead, int length)
{
    cout << "program ratating read : " << read[0] << endl; // should not modify
    for (int i = 0; i < length - 1; i++)
        rotatedRead[i] = read[i + 1];

    rotatedRead[length - 1] = read[0];
}

//Generate Sufixes and their SA's for a read
char **generateSuffixes(char *read, int length, int read_id)
{
    char **suffixes = (char **)malloc(length * sizeof(char *));
    suffixes[0] = (char *)malloc(length * sizeof(char));

    cout << "program generate suffixes length : " << length << endl; // should not modify
    cout << "program generate suffixes read = " << read[0] << endl;  // should not modify

    for (int j = 0; j < length; j++)
        suffixes[0][j] = read[j];

    for (int i = 1; i < length; i++)
    {
        suffixes[i] = (char *)malloc(length * sizeof(char));
        rotateRead(suffixes[i - 1], suffixes[i], length);
    }
    return suffixes;
}

//Comparator for Suffixes
int compSuffixes(char *suffix1, char *suffix2, int length)
{
    int ret = 0;

    for (int i = 0; i < length; i++)
    {
        if (suffix1[i] > suffix2[i])
            return 1;
        else if (suffix1[i] < suffix2[i])
            return -1;
    }
    return ret;
}

//Calculates the final FM-Index
int **makeFMIndex(char ***suffixes, int read_count, int read_length, int F_count[], char *L)
{
    int i, j;

    SA_Final = (int **)malloc(read_count * read_length * sizeof(int *));
    for (i = 0; i < read_count * read_length; i++)
        SA_Final[i] = (int *)malloc(2 * sizeof(int));

    //Temporary storage for collecting together all suffixes
    char **temp_suffixes = (char **)malloc(read_count * read_length * sizeof(char *));

    //Initalization of temporary storage
    cout << "###  Initalization of temporary storage" << endl;
    for (i = 0; i < read_count; i++)
    {
        for (j = 0; j < read_length; j++)
        {
            temp_suffixes[i * read_length + j] = (char *)malloc(read_length * sizeof(char));
            memcpy(&temp_suffixes[i * read_length + j], &suffixes[i][j], read_length * sizeof(char));

            SA_Final[i * read_length + j][0] = j;
            SA_Final[i * read_length + j][1] = i;
        }
    }

    char *temp = (char *)malloc(read_length * sizeof(char));

    int **L_count = (int **)malloc(read_length * read_count * sizeof(int *));

    cout << "###  Initalization of L_count" << endl;
    for (i = 0; i < read_length * read_count; i++)
    {
        L_count[i] = (int *)malloc(4 * sizeof(int));
        for (j = 0; j < 4; j++)
        {
            L_count[i][j] = 0;
        }
    }

    //Sorting of suffixes

    for (i = 0; i < read_count * read_length - 1; i++)
    {
        if (i % 10 == 0)
        {
            cout << "###  sorting  " << i << " / " << read_count * read_length - 1 << " suffixes" << endl;
        }
        for (j = 0; j < read_count * read_length - i - 1; j++)
        {
            if (compSuffixes(temp_suffixes[j], temp_suffixes[j + 1], read_length) > 0)
            {

                memcpy(temp, temp_suffixes[j], read_length * sizeof(char));
                memcpy(temp_suffixes[j], temp_suffixes[j + 1], read_length * sizeof(char));
                memcpy(temp_suffixes[j + 1], temp, read_length * sizeof(char));

                int temp_int;
                temp_int = SA_Final[j][0];
                SA_Final[j][0] = SA_Final[j + 1][0];
                SA_Final[j + 1][0] = temp_int;
                temp_int = SA_Final[j][1];
                SA_Final[j][1] = SA_Final[j + 1][1];
                SA_Final[j + 1][1] = temp_int;
            }
        }
    }

    free(temp);
    char this_F = '$';
    j = 0;

    //Calculation of F_count's
    cout << "###  calculation of F_count" << endl;
    for (i = 0; i < read_count * read_length; i++)
    {
        int count = 0;
        while (temp_suffixes[i][0] == this_F)
        {
            count++;
            i++;
        }
        F_count[j++] = j == 0 ? count : count + 1;

        this_F = temp_suffixes[i][0];
        if (temp_suffixes[i][0] == 'T')
            break;
    }

    //Calculation of L's and L_count's
    cout << "###  calculation of L_count" << endl;
    for (i = 0; i < read_count * read_length; i++)
    {
        char ch = temp_suffixes[i][read_length - 1];
        L[i] = ch;

        if (i > 0)
        {
            for (int k = 0; k < 4; k++)
                L_count[i][k] = L_count[i - 1][k];
        }
        cout << "program counting L_count ch = " << ch << endl; // should not modify

        if (ch == 'A')
            L_count[i][0]++;
        else if (ch == 'C')
            L_count[i][1]++;
        else if (ch == 'G')
            L_count[i][2]++;
        else if (ch == 'T')
            L_count[i][3]++;
    }

    return L_count;
}

//-----------------------DO NOT CHANGE--------------------------------------------

int main(int argc, char *argv[])
{

    char *read_data_file = "T5_260.txt";       // input DATA
    char *output_data_file = "pim_result.txt"; // output DATA

    char **reads = inputReads(read_data_file, &read_count, &read_length); //Input reads from file

    char ***suffixes = (char ***)malloc(read_count * sizeof(char **)); //Storage for read-wise suffixes

    cout << "###  input reads done" << endl;

    //-----------------------------Structures for correctness check----------------------------------------------
    L = (char *)malloc(read_count * read_length * sizeof(char *)); //Final storage for last column of sorted suffixes

    //-----------------------------Structures for correctness check----------------------------------------------

    //-----------Default implementation----------------

    //Generate read-wise suffixes
    for (int i = 0; i < read_count; i++)
    {
        cout << "###  generating " << i << " / " << read_count << " suffixes" << endl;
        suffixes[i] = generateSuffixes(reads[i], read_length, i);
    }

    //Calculate final FM-Index
    cout << "###  start making FMIndex" << endl;
    L_counts = makeFMIndex(suffixes, read_count, read_length, F_counts, L);
    cout << "###  end making FMIndex" << endl;

    //---------------Correction check and speedup calculation----------------------
    checker(output_data_file);

    cout << "end program" << endl;
    //-----------------------------------------------------------------------------
    return 0;
}
