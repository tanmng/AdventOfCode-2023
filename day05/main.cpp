#include "gflags/gflags.h"
#include "re2/re2.h"
#include <algorithm>
#include <iostream>
#include <memory>
#include <queue>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

DEFINE_bool(part2, false, "Run the binary to solve part 2");

using namespace std;
namespace {
const re2::RE2 kRuleLineRe("^([0-9]+) ([0-9]+) ([0-9]+)$");

// A number range - inclusive
struct NumRange {
  size_t begin;
  size_t end;
};

// A rule, the source range is inclusive
struct Rule {
  NumRange source;
  NumRange dest;
};

// Execute rules
void executeRules(shared_ptr<queue<NumRange>> &currentRanges,
                  shared_ptr<queue<NumRange>> &nextRanges,
                  vector<Rule> &rules) {
  // Change the number ranges
  while (!currentRanges->empty()) {
    auto range = currentRanges->front();
    currentRanges->pop();

    auto changedByRule = false;
    for (const auto &rule : rules) {
      if (rule.source.begin <= range.begin && range.end <= rule.source.end) {
        // The range is fully encapsulated inside the rule
        auto newRangeBegin = rule.dest.begin + range.begin - rule.source.begin;
        nextRanges->push(
            {newRangeBegin, newRangeBegin + (range.end - range.begin)});
        changedByRule = true;
        break;
      } else if (range.begin <= rule.source.begin &&
                 rule.source.end <= range.end) {
        // The rule source is fully encapsulated inside the range
        nextRanges->push(rule.dest);

        // We need to process the cut off - push them back to the queue
        if (range.begin < rule.source.begin) {
          currentRanges->push({range.begin, rule.source.begin - 1});
        }
        if (rule.source.end < range.end) {
          currentRanges->push({rule.source.end + 1, range.end});
        }
        changedByRule = true;
        break;
      } else if (range.begin <= rule.source.begin &&
                 rule.source.begin <= range.end &&
                 range.end <= rule.source.end) {
        // Overlap with the range being on the left of the rule source
        auto newRangeEnd = rule.dest.begin + range.end - rule.source.begin;
        nextRanges->push({rule.dest.begin, newRangeEnd});

        // We need to process the cut off
        if (range.begin < rule.source.begin) {
          currentRanges->push({range.begin, rule.source.begin - 1});
        }

        changedByRule = true;
        break;
      } else if (rule.source.begin <= range.begin &&
                 range.begin <= rule.source.end &&
                 rule.source.end <= range.end) {
        // Overlap with the range being on the right of the rule source
        auto newRangeBegin = rule.dest.begin + range.begin - rule.source.begin;
        nextRanges->push({newRangeBegin, rule.dest.end});

        // We need to process the cut off
        if (rule.source.end < range.end) {
          currentRanges->push({rule.source.end + 1, range.end});
        }

        changedByRule = true;
        break;
      }
    }

    if (!changedByRule) {
      // The range was not changed by any rule, just copy it over to nextRanges
      nextRanges->push(range);
    }
  }
  // Swap current and next - faster to use pointers as such than manually copying
  auto tempCurrent = currentRanges;
  currentRanges = nextRanges;
  nextRanges = tempCurrent;

  // Clear all the rules - ready for next set of rules
  rules.clear();
}

} // namespace

int main(int argc, char **argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  string line;

  // All the current number ranges and the one for next round
  auto currentRanges = make_shared<queue<NumRange>>();
  auto nextRanges = make_shared<queue<NumRange>>();
  vector<Rule> rules;

  size_t lineCounter = 0;
  while (getline(cin, line)) {
    if (line.length() == 0) {
      // End of a section

      // Execute the rules
      executeRules(currentRanges, nextRanges, rules);
      continue;
    } else {
      if (lineCounter == 0) {
        // Parse the first set of numbers - seeds
        istringstream ss(line);
        string temp; // the world seeds
        ss >> temp;
        size_t num, count;
        if (FLAGS_part2) {
          // Part 2 - we have range
          // Parse the numbers
          while (ss >> num >> count) {
            currentRanges->push({num, num + count});
          }
        } else {
          // Part 1 - simple numbers
          // Parse the numbers
          while (ss >> num) {
            currentRanges->push({num, num});
          }
        }
        // Skip the next line since it will trigger an end-of-section
        getline(cin, line);
      } else {
        size_t destBegin, sourceBegin, range;
        if (re2::RE2::FullMatch(line, kRuleLineRe, &destBegin, &sourceBegin,
                                &range)) {
          // Record the rules
          rules.push_back({{sourceBegin, sourceBegin + range - 1},
                           {destBegin, destBegin + range - 1}});
        } else {
          // Header line
          cout << line << endl;
        }
      }
      ++lineCounter;
    }
  }

  // Last rule execution
  executeRules(currentRanges, nextRanges, rules);

  auto lowestVal = numeric_limits<size_t>::max();
  while (!currentRanges->empty()) {
    auto range = currentRanges->front();
    currentRanges->pop();

    lowestVal = min(lowestVal, range.begin);
  }

  if (FLAGS_part2) {
    // Tally the result for part 2
  }
  cout << (FLAGS_part2 ? "Part 2: " : "Part 1: ") << lowestVal << endl;

  if (!FLAGS_part2) {
    cout << "Solution above is for part 1, if you need part 2 solution, add "
            "the flag --part2 to the command invocation"
         << endl;
  }
}
