/*
The game contains a weather mechanic, where bullets are affected by the weather. This is done using a linked list of weather instances and functions that manage the list which are defined in this file (weather.c).

Some definitions (e.g. the actual weather struct) are defined in weather.h.
*/
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#include "weather.h"


weather_Weather* weather_Init(double Direction, double Strength, int Time)
{
    /*
    This function initializes a new weather instance.
    Note that the function does not change the linked-list aspect of the weather_Weather struct (no references to next). This is done in the update_UpdateWeather function (located in update.c).
    */
    weather_Weather* Weather = malloc(sizeof(weather_Weather));
    Weather->Direction = Direction;
    Weather->Strength = Strength;
    Weather->Time = Time;
    return Weather;
}


void weather_WeatherSpeedUpdate(double* vx, double* vy, weather_Weather* WeatherInstance)
{
    /*
    This function updates x and y velocities of any object that is affected by the weather and changes the speeds in-place.
    Note that this function works with any object that has x and y velocities, but it is only used for bullets (at the time of writing this).
    */
    double Weather_vx = cos(WeatherInstance->Direction) * WeatherInstance->Strength;
    double Weather_vy = -sin(WeatherInstance->Direction) * WeatherInstance->Strength;

    *vx += Weather_vx;
    *vy += Weather_vy;
}