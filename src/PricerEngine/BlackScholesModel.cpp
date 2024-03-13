#include "BlackScholesModel.hpp"
#include <iostream>
#include <string>
#include <fstream>
#include <cmath>

using namespace std;

BlackScholesModel::BlackScholesModel(int nAssets, double interestRate, PnlVect *paymentDates, PnlVect *strikes, PnlMat *volatility) {
    
    volatility_ = pnl_mat_create(nAssets, nAssets);
    pnl_mat_clone(volatility_, volatility);
    paymentDates_ = paymentDates;
    strikes_ = strikes;
    nAssets_ = nAssets;
    interestRate_ = interestRate;
    G_ = pnl_vect_create(nAssets_);
    Ld_ = pnl_vect_create(nAssets_);
    computationHelper_ = pnl_vect_create_from_zero(nAssets_);
}

BlackScholesModel::~BlackScholesModel() {
    pnl_mat_free(&volatility_);
    pnl_vect_free(&G_);
    pnl_vect_free(&Ld_);
    pnl_vect_free(&computationHelper_);
}

void BlackScholesModel ::asset(PnlMat *path, double t, bool isMonitoringDate, PnlRng *rng, const PnlMat *past, int nbTimeSteps) {
    int indexNextPaymentDate = 0;

    while (indexNextPaymentDate < paymentDates_->size) {
        if (GET(paymentDates_, indexNextPaymentDate) > t) {
            break;
        }
        indexNextPaymentDate += 1;
    }
    pnl_mat_set_subblock(path, past, 0, 0);

    pnl_mat_get_row(computationHelper_, past, past->m-1);
    int indiceMin = std::min(indexNextPaymentDate, paymentDates_->size - 1);
    double dt = std::max(GET(paymentDates_, indiceMin) - t,0.0);
    stepSimulation(computationHelper_, dt, rng);
    if (isMonitoringDate) {
        pnl_mat_set_row(path, computationHelper_, indexNextPaymentDate);
    } else {
        pnl_mat_set_row(path, computationHelper_, past->m-1);
    }

    for (size_t index = indexNextPaymentDate+1; index < nbTimeSteps; index++) {
        dt = GET(paymentDates_, index) - GET(paymentDates_, index - 1);
        stepSimulation(computationHelper_, dt, rng);
        pnl_mat_set_row(path, computationHelper_, (int)index);
    }
}

void BlackScholesModel ::stepSimulation(PnlVect *current_spot, double dt, PnlRng *rng) {
    pnl_vect_rng_normal_d(G_, nAssets_, rng);
    PnlVect *to_prod = pnl_vect_new();
    double sigma_d;
    double si_m1_d;
    double spot_value;
    PnlVect *rowLd = pnl_vect_create_from_zero(nAssets_);
    for (int d = 0; d < nAssets_; d++) {
        pnl_mat_get_row(rowLd, volatility_, d);
        sigma_d = pnl_vect_norm_two(rowLd); 
        si_m1_d = GET(current_spot, d);
        pnl_mat_get_row(to_prod, volatility_, d);
        double random_term = pnl_vect_scalar_prod(to_prod, G_);
        spot_value = si_m1_d * exp((interestRate_ - (sigma_d * sigma_d / 2)) * dt +  sqrt(dt) * random_term);
        pnl_vect_set(current_spot, d, spot_value);
    }
    pnl_vect_free(&rowLd);
    pnl_vect_free(&to_prod);
}


void BlackScholesModel::shiftAsset(PnlMat *shift_path, const PnlMat *path, int d, double h, double t) {
    int indexNextPaymentDate = 0;

    while (indexNextPaymentDate < paymentDates_->size) {
        if (GET(paymentDates_, indexNextPaymentDate) > t) {
            break;
        }
        indexNextPaymentDate += 1;
    }

    pnl_mat_clone(shift_path, path);

    for (int i = indexNextPaymentDate; i < path->m; i++) {
        MLET(shift_path, i, d) *= 1. + h;
    }
}


