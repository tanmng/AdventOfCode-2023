#include "day19/lib.h"

#include <iostream>
#include <utility>

#include "absl/strings/str_split.h"
#include "gflags/gflags.h"
#include "re2/re2.h"

DEFINE_bool(part2, false, "Run the binary to solve part 2");

using namespace std;
using namespace tanmng::aoc2023::lib;
namespace {
const string kStartingPoint = "in";
const size_t kValueRangeMax = 4000;

/**
 * Execute all the rules, and return if we accept the part or not
 */
bool executeWorkflows(const map<string, WorkFlow> &flows, const Part &part) {
  string curFlow = kStartingPoint;
  while (true) {
    const auto &flow = flows.at(curFlow);
    const auto result = flow(part, curFlow);
    switch (result) {
    case RuleResult::kAccept:
      return true;
    case RuleResult::kReject:
      return false;
    case RuleResult::kPass:
    case RuleResult::kMove:
      // Nothing to do here
      continue;
      break;
    }
  }
}

/**
 * Part 2 - find all the combinations that are accepted
 */
size_t findAllAcceptedCombinations(const map<string, WorkFlow> &flows) {
  queue<PartRange> processingQueue;
  {
    PartRange foo;
    foo['x'] = {1, kValueRangeMax};
    foo['m'] = {1, kValueRangeMax};
    foo['a'] = {1, kValueRangeMax};
    foo['s'] = {1, kValueRangeMax};
    foo.workflowName = kStartingPoint;
    processingQueue.push(std::move(foo));
  }
  size_t result = 0;
  while (!processingQueue.empty()) {
    auto &range = processingQueue.front();
    processingQueue.pop();
    if (!range.validRange()) {
      // Something is wrong here - should just validate
      throw runtime_error("Invalid range");
    }
    if (range.accepted) {
      // Already accepted - no need to partition
      result += range.combinationsCount();
      continue;
    }
    const auto &workflow = flows.at(range.workflowName);
    workflow.partitionRange(range, processingQueue);
  }

  return result;
}
} // namespace

int main(int argc, char **argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  size_t answerPart1 = 0, answerPart2 = 0;
  string line;
  map<string, WorkFlow> flows;
  {
    bool parsingPart = false;
    while (getline(cin, line)) {
      if (line.size() == 0) {
        // Empty line, from now on we process the parts
        parsingPart = true;
      } else {
        if (parsingPart) {
          // Parsing parts
          Part foo(line);
          if (executeWorkflows(flows, foo)) {
            // This part is accepted
            answerPart1 += foo.sumOfValues();
          }
        } else {
          // Parsing workflows
          WorkFlow flow(line);
          flows.insert({flow.name, flow});
        }
      }
    }
  }

  // Part 1
  cout << "Part 1: " << answerPart1 << endl;
  // Part 2
  answerPart2 = findAllAcceptedCombinations(flows);
  cout << "Part 2: " << answerPart2 << endl;
}
