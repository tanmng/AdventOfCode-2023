#include <queue>
#include <string>
#include <utility>

#include "re2/re2.h"

namespace tanmng::aoc2023::lib {
typedef std::pair<size_t, size_t> ValueRange;

struct PartRange {
  // Inclusive range of values for each category of part
  ValueRange x, m, a, s;

  bool accepted = false;
  std::string workflowName;

  /**
   * Confirm if the range is valid
   */
  bool validRange() const;

  /**
   * Calculate how many combination the ranges can create
   */
  size_t combinationsCount() const;

  /**
   * Allow accesing fields of the range in a dict-like maner
   * But allow modifying them as well
   */
  ValueRange &operator[](const char field);
};

struct Part {
  /**
   * Parse the string representation of a part
   */
  Part(const std::string &partStr);
  /**
   * Return the sum of all values on the part
   */
  size_t sumOfValues() const;
  /**
   * Allow accessing fields of the part in a dict-like maner
   */
  const size_t operator[](const char field) const;

private:
  size_t x_, m_, a_, s_;
};

/**
 * Rule
 */
enum RuleResult {
  kAccept,
  kReject,
  kPass, // The rule didn't have any effect, just move on to the next rule
  kMove,
};

struct Rule {
  Rule(const std::string &ruleStr);

  /**
   * Execute the rule for given part
   */
  RuleResult operator()(const Part &part, std::string &nextWorkFlowName) const;

  /**
   * Partition a range by the rule
   *
   * Push a new range into the processing queue as a way to return
   * Do not push if not needed
   *
   * Return true if we need to continue partitioning the range after this rule
   * is done, in such case, modify the range to give the next rule the "left
   * over"
   */
  bool partitionRange(PartRange &range,
                      std::queue<PartRange> &processingQueue) const;

private:
  // The function that internally we use to perform the operator
  std::function<RuleResult(const Part &part, std::string &nextWorkFlowName)>
      actualOperator_;
  // The function that internally we use to do the partition
  std::function<bool(PartRange &range, std::queue<PartRange> &processingQueue)>
      actualPartition_;
};

/**
 * Workflow
 */
struct WorkFlow {
  RuleResult operator()(const Part &part, std::string &nextWorkFlowName) const;
  WorkFlow(const std::string &workflowStr);
  std::string name;

  /**
   * Pass through to underlying rules
   *
   */
  void partitionRange(PartRange &range,
                      std::queue<PartRange> &processingQueue) const;

private:
  std::vector<Rule> rules_;
};

} // namespace tanmng::aoc2023::lib
