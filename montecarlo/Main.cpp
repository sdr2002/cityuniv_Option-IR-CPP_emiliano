#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <cmath>
#include <filesystem>

#include "dynamics/AriOUModel.h"
#include "dynamics/GeoOUModel.h"
#include "dynamics/BSModel01.h"

#include "simulation/PathDepOption01.h"

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

void render_over_KRange(
    Model& modelDynamics,
    OptionToSample& optionBase,
    long N = 30000, bool RecordTerminalStockPrice = false
) {
    cout << endl << "Rendering " << modelDynamics.toString() << ":" << endl;

    vector<double> StrikeRange;
    vector<double> OptionPVs;

    vector<double> Sterminals;

    for (double K = 75; K <= 125; K += 5.0) {
        StrikeRange.push_back(K);

        OptionToSample* currentOption;
        // Use the `createOption` method of the derived class
        // Option 1: Use optionBase with setK()
        optionBase.setK(K); currentOption = &optionBase;
        // Option 2: Dynamically create a new option instance
        // unique_ptr option(optionBase.createOption(K)); currentOption = option.get();// slow

        if (!RecordTerminalStockPrice) {
            OptionPVs.push_back(currentOption->PriceByMC(modelDynamics, N, Sterminals));
            RecordTerminalStockPrice = true;
        } else {
            OptionPVs.push_back(currentOption->PriceByMC(modelDynamics, N));
        }
    }

    cout << setw(10) << "K"
         << setw(20) << "PV_Eur"
         << endl;
    for (size_t i=0; i<StrikeRange.size(); i++) {
        cout << setw(10) << StrikeRange[i]
             << setw(20) << OptionPVs[i]
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
    // double T = 1.0 / 12.0; // Expiry is 1 month.
    // int m = 30;  // Observations for one month

    double K = 100.0;
    double T = 3.0 / 12.0; // Expiry is 1 quarter.
    int m = 13;            // Weeky observations for 3 months!
    EuropeanCall eurCallShortTerm(T, m, K);

    long N = 30000; // Number of paths to sample

    /* Scenario 2: BS process stock dynamics model */
    BSModel bsModel(100.0, 0.03, 0.2);
    render_over_KRange(bsModel, eurCallShortTerm, N);

    /* Scenario 2: OU process stock dynamics model */
    AriOUModel ariOuModel(100.0, 0.03, 100.0, 52*log(2), 20.);
    render_over_KRange(ariOuModel, eurCallShortTerm, N);

    /* Scenario 3: Geometric OU model
     * Note lognormal distribution is only observable with extreme sigma values like 25. for short-term projection,
     * which is not realistic as it means the short-rate's volatility is 2500% per annum.
     * sigma around 0.02 ~ 0.1 is more like a realistic number to represent the bond dynamics or such,
     * TODO cross-check the legitimacy of the formula of GeoOUModel sampling*/
    GeoOUModel geoOuModel(100.0, 0.03, 0.045, 0.02, 2*log(2), 0.2);
    // render_over_KRange(geoOuModel, eurCallShortTerm, N);
    EuropeanCall eurCallLongTerm(10.0, 10*52, K);
    render_over_KRange(geoOuModel, eurCallLongTerm, N);
}