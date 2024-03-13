#include "pnl/pnl_vector.h"
#include "OptionVanille.hpp"
#include <algorithm>
#include <math.h>

OptionVanille ::OptionVanille(nlohmann::json &jsonParams) {
    jsonParams.at("MathPaymentDates").get_to(paymentDates_);
    jsonParams.at("Strikes").get_to(strikes_);
    nAssets_ = strikes_->size;
    nbTimeSteps_ = paymentDates_->size;
}



OptionVanille ::~OptionVanille() {
    pnl_vect_free(&paymentDates_);
    pnl_vect_free(&strikes_);
}


double OptionVanille ::payoff(const PnlMat *path, double interestRate) {
    bool C = true;
    double payoff = 0.0;
    double finalMaturity = GET(paymentDates_, nAssets_ - 1);
    double capital;
    for (size_t index = 0; index < nAssets_; index++) {
        capital = exp(interestRate * (finalMaturity - GET(paymentDates_, index)));
        payoff += capital * std::max(MGET(path, index, index)- GET(strikes_, index), 0.0) * (int)C;
        C = C && (MGET(path, index , index) <= GET(strikes_, index));
        if (!C) break;
    }
    return payoff;
}
