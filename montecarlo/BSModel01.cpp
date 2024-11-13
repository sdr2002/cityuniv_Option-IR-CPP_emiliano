#include "BSModel01.h"
#include "ModelCommon.h"
#include <cmath>


void BSModel::GenerateSamplePath(double T, int m, SamplePath& P)
{
    /* Pt = P0 * exp[(r - 0.5 * sigma^2) * t] * exp[sigma * sqrt(t) * Gauss(.)]  */
    const double dt = T/m;

    double Pt = GetP0();
    for (int k = 0; k < m; k++)
    {
        P[k] = Pt * exp((GetR() - 0.5 * pow(sigma, 2)) * dt + sigma * sqrt(dt) * Gauss());
        Pt = P[k];
    }
}
