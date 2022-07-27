#include <stdbool.h>
#include <SDL2\\SDL.h>
#include <math.h>

#include "calc.h"

// Note: Some functions are defined as macros in calc.h


double* calc_AngledPoints(double Point[2], double Angle, double Distance)
{
    double* NewPoint = malloc(sizeof(double) * 2);
    NewPoint[0] = Point[0] + (cos(Angle) * Distance);
    NewPoint[1] = Point[1] + (sin(Angle) * Distance);

    return NewPoint;
}


double* calc_AngledPointsDifference(double Point[2], double Angle, double Distance)
{
    double* NewPoint = calc_AngledPoints(Point, Angle, Distance);
    double* Difference = malloc(sizeof(double) * 2);
    Difference[0] = NewPoint[0] - Point[0];
    Difference[1] = NewPoint[1] - Point[1];

    free(NewPoint);
    return Difference;
}


double calc_NormalizeAngle(double Angle)
{
    while (Angle < 0) {
        Angle += TAU;
    }
    while (Angle >= TAU) {
        Angle -= TAU;
    }

    return Angle;
}