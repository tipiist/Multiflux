#pragma once

#include <iostream>
#include <nlohmann/json.hpp>
#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"
#include "pnl/pnl_random.h"
#include "MonteCarlo.hpp"
#include "BlackScholesModel.hpp"
#include "OptionMultiFlux.hpp"
 

class BlackScholesPricer {
public:
    PnlMat *volatility;
    PnlVect *paymentDates;
    PnlVect *strikes;
    int nAssets;
    double interestRate;
    double fdStep;
    int nSamples;
    OptionMultiFlux *opt_;
    PnlRng *rng_;
    BlackScholesModel *bs_;

    BlackScholesPricer(nlohmann::json &jsonParams);
    ~BlackScholesPricer();
    void priceAndDeltas(const PnlMat *past, double currentDate, bool isMonitoringDate, double &price, double &priceStdDev, PnlVect* &deltas, PnlVect* &deltasStdDev);
    void print();
};
