/*
 * Socket-based flight-management class for MultiSim
 *
 * Copyright (C) 2023 Simon D. Levy
 *
 * MIT License
 */

#pragma once

#include "../Thread.hpp"

#include "../sockets/TcpServerSocket.hpp"

class FCrazyflieThread : public FVehicleThread {

    private: 

        // Time : State
        double _telemetry[13] = {};

        // Socket comms
        TcpServerSocket * _server = NULL;

        // Guards socket comms
        bool _connected = false;

    protected:

        virtual void getMotors(
                const double time,
                const float * joyvals,
                const Dynamics * dynamics,
                float * motors,
                const uint8_t motorCount) override
        {
            (void)joyvals;
            (void)motorCount;

            if (_server) {

                if (_connected) {

                    static double fake_z;

                    fake_z += .001;

                    const double telemetry[] = {

                        // vehicle state
                        dynamics->vstate.x,
                        dynamics->vstate.y,
                        fake_z /*dynamics->vstate.z*/ ,
                        dynamics->vstate.phi,
                        dynamics->vstate.theta,
                        dynamics->vstate.psi
                    };

                    _server->sendData((void *)telemetry, sizeof(telemetry));

                    double sticks[4] = {};
                    _server->receiveData(sticks, sizeof(sticks));

                    /*
                    sprintf_s(_message,
                            "t=%3.3f  r=%+3.3f  p=%+3.3f  y=%+3.3f\n",
                            sticks[0], sticks[1], sticks[2], sticks[3]);*/

                    // reportRates();
                }

                else {

                    if (_server->acceptConnection()) {

                        _connected = true;
                    }
                }
            }
        }

    public:

        // Constructor, called main thread
        FCrazyflieThread(
                Dynamics * dynamics,
                const char * host = "127.0.0.1",
                const short port = 5000)
            : FVehicleThread(dynamics)
        {
            // Use non-blocking socket
            _server = new TcpServerSocket(host, port, true);

            _connected = false;
        }

        ~FCrazyflieThread(void) 
        {
            if (_server) {
                _server->closeConnection();
                _connected = false;
            }

            _server = NULL;
        }

        virtual void getMessage(char * message) override 
        {
            if (_connected) {
                strcpy(message, "Connected");
            }
            else {
                strcpy(message, "Waiting for client ...");
            }
        }


}; // class FCrazyflieThread
