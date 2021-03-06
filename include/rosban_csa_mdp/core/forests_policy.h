#pragma once

#include "rosban_csa_mdp/core/policy.h"

#include "rosban_regression_forests/core/forest.h"

#include <random>

namespace csa_mdp
{

/// This class implements a policy as a set of regression forests, one for each dimension
class ForestsPolicy : public Policy
{
public:
  ForestsPolicy();

  Eigen::VectorXd getRawAction(const Eigen::VectorXd &state) override;
  Eigen::VectorXd getRawAction(const Eigen::VectorXd &state,
                               std::default_random_engine * engine) const override;

  void to_xml(std::ostream & out) const override;
  void from_xml(TiXmlNode * node) override;
  std::string class_name() const override;

private:
  /// The policies
  std::vector<std::unique_ptr<regression_forests::Forest>> policies;

  /// Is the noise applied when requesting raw action?
  bool apply_noise;

  /// Random generator used to select noisy actions
  std::default_random_engine engine;
};

}
