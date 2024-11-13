//
// Created by sdr2002 on 12/11/24.
//

#include "GeoOUModel.h"
#include "ModelCommon.h"
#include <cmath>

void GeoOUModel::GenerateSamplePath(double T, int m, SamplePath& P)
{
    /* Random sample the security price by OU process as GROWTH rate of the price
     * The dynamics must be negative in order to pricing the zero coupon bond
     */
    const double dt = T/m;

    const double decay = exp(-drift * dt);
    const double rho = 1 - decay;

    const double sINFeffect = exp(GetSinf() * (dt - rho/drift)); // s_inf * integrate[1 - exp(-drift*t), 0, T] = exp( s_inf * (T - rho/drift) )

    double Pcurrent = GetP0();
    double Scurrent = s0;
    vector<double> S; // short-rate path
    for (int k = 0; k < m; k++)
    {
        const double randomOutput = Gauss();

        const double s0effect = exp(Scurrent/drift * rho); // s_0 * integrate[exp(-drift*t), t, 0, T] = exp( s_0 * rho/drift )
        /* volatility * integrate[ exp(-drift * t) * integrate[ exp(drift*s) * dW(s), s, 0, t], t, 0, T ]
         * Variance = ( volatility/drift )^2 * (T - 0.5/drift(rho^2 + 2*rho)) */
        const double var = pow(sigma/drift, 2.0)  * (dt - 0.5 * (pow(rho,2.0) + 2*rho)/drift);
        const double noise = exp(sqrt(var) * Gauss());
        P[k] = Pcurrent * s0effect * sINFeffect * noise;

        Scurrent += (-drift * Scurrent * dt + sigma * randomOutput) * dt;
        Pcurrent = P[k];
    }
}