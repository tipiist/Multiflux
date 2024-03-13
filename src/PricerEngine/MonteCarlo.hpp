#pragma once

#include "OptionMultiFlux.hpp"
#include "BlackScholesModel.hpp"
#include "pnl/pnl_random.h"

class MonteCarlo {
public:
    BlackScholesModel *mod_; 
    OptionMultiFlux *opt_;            
    PnlRng *rng_;            
    double fdStep_;          
    int nbSamples_;
    PnlVect *computeHelper_;
    PnlMat *shift_path_left;
    PnlMat *shift_path_right;

    /**
     * Constructeur
     */
    MonteCarlo(BlackScholesModel *mod, OptionMultiFlux *opt, PnlRng *rng, double fdStep, int nbSamples);

    /**
     * Destructeur
     */
    ~MonteCarlo();

    void price(const PnlMat *past, double t, bool isMonitoringDate, double &prix, double &std_dev);

    void delta(const PnlMat *past, double t, bool isMonitoringDate, PnlVect * delta, PnlVect *std_dev);
    void deltaComputeSum(PnlVect *delta, PnlVect *std_dev, PnlMat *path, double t);

    void deltaComputeValue(PnlVect *delta, PnlVect *std_dev, PnlVect *spot_, double t);
};

