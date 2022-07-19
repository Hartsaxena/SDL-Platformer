#ifndef WEATHER_H
#define WEATHER_H

typedef struct weather_Weather {
    double Direction;
    double Strength;
    int Time;
    struct weather_Weather* next;
} weather_Weather;

weather_Weather* weather_Init(double Direction, double Strength, int Time);
void weather_WeatherSpeedUpdate(double* vx, double* vy, weather_Weather* WeatherInstance);


#endif