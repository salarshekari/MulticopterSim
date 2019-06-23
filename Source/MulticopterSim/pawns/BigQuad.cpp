/*
* Class implementation for big quadcopter pawn class in MulticopterSim
*
* Copyright (C) 2019 Simon D. Levy
*
* MIT License
*/

#include "BigQuad.h"

#include "UObject/ConstructorHelpers.h"
#include "GameFramework/SpringArmComponent.h"

//
// Structures to hold static mesh initializations
DECLARE_STATIC_MESH(FFrameStatics,  "BigQuad/Frame.Frame",     FrameStatics)
DECLARE_STATIC_MESH(FMotorStatics,  "BigQuad/Motor.Motor",     MotorStatics)
DECLARE_STATIC_MESH(FProp1WStatics, "BigQuad/PropCCW.PropCCW", Prop1Statics)
DECLARE_STATIC_MESH(FProp2WStatics, "BigQuad/PropCCW.PropCCW", Prop2Statics)
DECLARE_STATIC_MESH(FProp3WStatics, "BigQuad/PropCW.PropCW",   Prop3Statics)
DECLARE_STATIC_MESH(FProp4WStatics, "BigQuad/PropCW.PropCW",   Prop4Statics)

// Constructor
ABigQuadPawn::ABigQuadPawn()
{
    Vehicle::objects_t objects = {0};

    objects.pawn = this;
    objects.frameMesh = FrameStatics.mesh.Get();
    objects.motorMesh = MotorStatics.mesh.Get();

    QuadXAP::build(objects, _layout,
            Prop1Statics.mesh.Get(), Prop2Statics.mesh.Get(), Prop3Statics.mesh.Get(), Prop4Statics.mesh.Get()); 

    _vehicle = new QuadXAP(objects, _params);
}

ABigQuadPawn::~ABigQuadPawn()
{
}

void ABigQuadPawn::PostInitializeComponents()
{
    _vehicle->PostInitializeComponents();

    Super::PostInitializeComponents();
}

// Called when the game starts or when spawned
void ABigQuadPawn::BeginPlay()
{
    fprintf(stderr, "ABigQuadPawn::BeginPlay\n");

    _vehicle->BeginPlay();

    Super::BeginPlay();
}

void ABigQuadPawn::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    fprintf(stderr, "ABigQuadPawn::EndPlay\n");

    _vehicle->EndPlay();

    Super::EndPlay(EndPlayReason);
}

// Called automatically on main thread
void ABigQuadPawn::Tick(float DeltaSeconds)
{
    _vehicle->Tick();

    Super::Tick(DeltaSeconds);
}
