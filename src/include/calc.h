#ifndef BARRIER_H
#define BARRIER_H

#define PI 3.14159265358979323846
#define TAU (PI * 2)

#define MAX(a, b) ((a > b) ? a : b)
#define MIN(a, b) ((a < b) ? a : b)
#define DEG_TO_RAD(x) ((x) * (PI / 180))

double* calc_AngledPoints(double Point[2], double Angle, double Distance);
double* calc_AngledPointsDifference(double Point[2], double Angle, double Distance);
double calc_NormalizeAngle(double Angle);


#endif