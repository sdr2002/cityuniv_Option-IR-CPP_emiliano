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
     *
     * From dr_t = drift(r_inf - r_t)*dt + sigma * G(.)t * dt,
     * Then the short-rate: r(t) = r0 * decay + rinf * rho + sigma * decay * integrate[exp(drift * s) * G(.)s, ds, s=0, s=t]
     *     where decay = exp[-drift * t], rho = 1 - decay
     *
     * Hence
     * P(T) = P0 * exp( +integrate[r(t), dt, t=0, t=T] )
     *      = P0 * exp(r0 * rho / drift) * exp(rinf * (T - rho/drift)) * exp(sigma * integrate[ exp(-drift * t) * integrate[ exp(drift*s) * G(.)s, dt, s=0, s=t] , dt, t=0, t=T ])
     *      = P0 * exp[(r0 * rho / drift) + rinf * (T - rho/drift)] * exp[ (sigma/drift) * sqrt{T - 0.5*(rho^2 + 2*rho)/drift} * Gauss() ]
     */
    const double dt = T/m;

    const double decay = exp(-drift * dt);
    const double rho = 1 - decay;
    const double Sinf_effect = exp(GetSinf() * (dt - rho/drift)); // s_inf * integrate[1 - exp(-drift*t), 0, T] = exp( s_inf * (T - rho/drift) )

    double St = s0;
    double Pt = GetP0();
    for (int k = 0; k < m; k++)
    {
        const double randomOutput = Gauss();

        const double S0_effect = exp(St * rho / drift);
        const double noise = exp(sigma/drift * sqrt(dt - 0.5 * (pow(rho,2.0) + 2*rho)/drift) * randomOutput);
        P[k] = Pt * S0_effect * Sinf_effect * noise;
        Pt = P[k];
        St += drift * (GetSinf() - St) * dt + sigma * randomOutput * dt;
    }
}