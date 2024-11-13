//
// Created by sdr2002 on 13/11/24.
//

#include "AriOUModel.h"
#include "ModelCommon.h"
#include <cmath>

void AriOUModel::GenerateSamplePath(double T, int m, SamplePath& P)
{
    /* Random sample the security price by OU process as DIFFENTIAL of the price
     * The dynamics must be negative in order to pricing the zero coupon bond.
     *
     * From dPt = drift * (Pinf - Pt)*dt + volatility * Gaussian_t() * dt,
     * d(exp[drift*t] * Pt) = drift * Pinf * exp[drift*t]*dt + volatility * exp[drift*t] * Gaussian_t() * dt
     *    where Gaussian(.) = Normal(mu=1, sigma=1)
     * Hence,
     * P(T) ~ P0 * decay + Pinf * rho + volatility * sqrt[ (1 - exp(-2 * drift * T) ) / (2 * drift)] * Gaussian_t()
     *    where decay = exp(-drift * T), rho = 1 - decay,
     * E[P(T)] = P0 * decay + Pinf * rho + volatility * sqrt[ (1 - exp(-2 * drift * T) ) / (2 * drift)] * E[Gaussian_t()]
     *         = P0 * decay + Pinf * rho
     */
    const double dt = T/m;
    const double decay = exp(-drift * dt);
    const double rho = 1 - decay;
    const double decay2 = pow(decay, 2);
    const double rho2 = 1 - decay2;

    double Pt = GetP0();
    for (int k = 0; k < m; k++)
    {
        P[k] = (Pt * decay) + (GetPinf() * rho) + sigma * sqrt( rho2 / (2 * drift)) * Gauss(); // P_{t+1}
        Pt = P[k]; // Roll to next timestep
    }
}