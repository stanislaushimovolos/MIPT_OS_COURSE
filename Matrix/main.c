#include <stdlib.h>
#include <assert.h>
#include "stdio.h"
#include <limits.h>
#include <errno.h>

#define MIN_ARG_NUM 2

typedef struct
{
    double **data;
    size_t columnNum;
    size_t rowsNum;
} Matrix;


int constructMatrix(Matrix *mat, size_t rowSz, size_t columnSz)
{
    assert(mat);
    assert(rowSz > 0);
    assert(columnSz > 0);

    mat->columnNum = columnSz;
    mat->rowsNum = rowSz;
    mat->data = (double **) calloc(rowSz, sizeof(double *));

    double **data = mat->data;

    for (int i = 0; i < rowSz; i++)
        data[i] = (double *) calloc(columnSz, sizeof(double));
    return 0;
}


int destructMatrix(Matrix *mat)
{
    assert(mat);

    double **data = mat->data;
    size_t dataSz = mat->rowsNum;

    for (int i = 0; i < dataSz; i++)
        free(data[i]);
    free(mat->data);

    mat->data = NULL;
    return 0;
}


int printMatrix(Matrix *mat)
{
    assert(mat);

    double **data = mat->data;
    size_t rowSz = mat->rowsNum;
    size_t columnSz = mat->columnNum;

    for (int i = 0; i < rowSz; i++)
    {
        for (int j = 0; j < columnSz; j++)
            printf("%lg ", data[i][j]);
        printf("\n");
    }
    printf("\n");
    return 0;
}


Matrix transposeMatrix(Matrix *mat)
{
    assert(mat);

    Matrix a = {};
    constructMatrix(&a, mat->columnNum, mat->rowsNum);

    double **dataOld = mat->data;
    double **dataNew = a.data;
    size_t rowSz = mat->rowsNum;
    size_t columnSz = mat->columnNum;

    for (int i = 0; i < rowSz; i++)
        for (int j = 0; j < columnSz; j++)
            dataNew[j][i] = dataOld[i][j];

    return a;
}


int readMatrix(Matrix *mat, int argc, char *argv[])
{
    if (argc >= MIN_ARG_NUM + 1)
    {
        size_t columnSz = 0;
        size_t rowSz = 0;

        char *endPtr;

        errno = 0;
        rowSz = strtoull(argv[1], &endPtr, 10);
        if (errno == ERANGE)
        {
            printf("Value is too large or too small (rows)\n");
            return EXIT_FAILURE;
        }
        if (endPtr == argv[1])
        {
            printf("No digits were found (rows)\n");
            return EXIT_FAILURE;
        }
        if (*endPtr != '\0')
        {
            printf("Unexpected characters after the number %lu\n (rows)", rowSz);
            return EXIT_FAILURE;
        }

        errno = 0;
        columnSz = strtoul(argv[2], &endPtr, 10);
        if (errno == ERANGE)
        {
            printf("Value is too large or too small (columns)\n");
            return EXIT_FAILURE;
        }
        if (endPtr == argv[2])
        {
            printf("No digits were found (columns)\n");
            return EXIT_FAILURE;
        }
        if (*endPtr != '\0')
        {
            printf("Unexpected characters after the number %lu\n (columns)", columnSz);
            return EXIT_FAILURE;
        }

        if (argc != columnSz * rowSz + 3)
        {
            printf("Wrong number of parameters\n");
            return EXIT_FAILURE;
        }
        constructMatrix(mat, rowSz, columnSz);
        double **data = mat->data;

        for (int i = 0; i < rowSz; i++)
            for (int j = 0; j < columnSz; j++)
            {
                errno = 0;
                data[i][j] = strtod(argv[i * (rowSz + 1) + j + 3], &endPtr);

                if (errno == ERANGE)
                {
                    printf("Value is too large or too small %zuth number\n", i * (rowSz + 1) + j);
                    return EXIT_FAILURE;
                }
                if (endPtr == argv[i * (rowSz + 1) + j + 3])
                {
                    printf("No digits were found %zuth number\n", i * (rowSz + 1) + j);
                    return EXIT_FAILURE;
                }
                if (*endPtr != '\0')
                {
                    printf("Unexpected characters after the %zuth number\n", i * (rowSz + 1) + j);
                    return EXIT_FAILURE;
                }
            }
        return 0;
    }
    printf("Expected > 2 arguments\n");
    return EXIT_FAILURE;
}


int main(int argc, char *argv[])
{
    Matrix mat = {}, transposed = {};

    int status = readMatrix(&mat, argc, argv);
    if (status)
        return EXIT_FAILURE;

    printMatrix(&mat);
    transposed = transposeMatrix(&mat);
    printMatrix(&transposed);

    destructMatrix(&transposed);
    destructMatrix(&mat);
    return 0;
}