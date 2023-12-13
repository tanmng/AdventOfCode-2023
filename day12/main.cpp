#include "gflags/gflags.h"
#include "re2/re2.h"
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

DEFINE_bool(part2, false, "Run the binary to solve part 2");

using namespace std;
namespace {
const size_t kFoldFactor = 5;
const re2::RE2 kLineRe("^([.#?]+) ([0-9,]+)$");
const char kDamaged = '#', kOperational = '.', kUnknown = '?';

/**
 * Confirm if the given record (as a string) matches with the record as number
 */
bool validRecord(const string &recordAsString,
                 const vector<size_t> &recordAsNums) {
  size_t numsIndex = 0;
  size_t currentGroup = 0;
  for (const auto &ch : recordAsString) {
    switch (ch) {
    case kDamaged:
      ++currentGroup;
      break;
    case kOperational:
      if (currentGroup > 0) {
        // We just finished a group
        if (numsIndex >= recordAsNums.size() ||
            currentGroup != recordAsNums.at(numsIndex)) {
          // This does not match
          return false;
        } else {
          // All OK - good match
          ++numsIndex;
          if (numsIndex > recordAsNums.size()) {
            // Too much
            return false;
          }
        }
        // Reset
        currentGroup = 0;
      }
      break;
    default:
      throw runtime_error("Invalid char in the string we use for test");
      break;
    }
  }

  // We might still be inside the last group
  if (currentGroup != 0) {

    if (numsIndex >= recordAsNums.size() ||
        currentGroup != recordAsNums.at(numsIndex)) {
      // This does not match
      return false;
    } else {
      // All OK - good match
      ++numsIndex;
    }
  }

  return numsIndex == recordAsNums.size();
}

map<size_t, vector<string>> allPermutations{
    {1, {"#", "."}},
    {2, {"##", "#.", ".#", ".."}},
    {3, {"###", "##.", "#.#", "#..", ".##", ".#.", "..#", "..."}}};
const vector<string> genPermutation(size_t len) {
  if (allPermutations.count(len) == 0) {
    // Process this
    allPermutations.insert({len, {}});
    auto subs = genPermutation(len - 1);
    for (const auto &sub : subs) {
      allPermutations.at(len).push_back(sub + kOperational);
      allPermutations.at(len).push_back(sub + kDamaged);
    }
  }
  return allPermutations[len];
}

/**
 * Explore the record line and return the possible permutation
 */
size_t processRecord(const string &damagedRecord,
                     const vector<size_t> &recordAsNums) {
  // Find all the damaged location
  string candidate = damagedRecord;
  vector<size_t> unknownLocations;
  for (size_t i = 0; i < damagedRecord.size(); ++i) {
    if (damagedRecord.at(i) == kUnknown) {
      unknownLocations.push_back(i);
    }
  }

  // Do all the permutation
  size_t result = 0;
  auto permutations = genPermutation(unknownLocations.size());
  for (const auto &permu : permutations) {
    // Set the values into damagedRecord and then validate it
    for (size_t i = 0; i < unknownLocations.size(); ++i) {
      auto unknownLocation = unknownLocations.at(i);
      candidate.at(unknownLocation) = permu[i];
    }

    if (validRecord(candidate, recordAsNums)) {
      ++result;
    }
  }

  return result;
}

} // namespace

int main(int argc, char **argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  size_t answer = 0, answerPart1 = 0, answerPart2 = 0;
  string line;

  // Stopping conditions

  while (getline(cin, line)) {
    string damagedRecordString, recordNumsString;
    vector<size_t> recordNums;
    if (re2::RE2::FullMatch(line, kLineRe, &damagedRecordString,
                            &recordNumsString)) {
      // Something to process
      // Parse the numbers
      {
        istringstream temp("," + recordNumsString);
        char tChar;
        size_t tNum;
        while (temp >> tChar >> tNum) {
          recordNums.push_back(tNum);
        }
      }
      if (!FLAGS_part2) {
        // Part 1
        auto cur = processRecord(damagedRecordString, recordNums);
        cout << damagedRecordString << " " << recordNumsString << " : " << cur
             << endl;
        answer += cur;
      } else {
        // Part 2
        auto unfoldedString = damagedRecordString;
        auto unfoldedNums = recordNums;
        for (size_t i = 1; i < kFoldFactor; ++i) {
          unfoldedString += kUnknown + damagedRecordString;
          for (const auto num: recordNums) {
            unfoldedNums.push_back(num);
          }
        }
        auto cur = processRecord(unfoldedString, unfoldedNums);
        cout << damagedRecordString << " " << recordNumsString << " : part 2 : " << cur
             << endl;
        answer += cur;
      }
    } else {
      throw runtime_error("Invalid line");
    }
  }

  if (FLAGS_part2) {
    // Tally the result for part 2
  }
  cout << (FLAGS_part2 ? "Part 2: " : "Part 1: ") << answer << endl;

  if (!FLAGS_part2) {
    cout << "Solution above is for part 1, if you need part 2 solution, add "
            "the flag --part2 to the command invocation"
         << endl;
  }
}
