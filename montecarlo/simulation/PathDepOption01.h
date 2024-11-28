#ifndef PathDepOption01_h
#define PathDepOption01_h

#include <memory>

#include "../dynamics/Model.h"

class OptionToSample {
public:
    double T;
    int m;

    OptionToSample(double T_, int m_) : T(T_), m(m_) {}
    virtual ~OptionToSample() = default;

    // Monte Carlo pricing methods
    double PriceByMC(Model& model, long N);
    double PriceByMC(Model& model, long N, std::vector<double>& Sterminals);

    // Pure virtual methods for derived classes
    virtual double Payoff(SamplePath& S) = 0;

    // Virtual methods for setting strike price and creating options
    virtual void setK(double K) = 0;
    virtual std::unique_ptr<OptionToSample> createOption(double K) const = 0;
};


class ArthmAsianCall : public OptionToSample {
public:
    double K;

    ArthmAsianCall(double T_, int m_, double K_) : OptionToSample(T_, m_), K(K_) {}

    // Override to dynamically update K
    void setK(double K_) override {
        K = K_;
    }

    // Override to create a new instance with a specific K
    std::unique_ptr<OptionToSample> createOption(double K_) const override {
        return std::make_unique<ArthmAsianCall>(T, m, K_);
    }

    double Payoff(SamplePath& S) override;
};

class EuropeanCall : public OptionToSample {
public:
    double K;

    EuropeanCall(double T_, int m_, double K_) : OptionToSample(T_, m_), K(K_) {}

    void setK(double K_) override {
        K = K_;
    }

    std::unique_ptr<OptionToSample> createOption(double K_) const override {
        return std::make_unique<EuropeanCall>(T, m, K_);
    }

    double Payoff(SamplePath& S) override;
};

class EuropeanPut : public OptionToSample {
public:
    double K;

    EuropeanPut(double T_, int m_, double K_) : OptionToSample(T_, m_), K(K_) {}

    void setK(double K_) override {
        K = K_;
    }

    std::unique_ptr<OptionToSample> createOption(double K_) const override {
        return std::make_unique<EuropeanPut>(T, m, K_);
    }

    double Payoff(SamplePath& S) override;
};



#endif
