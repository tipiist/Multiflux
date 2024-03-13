#pragma once
#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"

/// \brief Classe Option abstraite
class Option {
public:
    PnlVect *paymentDates_;            /// Dates de payements; Maturit�s
    PnlVect *strikes_;                 /// Les prix d'exercices � chaque date
    int nAssets_;                       /// dimension du mod�le, redondant avec la taille des vecteurs paymentDates et strikes
    /**
     * Calcule la valeur du payoff de l'option MultiFlux
     *
     * @param[in] path est une matrice de taille (N+1) x t_idx
     * contenant une trajectoire du mod�le telle que cr��e
     * par la fonction asset.
     * @return phi(trajectoire)
     */
    virtual double payoff(const PnlMat *path, double interestRate) = 0;
};

