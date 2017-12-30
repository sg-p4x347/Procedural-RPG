#include "pch.h"
#include "Stratum.h"


Stratum::Stratum() : Stratum::Stratum(0.f,0.1f)
{
}

Stratum::Stratum(float thickness, float hardness, bool s) : Thickness(thickness),Hardness(hardness),Static(s)
{
}
