#include "pch.h"
#include "MovementController.h"

namespace world {
	MovementController::MovementController() : Heading(Vector3::Zero), Jump(false), Speed(0.f)
	{
	}
}