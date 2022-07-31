struct VehicleState {

    //pub z:      f32,
    //pub dz:     f32,
    //pub dphi:   f32,
    //pub dtheta: f32,
    //pub dpsi:   f32
}

struct Demands {

    pub throttle: f32,
    pub roll:     f32,
    pub pitch:    f32,
    pub yaw:      f32
} 

enum PidController {

    Altitude{error_integral:f32},

    Angle{s:f32}
}

fn run_alt_hold(alt_pid:PidController, _demands:&Demands, _vstate:&VehicleState)
    ->  (Demands, PidController) {

    (Demands {
        throttle:0.0,
        roll:0.0,
        pitch:0.0,
        yaw:0.0}, 
        alt_pid)
}

fn update(
    pid:PidController,
    demands:&Demands,
    vstate:&VehicleState) -> (Demands, PidController) {

    match pid {

        PidController::Altitude{error_integral: _} =>
            run_alt_hold(pid, demands, vstate),

        PidController::Angle{s} =>
            (Demands {
                throttle:demands.throttle, 
                roll:demands.roll, 
                pitch:demands.pitch, 
                yaw:demands.yaw 
            },
            PidController::Angle{s:s})
    }
}

fn main() {

    let demands = Demands {
        throttle:0.0,
        roll:0.0,
        pitch:0.0,
        yaw:0.0
    };

    let vstate = VehicleState {
        //z:0.0,
        //dz:0.0,
        //dphi:0.0,
        //dtheta:0.0,
        //dpsi:0.0
    };

    let alt_pid = PidController::Altitude{error_integral:0.0};
    let angle_pid = PidController::Angle{s:2.0};

    let (_x, _new_alt_pid) = update(alt_pid, &demands, &vstate);
    let (_y, _new_angle_pid) = update(angle_pid, &demands, &vstate);

    //println!("{}, {}", x, y)
}
