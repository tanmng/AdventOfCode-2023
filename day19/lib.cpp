#include "day19/lib.h"

#include <exception>
#include <iostream>
#include <string>
#include <vector>

#include "absl/strings/str_split.h"
#include "re2/re2.h"

using std::max;
using std::min;
using std::out_of_range;
using std::runtime_error;
using std::string;
using std::vector;

namespace tanmng::aoc2023::lib {
const re2::RE2 kSkipRuleRe("^([a-z]{2,3})$");
const re2::RE2 kCompareRuleRe("^([xmas])([<>])([0-9]+):([a-z]{2,3}|A|R)$");
const re2::RE2 kPartRe("^\\{x=([0-9]+),m=([0-9]+),a=([0-9]+),s=([0-9]+)\\}$");
const re2::RE2 kWorkFlowRe("^([a-z]+)\\{(.+)\\}$");

ValueRange &PartRange::operator[](const char field) {
  switch (field) {
  case 'x':
    return x;
  case 'm':
    return m;
  case 'a':
    return a;
  case 's':
    return s;
  }
  throw out_of_range(
      "Trying to access an incorrect field of a PartRange object");
}

bool PartRange::validRange() const {
  return x.second >= x.first && m.second >= m.first && a.second >= a.first &&
         s.second >= s.first;
}

size_t PartRange::combinationsCount() const {
  size_t result = 1;
  result *= x.second - x.first + 1;
  result *= m.second - m.first + 1;
  result *= a.second - a.first + 1;
  result *= s.second - s.first + 1;
  return result;
}

Part::Part(const string &partStr) {
  if (!re2::RE2::FullMatch(partStr, kPartRe, &x_, &m_, &a_, &s_)) {
    throw runtime_error("Invalid string for part: " + partStr);
  }
}
size_t Part::sumOfValues() const { return x_ + m_ + a_ + s_; }
const size_t Part::operator[](const char field) const {
  switch (field) {
  case 'x':
    return x_;
  case 'm':
    return m_;
  case 'a':
    return a_;
  case 's':
    return s_;
  }
  throw out_of_range("Trying to access an incorrect field of a Part object");
}

RuleResult Rule::operator()(const Part &part, string &nextWorkFlowName) const {
  return actualOperator_(part, nextWorkFlowName);
}

bool Rule::partitionRange(PartRange &range,
                          std::queue<PartRange> &processingQueue) const {
  return actualPartition_(range, processingQueue);
}

Rule::Rule(const string &ruleStr) {
  if (ruleStr == "A") {
    actualOperator_ = [](const Part & /* unused */, string & /* unused */) {
      return RuleResult::kAccept;
    };
    actualPartition_ = [](PartRange &range,
                          std::queue<PartRange> &processingQueue) {
      // The rule simply accept, so the whole range is now accepted, do not
      // process further
      auto newRange = range;
      newRange.accepted = true;
      processingQueue.push(std::move(newRange));
      return false;
    };
    return;
  }
  if (ruleStr == "R") {
    actualOperator_ = [](const Part & /* unused */, string & /* unused */) {
      return RuleResult::kReject;
    };
    actualPartition_ = [](PartRange & /* unused */,
                          std::queue<PartRange> & /* unused */) {
      // This rule kill everything, so do not push anything back into the queue
      // Also, signial the workflow that no further processing is needed
      return false;
    };
    return;
  }
  string nextWorkFlow;
  if (re2::RE2::FullMatch(ruleStr, kSkipRuleRe, &nextWorkFlow)) {
    actualOperator_ = [=](const Part & /* unused */, string &nextWorkFlowName) {
      nextWorkFlowName = nextWorkFlow;
      return RuleResult::kMove;
    };
    actualPartition_ = [=](PartRange &range,
                           std::queue<PartRange> &processingQueue) {
      // The rule simply jump, no further process (within the same workflow)
      auto newRange = range;
      newRange.workflowName = nextWorkFlow;
      processingQueue.push(std::move(newRange));
      return false;
    };
    return;
  }
  char lhsChar, compChar;
  size_t rhs;
  if (re2::RE2::FullMatch(ruleStr, kCompareRuleRe, &lhsChar, &compChar, &rhs,
                          &nextWorkFlow)) {
    const auto lt = compChar == '<';
    if (nextWorkFlow == "A") {
      // Compare to accept
      if (lt) {
        actualOperator_ = [=](const Part &part, string & /* unused */) {
          if (part[lhsChar] < rhs) {
            // Accept
            return RuleResult::kAccept;
          } else {
            // Pass
            return RuleResult::kPass;
          }
        };
        actualPartition_ = [=](PartRange &range,
                               std::queue<PartRange> &processingQueue) {
          // The rule force upper limit to the range
          if (range[lhsChar].first > rhs) {
            // Nothing to process with this - let the next rule take care of it
            return true;
          } else {
            // The part that we pass on to the pointed workflow (in this case we
            // accept)
            auto newRange = range;
            newRange.accepted = true;
            newRange[lhsChar].second = min(newRange[lhsChar].second, rhs - 1);
            processingQueue.push(std::move(newRange));

            // Check if we need to continue with the next rule in the current
            // workflow
            if (rhs <= range[lhsChar].second) {
              // rhs is between min and max so we can continue with the next
              // rule
              range[lhsChar].first = rhs;
              return true;
            } else {
              // We already encapsulated the whole range when we accept, so
              // nothing for the next rule
              return false;
            }
          }
        };
      } else {
        actualOperator_ = [=](const Part &part, string & /* unused */) {
          if (part[lhsChar] > rhs) {
            // Accept
            return RuleResult::kAccept;
          } else {
            // Pass
            return RuleResult::kPass;
          }
        };
        actualPartition_ = [=](PartRange &range,
                               std::queue<PartRange> &processingQueue) {
          // The rule force lower limit to the range
          if (range[lhsChar].second < rhs) {
            // Nothing to process with this - let the next rule take care of it
            return true;
          } else {
            // The part that we pass on to the pointed workflow (in this case we
            // accept)
            auto newRange = range;
            newRange.accepted = true;
            newRange[lhsChar].first = max(newRange[lhsChar].first, rhs + 1);
            processingQueue.push(std::move(newRange));

            // Check if we need to continue with the next rule in the current
            // workflow
            if (rhs >= range[lhsChar].first) {
              // rhs is between min and max so we can continue with the next
              // rule
              range[lhsChar].second = rhs;
              return true;
            } else {
              // We already encapsulated the whole range when we accept, so
              // nothing for the next rule
              return false;
            }
          }
        };
      }
    } else if (nextWorkFlow == "R") {
      // Compare to reject
      if (lt) {
        actualOperator_ = [=](const Part &part, string & /* unused */) {
          if (part[lhsChar] < rhs) {
            // Reject
            return RuleResult::kReject;
          } else {
            // Pass
            return RuleResult::kPass;
          }
        };
        actualPartition_ = [=](PartRange &range,
                               std::queue<PartRange> &processingQueue) {
          // The rule force upper limit to the range to be rejected
          if (range[lhsChar].first > rhs) {
            // Nothing to process with this - let the next rule take care of it
            return true;
          } else {
            // The part that we pass on to the pointed workflow (in this case we
            // reject it) So do nothing

            // Check if we need to continue with the next rule in the current
            // workflow
            if (rhs <= range[lhsChar].second) {
              // rhs is between min and max so we can continue with the next
              // rule
              range[lhsChar].first = rhs;
              return true;
            } else {
              // We already encapsulated the whole range when we accept, so
              // nothing for the next rule
              return false;
            }
          }
        };
      } else {
        actualOperator_ = [=](const Part &part, string & /* unused */) {
          if (part[lhsChar] > rhs) {
            // Reject
            return RuleResult::kReject;
          } else {
            // Pass
            return RuleResult::kPass;
          }
        };
        actualPartition_ = [=](PartRange &range,
                               std::queue<PartRange> &processingQueue) {
          // The rule force lower limit to the range
          if (range[lhsChar].second < rhs) {
            // Nothing to process with this - let the next rule take care of it
            return true;
          } else {
            // The part that we pass on to the pointed workflow (in this case we
            // reject) So do nothing

            // Check if we need to continue with the next rule in the current
            // workflow
            if (rhs >= range[lhsChar].first) {
              // rhs is between min and max so we can continue with the next
              // rule
              range[lhsChar].second = rhs;
              return true;
            } else {
              // We already encapsulated the whole range when we accept, so
              // nothing for the next rule
              return false;
            }
          }
        };
      }
    } else {
      // Compare to move
      if (lt) {
        actualOperator_ = [=](const Part &part, string &nextWorkFlowName) {
          if (part[lhsChar] < rhs) {
            nextWorkFlowName = nextWorkFlow;
            return RuleResult::kMove;
          } else {
            // Pass
            return RuleResult::kPass;
          }
        };
        actualPartition_ = [=](PartRange &range,
                               std::queue<PartRange> &processingQueue) {
          // The rule force upper limit to the range
          if (range[lhsChar].first > rhs) {
            // Nothing to process with this - let the next rule take care of it
            return true;
          } else {
            // The part that we pass on to the pointed workflow
            auto newRange = range;
            newRange.workflowName = nextWorkFlow;
            newRange[lhsChar].second = min(newRange[lhsChar].second, rhs - 1);
            processingQueue.push(std::move(newRange));

            // Check if we need to continue with the next rule in the current
            // workflow
            if (rhs <= range[lhsChar].second) {
              // rhs is between min and max so we can continue with the next
              // rule
              range[lhsChar].first = rhs;
              return true;
            } else {
              // We already encapsulated the whole range when we accept, so
              // nothing for the next rule
              return false;
            }
          }
        };
      } else {
        actualOperator_ = [=](const Part &part, string &nextWorkFlowName) {
          if (part[lhsChar] > rhs) {
            nextWorkFlowName = nextWorkFlow;
            return RuleResult::kMove;
          } else {
            // Pass
            return RuleResult::kPass;
          }
        };
        actualPartition_ = [=](PartRange &range,
                               std::queue<PartRange> &processingQueue) {
          // The rule force lower limit to the range
          if (range[lhsChar].second < rhs) {
            // Nothing to process with this - let the next rule take care of it
            return true;
          } else {
            // The part that we pass on to the pointed workflow
            auto newRange = range;
            newRange.workflowName = nextWorkFlow;
            newRange[lhsChar].first = max(newRange[lhsChar].first, rhs + 1);
            processingQueue.push(std::move(newRange));

            // Check if we need to continue with the next rule in the current
            // workflow
            if (rhs >= range[lhsChar].first) {
              // rhs is between min and max so we can continue with the next
              // rule
              range[lhsChar].second = rhs;
              return true;
            } else {
              // We already encapsulated the whole range when we accept, so
              // nothing for the next rule
              return false;
            }
          }
        };
      }
    }
    return;
  }
  throw runtime_error("Failed to parse the following string to be a rule: " +
                      ruleStr);
}

WorkFlow::WorkFlow(const string &workflowStr) {
  string rulePart;

  if (!re2::RE2::FullMatch(workflowStr, kWorkFlowRe, &name, &rulePart)) {
    throw runtime_error("Invalid workflow string: " + workflowStr);
  }

  // Parse all the rules
  vector<string> ruleStrs = absl::StrSplit(rulePart, ',');
  for (const auto &ruleStr : ruleStrs) {
    rules_.emplace_back(Rule(ruleStr));
  }
}

RuleResult WorkFlow::operator()(const Part &part,
                                string &nextWorkFlowName) const {
  for (const auto &rule : rules_) {
    auto result = rule(part, nextWorkFlowName);
    if (result == RuleResult::kPass) {
      continue;
    } else {
      return result;
    }
  }

  throw runtime_error("Should not reach this, workflow with following name "
                      "didnt execute anything for part: " +
                      name);
}

void WorkFlow::partitionRange(PartRange &range,
                              std::queue<PartRange> &processingQueue) const {
  for (const auto &rule : rules_) {
    auto result = rule.partitionRange(range, processingQueue);
    if (result) {
      // Current rule told us to continue
      continue;
    } else {
      // We're done
      return;
    }
  }
  return;
}
} // namespace tanmng::aoc2023::lib