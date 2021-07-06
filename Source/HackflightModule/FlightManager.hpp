/*
   MulticopterSim FlightManager class using Hackflight

   Just spins propellers

   Copyright(C) 2019 Simon D.Levy

   MIT License
*/

#include "../MainModule/FlightManager.hpp"
#include "../MainModule/Dynamics.hpp"

#include <hfpure.hpp>

// PID controllers
#include <pidcontrollers/level.hpp>
#include <pidcontrollers/yaw.hpp>
#include <pidcontrollers/rate.hpp>
#include <pidcontrollers/althold.hpp>
#include <pidcontrollers/poshold.hpp>

#include "SimReceiver.hpp"
#include "SimBoard.hpp"
#include "SimSensors.hpp"
#include "SimMotor.hpp"

class FHackflightFlightManager : public FFlightManager {

    private:

        // PID controllers
		hf::RatePid ratePid = hf::RatePid(0.225, 0.001875, 0.375);
		hf::YawPid yawPid = hf::YawPid(1.0625, 0.005625);
        hf::LevelPid levelPid = hf::LevelPid(0.20);
        hf::AltitudeHoldPid altHoldPid;
        hf::PositionHoldPid posHoldPid;

        // "Board"
        SimBoard _board;

        // "Receiver" (joystick/gamepad)
        SimReceiver * _receiver = NULL;

        // "Sensors"
        SimSensors* _sensors = NULL;

        // "Motors" are passed to mixer so it can modify them
        SimMotor * _motors[100] = {};

        // Main firmware
        hf::HackflightPure * _hackflight = NULL;

    public:

        // Constructor
        FHackflightFlightManager(APawn * pawn, hf::Mixer * mixer, SimMotor ** motors, Dynamics * dynamics) 
            : FFlightManager(dynamics) 
        {
            // Store motors for later
            for (uint8_t k=0; k<_actuatorCount; ++k) {
                _motors[k] = motors[k];
            }

            // Pass PlayerController to receiver constructor in case we have no joystick / game-controller
            _receiver = new SimReceiver(UGameplayStatics::GetPlayerController(pawn->GetWorld(), 0));

            // Create Hackflight object
            _hackflight = new hf::HackflightPure(&_board, _receiver, mixer);

            // Add simulated sensor suite
            _sensors = new SimSensors(_dynamics);
            _hackflight->addSensor(_sensors);

            // Add PID controllers for all aux switch positions.
            // Position hold goes first, so it can have access to roll and yaw
            // stick demands before other PID controllers modify them.
            _hackflight->addClosedLoopController(&posHoldPid);
            _hackflight->addClosedLoopController(&ratePid);
            _hackflight->addClosedLoopController(&yawPid);
            _hackflight->addClosedLoopController(&levelPid);
            _hackflight->addClosedLoopController(&altHoldPid);

            // Start Hackflight firmware, indicating already armed
            _hackflight->begin(true);
        }

        virtual ~FHackflightFlightManager(void)
        {
            delete _hackflight;
        }

        virtual void getActuators(const double time, double * values) override
        {
            // Update the "receiver" (joystick or game controller)
            _receiver->update();

            // Update the Hackflight firmware, causing Hackflight's actuator
            // to set the values of the simulated motors
            _hackflight->update();

            // Set the time in the simulated board, so it can be retrieved by Hackflight
            _board.set(time);

            //  Get the new motor values
            for (uint8_t i=0; i < _actuatorCount; ++i) {
                values[i] = _motors[i]->getValue();
            }
        }

        void tick(void)
        {
            _receiver->tick();
        }

}; // HackflightFlightManager
