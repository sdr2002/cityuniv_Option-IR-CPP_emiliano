#include "PathDepOption01.h"
#include <cmath>

double PathDepOption::PriceByMC(Model& Model, long N)
{
    double H = 0.0;
    SamplePath P(m);
    for (long i = 0; i < N; i++)
    {
        Model.GenerateSamplePath(T, m, P);
        const double Hi = Payoff(P);
        H += Hi;
    }
    const double ExpectedH = H / N;

    const double DiscountFactor = exp(-Model.GetR() * T);
    return DiscountFactor * ExpectedH;
}

double PathDepOption::PriceByMC(Model& Model, long N, vector<double>& Sterminals)
{
    double H = 0.0;
    SamplePath P(m);
    for (long i = 0; i < N; i++)
    {
        Model.GenerateSamplePath(T, m, P);
        Sterminals.push_back(P.back());
        const double Hi = Payoff(P);
        H += Hi;
    }
    const double ExpectedH = H / N;

    const double DiscountFactor = exp(-Model.GetR() * T);
    return DiscountFactor * ExpectedH;
}

double ArthmAsianCall::Payoff(SamplePath& P)
{
    double Ave = 0.0;
    for (int k = 0; k < m; k++) Ave += P[k];
    Ave /= m;
    return max(Ave - K, 0.0);
}

double EuropeanCall::Payoff(SamplePath& P)
{
    double Sf = P.back();
    return max(Sf - K, 0.0);
}

double EuropeanPut::Payoff(SamplePath& P)
{
    double Sf = P.back();
    return max(Sf - K, 0.0);
}