#include "rosban_csa_mdp/core/policy_factory.h"

#include "rosban_csa_mdp/core/fa_policy.h"
#include "rosban_csa_mdp/core/forests_policy.h"
#include "rosban_csa_mdp/core/monte_carlo_policy.h"
#include "rosban_csa_mdp/core/opportunist_policy.h"
#include "rosban_csa_mdp/core/random_policy.h"

namespace csa_mdp
{

std::map<std::string,PolicyFactory::Builder> PolicyFactory::extra_builders;

PolicyFactory::PolicyFactory()
{
  registerBuilder("fa_policy",[](){return std::unique_ptr<Policy>(new FAPolicy);});
  registerBuilder("forests_policy",[](){return std::unique_ptr<Policy>(new ForestsPolicy);});
  registerBuilder("monte_carlo_policy",
                  [](){return std::unique_ptr<Policy>(new MonteCarloPolicy );});
  registerBuilder("opportunist_policy",
                  [](){return std::unique_ptr<Policy>(new OpportunistPolicy);});
  registerBuilder("random"        ,[](){return std::unique_ptr<Policy>(new RandomPolicy );});
  for (const auto & entry : extra_builders)
  {
    registerBuilder(entry.first, entry.second);
  }
}

void PolicyFactory::registerExtraBuilder(const std::string &name, Builder b)
{
  extra_builders[name] = b;
}

}
