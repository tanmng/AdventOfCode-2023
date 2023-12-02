#include "absl/strings/str_split.h"
#include "re2/re2.h"
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

namespace {
const re2::RE2 kCubeConfig("^([0-9]+) (.+)$");
const string kRed = "red", kGreen = "green", kBlue = "blue";
const size_t kRedMax = 12, kGreenMax = 13, kBlueMax = 14;
/**
 * Return if a game is valid (for part 1) or not and return the set power (for
 * part 2)
 */
pair<bool, size_t> processGame(const std::string gameRounds) {
  pair<bool, size_t> result{true, 1};
  unordered_map<string, size_t> maxCube{
      {kRed, 0},
      {kGreen, 0},
      {kBlue, 0},
  };
  vector<string> rounds = absl::StrSplit(gameRounds, "; ");
  for (const auto &round : rounds) {
    vector<string> cubeConfig = absl::StrSplit(round, ", ");
    unordered_map<string, size_t> frequencyMap{
        {kRed, 0},
        {kGreen, 0},
        {kBlue, 0},
    };
    for (const auto &config : cubeConfig) {
      // Get the name and the number
      string colorName;
      size_t count;
      if (!re2::RE2::FullMatch(config, kCubeConfig, &count, &colorName)) {
        throw runtime_error("Invalid round config " + config);
      }

      frequencyMap[colorName] += count;
    }

    if (frequencyMap[kRed] > kRedMax || frequencyMap[kGreen] > kGreenMax ||
        frequencyMap[kBlue] > kBlueMax) {
      // More than we have
      result.first = false;
    }
    for (const auto &item : maxCube) {
      maxCube[item.first] = max(maxCube[item.first], frequencyMap[item.first]);
    }
  }
  for (const auto &item : maxCube) {
    result.second *= maxCube[item.first];
  }
  return result;
}
} // namespace
int main() {
  string line;
  size_t answerPart1 = 0, answerPart2 = 0;
  size_t gameIndex = 0;
  while (getline(cin, line)) {
    // Load a game
    ++gameIndex;

    // Get the configuration
    vector<string> outterParts = absl::StrSplit(line, ": ");

    if (outterParts.size() != 2) {
      throw runtime_error("Invalid input line " + line);
    }

    auto processResult = processGame(outterParts[1]);

    if (processResult.first) {
      answerPart1 += gameIndex;
    }

    answerPart2 += processResult.second;
  }

  cout << "Part 1: " << answerPart1 << endl;
  cout << "Part 2: " << answerPart2 << endl;
  return 0;
}