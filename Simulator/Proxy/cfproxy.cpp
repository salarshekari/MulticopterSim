/*
   Proxy for testing MulticopterSim CrazyFlie comms

   Just lifts off to 40cm after enough throttle is applied; no
   other control.

   Copyright(C) 2023 Simon D.Levy

   MIT License
 */

#include <stdio.h>
#include <stdio.h>
#include <stdint.h>

#include "../Source/MultiSim/sockets/TcpServerSocket.hpp"
#include "../Source/MultiSim/dynamics/fixedpitch/QuadXBF.hpp"

// Comms
static const char * HOST = "127.0.0.1"; // localhost
static uint16_t  PORT = 5000;

// Time constant
static const double DELTA_T = 0.001;

// Throttle threshold for liftoff
static const float THROTTLE_THRESHOLD = 0.5;

// PI controller constants
static const double K_P = 1.0;
static const double K_I = 0.0;
static const double Z_TARGET = 0.40;

static Dynamics::vehicle_params_t vparams = {

    // Estimated
    2.E-06, // d torque constant [T=d*w^2]

    // https://www.dji.com/phantom-4/info
    1.380,  // m mass [kg]

    // Estimated
    2,      // Ix [kg*m^2] 
    2,      // Iy [kg*m^2] 
    3,      // Iz [kg*m^2] 
    38E-04, // Jr prop inertial [kg*m^2] 

    15000 // maxrpm
};

static FixedPitchDynamics::fixed_pitch_params_t fparams = {

    // Estimated
    5.E-06, // b force constatnt [F=b*w^2]
    0.350   // l arm length [m]
};

// Altitude PI controller
static float getThrottle(const Dynamics & dynamics)
{
    return 0.6;
}

int main(int argc, char ** argv)
{
    TcpServerSocket server = TcpServerSocket(HOST, PORT, true);

    // Guards socket comms
    bool connected = false;

    // Create quadcopter dynamics model
    QuadXBFDynamics dynamics =
        QuadXBFDynamics(vparams, fparams, false); // no auto-land

    // Set up initial conditions
    double rotation[3] = {0,0,0};
    dynamics.init(rotation);

    printf("Listening for client on %s:%d \n", HOST, PORT);

    // Loop forever, waiting for clients
    for (uint32_t k=0; ; k++) {

        if (connected) {
          
            const double pose[] = {

                dynamics.vstate.x,
                dynamics.vstate.y,
                -dynamics.vstate.z, // NED => ENU
                dynamics.vstate.phi,
                dynamics.vstate.theta,
                dynamics.vstate.psi
            };

            server.sendData((void *)pose, sizeof(pose));

            double joyvals[4] = {};
            server.receiveData(joyvals, sizeof(joyvals));

            float sticks[4] = {
                (float)joyvals[0] / 80,
                (float)joyvals[1] / 31,
                (float)joyvals[2] / 31,
                (float)joyvals[3] / 200,
            };

            static bool airborne;

            if (sticks[0] > THROTTLE_THRESHOLD) {
                airborne = true;
            }

            const float throttle = airborne ? getThrottle(dynamics) : 0;

            // Set all motors to same value for now
            float motors[4] = {throttle, throttle, throttle, throttle};

            // Update dynamics with motor values
            dynamics.update(motors, DELTA_T);

            // Set AGL to arbitrary positive value to avoid kinematic trick
            dynamics.setAgl(1);
        }

        else {

            connected = server.acceptConnection();

        }

    } // while (true)

    return 0;
}
