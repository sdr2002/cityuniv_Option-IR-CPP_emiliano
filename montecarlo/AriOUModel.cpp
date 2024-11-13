//
// Created by sdr2002 on 13/11/24.
//

#include "AriOUModel.h"
#include "ModelCommon.h"
#include <cmath>

void AriOUModel::GenerateSamplePath(double T, int m, SamplePath& P)
{
    /* Random sample the security price by OU process as DIFFENTIAL of the price
     * The dynamics must be negative in order to pricing the zero coupon bond
     */
    const double dt = T/m;
    const double decay = exp(-drift * dt);
    const double rho = 1 - decay;

    double Pcurrent = GetP0();
    for (int k = 0; k < m; k++)
    {
        P[k] = (Pcurrent * decay) + (GetSinf() * rho) + sigma * (sqrt(0.5 * (1 - pow(decay,2)) / drift) * Gauss()); // S_{t+1}
        Pcurrent = P[k]; // Roll to next timestep
    }
}