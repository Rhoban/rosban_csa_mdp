#include "rosban_csa_mdp/solvers/mre_fpf.h"

using regression_forests::TrainingSet;

namespace csa_mdp
{

MREFPF::Config::Config()
  : filter_samples(false), reward_max(0), update_type(UpdateType::MRE)
{
}

std::string MREFPF::Config::class_name() const
{
  return "MREFPFConfig";
}

void MREFPF::Config::to_xml(std::ostream &out) const
{
  FPF::Config::to_xml(out);
  rosban_utils::xml_tools::write<bool>  ("filter_samples", filter_samples, out);
  rosban_utils::xml_tools::write<double>("reward_max"    , reward_max    , out);
  rosban_utils::xml_tools::write<std::string>("update_type", to_string(update_type), out);
}

void MREFPF::Config::from_xml(TiXmlNode *node)
{
  FPF::Config::from_xml(node);
  filter_samples  = rosban_utils::xml_tools::read<bool>(node, "filter_samples");
  reward_max      = rosban_utils::xml_tools::read<int> (node, "reward_max"    );
  std::string update_type_str;
  update_type_str = rosban_utils::xml_tools::read<std::string>(node, "update_type");
  update_type = loadUpdateType(update_type_str);
}

MREFPF::MREFPF()
{
}

MREFPF::MREFPF(const MREFPF::Config & conf_,
               std::shared_ptr<KnownnessFunction> knownness_func_)
  : conf(conf_), knownness_func(knownness_func_)
{
}

TrainingSet MREFPF::getTrainingSet(const std::vector<Sample> &samples,
                                   std::function<bool(const Eigen::VectorXd&)> is_terminal)
{
  // Removing samples which have the same starting state if filter_samples is activated
  std::vector<Sample> filtered_samples;
  if (conf.filter_samples)
    filtered_samples = filterSimilarSamples(samples);
  else
    filtered_samples = samples;
  // Computing original training Set
  TrainingSet original_ts = FPF::getTrainingSet(filtered_samples, is_terminal);
  // If alternative mode, then do not modify samples
  if (conf.update_type == UpdateType::Alternative) return original_ts;
  // Otherwise use knownness to influence samples
  TrainingSet new_ts(original_ts.getInputDim());
  for (size_t i = 0; i < original_ts.size(); i++)
  {
    // Extracting information from original sample
    const regression_forests::Sample & original_sample = original_ts(i);
    Eigen::VectorXd input = original_sample.getInput();
    double reward         = original_sample.getOutput();
    // Getting knownness of the input
    double knownness = knownness_func->getValue(input);
    double new_reward = reward * knownness + conf.reward_max * (1 - knownness);
    new_ts.push(regression_forests::Sample(input, new_reward));
  }
  return new_ts;
}

void MREFPF::updateQValue(const std::vector<Sample> &samples,
                          std::function<bool(const Eigen::VectorXd&)> is_terminal)
{
  FPF::updateQValue(samples, is_terminal);
  if (conf.update_type == UpdateType::Alternative)
  {
    throw std::logic_error("Unimplemented update Alternative");
  }
}

std::vector<Sample> MREFPF::filterSimilarSamples(const std::vector<Sample> &samples) const
{
  std::vector<Sample> filtered_samples;
  for (const Sample &new_sample : samples)
  {
    bool found_similar = false;
    double tolerance = std::pow(10,-6);
    for (const Sample &known_sample : filtered_samples)
    {
      Eigen::VectorXd state_diff = known_sample.state - new_sample.state;
      Eigen::VectorXd action_diff = known_sample.action - new_sample.action;
      // Which is the highest difference between the two samples?
      double max_diff = std::max(state_diff.lpNorm<Eigen::Infinity>(),
                                 action_diff.lpNorm<Eigen::Infinity>());
      if (max_diff < tolerance)
      {
        found_similar = true;
        break;
      }
    }
    // Do not add similar samples
    if (found_similar) continue;
    filtered_samples.push_back(new_sample);
  }
  return filtered_samples;
}

std::string to_string(MREFPF::UpdateType type)
{
  switch (type)
  {
    case MREFPF::UpdateType::MRE: return "MRE";
    case MREFPF::UpdateType::Alternative: return "Alternative";
  }
  throw std::runtime_error("Unknown type in to_string(Type)");
}

MREFPF::UpdateType loadUpdateType(const std::string &type)
{
  if (type == "MRE")
  {
    return MREFPF::UpdateType::MRE;
  }
  if (type == "Alternative")
  {
    return MREFPF::UpdateType::Alternative;
  }
  throw std::runtime_error("Unknown MREFPF Update Type: '" + type + "'");
}

}