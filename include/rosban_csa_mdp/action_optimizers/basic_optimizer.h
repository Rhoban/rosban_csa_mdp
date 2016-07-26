#pragma once

#include "rosban_csa_mdp/action_optimizers/action_optimizer.h"

#include "rosban_fa/trainer.h"

namespace csa_mdp
{

class BasicOptimizer : public ActionOptimizer
{
public:
  BasicOptimizer();

  virtual Eigen::VectorXd optimize(const Eigen::VectorXd & input,
                                   std::shared_ptr<const Policy> current_policy,
                                   std::shared_ptr<Problem> model,
                                   Problem::RewardFunction reward_function,
                                   Problem::ValueFunction value_function,
                                   Problem::TerminalFunction terminal_function,
                                   double discount,
                                   std::default_random_engine * engine) const override;

  virtual std::string class_name() const override;
  virtual void to_xml(std::ostream &out) const override;
  virtual void from_xml(TiXmlNode *node) override;

private:
  int nb_additional_steps;
  int nb_simulations;
  int nb_actions;
  std::unique_ptr<rosban_fa::Trainer> trainer;
};

}
