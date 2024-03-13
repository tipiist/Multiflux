#pragma once

#include "pnl/pnl_random.h"
#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"

/// \brief Modèle de Black Scholes
class BlackScholesModel {
public:
    PnlMat *volatility_; /// matrice des volatilités
    PnlVect *paymentDates_; /// Dates de payements; Maturités
    PnlVect *strikes_; /// Les prix d'exercices à chaque date
    int nAssets_;       /// nombre d'actifs du modèle
    double interestRate_;       /// taux d'intérêt
    PnlVect *G_;
    PnlVect *Ld_;
    PnlVect *computationHelper_;

    /**
     *Constructeur
     */
    BlackScholesModel(int nAssets, double interestRate, PnlVect *paymentDates, PnlVect *strikes, PnlMat *volatility);

    ~BlackScholesModel();

    /**
     * Génération des trajectoires à partir d'une matrice past (prix des sousjacents aux dates de constatations précédetes à t)
     */
    void asset(PnlMat *path, double t, bool isMonitoringDate, PnlRng *rng, const PnlMat *past, int nbTimeSteps);

    /**
     * Shift d'une trajectoire du sous-jacent
     */
    void shiftAsset(PnlMat *shift_path, const PnlMat *path, int d, double h, double t);

    void stepSimulation(PnlVect *current_spot, double dt, PnlRng *rng);

};
