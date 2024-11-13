//
// Created by sdr2002 on 13/11/24.
//

#ifndef ARIOUMODEL_H
#define ARIOUMODEL_H

#include <vector>
#include <cstdlib>
#include <ctime>

#include "Model.h"

using namespace std;

class AriOUModel: public virtual Model
{
public:
    double s0; // initial short-rate: economically equivalent to OverNight repo rate of the pricing date
    double sINF;
    double drift;
    double sigma;

    AriOUModel(double P0_, double r_, double s0_, double sinf_, double drift_, double sigma_)
        : Model{P0_, r_}, s0(s0_), sINF(sinf_), drift(drift_), sigma(sigma_) { srand(time(NULL)); }

    void GenerateSamplePath(double T, int m, SamplePath& P) override;
    double GetSinf() { return sINF; }; // TODO validate if the asymptotic short-rate shall be the risk-free rate

    string toString() { return "AriOUModel"; };
};

#endif //ARIOUMODEL_H
