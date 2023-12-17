#include "gflags/gflags.h"
#include "re2/re2.h"
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

using namespace std;
namespace {
const re2::RE2 kRemoveStepRe("^([a-z]+)-$");
const re2::RE2 kAddStepRe("^([a-z]+)=([0-9]+)$");
typedef uint8_t HashType;
const HashType kHashMultiplier = 17;
const size_t kBoxCount = 1 << 8;
const char kDelim = ',';
const string kNullLabel = "NULL";

/**
 * Calculate the hash of a given string specified via the begin and end
 */
HashType quizHash(const string::iterator begin, const string::iterator end) {
  HashType result = 0;
  for (auto it = begin; it < end; ++it) {
    HashType val = *it;
    result += val;
    result *= kHashMultiplier;
  }
  return result;
}

/**
 * Perform the operation as instructed
 */
void performOperation(const string::iterator begin, const string::iterator end,
                      vector<unordered_map<string, size_t>> &boxes,
                      vector<vector<string>> &labelOrder) {
  string input(begin, end);
  string label;
  size_t lensValue;
  if (re2::RE2::FullMatch(input, kRemoveStepRe, &label)) {
    // Remove lens
    auto const boxIndex = quizHash(label.begin(), label.end());
    auto &targetBox = boxes.at(boxIndex);
    // Finally remove
    targetBox.erase(label);
    for (auto &lo : labelOrder.at(boxIndex)) {
      if (lo == label) {
        // Remove this label in the order
        lo = kNullLabel;
      }
    }
  } else if (re2::RE2::FullMatch(input, kAddStepRe, &label, &lensValue)) {
    auto const boxIndex = quizHash(label.begin(), label.end());
    auto &targetBox = boxes.at(boxIndex);
    if (targetBox.count(label) == 0) {
      // We did not have this label earlier - this is insert
      // otherwise is replace
      labelOrder.at(boxIndex).push_back(label);
    }
    // Add/update the label
    targetBox[label] = lensValue;
  } else {
    throw runtime_error("Invalid step " + input);
  }
}

} // namespace

int main(int argc, char **argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  size_t answerPart1 = 0, answerPart2 = 0;
  string line;
  // Input is only 1 line
  getline(cin, line);
  vector<unordered_map<string, size_t>> boxes{kBoxCount};
  vector<vector<string>> labelOrder{kBoxCount};

  auto start = line.begin();
  while (true) {
    auto stop = std::find(start, line.end(), kDelim);
    // Part 1 sum the hash
    answerPart1 += quizHash(start, stop);

    // Part 2 - update the boxes
    performOperation(start, stop, boxes, labelOrder);

    if (stop == line.end()) {
      // Ended
      break;
    }
    // Skip the delim
    start = stop + 1;
  }

  // Part 2 - add up the focusing power
  for (size_t boxIndex = 0; boxIndex < kBoxCount; ++boxIndex) {
    size_t lensIndex = 1;
    const auto &box = boxes.at(boxIndex);
    for (const auto &label : labelOrder.at(boxIndex)) {
      if (label != kNullLabel) {
        auto lensValue = box.at(label);
        answerPart2 += (boxIndex + 1) * lensIndex * lensValue;
        ++lensIndex;
      }
    }
  }

  cout << "Part 1: " << answerPart1 << endl;
  cout << "Part 2: " << answerPart2 << endl;
}
