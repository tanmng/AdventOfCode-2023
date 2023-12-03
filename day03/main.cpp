#include "re2/re2.h"
#include <iostream>
#include <unordered_map>
#include <utility>
#include <vector>

using namespace std;
namespace {
const char kNonSymbol = '.';
const char kGearSymbol = '*';
const re2::RE2 kNumberRe("([0-9]+)");

/**
 * Return if the character is a symbol
 */
bool isSymbol(const char character) {
  return character != kNonSymbol && (character < '0' || character > '9');
}
/**
 * Return a pair of boolean and a vector of index
 * the first one is true if the cells adjacent of a range has any symbol (ie.
 * not . and not a digit) the vector of index is vector of index of the gear
 * symbols adjacent to the number
 */
pair<bool, vector<size_t>> adjacentSymbol(const string &map, const size_t begin,
                                          const size_t len,
                                          const size_t rowSize) {
  pair<bool, vector<size_t>> result{false, {}};

  // Start by checking the left adjacent
  int targetIndex = begin - 1;
  // cout << targetIndex << endl;
  if (targetIndex >= 0 && targetIndex < map.size() &&
      isSymbol(map[targetIndex])) {
    result.first = true;
    // Record all the gears
    if (map[targetIndex] == kGearSymbol) {
      result.second.push_back(targetIndex);
    }
  }
  // CHeck the right adjacent
  targetIndex = begin + len;
  // cout << targetIndex << endl;
  if (targetIndex >= 0 && targetIndex < map.size() &&
      isSymbol(map[targetIndex])) {
    result.first = true;
    // Record all the gears
    if (map[targetIndex] == kGearSymbol) {
      result.second.push_back(targetIndex);
    }
  }

  // Check the row above
  for (targetIndex = ((int)begin - 1 - rowSize);
       targetIndex < ((int)begin + len + 1 - rowSize); ++targetIndex) {
    // cout << targetIndex << endl;
    if (targetIndex >= 0 && targetIndex < map.size() &&
        (targetIndex / rowSize) != (begin / rowSize) &&
        isSymbol(map[targetIndex])) {
      result.first = true;
      // Record all the gears
      if (map[targetIndex] == kGearSymbol) {
        result.second.push_back(targetIndex);
      }
    }
  }

  // Check the row below
  for (targetIndex = (begin - 1 + rowSize);
       targetIndex < (begin + len + 1 + rowSize); ++targetIndex) {
    // cout << targetIndex << map[targetIndex] << endl;
    if (targetIndex >= 0 && targetIndex < map.size() &&
        (targetIndex / rowSize) != (begin / rowSize) &&
        isSymbol(map[targetIndex])) {
      result.first = true;
      // Record all the gears
      if (map[targetIndex] == kGearSymbol) {
        result.second.push_back(targetIndex);
      }
    }
  }

  return result;
}
} // namespace

int main() {
  string allContent, line;
  size_t rowSize;
  size_t answerPart1 = 0, answerPart2 = 0;
  unordered_map<size_t, vector<unsigned long>>
      gearToAdjacentNumbers; // Mapping from a gear symbol to all its adjacent
                             // numbers
  while (cin >> line) {
    allContent += line;
    rowSize = line.length();
  }
  re2::StringPiece currentNumber, input(allContent);
  auto firstBegin = input.data();
  while (re2::RE2::FindAndConsume(&input, kNumberRe, &currentNumber)) {
    size_t numLen = input.data() - currentNumber.data();
    size_t numberValue = std::stoul(currentNumber.ToString());
    auto adjacentResult = adjacentSymbol(
        allContent, currentNumber.data() - firstBegin, numLen, rowSize);
    if (adjacentResult.first) {
      answerPart1 += numberValue;

      // Only in this case can we potentially have the list of gears
      for (const auto gearIndex : adjacentResult.second) {
        gearToAdjacentNumbers[gearIndex].push_back(numberValue);
      }
    }
  }

  cout << "Part 1: " << answerPart1 << endl;

  // Go through all the gears and sum up the one with 2 adjacent numbers
  for (const auto &kv : gearToAdjacentNumbers) {
    if (kv.second.size() == 2) {
      // Gear with 2 adjacent number
      answerPart2 += kv.second[0] * kv.second[1];
    }
  }
  cout << "Part 2: " << answerPart2 << endl;
}
