#include "MonteCarlo.hpp"
#include <cmath>

MonteCarlo::MonteCarlo(BlackScholesModel *mod, OptionMultiFlux *opt, PnlRng *rng, double fdStep, int nbSamples)
{
    mod_ = mod;
    opt_ = opt;
    rng_ = rng;
    fdStep_ = fdStep;
    nbSamples_ = nbSamples;

    computeHelper_ = pnl_vect_create(opt_->nAssets_);
    shift_path_left = pnl_mat_create(opt->nbTimeSteps_, opt_->nAssets_);
    shift_path_right = pnl_mat_create(opt->nbTimeSteps_ , opt_->nAssets_);
}


MonteCarlo::~MonteCarlo() {
    pnl_vect_free(&computeHelper_);
    pnl_mat_free(&shift_path_left);
    pnl_mat_free(&shift_path_right);
}

void MonteCarlo ::price(const PnlMat *past, double t, bool isMonitoringDate, double &prix, double &std_dev) {
    auto sum = 0.0;
    auto square_sum = 0.0;
    auto nb_steps = opt_->nbTimeSteps_;
    double payoff_result;
    double act = exp(-(mod_->interestRate_) * (GET(mod_->paymentDates_, mod_->paymentDates_->size - 1) - t ));
    double act_sq = exp(-2*(mod_->interestRate_) * (GET(mod_->paymentDates_, mod_->paymentDates_->size - 1) - t));
    {
        PnlMat *path = pnl_mat_create_from_zero(nb_steps, opt_->nAssets_);
        for (int i = 0; i < nbSamples_; ++i) {
            mod_->asset(path, t, isMonitoringDate, rng_, past, opt_->nbTimeSteps_);
            payoff_result = opt_->payoff(path, mod_->interestRate_);
            sum += payoff_result;
            square_sum += payoff_result * payoff_result;
        }
        pnl_mat_free(&path);
    }
    prix =  (act*sum) / (double)nbSamples_;
    std_dev =  ((act_sq * square_sum) / (double)nbSamples_) - (prix) * (prix);
    std_dev = sqrt(std::abs(std_dev) / (double)nbSamples_);
}

void MonteCarlo::delta(const PnlMat *past, double t, bool isMonitoringDate, PnlVect *delta, PnlVect *std_dev) {
    auto nb_steps = opt_->nbTimeSteps_;
    {
        PnlMat *path = pnl_mat_create(nb_steps, opt_->nAssets_);
        for (int i = 0; i < nbSamples_; i++) {
            mod_->asset(path, t, isMonitoringDate, rng_, past, opt_->nbTimeSteps_);
            deltaComputeSum(delta, std_dev, path, t);
        }
        pnl_mat_free(&path);
    }

    PnlVect *spot = pnl_vect_create(opt_->nAssets_);

    pnl_mat_get_row(spot, past, past->m - 1);

    deltaComputeValue(delta, std_dev, spot, t);

    pnl_vect_free(&spot);
}


void MonteCarlo ::deltaComputeSum(PnlVect *delta, PnlVect *std_dev, PnlMat *path, double t) {
    double difference;
    double intermediate_value;
    for (int d = 0; d < opt_->nAssets_; d++) {
        mod_->shiftAsset(shift_path_left, path, d, fdStep_, t);
        difference = opt_->payoff(shift_path_left, mod_->interestRate_);
        mod_->shiftAsset(shift_path_right, path, d, -fdStep_, t);
        difference = difference - opt_->payoff(shift_path_right, mod_->interestRate_);

        {
            intermediate_value = GET(delta, d) + difference;
            LET(delta, d) = intermediate_value;
        }

        {
            intermediate_value = GET(std_dev, d) + difference * difference;
            LET(std_dev, d) = intermediate_value;
        }
    }
}

void MonteCarlo ::deltaComputeValue(PnlVect *delta, PnlVect *std_dev, PnlVect *spot_t, double t) {
    pnl_vect_div_scalar(delta, 2 * fdStep_ * nbSamples_);
    pnl_vect_div_vect_term(delta, spot_t);

    pnl_vect_clone(computeHelper_, delta);
    pnl_vect_mult_vect_term(computeHelper_, computeHelper_);

    pnl_vect_div_scalar(std_dev, (2 * fdStep_) * (2 * fdStep_) * nbSamples_);
    pnl_vect_div_vect_term(std_dev, spot_t);
    pnl_vect_div_vect_term(std_dev, spot_t);

    pnl_vect_minus_vect(std_dev, computeHelper_);

    double T = pnl_vect_get(opt_->paymentDates_, opt_->paymentDates_->size - 1);
    pnl_vect_mult_scalar(delta, exp(-1 * mod_->interestRate_ * (T - t)));
    pnl_vect_mult_scalar(std_dev, exp(-2 * mod_->interestRate_ * (T - t)) / nbSamples_);
    pnl_vect_map_inplace(std_dev, abs);
    pnl_vect_map_inplace(std_dev, sqrt);
}
