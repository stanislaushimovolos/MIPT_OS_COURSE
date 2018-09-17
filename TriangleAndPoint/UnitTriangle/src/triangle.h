#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include <assert.h>

typedef struct
{
    double x, y;
} Point;


typedef struct
{
    Point *points;
    size_t size;
} Polygon;

int destructPolygon(Polygon *pol);

int constructPolygon(Polygon *pol, size_t size);

int isTriangle(Point first, Point second, Point third);

double triangleSquare(Point first, Point second, Point third);
