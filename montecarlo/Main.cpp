#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <cmath>
#include <filesystem>

#include "dynamics/AriOUModel.h"
#include "dynamics/GeoOUModel.h"
#include "dynamics/BSModel01.h"

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

void render_over_KRange(Model& modelDynamics, double T, int m, double K=100.0, long N=30000) {

    cout << endl << "Rendering " << modelDynamics.toString() << ":" << endl;

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

    // current_path is ${repo_directory}/${cmake_output_directory}, hence going to parent then append 'montecarlo' to direct to the scripts folder
    filesystem::path run_dir = filesystem::current_path().parent_path() / filesystem::path("montecarlo");
    string fname = "Sterminals_" + modelDynamics.toString() +".csv";
    ofstream SterminalsFile(run_dir /"output"/ fname);
    ostream_iterator<double> out_it (SterminalsFile,"\n");
    copy(Sterminals.begin(), Sterminals.end(), out_it);
    cout << "Sterminals saved to " + fname << endl;
}


int main() {
    // evaluateWithBlackScholesDynamics();
    // evaluateWithOrnsteinUhlenbeckDynamics();
    double K = 100.0;
    // double T = 1.0 / 12.0; // Expiry is 1 month.
    // int m = 30;  // Observations for one month

    double T = 3.0 / 12.0; // Expiry is 1 quarter.
    int m = 13;            // Daily observations for 3 months!

    long N = 30000;

    BSModel bsModel(100.0, 0.03, 0.2);
    render_over_KRange(bsModel, T, m, K, N);

    AriOUModel ariOuModel(100.0, 0.03, 100.0, 52*log(2), 20.);
    render_over_KRange(ariOuModel, T, m, K, N);

    /* Note sigma=25. is not realistic as it means the short-rate's volatility is 2500% per annum.
     * sigma around 0.02 ~ 0.1 is more like a realistic number to represent the bond dynamics or such,
     * but I put sigma=25. to provide the representative output of the stock distribution showing log-normal.
     * TODO cross-check the legitimacy of the formula of GeoOUModel sampling*/
    GeoOUModel geoOuModel(100.0, 0.03, 0.045, 0.02, 52*log(2), 25.);
    render_over_KRange(geoOuModel, T, m, K, N);
}