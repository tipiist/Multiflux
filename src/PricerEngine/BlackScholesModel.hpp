#pragma once

#include "pnl/pnl_random.h"
#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"

/// \brief Mod�le de Black Scholes
class BlackScholesModel {
public:
    PnlMat *volatility_; /// matrice des volatilit�s
    PnlVect *paymentDates_; /// Dates de payements; Maturit�s
    PnlVect *strikes_; /// Les prix d'exercices � chaque date
    int nAssets_;       /// nombre d'actifs du mod�le
    double interestRate_;       /// taux d'int�r�t
    PnlVect *G_;
    PnlVect *Ld_;
    PnlVect *computationHelper_;

    /**
     *Constructeur
     */
    BlackScholesModel(int nAssets, double interestRate, PnlVect *paymentDates, PnlVect *strikes, PnlMat *volatility);

    ~BlackScholesModel();

    /**
     * G�n�ration des trajectoires � partir d'une matrice past (prix des sousjacents aux dates de constatations pr�c�detes � t)
     */
    void asset(PnlMat *path, double t, bool isMonitoringDate, PnlRng *rng, const PnlMat *past, int nbTimeSteps);

    /**
     * Shift d'une trajectoire du sous-jacent
     */
    void shiftAsset(PnlMat *shift_path, const PnlMat *path, int d, double h, double t);

    void stepSimulation(PnlVect *current_spot, double dt, PnlRng *rng);

};
