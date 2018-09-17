
#include "triangle.h"

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


// Compute square of triangle
double triangleSquare(Point first, Point second, Point third)
{
    double square = fabs(
            0.5 * ((first.x - third.x) * (second.y - third.y) -
                   (first.y - third.y) * (second.x - third.x)));
    return square;
}