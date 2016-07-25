#pragma once

#include "rosban_csa_mdp/reward_predictors/reward_predictor.h"

#include <random>

namespace csa_mdp
{

class MonteCarloPredictor : public RewardPredictor
{
public:
  MonteCarloPredictor();

  void predict(const Eigen::VectorXd & input,
               std::shared_ptr<const Policy> policy,
               int nb_steps,
               std::shared_ptr<Problem> model,//TODO: Model class ?
               RewardFunction reward_function,
               ValueFunction value_function,
               double discount,
               double * mean,
               double * var) override;

  virtual std::string class_name() const override;
  virtual void to_xml(std::ostream &out) const override;
  virtual void from_xml(TiXmlNode *node) override;

private:
  int nb_predictions;
  //TODO add number of threads
  std::default_random_engine engine;
};

}
