#include "build/temp/_test_tr.c"
#include "unity.h"
#include "../src/triangle.h"








Polygon pol = {};

Point *points = 

               ((void *)0)

                   ;



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

    UnityAssertEqualNumber((UNITY_INT)((0)), (UNITY_INT)((res)), (

   ((void *)0)

   ), (UNITY_UINT)(55), UNITY_DISPLAY_STYLE_INT);

}





void test_out(void)

{

    int res = isInside(0, 0, 100, 0, 0, 100, 10, 10);

    UnityAssertEqualNumber((UNITY_INT)((1)), (UNITY_INT)((res)), (

   ((void *)0)

   ), (UNITY_UINT)(62), UNITY_DISPLAY_STYLE_INT);

}





void test_border(void)

{

    int res = isInside(0, 0, 100, 0, 0, 100, 50, 50);

    UnityAssertEqualNumber((UNITY_INT)((0)), (UNITY_INT)((res)), (

   ((void *)0)

   ), (UNITY_UINT)(69), UNITY_DISPLAY_STYLE_INT);

}





void test_triangleTop(void)

{

    int res = isInside(0, 0, 100, 0, 0, 100, 0, 0);

    UnityAssertEqualNumber((UNITY_INT)((0)), (UNITY_INT)((res)), (

   ((void *)0)

   ), (UNITY_UINT)(76), UNITY_DISPLAY_STYLE_INT);

}
