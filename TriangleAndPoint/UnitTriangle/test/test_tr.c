#include "../src/triangle.h"
#include "unity.h"

#define TRUE 1
#define FALSE 0

Polygon pol = {};
Point *points = NULL;

double firstSqr = 0, secondSqr = 0,
        thirdSqr = 0, realSqr = 0;


int isInside(double x1, double y1, double x2, double y2, double x3, double y3, double x, double y)
{
    points[0].x = x1;
    points[0].y = y1;

    points[1].x = x2;
    points[1].y = y2;

    points[2].x = x3;
    points[2].y = y3;

    points[3].x = x;
    points[3].y = y;

    realSqr = triangleSquare(points[0], points[1], points[2]);
    firstSqr = triangleSquare(points[0], points[1], points[3]);
    secondSqr = triangleSquare(points[0], points[2], points[3]);
    thirdSqr = triangleSquare(points[1], points[2], points[3]);

    return firstSqr + secondSqr + thirdSqr == realSqr && (firstSqr * secondSqr * thirdSqr != 0);
}


void setUp(void)
{
    printf("setUp\n");
    constructPolygon(&pol, 4);
    points = pol.points;
}


void tearDown(void)
{
    printf("tearDown\n");
    destructPolygon(&pol);
}


void test_in(void)
{
    int res = isInside(0, 0, 100, 0, 0, 100, 100, 100);
    TEST_ASSERT_EQUAL(FALSE, res);
}


void test_out(void)
{
    int res = isInside(0, 0, 100, 0, 0, 100, 10, 10);
    TEST_ASSERT_EQUAL(TRUE, res);
}


void test_border(void)
{
    int res = isInside(0, 0, 100, 0, 0, 100, 50, 50);
    TEST_ASSERT_EQUAL(FALSE, res);
}


void test_triangleTop(void)
{
    int res = isInside(0, 0, 100, 0, 0, 100, 0, 0);
    TEST_ASSERT_EQUAL(FALSE, res);
}


