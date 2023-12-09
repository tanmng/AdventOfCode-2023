#include "gflags/gflags.h"
#include "re2/re2.h"
#include <functional>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <numeric>
#include <vector>

DEFINE_bool(part2, false, "Run the binary to solve part 2");

using namespace std;
namespace {
const size_t nameLen = 3;
const string kStart = "AAA", kEnd = "ZZZ";
const re2::RE2
    kNodeLineRe("^([A-Z0-9]{3}) = \\(([A-Z0-9]{3}), ([A-Z0-9]{3})\\)$");

struct Node {
  string left;
  string right;
};

/**
 * Execution the rule from
 */
size_t countSteps(const string &start,
                  const function<bool(const string &)> &stopCondition,
                  const string &instruction,
                  const unordered_map<string, Node> &instructions) {
  size_t stepCount = 0;
  string temp = start;
  while (!stopCondition(temp)) {
    // Not yet stop
    if (instruction[stepCount++ % instruction.size()] == 'L') {
      // Go left
      temp = instructions.at(temp).left;
    } else {
      // Go right
      temp = instructions.at(temp).right;
    }
  }
  return stepCount;
}

} // namespace

int main(int argc, char **argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  size_t answer = 0;
  string instruction, temp;
  unordered_set<string> allStarts;
  unordered_map<string, Node> instructions;
  // Part 2
  getline(cin, instruction);
  // Skip the empty line
  getline(cin, temp);
  while (getline(cin, temp)) {
    string source, left, right;
    if (re2::RE2::FullMatch(temp, kNodeLineRe, &source, &left, &right)) {
      // Add this rule
      instructions.insert({source, {left, right}});
      if (source[nameLen - 1] == 'A') {
        allStarts.insert(source);
      }
    } else {
      throw runtime_error("Invalid instruction line " + temp);
    }
  }

  answer = 0;
  if (!FLAGS_part2) {
    // Part 1
    answer = countSteps(
        kStart, [](const string val) { return val == kEnd; }, instruction,
        instructions);
  } else {
    // The  data is specially engineered so that from any xxA, if you reach an
    // xxZ, it will resume the cycle and go back to that very xxZ - magical,
    // right? Took me a lot of time to think of a generic solution, urg

    // Find out all the cycles
    answer = instruction.size();
    for (const auto &curStart : allStarts) {
      auto temp = countSteps(
          curStart, [](const string val) { return val[nameLen - 1] == 'Z'; },
          instruction, instructions);
      answer = lcm(answer, temp);
      cout << curStart << ": LCM so far " << answer << endl;
    }
  }

  cout << (FLAGS_part2 ? "Part 2: " : "Part 1: ") << answer << endl;

  if (!FLAGS_part2) {
    cout << "Solution above is for part 1, if you need part 2 solution, add "
            "the flag --part2 to the command invocation"
         << endl;
  }
}
