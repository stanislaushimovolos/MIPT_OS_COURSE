#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <errno.h>
#include <assert.h>

#include "stdio.h"

#define ARG_NUMBER 8

typedef struct
{
    double x, y;
} Point;


typedef struct
{
    Point *points;
    size_t size;
} Polygon;


int constructPolygon(Polygon *pol, size_t size)
{
    assert(pol);
    assert(size > 0);

    pol->size = size;
    pol->points = (Point *) calloc(size + 1, sizeof(Point));
    assert(pol->points);
    return 0;
}


int destructPolygon(Polygon *pol)
{
    assert(pol);

    free(pol->points);
    pol->points = NULL;
    return 0;
}


// Compute square of the triangle
double triangleSquare(Point first, Point second, Point third)
{
    double square = fabs(
            0.5 * ((first.x - third.x) * (second.y - third.y) -
                   (first.y - third.y) * (second.x - third.x)));
    return square;
}

// Check if 3 points can create a triangle
// |x + y| < |x| + |y|
int isTriangle(Point first, Point second, Point third)
{
    double firstSide = 0, secondSide = 0, thirdSide = 0;

    firstSide = sqrt((first.x - second.x) * (first.x - second.x) + (first.y - second.y) * (first.y - second.y));
    secondSide = sqrt((second.x - third.x) * (second.x - third.x) + (second.y - third.y) * (second.y - third.y));
    thirdSide = sqrt((third.x - first.x) * (third.x - first.x) + (third.y - first.y) * (third.y - first.y));

    if (firstSide < secondSide + thirdSide &&
        secondSide < firstSide + thirdSide &&
        thirdSide < firstSide + secondSide)
        return 1;

    return 0;
}


int main(int argc, char *argv[])
{
    if (argc == ARG_NUMBER + 1)
    {
        Polygon pol = {};

        // Create polygon with 4 points (triangle + selected point)
        constructPolygon(&pol, 4);
        Point *points = pol.points;

        double valueKeeper = 0;
        char *endPtr;
        for (int i = 0; i < 4; i++)
        {
            errno = 0;
            valueKeeper = strtod(argv[2 * i + 1], &endPtr);
            if (errno == ERANGE)
            {
                printf("Value is too large or too small\n");
                return EXIT_FAILURE;
            }
            if (endPtr == argv[2 * i + 1])
            {
                printf("No digits were found (%d point)\n", i);
                return EXIT_FAILURE;
            }
            if (*endPtr != '\0')
            {
                printf("Unexpected characters after the number %lg\n", valueKeeper);
                return EXIT_FAILURE;
            }
            points[i].x = valueKeeper;

            errno = 0;
            valueKeeper = strtod(argv[2 * (i + 1)], &endPtr);
            if (errno == ERANGE)
            {
                printf("Value is too large or too small\n");
                return EXIT_FAILURE;
            }
            if (endPtr == argv[2 * (i + 1)])
            {
                printf("No digits were found (%d point)\n", i);
                return EXIT_FAILURE;
            }
            if (*endPtr != '\0')
            {
                printf("Unexpected characters after the number %lg\n", valueKeeper);
                return EXIT_FAILURE;
            }
            points[i].y = valueKeeper;
        }

        if (!isTriangle(points[1], points[2], points[0]))
        {
            printf("These points can't create a triangle\n");
            return EXIT_FAILURE;
        }

        // realSqr - square of the triangle
        double realSqr = triangleSquare(points[0], points[1], points[2]);
        double firstSqr = 0, secondSqr = 0, thirdSqr = 0;

        // Compute squares of entire triangles
        firstSqr = triangleSquare(points[0], points[1], points[3]);
        secondSqr = triangleSquare(points[0], points[2], points[3]);
        thirdSqr = triangleSquare(points[1], points[2], points[3]);

        // if sum of 3 triangles != real square => print(No)
        if (firstSqr + secondSqr + thirdSqr != realSqr
            || (firstSqr * secondSqr * thirdSqr == 0))
            printf("No\n");
        else
            printf("Yes\n");

        destructPolygon(&pol);
    }
    else
    {
        printf("Error, expected %d arguments\n", ARG_NUMBER);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
