//
// Created by sdr2002 on 29/10/24.
//

#include "Options09.h"
#include "../lecture2/BinModel02.h"
#include "BinLattice02.h"
#include <iostream>

using namespace std;

double EurOption::PriceByCRR(BinModel Model) {
    double q=Model.RiskNeutProb();
    int N=GetN();
    vector<double> Price(N+1);
    for (int i=0; i<=N; i++) {
        Price[i] = Payoff(Model.S(N, i));
    }
    for (int n=N-1; n>=0; n--) {
        for (int i=0; i<=n; i++) {
            Price[i] = (q * Price[i+1] + (1-q) * Price[i]) / (1 + Model.GetR());
        }
    }
    return Price[0];
}

double AmOption::PriceBySnell(
    BinModel Model,
    BinLattice<double>& PriceTree,
    BinLattice<bool>& StoppingTree)
{
    double q=Model.RiskNeutProb();
    int N=GetN();
    PriceTree.SetN(N);
    StoppingTree.SetN(N);

    double ContVal;
    for (int i=0; i<=N; i++) {
        PriceTree.SetNode(N, i, Payoff(Model.S(N, i)));
        StoppingTree.SetNode(N, i, 1);
    }
    for (int n = N - 1; n >= 0; n--)
    {
        for (int i = 0; i <= n; i++)
        {
            ContVal = (q * PriceTree.GetNode(n+1, i+1) + (1 - q) * PriceTree.GetNode(n+1, i)) / (1 + Model.GetR());
            PriceTree.SetNode(n, i, Payoff(Model.S(N, i)));
            StoppingTree.SetNode(n, i, 1);

            if (ContVal > PriceTree.GetNode(n, i)) {
                PriceTree.SetNode(n, i, ContVal);
                StoppingTree.SetNode(n, i, 0);
            } else if (PriceTree.GetNode(n, i) == 0.0) {
                StoppingTree.SetNode(n, i, 0);
            }
        }
    }
    return PriceTree.GetNode(0, 0);
}

int Call::GetInputGridParameters()
{
    cout << "Call - Enter call option data:" << endl;
    int N;
    cout << "Enter steps to expiry N: "; cin >> N;
    EurOption::SetN(N); AmOption::SetN(N);
    cout << "Enter strike price K: "; cin >> K;
    cout << endl;
    return 0;
}

int Call::GetDefaultGridParameters() {
    const int N = 6;
    const double K = 100;
    cout << "Call - Getting default data as "
         << "N=" << N << ", K=" << K
         << endl;
    EurOption::SetN(N); AmOption::SetN(N);
    SetK(K);
    return 0;
}

double Call::Payoff(double z)
{
    if (z > K) return z - K;
    return 0.0;
}

int Put::GetInputGridParameters()
{
    cout << "Put - Enter put option data:" << endl;
    int N;
    cout << "Enter steps to expiry N: "; cin >> N;
    EurOption::SetN(N); AmOption::SetN(N);
    cout << "Enter strike price K: "; cin >> K;
    cout << endl;
    return 0;
}

int Put::GetDefaultGridParameters() {
    const int N = 6;
    const double K = 100;
    cout << "Put - Getting default data as "
         << "N=" << N << ", K=" << K
         << endl;
    EurOption::SetN(N); AmOption::SetN(N);
    SetK(K);
    return 0;
}

double Put::Payoff(double z)
{
    if (z < K) return K - z;
    return 0.0;
}

double KnockOutCall::Payoff(double z) {
    if (z > K) {
        if (z > Barrier) return 0.0;
        return z - K;
    }
    return 0.0;
}

int KnockOutCall::GetInputGridParameters()
{
    cout << "KnockOutCall - Enter option data:" << endl;
    int N;
    cout << setw(5) << "Enter steps to expiry N: "; cin >> N;
    EurOption::SetN(N); AmOption::SetN(N);
    cout << setw(5) << "Enter strike price K: "; cin >> K;
    cout << setw(5) << "Enter barrier price K: "; cin >> K;
    cout << endl;
    return 0;
}

int KnockOutCall::GetDefaultGridParameters() {
    const int N = 6;
    const double K = 100;
    const double Barrier = 135;
    cout << "KnockOutCall - Getting default data as "
         << "N=" << N << ", K=" << K << ", Barrier=" << Barrier
         << endl;
    EurOption::SetN(N); AmOption::SetN(N);
    SetK(K);
    SetBarrier(Barrier);
    return 0;
}

int CallSpread::GetDefaultGridParameters() {
    const int N = 6;
    const double Kl = 100;
    const double Ks = 135;
    cout << "CallSpread - Getting default data as "
         << "N=" << N << ", Kl=" << Kl << ", Ks=" << Ks
         << endl;

    this->SetN(N);
    this->SetK(Kl, Ks);
    return 0;
}
