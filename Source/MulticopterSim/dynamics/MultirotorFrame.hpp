/*
 * Abstract class for multicopter frames
 *
 * For reference citation see MultirotorDynamics.hpp
 *
 * Copyright (C) 2019 Simon D. Levy
 *
 * MIT License
 */

#pragma once

#include <stdint.h>
#include <string.h>

class MultirotorFrame {

    private:

        uint8_t   _motorCount;

        double * _motorLocations; 

    protected:

        MultirotorFrame(const double * motorLocations, uint8_t motorCount) 
        {
            _motorCount = motorCount;

            _motorLocations = new double[4*motorCount];

            memcpy(_motorLocations, motorLocations, 12*sizeof(double));
        }

        virtual ~MultirotorFrame(void)
        {
            delete _motorLocations;
         }

    public:

        uint8_t motorCount(void)
        {
            return _motorCount;
        }

        // roll right
        virtual double u2(double * o) = 0;

        // pitch forward
        virtual double u3(double * o) = 0;

        // yaw cw
        virtual double u4(double * o) = 0;

        // motor direction for animation
        virtual int8_t motorDirection(uint8_t i)  = 0;

}; // class MultirotorFrame

