#pragma once

#include "OptionMultiFlux.hpp"
#include "json_reader.hpp"
#include "nlohmann/json.hpp"
#include <iostream>


class OptionVanille : public OptionMultiFlux {
public:
    
    OptionVanille(nlohmann::json &jsonParams);
    ~OptionVanille();
    double payoff(const PnlMat *path, double interestRate) override;
};
