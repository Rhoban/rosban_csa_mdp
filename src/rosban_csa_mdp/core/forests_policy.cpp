#include "rosban_csa_mdp/core/forests_policy.h"

#include "rosban_random/tools.h"

using regression_forests::Forest;

namespace csa_mdp
{

ForestsPolicy::ForestsPolicy()
  : Policy(), apply_noise(false)
{
  engine = rosban_random::getRandomEngine();
}

Eigen::VectorXd ForestsPolicy::getRawAction(const Eigen::VectorXd &state)
{
  return getRawAction(state, &engine);
}

Eigen::VectorXd ForestsPolicy::getRawAction(const Eigen::VectorXd &state,
                                            std::default_random_engine * external_engine) const
{
  bool delete_engine = false;
  if (apply_noise && external_engine == nullptr)
  {
    delete_engine = true;
    external_engine = rosban_random::newRandomEngine();
  }

  Eigen::VectorXd cmd(policies.size());
  if (apply_noise) {
    for (int dim = 0; dim < cmd.rows(); dim++) {
      cmd(dim) = policies[dim]->getRandomizedValue(state, *external_engine);
    }
  }
  else {
    for (int dim = 0; dim < cmd.rows(); dim++) {
      cmd(dim) = policies[dim]->getValue(state);
    }
  }
  if (delete_engine) { delete(external_engine); }
  return cmd;
}

void ForestsPolicy::to_xml(std::ostream & out) const
{
  (void) out;
  throw std::runtime_error("Not implemented yet: ForestsPolicy::to_xml");
}

void ForestsPolicy::from_xml(TiXmlNode * node)
{
  std::vector<std::string> paths;
  paths = rosban_utils::xml_tools::read_vector<std::string>(node, "paths");
  policies.clear();
  for (const std::string &path : paths)
  {
    std::unique_ptr<Forest> forest(new Forest());
    forest->load(path);
    policies.push_back(std::move(forest));
  }
  rosban_utils::xml_tools::try_read<bool>(node, "apply_noise", apply_noise);
}

std::string ForestsPolicy::class_name() const
{
  return "forests_policy";
}

}
