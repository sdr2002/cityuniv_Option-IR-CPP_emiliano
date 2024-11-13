#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <cmath>

#include "AriOUModel.h"
#include "GeoOUModel.h"
#include "PathDepOption01.h"

using namespace std;

int evaluateWithBlackScholesDynamics()
{
    cout << endl << "Running BSModel dynamics..." << endl;
    double S0 = 100.0, r = 0.03, sigma = 0.2;
    BSModel bsModel(S0, r, sigma);

    double T = 1.0 / 12.0, K = 100.0; // Expiry is 1 month.
    int m = 30;                       // Daily observations for one month!

    long N = 30000;

    ArthmAsianCall AriAsiCallOption(T, K, m);
    cout << "Arithmetic Asian Call Price = " << AriAsiCallOption.PriceByMC(bsModel, N) << endl;

    // lecture 4. Exercise 1
    EuropeanCall EurCallOption(T, K, m);
    cout << "European Call Price = " << EurCallOption.PriceByMC(bsModel, N) << endl;
    EuropeanCall EurPutOption(T, K, m);
    cout << "European Put Price = " << EurPutOption.PriceByMC(bsModel, N) << endl;

    return 0;
}

int evaluateWithOrnsteinUhlenbeckDynamics()
{
    cout << endl << "Running GeoOUModel dynamics..." << endl;
    double P0 = 100.0, r=0.03, s0=0.045, sINF=0.0, drift = 4*std::log(2.0), sigma = 0.2;
    GeoOUModel ouModel(P0, r, s0, sINF, drift, sigma);

    double T = 1.0 / 12.0, K = 100.0; // Expiry is 1 month.
    int m = 30;                       // Daily observations for one month!

    long N = 30000;

    ArthmAsianCall AriAsiCallOption(T, K, m);
    cout << "Arithmetic Asian Call Price = " << AriAsiCallOption.PriceByMC(ouModel, N) << endl;

    // lecture 4. Exercise 1
    EuropeanCall EurCallOption(T, K, m);
    cout << "European Call Price = " << EurCallOption.PriceByMC(ouModel, N) << endl;
    EuropeanCall EurPutOption(T, K, m);
    cout << "European Put Price = " << EurPutOption.PriceByMC(ouModel, N) << endl;

    return 0;
}

void render_over_KRange(Model& modelDynamics) {

    cout << endl << "Rendering " << modelDynamics.toString() << ":" << endl;
    // double T = 1.0 / 12.0, K = 100.0; // Expiry is 1 month.
    // int m = 30;                       // Daily observations for one month!

    double T = 3.0 / 12.0, K = 100.0; // Expiry is 1 month.
    int m = 13;                       // Daily observations for one month!

    long N = 30000;

    vector<double> StrikeRange;
    vector<double> EuropeanPVs;

    vector<double> Sterminals;
    bool TerminalStockPriceRecorded = false;
    for (double K=75; K<=125; K+=5.0) {
        StrikeRange.push_back(K);

        EuropeanCall EurCallOption(T, K, m);
        if (!TerminalStockPriceRecorded) {
            EuropeanPVs.push_back(EurCallOption.PriceByMC(modelDynamics, N, Sterminals));
            TerminalStockPriceRecorded = true;
        }
        else {
            EuropeanPVs.push_back(EurCallOption.PriceByMC(modelDynamics, N));
        }
    }

    cout << setw(10) << "K"
         << setw(20) << "PV_Eur"
         << endl;
    for (size_t i=0; i<StrikeRange.size(); i++) {
        cout << setw(10) << StrikeRange[i]
             << setw(20) << EuropeanPVs[i]
             << endl;
    }

    string fname = "Sterminals_" + modelDynamics.toString() +".csv";
    ofstream SterminalsFile("/home/sdr2002/dev/cityuniv_Option-IR-CPP_emiliano/montecarlo/" + fname);
    ostream_iterator<double> out_it (SterminalsFile,"\n");
    copy(Sterminals.begin(), Sterminals.end(), out_it);
    cout << "Sterminals saved to " + fname << endl;
}


int main() {
    // evaluateWithBlackScholesDynamics();
    // evaluateWithOrnsteinUhlenbeckDynamics();

    GeoOUModel geoOuModel(100.0, 0.03, 0.045, 0.06, 12*log(2), 0.2);
    render_over_KRange(geoOuModel);

    AriOUModel ariOuModel(100.0, 0.03, 0.045, 100.0, 12*log(2), 0.2);
    render_over_KRange(ariOuModel);

    BSModel bsModel(100.0, 0.05, 0.2);
    render_over_KRange(bsModel);
}